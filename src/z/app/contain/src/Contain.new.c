#define APPNAME     "Contain.new"
#define APPLICATION "Contain.new"
#define APP_OPTS   "directory"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT

#define REQUIRE_STRING_TYPE      DONOT_DECLARE
#define REQUIRE_CONTAIN_TYPE     DECLARE

#include <z/cenv.h>

int main (int argc, char **argv) {
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

  contain_t *cnt = Contain.new ();

  Contain.set.rootDir (cnt, argv[0]);

  if (argc > 1)
    Contain.set.argv (cnt, argv + 1);

  retval = Contain.run (cnt);
  if (NOTOK is retval)
    fprintf (stderr, "%s\n", Contain.get.err_msg (cnt));

  Contain.release (&cnt);

  return (retval < 0 ? 1 : 0);
}
