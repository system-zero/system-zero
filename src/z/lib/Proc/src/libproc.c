#define LIBRARY "Proc"

//#define REQUIRE_STD_GNU_SOURCE avoid for now (F_[GS]ETPIPE_SZ)

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_SYS_STAT
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_WAIT
#define REQUIRE_FCNTL
#define REQUIRE_GLOB

#define REQUIRE_LIST_MACROS
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_SYS_TYPE     DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_PROC_TYPE    DONOT_DECLARE

#include <z/cenv.h>

typedef struct proc_prop {
  pid_t  pid;

  char
    **envp,
    **argv,
    *stdin_buf;

  char error[PROC_MAXLEN_ERROR + 1];

   int
     argc,
     envc,
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
   ProcPipe_cb pipe_cb;

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

#define COMMENT_CHAR '#'

static int proc_exec (proc_t *, const char *);
static char **proc_parse (proc_t *, const char *);
static int proc_open (proc_t *);

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

  if ($my(argc))
    for (int i = 0; i <= $my(argc); i++)
      free ($my(argv)[i]);

  free ($my(argv));
  $my(argv) = NULL;
}

static void proc_release_env (proc_t *this) {
  if (NULL is $my(envp))
    return;

  for (int i = 0; i < $my(envc); i++)
    free ($my(envp)[i]);

  free ($my(envp));
  $my(envp) = NULL;
}

static void proc_release (proc_t *this) {
  ifnot (this) return;
  proc_release_argv (this);
  proc_release_env (this);

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
    fprintf (stream, "%s", line);

  ifnot (NULL is line) free (line);
  return 0;
}

static void proc_add_env (proc_t *this, const char *env, size_t len) {
  if (NULL is $my(envp))
    $my(envp) = Alloc (sizeof (char *));

  $my(envc)++;
  $my(envp) = Realloc ($my(envp), sizeof (char *) * ($my(envc)));
  $my(envp)[$my(envc) - 1] = Alloc (len + 1);
  Cstring.cp ($my(envp)[$my(envc) - 1], len + 1, env, len);
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
  $my(envc) = 0;
  $my(envp) = NULL;
  $my(is_bg) = 0;
  $my(setsid) = 0;
  $my(setpgid) = 0;
  $my(error)[0] = '\0';

  $my(read_stdout_cb) = proc_output_to_stream;
  $my(read_stderr_cb) = proc_output_to_stream;
  $my(pre_fork_cb) = proc_pre_fork_default_cb;
  $my(at_fork_cb) = proc_at_fork_default_cb;
  $my(pipe_cb) = NULL;
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

static int read_output_cb (proc_t *this, FILE *stream, FILE *fp) {
  (void) stream;
  string *s = (string *) $my(user_data);

  char *line = NULL;
  size_t len = 0;
  ssize_t nread;

  while (-1 isnot (nread = getline (&line, &len, fp)))
    String.append_with_len (s, line, nread);

  String.trim_end (s, '\n');

  ifnot (NULL is line) free (line);
  return OK;
}

static string *read_from_stdout_proc (const char *com) {
  proc_t *proc = proc_new ();
  string *s = String.new (8);
  proc->prop->user_data = s;
  proc->prop->read_stdout = 1;
  proc->prop->read_stdout_cb = read_output_cb;
  if (NULL is proc_parse (proc, com)) goto theerror;
  if (NOTOK is proc_open (proc)) goto theerror;
  proc_read (proc);
  proc_wait (proc);
  proc_release (proc);

  return s;

theerror:
  String.release (s);
  return NULL;
}

static int proc_expand_tilde (string **sa, char **bufp) {
  string *s = *sa;

  int done_expansion = 0;
  char *sp = *bufp;
  if (*sp is '~') {
    char *hdir = Sys.get.env_value ("HOME");
    if (s is NULL)
      s = String.new_with (hdir);
    else
      String.append_with (s, hdir);

    sp++;

    if (*sp and *sp isnot DIR_SEP and *sp isnot ' ')
      String.append_byte (s, DIR_SEP);
    *sa = s;
    done_expansion = 1;
  }

  *bufp = sp;
  return done_expansion;
}

static int proc_expand_dollar_brace (proc_t *this, string **sa, char **bufp) {
  (void) this;
  char *sp = *bufp;

  if (*sp isnot '$') return 0;
  if (*(sp + 1) isnot '{') return 0;

  char *startvar = sp + 1;

  sp += 2;

  if ('?' is *sp) {
    sp++;  startvar++;
    if (Cstring.eq_n (sp, ".to_string}", 11))
      sp += 10;

    goto get_value;
  }

  ifnot (('A' <= *sp and *sp <= 'Z') or
         ('a' <= *sp and *sp <= 'z')) {
    if (*sp isnot '}') {
      Cstring.cp ($my(error), PROC_MAXLEN_ERROR + 1, "unterminated variable expansion, awaiting }", PROC_MAXLEN_ERROR);
      return NOTOK;
    } else {
      *bufp = sp;
      return 1;
    }
  }

  startvar = sp;

  while (*sp) {
    if (*sp is '}')
      break;

    ifnot (('A' <= *sp and *sp <= 'Z') or
           ('a' <= *sp and *sp <= 'z') or
           ('0' <= *sp and *sp <= '9') or
            *sp is '_') {
      Cstring.cp_fmt ($my(error), PROC_MAXLEN_ERROR + 1, "illegal identifier '%c'", *sp);
      return NOTOK;
    }

    sp++;
  }

get_value:
  if (*sp isnot '}') {
    Cstring.cp ($my(error), PROC_MAXLEN_ERROR + 1, "unterminated variable expansion, awaiting }", PROC_MAXLEN_ERROR);
    return NOTOK;
  }

  char *endvar = sp;

  size_t nlen = endvar - startvar;
  char name[nlen + 1];
  Cstring.cp (name, nlen + 1, startvar, nlen);
  char *val = getenv (name);
  ifnot (NULL is val) {
    string *s = *sa;
    if (NULL is s)
      s = String.new_with (val);
    else
      String.append_with (s, val);
    *sa = s;
  }

  *bufp = sp;
  return 1;
}

static int proc_expand_dollar_paren (proc_t *this, string **sa, char **bufp) {
  char *sp = *bufp;

  if (*sp isnot '$') return 0;
  if (*(sp + 1) isnot '(') return 0;

  sp += 2;

  if (*sp is ')') {
    *bufp = sp;
    return 0;
  }

  char *startvar = sp;
  int is_in_str = 0;
  int is_a_pipe = 0;

  while (*sp) {
    if (*sp is ')') {
      ifnot (is_in_str)
        break;

      sp++;
      continue;
    }

    if (*sp is '"') {
      if (is_in_str) {
        if (*(sp - 1) is '\\') {
          sp++;
          continue;
        }
      }

      is_in_str = 1;
      sp++;
      continue;
    }

    if (*sp is '|') {
      ifnot (is_in_str)
        is_a_pipe = 1;
    }

    sp++;
  }

  if (*sp isnot ')') {
    Cstring.cp ($my(error), PROC_MAXLEN_ERROR + 1, "unterminated expression expansion, awaiting )", PROC_MAXLEN_ERROR);
    return NOTOK;
  }

  size_t clen = sp - startvar;
  char com[clen + 1];
  Cstring.cp (com, clen + 1, startvar, clen);

  string *sout = NULL;

  ifnot (is_a_pipe)
    sout = read_from_stdout_proc (com);
  else {
    if ($my(pipe_cb) isnot NULL) {
      if (NOTOK is $my(pipe_cb) (this, com, &sout))
        return NOTOK;
    } else
      return NOTOK;
  }

  if (NULL is sout) return NOTOK;

  string *s = *sa;
  if (NULL is s)
    s = sout;
  else {
    String.append_with_len (s, sout->bytes, sout->num_bytes);
    String.release (sout);
  }

  *sa = s;
  *bufp = sp;
  return 1;
}

static int proc_parse_env (proc_t *this, string **sa, char **bufp) {
  char *sp = *bufp;

  if (*sp isnot '$') return 0;

  sp++;

  if (*sp is '(' or *sp is '{') return 0;

  ifnot (('A' <= *sp and *sp <= 'Z') or ('a' <= *sp and *sp <= 'z')) {
    Cstring.cp_fmt ($my(error), PROC_MAXLEN_ERROR + 1, "illegal identifier '%c'", *sp);
    return NOTOK;
  }

  char *begOfName = sp;

  while (*sp) {
    if (*sp is '=')
      break;

    ifnot (('A' <= *sp and *sp <= 'Z') or
           ('a' <= *sp and *sp <= 'z') or
           ('_' is *sp) or
           ('0' <= *sp and *sp <= '9')) {
      Cstring.cp_fmt ($my(error), PROC_MAXLEN_ERROR + 1, "illegal identifier '%c'", *sp);
      return NOTOK;
    }

    sp++;
  }

  if (*sp isnot '=') {
    Cstring.cp ($my(error), PROC_MAXLEN_ERROR + 1, "unterminated variable declaration, awaiting =", PROC_MAXLEN_ERROR);
    return NOTOK;
  }

  size_t nameLen = sp - begOfName;
  String.append_with_len ((*sa), begOfName, nameLen + 1);
  //String.append_byte ((*sa), '=');

  int is_in_str = 0;

  sp++;

  if (*sp is '"') {
    is_in_str = 1;
    sp++;
  }

  while (1) {
    ifnot (*sp) {
      if (is_in_str) {
        Cstring.cp ($my(error), PROC_MAXLEN_ERROR + 1, "unterminated statement", PROC_MAXLEN_ERROR);
        return NOTOK;
      } else
        break;
    }

    if (*sp is ' ' or *sp is '\t') {
      ifnot (is_in_str)
        break;

      String.append_byte ((*sa), *sp);
      sp++;
      continue;
    }

    if (*sp is '"') {
      if (is_in_str) {
        if (*(sp - 1) isnot '\\')
          break;

        String.append_byte ((*sa), *sp);
        sp++;
        continue;

      } else {
        Cstring.cp ($my(error), PROC_MAXLEN_ERROR + 1, "syntax error, unexpected token '\"'", PROC_MAXLEN_ERROR);
        return NOTOK;
      }
    }

    if (*sp is '$') {
      if (*(sp + 1) is '(') {
        int err = proc_expand_dollar_paren (this, sa, &sp);

        if (NOTOK is err) return NOTOK;
        sp++;
        continue;
      }

      if (*(sp + 1) is '{') {
        int err = proc_expand_dollar_brace (this, sa, &sp);
        if (NOTOK is err) return NOTOK;
        sp++;
        continue;
      }
    }

    if (is_in_str) {
      String.append_byte ((*sa), *sp);
      sp++;
      continue;
    }

    ifnot (('A' <= *sp and *sp <= 'Z') or
           ('a' <= *sp and *sp <= 'z') or
           ('_' is *sp) or
           ('0' <= *sp and *sp <= '9') or
           ('/' is *sp) or
           ('~' is *sp) or
           (':' is *sp)) {
      Cstring.cp_fmt ($my(error), PROC_MAXLEN_ERROR + 1, "illegal identifier '%c'", *sp);
      return NOTOK;
    }

    String.append_byte ((*sa), *sp);
    sp++;
  }

  *bufp = sp;
  return 1;
}

static int proc_append_arg (proc_t *this, char **linep, char *arg, size_t len,
                                                             int just_append) {
  string *s = NULL;

  if (just_append) goto append;

  char *sp = arg;

  if (proc_expand_tilde (&s, &sp)) {
    if (*sp and (*sp isnot ' ' and *sp  isnot '\t' and *sp isnot '\n'))
      String.append_with (s, sp);
    int err = proc_append_arg (this, linep, s->bytes, s->num_bytes, 0);
    String.release (s);
    return err;
  }

  char *end = sp + len;
  if (*end is '"' or *end is '\0') { end--; len--; }
  while (*end is ' ') { end--; len--; }

  s = String.new (len);

  while (sp <= end) {
    if (*sp is '$') {
      int r = proc_expand_dollar_brace (this, &s, &sp);
      if (NOTOK is r) goto theerror;

      if (r) {
        sp++;
        *linep = sp;
        continue;
      }

      r = proc_expand_dollar_paren (this, &s, &sp);

      if (NOTOK is r) goto theerror;

      if (r) {
        sp++;
        *linep = sp;
        continue;
      }

      r = proc_parse_env (this, &s, &sp);

      if (r is NOTOK) goto theerror;
      if (r) {
        proc_add_env (this, s->bytes, s->num_bytes);
        *linep = sp;
        String.release (s);
        return OK;
      }
    }

    String.append_byte (s, *sp++);
  }

  ifnot (s->num_bytes) {
    String.release (s);
    return OK;
  }

  glob_t gl;

  int retval = glob (s->bytes, 0, NULL, &gl);
  ifnot (retval) {
    for (size_t i = 0; i < gl.gl_pathc; i++)
      proc_append_arg (this, linep, gl.gl_pathv[i], bytelen (gl.gl_pathv[i]), 1);

    globfree (&gl);
    String.release (s);

    return OK;

  } else if (retval isnot GLOB_NOMATCH)
    goto theerror;

append:
  $my(argc)++;
  $my(argv) = Realloc ($my(argv), sizeof (char *) * ($my(argc) + 1));

  if (s is NULL) {
    $my(argv)[$my(argc)-1] = Cstring.dup (arg, len);
  } else {
    $my(argv)[$my(argc)-1] = s->bytes;
    free (s);
  }

  $my(argv)[$my(argc)] = (char *) NULL;
  return OK;

theerror:
  String.release (s);
  return NOTOK;
}

static char **proc_parse (proc_t *this, const char *com) {
  if (NULL is com) return NULL;

  size_t len = bytelen (com);
  ifnot (len) return NULL;

  char *sp = (char *) com;

  char *tokbeg;
  $my(argv) = Alloc (sizeof (char *));

  while (*sp) {
    while (*sp and *sp is ' ') sp++;
    if (*sp is COMMENT_CHAR) {
      sp++;
      while (*sp and *sp isnot '\n')
        sp++;
      ifnot (*sp) break;
      sp++;
      continue;
    }

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
      ifnot (*sp) {
        Cstring.cp ($my(error), PROC_MAXLEN_ERROR + 1, "unterminated quoted string", PROC_MAXLEN_ERROR);
        goto theerror;
      }

      if (*(sp - 1) is '\\') goto parse_quoted;
      len = (size_t) (sp - tokbeg);
      sp++;
      goto add_arg;
    }

    if (*sp is '$' and *(sp + 1) is '(') {
      sp += 2;
      while (*sp and *sp isnot ')') sp++;
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

    if (NOTOK is proc_append_arg (this, &sp, tokbeg, len, 0))
      goto theerror;

    ifnot (*sp) break;
    sp++;
  }

  return $my(argv);

theerror:
  proc_release_argv (this);
  proc_release_env (this);
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

/* skip for now untill we need it
static int set_fd_size (int fd) {
  int max = 1 << 30;
  int min = 1 << 12;
  int c = 0;
  int cap = fcntl (fd, F_SETPIPE_SZ, max);
  while (-1 is cap) {
    max >>= c++;
    cap = fcntl (fd, F_SETPIPE_SZ, max);

    if (max < min)
      break;
  }

  return cap;
}
*/

static int proc_open (proc_t *this) {
  if (NULL is this) return NOTOK;

  ifnot ($my(argc)) {
    for (int i = 0; i < $my(envc); i++) {
      /* donot use putenv() here. The strings will be released and
       * the environment would end up with garbages */
      char *sp = Cstring.byte.in_str ($my(envp)[i], '=');
      if (NULL is sp) {
        setenv ($my(envp)[i], "", 1);
      } else {
        *sp = '\0';
        sp++;
        setenv ($my(envp)[i], sp, 1);
      }
    }

    return OK;
  }

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

    for (int i = 0; i < $my(envc); i++)
      putenv ($my(envp)[i]);

    execvp ($my(argv)[0], $my(argv));

    $my(sys_errno) = errno;
    fprintf (stderr, "%s: %s\n", $my(argv)[0], Error.errno_string (errno));
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

static void proc_set_stdin (proc_t *this, const char *buf, size_t size) {
  if (NULL is buf) return;
  $my(dup_stdin) = 1;
  $my(stdin_buf) = Alloc (size + 1);
  $my(stdin_buf_size) = size;
  Cstring.cp ($my(stdin_buf), size + 1, buf, size);
}

static void proc_set_user_data (proc_t *this, void *user_data) {
  $my(user_data) = user_data;
}

static void proc_set_pipe_cb (proc_t *this, ProcPipe_cb fn) {
  $my(pipe_cb) = fn;
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

static void proc_set_argv (proc_t *this, int argc, const char **argv) {
  $my(argv) = Alloc (sizeof (char *) * (argc + 1));
  for (int i = 0; i < argc; i++) {
    size_t len = bytelen (argv[i]);
    $my(argv)[i] = Alloc (len + 1);
    Cstring.cp ($my(argv)[i], len + 1, argv[i], len);
  }

  $my(argc) = argc;
  $my(argv)[$my(argc)] = (char *) NULL;
}

static char **proc_get_argv (proc_t *this) {
  return $my(argv);
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

static char *proc_get_error (proc_t *this) {
  return $my(error);
}

static ProcPipe_cb proc_get_pipe_cb (proc_t *this) {
  if (NULL is this) return NULL;
  return $my(pipe_cb);
}

static int proc_exec (proc_t *this, const char *com) {
  int retval = NOTOK;

  if (NULL is $my(argv))
    if (NULL is proc_parse (this, com))
      goto theend;

  if (NOTOK is proc_open (this)) goto theend;

  ifnot ($my(argc)) return OK;

  proc_read (this);

  ifnot ($my(is_bg))
    retval = proc_wait (this);

theend:
  return retval;
}

public proc_T __init_proc__ (void) {
  __INIT__ (sys);
  __INIT__ (error);
  __INIT__ (string);
  __INIT__ (cstring);

  Sys.init_environment (SysEnvOpts());

  return (proc_T) {
    .self = (proc_self) {
      .new = proc_new,
      .open = proc_open,
      .wait = proc_wait,
      .read = proc_read,
      .exec = proc_exec,
      .parse = proc_parse,
      .release = proc_release,
      .release_argv = proc_release_argv,
      .add_env = proc_add_env,
      .set = (proc_set_self) {
        .argv = proc_set_argv,
        .next = proc_set_next,
        .prev = proc_set_prev,
        .stdin = proc_set_stdin,
        .dup_stdin = proc_set_dup_stdin,
        .user_data = proc_set_user_data,
        .pipe_cb = proc_set_pipe_cb,
        .at_fork_cb = proc_set_at_fork_cb,
        .pre_fork_cb = proc_set_pre_fork_cb,
        .read_stream_cb = proc_set_read_stream_cb
        // .fd_size = set_fd_size
      },
      .unset = (proc_unset_self) {
        .stdin = proc_unset_stdin
      },
      .get = (proc_get_self) {
        .pid = proc_get_pid,
        .argv = proc_get_argv,
        .next = proc_get_next,
        .error = proc_get_error,
        .pipe_cb = proc_get_pipe_cb,
        .user_data = proc_get_user_data
      }
    }
  };
}
