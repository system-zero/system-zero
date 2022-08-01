#define REQUIRE_STDIO

#define REQUIRE_ERROR_TYPE    DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE

#define REQUIRE_STD_MODULE

#include <z/cenv.h>
#include<fcpp.h>

MODULE(generateerrno);

#define MAXLEN_ERROR_ARRAY  512
#define MAXLEN_ERROR_INPUT  512
#define MAXLEN_ERROR_OUTPUT 512
#define MAXLEN_ERROR_MSG    1024

typedef struct errorgen_t {
  string *line;
  string *errmsg;
  string *definitions[MAXLEN_ERROR_ARRAY];
  string *strerrors[MAXLEN_ERROR_ARRAY];
  int errors[MAXLEN_ERROR_ARRAY];
  int last_idx;
  char in[MAXLEN_ERROR_INPUT];
  char *ptr;
} errorgen_t;

static char *own_input (char *buffer, int size, void *userdata) {
  errorgen_t *t = (errorgen_t *) userdata;
  char *ptr = t->ptr;
  ifnot (*ptr) return NULL;

  int i = 0;
  while (*ptr and i < size)  buffer[i++] = *ptr++;
  buffer[i] = '\0';

  t->ptr = ptr;
  return buffer;
}

static void own_output (int c, void *userdata) {
  errorgen_t *t = (errorgen_t *) userdata;
  string *line = t->line;

  if (c isnot '\n') {
    String.append_byte (line, c);
    return;
  }

  ifnot (Cstring.eq_n (line->bytes, "#define E", 9))
    goto theend;

  string *s = NULL;

  char *ptr = line->bytes + line->num_bytes - 1;
  while (*(ptr - 1) isnot ' ' and *(ptr - 1) isnot '\t') ptr--;

  int r = 0;
  while ('0' <= *ptr and *ptr <= '9')
    r = 10 * r + (*ptr++ - '0');

  ptr = line->bytes + 8;
  while (*ptr is ' ' or *ptr is '\t') ptr++;

  int idx = r;

  ifnot (r) {
    if (Cstring.eq_n (ptr, "EWOULDBLOCK", sizeof ("EWOULDBLOCK") - 1)) {
      r = EAGAIN;
      s = String.new_with ("EWOULDBLOCK");
      idx = 135;
    } else if (Cstring.eq_n (ptr, "ENOTSUP", sizeof ("ENOTSUP") - 1)) {
      r = EOPNOTSUPP;
      s = String.new_with ("ENOTSUP");
      idx = 136;
    } else if (Cstring.eq_n (ptr, "EDEADLOCK", sizeof ("EDEADLOCK") - 1)) {
      r = EDEADLK;
      s = String.new_with ("EDEADLOCK");
      idx = 137;
    } else
      goto theend;

  } else {
    s = String.new (16);
    while (*ptr and *ptr isnot ' ' and *ptr isnot '\t') String.append_byte (s, *ptr++);
  }

  t->errors[idx] = r;
  t->strerrors[idx] = String.new_with (Error.errno_string (r));
  t->definitions[idx] = s;
  if (idx > t->last_idx) t->last_idx = idx;

theend:
  String.clear (line);
}

static void own_error (void *userdata, char *format, va_list arg) {
  errorgen_t *t = (errorgen_t *) userdata;
  char buf[MAXLEN_ERROR_MSG];
  int n = vsnprintf (buf, MAXLEN_ERROR_MSG, format, arg);
  if (n > 0) String.append_with_len (t->errmsg, buf, n);
}

static VALUE generateerrno (la_t *this) {
  (void) this;
  struct fcppTag tags[FCPP_MAX_TAGS];
  struct fcppTag *tagptr = tags;

  errorgen_t t;

  t.last_idx = 0;

  for (size_t i = 0; i < MAXLEN_ERROR_ARRAY; i++) {
    t.definitions[i] = NULL;
    t.strerrors[i] = NULL;
    t.errors[i] = -1;
  }

  t.line   = String.new (MAXLEN_ERROR_OUTPUT);
  t.errmsg = String.new (128);

  t.definitions[0] = String.new_with ("SUCCESS");
  t.strerrors[0]   = String.new_with ("Success");
  t.errors[0]      = 0;

  const char input[] = "#include <errno.h>\n";
  Cstring.cp (t.in, MAXLEN_ERROR_INPUT, input, sizeof(input));
  t.ptr = t.in;

  tagptr->tag = FCPPTAG_USERDATA;
  tagptr->data = &t;
  tagptr++;

  tagptr->tag = FCPPTAG_INCLUDE_DIR;
  tagptr->data = (void *) "/usr/include/";
  tagptr++;

  tagptr->tag = FCPPTAG_INPUT_NAME;
  tagptr->data = (char *) "own input";
  tagptr++;

  tagptr->tag = FCPPTAG_INPUT;
  tagptr->data =(void *) own_input;
  tagptr++;

  tagptr->tag = FCPPTAG_OUTPUT_DEFINES;
  tagptr->data = (void *) TRUE;
  tagptr++;

  tagptr->tag = FCPPTAG_OUTPUT;
  tagptr->data = (void *) own_output;
  tagptr++;

  tagptr->tag = FCPPTAG_ERROR;
  tagptr->data = (void *) own_error;
  tagptr++;

  tagptr->tag = FCPPTAG_END;
  tagptr->data = 0;
  tagptr++;

  fcppPreProcess (tags);

  String.release (t.line);

  int num = t.last_idx + 1;

  Vmap_t *m = Vmap.new (8);

  ArrayType *definitions = ARRAY_INIT_WITH_LEN(STRING_TYPE, num);
  ArrayType *strerrors   = ARRAY_INIT_WITH_LEN(STRING_TYPE, num);
  ArrayType *errors      = ARRAY_INIT_WITH_LEN(INTEGER_TYPE, num);

  string **def_array = (string **) AS_ARRAY(definitions->value);
  string **str_array = (string **) AS_ARRAY(strerrors->value);
  integer *err_array = (integer *) AS_ARRAY(errors->value);

  for (int i = 0; i < num; i++) {
    ifnot (NULL is t.definitions[i]) {
      def_array[i] = t.definitions[i];
      str_array[i] = t.strerrors[i];
      err_array[i] = t.errors[i];
      continue;
    }

    def_array[i] = String.new_with ("EUNKNOWN");
    str_array[i] = String.new_with ("Unknown error");
    err_array[i] = i;
  }

  La.map.set_value (this, m, "definitions", ARRAY(definitions), 1);
  La.map.set_value (this, m, "strerrors",   ARRAY(strerrors), 1);
  La.map.set_value (this, m, "errors",      ARRAY(errors), 1);
  La.map.set_value (this, m, "errmsg",      STRING(t.errmsg), 1);

  return MAP(m);
}

public int __init_generateerrno_module__ (la_t *this) {
  __INIT_MODULE__(this);

  __INIT__(error);
  __INIT__(string);
  __INIT__(cstring);

  LaDefCFun lafuns[] = {
    { "generateerrno",         PTR(generateerrno), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Generate = {
      errno : generateerrno
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_generateerrno_module__ (la_t *this) {
  (void) this;
}
