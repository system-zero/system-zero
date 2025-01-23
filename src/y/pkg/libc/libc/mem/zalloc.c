// provides: void *mem_alloc (uint)
// provides: void *mem_calloc (uint)
// provides: void *mem_realloc (void *, uint)
// provides: int mem_release (void **)
// provides: int mem_init (uint)
// provides: int mem_deinit (void)
// provides: uint mem_get_actual_size (void *)
// provides: void mem_mark_unused (void *)
// provides: int mem_debug_all (int)
// requires: convert/decimal.h
// requires: convert/format.c
// requires: sys/brk.c
// requires: stdlib/_exit.c
// requires: mem/zalloc.h

/* A memory allocator that uses the sys_sbrk () syscall. That simply
   means, that no other code should use this syscall other than this
   unit, otherwise it is unpredicted behavior (and there is nothing
   than can be done for this, as there is no such mechanism in C and
   kernel (dissallowing access to functionality except maybe from the
   first served in a process that gets some credentials for later use)).

   The maximum allocated size is at UINT_MAX - 15. The header overhead
   is  rather reasonable (16 bytes per allocation (this should be true
   on  most architectures)), while still allow us to traverse the tree
   in ascending or descending order, so we can split or merge contiguous
   memory chunks, on allocating and releasing respectively. This leave
   us with 7 bytes free for use.

   The alignmemt is at 16 bytes so the minumum chunk costs 32 bytes.

   The mem_release() function always nullifies the pointer so it expects
   a (void **) double pointer argument. The 'Release' macro wraps this
   detail.

   Since mem_realloc() function could modify the pointer argument if it
   isn't NULL, the way to use it is:
      ptr = mem_realloc (ptr, size);

   This same function it is like sys_malloc(), when the pointer argument
   is NULL. If its not then, if the new size is smaller than the current
   pointer's size then it simply tries to split the unused size. When it
   finds that the next chunk is unused and fits the size, it merges this
   next chunk.

   The mem_calloc() function signature is not that of the standard calloc,
   as it is like mem_alloc(), except that initializes the memory to zero.
*/

static_assert (sizeof (MemChunk) == 16, "MemChunk should be at 16 bytes");

static void *__mem_alloc__ (uint);
static void *__init_alloc__ (uint);

MemAlloc Malloc = __init_alloc__;

#ifdef MEM_DEBUG
int totalMemRequests    = 0;
int totalMemReleases    = 0;
int totalMemPrevMerges  = 0;
int totalMemNextMerges  = 0;
int totalMemSplits      = 0;
int totalMemIncrements  = 0;
#endif

int MemExitOnDoubleFree = 1;
int MemExitOnENOMEM     = 1;
uint MemSplitWhenIsAtleast  = (1 << 8);
uint MemExtraSpaceOnRealloc = (1 << 8);
uint MemIncreaseExtrabytes  = (1 << 12);

static void *BegBreakPoint = NULL;
static void *EndBreakPoint = NULL;

static MemChunk *MemHead = NULL;
static MemChunk *FirstFreeChunk = NULL;

static inline uint mem_diff (void *end, void *beg) {
  return (char *) end - (char *) beg;
}

static inline void *mem_zero (void *ptr, uint size) {
  char *sp = (char *) ptr;
  for (uint i = 0; i < size; i++)
    sp[i] = 0;

  return ptr;
}

static inline uint mem_align (uint size) {
  return (size + (MEM_ALIGN_SIZE - 1)) & ~(MEM_ALIGN_SIZE - 1);
}

static inline void *mem_increment_breakpoint (void *endbreakpoint, uint inc) {
  long r = syscall1 (NR_brk, (long) endbreakpoint + inc);

  if (-1 == r) {
    sys_errno = ENOMEM;

    if (MemExitOnENOMEM)
      exit (1);

    return (void *) -1;
  }

  return (void *) r;
}

static inline void *mem_get_current_breakpoint (void) {
  return sys_sbrk (0);
}

static inline MemChunk *mem_get_chunk_from_ptr (void *ptr) {
  return (MemChunk *) ((char *) ptr - MEM_HEADER_SIZE);
}

static inline void *mem_get_ptr_from_chunk (MemChunk *mem) {
  return (char *) mem + MEM_HEADER_SIZE;
}

static inline MemChunk *mem_get_next_chunk (MemChunk *mem) {
  /* it probably cannot go out of bounds, but it can return
     itself if it is the end chunk, though current code it
     never calls this function with the end chunk argument */
  return (MemChunk *) ((char *) mem + mem->chunk_size);
}

static inline MemChunk *mem_get_prev_chunk (MemChunk *mem) {
  if (mem->prev_chunk_size == 0)
    return NULL;

 return (MemChunk *) ((char *) mem - mem->prev_chunk_size);
}

static inline MemChunk *mem_get_end_chunk (void *endbreakpoint) {
  return (MemChunk *) ((char *) endbreakpoint - MEM_HEADER_SIZE);
}

static inline MemChunk *mem_get_last_chunk (void *endbreakpoint) {
  return mem_get_prev_chunk (mem_get_end_chunk (endbreakpoint));
}

static inline void mem_set_used (MemChunk *m) {
  m->is_used = 1;
}

static inline void mem_set_unused (MemChunk *m) {
  m->is_used = 0;
}

static inline int mem_is_used (MemChunk *m) {
  return m->is_used;
}

/* same thing, just a different thought flow that works better for some minds,
   that is true for them when something is zero, so we can accept this waste i
   suppose */
static inline int mem_is_unused (MemChunk *m) {
  return m->is_used == 0;
}

static inline MemChunk *mem_set_end_chunk (void *endbreakpoint, uint prevsize) {
  MemChunk *end = mem_get_end_chunk (endbreakpoint);
  end->chunk_size = 0;
  end->prev_chunk_size = prevsize;
  return end;
}

static inline int mem_isnot_the_end_chunk (MemChunk *mem) {
  return mem->chunk_size != 0;
}

static inline int mem_is_the_end_chunk (MemChunk *mem) {
  return mem->chunk_size == 0;
}

static MemChunk *mem_find_unused_chunk (MemChunk *mem, uint size) {
  MemChunk *it = mem;

  FirstFreeChunk = NULL;

  while (mem_isnot_the_end_chunk (it)) {
    if (mem_is_unused (it)) {
      if (FirstFreeChunk == NULL)
          FirstFreeChunk = it;

      if (it->chunk_size >= size)
        return it;
    }

    it = mem_get_next_chunk (it);
  }

  FirstFreeChunk = mem;

  return NULL;
}

static inline MemChunk *mem_new_next_chunk (MemChunk *mem, uint chunk_size) {
  MemChunk *next = mem_get_next_chunk (mem);
  next->chunk_size = chunk_size;
  next->prev_chunk_size = mem->chunk_size;
  mem_set_unused (next);
  return next;
}

/* this function doesn't check its arguments, so it is assumed
   that sizes are in bounds and aligned
 */
static MemChunk *mem_make_new_next_chunk (MemChunk *mem, uint newsize) {
  MemChunk *next = mem_new_next_chunk (mem, newsize);
  MemChunk *nextnext = mem_get_next_chunk (next); // always succeeds
  nextnext->prev_chunk_size = next->chunk_size;
  return next;
}

/* it returns the new next chunk if the split has been performed
   or mem otherwise */
static MemChunk *mem_split_chunk (MemChunk *mem, uint mem_needsize) {
  uint newsize = mem->chunk_size - mem_needsize;

  #ifdef MEM_DEBUG
  if (newsize % MEM_ALIGN_SIZE != 0)
     tostderr ("%s: new size %u is not aligned (requested size: %u)\n",
        __func__, newsize, mem_needsize);
  #endif

  if (newsize < MemSplitWhenIsAtleast)
    return mem;

  #ifdef MEM_DEBUG
  totalMemSplits++;
  #endif

  mem->chunk_size = mem_needsize;

  if (FirstFreeChunk > mem)
      FirstFreeChunk = mem;

  return mem_make_new_next_chunk (mem, newsize);
}

static MemChunk *mem_merge_next_chunk (MemChunk *mem, MemChunk *next) {
  next->prev_chunk_size = mem->chunk_size;

  if (mem_is_used (next) || mem_is_the_end_chunk (next))
    return mem;

  #ifdef MEM_DEBUG
  totalMemNextMerges++;
  #endif

  mem->chunk_size += next->chunk_size;

  MemChunk *nextnext = mem_get_next_chunk (next);

  nextnext->prev_chunk_size = mem->chunk_size;

  return mem;
}

static MemChunk *mem_merge_prev_chunk (MemChunk *mem, MemChunk *prev) {
  if (NULL == prev || mem_is_used (prev))
    return mem;

  #ifdef MEM_DEBUG
  totalMemPrevMerges++;
  #endif

  /* if we ever call this function more than once then we may want to
     check the possibility that FirstFreeChunk might be equal to mem,
     as then it will simply point to a disaster point in memory, when
     it will be called again from mem_alloc (), for now it is handled
     by mem_release () */
  #if 0
  if (FirstFreeChunk > prev)
      FirstFreeChunk = prev;
  #endif

  prev->chunk_size += mem->chunk_size;

  return prev;
}

static MemChunk *mem_merge_chunks (MemChunk *mem) {
  MemChunk *prev = mem_get_prev_chunk (mem);

  #ifdef MEM_DEBUG
  tostdout ("merging: %p with size %u and %u, is unused? %d\n", mem, mem->chunk_size, mem->prev_chunk_size, mem_is_unused (mem));
  #endif

  MemChunk *m = mem_merge_prev_chunk (mem, prev);

  MemChunk *next = mem_get_next_chunk (mem);

  return mem_merge_next_chunk (m, next);
}

static MemChunk *mem_increase (uint size, uint split_at) {
  MemChunk *mem = mem_get_end_chunk (EndBreakPoint);
  MemChunk *lastchunk = mem_get_prev_chunk (mem);

  if (mem_increment_breakpoint (EndBreakPoint, size) == (void *) -1)
    return NULL;

  #ifdef MEM_DEBUG
  totalMemIncrements++;
  #endif

  mem->chunk_size = size;
  mem->prev_chunk_size = lastchunk->chunk_size;

  mem_set_used (mem);

  EndBreakPoint = mem_get_current_breakpoint ();

  mem_set_end_chunk (EndBreakPoint, mem->chunk_size);

  mem_split_chunk (mem, split_at);

  return mem;
}

void *__mem_alloc__ (uint size) {
  uint sz = mem_align (size + MEM_HEADER_SIZE);

  #ifdef MEM_DEBUG
  totalMemRequests++;
  tostdout ("%s: request for %u bytes will try to allocate %u\n",
    __func__, size, sz);
  #endif

  MemChunk *mem = mem_find_unused_chunk (FirstFreeChunk, sz);

  if (mem == NULL) {

    MemChunk *new = mem_increase (sz + MemIncreaseExtrabytes, sz);

    if (NULL == new) return NULL;

    return mem_get_ptr_from_chunk (new);
  }

  mem_set_used (mem);

  /* if previously allocated mem size is bigger than the requested */
  mem_split_chunk (mem, sz);
  /* this should also chain with the next chunk (if available, otherwise
     it will leave it with the old value)  */

  #ifdef MEM_DEBUG
  tostdout ("we already have space for %p with size %u | %u\n",
      mem, mem->chunk_size, mem->prev_chunk_size);
  #endif

  return mem_get_ptr_from_chunk (mem);
}

void *mem_alloc (uint size) {
  return Malloc (size);
}

void *mem_calloc (uint size) {
  void *ptr = Malloc (size);

  if (NULL == ptr) return NULL;

  return mem_zero (ptr, mem_get_actual_size (ptr));
}

void *mem_realloc (void *ptr, uint size) {
  if (ptr == NULL)
    return Malloc (size);

  MemChunk *mem = mem_get_chunk_from_ptr (ptr);

  uint ptr_sz = mem_get_actual_size (ptr);

  if (ptr_sz > size) {
    uint sz = mem_align (size + MEM_HEADER_SIZE);
    mem_split_chunk (mem, sz);
    uint actual_sz = mem_get_actual_size (ptr);
    mem_zero ((char *) ptr + size, actual_sz - size);
    return ptr;
  }

  char *mem_ptr = ptr;

  MemChunk *next = mem_get_next_chunk (mem);

  if (next != NULL && mem_is_unused (next)) {
    uint sz = mem_align (size + MemExtraSpaceOnRealloc + MEM_HEADER_SIZE);
    uint needfromnext = sz - mem->chunk_size;

    if (needfromnext <= next->chunk_size) {
      if (next->chunk_size > (needfromnext + MemSplitWhenIsAtleast)) {
        MemChunk *new = mem_split_chunk (next, needfromnext);

        if (new == next)
          goto outofcond; // probably we are safe but be sure
      }

      mem_merge_next_chunk (mem, next);
      return ptr;

      outofcond:
    }
  }

  void *p = Malloc (size);
  if (NULL == p) return NULL;

  MemChunk *newchunk = mem_get_chunk_from_ptr (p);

  char *new_ptr = (char *) p;
  uint new_sz = mem_get_actual_size (p);

  for (uint i = 0; i < ptr_sz && i < new_sz; i++)
    new_ptr[i] = mem_ptr[i];

  mem_set_used (newchunk);

  mem_release (&ptr);

  return new_ptr;
}

int mem_release (void **ptr) {
  if (*ptr == NULL) {
    if (MemExitOnDoubleFree)
      exit (1);
    return -1;
  }

  #ifdef MEM_DEBUG
  totalMemReleases++;
  #endif

  MemChunk *mem = mem_get_chunk_from_ptr (*ptr);

  mem_set_unused (mem);

  mem = mem_merge_chunks (mem);

  if (FirstFreeChunk > mem)
      FirstFreeChunk = mem;

  *ptr = NULL;

  return 0;
}

static void *__init_alloc__ (uint size) {
  if (-1 == mem_init (size))
    return NULL;

  Malloc = __mem_alloc__;
  return Malloc (size);
}

int mem_init (uint size) {
  if (BegBreakPoint != NULL)
    return 0;

  BegBreakPoint = mem_get_current_breakpoint ();

                            /* beg header plus the end header */
  uint sz = mem_align (size + (MEM_HEADER_SIZE * 2));

  if (mem_increment_breakpoint (BegBreakPoint, sz) == (void *) -1)
    return -1;

  MemHead = (MemChunk *) BegBreakPoint;
  MemHead->chunk_size = sz - MEM_HEADER_SIZE;
  MemHead->prev_chunk_size = 0;
  mem_set_unused (MemHead);

  FirstFreeChunk = MemHead;

  EndBreakPoint = mem_get_current_breakpoint ();

  mem_set_end_chunk (EndBreakPoint, MemHead->chunk_size);

  #ifdef MEM_DEBUG
  tostdout ("%p BegBreakPoint\n%p EndBreakPoint\n", BegBreakPoint, EndBreakPoint);
  #endif

  return 0;
}

int mem_deinit (void) {
  if (BegBreakPoint == NULL)
    return 0;

  ptrdiff_t sz = (ptrdiff_t ) mem_diff (EndBreakPoint, BegBreakPoint);

  if (-1 == syscall1 (NR_brk, -sz))
    return -1;

  EndBreakPoint = mem_get_current_breakpoint ();

  BegBreakPoint = NULL;
  Malloc = __init_alloc__;

  return 0;
}

uint mem_get_actual_size (void *ptr) {
  MemChunk *mem = mem_get_chunk_from_ptr (ptr);
  return mem->chunk_size - MEM_HEADER_SIZE;
}

void mem_mark_unused (void *ptr) {
  MemChunk *mem = mem_get_chunk_from_ptr (ptr);

  mem_set_unused (mem);

  if (FirstFreeChunk > mem)
      FirstFreeChunk = mem;
}

#ifdef MEM_DEBUG
#define OK_MSG    "\033[%32mok\033[m"
#define NOTOK_MSG "\033[%31mnotok\033[m"

int mem_debug_all (int verbose) {
  int num_failed = 0;

  MemChunk *mem = MemHead;

  MemChunk *lastchunk = mem_get_last_chunk (EndBreakPoint);

  uint totalAscAllocationChunks = 0;
  uint totalUnused = 0;
  uint totalUsed = 0;

  uint total = MEM_HEADER_SIZE;

  while (1) {
    total += mem->chunk_size;

    int is_used = mem_is_used (mem);

    if (verbose)
      tostdout ("%p, chunk_size %u prevsz %u used [%s] ",
        mem, mem->chunk_size, mem->prev_chunk_size, is_used ? "yes" : "no");

    if (mem_is_the_end_chunk (mem)) {
      tostdout ("this is the end chunk\n");
      break;
    }

    totalUnused += is_used == 0;
    totalUsed   += is_used;

    totalAscAllocationChunks++;

    MemChunk *next = mem_get_next_chunk (mem);

    if (mem_isnot_the_end_chunk (next)) {
      if (verbose)
        tostdout (" [%s]", next->prev_chunk_size == mem->chunk_size ? OK_MSG : NOTOK_MSG);

      num_failed += (next->prev_chunk_size != mem->chunk_size);
    } else {
      if (verbose)
        tostdout (" is the last chunk? [%s]", lastchunk == mem ? OK_MSG : NOTOK_MSG);

      num_failed += (lastchunk != mem);
    }

    if (verbose)
      tostdout ("\n");

    mem = next;
  }

  uint diff = mem_diff (EndBreakPoint, BegBreakPoint);

  if (verbose) {
    tostdout ("Total memory requested from kernel %u bytes [%s]\n",
      diff, diff == total ? OK_MSG : NOTOK_MSG);

    tostdout ("Looping in descending order\n");
  }

  num_failed += (diff != total);

  uint totalDescAllocationChunks = 0;

  total = MEM_HEADER_SIZE;

  mem = mem_get_end_chunk (EndBreakPoint);

  while (1) {
    total += mem->chunk_size;

    if (verbose)
      tostdout ("%p, chunk_size %u prevsz %u used [%s] ",
        mem, mem->chunk_size, mem->prev_chunk_size, mem_is_used(mem) ? "yes" : "no");

    if (verbose && mem_is_the_end_chunk (mem))
      tostdout ("this is the end chunk ");

    MemChunk *prev = mem_get_prev_chunk (mem);

    if (prev) {
      if (verbose)
        tostdout (" [%s]", prev->chunk_size == mem->prev_chunk_size ? OK_MSG : NOTOK_MSG);

      num_failed += (prev->chunk_size != mem->prev_chunk_size);
    } else {
      if (verbose)
        tostdout ("is the first chunk? [%s]", MemHead == mem ? OK_MSG : NOTOK_MSG);

      num_failed += (MemHead != mem);
    }

    if (verbose)
      tostdout ("\n");

    if (mem_isnot_the_end_chunk (mem))
      totalDescAllocationChunks++;

    mem = prev;

    if (mem == NULL)
      break;
  }

  num_failed += totalAscAllocationChunks != totalDescAllocationChunks;
  num_failed += (diff != total);

  if (verbose) {
    tostdout ("Total memory requested from kernel %u bytes [%s]\n",
      diff, diff == total ? OK_MSG : NOTOK_MSG);

    tostdout ("Total Chunks: %u - %u [%s]\n",
      totalAscAllocationChunks,
      totalDescAllocationChunks,
      totalAscAllocationChunks == totalDescAllocationChunks ? OK_MSG : NOTOK_MSG);

    tostdout ("Unused Chunks: %d Used Chunks: %d\n",
      totalUnused, totalUsed);

    #ifdef MEM_DEBUG
    tostdout ("Total Prev Memory Merges: [%d]\n", totalMemPrevMerges);
    tostdout ("Total Next Memory Merges: [%d]\n", totalMemNextMerges);
    tostdout ("Total Memory Merges: [%d]\n", totalMemPrevMerges + totalMemNextMerges);
    tostdout ("Total Memory Splits: [%d]\n", totalMemSplits);
    tostdout ("Total Memory Increments:[%d]\n",totalMemIncrements);
    tostdout ("Total Memory Releases: [%d]\n", totalMemReleases);
    tostdout ("Total Memory Requests: [%d]\n", totalMemRequests);
    #endif

    if (num_failed)
      tostdout ("\033[%%31mFailed %d tests\033[m\n", num_failed);
  }

  return num_failed;
}
#endif

// end
/* very first observations:
  Running some tests with the static language with mem_init (1 << 16):
  (that is a little before the deinitialization)

  Total memory requested from kernel 717648 bytes
  Total Chunks: 2240
  Unused Chunks: 169 Used Chunks: 2071
  Total Prev Memory Merges: [3282]
  Total Next Memory Merges: [9499]
  Total Memory Merges: [12781]
  Total Memory Splits: [14804]
  Total Memory Increments:[216]
  Total Memory Releases: [14213]
  Total Memory Requests: [16284]

  The first unused chunk (of all the 169) was at 1353 position, so
  the find function has 887 chunks to proceed, with 169 of them as
  unused.

  Ideally long lived allocations should occupy the first chunks.

  [In|De]creasing the MemSplitWhenIsAtleast variable yields various
  results that might matters.

  Initializing a big chunk on the start up it looks that it helps.

  When a merging is performed, it grows up the posibility to find
  an unused chunk with the requested size, but it also looks that
  decreases the unused chunks.

  So might an application can tune a bit the behavior.
*/

/* other
  There cases (namely new memory requests to the kernel) where the
  find function could benefit, if it will start searching from the
  end, and in descending order.

  Since a field of a type that occupies four bytes is free for use,
  then it can be used as a refcount.

  Another version that could use a type with 8 bytes per allocation,
  and 24 bytes per header, could be:

  typedef struct __attribute__((packed)) {
    uint64_t chunk_size;
    uint64_t prev_chunk_size;
    uint user_flags;
    short unused;
    char is_used;
    char pointer[1];
  } MemChunk;
*/

/* funny
  By [ab]using the memory interface, the string type functions now
  actually use as their memory size the actual size of the pointer
  and which usually is larger than the requested. This allow in a
  lot of cases to operations to use this extra space, that before
  they didn't knew that it was there and needlessly reallocated.
 */

/* seriously
  Here we just cheat, as with standard malloc you can't get this
  information, so we've an advantage when competing...

  But because the string type behaves in ways like the allocator
  (because of its dynamic usage), a special string type, can use
  the allocator structure. At least even now the mem_size can be
  traversed. Of course such a string can use sizeof(size_t) less
  bytes that the original, but at the same time will be tied with
  this libc. This type requires an allocator, and using sbrk() it
  offers us precious flexible contiguous blocks and so simplicity
  and flexibility in implementation, it has yet its own concerns.
  If we manage though to avoid the concerns, then this allocator
  can be used on quite many tasks other than a string type.

  In other words and depending from the nature of the application
  some code can use the structure to build its own types in a (at
  least) more economical way, so the point is that it can be used
  as a foundation to build flexible, economical and (probably and
  by free of charge) performed applications.

  But first what is required is to keep that code small, so we can
  inspect every bit of the code, that it does what it was intented
  to do. And then we can build upon the stabilized interface, and
  possible (if we want to go sophisticated) other allocators, and
  which wouldn't conflict with each other.

  In any case it is a low level thing that affects almost all the
  bits of code from the very very beginning.
 */

