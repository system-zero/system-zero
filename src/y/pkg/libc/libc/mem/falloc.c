// provides: void *mem_alloc (uint)
// provides: void *mem_calloc (uint)
// provides: void *mem_realloc (void *, uint)
// provides: int mem_release (void **)
// provides: int mem_init (uint)
// provides: int mem_deinit (void)
// provides: uint mem_get_actual_size (void *)
// provides: void mem_mark_unused (void *)
// provides: int mem_debug_all (int)
// requires: sys/brk.c
// requires: stdlib/_exit.c
// requires: mem/falloc.h

/* This was intented to be a Fast allocator because it uses a specialized
   field to set the [un]used flag and two pointers to locate the next and
   previous chunks (which results to 24 bytes memory overhead), but wasn't
   the case. It performs at its best equal to talloc which uses arithmetic
   to find those chunks and bit manipulation to store the flags.

   As it was tricky to get this thing right, it stays as a reference in the
   repository for future perhaps usage.

   By testing the implementations, it was revealed that raising the extra bytes
   at 4096 instead of 1024 (when we are allocating new chunks from the kernel),
   all they were able to perform two bits better, so calls to sbrk() cost.
*/

static_assert (sizeof (MemChunk) == 24, "MemChunk should be at 24 bytes");

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
  return mem->next;
}

static inline MemChunk *mem_get_prev_chunk (MemChunk *mem) {
 return mem->prev;
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

static inline MemChunk *mem_set_end_chunk (void *endbreakpoint, MemChunk *prev) {
  MemChunk *end = mem_get_end_chunk (endbreakpoint);
  end->chunk_size = 0;
  end->next = NULL;
  end->prev = prev;
  prev->next = end;
  return end;
}

static inline int mem_isnot_the_end_chunk (MemChunk *mem) {
  return mem->next != NULL;
}

static inline int mem_is_the_end_chunk (MemChunk *mem) {
  return mem->next == NULL;
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
  MemChunk *new = (MemChunk *) ((char *) mem + mem->chunk_size);

  new->chunk_size = chunk_size;

  MemChunk *next =mem_get_next_chunk (mem);

  next->prev = new;
  new->next = next;
  new->prev = mem;
  mem->next = new;

  mem_set_unused (new);

  if (FirstFreeChunk > new)
      FirstFreeChunk = new;

  return new;
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

  return mem_new_next_chunk (mem, newsize);
}

static MemChunk *mem_merge_next_chunk (MemChunk *mem, MemChunk *next) {
  next->prev = mem;

  if (mem_is_used (next) || mem_is_the_end_chunk (next))
    return mem;

  #ifdef MEM_DEBUG
  totalMemNextMerges++;
  #endif

  mem->chunk_size += next->chunk_size;

  MemChunk *nextnext = mem_get_next_chunk (next);

  nextnext->prev = mem;
  mem->next = nextnext;

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
  prev->next = mem->next;

  return prev;
}

static MemChunk *mem_merge_chunks (MemChunk *mem) {
  MemChunk *prev = mem_get_prev_chunk (mem);

  #ifdef MEM_DEBUG
  tostdout ("merging: %p with size %u and %u, is unused? %d\n", mem, mem->chunk_size, mem->prev == NULL ? 0 : mem->prev->chunk_size, mem_is_unused (mem));
  #endif

  MemChunk *m = mem_merge_prev_chunk (mem, prev);

  MemChunk *next = mem_get_next_chunk (mem);

  return mem_merge_next_chunk (m, next);
}

static MemChunk *mem_increase (uint size, uint split_at) {
  MemChunk *mem = mem_get_end_chunk (EndBreakPoint);

  if (mem_increment_breakpoint (EndBreakPoint, size) == (void *) -1)
    return NULL;

  #ifdef MEM_DEBUG
  totalMemIncrements++;
  #endif

  mem->chunk_size = size;

  mem_set_used (mem);

  EndBreakPoint = mem_get_current_breakpoint ();

  mem_set_end_chunk (EndBreakPoint, mem);

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

  mem_split_chunk (mem, sz);

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
  MemHead->prev = NULL;
  mem_set_unused (MemHead);

  FirstFreeChunk = MemHead;

  EndBreakPoint = mem_get_current_breakpoint ();

  mem_set_end_chunk (EndBreakPoint, MemHead);

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

  uint totalAscAllocationChunks = 0;
  uint totalUnused = 0;
  uint totalUsed = 0;

  MemChunk *lastchunk = mem_get_last_chunk (EndBreakPoint);

  MemChunk *mem = MemHead;

  uint total = MEM_HEADER_SIZE;

  while (1) {
    total += mem->chunk_size;

    int is_used = mem_is_used (mem);

    if (verbose)
      tostdout ("%p, chunk_size %u prevsz %u used [%s] ",
        mem, mem->chunk_size, mem->prev == NULL ? 0 : mem->prev->chunk_size, is_used ? "yes" : "no");

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
        tostdout (" [%s]", next->prev->chunk_size == mem->chunk_size ? OK_MSG : NOTOK_MSG);

      num_failed += (next->prev->chunk_size != mem->chunk_size);
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
        mem, mem->chunk_size, mem->prev == NULL ? 0 : mem->prev->chunk_size, mem_is_used(mem) ? "yes" : "no");

    if (verbose && mem_is_the_end_chunk (mem))
      tostdout ("this is the end chunk ");

    MemChunk *prev = mem_get_prev_chunk (mem);

    if (prev) {
      if (verbose)
        tostdout (" [%s]", prev->chunk_size == mem->prev->chunk_size ? OK_MSG : NOTOK_MSG);

      num_failed += (prev->chunk_size != mem->prev->chunk_size);
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

    tostdout ("Total Prev Memory Merges: [%d]\n", totalMemPrevMerges);
    tostdout ("Total Next Memory Merges: [%d]\n", totalMemNextMerges);
    tostdout ("Total Memory Merges: [%d]\n", totalMemPrevMerges + totalMemNextMerges);
    tostdout ("Total Memory Splits: [%d]\n", totalMemSplits);
    tostdout ("Total Memory Increments:[%d]\n",totalMemIncrements);
    tostdout ("Total Memory Releases: [%d]\n", totalMemReleases);
    tostdout ("Total Memory Requests: [%d]\n", totalMemRequests);

    if (num_failed)
      tostdout ("\033[%%31mFailed %d tests\033[m\n", num_failed);
  }

  return num_failed;
}
#endif
