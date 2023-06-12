// provides: void *sys_malloc (size_t)
// provides: void *sys_realloc (void *, size_t)
// provides: uint sys_free (void *)
// provides: int  mem_init (size_t)
// provides: int  mem_deinit (void)
// requires: sys/brk.c
// requires: stdlib/_exit.c
// requires: stdlib/alloc.h
// requires: stdio/stdio.c

/* this pulled from:
   https://github.com/miguelperes/custom-malloc
   at :commit a07735c5ef3f467b0b4f1cf0fb217e737e74a3a7

   It should be noted that initially has been tried the code from:
   https://github.com/RAGUL1902/Dynamic-Memory-Allocation-in-C
   with the exact logic. This is condireded as an important factor
   to the realization of the mechanics.

   Equally thank both and anyone involved.
*/

/* Note that the original code has been adapted to the environment.

   Some of the changes:

   Removed the MULTIPLIER macro definition, that on every new allocation,
   it used to reserve the requested (size * MULTIPLIER), so to avoid calling
   sys_sbrk() repeatedly.

   However one could make early an estimated by the nature of the application,
   desired allocation by using this interface. This is for total user control.

   Additionaly implemented and exposed
      realloc(), mem_init(), mem_deinit()

   Also, we now exit on sys_sbrk() failure. This can be disabled with the:
   MEM_DO_NOT_EXIT_ON_FAILURE macro definition before including this libc.

   Also, when merging chunks, we don't longer stop to the first one to the
   lhs and rhs chunks of the current freed object, but we continue to loop
   until we find a NULL chunk pointer or a chunk that is not available.

   For now when the requested size is zero, then eight bytes are allocated.
   Haven't settled down on this and how to handle properly. The user should
   know it with a way or even exit hard. This should be handled by a macro
   or even with an option.
*/

/* Function Interface and Semantics:
     sys_malloc (size)
       returns NULL or calll exit() on failure, a void pointer otherwise and
         fills whith zero the allocated space

     sys_realloc(ptr, size)
       when ptr is NULL it calls malloc (size)
       returns NULL or exits() on failure, a void pointer otherwise and:
          copy the old data to the new space
          fill the extra space if any, with zero, if the MEM_ZERO_ON_REALLOC
            has been defined
          release pointer
          set pointer to the allocated memory address 

     sys_free (ptr)
       this free() returns an unsigned int, which is greater than zero on failure
       and zero otherwise
       it fills also with zero the ptr, when the macro MEM_ZERO_FREED has been defined

     mem_init (size)
       initialize the memory arena with size
       returns zero if the arena has already been initialized, -1 on failure or
       1 on success

       This was meant to be called on the application initialization.
       It doesn't required to be called at any point.

     mem_deinit ()
       deinitialize the memory arena by setting the breakpoint to the initial value
       returns zero if the arena has already been deinitialized or on success, and
       -1 on failure

       It can be called at any time to release the allocated space.

       For now due to not knowing what the kernel does with the released space,
       we are not filling with zero the deallocated bytes. Perhaps this should
       be an option, with the cost of the writing overhead.

       Another question is, if the new break points within the application are
       stored continously in memory by the kernel. What if another application
       is calling sbrk() at the middle? Is there a guarantee? I guess not.

       For now we find the required space for deallocation by substracting the
       current break pointer with the first one allocated in the list and then
       set the break pointer with sys_sbrk (space) that should be the head.

       We can probably use might expensive ways to go back in list and calling
       sbrk() with the exact calculation, but since we don't really know what
       the kernel does, we might overwrite outer data and not only ours.
*/

/* Macros: this is the recomended way to use the interface

     Alloc(size)

     Realloc(ptr, size)

     Release(ptr) : On success case, this also sets the pointer to NULL.

     MemInit(size)

     MemDeinit()

     MEM_DO_NOT_EXIT_ON_FAILURE: this doesn't calls exit() on failure
     MEM_ZERO_FREED: this fills ptr, with zero when the memory is being freed 
     MEM_ZERO_ON_REALLOC: this fills the extra allocated space on ptr, with zero
     MEM_DO_NOT_EXIT_ON_DOUBLE_FREE_POINTER: do not exit on double freed pointer
     MEM_DO_NOT_EXIT_ON_NULL_POINTER: do not exit on NULL pointer
*/

/* This is probably the simplest thing one can do to get a dynamic memory interface.
   The implementation is self explained from the code. There is no magic and it
   doesn't get time to understand how an allocator basically works. Quite probably
   this should be slow compared with overoptimized allocators with sophisticated
   algorithms.
 */

#define STRUCT_SIZE   (sizeof (memChunkT))
#if __WORDSIZE == 64
#define STRUCT_OFFSET (STRUCT_SIZE - 8)
#else
#define STRUCT_OFFSET (STRUCT_SIZE - 4)
#endif

#define ALIGN_SIZE 8
#define ALIGN(__sz__) (((__sz__) + (ALIGN_SIZE - 1)) & ~(ALIGN_SIZE - 1))

typedef struct memChunkT {
  size_t size;
  int is_available;
  struct memChunkT* next;
  struct memChunkT* prev;
  char end[1];
} memChunkT;

memChunkT *memHead = NULL;
memChunkT *lastChunk = NULL;
void *endBreakPoint = NULL;

/* not ready
   pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
*/

static intptr_t mem_arena_size (void *beg, void *end) {
  return (char *) end - (char *) beg;
}

#ifdef MEM_DEBUG
static int memnumBrks = 0;
static size_t memnumSize = 0;

int mem_validate (void);
int mem_validate (void) {
  memChunkT* ptr = memHead;
  memChunkT* prev = ptr;

  while (ptr != NULL) {
    if (ptr->next != NULL) {
      if (ptr->next <= ptr) {
        sys_fprintf (sys_stderr, "ERROR ptr %p, next %p\n", ptr, ptr->next);
        return -1;
      }
    }

    ptr = ptr->next;
    if (ptr != NULL && (char *) prev + prev->size != (char *) ptr) {
        sys_fprintf (sys_stderr, "ERROR prev + size is not ptr %p, next %p prev %p diff %d prev-size %u\n",
         prev + prev->size, ptr, prev, (char *) ptr - (char *) prev, prev->size);
        return -1;
     }

    prev = ptr;

  }

  if (prev)
   if ((char *) prev + prev->size != (char *) endBreakPoint) {
    sys_fprintf (sys_stderr, "ERROR last one isnot endBreakpoin %p, size %p, endbrkp %p diff %u\n",
    prev, prev->size, endBreakPoint, (char *) endBreakPoint- (char *) prev);
    return -1;
   }

  return 0;
}
#endif

static memChunkT *findChunk (memChunkT *chunkptr, uint size, memChunkT **lastchunk) {
  memChunkT* ptr = chunkptr;

  while (ptr != NULL) {
    if (ptr->is_available && ptr->size >= size){
      return ptr;
    }

    *lastchunk = ptr;
    ptr = ptr->next;
  }

  return ptr;
}

static void splitChunk (memChunkT *cur, size_t size) {
  size_t atleast = ALIGN_SIZE + STRUCT_SIZE;
  size_t newsz = cur->size - size;

  if (cur->size < size || newsz < atleast)
    return;

  memChunkT *newchunk = (memChunkT *) ((char *) cur + size);

  newchunk->size = newsz;
  newchunk->is_available = 1;

  newchunk->next = cur->next;
  if (newchunk->next != NULL)
    newchunk->next->prev = newchunk;

  newchunk->prev = cur;
  cur->next = newchunk;

  cur->size = size;
  cur->is_available = 0;
}

static void mergeChunks (memChunkT *freed) {
  memChunkT *cur = freed;
  memChunkT *prev = cur->prev;

  while (prev != NULL && prev->is_available) {
    prev->size += cur->size;
    prev->next = cur->next;

    if (cur->next != NULL)
      cur->next->prev = prev;

    cur = prev;
    prev = prev->prev;
  }

  memChunkT *next = cur->next;

  while (next != NULL && next->is_available) {
    cur->size += next->size;
    cur->next = next->next;

    if (next->next != NULL)
      next->next->prev = cur;

    next = next->next;
  }
}

static memChunkT *increaseAllocation (memChunkT *lastchunk, size_t size) {
  memChunkT* curbreak = endBreakPoint;

  if (sys_sbrk (size) == (void *) -1) {
    sys_errno = ENOMEM;
    return NULL;
  }

  curbreak->size = size;
  curbreak->is_available = 0;
  curbreak->next = NULL;
  curbreak->prev = lastchunk;

  if (lastchunk != NULL)
  lastchunk->next = curbreak;

  if (memHead == NULL)
    memHead = curbreak;

#ifdef MEM_DEBUG
  memnumBrks++;
  memnumSize += size;
  if (-1 == mem_validate ())
    exit (1);
#endif

  endBreakPoint = sys_sbrk (0);

  return curbreak;
}

int mem_deinit (void) {
  if (memHead == NULL) return 0;

  intptr_t sz = mem_arena_size (memHead, endBreakPoint);

  if ((void *) -1 == sys_sbrk (-sz))
    return -1;

  endBreakPoint = sys_sbrk (0);

  memHead = NULL;

#ifdef MEM_DEBUG
  sys_fprintf (sys_stdout, "Num of brk() system calls %d\n", memnumBrks);
  sys_fprintf (sys_stdout, "Total allocated size %u\n", memnumSize);
#endif

  return 0;
}

int mem_init (size_t sz) {
  if (memHead != NULL)
    return 0;

  endBreakPoint = sys_sbrk (0);

  // pthread_mutex_lock (&lock);

  if (sys_sbrk (sz) == (void *) -1) {
    // pthread_mutex_unlock (&lock);
    sys_errno = ENOMEM;
    #ifndef MEM_DO_NOT_EXIT_ON_FAILURE
    exit (1);
    #endif
    return -1;
  }

  memHead = endBreakPoint;
  memHead->size = sz;
  memHead->is_available = 1;
  memHead->next = NULL;
  memHead->prev = NULL;

#ifdef MEM_DEBUG
  memnumBrks++;
  memnumSize += sz;
#endif

  endBreakPoint = sys_sbrk (0);

  // pthread_mutex_unlock (&lock);

  return 1;
}

static void *mem_zero (void *mem, size_t size) {
  char *sp = (char *) mem;
  for (size_t i = 0; i < size; i++)
    sp[i] = 0;

  return mem;
}

void *sys_malloc (size_t size) {
  size_t sz = ALIGN(size + STRUCT_SIZE);

  switch (mem_init (sz)) {
    case  1: {
      splitChunk (memHead, sz);
      memHead->is_available = 0;
      return mem_zero (memHead->end, sz - STRUCT_SIZE);
    }

    case -1:
    #ifndef MEM_DO_NOT_EXIT_ON_FAILURE
      exit (1);
    #endif

    return NULL;
  }

  // pthread_mutex_lock (&lock);

  memChunkT *freechunk = NULL;
  memChunkT *lastchunk = NULL;

  freechunk = findChunk (memHead, sz, &lastchunk);

  if (freechunk == NULL) {
    freechunk = increaseAllocation (lastchunk, sz);

    if (freechunk == NULL) {
      // pthread_mutex_unlock (&lock);
    #ifndef MEM_DO_NOT_EXIT_ON_FAILURE
      exit (1);
    #endif

      return NULL;
    }

    // pthread_mutex_unlock (&lock);
    return mem_zero (freechunk->end, sz - STRUCT_SIZE);
  }

  freechunk->is_available = 0;

  splitChunk (freechunk, sz);

  // pthread_mutex_unlock (&lock);    

  return mem_zero (freechunk->end, sz - STRUCT_SIZE);
}

void *sys_realloc (void *ptr, size_t size) {
  char *old_p = NULL;
  memChunkT *oldchunk = NULL;
  size_t osz = 0;

  if (ptr != NULL) {
    old_p = (char *) ptr;
    oldchunk = (memChunkT *) (old_p - STRUCT_OFFSET);
    osz = oldchunk->size - STRUCT_SIZE;
  }

  void *p = sys_malloc (size);

  if (p == NULL) return NULL;

  if (ptr == NULL) {
    ptr = p;
    return ptr;
  }

  // pthread_mutex_lock (&lock);
  char *new_p = (char *) p;
  memChunkT *newchunk = (memChunkT *) (new_p - STRUCT_OFFSET);
  size_t nsz = newchunk->size - STRUCT_SIZE;

  size_t i = 0;
  for (; i < osz && i < nsz; i++)
    new_p[i] = old_p[i];

  sys_free (ptr);
  ptr = p;

  newchunk->is_available = 0;

  // pthread_mutex_unlock (&lock);    

  return ptr;
}


uint sys_free (void *ptr) {
  if (ptr == NULL) {
  #ifndef MEM_DO_NOT_EXIT_ON_NULL_POINTER
    sys_fprintf (sys_stderr, "%s: argument is a NULL pointer\n", __func__);
    exit (1);
  #endif
    return 1;
  }
  // pthread_mutex_lock (&lock);
  memChunkT *chunk = (memChunkT *) ((char *) ptr - STRUCT_OFFSET);

  if (chunk->is_available) {
  #ifndef MEM_DO_NOT_EXIT_ON_DOUBLE_FREE_POINTER
    sys_fprintf (sys_stderr, "Double free pointer %p\n", chunk);
    exit (1);
  #endif
  }

  #ifdef MEM_ZERO_FREED
  char *sp = (char *) ptr;
  size_t sz = chunk->size - STRUCT_SIZE;
  for (size_t i = 0; i < sz; i++)
    sp[i] = 0;
  #endif

  if ((char *) chunk >= (char *) memHead && (char *) chunk < (char *) endBreakPoint) {
    chunk->is_available = 1;

    mergeChunks (chunk);
    // pthread_mutex_unlock (&lock);

    return 0;
  }

  // pthread_mutex_unlock (&lock);
  return 1;
}
