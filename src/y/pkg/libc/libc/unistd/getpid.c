// provides: long sys_getpid (void)

long sys_getpid (void) {
  return syscall0 (NR_getpid);
}
