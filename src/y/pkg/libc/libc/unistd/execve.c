// provides: long execve (const char *, char *const *, char **)

long execve (const char* exe, char *const *argv, char **envp) {
  return syscall3 (NR_execve, (long) exe, (long) argv, (long) envp);
}
