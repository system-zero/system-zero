#define APPLICATION "La"
#define APP_OPTS    "script"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD

#define REQUIRE_VMAP_TYPE    DECLARE
#define REQUIRE_PATH_TYPE    DONOT_DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE DONOT_DECLARE
#define REQUIRE_USTRING_TYPE DONOT_DECLARE
#define REQUIRE_LA_TYPE      DECLARE

#include <z/cenv.h>

#ifdef REQUIRE_PATH_MODULE
#include "../../../la-modules/path/path-module.c"
#endif

#ifdef REQUIRE_FILE_MODULE
#include "../../../la-modules/file/file-module.c"
#endif

#ifdef REQUIRE_STRING_MODULE
#include "../../../la-modules/string/string-module.c"
#endif

#ifdef REQUIRE_STD_MODULE
#include "../../../la-modules/std/std-module.c"
#endif

#ifdef REQUIRE_TERM_MODULE
#include "../../../la-modules/term/term-module.c"
#endif

#ifdef REQUIRE_DIR_MODULE
#include "../../../la-modules/dir/dir-module.c"
#endif

int main (int argc, char **argv) {
  la_T *LaN = __init_la__ ();
  __LA__ = *LaN;

  __INIT__ (string);
  __INIT__ (file);

  __INIT_APP__;

  argparse_option_t options[] = {
   // OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  argparse_flags |= ARGPARSE_DONOT_EXIT_ON_UNKNOWN;

  PARSE_ARGS;

  string_t *evalbuf = NULL;
  la_t *la = NULL;

  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    evalbuf = String.new (256);

    int maxlen = 4095;
    forever {
      char buf[maxlen + 1];
      idx_t nbytes = IO.fd.read (STDIN_FILENO, buf, maxlen);
      if (NOTOK is nbytes)
        goto theend;

      ifnot (nbytes) goto eval;

      String.append_with_len (evalbuf, buf, nbytes);
    }
  }

  CHECK_ARGC;

eval:
  la = La.init_instance (LaN, LaOpts(.argc = argc, .argv = argv));

  #ifdef REQUIRE_PATH_MODULE
    __INIT__ (vmap);
    __init_path_module__ (la);
  #endif

  #ifdef REQUIRE_FILE_MODULE
    __init_file_module__ (la);
  #endif

  #ifdef REQUIRE_STRING_MODULE
    __init_string_module__ (la);
  #endif

  #ifdef REQUIRE_STD_MODULE
    __init_std_module__ (la);
  #endif

  #ifdef REQUIRE_TERM_MODULE
    __init_term_module__ (la);
  #endif

  #ifdef REQUIRE_DIR_MODULE
    __init_dir_module__ (la);
  #endif

  if (NULL is evalbuf) {
    ifnot (argc) goto theend;
    retval = La.load_file (LaN, la, argv[0]);
  } else
    retval = La.eval_string (la, evalbuf->bytes);

theend:
  __deinit_la__ (&LaN);
  String.release (evalbuf);
  return retval < 0 ? 1 : retval;
}
