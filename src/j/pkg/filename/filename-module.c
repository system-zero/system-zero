#define REQUIRE_FILE_EXISTS
#define REQUIRE_FILE_IS_WRITABLE
#define REQUIRE_BYTELEN
#define REQUIRE_RENAME
#define REQUIRE_IS_DIRECTORY
#define REQUIRE_PATH_BASENAME
#define REQUIRE_STR_COPY
#define REQUIRE_STR_EQ
#define REQUIRE_STDIO

#define REQUIRE_MODULE_COMPAT
#include <libc.h>

MODULE(filename);

#define GET_OPT_CHAR() ({                                         \
  VALUE _v_char = La.get.qualifier (this, "char", INT('_'));      \
  ifnot (IS_INT(_v_char))                                         \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier"); \
  if ('~' < AS_INT(_v_char) || AS_INT(_v_char) < ' ')             \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a char within the ascii range"); \
  AS_INT(_v_char);                                                \
})

static VALUE filename_strip_nonsense (la_t *this, VALUE v_fname) {
  ifnot (IS_STRING(v_fname)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  int c = GET_OPT_CHAR();
  int verbose = GET_OPT_VERBOSE_WITH(1);
  size_t fnamelen = AS_STRING(v_fname)->num_bytes;

  char *fname = AS_STRING_BYTES(v_fname);

  ifnot (file_exists (fname)) {
    if (verbose)
      sys_fprintf (sys_stderr, "%s: doesn't exists\n", fname);
    return NOTOK_VALUE;
  }

  if (is_directory (fname)) {
    if (verbose)
      sys_fprintf (sys_stderr, "%s: is a directory\n", fname);
    return NOTOK_VALUE;
  }

  ifnot (file_is_writable (fname)) {
    if (verbose)
      sys_fprintf (sys_stderr, "%s: is not writable\n", fname);
    return NOTOK_VALUE;
  }

  char *basename = path_basename (fname);
  if (NULL == basename) {
    if (verbose)
      sys_fprintf (sys_stderr, "%s: can not get the basename\n", fname);
    return NOTOK_VALUE;
  }

  char new[fnamelen + 1];
  char *sp = fname;
  char *spn = new;
  while (*sp && sp!=basename)
    *spn++ = *sp++;

  int chars[10] = {'&', '[', ']', '(', ')', '\'', '"', ',', ';', '|'};

  int prev = 0;

  while (*sp) {
    int found = 0;
    for (int i = 0; i < 9; i++) {
      if (*sp == chars[i]) {
        ifnot (prev == c)
          *spn++ = c;
        prev = c;
        found = 1;
        break;
      }
    }

    ifnot (found) {
      if ((uchar) *sp > '~') {
        ifnot (prev == c) {
          prev = c;
          *spn++ = c;
        }
      } else {
        prev = *sp;
        *spn++ = *sp;
      }
    }

    sp++;
  }

  *spn = '\0';

  if (str_eq (fname, new)) return OK_VALUE;

  int r = sys_rename (fname, new);

  if (verbose) {
    if (-1 == r)
      sys_fprintf (sys_stderr, "%s: %s\n", fname, errno_string (sys_errno));
    else
      sys_fprintf (sys_stderr, "%s -> %s\n", fname, new);
  }

  return INT(r);
}

static VALUE filename_strip_ws (la_t *this, VALUE v_fname) {
  ifnot (IS_STRING(v_fname)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  int c = GET_OPT_CHAR();
  int verbose = GET_OPT_VERBOSE_WITH(1);
  size_t fnamelen = AS_STRING(v_fname)->num_bytes;

  char *fname = AS_STRING_BYTES(v_fname);
  ifnot (file_exists (fname)) {
    if (verbose)
      sys_fprintf (sys_stderr, "%s: doesn't exists\n", fname);
    return NOTOK_VALUE;
  }

  if (is_directory (fname)) {
    if (verbose)
      sys_fprintf (sys_stderr, "%s: is a directory\n", fname);
    return NOTOK_VALUE;
  }

  ifnot (file_is_writable (fname)) {
    if (verbose)
      sys_fprintf (sys_stderr, "%s: is not writable\n", fname);
    return NOTOK_VALUE;
  }

  char *basename = path_basename (fname);
  if (NULL == basename) {
    if (verbose)
      sys_fprintf (sys_stderr, "%s: can not get the basename\n", fname);
    return NOTOK_VALUE;
  }

  char new[fnamelen + 1];
  char *sp = fname;
  char *spn = new;
  while (*sp && sp != basename)
    *spn++ = *sp++;

  int wasnt_ws = 0;
  int prev = 0;

  while (*sp) {
    if (*sp == ' ' || *sp == '\t' || *sp == '\n') {
      if (wasnt_ws && prev != c)
        *spn++ = c;
      wasnt_ws = 0;
      prev = c;
      sp++;
      continue;
    }

    wasnt_ws = 1;
    ifnot (*sp == c && prev == c)
      *spn++ = *sp;

    prev = *sp++;
  }

  *spn = '\0';

  if (str_eq (fname, new)) return OK_VALUE;

  int r = sys_rename (fname, new);
  if (verbose) {
    if (-1 == r)
      sys_fprintf (sys_stderr, "%s: %s\n", fname, errno_string (sys_errno));
    else
      sys_fprintf (sys_stderr, "%s -> %s\n", fname, new);
  }

  return INT(r);
}

public int __init_filename_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    { "filename_strip_ws", PTR(filename_strip_ws), 1 },
    { "filename_strip_nonsense", PTR(filename_strip_nonsense), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK != (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Filename = {
      strip : {
        ws : filename_strip_ws,
        nonsense : filename_strip_nonsense
      }
    }
  );

  err = La.eval_string (this, evalString);
  if (err != LA_OK) return err;
  return LA_OK;
}

public void __deinit_filename_module__ (la_t *this) {
  (void) this;
}
