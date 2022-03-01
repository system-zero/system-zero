// provides: void *sys_brk (void *)

void *sys_brk (void *ptr) {
  return (void*) syscall1 (NR_brk, (long) ptr);
}
