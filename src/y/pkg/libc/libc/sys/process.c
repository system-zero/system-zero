// provides: int sys_process (Proc *)
// provides: void proc_release_state (Proc *)
// requires: stdlib/_exit.c
// requires: sys/waitpid.c
// requires: sys/pid.h
// requires: sys/epoll.c
// requires: unistd/fcntl.c
// requires: unistd/setsid.c
// requires: unistd/fork.c
// requires: unistd/setpgid.c
// requires: unistd/dup2.c
// requires: unistd/close.c
// requires: unistd/read.c
// requires: unistd/write.c
// requires: unistd/execve.c
// requires: unistd/pipe.c
// requires: stdio/stdio.c
// requires: string/str_eq.c
// requires: string/string.h
// requires: string/string_alloc.c
// requires: string/string_release_bytes.c
// requires: string/string_append_with_len.c
// requires: sys/process.h
// requires: sys/ev.c

#define PIPE_READ_END  0
#define PIPE_WRITE_END 1

static void close_pipe (int num, ...) {
  va_list ap;
  int *p;
  int is_open;

  va_start (ap, num);
  is_open = va_arg (ap, int);
  p = va_arg (ap, int *);
  if (is_open) {
    sys_close (p[0]);
    sys_close (p[1]);
  }
  va_end (ap);
}

static int procread_fun (event_t *event) {
  char buf[1024];
  Proc *proc = event->client_data;

  for (;;) {
    int n = sys_read (event->fd, buf, sizeof (buf));

    if (n <= 0 /* || errno == EAGAIN */ )
      break;

    if (event->fd == proc->stdout_fd) {
      if (proc->print_stdout) tostdout ("%.*s", n, buf);
      if (proc->read_stdout)
        string_append_with_len (&proc->stdout_string, buf, n);
      if (proc->stdout_fp != NULL)
        sys_write (sys_fileno (proc->stdout_fp), buf, n);

    } else if (event->fd == proc->stderr_fd) {
      if (proc->print_stderr) tostderr ("%.*s", n, buf);
      if (proc->read_stderr)
        string_append_with_len (&proc->stderr_string, buf, n);
      if (proc->stderr_fp != NULL)
        sys_write (sys_fileno (proc->stderr_fp), buf, n);
    }

    if (n < (int) sizeof (buf)) break;
  }

  return 0;
}

int sys_process (Proc *proc) {
  proc->retval = -1;
  if (NULL == proc->argv)
    return -1;

  Ev ev = EvNew ();

  proc->pid = -1;

  proc->stdout_fp = proc->stderr_fp = NULL;

  int stdout_fds[2];
  int stderr_fds[2];

  if (proc->redir_stdout != NULL) {
    if (*proc->redir_stdout_mode == '\0') {
       *proc->redir_stdout_mode = 'w';
       *(proc->redir_stdout_mode + 1) = '\0';
    }

    proc->stdout_fp = sys_fopen (proc->redir_stdout, proc->redir_stdout_mode);
    if (NULL == proc->stdout_fp) {
      tostderr ("%s: %s\n", proc->redir_stdout, errno_string (sys_errno));
      goto theend;
    }
  }

  if (proc->read_stdout)
    string_alloc_bytes (&proc->stdout_string, 0);

  if (proc->redir_stderr != NULL) {
    if (*proc->redir_stderr_mode == '\0') {
        *proc->redir_stderr_mode = 'w';
        *(proc->redir_stderr_mode + 1) = '\0';
    }

    if (NULL != proc->redir_stdout &&
         str_eq (proc->redir_stdout, proc->redir_stderr)) {
       proc->stderr_fp = proc->stdout_fp;
    } else {
      proc->stderr_fp = sys_fopen (proc->redir_stderr, proc->redir_stderr_mode);
      if (NULL == proc->stderr_fp) {
        tostderr ("%s: %s\n", proc->redir_stderr, errno_string (sys_errno));
        goto theend;
      }
    }
  }

  if (proc->read_stderr)
    string_alloc_bytes (&proc->stderr_string, 0);

  if (proc->redir_stdout != NULL || proc->read_stdout)
    if (-1 == sys_pipe (stdout_fds))
      goto theend;

  if (proc->redir_stderr != NULL || proc->read_stderr) {
    if (-1 == sys_pipe (stderr_fds)) {
      close_pipe (1, proc->redir_stdout, stdout_fds);
      goto theend;
    }
  }

  if (-1 == (proc->pid = sys_fork ())) {
    close_pipe (1,
      proc->redir_stdout, stdout_fds,
      proc->redir_stderr, stderr_fds);
    goto theend;
  }

  if (0 == proc->pid) {
    if (proc->set_sid)  sys_setsid ();
    if (proc->set_pgid) sys_setpgid (0, 0);

    if (proc->redir_stdout != NULL || proc->read_stdout) {
      sys_close (stdout_fds[PIPE_READ_END]);
      sys_dup2 (stdout_fds[PIPE_WRITE_END], sys_fileno (sys_stdout));
    }

    if (proc->redir_stderr != NULL || proc->read_stderr) {
      sys_close (stderr_fds[PIPE_READ_END]);
      sys_dup2 (stderr_fds[PIPE_WRITE_END], sys_fileno (sys_stderr));
    }

    sys_execve (proc->argv[0], proc->argv, proc->envp);

    tostderr ("%s: %s\n", proc->argv[0], errno_string (sys_errno));
    _exit (1);
  }

  if (proc->redir_stdout != NULL || proc->read_stdout)
     sys_close (stdout_fds[PIPE_WRITE_END]);

  if (proc->redir_stderr != NULL || proc->read_stderr)
    sys_close (stderr_fds[PIPE_WRITE_END]);

  if (proc->redir_stdout == NULL && 0 == proc->read_stdout &&
      proc->redir_stderr == NULL && 0 == proc->read_stderr)
    goto theend;

  ev_new_state (&ev, 2);

  EventFun fun[NUM_EVENTS];
  fun[READ_EVENT] = procread_fun;
  fun[WRITE_EVENT] = NULL;

  if (proc->redir_stdout != NULL || proc->read_stdout) {
    proc->stdout_fd = stdout_fds[PIPE_READ_END];
    int flags = sys_fcntl (proc->stdout_fd, F_GETFL);
    sys_fcntl3 (proc->stdout_fd, F_SETFL, flags|O_NONBLOCK);
    ev_add_event (&ev, proc->stdout_fd, EPOLLIN|EPOLLET, fun, proc);
  }

  if (proc->redir_stderr != NULL || proc->read_stderr) {
    proc->stderr_fd = stderr_fds[PIPE_READ_END];
    int flags = sys_fcntl (proc->stderr_fd, F_GETFL);
    sys_fcntl3 (proc->stderr_fd, F_SETFL, flags|O_NONBLOCK);
    ev_add_event (&ev, proc->stderr_fd, EPOLLIN|EPOLLET, fun, proc);
  }

  ev_watch (&ev);

  tostdout ("string:\n%s\n", proc->stdout_string.bytes);

theend:
  ev_release_state (&ev);

  if (-1 != proc->pid) {
    int status;
    sys_waitpid (proc->pid, &status,  0);

    int r = 0;
    if (WIFEXITED (status))
      r = WEXITSTATUS (status);

    proc->retval = r;
  }

  return proc->retval;
}

void proc_release_state (Proc *proc) {
  if (proc->stdout_fp != NULL)
    sys_fclose (proc->stdout_fp);

  if (proc->stderr_fp != NULL && (proc->redir_stdout != NULL &&
       0 == str_eq (proc->redir_stdout, proc->redir_stderr)))
    sys_fclose (proc->stderr_fp);

  if (proc->stdout_string.bytes != NULL)
    string_release_bytes (&proc->stdout_string);

  if (proc->stderr_string.bytes != NULL)
    string_release_bytes (&proc->stderr_string);
}
