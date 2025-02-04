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
// requires: unistd/fd.c
// requires: unistd/execve.c
// requires: unistd/pipe.c
// requires: convert/format.c
// requires: string/str_eq.c
// requires: string/string.c
// requires: sys/process.h
// requires: sys/ev.c

// requires: stdio/stdio.c
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
  char buf[4096];
  Proc *proc = event->client_data;

  for (;;) {
    int n = fd_read (event->fd, buf, sizeof (buf));

    if (n <= 0)
      break;

    if (event->fd == proc->stdout_fd) {
      if (proc->print_stdout) tostdout ("%.*s", n, buf);

      if (proc->read_stdout)
        string_append_with_len (proc->stdout_string, buf, n);

      if (proc->redir_stdout_fd != -1)
        fd_write (proc->redir_stdout_fd, buf, n);

    } else if (event->fd == proc->stderr_fd) {
      if (proc->print_stderr) tostderr ("%.*s", n, buf);

      if (proc->read_stderr)
        string_append_with_len (proc->stderr_string, buf, n);

      if (proc->redir_stderr_fd != -1)
        fd_write (proc->redir_stderr_fd, buf, n);
    }

    if (n < (int) sizeof (buf)) break;
  }

  return 0;
}

int sys_process (Proc *proc) {
  if (NULL == proc->argv)
    return -1;

  Ev ev = EvNew ();

  proc->exitval = proc->pid =
    proc->redir_stdout_fd = proc->redir_stderr_fd = -1;

  int stdout_fds[2];
  int stderr_fds[2];
  int stdin_fds[2];

  int open_stdout = (proc->redir_stdout_fname != NULL || proc->read_stdout);
  int open_stderr = (proc->redir_stderr_fname != NULL || proc->read_stderr);
  int write_stdin = proc->stdin_string != NULL;

  if (proc->redir_stdout_fname != NULL) {
    proc->redir_stdout_fd = sys_open3 (proc->redir_stdout_fname, proc->redir_stdout_flags, proc->redir_stdout_perm);
    if (-1 == proc->redir_stdout_fd) {
      tostderr ("%s: %s\n", proc->redir_stdout_fname, errno_string (sys_errno));
      goto theend;
    }
  }

  if (proc->read_stdout)
    proc->stdout_string = string_new (0);

  if (proc->redir_stderr_fname != NULL) {
    if (NULL != proc->redir_stdout_fname &&
         str_eq (proc->redir_stdout_fname, proc->redir_stderr_fname)) {
       proc->redir_stderr_fd = proc->redir_stdout_fd;
    } else {
      proc->redir_stderr_fd = sys_open3 (proc->redir_stderr_fname, proc->redir_stderr_flags, proc->redir_stderr_perm);
      if (-1 == proc->redir_stderr_fd) {
        tostderr ("%s: %s\n", proc->redir_stderr_fname, errno_string (sys_errno));
        goto theend;
      }
    }
  }

  if (proc->read_stderr)
    proc->stderr_string = string_new (0);

  if (open_stdout)
    if (-1 == sys_pipe (stdout_fds))
      goto theend;

  if (open_stderr) {
    if (-1 == sys_pipe (stderr_fds)) {
      close_pipe (1, open_stdout, stdout_fds);
      goto theend;
    }
  }

  if (write_stdin) {
    if (-1 == sys_pipe (stdin_fds)) {
      close_pipe (1,
        open_stdout, stdout_fds,  open_stderr, stderr_fds);
      goto theend;
    }
  }

  if (-1 == (proc->pid = sys_fork ())) {
    close_pipe (1,
      open_stdout, stdout_fds,  open_stderr, stderr_fds,
      write_stdin, stdin_fds);
    goto theend;
  }

  if (0 == proc->pid) {
    if (proc->set_sid)  sys_setsid ();

    if (proc->set_pgid) sys_setpgid (0, 0);

    if (open_stdout) {
      sys_close (stdout_fds[PIPE_READ_END]);
      sys_dup2 (stdout_fds[PIPE_WRITE_END], 1);
    }

    if (open_stderr) {
      sys_close (stderr_fds[PIPE_READ_END]);
      sys_dup2 (stderr_fds[PIPE_WRITE_END], 2);
    }

    if (write_stdin) {
      sys_close (stdin_fds[PIPE_WRITE_END]);
      sys_dup2 (stdin_fds[PIPE_READ_END], 0);
      sys_close (stdin_fds[PIPE_READ_END]);
    }

    sys_execve (proc->argv[0], proc->argv, proc->envp);

    tostderr ("%s: %s\n", proc->argv[0], errno_string (sys_errno));
    _exit (1);
  }

  if (write_stdin) {
    sys_close (stdin_fds[PIPE_READ_END]);
    fd_write (stdin_fds[PIPE_WRITE_END],
          proc->stdin_string->bytes, proc->stdin_string->num_bytes);
    sys_close (stdin_fds[PIPE_WRITE_END]);
  }

  if (open_stdout) sys_close (stdout_fds[PIPE_WRITE_END]);

  if (open_stderr) sys_close (stderr_fds[PIPE_WRITE_END]);

  if (open_stdout + open_stderr == 0)
    goto theend;

  ev_new_state (&ev, 2);

  EventFun fun[NUM_EVENTS];
  fun[READ_EVENT] = procread_fun;
  fun[WRITE_EVENT] = NULL;

  if (open_stdout) {
    proc->stdout_fd = stdout_fds[PIPE_READ_END];
    int flags = sys_fcntl (proc->stdout_fd, F_GETFL);
    sys_fcntl3 (proc->stdout_fd, F_SETFL, flags|O_NONBLOCK);
    ev_add_event (&ev, proc->stdout_fd, EPOLLIN|EPOLLET, fun, proc);
  }

  if (open_stderr) {
    proc->stderr_fd = stderr_fds[PIPE_READ_END];
    int flags = sys_fcntl (proc->stderr_fd, F_GETFL);
    sys_fcntl3 (proc->stderr_fd, F_SETFL, flags|O_NONBLOCK);
    ev_add_event (&ev, proc->stderr_fd, EPOLLIN|EPOLLET, fun, proc);
  }

  ev_watch (&ev, 20);

theend:
  ev_release_state (&ev);

  if (-1 != proc->pid) {
    int status;
    sys_waitpid (proc->pid, &status,  0);

    int r = 0;
    if (WIFEXITED (status))
      r = WEXITSTATUS (status);

    proc->exitval = r;
  }

  return proc->exitval;
}

void proc_release_state (Proc *proc) {
  if (proc->redir_stdout_fd != -1) {
    sys_close (proc->redir_stdout_fd);
    proc->redir_stdout_fd = -1;
  }

  if (proc->redir_stderr_fd != -1) {
    if (proc->redir_stdout_fname == NULL ||
        0 == str_eq (proc->redir_stdout_fname, proc->redir_stderr_fname))
      sys_close (proc->redir_stderr_fd);

    proc->redir_stdout_fd = -1;
  }

  string_release (proc->stdout_string);
  string_release (proc->stderr_string);
}

/* test {
// num-tests: 2
#define REQUIRE_PROCESS
#define REQUIRE_STR_EQ
#define REQUIRE_MAKE_DIR
#define REQUIRE_FD
#define REQUIRE_FCNTL_H
#define REQUIRE_READFILE
#define REQUIRE_UNLINK
#define REQUIRE_RMDIR
#define REQUIRE_STRING
#define REQUIRE_ATOI

#include <libc.h>

#define PROCESS_DIR "/tmp/process_test"

static int test_init (void) {
  if (-1 == make_dir (PROCESS_DIR, 0700)) return -1;
  int fd = sys_open3 ( PROCESS_DIR "/a", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
  if (-1 == fd) return -1;
  sys_close (fd);
  fd = sys_open3 ( PROCESS_DIR "/b", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
  if (-1 == fd) return -1;
  sys_close (fd);
  return 0;
}

static int first_test (int total) {
  int retval = -1;
  tostdout ("[%d] testing process() %s - ", total, __func__);

  if (-1 == test_init ()) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    return -1;
  }

  const char *argv[] = {"/bin/ls", PROCESS_DIR, PROCESS_DIR "/duckdrunkdunk", NULL};
  Proc proc = NewProc (
    .argv = (char **) argv,
    .redir_stdout_fname = (char *) PROCESS_DIR "/out",
    .redir_stderr_fname = (char *) PROCESS_DIR "/out",
    .read_stdout = 1,
    .read_stderr = 1,
    .print_stdout = 0,
    .print_stderr = 0
  );

  readfile_t rf;
  rf.bytes = Alloc (256);
  rf.num_bytes = 0;
  rf.file = PROCESS_DIR "/out";

  retval = sys_process (&proc);

  if (ENOENT != retval) {
    retval = -1;
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  readfile_u (&rf);

  if (0 == str_eq (rf.bytes,
      "/bin/ls: cannot access '" PROCESS_DIR "/duckdrunkdunk': No such file or directory\n"
      PROCESS_DIR ":\na\nb\nout\n")) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  if (0 == str_eq (proc.stderr_string->bytes,
      "/bin/ls: cannot access '" PROCESS_DIR "/duckdrunkdunk': No such file or directory\n")) {
    retval = -1;
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  if (0 == str_eq (proc.stdout_string->bytes,
      PROCESS_DIR ":\na\nb\nout\n")) {
    retval = -1;
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  sys_unlink (PROCESS_DIR "/b");
  sys_unlink (PROCESS_DIR "/a");
  sys_unlink (PROCESS_DIR "/out");
  sys_rmdir  (PROCESS_DIR);

  Release (rf.bytes);

  proc_release_state (&proc);
  return retval;
}

static int second_test (int total) {
  int retval = -1;
  tostdout ("[%d] testing process() %s - ", total, __func__);

  const char *argv[] = {"/bin/wc", "-l", NULL};
  Proc proc = NewProc (
    .argv = (char **) argv,
    .stdin_string = string_new_with ("\n\n\n\n"),
    .read_stdout = 1
  );

  retval = sys_process (&proc);

  if (0 != retval) {
    retval = -1;
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  if (0 == str_eq (proc.stdout_string->bytes, "4\n")) {
    retval = -1;
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 0;

theend:
  proc_release_state (&proc);
  return retval;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;

  int total = str_to_int (argv[1]);
  int num_tests = 0;
  int failed = 0;

  num_tests++;  total++;
  if (first_test (total) == -1) failed++;
  if (second_test (total) == -1) failed++;

  return failed;
}
} */
