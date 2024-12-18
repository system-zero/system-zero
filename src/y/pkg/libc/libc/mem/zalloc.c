// provides: void *mem_alloc (uint)
// provides: void *mem_calloc (uint)
// provides: void *mem_realloc (void *, uint)
// provides: int mem_release (void **)
// provides: int mem_init (uint)
// provides: uint mem_get_actual_size (void *)
// provides: int mem_debug_all (int)
// requires: convert/format.c
// requires: sys/brk.c
// requires: stdlib/_exit.c
// requires: mem/zalloc.h

/* A memory allocator that uses the sys_sbrk () syscall. That simply
   means that no other code should use this syscall, otherwise it is
   undefined behavior.

   The maximum allocated size is at UINT_MAX. The header overhead is
   rather reasonable (16 bytes per allocation (this should be true on
   most architectures)), while still allow us to traverse the tree in
   ascending or descending order, so we can split or merge contiguous
   memory chunks, on allocating and releasing respectively.

   The alignmemt is 16 bytes so the minumum chunk costs 32 bytes.

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
*/

static void *__init_alloc__ (uint);

MemAlloc Malloc = __init_alloc__;

int MemExitOnDoubleFree = 1;
int MemExitOnENOMEM     = 1;

static void *BegBreakPoint = NULL;
static void *EndBreakPoint = NULL;

static MemChunk *MemHead = NULL;

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
  return (size + (ALIGN_SIZE - 1)) & ~(ALIGN_SIZE - 1);
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
  return (MemChunk *) (((char *) ptr) - HEADER_SIZE + 1);
}

static inline void *mem_get_ptr_from_chunk (MemChunk *mem) {
  return mem->pointer;
}

static inline MemChunk *mem_get_next_chunk (MemChunk *mem) {
  /* it probably cannot go out of bounds, but it can return
     itself if it is the end chunk, thouth current code it
     never calls this function with the end chunk argument */
  return (MemChunk *) ((char *) mem + mem->chunk_size);
}

static inline MemChunk *mem_get_prev_chunk (MemChunk *mem) {
  if (mem->prev_chunk_size == 0)
    return NULL;

 return (MemChunk *) ((char *) mem - mem->prev_chunk_size);
}

static inline MemChunk *mem_get_end_chunk (void *endbreakpoint) {
  return (MemChunk *) ((char *) endbreakpoint - HEADER_SIZE);
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

static inline MemChunk *mem_new_next_chunk (MemChunk *mem, uint chunk_size) {
  MemChunk *next = mem_get_next_chunk (mem);
  next->chunk_size = chunk_size;
  next->prev_chunk_size = mem->chunk_size;
  mem_set_unused (next);
  return next;
}

static inline int mem_isnot_the_end_chunk (MemChunk *mem) {
  return mem->chunk_size != 0;
}

static inline int mem_is_the_end_chunk (MemChunk *mem) {
  return mem->chunk_size == 0;
}

static MemChunk *mem_find_unused_chunk (MemChunk *mem, uint size) {
   MemChunk *cur = mem;

  while (mem_isnot_the_end_chunk (cur)) {
    if (mem_is_unused (cur) && cur->chunk_size >= size)
      return cur;

    cur = mem_get_next_chunk (cur);
  }

  return NULL;
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
  if (newsz % ALIGN_SIZE != 0)
     tostderr ("%s: new size %u is not aligned (requested size: %u)\n",
        __func__, newsize, mem_needsize);
#endif

  if (newsize < SPLIT_WHEN_IS_AT_LEAST)
    return mem;

  mem->chunk_size = mem_needsize;

  return mem_make_new_next_chunk (mem, newsize);
}

static MemChunk *mem_merge_next_chunk (MemChunk *mem, MemChunk *next) {
  next->prev_chunk_size = mem->chunk_size;

  if (mem_is_used (next) || mem_is_the_end_chunk (next))
    return mem;

  mem->chunk_size += next->chunk_size;

  MemChunk *nextnext = mem_get_next_chunk (next);

  nextnext->prev_chunk_size = mem->chunk_size;

  return mem;
}

static MemChunk *mem_merge_prev_chunk (MemChunk *mem, MemChunk *prev) {
  if (NULL == prev || mem_is_used (prev))
    return mem;

  prev->chunk_size += mem->chunk_size;
  return prev;
}

static MemChunk *mem_merge_chunks (MemChunk *mem) {
  MemChunk *prev = mem_get_prev_chunk (mem);
#ifdef MEM_DEBUG
  tostdout ("merging: %p with size %u and %u, is available? %d\n", mem, mem->chunk_size, mem->prev_chunk_size, mem_is_unused (mem));
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

  mem->chunk_size = size;
  mem->prev_chunk_size = lastchunk->chunk_size;

  mem_set_used (mem);

  EndBreakPoint = mem_get_current_breakpoint ();

  mem_set_end_chunk (EndBreakPoint, mem->chunk_size);

  mem_split_chunk (mem, split_at);

  return mem;
}

void *mem_alloc (uint size) {
  uint sz = mem_align (size) + HEADER_SIZE;

#ifdef MEM_DEBUG
  tostdout ("%s: request for %u bytes will try to allocate %u\n",
    __func__, size, sz);
#endif

  MemChunk *mem = mem_find_unused_chunk (MemHead, sz);

  if (mem == NULL) {
    MemChunk *new = mem_increase (sz + INCREASE_EXTRA_BYTES, INCREASE_EXTRA_BYTES);

    if (NULL == new) return NULL;

    return mem_get_ptr_from_chunk (new);
#ifdef MEM_DEBUG
    tostdout ("we had to increase with new ptr %p\n", new);
#endif

    mem_split_chunk (new, sz);

#ifdef MEM_DEBUG
    tostdout ("finally %p size %u | %u good luck\n",
        new, new->chunk_size, new->prev_chunk_size);
#endif

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
    uint sz = mem_align (size) + HEADER_SIZE;
    mem_split_chunk (mem, sz);
    return ptr;
  }

  char *mem_ptr = ptr;

  MemChunk *next = mem_get_next_chunk (mem);

  if (next != NULL && mem_is_unused (next)) {
    uint sz = mem_align (size) + HEADER_SIZE;
    if (sz <= next->chunk_size) {
      mem_merge_next_chunk (mem, next);
      return ptr;
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

  MemChunk *mem = mem_get_chunk_from_ptr (*ptr);

  mem_set_unused (mem);

  mem_merge_chunks (mem);

  *ptr = NULL;

  return 0;
}

static void *__init_alloc__ (uint size) {
  if (-1 == mem_init (size))
    return NULL;

  Malloc = mem_alloc;
  return Malloc (size);
}

int mem_init (uint size) {
  if (BegBreakPoint != NULL)
    return 0;

  BegBreakPoint = mem_get_current_breakpoint ();

  uint sz = mem_align (size) + (HEADER_SIZE * 2); // beg header plus the end header

  if (mem_increment_breakpoint (BegBreakPoint, sz) == (void *) -1)
    return -1;

  MemHead = (MemChunk *) BegBreakPoint;
  MemHead->chunk_size = sz - HEADER_SIZE;
  MemHead->prev_chunk_size = 0;
  mem_set_unused (MemHead);

  EndBreakPoint = mem_get_current_breakpoint ();

  mem_set_end_chunk (EndBreakPoint, MemHead->chunk_size);

#ifdef MEM_DEBUG
  tostdout ("%p BegBreakPoint\n%p EndBreakPoint\n", BegBreakPoint, EndBreakPoint);
#endif

  return 0;
}

uint mem_get_actual_size (void *ptr) {
  MemChunk *mem = mem_get_chunk_from_ptr (ptr);
  return mem->chunk_size - HEADER_SIZE;
}

#define OK_MSG    "\033[%32mok\033[m"
#define NOTOK_MSG "\033[%31mnotok\033[m"

int mem_debug_all (int verbose) {
  int num_failed = 0;

  MemChunk *mem = MemHead;

  MemChunk *lastchunk = mem_get_last_chunk (EndBreakPoint);

  uint total = HEADER_SIZE;

  while (1) {
    total += mem->chunk_size;

    if (verbose)
      tostdout ("%p, chunk_size %u prevsz %u used [%s] ",
        mem, mem->chunk_size, mem->prev_chunk_size, mem_is_used(mem) ? "yes" : "no");

    if (mem_is_the_end_chunk (mem)) {
      tostdout ("this is the end chunk\n");
      break;
    }

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

  if (verbose)
    tostdout ("Total memory requested from kernel %u bytes [%s]\n",
      diff, diff == total ? OK_MSG : NOTOK_MSG);

  num_failed +=  (diff != total);

  total = HEADER_SIZE;

  mem = mem_get_end_chunk (EndBreakPoint);

  while (1) {
    total += mem->chunk_size;

    if (verbose)
      tostdout ("%p, chunk_size %u prevsz %u used [%s] ",
        mem, mem->chunk_size, mem->prev_chunk_size, mem_is_used(mem) ? "yes" : "no");

    if (verbose && mem_is_the_end_chunk (EndBreakPoint))
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

    mem = prev;
    if (mem == NULL)
      break;
  }

  if (verbose)
    tostdout ("Total memory requested from kernel %u bytes [%s]\n",
      diff, diff == total ? OK_MSG : NOTOK_MSG);

  num_failed +=  (diff != total);

  if (num_failed && verbose)
    tostdout ("\033[%%31mFailed %d tests\033[m\n", num_failed);

  return num_failed;
}
