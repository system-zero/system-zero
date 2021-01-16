//#define APPNAME     "Term.sane"
#define APPLICATION "Term.sane"
#define APP_OPTS   " "

#define REQUIRE_TERMIOS
#define REQUIRE_STDIO

#define REQUIRE_STRING_TYPE  DONOT_DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_TERM_TYPE    DECLARE

#include <z/cenv.h>

int main (int argc, char **argv) {
  __INIT__ (term);
  __INIT__ (cstring);

  __INIT_APP__;

  int raw_mode = Cstring.eq_n (argv[0], "Term.raw", 8);

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  PARSE_ARGS;

  typedef int (*Term_cb) (term_t *);
  Term_cb fn;

  if (raw_mode)
    fn = Term.raw_mode;
  else
    fn = Term.sane_mode;

  term_t *term = Term.new ();
  retval = fn (term);
  return retval is NOTOK ? 1 : 0;
}
