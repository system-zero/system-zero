// provides: long sys_fcntl (int, int)

long sys_fcntl (int fd, int cmd) {
  return syscall2 (NR_fcntl, fd, cmd);
}
