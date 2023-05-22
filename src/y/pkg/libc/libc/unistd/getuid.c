// provides: long sys_getuid (void)

long sys_getuid (void) {
  return syscall0 (NR_getuid);
}
