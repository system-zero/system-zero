// provides: long sys_close (int)

long sys_close (int fd) {
  return syscall1 (NR_close, fd);
}
