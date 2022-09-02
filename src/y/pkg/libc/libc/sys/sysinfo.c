// provides: long sys_info (struct sysinfo *)
// requires: sys/sysinfo.h

long sys_info (struct sysinfo *s) {
  return syscall1 (NR_sysinfo, (long) s);
}
