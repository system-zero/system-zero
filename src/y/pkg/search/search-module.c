#define REQUIRE_STDIO
#define REQUIRE_FCNTL
#define REQUIRE_SYS_STAT
#define REQUIRE_UNISTD

#define REQUIRE_STD_MODULE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_RE_TYPE      DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE

#include <z/cenv.h>

MODULE(search);

#define GET_OPT_TOSTDOUT() ({                                               \
  VALUE _v_tostdout = La.get.qualifier (this, "tostdout", INT(0));          \
  ifnot (IS_INT(_v_tostdout))                                               \
    THROW(LA_ERR_TYPE_MISMATCH, "tostdout, awaiting an integer qualifier"); \
  AS_INT(_v_tostdout);                                                      \
})

typedef struct search_file_t {
  Vstring_t *result;
  const char *file;
  re_t *re;
  int with_line_number;
  int with_filename;
  int tostdout;
  struct stat *st;
} search_file_t;

static int __search_file (search_file_t *args) {
  if (args->file is NULL) return NOTOK;
  if (args->re   is NULL) return NOTOK;
  if (args->st   is NULL) return NOTOK;
  ifnot (S_ISREG (args->st->st_mode)) return OK;

  size_t filelen = bytelen (args->file);

  int fd = open (args->file, O_RDONLY);
  if (fd is -1) return NOTOK;

  int retval = OK;
  int c;
  size_t len = MAXLEN_LINE;
  char buf[2];
  char *line = Alloc (sizeof (char) * len);

  size_t num_lines = 0;
  size_t num_read = 0;
  int inloop = 1;

  while (inloop) {
    if (NOTOK is IO.fd.read (fd, buf, 1)) {
      retval = NOTOK;
      goto theend;
    }

    c = *buf;
    if (c is '\0') {
      ifnot (num_read) goto theend;
      inloop = 0;
    }

    if (num_read is len) {
      len += len / 2;
      line = Realloc (line, len);
    }

    if (c isnot '\n' and c isnot '\0') {
      line[num_read++] = c;
      continue;
    }

    line[num_read] = '\0';

    num_lines++;

    int r = Re.exec (args->re, line, num_read);

    if (r >= 0) {
      Re.reset_captures (args->re);
      string *s = String.new (num_read + 1);

      if (args->with_filename) {
        String.append_with_len (s, args->file, filelen);
        String.append_byte (s, ':');
      }

      if (args->with_line_number)
        String.append_with_fmt (s, "%zd:", num_lines);

      String.append_with_len (s, line, num_read);
      if (args->tostdout) {
        fprintf (stdout, "%s\n", s->bytes);
        String.release (s);
      } else {
        vstring_t *vs = Vstring.new_item ();
        vs->data = s;
        Vstring.append (args->result, vs);
      }
    }

    num_read = 0;
    *line = '\0';
  }

theend:
  free (line);
  close (fd);
  return retval;
}

static int process_dir (dirwalk_t *dw, const char *dir, struct stat *st) {
  (void) dw; (void) dir; (void) st;
  return 1;
}

static int process_file (dirwalk_t *dw, const char *file, struct stat *st) {
  search_file_t *args = dw->user_data;
  args->file = file;
  args->st = st;
  __search_file (args);
  return 1;
}

static VALUE search_file (la_t *this, VALUE v_file, VALUE v_pat) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_pat)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *file = AS_STRING_BYTES(v_file);
  char *pat  = AS_STRING_BYTES(v_pat);

  int recursive = GET_OPT_RECURSIVE();
  int without_filename = GET_OPT_WITHOUT_FILENAME();
  int with_line_number = GET_OPT_WITH_LINE_NUMBER();
  int max_depth = GET_OPT_MAX_DEPTH();
  int tostdout = GET_OPT_TOSTDOUT();

  ArrayType *array = NULL;

  search_file_t args = (search_file_t) {
    .tostdout = tostdout,
    .re = Re.new (pat, 0, RE_MAX_NUM_CAPTURES, Re.compile),
    .with_filename = without_filename is 0,
    .with_line_number = with_line_number,
    .st = NULL
  };

  ifnot (tostdout)
    args.result = Vstring.new ();

  VALUE retvalue = NULL_VALUE;

  if (Dir.is_directory (file)) {
    if (0 is recursive and max_depth >= 0) {
      fprintf (stderr, "%s: is a directory and recursive hasn't been set\n", file);
      goto theend;
    }

    if (max_depth < 0) max_depth = DIRWALK_MAX_DEPTH;

    Vstring_t unused;
    dirwalk_t *dw = Dir.walk.new (process_dir, process_file);
    dw->user_data = &args;
    dw->depth = max_depth;
    dw->files = &unused;
    retvalue = INT(Dir.walk.run (dw, file));
    dw->files = NULL;
    Dir.walk.release (&dw);
    goto theend;
  }

  struct stat st;
  if (-1 is lstat (file, &st)) return NULL_VALUE;
  args.st = &st;
  args.file = file;

  retvalue = INT(__search_file (&args));

theend:
  Re.release (args.re);

  ifnot (tostdout) {
    array = VSTRING_TO_ARRAY(args.result);
    Vstring.release (args.result);
    return ARRAY(array);
  }

  return retvalue;
}

public int __init_search_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(io);
  __INIT__(re);
  __INIT__(dir);
  __INIT__(file);
  __INIT__(vstring);

  LaDefCFun lafuns[] = {
    { "search_file", PTR(search_file), 2 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Search = {
      file : search_file
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_search_module__ (la_t *this) {
  (void) this;
}
