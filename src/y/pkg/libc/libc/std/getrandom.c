// provides: long sys_getrandom (void *, size_t, int)
// requires: std/random.h

long sys_getrandom (void *buf, size_t len, int flags) {
  return syscall3 (NR_getrandom, (long) buf, len, flags);
}
