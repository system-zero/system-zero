// provides: void *sys_brk (void *)
// provides: void *sys_sbrk (size_t)

void *sys_brk (void *ptr) {
  return (void *) syscall1 (NR_brk, (long) ptr);
}

void *sys_sbrk (size_t inc) {
  void *cur_brk = (void *) syscall1 (NR_brk, 0);

  if (0 == inc)
    return cur_brk;

  long r = syscall1 (NR_brk, (long) cur_brk + inc);
  if (-1 == r)
    return (void *) -1;

  return (void *) r;
}
