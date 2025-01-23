#define REQUIRE_STRING
#define REQUIRE_MAP
#define REQUIRE_STDIO
#define REQUIRE_ALLOC
#define REQUIRE_STR_EQ
#define REQUIRE_STR_EQ_N
#define REQUIRE_STR_NEW
#define REQUIRE_STR_STR
#define REQUIRE_STR_CMP
#define REQUIRE_QSORT
#define REQUIRE_STRTOL
#define REQUIRE_ATOI
#define REQUIRE_DECIMAL_TO_STRING
#define REQUIRE_READFILE

#include <libc.h>

MODULE(std);

static VALUE _map_set (l_t *this, VALUE v_map, VALUE v_key, VALUE v_val) {
  ifnot (IS_MAP(v_map)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Map_Type *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  int retval = L.map.reset_value (this, map, key, v_val);
  return INT(retval);
}

static VALUE _map_get (l_t *this, VALUE v_map, VALUE v_key) {
  (void) this;
  ifnot (IS_MAP(v_map)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Map_Type *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);
  VALUE *v = (VALUE *) map_get_value (map, key);
  return (*v);
}

static VALUE _map_remove (l_t *this, VALUE v_map, VALUE v_key) {
  ifnot (IS_MAP(v_map)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Map_Type *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  VALUE *val = map_pop_value (map, key);
  if (val != NULL)
    L.map.release_value (this, val);

  return OK_VALUE;
}

static VALUE _map_key_exists (l_t *this, VALUE v_map, VALUE v_key) {
  (void) this;
  ifnot (IS_MAP(v_map)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Map_Type *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  return INT(map_key_exists (map, key));
}

static VALUE _map_keys (l_t *this, VALUE v_map) {
  (void) this;
  ifnot (IS_MAP(v_map)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a map");
  Map_Type *map = AS_MAP(v_map);
  int num = map->num_keys;
  ArrayType *v_keys = Alloc (sizeof (ArrayType));
  v_keys->type = STRING_TYPE;
  v_keys->len  = num;
  char **ar = map_keys (map);

  string **keys = Alloc (sizeof (string) * num);
  for (int i = 0; i < num; i++) {
    size_t len = bytelen (ar[i]);
    keys[i] = string_new_with_allocated (ar[i], len, len);
  }

  v_keys->value = ARRAY(keys);
  return ARRAY(v_keys);
}

static int cmp_string (const void *a, const void *b) {
  const string *sa = *(string **) a, *sb = * (string  **)b;
  return str_cmp (sa->bytes, sb->bytes);
}

static int cmp_int (const void *a, const void *b) {
  const int ia = * (integer *) a, ib = *(integer *) b;
  if (ia > ib) return 1;
  if (ia < ib) return -1;
  return 0;
}

static int cmp_number (const void *a, const void *b) {
  const number na = *(number *) a, nb = *(number *) b;
  if (na > nb) return 1;
  if (na < nb) return -1;
  return 0;
}

static VALUE array_sort (l_t *this, VALUE v_array) {
  ifnot (IS_ARRAY(v_array)) THROW(L_ERR_TYPE_MISMATCH, "awaiting an array");
  ArrayType *array = (ArrayType *) AS_ARRAY(v_array);
  int type = array->type;
  int len = array->len;
  VALUE value = array->value;

  switch (type) {
    case STRING_TYPE: {
      string **s_ar = (string **) AS_ARRAY(value);
      string **new_s_ar = Alloc (len * sizeof (string));

      for (int i = 0; i < len; i++)
        new_s_ar[i] = string_new_with_len (s_ar[i]->bytes, s_ar[i]->num_bytes);

      quick_sort (new_s_ar, len, sizeof (string *), cmp_string);

      ArrayType *new_array = Alloc (sizeof (ArrayType));
      new_array->type = STRING_TYPE;
      new_array->len  = len;
      new_array->value = ARRAY(new_s_ar);
      return ARRAY(new_array);
    }

    case INTEGER_TYPE: {
      integer *i_ar = (integer *) AS_ARRAY(value);
      integer *new_i_ar = Alloc (len * sizeof (integer));
      for (int i = 0; i < len; i++)
        new_i_ar[i] = i_ar[i];

      quick_sort (new_i_ar, len, sizeof (new_i_ar[0]), cmp_int);

      ArrayType *new_array = Alloc (sizeof (ArrayType));
      new_array->type = INTEGER_TYPE;
      new_array->len  = len;
      new_array->value = ARRAY(new_i_ar);
      return ARRAY(new_array);
    }

    case NUMBER_TYPE: {
      number *i_ar = (number *) AS_ARRAY(value);
      number *new_i_ar = Alloc (len * sizeof (number));
      for (int i = 0; i < len; i++)
        new_i_ar[i] = i_ar[i];

      quick_sort (new_i_ar, len, sizeof (new_i_ar[0]), cmp_number);

      ArrayType *new_array = Alloc (sizeof (ArrayType));
      new_array->type = NUMBER_TYPE;
      new_array->len  = len;
      new_array->value = ARRAY(new_i_ar);
      return ARRAY(new_array);
    }

    default: {
      L.set.CFuncError (this, L_ERR_TYPE_MISMATCH);
      char buf[512];
      sys_snprintf (buf, 512, "%s, has been implemented for (Integer|String|Number)Type", __func__);
      L.set.curMsg (this, buf);
      return NULL_VALUE;
    }
  }

  return NULL_VALUE;
}

static VALUE array_where (l_t *this, VALUE v_array, VALUE v_expr) {
  ifnot (IS_ARRAY(v_array)) THROW(L_ERR_TYPE_MISMATCH, "awaiting an array");
  ArrayType *array = (ArrayType *) AS_ARRAY(v_array);
  int type = array->type;
  if (type != v_expr.type) {
    L.set.CFuncError (this, L_ERR_TYPE_MISMATCH);
    char buf[512];
    sys_snprintf (buf, 512, "%s, expression is not the same type", __func__);
    L.set.curMsg (this, buf);
    return NULL_VALUE;
  }

  switch (type) {
    case STRING_TYPE:
    case INTEGER_TYPE:
    case NUMBER_TYPE:
      break;

    default: {
      L.set.CFuncError (this, L_ERR_TYPE_MISMATCH);
      char buf[512];
      sys_snprintf (buf, 512, "%s, has been implemented for (Integer|String|Number)Type", __func__);
      L.set.curMsg (this, buf);
      return NULL_VALUE;
    }
  }

  integer *r_ar = Alloc (sizeof (integer));
  integer len = 0;

  switch (type) {
    case STRING_TYPE: {
      char *expr = AS_STRING_BYTES(v_expr);
      string **s_ar = (string **) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++) {
        if (str_eq (s_ar[i]->bytes, expr)) {
          len++;
          r_ar = Realloc (r_ar, len * sizeof (integer));
          r_ar[len-1] = i;
        }
      }
      break;
    }

    case INTEGER_TYPE: {
      int expr = AS_INT(v_expr);
      integer *i_ar = (integer *) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++) {
        if (i_ar[i] == expr) {
          len++;
          r_ar = Realloc (r_ar, len * sizeof (integer));
          r_ar[len-1] = i;
        }
      }
      break;
    }

    case NUMBER_TYPE: {
      double expr = AS_NUMBER(v_expr);
      double *d_ar = (double *) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++) {
        if (d_ar[i] == expr) {
          len++;
          r_ar = Realloc (r_ar, len * sizeof (integer));
          r_ar[len-1] = i;
        }
      }
      break;
    }
  }

  ArrayType *res_array = Alloc (sizeof (ArrayType));
  res_array->type = INTEGER_TYPE;
  res_array->len  = len;
  res_array->value = ARRAY(r_ar);
  return ARRAY(res_array);
}

#if 0
static VALUE array_len (l_t *this, VALUE v_array) {
  (void) this;
  ifnot (IS_ARRAY(v_array)) THROW(L_ERR_TYPE_MISMATCH, "awaiting an array");
  ArrayType *array = (ArrayType *) AS_ARRAY(v_array);
  return INT(array->len);
}

static VALUE string_to_array (l_t *this, VALUE v_s) {
  (void) this;
  ifnot (IS_STRING(v_s)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING(v_s);

  ArrayType *array = ARRAY_NEW(INTEGER_TYPE, (int) s->num_bytes);
  integer *ar = (integer *) AS_ARRAY(array->value);

  char *sp = s->bytes;
  for (size_t i = 0; i < s->num_bytes; i++)
    ar[i] = *sp++;

  return ARRAY(array);
}

static VALUE string_cmp (l_t *this, VALUE v_sa, VALUE v_sb) {
  (void) this;
  ifnot (IS_STRING(v_sa)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_sb)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");

  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);

  int retval = Cstring.cmp (sa, sb);

  ifnot (retval) return INT(0);
  if (retval < 0) return INT(-1);
  return INT(1);
}

static VALUE string_cmp_n (l_t *this, VALUE v_sa, VALUE v_sb, VALUE v_n) {
  (void) this;
  ifnot (IS_STRING(v_sa)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_sb)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_n))     THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);
  int n = AS_INT(v_n);

  int retval = Cstring.cmp_n (sa, sb, n);

  ifnot (retval) return INT(0);
  if (retval < 0) return INT(-1);
  return INT(1);
}
#endif

static VALUE string_eq_n (l_t *this, VALUE v_sa, VALUE v_sb, VALUE v_n) {
  (void) this;
  ifnot (IS_STRING(v_sa)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_sb)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_n))     THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);
  int n = AS_INT(v_n);

  return INT(str_eq_n (sa, sb, n));
}

static VALUE string_eq (l_t *this, VALUE v_sa, VALUE v_sb) {
  (void) this;
  ifnot (IS_STRING(v_sa)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_sb)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");

  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);
  return INT(str_eq (sa, sb));
}

#if 0
static VALUE string_byte_in_str (l_t *this, VALUE v_str, VALUE v_byte) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_byte)) THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");
  char *str = AS_STRING_BYTES(v_str);
  int byte = AS_INT(v_byte);
  if (byte < 0 || byte > 255)  THROW(L_ERR_OUTOFBOUNDS, "awaiting a byte in the ASCII range");
  char *new = Cstring.byte.in_str (str, byte);
  if (NULL == new)
    return NULL_VALUE;
  string *result = String.new_with (new);
  return STRING(result);
}

static VALUE string_bytes_in_str (l_t *this, VALUE v_str, VALUE v_bytes) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_bytes)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  char *str = AS_STRING_BYTES(v_str);
  char *bytes = AS_STRING_BYTES(v_bytes);
  char *new = Cstring.bytes_in_str (str, bytes);
  if (NULL == new)
    return NULL_VALUE;
  string *result = String.new_with (new);
  return STRING(result);
}
#endif

static VALUE string_delete_substr (l_t *this, VALUE v_str, VALUE v_substr) {
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_substr)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");

  string *s = AS_STRING(v_str);
  char *str = s->bytes;
  char *substr = AS_STRING_BYTES(v_substr);

  string *new = string_new_with_len (str, s->num_bytes);
  VALUE r = STRING(new);

  char *sp = str_str (str, substr);
  if (NULL == sp) return r;

  int idx = sp - str;
  int len = bytelen (substr);
  string_delete_numbytes_at (new, len, idx);
  return r;
}

#if 0
static VALUE string_advance_after_bytes (l_t *this, VALUE v_str, VALUE v_bytes, VALUE v_repeat) {
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_bytes)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_repeat)) THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *str = AS_STRING_BYTES(v_str);
  string *s = AS_STRING(v_bytes);
  char *bytes = s->bytes;
  size_t len = s->num_bytes;
  ifnot (len) return STRING_NEW("");

  int repeat = AS_INT(v_repeat);
  ifnot (repeat) return NULL_VALUE;

  char *src = str;
  char *sp = NULL;
  int num = 0;
  while (num < repeat) {
    sp = Cstring.bytes_in_str (src, bytes);
    if (NULL == sp)
      return NULL_VALUE;

    num++;
    src = sp + len;

    if (*src == '\0')
      return STRING_NEW("");
  }

  return STRING_NEW(src);
}

static VALUE string_advance_on_byte (l_t *this, VALUE v_str, VALUE v_byte) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_byte)) THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");
  char *str = AS_STRING_BYTES(v_str);
  int byte = AS_INT(v_byte);
  if (byte < 0 || byte > 255)  THROW(L_ERR_OUTOFBOUNDS, "awaiting a byte in the ASCII range");

  char *sp = Cstring.byte.in_str (str, byte);
  if (NULL == sp) return NULL_VALUE;

  string *new = String.new_with (sp);
  return STRING(new);
}

static VALUE string_advance (l_t *this, VALUE v_str, VALUE v_n) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_n)) THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");
  string *str = AS_STRING(v_str);
  ifnot (str->num_bytes) return STRING_NEW("");

  int n = AS_INT(v_n);
  if (n < 0) return STRING_NEW("");

  if (n > (int) str->num_bytes) return NULL_VALUE;

  string *new = String.new_with_len (str->bytes + n, str->num_bytes - n);

  return STRING(new);
}
#endif

static VALUE string_tokenize (l_t *this, VALUE v_str, VALUE v_tok) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_tok)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");

  char *str = AS_STRING_BYTES(v_str);
  char *tok = AS_STRING_BYTES(v_tok);

  size_t toklen = AS_STRING(v_tok)->num_bytes;
  char *sp = str;
  char *beg = sp;

  int len = 0;

  ArrayType *array = ARRAY_NEW(STRING_TYPE, len);

  string *s = NULL;

  int end = 0;

// we want:
// " a  b c   d  " = ["a", "b", "c", "d"] with " " as separator
  for (;;) {
    if (end) break;

    ifnot (*sp) {
      ifnot (*beg) break;
      end = 1;
      goto add_element;
    }

    if (str_eq_n (sp, tok, toklen)) {

    add_element:
      if (sp == beg) { beg = sp + 1; goto next; }

      s = string_new_with_len (beg, sp - beg);

      VALUE star = STRING(s);
      array = ARRAY_APPEND(array, star);
      beg = sp + 1;
    }

  next:
    sp++;
  }

  return ARRAY(array);
}


static VALUE string_to_integer (l_t *this, VALUE v_str) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  char *str = AS_STRING_BYTES(v_str);
  if (str[0] == '0') {
    if (str[1] == 'b')
      return INT(str_to_long (&str[2], NULL, 2));
    else if (str[1] == 'x' || str[1] == 'X')
      return INT(str_to_long (&str[2], NULL, 16));
    else if (str[1] != '\0')
      return INT(str_to_long (&str[1], NULL, 8));
  }

  return INT(str_to_int (str));
}

#if 0
static VALUE string_to_number (l_t *this, VALUE v_str) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  return NUMBER(strtod (AS_STRING_BYTES(v_str), NULL));
}

static VALUE string_bytelen (l_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  return INT(bytelen (AS_STRING_BYTES(v_str)));
}

static VALUE string_numchars (l_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING(v_str);
  return INT(Ustring.char_num (s->bytes, s->num_bytes)); // a small disharmony here
}

static VALUE string_to_upper (l_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING(v_str);
  ifnot (s->num_bytes) return STRING_NEW("");

  char buf[s->num_bytes + 1];
  Ustring.change_case (buf, s->bytes, s->num_bytes, TO_UPPER);
  return STRING(String.new_with (buf));
}

static VALUE string_to_lower (l_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING(v_str);
  ifnot (s->num_bytes) return STRING_NEW("");

  char buf[s->num_bytes + 1];
  Ustring.change_case (buf, s->bytes, s->num_bytes, TO_LOWER);
  return STRING(String.new_with (buf));
}

static VALUE string_trim_byte_at_end (l_t *this, VALUE v_str, VALUE v_byte) {
  ifnot (IS_STRING(v_str)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_byte))   THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");
  string *s = AS_STRING(v_str);
  int byte = AS_INT(v_byte);
  if (byte < 0 || byte > 255)  THROW(L_ERR_OUTOFBOUNDS, "awaiting a byte in the ASCII range");
  string *new = String.dup (s);
  String.trim_end (new, byte);
  return STRING(new);
}
#endif

static VALUE integer_to_string (l_t *this, VALUE v_int, VALUE v_base) {
  (void) this;
  if (IS_INT(v_int) == 0 || IS_INT(v_base) == 0)
    THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  int base = AS_INT(v_base);

  decimal_t dec = {.base = base};

  char *buf = int_to_string (&dec, AS_INT(v_int));

  string *s = string_new_with_len (buf, dec.size);

  switch (base) {
    case  8: string_prepend_byte (s, '0'); break;
    case 16: string_prepend_with (s, "0x"); break;
    case  2: {
      int n = dec.size % 4;
      for (int i = 0; i < n; i++)
        string_prepend_with (s, "0");

      string_prepend_with (s, "0b");
      break;
    }
  }

  return STRING(s);
}

#if 0
static VALUE integer_char (l_t *this, VALUE v_c) {
  (void) this;
  ifnot (IS_INT(v_c)) THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  utf8 c = AS_INT(v_c);
  char buf[8];
  int len = 0;
  Ustring.character (c, buf, &len);
  string *s = String.new_with (buf);
  if (len == 1 and c == '\\')
    String.append_byte (s, '\\');

  return STRING(s);
}

static VALUE integer_eq (l_t *this, VALUE v_fint, VALUE v_sint) {
  (void) this;
  if (IS_INT(v_fint) == 0 || IS_INT(v_sint) == 0)
    THROW(L_ERR_TYPE_MISMATCH, "awaiting an integer");

  return INT(AS_INT(v_fint) == AS_INT(v_sint));
}
#endif

static VALUE _readfile (l_t *this, VALUE v_file) {
  (void) this;

  ifnot (IS_STRING(v_file)) THROW(L_ERR_TYPE_MISMATCH, "awaiting a string");

  readfile_t rf = { .file = AS_STRING_BYTES(v_file) };
  if (-1 == readfile_u (&rf))
    return NULL_VALUE;

  string *n = string_new_with_allocated (rf.bytes, rf.num_bytes, rf.mem_size);
  return STRING(n);
}

public int __init_std_module__ (l_t *this) {
  __INIT_MODULE__(this);

  LDefCFun lfuns[] = {
    { "map_set",            PTR(_map_set), 3 },
    { "map_get",            PTR(_map_get), 2 },
    { "map_keys",           PTR(_map_keys), 1 },
    { "map_remove",         PTR(_map_remove), 2 },
    { "map_key_exists",     PTR(_map_key_exists), 2 },
#if 0
    { "array_len",          PTR(array_len), 1},
#endif
    { "array_sort",         PTR(array_sort), 1},
    { "array_where",        PTR(array_where), 2 },
    { "string_eq",          PTR(string_eq), 2 },
    { "string_eq_n",        PTR(string_eq_n), 3 },
#if 0
    { "string_cmp",         PTR(string_cmp), 2 },
    { "string_cmp_n",       PTR(string_cmp_n), 3 },
    { "string_advance",     PTR(string_advance), 2 },
    { "string_bytelen",     PTR(string_bytelen), 1 },
    { "string_to_array",    PTR(string_to_array), 1},
    { "string_numchars",    PTR(string_numchars), 1 },
    { "string_to_upper",    PTR(string_to_upper), 1 },
    { "string_to_lower",    PTR(string_to_lower), 1 },
#endif
    { "string_tokenize",    PTR(string_tokenize), 2 },
#if 0
    { "string_to_number",   PTR(string_to_number), 1 },
#endif
    { "string_to_integer",  PTR(string_to_integer), 1 },
#if 0
    { "string_byte_in_str", PTR(string_byte_in_str), 2 },
    { "string_bytes_in_str", PTR(string_bytes_in_str), 2 },
#endif
    { "string_delete_substr", PTR(string_delete_substr), 2 },
#if 0
    { "string_advance_on_byte", PTR(string_advance_on_byte), 2},
    { "string_advance_after_bytes", PTR(string_advance_after_bytes), 3},
    { "string_trim_byte_at_end", PTR(string_trim_byte_at_end), 2},
    { "integer_eq",         PTR(integer_eq), 2},
    { "integer_char",       PTR(integer_char), 1 },
#endif
    { "integer_to_string",  PTR(integer_to_string), 2},
    { "fileread",           PTR(_readfile), 1},

    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lfuns[i].name; i++) {
    if (L_OK != (err = L.def (this, lfuns[i].name, L_CFUNC (lfuns[i].nargs), lfuns[i].val)))
      return err;
  }

  if (L.def_std (this, "BINARY",  INTEGER_TYPE, INT(2),  1)) return L_NOTOK;
  if (L.def_std (this, "OCTAL",   INTEGER_TYPE, INT(8),  1)) return L_NOTOK;
  if (L.def_std (this, "DECIMAL", INTEGER_TYPE, INT(10), 1)) return L_NOTOK;
  if (L.def_std (this, "HEX",     INTEGER_TYPE, INT(16), 1)) return L_NOTOK;

  const char evalString[] = EvalString (
    public const Map = {
       set : map_set,
       get : map_get,
       keys : map_keys,
       remove : map_remove,
       key_exists : map_key_exists
     };

    public const String = {
      eq : string_eq,
      eq_n : string_eq_n,
      tokenize : string_tokenize,
      to_integer : string_to_integer,
      delete_substr : string_delete_substr
    };

    public const Integer = {
      to_string : integer_to_string
    };
    public const Array = {
      sort : array_sort,
      where : array_where
    };

    public const Std = {
      map     : Map,
      array   : Array,
      string  : String
    };

    public const File = {
      read    : fileread
    };
 );

  err = L.eval_string (this, evalString);

  if (err != L_OK) return err;
  return L_OK;
}

public void __deinit_std_module__ (l_t *this) {
  (void) this;
  return;
}
