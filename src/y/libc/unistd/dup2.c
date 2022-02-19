// provides: long sys_dup2 (int, int)

long sys_dup2 (int fda, int fdb) {
  return syscall3 (NR_dup3, fda, fdb, 0);
}
