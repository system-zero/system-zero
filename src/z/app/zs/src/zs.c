// APPLICATION "zs"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_TIME
#define REQUIRE_SIGNAL
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_SH_TYPE      DECLARE
#define REQUIRE_RLINE_TYPE   DECLARE

#include <z/cenv.h>

#define DEFAULT_ROOT_PROMPT "\033[31m$\033[m "
#define DEFAULT_USER_PROMPT "\033[32m$\033[m "

#define MAXLEN_DIR     4096
#define MAXLEN_COMMAND 8192

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

static int zs_interactive (sh_t *this) {
  int retval = OK;
  char *line;

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

    if (Cstring.eq (line, "exit")) {
      free (line);
      break;
    }

    if (Cstring.eq_n (line, "cd ", 3)) {
      char *path = line + 3;
      if (-1 is chdir (path))
        fprintf (stderr, "cd: %s\n", strerror (errno));
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

  Rline.history.save (rline);
  Rline.history.release (rline);
  Rline.release (rline);

  return retval;
}

int main (int argc, char **argv) {
  __INIT__ (sh);
  __INIT__ (rline);
  __INIT__ (cstring);

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
  Sh.release (this);
  return retval;
}
