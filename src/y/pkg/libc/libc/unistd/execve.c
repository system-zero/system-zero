// provides: long sys_execve (const char *, char *const *, char *const *)

long sys_execve (const char *pathname, char *const *argv, char *const *envp) {
  return syscall3 (NR_execve, (long) pathname, (long) argv, (long) envp);
}
