// provides: long sys_geteuid (void)

long sys_geteuid (void) {
  return syscall0 (NR_geteuid);
}
