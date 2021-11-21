#define LIBRARY "Proc"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_SYS_STAT
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_WAIT

#define REQUIRE_LIST_MACROS
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_PROC_TYPE    DONOT_DECLARE

#include <z/cenv.h>

typedef struct proc_prop {
  pid_t  pid;

  char
    *stdin_buf,
    **argv;

   int
     argc,
     is_bg,
     status,
     retval,
     sys_errno,
     dup_stdin,
     read_stdout,
     read_stderr,
     stdin_fds[2],
     stdout_fds[2],
     stderr_fds[2],
     setsid,
     setpgid;

   size_t stdin_buf_size;

   ProcRead_cb read_stdout_cb;
   ProcRead_cb read_stderr_cb;
   ProcAtFork_cb at_fork_cb;
   ProcPreFork_cb pre_fork_cb;

   void *user_data;
} proc_prop;

struct proc_t {
  proc_prop *prop;
  proc_t *next;
  proc_t *prev;
};

typedef struct Proc_prop {
  int saved_stdin;

  proc_t *head;
  proc_t *tail;
  proc_t *current;
  int num_items;
  int cur_idx;
} Proc_prop;

struct Proc_t {
  Proc_prop *prop;
};

#define $myprop    this->prop
#define $my(__v__) $myprop->__v__

#define PIPE_READ_END  0
#define PIPE_WRITE_END 1

static int proc_pre_fork_default_cb (proc_t *this) {
  (void) this;
  return OK;
}

static int proc_at_fork_default_cb (proc_t *this) {
  (void) this;
  return OK;
}

static void proc_release_argv (proc_t *this) {
  if (NULL is $my(argv))
    return;

  for (int i = 0; i <= $my(argc); i++)
    free ($my(argv)[i]);
  free ($my(argv));
}

static void proc_release (proc_t *this) {
  ifnot (this) return;
  proc_release_argv (this);
  if (NULL isnot $my(stdin_buf))
    free ($my(stdin_buf));

  free (this->prop);
  free (this);
}

static int proc_output_to_stream (proc_t *this, FILE *stream, FILE *read_fp) {
  (void) this;
  char *line = NULL;
  size_t len = 0;
  while (-1 isnot getline (&line, &len, read_fp))
    fprintf (stream, "%s\r", line);

  ifnot (NULL is line) free (line);
  return 0;
}

static proc_t *proc_new (void) {
  proc_t *this = Alloc (sizeof (proc_t));
  $myprop = Alloc (sizeof (proc_prop));
  $my(pid) = -1;
  $my(stdin_buf) = NULL;
  $my(dup_stdin) = 0;
  $my(read_stdout) = 0;
  $my(read_stderr) = 0;
  $my(argc) = 0;
  $my(argv) = NULL;
  $my(is_bg) = 0;
  $my(setsid) = 0;
  $my(setpgid) = 0;

  $my(read_stdout_cb) = proc_output_to_stream;
  $my(read_stderr_cb) = proc_output_to_stream;
  $my(pre_fork_cb) = proc_pre_fork_default_cb;
  $my(at_fork_cb) = proc_at_fork_default_cb;
  return this;
}

static int proc_wait (proc_t *this) {
  if (-1 is $my(pid)) return NOTOK;
  $my(status) = 0;
  waitpid ($my(pid), &$my(status), 0);
 // waitpid ($my(pid), &$my(status), WNOHANG|WUNTRACED);

  if (WIFEXITED ($my(status)))
    $my(retval) = WEXITSTATUS ($my(status));
  else
    $my(retval) = -1;

  $my(pid) = -1;

  return $my(retval);
}

static int proc_read (proc_t *this) {
  int retval = NOTOK;

  FILE *fp = NULL;

  if ($my(read_stdout)) {
    if ($my(read_stdout_cb) isnot NULL) {
      fp = fdopen ($my(stdout_fds)[PIPE_READ_END], "r");
      retval = $my(read_stdout_cb) (this, stdout, fp);
      fclose (fp);
    }
  }

  if ($my(read_stderr)) {
    if ($my(read_stderr_cb) isnot NULL) {
      fp = fdopen ($my(stderr_fds)[PIPE_READ_END], "r");
      retval = $my(read_stderr_cb) (this, stderr, fp);
      fclose (fp);
    }
  }

  return retval;
}

static char **proc_parse (proc_t *this, char *com) {
  if (NULL is com) return NULL;

  size_t len = bytelen (com);
  ifnot (len) return NULL;

  char *sp = com;

  char *tokbeg;
  $my(argv) = Alloc (sizeof (char *));

  while (*sp) {
    while (*sp and *sp is ' ') sp++;
    ifnot (*sp) break;

    if (*sp is '&' and *(sp + 1) is 0) {
      $my(is_bg) = 1;
      break;
    }

    tokbeg = sp;

    if (*sp is '"') {
      sp++;
      tokbeg++;

parse_quoted:
      while (*sp and *sp isnot '"') sp++;
      ifnot (*sp) goto theerror;
      if (*(sp - 1) is '\\') goto parse_quoted;
      len = (size_t) (sp - tokbeg);
      sp++;
      goto add_arg;
    }

    while (*sp and *sp isnot ' ') sp++;
    ifnot (*sp) {
      if (*(sp - 1) is '&') {
        $my(is_bg) = 1;
        sp--;
      }
    }

    len = (size_t) (sp - tokbeg);

add_arg:
    $my(argc)++;
    $my(argv) = Realloc ($my(argv), sizeof (char *) * ($my(argc) + 1));
    $my(argv)[$my(argc)-1] = Alloc (len + 1);
    Cstring.cp ($my(argv)[$my(argc)-1], len + 1, tokbeg, len);
    ifnot (*sp) break;
    sp++;
  }

  $my(argv)[$my(argc)] = (char *) NULL;
  return $my(argv);

theerror:
  proc_release_argv (this);
  return NULL;
}

static void proc_close_pipe (int num, ...) {
  va_list ap;
  int *p;
  int is_open;
  va_start (ap, num);
  is_open = va_arg (ap, int);
  p = va_arg (ap, int *);
  if (is_open) {
    close (p[0]);
    close (p[1]);
  }
  va_end (ap);
}

static int proc_open (proc_t *this) {
  if (NULL is this) return NOTOK;
  ifnot ($my(argc)) return NOTOK;

  if ($my(dup_stdin)) {
    if (-1 is pipe ($my(stdin_fds)))
      return NOTOK;
  }

  if ($my(read_stdout)) {
    if (-1 is pipe ($my(stdout_fds))) {
      proc_close_pipe (1, $my(dup_stdin), $my(stdin_fds));
      return NOTOK;
    }
  }

  if ($my(read_stderr)) {
    if (-1 is pipe ($my(stderr_fds))) {
      proc_close_pipe (1,
          $my(dup_stdin), $my(stdin_fds),
          $my(read_stdout), $my(stdout_fds));
      return NOTOK;
    }
  }

  if (NOTOK is $my(pre_fork_cb) (this)) {
    proc_close_pipe (1,
        $my(dup_stdin), $my(stdin_fds),
        $my(read_stdout), $my(stdout_fds),
        $my(read_stderr), $my(stderr_fds));
    return NOTOK;
  }

  if (-1 is ($my(pid) = fork ())) {
    proc_close_pipe (1,
       $my(dup_stdin), $my(stdin_fds),
       $my(read_stdout), $my(stdout_fds),
       $my(read_stderr), $my(stderr_fds));
    return NOTOK;
  }

  ifnot ($my(pid)) {
    if ($my(setpgid)) setpgid (0, 0);
    if ($my(setsid)) setsid ();

    if ($my(read_stderr)) {
      dup2 ($my(stderr_fds)[PIPE_WRITE_END], fileno (stderr));
      close ($my(stderr_fds)[PIPE_READ_END]);
    }

    if ($my(read_stdout)) {
      close ($my(stdout_fds)[PIPE_READ_END]);
      dup2 ($my(stdout_fds)[PIPE_WRITE_END], fileno (stdout));
    }

    if ($my(dup_stdin)) {
      dup2 ($my(stdin_fds)[PIPE_READ_END], STDIN_FILENO);
      ifnot (NULL is $my(stdin_buf)) {
        int ign = write ($my(stdin_fds)[PIPE_WRITE_END], $my(stdin_buf), $my(stdin_buf_size));
        (void) ign;
      }

      close ($my(stdin_fds)[PIPE_WRITE_END]);
    }

    if (NOTOK is $my(at_fork_cb) (this))
      _exit (1);

    execvp ($my(argv)[0], $my(argv));
    $my(sys_errno) = errno;
    _exit (1);
  }

  if ($my(dup_stdin)) close ($my(stdin_fds)[PIPE_READ_END]);
  if ($my(read_stdout)) close ($my(stdout_fds)[PIPE_WRITE_END]);
  if ($my(read_stderr)) close ($my(stderr_fds)[PIPE_WRITE_END]);

  return $my(pid);
}

static void proc_unset_stdin (proc_t *this) {
  $my(dup_stdin) = 0;

  ifnot (NULL is $my(stdin_buf))
    free ($my(stdin_buf));
  $my(stdin_buf) = NULL;
}

static void proc_set_next (proc_t *this, proc_t *node) {
  this->next = node;
}

static void proc_set_prev (proc_t *this, proc_t *node) {
  this->prev = node;
}

static void proc_set_stdin (proc_t *this, char *buf, size_t size) {
  if (NULL is buf) return;
  $my(dup_stdin) = 1;
  $my(stdin_buf) = Alloc (size + 1);
  $my(stdin_buf_size) = size;
  Cstring.cp ($my(stdin_buf), size + 1, buf, size);
}

static void proc_set_user_data (proc_t *this, void *user_data) {
  $my(user_data) = user_data;
}

static void proc_set_pre_fork_cb (proc_t *this, ProcPreFork_cb cb) {
  $my(pre_fork_cb) = cb;
}

static void proc_set_at_fork_cb (proc_t *this, ProcAtFork_cb cb) {
  $my(at_fork_cb) = cb;
}

static void proc_set_dup_stdin (proc_t *this, int val) {
  $my(dup_stdin) = 0 isnot val;
}

static void proc_set_read_stream_cb (proc_t *this, int stream_flags, ProcRead_cb cb) {
  if (stream_flags & PROC_READ_STDOUT) {
    $my(read_stdout) = 1;
    $my(read_stdout_cb) = cb;
  }

  if (stream_flags & PROC_READ_STDERR) {
    $my(read_stderr) = 1;
    $my(read_stderr_cb) = cb;
  }
}

static proc_t *proc_get_next (proc_t *this) {
  if (NULL is this) return NULL;
  return this->next;
}

static pid_t proc_get_pid (proc_t *this) {
  if (NULL is this) return -1;
  return $my(pid);
}

static void *proc_get_user_data (proc_t *this) {
  if (NULL is this) return NULL;
  return $my(user_data);
}

static int proc_exec (proc_t *this, char *com) {
  int retval = NOTOK;

  if (NULL is $my(argv))
    if (NULL is proc_parse (this, com))
      goto theend;

  if (NOTOK is proc_open (this)) goto theend;

  proc_read (this);

  ifnot ($my(is_bg))
    retval = proc_wait (this);

theend:
  return retval;
}

public proc_T __init_proc__ (void) {
  __INIT__ (cstring);

  return (proc_T) {
    .self = (proc_self) {
      .new = proc_new,
      .open = proc_open,
      .wait = proc_wait,
      .read = proc_read,
      .exec = proc_exec,
      .parse = proc_parse,
      .release = proc_release,
      .set = (proc_set_self) {
        .next = proc_set_next,
        .prev = proc_set_prev,
        .stdin = proc_set_stdin,
        .dup_stdin = proc_set_dup_stdin,
        .user_data = proc_set_user_data,
        .at_fork_cb = proc_set_at_fork_cb,
        .pre_fork_cb = proc_set_pre_fork_cb,
        .read_stream_cb = proc_set_read_stream_cb,
      },
      .unset = (proc_unset_self) {
        .stdin = proc_unset_stdin
      },
      .get = (proc_get_self) {
        .pid = proc_get_pid,
        .next = proc_get_next,
        .user_data = proc_get_user_data
      }
    }
  };
}
