// provides: long sys_read (int, void *, unsigned long)
// provides: #define read sys_read

long sys_read (int fd, void *buf, unsigned long len) {
  return syscall3 (NR_read, fd, (long) buf, len);
}
