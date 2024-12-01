// provides: long sys_setsid (void)

long sys_setsid (void) {
  return syscall0 (NR_setsid);
}
