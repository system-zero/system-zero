// provides: long sys_execve (const char *, const char **, char **)

long sys_execve (const char* exe, const char **argv, char **envp) {
  return syscall3 (NR_execve, (long) exe, (long) argv, (long) envp);
}
