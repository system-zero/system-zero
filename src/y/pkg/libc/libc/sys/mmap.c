// provides: void *sys_mmap (void *, size_t, int, int, int, size_t)
// provides: void *sys_mremap (void *, size_t, size_t, int)
// provides: long sys_munmap (void *, unsigned long)
// provides: long sys_mprotect (void *,  size_t, int)
// requires: sys/mmap.h

void *sys_mmap (void *addr, size_t length, int prot, int flags, int fd, size_t offset) {
#ifdef NR_mmap2
  return (void*)syscall6 (NR_mmap2, (long) addr, length, prot, flags, fd, offset);
#else
  return (void*)syscall6 (NR_mmap, (long) addr, length, prot, flags, fd, offset);
#endif
}

void *sys_mremap (void *old, size_t oldsize, size_t newsize, int flags) {
  return (void*) syscall4 (NR_mremap, (long) old, oldsize, newsize, flags);
}

long sys_munmap (void *ptr, unsigned long len) {
  return syscall2 (NR_munmap, (long) ptr, len);
}

long sys_mprotect (void *ptr,  size_t len, int prot) {
  return syscall3 (NR_mprotect, (long) ptr, len, prot);
}
