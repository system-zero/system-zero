// provides: void *mem_alloc (size_t)
// provides: void *mem_calloc (size_t)
// provides: void *mem_realloc (void *, size_t)
// provides: int mem_release (void **)
// provides: int mem_init (size_t)
// provides: int mem_deinit (void)
// provides: uint mem_get_actual_size (void *)
// provides: void mem_mark_unused (void *)
// provides: int mem_debug_all (int)
// requires: sys/brk.c
// requires: stdlib/_exit.c
// requires: mem/talloc.h

/* if running on DEBUG MODE and the user hasn't provided a tostd[out|err]
   function, those two are required */
// requires: convert/decimal.h
// requires: convert/format.c

/* This is a Tiny allocator which is like the zalloc.[ch] implementation
   but with one actual difference.

   Both the header size and the alignment is at 8 bytes, so the maximum
   allocated size is at UINT_MAX minus 7 and the minimum chunk size that
   is allocated is at 16 bytes. 

   However in this implementation (which has no specific field to store
   the un|used flag) we use the unused lowest order bit of the chunk_size
   struct member (because of the alignment this bit is always zero), and
   so the calculations are different. The semantics however should be the
   same and so and the underlying algorithm.

   Running the tests of the language it uses 48K less than the 16 bytes
   version and performs the same (though it hadn't to be that way, as it
   uses extented calculations).
*/

static void *__mem_alloc__ (size_t);
static void *__init_alloc__ (size_t);

MemAlloc Malloc = __init_alloc__;

static_assert (sizeof (MemChunk) == 8, "MemChunk size should be 8 bytes");

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

#define MEMMAXSIZE (((1UL << 31) - MEM_HEADER_SIZE) - MEM_ALIGN_SIZE)

static inline int mem_safe_size (size_t size) {
  return MEMMAXSIZE >= size;
}

static inline int mem_noop_size (size_t size) {
  (void) size;
  return 0;
}

int (*MemIsSizeOk) (size_t) = mem_safe_size;

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

static inline uint __get_chunk_size__ (uint size) {
  return (size & (~(1 << 0)));
}

static inline MemChunk *mem_get_next_chunk (MemChunk *mem) {
  /* it probably cannot go out of bounds, but it can return
     itself if it is the end chunk, though current code it
     never calls this function with the end chunk argument */
  return (MemChunk *) ((char *) mem + __get_chunk_size__ (mem->chunk_size));
}

static inline MemChunk *mem_get_prev_chunk (MemChunk *mem) {
  uint size = __get_chunk_size__ (mem->prev_chunk_size);

  if (size == 0)
    return NULL;

 return (MemChunk *) ((char *) mem - size);
}

static inline MemChunk *mem_get_end_chunk (void *endbreakpoint) {
  return (MemChunk *) ((char *) endbreakpoint - MEM_HEADER_SIZE);
}

static inline MemChunk *mem_get_last_chunk (void *endbreakpoint) {
  return mem_get_prev_chunk (mem_get_end_chunk (endbreakpoint));
}

static inline void mem_set_used (MemChunk *mem) {
  mem->chunk_size |= (1 << 0);
}

static inline void mem_set_unused (MemChunk *mem) {
  mem->chunk_size &= ~(1 << 0);
}

static inline int mem_is_used (MemChunk *mem) {
  return (mem->chunk_size & (1 << 0)) == 1;
}

/* same thing, just a different thought flow that works better for some minds,
   that is true for them when something is zero, so we can accept this waste i
   suppose */
static inline int mem_is_unused (MemChunk *mem) {
  return (mem->chunk_size & (1 << 0)) == 0;
}

static inline MemChunk *mem_set_end_chunk (void *endbreakpoint, uint prevsize) {
  MemChunk *end = mem_get_end_chunk (endbreakpoint);
  end->chunk_size = 0;
  end->prev_chunk_size = __get_chunk_size__ (prevsize);
  return end;
}

static inline MemChunk *mem_new_next_chunk (MemChunk *mem, uint chunk_size) {
  MemChunk *next = mem_get_next_chunk (mem);
  next->chunk_size = chunk_size;
  next->prev_chunk_size = __get_chunk_size__ (mem->chunk_size);
  mem_set_unused (next);
  if (FirstFreeChunk > next)
      FirstFreeChunk = next;

  return next;
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

      if (__get_chunk_size__ (it->chunk_size) >= size)
        return it;
    }

    it = mem_get_next_chunk (it);
  }

  FirstFreeChunk = mem;

  return NULL;
}

/* this function doesn't check its arguments, so it is assumed
   that sizes are in bounds and aligned
 */
static MemChunk *mem_make_new_next_chunk (MemChunk *mem, uint newsize) {
  MemChunk *next = mem_new_next_chunk (mem, newsize);
  MemChunk *nextnext = mem_get_next_chunk (next); // always succeeds
  nextnext->prev_chunk_size = __get_chunk_size__ (next->chunk_size);
  return next;
}

/* it returns the new next chunk if the split has been performed
   or mem otherwise */
static MemChunk *mem_split_chunk (MemChunk *mem, uint mem_needsize) {
  uint newsize = __get_chunk_size__ (mem->chunk_size) - mem_needsize;

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

  /* here we set the size but we do not know if we have to set it as unused
   * or used, as this is callers duty, but in any case whatever is to set
   * it has to be set after this function return */
  mem->chunk_size = mem_needsize;

  return mem_make_new_next_chunk (mem, newsize);
}

static MemChunk *mem_merge_next_chunk (MemChunk *mem, MemChunk *next) {
  next->prev_chunk_size = __get_chunk_size__ (mem->chunk_size);

  if (mem_is_used (next) || mem_is_the_end_chunk (next))
    return mem;

  #ifdef MEM_DEBUG
  totalMemNextMerges++;
  #endif

  mem->chunk_size += __get_chunk_size__ (next->chunk_size);

  MemChunk *nextnext = mem_get_next_chunk (next);

  nextnext->prev_chunk_size = __get_chunk_size__ (mem->chunk_size);

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

  prev->chunk_size += __get_chunk_size__ (mem->chunk_size);

  return prev;
}

static MemChunk *mem_merge_chunks (MemChunk *mem) {
  MemChunk *prev = mem_get_prev_chunk (mem);

  MemChunk *m = mem_merge_prev_chunk (mem, prev);

  MemChunk *next = mem_get_next_chunk (mem);

  return mem_merge_next_chunk (m, next);
}

static MemChunk *mem_increase (uint size, uint split_at) {
  MemChunk *mem = mem_get_end_chunk (EndBreakPoint);
  MemChunk *pmem = mem_get_prev_chunk (mem);

  if (mem_increment_breakpoint (EndBreakPoint, size) == (void *) -1)
    return NULL;

  #ifdef MEM_DEBUG
  totalMemIncrements++;
  #endif

  mem->chunk_size = size;
  mem->prev_chunk_size = __get_chunk_size__ (pmem->chunk_size);

  EndBreakPoint = mem_get_current_breakpoint ();

  mem_set_end_chunk (EndBreakPoint, mem->chunk_size);

  mem_split_chunk (mem, split_at);

  mem_set_used (mem);

  return mem;
}

void *__mem_alloc__ (size_t size) {
  if (0 == MemIsSizeOk (size))
    return NULL;

  uint sz = mem_align (size + MEM_HEADER_SIZE);

  #ifdef MEM_DEBUG
  totalMemRequests++;
  #endif

  MemChunk *mem = mem_find_unused_chunk (FirstFreeChunk, sz);

  if (mem == NULL) {

    MemChunk *new = mem_increase (sz + MemIncreaseExtrabytes, sz);

    if (NULL == new) return NULL;

    return mem_get_ptr_from_chunk (new);
  }

  /* if previously allocated mem size is bigger than the requested */
  mem_split_chunk (mem, sz);
  /* this should also chain with the next chunk (if available, otherwise
     it will leave it with the old value)  */

  mem_set_used (mem);

  return mem_get_ptr_from_chunk (mem);
}

void *mem_alloc (size_t size) {
  return Malloc (size);
}

void *mem_calloc (size_t size) {
  void *ptr = Malloc (size);

  if (NULL == ptr) return NULL;

  return mem_zero (ptr, mem_get_actual_size (ptr));
}

void *mem_realloc (void *ptr, size_t size) {
  if (ptr == NULL)
    return Malloc (size);

  MemChunk *mem = mem_get_chunk_from_ptr (ptr);

  uint ptr_sz = mem_get_actual_size (ptr);

  if (ptr_sz > size) {
    uint sz = mem_align (size + MEM_HEADER_SIZE);
    mem_split_chunk (mem, sz);
    mem_set_used (mem);
    uint actual_sz = mem_get_actual_size (ptr);
    mem_zero ((char *) ptr + size, actual_sz - size);
    return ptr;
  }

  char *mem_ptr = ptr;

  MemChunk *next = mem_get_next_chunk (mem);

  if (next != NULL && mem_is_unused (next)) {
    uint sz = mem_align (size + MemExtraSpaceOnRealloc + MEM_HEADER_SIZE);
    uint actualsize = __get_chunk_size__ (mem->chunk_size);
    uint needfromnext = sz - actualsize;

    uint nextsize = __get_chunk_size__ (next->chunk_size);
    if (needfromnext <= nextsize) {
      if (nextsize > (needfromnext + MemSplitWhenIsAtleast)) {
        MemChunk *new = mem_split_chunk (next, needfromnext);
        if (new == next)  // probably we are safe (the split has been performed)
          goto outofcond; // but be sure
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

static void *__init_alloc__ (size_t size) {
  if (-1 == mem_init (size))
    return NULL;

  Malloc = __mem_alloc__;
  return Malloc (size);
}

int mem_init (size_t size) {
  if (BegBreakPoint != NULL)
    return 0;

  BegBreakPoint = mem_get_current_breakpoint ();

  if (0 == MemIsSizeOk (size))
    return -1;

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
  return __get_chunk_size__ (mem->chunk_size) - MEM_HEADER_SIZE;
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

  uint total = MEM_HEADER_SIZE;
  MemChunk *mem = MemHead;

  while (1) {
    total += __get_chunk_size__ (mem->chunk_size);

    int is_used = mem_is_used (mem);

    if (verbose)
      tostdout ("%p, chunk_size %u prevsz %u used [%s] ", mem,
        __get_chunk_size__ (mem->chunk_size),
        __get_chunk_size__ (mem->prev_chunk_size), is_used ? "yes" : "no");

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
        tostdout (" [%s]",
          __get_chunk_size__ (next->prev_chunk_size) ==
          __get_chunk_size__ (mem->chunk_size) ? OK_MSG : NOTOK_MSG);

      num_failed += (__get_chunk_size__ (next->prev_chunk_size) !=
                     __get_chunk_size__ (mem->chunk_size));
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
    tostdout ("Total memory requested: (diff) %u bytes (calculated) %u [%s]\n",
      diff, total, diff == total ? OK_MSG : NOTOK_MSG);

    tostdout ("Looping in descending order\n");
  }

  num_failed += (diff != total);

  uint totalDescAllocationChunks = 0;

  total = MEM_HEADER_SIZE;

  mem = mem_get_end_chunk (EndBreakPoint);

  while (1) {
    total += __get_chunk_size__ (mem->chunk_size);

    if (verbose)
      tostdout ("%p, chunk_size %u prevsz %u used [%s] ", mem,
        __get_chunk_size__ (mem->chunk_size),
        __get_chunk_size__ (mem->prev_chunk_size), mem_is_used(mem) ? "yes" : "no");

    if (verbose && mem_is_the_end_chunk (mem))
      tostdout ("this is the end chunk ");

    MemChunk *prev = mem_get_prev_chunk (mem);

    if (prev) {
      if (verbose)
        tostdout (" [%s]",
          __get_chunk_size__ (prev->chunk_size) ==
          __get_chunk_size__ (mem->prev_chunk_size) ? OK_MSG : NOTOK_MSG);

      num_failed += (__get_chunk_size__ (prev->chunk_size) !=
                     __get_chunk_size__ (mem->prev_chunk_size));
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
    tostdout ("Total memory requested: (diff) %u bytes (calculated) %u [%s]\n",
      diff, total, diff == total ? OK_MSG : NOTOK_MSG);

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
