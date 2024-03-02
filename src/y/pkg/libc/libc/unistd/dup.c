// provides: long sys_dup (int)

long sys_dup (int fd) {
  return syscall1 (NR_dup, fd);
}
