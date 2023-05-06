// provides: long sys_umask (int)

long sys_umask (int mode) {
  return syscall1 (NR_umask, mode);
}
