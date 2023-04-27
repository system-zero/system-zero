// provides: long sys_fcntl (int, int)
// provides: long sys_fcntl3 (int, int, int)

long sys_fcntl (int fd, int cmd) {
  return syscall2 (NR_fcntl, fd, cmd);
}

long sys_fcntl3 (int fd, int cmd, int arg) {
  return syscall3 (NR_fcntl, fd, cmd, arg);
}
