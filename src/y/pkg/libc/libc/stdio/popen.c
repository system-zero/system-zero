// provides: FILE *sys_popen (const char *, const char *)
// provides: int sys_pclose (FILE *)
// requires: unistd/close.c
// requires: unistd/dup2.c
// requires: unistd/execve.c
// requires: unistd/fcntl.c
// requires: unistd/fork.c
// requires: unistd/pipe.c
// requires: sys/waitpid.c
// requires: stdio/stdio.c
// requires: stdio/stdio.h
// requires: sys/pid.h

FILE *sys_popen (const char *command, const char *mode) {
  if (NULL == command || NULL == mode) return NULL;

  int is_read = 0;
  int close_on_exec = 0;

  switch (*mode) {
    case 'w': break;
    case 'r': is_read = 1; break;
    default:
      sys_errno = EINVAL;
      return NULL;
  }

  switch (*(mode + 1)) {
    case '\0': break;
    case 'e' : close_on_exec = 1; break;
    default:
      sys_errno = EINVAL;
      return NULL;
  }

  int fds[2];

  if (-1 == sys_pipe (fds))
    return NULL;

  FILE *fp = sys_fdopen (fds[!is_read], mode);

  if (NULL == fp) {
    sys_close (fds[0]);
    sys_close (fds[1]);
    sys_fclose (fp);
    return NULL;
  }

  int pid = sys_fork ();

  if (-1 == pid) {
    sys_close (fds[0]);
    sys_close (fds[1]);
    sys_fclose (fp);
    return NULL;
  }

  if (0 == pid) {
    sys_close (fds[!is_read]);
    sys_close (is_read);

    sys_dup2 (fds[is_read], is_read);
    sys_close (fds[is_read]);

    const char *argv[] = {"/bin/sh", "-c" , command, NULL};
    sys_execve ("/bin/sh", (char *const *) argv, environ);
    _exit (127);
  }

  sys_close (fds[is_read]);

  if (close_on_exec)
    sys_fcntl3 (fds[!is_read], F_SETFD, FD_CLOEXEC);

  fp->pid = pid;

  return fp;
}

int sys_pclose (FILE *fp) {
  pid_t pid = fp->pid;

  sys_fclose (fp);

  int status;
  if (sys_waitpid (pid, &status, 0) >= 0)
    return status;

  return -1;
}
