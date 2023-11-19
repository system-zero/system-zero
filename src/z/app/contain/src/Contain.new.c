#define APPLICATION "Contain.new"
#define APP_OPTS "directory"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT

#define REQUIRE_ERROR_TYPE       DECLARE
#define REQUIRE_CONTAIN_TYPE     DECLARE
#define REQUIRE_STRING_TYPE      DONOT_DECLARE

#include <z/cenv.h>

int main (int argc, char **argv) {
  __INIT__ (error);
  __INIT__ (contain);

  __INIT_APP__;
  argparse_flags |= ARGPARSE_DONOT_EXIT_ON_UNKNOWN;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  PARSE_ARGS;

  CHECK_ARGC;

  const char *dir = argv[0];

  contain_t *cnt = Contain.new ();

  const char *rootdir;
  if (NULL == (rootdir = Contain.set.rootDir (cnt, (char *) dir)))
    goto theend;

  if (-1 == chdir (rootdir)) {
    fprintf (stderr, "%s: couldn't change directory, %s\n",
       dir, Error.errno_string (errno));
    return 1;
  }

  if (argc > 1)
    Contain.set.argv (cnt, argv + 1);

  retval = Contain.run (cnt);

  if (NOTOK is retval)
    fprintf (stderr, "%s\n", Contain.get.err_msg (cnt));

theend:
  Contain.release (&cnt);

  return (retval < 0 ? 1 : 0);
}
