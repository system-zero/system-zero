/* A couple of logic bits derived from:
 * https://notabug.org/rain1/s/
 * Many thanks.
 */

#define LIBRARY "Sh"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_SYS_WAIT
#define REQUIRE_SIGNAL

#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_SYS_TYPE     DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_PROC_TYPE    DECLARE
#define REQUIRE_PATH_TYPE    DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_IO_TYPE      DECLARE
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

#define SH_NO_BUILTIN    -10
#define SH_EXIT_NOW      -11

typedef struct sh_prop {
  char error[SH_MAXLEN_ERROR];
  int saved_stdin;
  int exit_val;
  int should_exit;
  int is_a_pipe;
  string **pipe_output;
  Vstring_t *cdpath;

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
  sh_t *sh;

  int
    type,
    exit_val,
    redir_type,
    should_exit,
    saved_stderr,
    skip_next_proc,
    should_skip_wait,
    stdout_fds[2],
    stderr_fds[2],
    redir_streams;

  string
    *redir_stdout_file,
    *redir_stderr_file;

  sigset_t mask;
} shproc_t;

static int sh_pipe_cb (proc_t *, char *, string **);

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

    shproc_t *sh = Proc.get.user_data (p);
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
  Vstring.release ($my(cdpath));

  free ($myprop);
  free (this);
  this = NULL;
}

static int sh_get_exit_val (sh_t *this) {
  return $my(exit_val);
}

static char *sh_get_error (sh_t *this) {
  return $my(error);
}

static int sh_should_exit (sh_t *this) {
  return $my(should_exit);
}

static int sh_read_stream_cb (proc_t *proc, FILE *stream, FILE *read_fp) {
  shproc_t *sh = (shproc_t *) Proc.get.user_data (proc);

  string *file = NULL;

  if (stream is stdout)
    file = sh->redir_stdout_file;
  else if (stream is stderr)
    file = sh->redir_stderr_file;

  if (NULL is file) return OK;

  FILE *fp = NULL;
  if (sh->redir_type is REDIR_NOCLOBBER) {
    if (0 is access (file->bytes, F_OK)) {
      Stderr.print_fmt ("%s: exists, use the >| operator to continue writting\n",
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

  if (fp is NULL) return OK;

  char *line = NULL;
  size_t len = 0;
  while (-1 isnot getline (&line, &len, read_fp))
    fprintf (fp, "%s", line);

  ifnot (NULL is line) free (line);

  fclose (fp);

  return OK;
}

static int sh_read_pipe_cb (proc_t *proc, FILE *stream, FILE *read_fp) {
  (void) stream;

  shproc_t *sh = (shproc_t *) Proc.get.user_data (proc);
  sh_t *this = sh->sh;

  string *out = NULL;

  out = *$my(pipe_output);

  if (NULL is out)
    out = String.new (32);

  char *line = NULL;
  size_t len = 0;
  ssize_t nread = 0;

  while (-1 isnot (nread = getline (&line, &len, read_fp)))
    String.append_with_len (out, line, nread);

  String.trim_end (out, '\n');

  ifnot (NULL is line) free (line);

  *$my(pipe_output) = out;

  return OK;
}

static int sh_pre_fork_default_cb (proc_t *proc) {
  shproc_t *sh = (shproc_t *) Proc.get.user_data (proc);

  sigemptyset (&sh->mask);
  sigaddset (&sh->mask, SIGCHLD);
  sigprocmask (SIG_BLOCK, &sh->mask, NULL);

  return OK;
}

static int sh_pre_fork_pipeline_cb (proc_t *this) {
  shproc_t *sh = (shproc_t *) Proc.get.user_data (this);

  if (-1 is pipe (&sh->stdout_fds[0]))
    return NOTOK;

  /*if (sh->redir_streams & PROC_READ_STDERR)
    if (-1 is pipe (&sh->stderr_fds[0]))
      return NOTOK;
  */

  return sh_pre_fork_default_cb (this);
}

static int sh_at_fork_default_cb (proc_t *proc) {
  shproc_t *sh = (shproc_t *) Proc.get.user_data (proc);

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
  shproc_t *sh = (shproc_t *) Proc.get.user_data (proc);

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

static int sh_builtins (shproc_t *sh, proc_t *proc) {
  int retval = SH_NO_BUILTIN;
  sh_t *this = sh->sh;

  char **argv = Proc.get.argv (proc);

  if (NULL is argv or NULL is argv[0]) return retval;

  char *com = argv[0];

  size_t len = bytelen (com);

  ifnot (len) return retval;

  size_t arlen = 0;
  int idx = 0;
  while (argv[idx++] isnot NULL) arlen++;

  if (Cstring.eq (com, "exit")) {
    sh->should_exit = 1;

    if (argv[1] isnot NULL) {
      int r = atoi (argv[1]);
      ifnot (r) {
        if (bytelen (argv[1]) isnot 1 or *argv[1] isnot '0')
          r = 1;
      }

      if (r < 0) r = -r;
      if (r > 127) r = 127;

      sh->exit_val = r;
    } else
      sh->exit_val = 0;

    return SH_EXIT_NOW;
  }

  if (Cstring.eq (com, "pwd")) {
    char *curdir = Dir.current ();
    if (NULL is curdir) {
      fprintf (stderr, "couldn't get current working directory\n");
      retval = 1;
    }  else {
      fprintf (stdout, "%s\n", curdir);
      free (curdir);
      retval = 0;
    }

    return retval;
  }

  if (Cstring.eq (argv[0], "unsetenv")) {
    if (argv[1] is NULL) {
      fprintf (stderr, "unsetenv: awaiting a environment name\n");
      return 1;
    }

    retval = unsetenv (argv[1]);

    if (retval isnot 0) {
      retval = 1;
      fprintf (stderr, "unsetenv: %s\n", Error.errno_string (errno));
    }

    return retval;
  }

  int autochdir = arlen is 1 and Dir.is_directory (com);
  /* note that even infinity ../../.. that goes way behind / it is
   * considered as / (zsh does the same) */

  if (Cstring.eq (com, "cd") or autochdir) {
    char *sp = NULL;
    char *path = autochdir ? com : argv[1];

    if (path is NULL) {
      path = Sys.get.env_value ("HOME");
    } else if (*path is '-') {
      int depth = 0;
      ifnot (*(path + 1))
        depth++;
      else
        depth = atoi (path + 1);

      if (depth is 0 or depth >= $my(cdpath)->num_items)
        return 1;

      vstring_t *it = $my(cdpath)->tail;
      for (int i = 0; i < depth; i++) {
        if (it is NULL) return 1;
        it = it->prev;
      }

      if (it is NULL) return 1;
      ifnot (it->data->num_bytes) return 1;

      sp = it->data->bytes;
      goto change_dir;

    } else
      ifnot (Dir.is_directory (path)) {
        errno = ENOTDIR;
        fprintf (stderr, "cd: %s %s\n", path, Error.errno_string (errno));
        return errno;
      }

    char p[MAXLEN_PATH + 1];
    sp = Path.real (path, p);

    if (sp is NULL) {
      fprintf (stderr, "cd: %s %s\n", path, Error.errno_string (errno));
      return errno;
    }

    change_dir: {}

    char *curdir = Dir.current ();
    if (NULL is curdir) {
      fprintf (stderr, "couldn't get current working directory\n");
      return 1;
    }  else {
      if (Cstring.eq (curdir, sp)) { //$my(cdpath)->tail->data->bytes)) {
        free (curdir);
        return 0;
      }

      free (curdir);
    }

    if (-1 is chdir (sp)) {
      fprintf (stderr, "cd: %s %s\n", sp, Error.errno_string (errno));
      return errno;
    }

    setenv ("PWD", sp, 1);

    Vstring.append_with ($my(cdpath), sp);
    return 0;
  }

  return retval;
}

static int sh_interpret (proc_t *proc) {
  if (NULL is proc) return 1;

  shproc_t *sh = (shproc_t *) Proc.get.user_data (proc);

  int retval = 1;
  sh->exit_val = 0;
  sh->should_exit = 0;
  sh->should_skip_wait = 0;
  sh->skip_next_proc = 0;
  sh_t *this = sh->sh;

  retval = SH_NO_BUILTIN;

  if (sh->type isnot PIPE_TYPE)
    retval = sh_builtins (sh, proc);

  if (retval isnot SH_NO_BUILTIN) {
    if (retval is SH_EXIT_NOW) return OK;

    switch (sh->type) {
      case CONJ_TYPE:
        sh->skip_next_proc = retval isnot 0;
        break;

      case DISJ_TYPE:
        sh->skip_next_proc = retval is 0;
        break;
    }

    return retval;
  }

  switch (sh->type) {
    case COMMAND_TYPE:
      if ($my(is_a_pipe) and Proc.get.next (proc) is NULL)
        Proc.set.read_stream_cb (proc, PROC_READ_STDOUT, sh_read_pipe_cb);

      retval = Proc.exec (proc, NULL);

      if ($my(is_a_pipe) and Proc.get.next (proc) is NULL)
        Proc.set.read_stream_cb (proc, PROC_READ_STDOUT, NULL);

      break;

    case CONJ_TYPE:
      retval = Proc.exec (proc, NULL);
      sh->skip_next_proc = retval isnot 0;
      break;

    case DISJ_TYPE:
      retval = Proc.exec (proc, NULL);
      sh->skip_next_proc = retval is 0;
      break;

    case PIPE_TYPE:
      retval = Proc.open (proc);

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

      sh->should_skip_wait = 1;

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
  $my(exit_val) = 0; $my(is_a_pipe) = 0;
  $my(pipe_output) = NULL;
  $my(cdpath) = Vstring.new ();

  char *cwd = getenv ("PWD");
  ifnot (NULL is cwd)
    Vstring.append_with ($my(cdpath), cwd);

  return this;
}

/* more than a couple of cases are not handled yet:
 *  - quoted arguments
 *  - globbing
 *  - valid redirections
 *  - informative messages
 *  - ...
 */
static int sh_parse (sh_t *this, char *buf) {
  if (NULL is buf) {
    Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "INTERNAL ERROR, got NULL string", SH_MAXLEN_ERROR);
    return NOTOK;
  }

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

  int is_in_dolar_paren = 0;
  int should_set_pipe_cb = 0;

  while (*sp) {
    if (*sp is '#') {
      sp++;
      while (*sp and *sp isnot '\n')
        sp++;
      if (*sp is '\n') sp++;
      continue;
    }

    if (*sp is '$' and *(sp + 1) is '(') {
      sp++;
      should_set_pipe_cb = 1;
      is_in_dolar_paren = 1;
      goto next;
    }

    if (*sp is ')') {
      if (is_in_dolar_paren)
        is_in_dolar_paren = 0;
      else {
        Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "parsing error, illegal token ')'", SH_MAXLEN_ERROR);
        goto theerror;
      }
    }

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
      if (is_in_dolar_paren) {
        sp++;
        continue;
      }

      ifnot (*(sp + 1)) {
        Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "unterminated command, found end of file", SH_MAXLEN_ERROR);
        goto theerror;
      }

      if (*(sp + 1) is '|') {
        ifnot (*(sp + 2)) {
          Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "parsing error, illegal token '|'", SH_MAXLEN_ERROR);
          goto theerror;
        }

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
      ifnot (*(sp + 1)) {
        Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "unterminated command, found end of file", SH_MAXLEN_ERROR);
        goto theerror;
      }

      if (*(sp + 1) isnot '&') {
        Cstring.cp_fmt ($my(error), SH_MAXLEN_ERROR + 1, "parsing error, illegal token '%c', awaiting '&'", *(sp + 1));
        goto theerror;
      }

      ifnot (*(sp + 2)) {
        Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "unterminated command, found end of file", SH_MAXLEN_ERROR);
        goto theerror;
      }

      type = CONJ_TYPE;
      *sp = '\0';
      sp += 2;

      goto add_proc;
    }

    if (*sp is '>') {
      *sp++ = '\0';

      ifnot (*sp) {
        Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "unterminated command, found end of file", SH_MAXLEN_ERROR);
        goto theerror;
      }

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
      ifnot (*sp) {
        Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "unterminated command, found end of file", SH_MAXLEN_ERROR);
        goto theerror;
      }

      goto add_proc;
    }

    goto next;

    add_proc:
      p = Proc.new ();
      sh = Alloc (sizeof (shproc_t));
      sh->sh = this;
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

      Proc.set.user_data (p, sh);

      if (should_set_pipe_cb)
        Proc.set.pipe_cb (p, sh_pipe_cb);

      if (NULL is Proc.parse (p, buf)) {
        char *err = Proc.get.error (p);
        if (*err)
          Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, err, SH_MAXLEN_ERROR);
        goto theerror;
      }

      Proc.set.pipe_cb (p, NULL);

      if (type is PIPE_TYPE) {
        Proc.set.at_fork_cb (p, sh_at_fork_pipeline_cb);
        Proc.set.pre_fork_cb (p, sh_pre_fork_pipeline_cb);
        if (redir_stderr_to_stdout)
         sh->redir_streams |= PROC_READ_STDERR;
      } else {
        Proc.set.pre_fork_cb (p, sh_pre_fork_default_cb);
        Proc.set.at_fork_cb (p, sh_at_fork_default_cb);
      }

      sh_append_proc (this, p);

      buf = sp;
      type = COMMAND_TYPE;
      redir_type = NO_REDIR;
      redir_streams = 0;
      redir_stderr = 0;
      redir_stderr_to_stdout = 0;

      ifnot (*sp)
        goto theend;

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
  if (NULL is this) {
    Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "INTERNAL ERROR, got NULL shell instance", SH_MAXLEN_ERROR);
    return NOTOK;
  }

  $my(error)[0] = '\0';

  if (NOTOK is sh_parse (this, buf))
    return NOTOK;

  $my(saved_stdin) = dup (STDIN_FILENO);

  CUR_PID = -1;
  signal (SIGINT, sh_sigint_handler);

  int retval = OK;

  setenv ("?", "0", 1);
  setenv ("?.to_string", "", 1);

  proc_t *p = $my(head);

  shproc_t *sh = (shproc_t *) Proc.get.user_data (p);
  int is_pipeline = sh->type is PIPE_TYPE;

  while (p) {
    retval = sh_interpret (p);

    char s[32];
    Cstring.itoa ((retval is NOTOK ? 1 : retval), s, 10);
    setenv ("?", s, 1);

    if (retval > 0)
      setenv ("?.to_string", Error.errno_string (retval), 1);
    else if (retval is NOTOK)
      setenv ("?.to_string", "shell internal error", 1);
    else
      setenv ("?.to_string", "", 1);

    sh = (shproc_t *) Proc.get.user_data (p);

    $my(exit_val) = sh->exit_val;
    $my(should_exit) = sh->should_exit;

    if (sh->should_exit or retval is NOTOK)
      break;

    if (sh->skip_next_proc)
      p = Proc.get.next (p);

    if (p isnot NULL)
      p = Proc.get.next (p);

    if (is_pipeline and 0 is sh->should_skip_wait) {
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

  return retval;
}

static int sh_exec_file (sh_t *this, const char *fname) {
  $my(error)[0] = '\0';

  ifnot (File.exists (fname)) {
    Cstring.cp ($my(error), SH_MAXLEN_ERROR + 1, "file does not exist", SH_MAXLEN_ERROR);
    return NOTOK;
  }

  FILE *fp = fopen (fname, "r");
  if (NULL is fp) {
    Cstring.cp_fmt ($my(error), SH_MAXLEN_ERROR + 1, Error.errno_string (errno), SH_MAXLEN_ERROR);
    return NOTOK;
  }

  int retval = OK;

  char *line = NULL;
  string *s = String.new (32);
  size_t len = 0;
  ssize_t nread  = 0;

  while (-1 isnot (nread  = getline (&line, &len, fp))) {
    ifnot (nread) continue;
    if (line[nread - 1] is '\n') nread--;
    while (nread and
          (line[nread - 1] is ' ' or
           line[nread - 1] is '\t'))
      nread--;

    ifnot (nread) continue;

    if (line[nread - 1] is '\\') {
      String.append_with_len (s, line, nread - 1);
      String.append_byte (s, ' ');
      continue;
    }

    String.append_with_len (s, line, nread);
    retval = sh_exec (this, s->bytes);
    sh_release_list (this);
    String.clear (s);

    if ($my(should_exit) or retval is NOTOK)
      break;
  }

  fclose (fp);
  ifnot  (NULL is line)
    free (line);

  String.release (s);

  return retval;
}

static int sh_pipe_cb (proc_t *proc, char *com, string **out) {
  shproc_t *sh = Proc.get.user_data (proc);
  sh_t *old = sh->sh;

  sh_t *this = sh_new ();

  $my(is_a_pipe) = 1;
  $my(pipe_output) = out;

  sh->sh = this;

  int retval = sh_exec (this, com);

  out = $my(pipe_output);

  sh_release (this);

  sh->sh = old;

  return retval;
}

public sh_T __init_sh__ (void) {
  __INIT__ (io);
  __INIT__ (sys);
  __INIT__ (dir);
  __INIT__ (proc);
  __INIT__ (path);
  __INIT__ (file);
  __INIT__ (error);
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (vstring);

  Sys.init_environment (SysEnvOpts());

  return (sh_T) {
    .self = (sh_self) {
      .new = sh_new,
      .exec = sh_exec,
      .exec_file = sh_exec_file,
      .release = sh_release,
      .should_exit = sh_should_exit,
      .release_list = sh_release_list,
      .get = (sh_get_self) {
        .error = sh_get_error,
        .exit_val = sh_get_exit_val
      }
    }
  };
}

public void __deinit_sh__ (sh_T **thisp) {
  __deinit_sys__ ();
  (void) thisp;
  return;
}
