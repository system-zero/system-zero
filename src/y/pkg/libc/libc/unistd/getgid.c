// provides: long sys_getgid (void)
long sys_getgid (void) {
  return syscall0 (NR_getgid);
}
