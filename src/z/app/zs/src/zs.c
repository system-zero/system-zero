#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include <zc.h>
#include <libcstring.h>
#include <libsh.h>
#include <librline.h>

static  cstring_T CstringT;
#define Cstring   CstringT.self

static  sh_T ShT;
#define Sh   ShT.self

static  rline_T RlineT;
#define Rline   RlineT.self

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
  ShT = __init_sh__ ();
  RlineT = __init_rline__ ();
  CstringT = __init_cstring__ ();

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
