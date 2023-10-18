#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SIGNAL
#define REQUIRE_SYS_SELECT
#define REQUIRE_SYS_WAIT
#define REQUIRE_STDARG

#define REQUIRE_STD_MODULE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_SH_TYPE      DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_PROC_TYPE    DECLARE

#include <z/cenv.h>

MODULE(sh);

#ifndef BUFSZ
#define BUFSZ 4096
#endif

#define PIPE_READ_END  0
#define PIPE_WRITE_END 1

#define IS_SH(__v__)({ int _r_ = 0; \
  if (IS_OBJECT(__v__)) { object *_o_ = AS_OBJECT(__v__); _r_ = Cstring.eq (_o_->name, "ShType");}\
  _r_; })

#define AS_SH(__v__)\
({object *_o_ = AS_OBJECT(__v__); sh_t *_s_ = (sh_t *) AS_OBJECT (_o_->value); _s_;})

static VALUE sh_exec (la_t *this, VALUE v_sh, VALUE v_command) {
  (void) this;
  ifnot (IS_SH(v_sh)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a sh object");
  ifnot (IS_STRING(v_command)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  sh_t *sh = AS_SH(v_sh);

  char *command = AS_STRING_BYTES(v_command);
  Sh.release_list (sh);
  return INT(Sh.exec (sh, command));
}

static void close_pipe (int num, ...) {
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

static VALUE sh_process (la_t *this, VALUE v_opts) {
  (void) this;
  ifnot (IS_MAP(v_opts)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");

  Vmap_t *retval = Vmap.new (4);
  La.map.set_value (this, retval, "retval",        NOTOK_VALUE, 1);
  La.map.set_value (this, retval, "stdout_string", NULL_VALUE, 1);
  La.map.set_value (this, retval, "stderr_string", NULL_VALUE, 1);

  Vmap_t *opts = AS_MAP (v_opts);

  ifnot (Vmap.key_exists (opts, "argv")) {
    fprintf (stderr, "%s, awaiting an argv option\n", __func__);
    return MAP(retval);
  }

  VALUE *vargv = Vmap.get (opts, "argv");
  ArrayType *array = (ArrayType *) AS_ARRAY((*vargv));

  if (array->type isnot STRING_TYPE)
    THROW(LA_ERR_TYPE_MISMATCH, "argv: awaiting a string type array");

  string **ar = (string **) AS_ARRAY(array->value);

  char **argv = Alloc (sizeof (char *) * (array->len + 1));
  for (size_t i = 0; i < array->len; i++)
    argv[i] = Cstring.dup (ar[i]->bytes, ar[i]->num_bytes);

  argv[array->len] = NULL;

  char buf[BUFSZ];
  int pid = -1;
  int status = 0;

  int set_sid = 0;
  int set_pgid = 0;

  char *redir_stdout = NULL;
  const char *redir_stdout_mode = "w";
  FILE *stdout_fp = NULL;
  string *stdout_string = NULL;
  int read_stdout = 0;
  int print_stdout = 0;
  int stdout_fd = -1;
  int stdout_fds[2];

  char *redir_stderr = NULL;
  const char *redir_stderr_mode = "w";
  FILE *stderr_fp = NULL;
  string *stderr_string = NULL;
  int read_stderr = 0;
  int print_stderr = 0;
  int stderr_fd = -1;
  int stderr_fds[2];

  if (Vmap.key_exists (opts, "redir_stdout")) {
    VALUE *vredir_stdout = Vmap.get (opts, "redir_stdout");
    ifnot (IS_NULL ((*vredir_stdout))) {
      redir_stdout = AS_STRING_BYTES((*vredir_stdout));
      stdout_fp = fopen (redir_stdout, redir_stdout_mode);
      if (NULL is stdout_fp) {
        fprintf (stderr, "%s: %s\n", redir_stdout, Error.errno_string (errno));
        goto theend;
      }
    }
  }

  if (Vmap.key_exists (opts, "redir_stdout_mode")) {
    VALUE *vredir_stdout_mode = Vmap.get (opts, "redir_stdout_mode");
    ifnot (IS_NULL ((*vredir_stdout_mode)))
      redir_stdout_mode = AS_STRING_BYTES((*vredir_stdout_mode));
  }

  if (Vmap.key_exists (opts, "read_stdout")) {
    VALUE *vread_stdout = Vmap.get (opts, "read_stdout");
    read_stdout = AS_INT((*vread_stdout));
    if (read_stdout) {
      stdout_string = String.new (8);
      La.map.set_value (this, retval, "stdout_string", STRING(stdout_string), 1);
    }
  }

  if (Vmap.key_exists (opts, "print_stdout")) {
    VALUE *vprint_stdout = Vmap.get (opts, "print_stdout");
    print_stdout = AS_INT((*vprint_stdout));
  }

  if (Vmap.key_exists (opts, "redir_stderr")) {
    VALUE *vredir_stderr = Vmap.get (opts, "redir_stderr");
    ifnot (IS_NULL ((*vredir_stderr))) {
      redir_stderr = AS_STRING_BYTES((*vredir_stderr));
      if (NULL != redir_stdout && Cstring.eq (redir_stdout, redir_stderr)) {
        stderr_fp = stdout_fp;
      } else {
        stderr_fp = fopen (redir_stderr, redir_stderr_mode);
        if (NULL is stderr_fp) {
          fprintf (stderr, "%s: %s\n", redir_stderr, Error.errno_string (errno));
          goto theend;
        }
      }
    }
  }

  if (Vmap.key_exists (opts, "redir_stderr_mode")) {
    VALUE *vredir_stderr_mode = Vmap.get (opts, "redir_stderr_mode");
    ifnot (IS_NULL ((*vredir_stderr_mode)))
      redir_stderr_mode = AS_STRING_BYTES((*vredir_stderr_mode));
  }

  if (Vmap.key_exists (opts, "read_stderr")) {
    VALUE *vread_stderr = Vmap.get (opts, "read_stderr");
    read_stderr = AS_INT((*vread_stderr));
    if (read_stderr) {
      stderr_string = String.new (8);
      La.map.set_value (this, retval, "stderr_string", STRING(stderr_string), 1);
    }
  }

  if (Vmap.key_exists (opts, "print_stderr")) {
    VALUE *vprint_stderr = Vmap.get (opts, "print_stderr");
    print_stderr = AS_INT((*vprint_stderr));
  }

  if (redir_stdout != NULL || read_stdout)
    if (-1 is pipe (stdout_fds))
      goto theend;

  if (redir_stderr != NULL || read_stderr) {
    if (-1 is pipe (stderr_fds)) {
      close_pipe (1, redir_stdout, stdout_fds);
      goto theend;
    }
  }

  if (-1 == (pid = fork ())) {
    close_pipe (1,
      redir_stdout, stdout_fds,
      redir_stderr, stderr_fds);
    goto theend;
  }

  char **envp = NULL;
  int envc = 0;

  ifnot (pid) {
    if (set_sid)
      setsid ();
    if (set_pgid)
      setpgid (0, 0);

    if (redir_stdout != NULL || read_stdout) {
      close (stdout_fds[PIPE_READ_END]);
      dup2 (stdout_fds[PIPE_WRITE_END], fileno (stdout));
    }

    if (redir_stderr != NULL || read_stderr) {
      close (stderr_fds[PIPE_READ_END]);
      dup2 (stderr_fds[PIPE_WRITE_END], fileno (stderr));
    }

    for (int i = 0; i < envc; i++)
      putenv (envp[i]);

    execvp (argv[0], argv);

    fprintf (stderr, "%s: %s\n", argv[0], Error.errno_string (errno));
    _exit (1);
  }

  if (redir_stdout != NULL || read_stdout) close (stdout_fds[PIPE_WRITE_END]);
  if (redir_stderr != NULL || read_stderr) close (stderr_fds[PIPE_WRITE_END]);

  if (redir_stdout == NULL && 0 == read_stdout && redir_stderr == NULL && 0 == read_stderr)
    goto theend;

  int maxfd = 1;

  if (redir_stdout != NULL || read_stdout) {
    stdout_fd = stdout_fds[PIPE_READ_END];
    if (maxfd <= stdout_fd)
      maxfd = stdout_fd + 1;
  }

  if (redir_stderr != NULL || read_stderr) {
    stderr_fd = stderr_fds[PIPE_READ_END];
    if (maxfd <= stderr_fd)
      maxfd = stderr_fd + 1;
  }

  int total_read;
  int numread;

  fd_set read_mask;
  struct timeval *tv = NULL;

  for (;;) {
    FD_ZERO (&read_mask);
    if (redir_stdout != NULL || read_stdout)
      FD_SET (stdout_fd, &read_mask);

    if (redir_stderr != NULL || read_stderr)
      FD_SET (stderr_fd, &read_mask);

    if (0 >= select (maxfd, &read_mask, NULL, NULL, tv)) {
      switch (errno) {
        case EIO:
        case EINTR:
          continue;
        default:
          goto theend;
      }
    }

    total_read = 0;

    if ((redir_stdout != NULL || read_stdout) && FD_ISSET (stdout_fd, &read_mask)) {
      buf[0] = '\0';
      if (0 > (numread = read (stdout_fd, buf, BUFSZ - 1))) {
        switch (errno) {
          case EIO:
            break;
          default:
            goto theend;
        }
      }

      total_read += numread;

      if (print_stdout) {
        if (read_stdout) {
          for (int i = 0; i < numread; i++) {
            String.append_byte (stdout_string, buf[i]);
            fprintf (stdout, "%c", buf[i]);
          }
        } else {
          for (int i = 0; i < numread; i++) {
            fprintf (stdout_fp, "%c", buf[i]);
            fprintf (stdout, "%c", buf[i]);
          }
        }
      } else {
        for (int i = 0; i < numread; i++)
          fprintf (stdout_fp, "%c", buf[i]);
      }
    }

    if ((redir_stderr != NULL || read_stderr) && FD_ISSET (stderr_fd, &read_mask)) {
      buf[0] = '\0';
      if (0 > (numread = read (stderr_fd, buf, BUFSZ - 1))) {
        switch (errno) {
          case EIO:
            break;
          default:
            goto theend;
        }
      }

      total_read += numread;

      if (print_stderr) {
        if (read_stderr) {
          for (int i = 0; i < numread; i++) {
            String.append_byte (stderr_string, buf[i]);
            fprintf (stderr, "%c", buf[i]);
          }
        } else {
          for (int i = 0; i < numread; i++) {
            fprintf (stderr_fp, "%c", buf[i]);
            fprintf (stderr, "%c", buf[i]);
          }
        }
      } else {
        for (int i = 0; i < numread; i++)
          fprintf (stderr_fp, "%c", buf[i]);
      }
    }

    ifnot (total_read) break;
  }


theend:
  if (-1 isnot pid) {
    waitpid (pid, &status,  0);

    int r = 0;
    if (WIFEXITED (status))
      r = WEXITSTATUS (status);

    La.map.set_value (this, retval, "retval", INT(r), 1);
  }

  for (size_t i = 0; i < array->len; i++)
    free (argv[i]);
  free (argv);

  if (stdout_fp != NULL)
    fclose (stdout_fp);

  if (stderr_fp != NULL && 0 == Cstring.eq (redir_stdout, redir_stderr))
    fclose (stderr_fp);

  return MAP(retval);
}

static VALUE sh_release (la_t *this, VALUE v_sh) {
  (void) this;
  ifnot (IS_SH(v_sh)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a sh object");
  sh_t *sh = AS_SH(v_sh);
  Sh.release (sh);
  return OK_VALUE;
}

static VALUE sh_new (la_t *this) {
  (void) this;

  sh_t *sh = Sh.new ();
  VALUE v = OBJECT(sh);
  object *o = La.object.new (sh_release, NULL, "ShType", v);
  return OBJECT(o);
}

public int __init_sh_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(sh);
  __INIT__(vmap);
  __INIT__(cstring);
  __INIT__(error);
  __INIT__(proc);

  LaDefCFun lafuns[] = {
    { "sh_new",             PTR(sh_new), 0 },
    { "sh_exec",            PTR(sh_exec), 2 },
    { "sh_process",         PTR(sh_process), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Sh = {};

    Type Shell {
      private
      instance,

      init : func {
        this.instance = sh_new ()
      },

      public
      exec : func (command) {
        return sh_exec (this.instance, command; qualifiers ())
      },
      process : func (opts) {
        return sh_process (opts; qualifiers ())
      }
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_sh_module__ (la_t *this) {
  (void) this;
  sh_T *shp = &shType;
  __deinit_sh__ (&shp);
  return;
}
