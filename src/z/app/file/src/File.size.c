#define APPLICATION "File.size"
#define APP_OPTS "filename"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STRING_TYPE  DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE DONOT_DECLARE
#define REQUIRE_FILE_TYPE    DECLARE

#include <z/cenv.h>

int main (int argc, char **argv) {
  __INIT_APP__;
  __INIT__ (file);

  size_t size = 0;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  PARSE_ARGS;

  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    char filename[MAXLEN_PATH];
    if (NOTOK is Io.fd.read (STDIN_FILENO, filename, MAXLEN_PATH))
      retval = 1;
    else
      size = File.size (filename);

    goto print;
  }

  CHECK_ARGC;

  size = File.size (argv[0]);

print:
  fprintf (stdout, "%zd\n", size);

  return retval;
}
