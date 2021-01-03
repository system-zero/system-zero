#define APPLICATION "Dir.rm"
#define APP_OPTS "directory-name"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT

#define REQUIRE_STRING_TYPE  DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE DONOT_DECLARE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_DIR_TYPE     DECLARE

#include <z/cenv.h>

int main (int argc, char **argv) {
  __INIT__ (dir);
  __INIT_APP__;

  int verbose = 0;
  int parents = 0;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN('v', "verbose", &verbose, "print a message when creating a directory", NULL, 0, 0),
    OPT_BOOLEAN('p', "parents", &parents, "remove directory and its ancestors", NULL, 0, 0),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  PARSE_ARGS;

  DirRm fn = Dir.rm;

  if (parents) fn = Dir.rm_parents;

  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    char dname[MAXLEN_PATH];
    if (NOTOK is Io.fd.read (STDIN_FILENO, dname, MAXLEN_PATH))
      retval = 1;
    else
      retval = fn (dname, DirOpts (.msg = verbose));

    goto theend;
  }

  CHECK_ARGC;

  retval = fn (argv[0], DirOpts (.msg = verbose));

theend:
  return (retval < 0 ? 1 : 0);
}
