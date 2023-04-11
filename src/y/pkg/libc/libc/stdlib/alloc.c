// provides: void *sys_malloc (size_t)
// provides: void *sys_calloc (size_t, size_t)
// provides: void *sys_realloc (void *, size_t)
// provides: uint sys_free (void *)
// provides: int  mem_init (size_t)
// provides: int  mem_deinit (void)
// requires: sys/brk.c
// requires: stdlib/_exit.c

/* this pulled from:
   https://github.com/miguelperes/custom-malloc
   at :commit a07735c5ef3f467b0b4f1cf0fb217e737e74a3a7

   It should be noted that initially has been tried the code from:
   https://github.com/RAGUL1902/Dynamic-Memory-Allocation-in-C
   with the exact logic. This is condireded as an important factor
   to the realization off the mechanics.

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
      realloc(), calloc(), mem_init(), mem_deinit()

   Also, we now exit on sys_sbrk() failure. This can be disabled with the:
   MEM_DO_NOT_EXIT_ON_FAILURE macro definition before including this libc.

   Also, when merging chunks, we don't longer stop to the first one to the
   lhs and rhs chunks of the current freed object, but we continue to loop
   until we find a NULL chunk pointer or the chunk is not available.

   For now when the requested size is zero, then eight bytes are allocated.
   Haven't settled down on this and how to handle properly. The user should
   know it with a way or even exit hard. This should be handled by a macro
   or even with an option.
*/

/* Function Interface and Semantics:
     sys_malloc (size)
       returns NULL or calll exit() on failure, a void pointer otherwise

     sys_realloc(ptr, size)
       when ptr is NULL it calls malloc (size)
       returns NULL or exits() on failure, a void pointer otherwise and:
          copy the old data to the new space
          fill the extra space if any, with zero, if the MEM_ZERO_ON_REALLOC
            has been defined
          release pointer
          set pointer to the allocated memory address 

     sys_calloc(nmemb, size)
       checks for integer overflows, if the macro MEM_WITHOUT_INT_OVERFLOW_CHECK
         hasn't been defined
       returns NULL or call exit() on failure, a void pointer otherwise and
       fills whith zero the allocated space

     sys_free (ptr)
       this free() returns an unsigned int, which is greater than zero on failure
       and zero otherwise
       it fills also with zero the ptr, when the macro MEM_ZERO_FREED has been defined

     mem_init (size)
       initialize the memory arena with size
       returns zero if the arena has already been initialized, -1 on failure or
       1 on success

       This was meant to be called on the application initialization.
       It doesn'r required to be called at any point.

     mem_deinit ()
       deinitialize the memory arena by setting the breakoint to the initial value
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

     Alloc(size)  : this actually calls calloc() with 1 as nmemb.

     Realloc(ptr, size)

     Release(ptr) : On success case, this also sets the pointer to NULL.

     MemInit(size)

     MemDeinit()

     MEM_DO_NOT_EXIT_ON_FAILURE: this doesn't calls exit() on failure
     MEM_ZERO_FREED: this fills ptr, with zero when the memory is being freed 
     MEM_ZERO_ON_REALLOC: this fills the extra allocated space on ptr, with zero
     MEM_WITHOUT_INT_OVERFLOW_CHECK: this avoids the check for overflows
*/

/* This is probably the simplest thing one can do to get a dynamic memory interface.
   The implementation is self explained from the code. There is no magic and it
   doesn't get time to understand how an allocator basically works. Quite probably
   this should be slow compared with overoptimized allocators with sophisticated
   algorithms.
 */

#define STRUCT_SIZE   (sizeof (memChunkT))
#define STRUCT_OFFSET (STRUCT_SIZE - 4)
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
memChunkT *lastVisited = NULL;
void *endBreakPoint = NULL;

/* not ready
   pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
*/

static memChunkT *findChunk (memChunkT *chunkptr, uint size) {
  memChunkT* ptr = chunkptr;

  while (ptr isnot NULL) {
    if (ptr->size >= (size + STRUCT_SIZE) and ptr->is_available)
      return ptr;

    lastVisited = ptr; // this should be the tail of the list if ptr->next is NULL
    ptr = ptr->next;
  }

  return ptr;
}

static void splitChunk (memChunkT* ptr, unsigned int size) {
  memChunkT *newchunk;

  newchunk = (memChunkT *) (ptr->end + size);
  newchunk->size = ptr->size - size - STRUCT_SIZE;
  newchunk->is_available = 1;
  newchunk->next = ptr->next;
  newchunk->prev = ptr;

 if (newchunk->next isnot NULL)
   newchunk->next->prev = newchunk;

  ptr->size = size;
  ptr->is_available = 0;
  ptr->next = newchunk;
}

static memChunkT *increaseAllocation (memChunkT *lastptr, unsigned int size) {
  endBreakPoint = sys_sbrk (0);
  memChunkT* curbreak = endBreakPoint;

  if (sys_sbrk (size + STRUCT_SIZE) is (void *) -1) {
    sys_errno = ENOMEM;
    return NULL;
  }

  curbreak->size = size;
  curbreak->is_available = 0;
  curbreak->next = NULL;
  curbreak->prev = lastptr;
  lastptr->next = curbreak;

  if (curbreak->size > size)
    splitChunk (curbreak, size);

  return curbreak;
}

static void mergeChunkPrev (memChunkT *freed) {
  memChunkT *cur = freed;
  memChunkT *prev = freed->prev;

  while (prev isnot NULL and prev->is_available) {
    prev->size += (cur->size + STRUCT_SIZE);
    prev->next = cur->next;
    if(cur->next isnot NULL)
      cur->next->prev = prev;
    cur = prev;
    prev = prev->prev;
  }
}

static void mergeChunkNext (memChunkT *freed) {
  memChunkT *next;
  next = freed->next;
  while (next isnot NULL and next->is_available) {
    freed->size += (STRUCT_SIZE + next->size);
    freed->next = next->next;
    if (next->next isnot NULL)
      next->next->prev = freed;
    next = next->next;
  }
}

int mem_deinit (void) {
  if (memHead is NULL) return 0;

  endBreakPoint = sys_sbrk (0);

  intptr_t sz = (char *) endBreakPoint - (char *) memHead;

  if ((void *) -1 is sys_sbrk (-sz))
    return -1;

  endBreakPoint = sys_sbrk (0);
  memHead = NULL;

  return 0;
}

int mem_init (size_t size) {
  if (memHead isnot NULL)
    return 0;

  endBreakPoint = sys_sbrk (0);

  // pthread_mutex_lock (&lock);
  if (sys_sbrk (size) is (void*) -1) {
    // pthread_mutex_unlock (&lock);
    sys_errno = ENOMEM;
    #ifndef MEM_DO_NOT_EXIT_ON_FAILURE
    exit (1);
    #endif
    return -1;
  }

  memHead = endBreakPoint;
  memHead->size = size - STRUCT_SIZE;
  memHead->is_available = 0;
  memHead->next = NULL;
  memHead->prev = NULL;

  // pthread_mutex_unlock (&lock);
  return 1;
}

void *sys_malloc (size_t size) {
  ifnot (size) size = ALIGN_SIZE;

  unsigned int sz = ALIGN(size);

  switch (mem_init (sz + STRUCT_SIZE)) {
    case  1: return memHead->end;
    case -1: return NULL;
  }

  // pthread_mutex_lock (&lock);

  memChunkT *freechunk = NULL;
  freechunk = findChunk (memHead, sz);

  if (freechunk is NULL) {
    freechunk = increaseAllocation (lastVisited, sz);
    if (freechunk is NULL) {
      // pthread_mutex_unlock (&lock);
    #ifndef MEM_DO_NOT_EXIT_ON_FAILURE
      exit (1);
    #endif
      return NULL;
    }

    // pthread_mutex_unlock (&lock);
    return freechunk->end;
  }

  if (freechunk->size > sz)
    splitChunk (freechunk, sz);

  // pthread_mutex_unlock (&lock);    
  return freechunk->end;
}

void *sys_realloc (void *ptr, size_t size) {
  void *p = sys_malloc (size);

  if (p is NULL) return NULL;

  if (ptr is NULL) {
    ptr = p;
    return ptr;
  }

  // pthread_mutex_lock (&lock);
  char *new_p = (char *) p;
  memChunkT *newchunk = (memChunkT *) ((char *) new_p - STRUCT_OFFSET);
  char *old_p = (char *) ptr;
  memChunkT *oldchunk = (memChunkT *) ((char *) old_p - STRUCT_OFFSET);

  size_t i = 0;
  for (; i < oldchunk->size and i < newchunk->size; i++)
    new_p[i] = old_p[i];

  #ifdef MEM_ZERO_ON_REALLOC
  for (; i < newchunk->size; i++)
    new_p[i] = 0;
  #endif

  sys_free (ptr);
  ptr = p;
  newchunk->is_available = 0;

  // pthread_mutex_unlock (&lock);    
  return ptr;
}

void *sys_calloc (size_t nmemb, size_t size) {
  #ifndef MEM_WITHOUT_INT_OVERFLOW_CHECK
  if (nmemb and size) {
    /* this is from picolibc/newlib/libc/stdlib/mul_overflow.h */
    if ((nmemb > SIZE_MAX / size) or (size > SIZE_MAX / nmemb)) {
      sys_errno = EINTEGEROVERFLOW;
      return NULL;
    }
  }
  #endif

  void *s = sys_malloc (nmemb * size);
  if (s is NULL) return NULL;

  char *sp = (char *) s;
  for (size_t i = 0; i < size; i++) sp[i] = 0;

  return s;
}

uint sys_free (void *ptr) {
  // pthread_mutex_lock (&lock);
  memChunkT *toFree = (memChunkT *) ((char *) ptr - STRUCT_OFFSET);
  #ifdef MEM_ZERO_FREED
  char *sp = (char *) ptr;
  for (size_t i = 0; i < toFree->size; i++)
    sp[i] = 0;
  #endif

  if (toFree >= memHead and toFree <= (memChunkT *) endBreakPoint) {
    toFree->is_available = 1;
    mergeChunkNext (toFree);
    mergeChunkPrev (toFree);
    // pthread_mutex_unlock (&lock);
    return 0;
  }

  // pthread_mutex_unlock (&lock);
  return 1;
}

#ifndef WITHOUT_ALLOC_INTERFACE
  #ifdef Alloc
  #undef Alloc
  #endif

  #ifdef Realloc
  #undef Realloc
  #endif

  #ifdef Release
  #undef Release
  #endif

  #ifdef MemInit
  #undef MemInit
  #endif

  #ifdef MemDeinit
  #undef MemDeinit
  #endif

  #define Alloc(__sz__) sys_calloc (1, __sz__)

  #define Realloc sys_realloc

  #define Release(__p__) ({    \
    int r_ = sys_free (__p__); \
    ifnot (r_) __p__ = NULL;   \
    r_;                        \
  })

  #define MemInit mem_init
  #define MemDeinit mem_deinit

#endif /* WITHOUT_ALLOC_INTERFACE */
