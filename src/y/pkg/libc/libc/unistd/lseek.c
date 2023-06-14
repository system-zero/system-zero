// provides: long sys_lseek (int, int64_t, int)

long sys_lseek (int fd, int64_t off, int whence) {
  long ret;

  if((ret = syscall3 (NR_lseek, fd, off, whence)) < 0)
    return ret;

  return 0;
}
