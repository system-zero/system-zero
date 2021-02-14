// APPLICATION "zs"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_TIME
#define REQUIRE_SYS_STAT
#define REQUIRE_SIGNAL
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_STRING_TYPE  DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_SH_TYPE      DECLARE
#define REQUIRE_SYS_TYPE     DECLARE
#define REQUIRE_RLINE_TYPE   DECLARE

#include <z/cenv.h>

#define DEFAULT_ROOT_PROMPT "\033[31m$\033[m "
#define DEFAULT_USER_PROMPT "\033[32m$\033[m "

#define MAXLEN_DIR     4096
#define MAXLEN_COMMAND 8192

#define ZS_RETURN       -10
#define ZS_CONTINUE     -11
#define ZS_NOTHING_TODO -12

static void zs_completion (const char *buf, rlineCompletions *lc, void *userdata) {
  rline_t *this = (rline_t *) userdata;

  if (buf[0] is '\0') {
    Rline.add_completion (this, lc, "exit");
    Rline.add_completion (this, lc, "cd");
    return;
  }

  if (buf[0] is 'e')
    Rline.add_completion (this, lc, "exit");
}

static char *zs_hints (const char *buf, int *color, int *bold, void *userdata) {
  (void) buf; (void) color; (void) bold; (void) userdata;
  return NULL;
}

static rline_t *zs_init_rline (char *histfile) {
  rline_t *this = Rline.new ();
  Rline.set.completion_cb (this, zs_completion, this);
  Rline.set.hints_cb (this, zs_hints, this); // UNUSED
  Rline.history.set.file (this, histfile);
  Rline.history.load (this);

  ifnot (getuid ())
    Rline.set.prompt (this, DEFAULT_ROOT_PROMPT);
  else
    Rline.set.prompt (this, DEFAULT_USER_PROMPT);

  return this;
}

static int zs_builtins (char *line, Vstring_t *cdpath) {
  Cstring.trim.end (line, ' ');

  if (Cstring.eq (line, "exit")) {
    free (line);
    return ZS_RETURN;
  }

  if (Cstring.eq_n (line, "cd", 2)) {
    char *path = line + 2;
    while (*path is ' ') path++;

    ifnot (*path) {
      path = Sys.get.env_value ("HOME");
    } else  if (Cstring.eq (path, "-")) {
      // handle -1, -2, ...
      if (cdpath->tail->prev isnot NULL)
        path = cdpath->tail->prev->data->bytes;
      else
        return ZS_CONTINUE;
    }

    if (Cstring.eq (path, cdpath->tail->data->bytes))
      return ZS_CONTINUE;

    if (-1 is chdir (path))
      Stderr.print_fmt ("cd: %s %s\n", path, strerror (errno));

    Vstring.append_with (cdpath, path);
    return ZS_CONTINUE;
  }

  return ZS_NOTHING_TODO;
}

static int zs_interactive (sh_t *this) {
  int retval = OK;
  char *line;

  char *cwd = Dir.current ();
  if (NULL is cwd) {
    Stderr.print ("cannot determinate current directory\n");
    return NOTOK;
  }

  Vstring_t *cdpath = Vstring.new ();
  Vstring.append_with (cdpath, cwd);
  free (cwd);
  cwd = NULL;

  size_t len = bytelen (TMPDIR) + sizeof ("/.zs_history");

  char histfile[len + 1];
  snprintf (histfile, len + 1, TMPDIR "/.zs_history");
  rline_t *rline = zs_init_rline (histfile);

  for (;;) {
    errno = 0;
    line = Rline.edit (rline);
    if (line is NULL) {
      if (errno is EAGAIN)
        continue;
       break;
    }

    int builtin = zs_builtins (line, cdpath);
    switch (builtin) {
      case ZS_RETURN:
        goto theend;

      case ZS_CONTINUE:
        goto next;
     }

    signal (SIGINT, SIG_IGN);
    retval = Sh.exec (this, line);
    signal (SIGINT, SIG_DFL);

    next:
    Rline.history.add (rline, line);
    free (line);
    Sh.release_list (this);
  }

theend:
  Rline.history.save (rline);
  Rline.history.release (rline);
  Rline.release (rline);
  Vstring.release (cdpath);

  return retval;
}

int main (int argc, char **argv) {
  __INIT__ (io);
  __INIT__ (sh);
  __INIT__ (sys);
  __INIT__ (dir);
  __INIT__ (rline);
  __INIT__ (vstring);
  __INIT__ (cstring);

  Sys.init_environment (SysEnvOpts());

  char dir[MAXLEN_DIR]; dir[0] = '\0';
  char command[MAXLEN_COMMAND]; command[0] = '\0';

  argc--; argv++;

  int nargc = 0;

  for (int i = 0; i < argc; i++) {
    if (Cstring.eq_n (argv[i], "--chdir=", 8)) {
      Cstring.cp (dir, MAXLEN_DIR, argv[i] + 8, MAXLEN_DIR);
      continue;
    }

    nargc++;

    if (1 isnot nargc)
      Cstring.cat (command, MAXLEN_COMMAND, " ");

    Cstring.cat (command, MAXLEN_COMMAND, argv[i]);
  }

  if (dir[0]) chdir (dir);

  sh_t *this = Sh.new ();
  int retval = 0;

  ifnot (nargc) {
    retval = zs_interactive (this);
    goto theend;
  }

  retval = Sh.exec (this, command);

theend:
  __deinit_sys__ ();

  Sh.release (this);
  return retval;
}
