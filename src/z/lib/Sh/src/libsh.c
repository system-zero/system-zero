/* a couple of logic bits derived from:
 * https://notabug.org/rain1/s/
 * Many thanks.
 */

#define LIBRARY "Sh"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_WAIT
#define REQUIRE_SIGNAL

#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_PROC_TYPE    DECLARE
#define REQUIRE_SH_TYPE      DONOT_DECLARE

#include <z/cenv.h>

#define $myprop    this->prop
#define $my(__v__) $myprop->__v__

static pid_t CUR_PID = -1;

#define COMMAND_TYPE     0
#define PIPE_TYPE        1
#define CONJ_TYPE        2
#define DISJ_TYPE        3

#define NO_REDIR         0
#define REDIR_NOCLOBBER  1
#define REDIR_APPEND     2
#define REDIR_CLOBBER    3

typedef struct sh_prop {
  int saved_stdin;

  proc_t *head;
  proc_t *tail;
  proc_t *current;
  int num_items;
  int cur_idx;
} sh_prop;

struct sh_t {
  sh_prop *prop;
};

typedef struct shproc_t {
  int
    type,
    saved_stderr,
    stdout_fds[2],
    stderr_fds[2],
    should_exit,
    skip_next_proc,
    redir_type,
    redir_streams;

  string_t
    *redir_stdout_file,
    *redir_stderr_file;

  sigset_t mask;
} shproc_t;

static int sh_append_proc (sh_t *this, proc_t *proc) {
  if ($my(head) is NULL) {
    $my(head) = proc;
    $my(tail) = proc;
    $my(current) = $my(head);
    $my(cur_idx) = 0;
  } else {
    Proc.set.next ($my(tail), proc);
    Proc.set.prev (proc, $my(tail));
    Proc.set.next (proc,  NULL);
    $my(tail) = proc;
  }

  return ++$my(num_items);
}

static void sh_release_list (sh_t *this) {
  proc_t *p = $my(head);
  while (p) {
    proc_t *t = Proc.get.next (p);

    shproc_t *sh = Proc.get.userdata (p);
    String.release (sh->redir_stdout_file);
    String.release (sh->redir_stderr_file);
    if (sh->saved_stderr isnot STDERR_FILENO) {
      dup2 (STDERR_FILENO, sh->saved_stderr);
      close (sh->saved_stderr);
    }

    free (sh);

    Proc.release (p);

    p = t;
  }

  $my(head) = $my(tail) = $my(current) = NULL;
  $my(num_items) = 0;
  $my(cur_idx) = -1;
}

static void sh_release (sh_t *this) {
  if (this is NULL) return;

  sh_release_list (this);

  free ($myprop);
  free (this);
  this = NULL;
}

static int sh_read_stream_cb (proc_t *proc, FILE *stream, FILE *read_fp) {
  shproc_t *sh = (shproc_t *) Proc.get.userdata (proc);

  string_t *file = NULL;

  if (stream is stdout)
    file = sh->redir_stdout_file;
  else if (stream is stderr)
    file = sh->redir_stderr_file;

  if (NULL is file) return OK;

  FILE *fp = NULL;
  if (sh->redir_type is REDIR_NOCLOBBER) {
    if (0 is access (file->bytes, F_OK)) {
      fprintf (stderr, "%s: exists, use the >| operator to continue writting\n",
          file->bytes);
      return OK;
    }

    fp = fopen (file->bytes, "w");
  } else if (sh->redir_type is REDIR_APPEND) {
    fp = fopen (file->bytes, "a+");
  } else {
    if (stream is stdout) {
      fp = fopen (file->bytes, "w");
    } else {
      if (sh->redir_streams & PROC_READ_STDOUT)
        fp = fopen (file->bytes, "a+");
      else
        fp = fopen (file->bytes, "w");
    }
  }

  char *line = NULL;
  size_t len = 0;
  while (-1 isnot getline (&line, &len, read_fp))
    fprintf (fp, "%s", line);

  ifnot (NULL is line) free (line);

  fclose (fp);

  return OK;
}

static int sh_pre_fork_default_cb (proc_t *proc) {
  shproc_t *sh = (shproc_t *) Proc.get.userdata (proc);

  sigemptyset (&sh->mask);
  sigaddset (&sh->mask, SIGCHLD);
  sigprocmask (SIG_BLOCK, &sh->mask, NULL);

  return OK;
}

static int sh_pre_fork_pipeline_cb (proc_t *this) {
  shproc_t *sh = (shproc_t *) Proc.get.userdata (this);

  if (-1 is pipe (&sh->stdout_fds[0]))
    return NOTOK;

  /*if (sh->redir_streams & PROC_READ_STDERR)
    if (-1 is pipe (&sh->stderr_fds[0]))
      return NOTOK;
  */

  return sh_pre_fork_default_cb (this);
}

static int sh_at_fork_default_cb (proc_t *proc) {
  shproc_t *sh = (shproc_t *) Proc.get.userdata (proc);

  sigprocmask (SIG_UNBLOCK, &sh->mask, NULL);

  CUR_PID = getpid ();

  if (sh->redir_streams & PROC_READ_STDERR) {
    if (sh->redir_streams & PROC_READ_STDOUT) {
      sh->saved_stderr = dup (STDERR_FILENO);
      dup2 (STDOUT_FILENO, STDERR_FILENO);
    }
  }

  return OK;
}

static int sh_dup_stream (int *fds, int stream_fd) {
  if (-1 is close (fds[0]))
    return NOTOK;

  if (-1 is close (stream_fd))
    return NOTOK;

  if (-1 is dup (fds[1]))
    return NOTOK;

  if (-1 is close (fds[1]))
    return NOTOK;

  return OK;
}

static int sh_at_fork_pipeline_cb (proc_t *proc) {
  shproc_t *sh = (shproc_t *) Proc.get.userdata (proc);

  if (NOTOK is sh_dup_stream (sh->stdout_fds, STDOUT_FILENO))
    return NOTOK;

  sh->redir_streams |= PROC_READ_STDOUT;

  return sh_at_fork_default_cb (proc);
}

static void sh_sigint_handler (int sig) {
  (void) sig;

  if (CUR_PID is -1) return;

  kill (-CUR_PID, SIGINT);
}

static int sh_interpret (proc_t *this) {
  if (NULL is this) return 1;

  shproc_t *sh = (shproc_t *) Proc.get.userdata (this);

  int retval = 1;
  sh->should_exit = 0;
  sh->skip_next_proc = 0;

  switch (sh->type) {
    case COMMAND_TYPE:
      retval = Proc.exec (this, NULL);
      break;

    case CONJ_TYPE:
      retval = Proc.exec (this, NULL);
      sh->skip_next_proc = retval isnot 0;
      break;

    case DISJ_TYPE:
      retval = Proc.exec (this, NULL);
      sh->skip_next_proc = retval is 0;
      break;

    case PIPE_TYPE:
      retval = Proc.open (this);

      if (NOTOK is retval) {
        sh->should_exit = 1;
        break;
      }

      if (-1 is close (sh->stdout_fds[1]) or
          -1 is close (STDIN_FILENO) or
          -1 is dup (sh->stdout_fds[0]) or
          -1 is close (sh->stdout_fds[0])) {
        retval = -1;
        sh->should_exit = 1;
        break;
      }

      //retval = Proc.wait (this);
      /* allow
       * if (retval isnot 0)
       * sh->should_exit = 1;
       */

      break;

    default:
      break;
   }

   return retval;
}

static sh_t *sh_new (void) {
  sh_t *this = Alloc (sizeof (sh_t));
  $myprop = Alloc (sizeof (sh_prop));
  $my(head) = $my(tail) = $my(current) = NULL;
  $my(cur_idx) = -1; $my(num_items) = 0;
  return this;
}

/* more than couple of cases are not handled yet:
 *  - quoted arguments
 *  - globbing
 *  - valid redirections
 *  - informative messages
 *  - ...
 */
static int sh_parse (sh_t *this, char *buf) {
  if (NULL is buf) return NOTOK;

  size_t len = bytelen (buf);
  ifnot (len) return NOTOK;

  char cbuf[len + 1];
  Cstring.cp (cbuf, len + 1, buf, len);

  proc_t *p;
  shproc_t *sh;

  char *sp = cbuf;
  buf = sp;

  int type = COMMAND_TYPE;
  int redir_type = NO_REDIR;
  int redir_streams = 0;
  int redir_stderr = 0;
  int redir_stderr_to_stdout = 0;

  while (*sp) {
    if (*sp is '2') {
      if (sp is buf) goto next;
      if (*(sp - 1) isnot ' ' and *(sp - 1) isnot '\t') goto next;
      if (Cstring.eq_n (sp, "2>&1", 4)) {
        redir_stderr_to_stdout = 1;
        *sp = '\0';
        sp += 3;
        goto next;
      }

      if (*(sp + 1) isnot '>') goto next;

      redir_stderr = 1;
      *sp = '\0';
      goto next;
    }

    if (*sp is '|') {
      ifnot (*(sp + 1)) goto theerror;

      if (*(sp + 1) is '|') {
        ifnot (*(sp + 2)) goto theerror;

        type = DISJ_TYPE;
        *sp = '\0';
        sp += 2;
        goto add_proc;
      }

      type = PIPE_TYPE;
      *sp = '\0';
      sp++;
      goto add_proc;
    }

    if (*sp is '&') {
      ifnot (*(sp + 1)) goto theerror;

      if (*(sp + 1) isnot '&') goto theerror;

      ifnot (*(sp + 2)) goto theerror;

      type = CONJ_TYPE;
      *sp = '\0';
      sp += 2;

      goto add_proc;
    }

    if (*sp is '>') {
      *sp++ = '\0';

      ifnot (*sp) goto theerror;

      redir_type = REDIR_NOCLOBBER;

      if (*sp is '&') {
        redir_streams = (PROC_READ_STDOUT|PROC_READ_STDERR);
        sp++;
      } else
        if (redir_stderr)
          redir_streams |= PROC_READ_STDERR;
        else
          redir_streams |= PROC_READ_STDOUT;

      if (*sp is '>' or *sp is '|') {
        redir_type = (*sp is '>' ? REDIR_APPEND : REDIR_CLOBBER);
        sp++;
      }

      while (*sp is ' ' or *sp is '\t') sp++;
      ifnot (*sp) goto theerror;

      goto add_proc;
    }

    goto next;

    add_proc:
      p = Proc.new ();
      sh = Alloc (sizeof (shproc_t));
      sh->redir_stdout_file = NULL;
      sh->redir_stderr_file = NULL;
      sh->saved_stderr = STDERR_FILENO;
      sh->type = type;
      sh->redir_type = redir_type;
      sh->redir_streams = redir_streams;

      if (sh->redir_type isnot NO_REDIR) {
        char file[(sp - cbuf) + 1];

        int idx = 0;
        while (*sp) file[idx++] = *sp++;

        file[idx] = '\0';

        if (sh->redir_streams & PROC_READ_STDOUT) {
          sh->redir_stdout_file = String.new_with (file);
          Proc.set.read_stream_cb (p, PROC_READ_STDOUT, sh_read_stream_cb);
        } else if (sh->redir_streams & PROC_READ_STDERR) {
          sh->redir_stderr_file = String.new_with (file);
          Proc.set.read_stream_cb (p, PROC_READ_STDERR, sh_read_stream_cb);
        }
      }

      Proc.parse (p, buf);

      if (type is PIPE_TYPE) {
        Proc.set.at_fork_cb (p, sh_at_fork_pipeline_cb);
        Proc.set.pre_fork_cb (p, sh_pre_fork_pipeline_cb);
        if (redir_stderr_to_stdout)
          sh->redir_streams |= PROC_READ_STDERR;
      } else {
        Proc.set.pre_fork_cb (p, sh_pre_fork_default_cb);
        Proc.set.at_fork_cb (p, sh_at_fork_default_cb);
      }

      Proc.set.userdata (p, sh);
      sh_append_proc (this, p);

      buf = sp;
      type = COMMAND_TYPE;
      redir_type = NO_REDIR;
      redir_streams = 0;
      redir_stderr = 0;
      redir_stderr_to_stdout = 0;

      ifnot (*sp) goto theend;

    next: sp++;
  }

  if (*buf) goto add_proc;

theend:
  return OK;

theerror:
  sh_release_list (this);
  return NOTOK;
}

static int sh_exec (sh_t *this, char *buf) {
  if (NULL is this) return NOTOK;

  if (NOTOK is sh_parse (this, buf))
    return NOTOK;

  $my(saved_stdin) = dup (STDIN_FILENO);

  CUR_PID = -1;
  signal (SIGINT, sh_sigint_handler);

  int retval = 0;
  proc_t *p = $my(head);

  shproc_t *sh = (shproc_t *) Proc.get.userdata (p);
  int is_pipeline = sh->type is PIPE_TYPE;

  while (p) {
    retval = sh_interpret (p);

    sh = (shproc_t *) Proc.get.userdata (p);

    if (sh->should_exit or retval is NOTOK)
      break;

    if (sh->skip_next_proc) {
      p = Proc.get.next (p);
    }

    if (p isnot NULL)
      p = Proc.get.next (p);

    if (is_pipeline) {
      proc_t *proc = $my(head);
      while (proc and proc isnot $my(tail)) {
        pid_t pid = Proc.get.pid (proc);
        if (pid isnot -1)
          waitpid (-1, NULL, 0);
        proc = Proc.get.next (proc);
      }
    }
  }

  dup2 ($my(saved_stdin), STDIN_FILENO);

  return (retval < OK ? 1 : retval);
}

public sh_T __init_sh__ (void) {
  __INIT__ (proc);
  __INIT__ (string);
  __INIT__ (cstring);

  return (sh_T) {
    .self = (sh_self) {
      .new = sh_new,
      .exec = sh_exec,
      .release = sh_release,
      .release_list = sh_release_list
    }
  };
}
