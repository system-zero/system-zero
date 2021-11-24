#define LIBRARY "String"

#define REQUIRE_STDIO
#define REQUIRE_STDARG

#define REQUIRE_STRING_TYPE DONOT_DECLARE

#include <z/cenv.h>

static size_t byte_cp (char *dest, const char *src, size_t nelem) {
  const char *sp = src;
  size_t len = 0;

  while (len < nelem and *sp) { // this differs in memcpy()
    dest[len] = *sp++;
    len++;
  }

  return len;
}

static size_t cstring_cp (char *dest, size_t dest_len, const char *src, size_t nelem) {
  size_t num = (nelem > (dest_len - 1) ? dest_len - 1 : nelem);
  size_t len = (NULL is src ? 0 : byte_cp (dest, src, num));
  dest[len] = '\0';
  return len;
}

static size_t cstring_byte_mv (char *str, size_t len, size_t to_idx,
                                   size_t from_idx, size_t nelem) {
  if (from_idx is to_idx) return 0;
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

static void string_release (string_t *this) {
  if (this is NULL) return;
  if (this->mem_size) free (this->bytes);
  free (this);
}

static void string_clear (string_t *this) {
  ifnot (this->num_bytes) return;
  this->bytes[0] = '\0';
  this->num_bytes = 0;
}

static void string_clear_at (string_t *this, int idx) {
  if (0 > idx) idx += this->num_bytes;
  if (idx < 0) return;
  if (idx > (int) this->num_bytes) idx = this->num_bytes;
  this->bytes[idx] = '\0';
  this->num_bytes = idx;
}

static size_t string_align (size_t size) {
  size_t sz = 8 - (size % 8);
  sz = sizeof (char) * (size + (sz < 8 ? sz : 0));
  return sz;
}

/* this is not like realloc(), as len here is the extra size */
static string_t *string_reallocate (string_t *this, size_t len) {
  size_t sz = string_align (this->mem_size + len + 1);
  this->bytes = Realloc (this->bytes, sz);
  this->mem_size = sz;
  return this;
}

static string_t *string_new (size_t len) {
  string_t *s = Alloc (sizeof (string_t));
  size_t sz = (len <= 0 ? 8 : string_align (len));
  s->bytes = Alloc (sz);
  s->mem_size = sz;
  s->num_bytes = 0;
  s->bytes[0] = '\0';
  return s;
}

static string_t *string_new_with_len (const char *bytes, size_t len) {
  string_t *new = Alloc (sizeof (string_t));
  size_t sz = string_align (len + 1);
  char *buf = Alloc (sz);
  len = cstring_cp (buf, sz, bytes, len);
  new->bytes = buf;
  new->num_bytes = len;
  new->mem_size = sz;
  return new;
}

static string_t *string_new_with (const char *bytes) {
  size_t len = (NULL is bytes ? 0 : bytelen (bytes));
  return string_new_with_len (bytes, len); /* this succeeds even if bytes is NULL */
}

static string_t *string_new_with_fmt (const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len+1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return string_new_with_len (bytes, len);
}

static string_t *string_insert_at_with_len (string_t *this,
                     int idx, const char *bytes, size_t len) {
  size_t bts = this->num_bytes + len;
  if (bts >= this->mem_size) string_reallocate (this, bts - this->mem_size + 1);

  if (idx is (int) this->num_bytes) {
    byte_cp (this->bytes + this->num_bytes, bytes, len);
  } else {
    cstring_byte_mv (this->bytes, this->mem_size - 1, idx + len, idx, this->num_bytes - idx);
    byte_cp (this->bytes + idx, bytes, len);
  }

  this->num_bytes += len;
  this->bytes[this->num_bytes] = '\0';
  return this;
}

static string_t *string_insert_at_with (string_t *this, int idx, const char *bytes) {
  if (0 > idx) idx = this->num_bytes + idx + 1;
  if (idx < 0 or idx > (int) this->num_bytes) {
    //tostderr (STR_FMT ("this->bytes:\n%s\nlen: %zd\n(argument) index: %d\n",
    //     this->bytes, this->num_bytes, idx));
    return this;
  }

  size_t len = bytelen (bytes);
  ifnot (len) return this;

  return string_insert_at_with_len (this, idx, bytes, len);
}

static string_t *string_append_byte (string_t *this, char c) {
  int bts = this->mem_size - (this->num_bytes + 2);
  if (1 > bts) string_reallocate (this, 8);
  this->bytes[this->num_bytes++] = c;
  this->bytes[this->num_bytes] = '\0';
  return this;
}

static string_t *string_insert_byte_at (string_t *this, char c, int idx) {
  char buf[2]; buf[0] = c; buf[1] = '\0';
  return string_insert_at_with_len (this, idx, buf, 1);
}

static string_t *string_prepend_byte (string_t *this, char c) {
  int bts = this->mem_size - (this->num_bytes + 2);
  if (1 > bts) string_reallocate (this, 8);

  cstring_byte_mv (this->bytes, this->num_bytes + 1, 1, 0, this->num_bytes);

  this->bytes[0] = c;
  this->bytes[++this->num_bytes] = '\0';
  return this;
}

static string_t *string_append_with (string_t *this, const char *bytes) {
  return string_insert_at_with_len (this, this->num_bytes, bytes, bytelen (bytes));
}

static string_t *string_append_with_len (string_t *this, const char *bytes,
                                                                 size_t len) {
  return string_insert_at_with_len (this, this->num_bytes, bytes, len);
}

static string_t *string_prepend_with (string_t *this, const char *bytes) {
  return string_insert_at_with (this, 0, bytes);
}

static string_t *string_append_with_fmt (string_t *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return string_insert_at_with_len (this, this->num_bytes, bytes, len);
}

static string_t *string_prepend_with_fmt (string_t *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return string_insert_at_with_len (this, 0, bytes, len);
}

static string_t *string_dup (string_t *this) {
  return string_new_with_len (this->bytes, this->num_bytes);
}

static int string_delete_numbytes_at (string_t *this, int num, int idx) {
  if (num < 0) return NOTOK;
  ifnot (num) return OK;

  if (idx < 0 or idx >= (int) this->num_bytes or
      idx + num > (int) this->num_bytes)
    return EINDEX;

  if (idx + num isnot (int) this->num_bytes)
    byte_cp (this->bytes + idx, this->bytes + idx + num,
        this->num_bytes - (idx + num - 1));

  this->num_bytes -= num;
  this->bytes[this->num_bytes] = '\0';
  return OK;
}

static string_t *string_replace_numbytes_at_with (
        string_t *this, int num, int idx, const char *bytes) {
  if (string_delete_numbytes_at (this, num, idx) isnot OK)
    return this;
  return string_insert_at_with (this, idx, bytes);
}

static string_t *string_replace_with (string_t *this, char *bytes) {
  string_clear (this);
  return string_append_with (this, bytes);
}

static string_t *string_replace_with_len (string_t *this, const char *bytes,
                                                                  size_t len) {
  string_clear (this);
  return string_insert_at_with_len (this, 0, bytes, len);
}

static string_t *string_replace_with_fmt (string_t *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return string_replace_with_len (this, bytes, len);
}

static string_t *string_trim_end (string_t *this, char c) {
  char *sp = this->bytes + this->num_bytes - 1;

  while (1) {
    if (*sp-- isnot c) break;
    string_clear_at (this, -1);
    ifnot (this->num_bytes) break;
  }

  return this;
}

static string_t *string_new_with_allocated (const char *allocated, size_t len) {
  string_t *s = Alloc (sizeof (string_t));
  s->bytes = (char *) allocated;
  s->num_bytes = len;
  s->mem_size = len;
  s->bytes[len] = '\0';
  return s;
}

public string_T __init_string__ (void) {
  return (string_T) {
    .self = (string_self) {
      .new = string_new,
      .dup = string_dup,
      .clear = string_clear,
      .clear_at = string_clear_at,
      .release = string_release,
      .new_with = string_new_with,
      .trim_end = string_trim_end,
      .reallocate = string_reallocate,
      .new_with_len = string_new_with_len,
      .new_with_fmt = string_new_with_fmt,
      .insert_at_with = string_insert_at_with,
      .insert_at_with_len  = string_insert_at_with_len,
      .insert_byte_at = string_insert_byte_at,
      .append_with = string_append_with,
      .append_with_fmt = string_append_with_fmt,
      .append_with_len = string_append_with_len,
      .append_byte = string_append_byte,
      .prepend_with = string_prepend_with,
      .prepend_with_fmt = string_prepend_with_fmt,
      .prepend_byte = string_prepend_byte,
      .delete_numbytes_at = string_delete_numbytes_at,
      .replace_numbytes_at_with = string_replace_numbytes_at_with,
      .replace_with = string_replace_with,
      .replace_with_len = string_replace_with_len,
      .replace_with_fmt = string_replace_with_fmt,
      .new_with_allocated = string_new_with_allocated
    }
  };
}
