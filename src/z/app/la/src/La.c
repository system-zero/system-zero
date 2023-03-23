#define APPLICATION "La"
#define APP_OPTS    "filename"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_TERMIOS

#define REQUIRE_VMAP_TYPE    DECLARE
#define REQUIRE_PATH_TYPE    DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_VSTRING_TYPE DONOT_DECLARE
#define REQUIRE_USTRING_TYPE DONOT_DECLARE
#define REQUIRE_RLINE_TYPE   DECLARE
#define REQUIRE_LA_TYPE      DECLARE

#include <z/cenv.h>

#ifdef STATIC
#include "../../../la-modules/std/std-module.c"
#endif

#ifdef REQUIRE_PATH_MODULE
#include "../../../la-modules/path/path-module.c"
#endif

#ifdef REQUIRE_FILE_MODULE
#include "../../../la-modules/file/file-module.c"
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

#ifdef REQUIRE_SYS_MODULE
#include "../../../la-modules/sys/sys-module.c"
#endif

#ifdef REQUIRE_TIME_MODULE
#include "../../../la-modules/time/time-module.c"
#endif

#ifdef REQUIRE_CRYPT_MODULE
#include "../../../la-modules/crypt/crypt-module.c"
#endif

#ifdef REQUIRE_RAND_MODULE
#include "../../../la-modules/rand/rand-module.c"
#endif

#ifdef REQUIRE_IO_MODULE
#include "../../../la-modules/io/io-module.c"
#endif

#ifdef REQUIRE_URL_MODULE
#include "../../../la-modules/url/url-module.c"
#endif

#ifdef REQUIRE_DL_MODULE
#include "../../../la-modules/dl/dl-module.c"
#endif

#ifdef REQUIRE_MOON_SUN_MODULES
#include "../../../la-modules/sun/sun-module.c"
#include "../../../la-modules/moon/moon-module.c"
#endif

#ifdef REQUIRE_MAN_MODULE
#include "../../../la-modules/man/man-module.c"
#endif

#ifdef REQUIRE_NET_MODULE
#include "../../../la-modules/net/net-module.c"
#endif

const char *keywords[] = {
  "for ", "exit (", "var ", "import (\"", "if (", "ifnot (",
  "include (\"", "print (", "println (", NULL
};

static int la_completion (const char *buf, int curpos, rlineCompletions *lc, void *userdata) {
  rline_t *this = (rline_t *) userdata;

  dirlist_t *dlist = NULL;
  char *dirname = NULL;

  const char *lptr = buf + curpos;
  size_t lptrlen = bytelen (lptr);

  size_t buflen = bytelen (buf) - lptrlen;
  char ptrbuf[buflen + 1];

  int i = 0;
  if (buf[0] is '\0') {
    while (keywords[i]) {
      Rline.add_completion (this, lc, (char *) keywords[i], -1);
      i++;
    }

    return 0;
  }

  string *arg = String.new (buflen);

  char *ptr = (char *) buf + buflen;
  int ptrlen = buflen;

  if (buf[buflen-1] is '"') {
    while (ptrlen--) {
      char c = *(ptr-1);
      if (('0' <= c and c <= '9') or
          ('a' <= c and c <= 'z') or
          ('A' <= c and c <= 'Z') or
          c is '_' or c is DIR_SEP or c is '.') {
        ptr--;
        continue;
      }
      break;
    }

    size_t diff = buflen - ptrlen - 1;
    buflen -= diff;
    ptrlen = diff;
    goto filename_completion;
  }

  int is_filename = 0;
  while (ptrlen--) {
    char c = *(ptr-1);
    if (('0' <= c and c <= '9') or
        ('a' <= c and c <= 'z') or
        ('A' <= c and c <= 'Z') or
        c is '_' or c is '.') {
      ptr--;
      continue;
    }

    if (c is '/') { // quarks so it is a duck
      is_filename = 1;
      ptr--;
      continue;
    }
    break;
  }

  if (is_filename) {
    size_t diff = buflen - ptrlen - 1;
    buflen -= diff;
    ptrlen = diff;
    goto filename_completion;
  }

  if (-1 is ptrlen) {
    while (keywords[i]) {
      if (Cstring.eq_n (keywords[i], buf, buflen))
        Rline.add_completion (this, lc, (char *) keywords[i], -1);
      i++;
    }
    goto theend;
  }

  size_t diff = buflen - ptrlen - 1;
  buflen -= diff;
  ptrlen = diff;

  ifnot (ptrlen)
    goto filename_completion;

  int num = 0;
  String.replace_with_len (arg, buf, buflen);

  i = 0;
  while (keywords[i]) {
    if (Cstring.eq_n (keywords[i], ptr, ptrlen)) {
      String.append_with (arg, keywords[i]);
      Rline.add_completion (this, lc, arg->bytes, -1);
      String.clear_at (arg, buflen);
      num++;
    }

    i++;
  }

  if (num) goto theend;

  filename_completion: {}

  Cstring.cp (ptrbuf, ptrlen + 1, ptr, ptrlen);
  String.replace_with_len (arg, buf, buflen);

  dirname = Path.dirname (ptrbuf);
  char *basename = Path.basename (ptrbuf);

  is_filename = 0;

  if (Dir.is_directory (ptrbuf) or (File.is_lnk (ptrbuf) and Dir.lnk_is_directory (ptrbuf))) {
    if (ptrbuf[ptrlen-1] isnot DIR_SEP) {
      String.append_with_fmt (arg, "%s%c", ptr, DIR_SEP);
      String.append_with_len (arg, lptr, lptrlen);
      Rline.add_completion (this, lc, arg->bytes, arg->num_bytes - lptrlen);
      Rline.set.flags (this, lc, RLINE_ACCEPT_ONE_ITEM);
      goto theend;
    }

    is_filename = 1;
    dlist  = Dir.list (ptrbuf, DIRLIST_LNK_IS_DIRECTORY);
  } else ifnot (ptrlen) {
    get_current: {}
    char *cwd = Dir.current ();
    if (NULL is cwd) return 0;
    dlist = Dir.list (cwd, 0);
    free (cwd);
  } else {
    if (0 is Dir.is_directory (dirname) and (
        0 is File.is_lnk (dirname) and Dir.lnk_is_directory (dirname)))
      goto get_current;

    dlist = Dir.list (dirname, DIRLIST_LNK_IS_DIRECTORY);
  }

  if (NULL is dlist) goto theend;

  vstring_t *it = dlist->list->head;

  size_t dirlen = bytelen (dirname);
  size_t bname_len = bytelen (basename);

  if (1 is dirlen and *dirname is '.' and 0 is bname_len + is_filename) {
    while (it) {
      if (it->data->bytes[0] isnot '.') {
        String.append_with_len (arg, it->data->bytes, it->data->num_bytes);
        String.append_with_len (arg, lptr, lptrlen);
        Rline.add_completion (this, lc, arg->bytes, arg->num_bytes - lptrlen);
        String.clear_at (arg, buflen);
      }
      it = it->next;
    }
  } else {
    while (it) {
      if (is_filename) {
        String.append_with_fmt (arg, "%s%s%s", ptrbuf,
            (ptrbuf[ptrlen-1] is DIR_SEP ? "" : DIR_SEP_STR), it->data->bytes);
        String.append_with_len (arg, lptr, lptrlen);
        Rline.add_completion (this, lc, arg->bytes, arg->num_bytes - lptrlen);
        String.clear_at (arg, buflen);

      } else if (Cstring.eq_n (it->data->bytes, basename, bname_len)) {
        if (dirlen is 1 and *dirname is '.')
          String.append_with_len (arg, it->data->bytes, it->data->num_bytes);
        else
          String.append_with_fmt (arg, "%s%c%s", dirname, DIR_SEP, it->data->bytes);

        String.append_with_len (arg, lptr, lptrlen);
        Rline.add_completion (this, lc, arg->bytes, arg->num_bytes - lptrlen);
        String.clear_at (arg, buflen);
      }

      it = it->next;
    }
  }

theend:
  ifnot (NULL is dlist) dlist->release (dlist);
  ifnot (NULL is dirname) free (dirname);
  String.release (arg);
  return 0;
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

  #ifndef STATIC
    String.append_with (evalbuf,
      "import (\"sh\"); "
      "import (\"os\"); "
      "import (\"io\"); "
      "import (\"dl\"); "
      "import (\"std\"); "
      "import (\"dir\"); "
      "import (\"sys\"); "
      "import (\"url\"); "
      "import (\"sun\"); "
      "import (\"man\"); "
      "import (\"net\"); "
      "import (\"moon\"); "
      "import (\"path\"); "
      "import (\"file\"); "
      "import (\"rand\"); "
      "import (\"term\"); "
      "import (\"time\"); "
      "import (\"crypt\"); "
      "const zs = New Shell ()"
      );
    if (La.eval_string (this, evalbuf->bytes) isnot LA_OK) {
      String.release (evalbuf);
      return LA_NOTOK;
    }
    String.clear (evalbuf);
  #endif

  int should_eval = 0;
  char *line;

  len = 0;

  fprintf (stdout, "language interpreter (%s), type CTRL-d or exit (v) to exit\n", VERSION_STRING);

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
      if (La.get.didExit (this)) {
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

  __INIT__ (dir);
  __INIT__ (path);
  __INIT__ (file);
  __INIT__ (rline);
  __INIT__ (string);
  __INIT__ (cstring);

  __INIT_APP__;

  (void) vmapType;

  argparse_option_t options[] = {
    OPT_END()
  };

  argparse_flags |= ARGPARSE_DONOT_EXIT_ON_UNKNOWN;

  PARSE_ARGS;

  string_t *evalbuf = NULL;
  la_t *la = NULL;

  ifnot (FdReferToATerminal (STDIN_FILENO) + argc) { // we are in a pipe end
    evalbuf = String.new (256); // and there is no script to execute -
    int maxlen = 4095; // though there isn't a certainity, but we do our best for now

    forever {
      char buf[maxlen + 1];
      idx_t nbytes = IO.fd.read (STDIN_FILENO, buf, maxlen);

      if (NOTOK is nbytes) goto theend;

      ifnot (nbytes) break;

      String.append_with_len (evalbuf, buf, nbytes);
    }
  }

  la = La.init_instance (LaN, LaOpts(.argc = argc, .argv = (const char **) argv));

  #ifdef STATIC
    __init_std_module__ (la);
  #endif

  #ifdef REQUIRE_PATH_MODULE
    __INIT__ (vmap);
    __init_path_module__ (la);
  #endif

  #ifdef REQUIRE_FILE_MODULE
    __init_file_module__ (la);
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

  #ifdef REQUIRE_SYS_MODULE
    __init_sys_module__ (la);
  #endif

  #ifdef REQUIRE_TIME_MODULE
    __init_time_module__ (la);
  #endif

  #ifdef REQUIRE_CRYPT_MODULE
    __init_crypt_module__ (la);
  #endif

  #ifdef REQUIRE_RAND_MODULE
    __init_rand_module__ (la);
  #endif

  #ifdef REQUIRE_IO_MODULE
    __init_io_module__ (la);
  #endif

  #ifdef REQUIRE_URL_MODULE
    __init_url_module__ (la);
  #endif

  #ifdef REQUIRE_DL_MODULE
    __init_dl_module__ (la);
  #endif

  #ifdef REQUIRE_MOON_SUN_MODULES
    __init_sun_module__ (la);
    __init_moon_module__ (la);
  #endif

  #ifdef REQUIRE_MAN_MODULE
    __init_man_module__ (la);
  #endif

  #ifdef REQUIRE_NET_MODULE
    __init_net_module__ (la);
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
