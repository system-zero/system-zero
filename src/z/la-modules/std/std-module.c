#define REQUIRE_STDIO

#define REQUIRE_STD_MODULE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_USTRING_TYPE  DECLARE

#include <z/cenv.h>

MODULE(std);

static VALUE map_set (la_t *this, VALUE v_map, VALUE v_key, VALUE v_val) {
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  int retval = La.map.reset_value (this, map, key, v_val);
  return INT(retval);
}

static VALUE map_get (la_t *this, VALUE v_map, VALUE v_key) {
  (void) this;
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);
  VALUE *v = (VALUE *) Vmap.get (map, key);
  return (*v);
}

static VALUE map_remove (la_t *this, VALUE v_map, VALUE v_key) {
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  VALUE *val = Vmap.pop (map, key);
  if (val isnot NULL)
    La.map.release_value (this, val);

  return OK_VALUE;
}

static VALUE map_key_exists (la_t *this, VALUE v_map, VALUE v_key) {
  (void) this;
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  ifnot (IS_STRING(v_key)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string as a key");
  Vmap_t *map = AS_MAP(v_map);
  char *key = AS_STRING_BYTES(v_key);

  return INT(Vmap.key_exists (map, key));
}

static VALUE map_keys (la_t *this, VALUE v_map) {
  (void) this;
  ifnot (IS_MAP(v_map)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a map");
  Vmap_t *map = AS_MAP(v_map);
  int num = Vmap.num_keys (map);
  ArrayType *v_keys = Alloc (sizeof (ArrayType));
  v_keys->type = STRING_TYPE;
  v_keys->len  = num;
  string **keys = Vmap.keys (map);
  v_keys->value = ARRAY(keys);
  return ARRAY(v_keys);
}

static int cmp_string (const void *a, const void *b) {
  const string *sa = *(string **) a, *sb = * (string  **)b;
  return Cstring.cmp (sa->bytes, sb->bytes);
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

static VALUE array_sort (la_t *this, VALUE v_array) {
  ifnot (IS_ARRAY(v_array)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an array");
  ArrayType *array = (ArrayType *) AS_ARRAY(v_array);
  int type = array->type;
  size_t len = array->len;
  VALUE value = array->value;

  switch (type) {
    case STRING_TYPE: {
      string **s_ar = (string **) AS_ARRAY(value);
      string **new_s_ar = Alloc (len * sizeof (string));
      for (size_t i = 0; i < len; i++)
        new_s_ar[i] = String.new_with_len (s_ar[i]->bytes, s_ar[i]->num_bytes);

      qsort (new_s_ar, len, sizeof(string *), cmp_string);

      ArrayType *new_array = Alloc (sizeof (ArrayType));
      new_array->type = STRING_TYPE;
      new_array->len  = len;
      new_array->value = ARRAY(new_s_ar);
      return ARRAY(new_array);
    }

    case INTEGER_TYPE: {
      integer *i_ar = (integer *) AS_ARRAY(value);
      integer *new_i_ar = Alloc (len * sizeof (integer));
      for (size_t i = 0; i < len; i++)
        new_i_ar[i] = i_ar[i];

      qsort (new_i_ar, len, sizeof(new_i_ar[0]), cmp_int);

      ArrayType *new_array = Alloc (sizeof (ArrayType));
      new_array->type = INTEGER_TYPE;
      new_array->len  = len;
      new_array->value = ARRAY(new_i_ar);
      return ARRAY(new_array);
    }

    case NUMBER_TYPE: {
      number *i_ar = (number *) AS_ARRAY(value);
      number *new_i_ar = Alloc (len * sizeof (number));
      for (size_t i = 0; i < len; i++)
        new_i_ar[i] = i_ar[i];

      qsort (new_i_ar, len, sizeof(new_i_ar[0]), cmp_number);

      ArrayType *new_array = Alloc (sizeof (ArrayType));
      new_array->type = NUMBER_TYPE;
      new_array->len  = len;
      new_array->value = ARRAY(new_i_ar);
      return ARRAY(new_array);
    }

    default:
      La.set.CFuncError (this, LA_ERR_TYPE_MISMATCH);
      La.set.curMsg (this, STR_FMT("%s, has been implemented for (Integer|String|Number)Type", __func__));
      return NULL_VALUE;
  }
  return NULL_VALUE;

}

static VALUE array_where (la_t *this, VALUE v_array, VALUE v_expr) {
  ifnot (IS_ARRAY(v_array)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an array");
  ArrayType *array = (ArrayType *) AS_ARRAY(v_array);
  int type = array->type;
  if (type isnot v_expr.type) {
    La.set.CFuncError (this, LA_ERR_TYPE_MISMATCH);
    La.set.curMsg (this, STR_FMT("%s, expression is not the same type", __func__));
    return NULL_VALUE;
  }

  switch (type) {
    case STRING_TYPE:
    case INTEGER_TYPE:
    case NUMBER_TYPE:
      break;

    default:
      La.set.CFuncError (this, LA_ERR_TYPE_MISMATCH);
      La.set.curMsg (this, STR_FMT("%s, has been implemented for (Integer|String|Number)Type", __func__));
      return NULL_VALUE;
  }

  integer *r_ar = Alloc (sizeof (integer));
  integer len = 0;

  switch (type) {
    case STRING_TYPE: {
      char *expr = AS_STRING_BYTES(v_expr);
      string **s_ar = (string **) AS_ARRAY(array->value);
      for (size_t i = 0; i < array->len; i++) {
        if (Cstring.eq (s_ar[i]->bytes, expr)) {
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
        if (i_ar[i] is expr) {
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
        if (d_ar[i] is expr) {
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

static VALUE array_len (la_t *this, VALUE v_array) {
  (void) this;
  ifnot (IS_ARRAY(v_array)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an array");
  ArrayType *array = (ArrayType *) AS_ARRAY(v_array);
  return INT(array->len);
}

static VALUE string_to_array (la_t *this, VALUE v_s) {
  (void) this;
  ifnot (IS_STRING(v_s)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING(v_s);

  ArrayType *array = ARRAY_NEW(INTEGER_TYPE, (int) s->num_bytes);
  integer *ar = (integer *) AS_ARRAY(array->value);

  char *sp = s->bytes;
  for (size_t i = 0; i < s->num_bytes; i++)
    ar[i] = *sp++;

  return ARRAY(array);
}

static VALUE string_cmp (la_t *this, VALUE v_sa, VALUE v_sb) {
  (void) this;
  ifnot (IS_STRING(v_sa)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_sb)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);

  int retval = Cstring.cmp (sa, sb);

  ifnot (retval) return INT(0);
  if (retval < 0) return INT(-1);
  return INT(1);
}

static VALUE string_cmp_n (la_t *this, VALUE v_sa, VALUE v_sb, VALUE v_n) {
  (void) this;
  ifnot (IS_STRING(v_sa)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_sb)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_n))     THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);
  int n = AS_INT(v_n);

  int retval = Cstring.cmp_n (sa, sb, n);

  ifnot (retval) return INT(0);
  if (retval < 0) return INT(-1);
  return INT(1);
}

static VALUE string_eq_n (la_t *this, VALUE v_sa, VALUE v_sb, VALUE v_n) {
  (void) this;
  ifnot (IS_STRING(v_sa)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_sb)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_n))     THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);
  int n = AS_INT(v_n);

  return INT(Cstring.eq_n (sa, sb, n));
}

static VALUE string_eq (la_t *this, VALUE v_sa, VALUE v_sb) {
  (void) this;
  ifnot (IS_STRING(v_sa)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_sb)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

  char *sa = AS_STRING_BYTES(v_sa);
  char *sb = AS_STRING_BYTES(v_sb);
  return INT(Cstring.eq (sa, sb));
}

static VALUE string_byte_in_str (la_t *this, VALUE v_str, VALUE v_byte) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_byte)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  char *str = AS_STRING_BYTES(v_str);
  int byte = AS_INT(v_byte);
  if (byte < 0 or byte > 255)  THROW(LA_ERR_OUTOFBOUNDS, "awaiting a byte in the ASCII range");
  char *new = Cstring.byte.in_str (str, byte);
  if (NULL is new)
    return NULL_VALUE;
  string *result = String.new_with (new);
  return STRING(result);
}

static VALUE string_bytes_in_str (la_t *this, VALUE v_str, VALUE v_bytes) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_bytes)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *str = AS_STRING_BYTES(v_str);
  char *bytes = AS_STRING_BYTES(v_bytes);
  char *new = Cstring.bytes_in_str (str, bytes);
  if (NULL is new)
    return NULL_VALUE;
  string *result = String.new_with (new);
  return STRING(result);
}

/* this function serves as a prototype and it'll be used in the 'y' namespace */

// name: we may leave this unimplemented for the parser (but see this as one
//       token as we may use the space after as a delimeter for multiply statements)
// return: "null or string" (probably (") as a open/close delimeter to mean
//            specific things based on this specific context )
// does: advance the beginning of a string after the last found byte of the
//       given byte sequence, after repeated "nth" times the search
// when: hasn't been found returns null, if found and the last found byte + 1
//       is the NUL byte ('\0') then returns an empty string,
//       likewise for the case the length of the source string is zero length
// else: returns the string advanced after the last found byte of the sequence  
static VALUE string_advance_after_bytes (la_t *this, VALUE v_str, VALUE v_bytes, VALUE v_repeat) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_bytes)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_repeat)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

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
    if (NULL is sp)
      return NULL_VALUE;

    num++;
    src = sp + len;

    if (*src is '\0')
      return STRING_NEW("");
  }

  return STRING_NEW(src);
}

static VALUE string_advance_on_byte (la_t *this, VALUE v_str, VALUE v_byte) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_byte)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  char *str = AS_STRING_BYTES(v_str);
  int byte = AS_INT(v_byte);
  if (byte < 0 or byte > 255)  THROW(LA_ERR_OUTOFBOUNDS, "awaiting a byte in the ASCII range");

  char *sp = Cstring.byte.in_str (str, byte);
  if (NULL is sp) return NULL_VALUE;

  string *new = String.new_with (sp);
  return STRING(new);
}

static VALUE string_advance (la_t *this, VALUE v_str, VALUE v_n) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_n)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  string *str = AS_STRING(v_str);
  ifnot (str->num_bytes) return STRING_NEW("");

  int n = AS_INT(v_n);
  if (n < 0) return STRING_NEW("");

  if (n > (int) str->num_bytes) return NULL_VALUE;

  string *new = String.new_with_len (str->bytes + n, str->num_bytes - n);

  return STRING(new);
}

static VALUE string_tokenize (la_t *this, VALUE v_str, VALUE v_tok) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_STRING(v_tok)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");

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

    if (Cstring.eq_n (sp, tok, toklen)) {

    add_element:
      if (sp is beg) { beg = sp + 1; goto next; }

      s = String.new_with_len (beg, sp - beg);

      VALUE star = STRING(s);
      array = ARRAY_APPEND(array, star);
      beg = sp + 1;
    }

  next:
    sp++;
  }

  return ARRAY(array);
}

static VALUE string_to_integer (la_t *this, VALUE v_str) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *str = AS_STRING_BYTES(v_str);
  if (str[0] is '0') {
    if (str[1] is 'b')
      return INT(strtol (&str[2], NULL, 2));
    else if (str[1] is 'x' or str[1] is 'X')
      return INT(strtol(&str[2], NULL, 16));
    else if (str[1] isnot '\0')
      return INT(strtol(&str[1], NULL, 8));
  }

  return INT(atoi (str));
}

static VALUE string_to_number (la_t *this, VALUE v_str) {
  (void) this;
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  return NUMBER(strtod (AS_STRING_BYTES(v_str), NULL));
}

static VALUE string_bytelen (la_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  return INT(bytelen (AS_STRING_BYTES(v_str)));
}

static VALUE string_numchars (la_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING(v_str);
  return INT(Ustring.char_num (s->bytes, s->num_bytes)); // a small disharmony here
}

static VALUE string_to_upper (la_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING(v_str);
  ifnot (s->num_bytes) return STRING_NEW("");

  char buf[s->num_bytes + 1];
  Ustring.change_case (buf, s->bytes, s->num_bytes, TO_UPPER);
  return STRING(String.new_with (buf));
}

static VALUE string_to_lower (la_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING(v_str);
  ifnot (s->num_bytes) return STRING_NEW("");

  char buf[s->num_bytes + 1];
  Ustring.change_case (buf, s->bytes, s->num_bytes, TO_LOWER);
  return STRING(String.new_with (buf));
}

static VALUE string_trim_byte_at_end (la_t *this, VALUE v_str, VALUE v_byte) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_byte))   THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");
  string *s = AS_STRING(v_str);
  int byte = AS_INT(v_byte);
  if (byte < 0 or byte > 255)  THROW(LA_ERR_OUTOFBOUNDS, "awaiting a byte in the ASCII range");
  string *new = String.dup (s);
  String.trim_end (new, byte);
  return STRING(new);
}

static VALUE integer_to_string (la_t *this, VALUE v_int, VALUE v_base) {
  (void) this;
  if (IS_INT(v_int) is 0 or IS_INT(v_base) is 0)
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  int base = AS_INT(v_base);
  if (base isnot 2 and base isnot 8 and base isnot 10 and base isnot 16)
    THROW(LA_ERR_SYNTAX, "awaiting 2 or 8 or 10 or 16 as a convertion base");

  char buf[1024];
  Cstring.itoa (AS_INT(v_int), buf, base);
  string *s = String.new_with (buf);
  switch (base) {
    case 10: break;
    case 8: String.prepend_byte (s, '0'); break;
    case 16: String.prepend_with (s, "0x"); break;
    case 2: {
      int num = s->num_bytes;
      if (num is 4 or num is 8 or num is 16 or num is 32 or num is 64)
        break;
      if (num < 4) {
        for (int i = 0; i < (4 - num); i++)
          String.prepend_byte (s, '0');
      } else if (num < 8) {
        for (int i = 0; i < (8 - num); i++)
          String.prepend_byte (s, '0');
      } else if (num < 16) {
        for (int i = 0; i < (16 - num); i++)
          String.prepend_byte (s, '0');
      } else if (num < 32) {
        for (int i = 0; i < (32 - num); i++)
          String.prepend_byte (s, '0');
      } else if (num < 64) {
        for (int i = 0; i < (64 - num); i++)
          String.prepend_byte (s, '0');
      }
      String.prepend_with (s, "0b");
      break;
    }
  }

  return STRING(s);
}

static VALUE integer_char (la_t *this, VALUE v_c) {
  (void) this;
  ifnot (IS_INT(v_c)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  utf8 c = AS_INT(v_c);
  char buf[8];
  int len = 0;
  Ustring.character (c, buf, &len);
  string *s = String.new_with (buf);
  if (len is 1 and c is '\\')
    String.append_byte (s, '\\');

  return STRING(s);
}

static VALUE integer_eq (la_t *this, VALUE v_fint, VALUE v_sint) {
  (void) this;
  if (IS_INT(v_fint) is 0 or IS_INT(v_sint) is 0)
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer");

  return INT(AS_INT(v_fint) is AS_INT(v_sint));
}

public int __init_std_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(vstring);
  __INIT__(cstring);
  __INIT__(ustring);

  LaDefCFun lafuns[] = {
    { "map_set",            PTR(map_set), 3 },
    { "map_get",            PTR(map_get), 2 },
    { "map_keys",           PTR(map_keys), 1 },
    { "map_remove",         PTR(map_remove), 2 },
    { "map_key_exists",     PTR(map_key_exists), 2 },
    { "array_len",          PTR(array_len), 1},
    { "array_sort",         PTR(array_sort), 1},
    { "array_where",        PTR(array_where), 2 },
    { "string_eq",          PTR(string_eq), 2 },
    { "string_eq_n",        PTR(string_eq_n), 3 },
    { "string_cmp",         PTR(string_cmp), 2 },
    { "string_cmp_n",       PTR(string_cmp_n), 3 },
    { "string_advance",     PTR(string_advance), 2 },
    { "string_bytelen",     PTR(string_bytelen), 1 },
    { "string_to_array",    PTR(string_to_array), 1},
    { "string_numchars",    PTR(string_numchars), 1 },
    { "string_to_upper",    PTR(string_to_upper), 1 },
    { "string_to_lower",    PTR(string_to_lower), 1 },
    { "string_tokenize",    PTR(string_tokenize), 2 },
    { "string_to_number",   PTR(string_to_number), 1 },
    { "string_to_integer",  PTR(string_to_integer), 1 },
    { "string_byte_in_str", PTR(string_byte_in_str), 2 },
    { "string_bytes_in_str", PTR(string_bytes_in_str), 2 },
    { "string_advance_on_byte", PTR(string_advance_on_byte), 2},
    { "string_advance_after_bytes", PTR(string_advance_after_bytes), 3},
    { "string_trim_byte_at_end", PTR(string_trim_byte_at_end), 2},
    { "integer_eq",         PTR(integer_eq), 2},
    { "integer_char",       PTR(integer_char), 1 },
    { "integer_to_string",  PTR(integer_to_string), 2},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Map = {
       set : map_set,
       get : map_get,
       keys : map_keys,
       remove : map_remove,
       key_exists : map_key_exists
     };

    public var Array = {
      len   : array_len,
      sort  : array_sort,
      where : array_where
    };

    public var String = {
       eq : string_eq,
       eq_n : string_eq_n,
       cmp : string_cmp,
       cmp_n : string_cmp_n,
       advance : string_advance,
       bytelen : string_bytelen,
       numchars : string_numchars,
       tokenize : string_tokenize,
       to_upper : string_to_upper,
       to_lower : string_to_lower,
       to_array : string_to_array,
       to_number : string_to_number,
       to_integer : string_to_integer,
       byte_in_str : string_byte_in_str,
       bytes_in_str : string_bytes_in_str,
       advance_on_byte : string_advance_on_byte,
       advance_after_bytes : string_advance_after_bytes,
       trim_byte_at_end : string_trim_byte_at_end
     };

     public var Integer = {
       eq        : integer_eq,
       char      : integer_char,
       to_string : integer_to_string
     };

    public const Std = false
 );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_std_module__ (la_t *this) {
  (void) this;
  return;
}
