#define APPLICATION "env"
#define APP_OPTS   "[opts] [prog]"

#define REQUIRE_STDIO

#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_STRING_TYPE  DONOT_DECLARE
#include <z/cenv.h>

extern char **environ;

int main (int argc, char **argv) {
  __INIT__ (cstring);
  __INIT_APP__;

  char *unset = NULL;
  int ignore = 0;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN('i',  NULL, &ignore, "ignore environment", NULL, 0, 0),
    OPT_STRING('u', "unset", &unset, "remove variable from the environment", NULL, 0, 0),
    OPT_END()
  };

  argparse_flags |= ARGPARSE_DONOT_EXIT_ON_UNKNOWN;

  PARSE_ARGS;

  if (unset)
    unsetenv (unset);

  int idx = 0;
  for (; idx < argc; idx++) {
    if (Cstring.byte.in_str (argv[idx], '='))
      putenv (argv[idx]);
    else
      break;
  }

  if (idx isnot argc)
    execvp (argv[idx], argv + idx);

  ifnot (ignore)
    for (; *environ; ++environ)
      fprintf (stdout, "%s\n", *environ);

  exit (retval);
}
