// provides: void *vmem_alloc (size_t)
// provides: void vmem_release (void **)
// provides: size_t vmem_get_actual_size (void *)
// requires: mem/valloc.h
// requires: sys/mmap.c
// requires: sys/mmap.h

/* This interface is an abstraction around the mmap() system call that
   uses the virtual address space:

     https://stackoverflow.com/questions/5877797/how-does-mmap-work
     https://en.wikipedia.org/wiki/Virtual_memory

   The allocators that are using the sbrk() system to claim memory to
   userspace, have the advantage that the layout of the reseved memory
   is contiguous and so provide flexibility and simplicity.

   Their main disadvantage however, is that the released memory doesn't
   really always can go back to the kernel, unless it is the last chunk,
   as to do this you have to call sbrk() with a negative value from the
   breakpoint. mmap() though it doesn't have this limitation as it uses
   addresses which are outside of the heap (the kernel do that for us).

   For this reason only should be adequate for tasks that require large
   chunks that it is best to really released after completion. With the
   other interfaces this is not guaranted, in fact current implementations
   don't even try to do (perhaps they should).

   For now it doesn't do nothing else than to keep track of the allocated
   size, to aling the request to 4096 bytes (pagesize), and do the right
   system calls and probably its best to keep it this way. Other higher
   interfaces (like arenas or flexible arrays) can be built upon this.
 */

/* Status
   For now it isn't tested other than simple ones. Its structure has only
   a size_t member (though it is questionable to ask so wide in 32bit, so
   it can be better abstracted and use uint instead probably). Also there
   are no macros yet.
 */

static inline VmemChunk *vmem_get_from_ptr (void *ptr) {
  return (VmemChunk *) ((char *) ptr - VMEM_HEADER_SIZE);
}

static inline void *vmem_get_ptr (VmemChunk *vmem) {
  return (char *) vmem + VMEM_HEADER_SIZE;
}

static inline size_t vmem_align (size_t size) {
  return (size + (VMEM_ALIGN_SIZE - 1)) & ~(VMEM_ALIGN_SIZE - 1);
}

static inline void *vmem_new (size_t size, size_t *newsize) {
  *newsize = vmem_align (size + VMEM_HEADER_SIZE);
  return sys_mmap (NULL, *newsize,
      PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
}

size_t vmem_get_actual_size (void *ptr) {
  VmemChunk *vmem = vmem_get_from_ptr (ptr);
  return vmem->size - VMEM_HEADER_SIZE;
}

void *vmem_alloc (size_t size) {
  size_t actualsize = 0;
  VmemChunk *vmem = vmem_new (size, &actualsize);

  if ((void *) -1 == vmem)
    return vmem;

  vmem->size = actualsize;

  return vmem_get_ptr (vmem);
}

void vmem_release (void **vmemp) {
  if (*vmemp == NULL)
    return;

  VmemChunk *vmem = vmem_get_from_ptr (*vmemp);

  sys_munmap (vmem, vmem->size);
  *vmemp = NULL;
}
