#define APPNAME     "Dir.make"
#define APPLICATION "Dir.make"
#define APP_OPTS   "directory-name"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT

#define REQUIRE_STRING_TYPE  DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE DONOT_DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_DIR_TYPE     DECLARE

#include <z/cenv.h>

static int dir_make_print (FILE *fp, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len+1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  bytes[len-1] = '\0';

  char *dir = Cstring.byte.in_str (bytes, ':') + 2;
  struct stat st;

  if (-1 is stat (dir, &st)) {
    Stderr.print_fmt ("stat: %s, %s\n", dir, Error.errno_string (errno));
    return OK;
  }

  char mode_string[16];
  File.mode.stat_to_string (mode_string, st.st_mode);
  char mode_oct[8]; snprintf (mode_oct, 8, "%o", st.st_mode);

  fprintf (fp, "%s, with mode %s (%s)\n",
      bytes, mode_oct + 1, mode_string);

  return OK;
}

int main (int argc, char **argv) {
  __INIT__ (error);
  __INIT__ (dir);
  __INIT__ (file);
  __INIT__ (cstring);

  __INIT_APP__;

  int parents = 0;
  int verbose = 0;
  char *mode_string = NULL;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_STRING('m', "mode", &mode_string, "set file mode in octal", NULL, 0, 0),
    OPT_BOOLEAN('p', "parents", &parents, "make parents directories as needed", NULL, 0, 0),
    OPT_BOOLEAN('v', "verbose", &verbose, "print a message when creating a directory", NULL, 0, 0),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  PARSE_ARGS;

  /* dispatch */
  DirMake fn = Dir.make;

  if (parents)
    fn = Dir.make_parents;

  mode_t mask = umask (0);
  mode_t mode = 0777 & ~mask;
  umask (mask);

  ifnot (NULL is mode_string) {
    mode_t m = File.mode.from_octal_string (mode_string);

    ifnot (m) {
      Stderr.print_fmt (APPNAME ": not a valid mode %s\n", mode_string);
      return 1;
    }

    mode = (mode & 0) | m;
  }

  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    char dname[MAXLEN_PATH];
    if (NOTOK is IO.fd.read (STDIN_FILENO, dname, MAXLEN_PATH))
      retval = 1;
    else
      retval = fn (dname, mode, DirOpts (
        .msg = verbose,
        .msg_cb = dir_make_print
      ));

    goto theend;
  }

  CHECK_ARGC;

  retval = fn (argv[0], mode, DirOpts (
    .msg = verbose,
    .msg_cb = dir_make_print));

theend:
  return (retval < 0 ? 1 : 0);
}
