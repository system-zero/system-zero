// provides: long sys_getegid (void)

long sys_getegid (void) {
  return syscall0 (NR_getegid);
}
