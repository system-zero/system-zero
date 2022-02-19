// provides: ssize_t sys_write (int, const void *, int)

ssize_t sys_write (int fd, const void *buf, int len) {
  return syscall3 (NR_write, fd, (long) buf, len);
}
