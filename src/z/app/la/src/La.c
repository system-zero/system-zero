#define APPLICATION "La"
#define APP_OPTS    "filename"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_TERMIOS

#define REQUIRE_VMAP_TYPE    DECLARE
#define REQUIRE_PATH_TYPE    DONOT_DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE DONOT_DECLARE
#define REQUIRE_USTRING_TYPE DONOT_DECLARE
#define REQUIRE_RLINE_TYPE   DECLARE
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

#ifdef REQUIRE_SH_MODULE
#include "../../../la-modules/sh/sh-module.c"
#endif

#ifdef REQUIRE_OS_MODULE
#include "../../../la-modules/os/os-module.c"
#endif

static void la_completion (const char *buf, rlineCompletions *lc, void *userdata) {
  rline_t *this = (rline_t *) userdata;

  if (buf[0] is '\0') {
    Rline.add_completion (this, lc, "exit (");
    return;
  }

  if (buf[0] is 'e')
    Rline.add_completion (this, lc, "exit (");
}

static char *la_hints (const char *buf, int *color, int *bold, void *userdata) {
  (void) buf; (void) color; (void) bold; (void) userdata;
  return NULL;
}

static rline_t *la_init_rline (char *histfile) {
  rline_t *this = Rline.new ();
  Rline.set.completion_cb (this, la_completion, this);
  Rline.set.hints_cb (this, la_hints, this); // UNUSED
  Rline.history.set.file (this, histfile);
  Rline.history.load (this);
  return this;
}

static int parse_line (string *evalbuf, char *line, int *in_string,
    int *block_count, int *expr_count) {

  size_t len = bytelen (line);
  int c = 0;
  int prev_c;

  for (size_t i = 0; i < len; i++) {
    prev_c = c;
    c = line[i];

    if (c is ';') {
      String.append_byte (evalbuf, c);

      if (0 is *expr_count and 0 is *in_string and  0 is *block_count and
          i is len - 1) {
        return 1;
      }
      continue;
    }

    if (c is '"') {
      if (prev_c isnot '\\') {
        if (*in_string)
          (*in_string)--;
        else
          (*in_string)++;
      }

      String.append_byte (evalbuf, c);
      continue;
    }

    if (c is '{') {
      ifnot (*in_string)
        (*block_count) += 1;

      String.append_byte (evalbuf, c);
      continue;
    }

    if (c is '}') {
      ifnot (*in_string)
        (*block_count) -= 1;

      String.append_byte (evalbuf, c);
      continue;
    }

    if (c is '(') {
      ifnot (*in_string)
        (*expr_count)++;

      String.append_byte (evalbuf, c);
      continue;
    }

    if (c is ')') {
      ifnot (*in_string)
        (*expr_count)--;

      String.append_byte (evalbuf, c);
      continue;
    }

    if (c >= ' ')
      String.append_byte (evalbuf, c);
  }

  String.append_byte (evalbuf, '\n');

  return 0;
}

static int la_interactive (la_t *this) {
  size_t len = bytelen (DATADIR "/.lai_history");

  char histfile[len + 1];
  snprintf (histfile, len + 1, DATADIR "/.lai_history");
  rline_t *rline = la_init_rline (histfile);

  int in_string = 0;
  int block_count = 0;
  int expr_count = 0;

  int retval = LA_OK;

  string *evalbuf = String.new (256);

  int should_eval = 0;
  char *line;

  len = 0;

  fprintf (stdout, "language interpreter (%s), type CTRL-d to exit\n", VERSION_STRING);

  Rline.set.prompt (rline, "");

  for (;;) {
    errno = 0;
    line = Rline.edit (rline);
    if (line is NULL) {
      if (errno is EAGAIN)
        continue;
       break;
    }

    should_eval = parse_line (evalbuf, line, &in_string, &block_count, &expr_count);

    if (should_eval) {
      retval = La.eval_string (this, evalbuf->bytes + len);
      len = evalbuf->num_bytes;
      if (retval < LA_OK or La.get.didExit (this)) {
        Rline.history.add (rline, line);
        free (line);
        break;
      }

      Rline.set.prompt (rline, "");
    } else if (block_count or expr_count)
      Rline.set.prompt (rline, "  ");

    Rline.history.add (rline, line);
    free (line);
  }

  String.release (evalbuf);
  Rline.history.save (rline);
  Rline.history.release (rline);
  Rline.release (rline);
  return retval;
}

int main (int argc, char **argv) {
  la_T *LaN = __init_la__ ();
  __LA__ = *LaN;

  __INIT__ (string);
  __INIT__ (file);
  __INIT__ (rline);

  __INIT_APP__;

  (void) vmapType;

  argparse_option_t options[] = {
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

  #ifdef REQUIRE_SH_MODULE
    __init_sh_module__ (la);
  #endif

  #ifdef REQUIRE_OS_MODULE
    __init_os_module__ (la);
  #endif

  if (NULL is evalbuf) {
    ifnot (argc)
      retval = la_interactive (la);
    else
      retval = La.load_file (LaN, la, argv[0]);
  } else
    retval = La.eval_string (la, evalbuf->bytes);

theend:
  __deinit_la__ (&LaN);
  String.release (evalbuf);
  return retval < 0 ? 1 : retval;
}
