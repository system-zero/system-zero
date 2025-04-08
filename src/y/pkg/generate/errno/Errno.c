#define _POSIX_C_SOURCE 200809L  // lstat
#include <stdio.h>
#include <stdlib.h>   // malloc
#include <string.h>   // strerror, strlen
#include <unistd.h>   // access, *_OK
#include <stdarg.h>
#include <sys/stat.h> // lstat
#include <errno.h>

#include <fcpp.h>

typedef struct string {
    char *bytes;
  size_t  num_bytes;
  size_t  mem_size;
} string;

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

#define ifnot(__expr__) if (0 == (__expr__))
#define tostderr(_fmt_, ...) fprintf (stderr, _fmt_, ##__VA_ARGS__)
#define tostdout(_fmt_, ...) fprintf (stdout, _fmt_, ##__VA_ARGS__)
#define DIR_SEP  '/'

static int exists (const char *f) {
  struct stat st;
  if (-1 == lstat (f, &st))
    return 0;

  return 1;
}

static int is_directory (const char *d) {
  struct stat st;
  if (-1 == lstat (d, &st))
    return 0;

  return S_ISDIR (st.st_mode);
}

static int is_writable (const char *dir) {
  return (0 == access (dir, W_OK));
}

static char *path_concat (const char *a, size_t a_sz, const char *b, size_t b_sz) {
  char *spb = (char *) b;
  while (*spb == DIR_SEP) {
    spb++;
    b_sz--;
  }

  if (b_sz) {
    char *sp = (char *) a + (a_sz - 1);
    while (*sp == DIR_SEP) {
      sp--;
      a_sz--;
    }
  }

  char *new = malloc (a_sz + b_sz + (b_sz != 0) + 1);
  *new = '\0';

  size_t i = 0;
  for (; i < a_sz; i++)
    new[i] = a[i];

  if (b_sz) {
    new[i++] = DIR_SEP;

    for (; i < a_sz + b_sz + 1; i++)
      new[i] = *spb++;
  } else {
    while (new[i - 1] == DIR_SEP)
      if (new[i - 2] == DIR_SEP)
        i--;
      else
        break;
  }

  new[i] = '\0';
  return new;
}

static size_t str_byte_copy (char *dest, const char *src, size_t nbytes) {
  const char *sp = src;
  size_t len = 0;

  while (len < nbytes && *sp) {
    dest[len] = *sp++;
    len++;
  }

  return len;
}

static size_t str_copy (char *dest, size_t dest_len, const char *src, size_t nbytes) {
  size_t num = (nbytes > (dest_len - 1) ? dest_len - 1 : nbytes);
  size_t len = (NULL == src ? 0 : str_byte_copy (dest, src, num));
  dest[len] = '\0';
  return len;
}

static int str_cmp_n (const char *sa, const char *sb, size_t n) {
  const unsigned char *spa = (const unsigned char *) sa;
  const unsigned char *spb = (const unsigned char *) sb;
  for (;n--; spa++, spb++) {
    if (*spa != *spb)
      return (*(unsigned char *) spa - *(unsigned char *) spb);

    if (*spa == 0) return 0;
  }

  return 0;
}

static int str_eq_n (const char *sa, const char *sb, size_t n) {
  return (str_cmp_n (sa, sb, n) == 0);
}

static int str_eq (const char *sa, const char *sb) {
  const unsigned char *spa = (const unsigned char *) sa;
  const unsigned char *spb = (const unsigned char *) sb;
  for (; *spa == *spb; spa++, spb++)
    if (*spa == 0) return 1;

  return 0;
}

static size_t string_align (size_t size) {
  size_t sz = 8 - (size % 8);
  sz = sizeof (char) * (size + (sz < 8 ? sz : 0));
  return sz;
}

static string *string_new (size_t len) {
  string *s = malloc (sizeof (string));
  size_t sz = (len <= 0 ? 8 : string_align (len));
  s->bytes = malloc (sz);
  s->mem_size = sz;
  s->num_bytes = 0;
  s->bytes[0] = '\0';
  return s;
}

static string *string_new_with_len (const char *bytes, size_t len) {
  string *new = malloc (sizeof (string));
  size_t sz = string_align (len + 1);
  char *buf = malloc (sz);
  len = str_copy (buf, sz, bytes, len);
  new->bytes = buf;
  new->num_bytes = len;
  new->mem_size = sz;
  return new;
}

static string *string_new_with (const char *bytes) {
  size_t len = (NULL == bytes ? 0 : strlen (bytes));
  return string_new_with_len (bytes, len); /* this succeeds even if bytes is NULL */
}

static string *string_reallocate (string *this, size_t len) {
  size_t sz = string_align (this->mem_size + len + 1);
  this->bytes = realloc (this->bytes, sz);
  this->mem_size = sz;
  return this;
}

static string *string_append_byte (string *this, char c) {
  int bts = this->mem_size - (this->num_bytes + 2);
  if (1 > bts) string_reallocate (this, 8);
  this->bytes[this->num_bytes++] = c;
  this->bytes[this->num_bytes] = '\0';
  return this;
}

static size_t str_byte_mv (char *str, size_t len, size_t to_idx,
                                   size_t from_idx, size_t nelem) {
  if (from_idx == to_idx) return 0;
  while (to_idx + nelem > len) nelem--;

  size_t n = nelem;

  if (to_idx > from_idx) {
    char *sp = str + from_idx + nelem;
    char *dsp = str + to_idx + nelem;

    while (nelem--) *--dsp = *--sp;
    return (n - nelem) - 1;
  }

  while (from_idx + nelem > len) nelem--;
  n = nelem;

  char *sp = str + from_idx;
  char *dsp = str + to_idx;

  while (nelem) {
    ifnot (*sp) {  // stop at the first null byte
      *dsp = '\0'; // this differs in memmove()
      break;
    }

    *dsp++ = *sp++;
    nelem--;
  }

  return n - nelem;
}

static string *string_insert_at_with_len (string *this, int idx, const char *bytes, size_t len) {
  size_t bts = this->num_bytes + len;
  if (bts >= this->mem_size) string_reallocate (this, bts - this->mem_size + 1);

  if (idx == (int) this->num_bytes) {
    str_byte_copy (this->bytes + this->num_bytes, bytes, len);
  } else {
    str_byte_mv (this->bytes, this->mem_size - 1, idx + len, idx, this->num_bytes - idx);
    str_byte_copy (this->bytes + idx, bytes, len);
  }

  this->num_bytes += len;
  this->bytes[this->num_bytes] = '\0';
  return this;
}

static string *string_append_with_len (string *this, const char *bytes,
                                                                 size_t len) {
  return string_insert_at_with_len (this, this->num_bytes, bytes, len);
}

static void string_release (string *this) {
  if (this == NULL) return;
  if (this->mem_size)
    free (this->bytes);
  free (this);
}

static void string_clear (string *this) {
  ifnot (this->num_bytes) return;
  this->bytes[0] = '\0';
  this->num_bytes = 0;
}

static char *own_input (char *buffer, int size, void *userdata) {
  errorgen_t *t = (errorgen_t *) userdata;
  char *ptr = t->ptr;
  ifnot (*ptr) return NULL;

  int i = 0;
  while (*ptr && i < size)
    buffer[i++] = *ptr++;

  buffer[i] = '\0';

  t->ptr = ptr;
  return buffer;
}

static void own_output (int c, void *userdata) {
  errorgen_t *t = (errorgen_t *) userdata;
  string *line = t->line;

  if (c != '\n') {
    string_append_byte (line, c);
    return;
  }

  ifnot (str_eq_n (line->bytes, "#define E", 9))
    goto theend;

  string *s = NULL;

  char *ptr = line->bytes + line->num_bytes - 1;
  while (*(ptr - 1) != ' ' && *(ptr - 1) != '\t')
    ptr--;

  int r = 0;
  while ('0' <= *ptr && *ptr <= '9')
    r = 10 * r + (*ptr++ - '0');

  ptr = line->bytes + 8;
  while (*ptr == ' ' || *ptr == '\t') ptr++;

  int idx = r;

  ifnot (r) {
    if (str_eq_n (ptr, "EWOULDBLOCK", sizeof ("EWOULDBLOCK") - 1)) {
      r = EAGAIN;
      s = string_new_with ("EWOULDBLOCK");
      idx = 135;
    } else if (str_eq_n (ptr, "ENOTSUP", sizeof ("ENOTSUP") - 1)) {
      r = EOPNOTSUPP;
      s = string_new_with ("ENOTSUP");
      idx = 136;
    } else if (str_eq_n (ptr, "EDEADLOCK", sizeof ("EDEADLOCK") - 1)) {
      r = EDEADLK;
      s = string_new_with ("EDEADLOCK");
      idx = 137;
    } else
      goto theend;

  } else {
    s = string_new (16);
    while (*ptr && *ptr != ' ' && *ptr != '\t')
      string_append_byte (s, *ptr++);
  }

  t->errors[idx] = r;
  t->strerrors[idx] = string_new_with (strerror (r));
  t->definitions[idx] = s;
  if (idx > t->last_idx)
    t->last_idx = idx;

theend:
  string_clear (line);
}

static void own_error (void *userdata, char *format, va_list arg) {
  errorgen_t *t = (errorgen_t *) userdata;
  char buf[MAXLEN_ERROR_MSG];
  int n = vsnprintf (buf, MAXLEN_ERROR_MSG, format, arg);
  if (n > 0)
    string_append_with_len (t->errmsg, buf, n);
}

int main (int argc, char **argv) {
  int force = 0;
  char *name = argv[0];

  int __argc = 0;
  char **__argv = argv;

  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];

    if (str_eq (arg, "--force")) {
      force = 1;
      continue;
    }

    if (str_eq (arg, "-h") || str_eq (arg, "--help")) {
      tostdout ("%s: [--force] directory\n", name);
      return 0;
    }

    if ('-' == *arg) {
      tostderr ("unknown argument %s\n", arg);
      return 1;
    }

    __argv[__argc++] = arg;
  }

  __argv[__argc] = NULL;

  if (0 == force) {
    if (0 == __argc)
      tostdout ("%s: [--force] directory\n", name);
    return 1;
  }

  if (0 == __argc) {
    tostderr ("%s: awaiting a directory\n", name);
    return 1;
  }

  char *errdir = __argv[0];

  if (0 == exists (errdir)) {
    if (-1 == mkdir (errdir, 0755)) {
      tostderr ("%s: can not make directory, %s\n",
        errdir, strerror (errno));
      return 1;
    }
  }

  if (0 == is_directory (errdir)) {
    tostderr ("%s: not a directory\n", errdir);
    return 1;
  }

  if (0 == is_writable (errdir)) {
    tostderr ("%s: directory is not writable\n", errdir);
    return 1;
  }

  size_t errdir_len = strlen (errdir);

  char *errors_file = path_concat (errdir, errdir_len, "errors.h", 8);
  char *errors_list = path_concat (errdir, errdir_len, "error_list.h", 12);
  char *errnostring = path_concat (errdir, errdir_len, "errno_string.c", 14);

  struct fcppTag tags[FCPP_MAX_TAGS];
  struct fcppTag *tagptr = tags;

  errorgen_t t;

  t.last_idx = 0;

  for (size_t i = 0; i < MAXLEN_ERROR_ARRAY; i++) {
    t.definitions[i] = NULL;
    t.strerrors[i] = NULL;
    t.errors[i] = -1;
  }

  t.line   = string_new (MAXLEN_ERROR_OUTPUT);
  t.errmsg = string_new (128);

  t.definitions[0] = string_new_with ("SUCCESS");
  t.strerrors[0]   = string_new_with ("Success");
  t.errors[0]      = 0;

  const char input[] = "#include <errno.h>\n";
  str_copy (t.in, MAXLEN_ERROR_INPUT, input, sizeof(input));
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

  string_release (t.line);

  int num = t.last_idx + 5;

  string **def_array = t.definitions;
  string **str_array = t.strerrors;
  int     *err_array = t.errors;

  int i = 0;
  for (; i < num - 4; i++) {
    ifnot (NULL == t.definitions[i])
      continue;

    def_array[i] = string_new_with ("EUNKNOWN");
    str_array[i] = string_new_with ("Unknown error");
    err_array[i] = i;
  }

  def_array[i] = string_new_with ("EINDEX");
  str_array[i] = string_new_with ("Index is out of range");
  err_array[i] = i;
  i++;

  def_array[i] = string_new_with ("EINTEGEROVERFLOW");
  str_array[i] = string_new_with ("Integer overflow");
  err_array[i] = i;
  i++;

  def_array[i] = string_new_with ("ECANNOTGETCWD");
  str_array[i] = string_new_with ("Can not get current directory");
  err_array[i] = i;
  i++;

  def_array[i] = string_new_with ("ENOTENOUGHSPACE");
  str_array[i] = string_new_with ("Not enough space");
  err_array[i] = i;

  num = i + 1;

  int retval = -1;
  FILE *errorsFp = NULL;
  FILE *errorslistFp = NULL;
  FILE *errnostringFp = NULL;

  errorsFp = fopen (errors_file, "w");
  if (NULL == errorsFp) {
    tostderr ("fopen(): %s: cannot open file pointer, %s\n",
      errors_file, strerror (errno));
    goto theend;
  }

  errorslistFp = fopen (errors_list, "w");
  if (NULL == errorslistFp) {
    tostderr ("fopen(): %s: cannot open file pointer, %s\n",
      errors_list, strerror (errno));
    goto theend;
  }

  errnostringFp = fopen (errnostring, "w");
  if (NULL == errnostringFp) {
    tostderr ("fopen(): %s: cannot open file pointer, %s\n",
      errnostring, strerror (errno));
    goto theend;
  }

  for (i = 0; i < num; i++) {
    char *def = def_array[i]->bytes;
    fprintf (errorsFp,"#ifndef %s\n", def);
    fprintf (errorsFp,"#define %s ",  def);
    if (i != err_array[i])
      fprintf (errorsFp,"%s\n", def_array[err_array[i]]->bytes);
    else
      fprintf (errorsFp,"%d\n", err_array[i]);

    fprintf (errorsFp,"#endif /* %s */\n", def);
    if (i + 1 != num)
      fprintf (errorsFp,"\n");
  }


  fprintf (errorslistFp, "// requires: error/errors.h\n\n");
  fprintf (errorslistFp, "#define LAST_ERROR %d\n\n", num - 5);
  fprintf (errorslistFp, "static const struct sys_errorlist_type {\n");
  fprintf (errorslistFp, "  const char *name;\n  const char *msg;\n  int errnum;\n");
  fprintf (errorslistFp, "} sys_errorlist[] = {\n");

  for (i = 0; i < num; i++) {
    fprintf (errorslistFp, "  { \"%s\", \"%s\", ", def_array[i]->bytes,  str_array[i]->bytes);

    if (i != err_array[i])
      fprintf (errorslistFp,"%s},\n", def_array[err_array[i]]->bytes);
    else
      fprintf (errorslistFp,"%d},\n", err_array[i]);
  }

  fprintf (errorslistFp, "};\n\n");
  fprintf (errorslistFp, "static int get_error_num (int errnum) {\n");
  fprintf (errorslistFp, "  if (LAST_ERROR < errnum || errnum < 0)\n");
  fprintf (errorslistFp, "    return EUNKNOWN;\n\n  return errnum;\n}\n");

  fprintf (errnostringFp, "// provides: char *errno_string (int)\n");
  fprintf (errnostringFp, "// provides: char *errno_name (int)\n");
  fprintf (errnostringFp, "// requires: error/errors.h\n");
  fprintf (errnostringFp, "// requires: error/error_list.h\n\n");
  fprintf (errnostringFp, "char *errno_string (int errnum) {\n");
  fprintf (errnostringFp, "  errnum = get_error_num (errnum);\n");
  fprintf (errnostringFp, "  return (char *) sys_errorlist[errnum].msg;\n}\n\n");
  fprintf (errnostringFp, "char *errno_name (int errnum) {\n");
  fprintf (errnostringFp, "  errnum = get_error_num (errnum);\n");
  fprintf (errnostringFp, "  return (char *) sys_errorlist[errnum].name;\n}\n");
  retval = 0;

theend:
  free (t.errmsg);
  free (errors_file);
  free (errors_list);
  free (errnostring);

  for (i = 0; i < num; i++) {
    string_release (def_array[i]);
    string_release (str_array[i]);
  }

  if (errorsFp != NULL)
    fclose (errorsFp);

  if (errorslistFp != NULL)
    fclose (errorslistFp);

  if (errnostringFp != NULL)
    fclose (errnostringFp);

  return retval;
}
