// provides: void string_clear (string *)
// provides: void string_clear_at (string *, int)
// provides: void string_release (string *)
// provides: string *string_new (size_t)
// provides: string *string_dup (string *)
// provides: string *string_trim_end (string *, char)
// provides: string *string_new_with (const char *)
// provides: string *string_new_with_len (const char *, size_t)
// provides: string *string_new_with_fmt (const char *, ...)
// provides: string *string_insert_at_with (string *, int, const char *)
// provides: string *string_insert_at_with_len (string *, int, const char *, size_t)
// provides: string *string_insert_byte_at (string *, char, int)
// provides: string *string_append_byte (string *, char)
// provides: string *string_append_with (string *, const char *)
// provides: string *string_append_with_fmt (string *, const char *, ...)
// provides: string *string_append_with_len (string *, const char *, size_t)
// provides: string *string_prepend_byte (string *, char)
// provides: string *string_prepend_with (string *, const char *)
// provides: string *string_prepend_with_fmt (string *, const char *, ...)
// provides: int string_delete_numbytes_at (string *, int, int)
// provides: string *string_replace_numbytes_at_with (string *, int, int, const char *)
// provides: string *string_replace_with (string *, const char *)
// provides: string *string_replace_with_len (string *, const char *, size_t)
// provides: string *string_replace_with_fmt (string *, const char *, ...)
// provides: string *string_append_utf8 (string *, int code)
// provides: string *string_new_with_allocated (const char *, size_t, size_t)
// requires: string/string.h
// requires: string/bytelen.c
// requires: string/str_copy.c
// requires: string/str_byte_copy.c
// requires: string/str_byte_move.c
// requires: utf8/utf8_character.c
// requires: stdlib/alloc.c
// requires: string/vsnprintf.c

typedef string string_t;

void string_release (string *this) {
  if (this == NULL) return;
  if (this->mem_size) Release (this->bytes);
  Release (this);
}

void string_clear (string *this) {
  if (!this->num_bytes) return;
  this->bytes[0] = '\0';
  this->num_bytes = 0;
}

void string_clear_at (string *this, int idx) {
  if (0 > idx) idx += this->num_bytes;
  if (idx < 0) return;
  if (idx > (int) this->num_bytes) idx = this->num_bytes;
  this->bytes[idx] = '\0';
  this->num_bytes = idx;
}

static inline size_t string_align (size_t size) {
  return ((size + (8 - 1)) & ~(8 - 1));
}

/* this is not like realloc(), as len here is the extra size */
static string *string_reallocate (string *this, size_t len) {
  size_t sz = string_align (this->mem_size + len + 1);
  this->bytes = Realloc (this->bytes, sz);
  this->mem_size = sz;
  return this;
}

string *string_new (size_t size) {
  string *s = Alloc (sizeof (string));
  size_t sz = (size <= 0 ? 8 : string_align (size));
  s->bytes = Alloc (sz);
  s->mem_size = sz;
  s->num_bytes = 0;
  s->bytes[0] = '\0';
  return s;
}

string *string_new_with_len (const char *bytes, size_t len) {
  string *new = Alloc (sizeof (string));
  size_t sz = string_align (len + 1);
  char *buf = Alloc (sz);
  len = str_copy (buf, sz, bytes, len);
  new->bytes = buf;
  new->num_bytes = len;
  new->mem_size = sz;
  return new;
}

string *string_new_with (const char *bytes) {
  size_t len = (NULL == bytes ? 0 : bytelen (bytes));
  return string_new_with_len (bytes, len); /* this succeeds even if bytes is NULL */
}

string *string_new_with_fmt (const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE(fmt);
  char bytes[len + 1];
  va_list ap; va_start(ap, fmt);
  sys_vsnprintf (bytes, len + 1, fmt, ap);
  va_end(ap);

  return string_new_with_len (bytes, len);
}

string *string_insert_at_with_len (string *this,
         int idx, const char *bytes, size_t len) {
  size_t bts = this->num_bytes + len;
  if (bts >= this->mem_size) string_reallocate (this, bts - this->mem_size + 1);

  if (idx == (int) this->num_bytes) {
    str_byte_copy (this->bytes + this->num_bytes, bytes, len);
  } else {
    str_byte_move (this->bytes, this->mem_size - 1, idx + len, idx, this->num_bytes - idx);
    str_byte_copy (this->bytes + idx, bytes, len);
  }

  this->num_bytes += len;
  this->bytes[this->num_bytes] = '\0';
  return this;
}

string *string_insert_at_with (string *this, int idx, const char *bytes) {
  if (0 > idx) idx = this->num_bytes + idx + 1;
  if (idx < 0 or idx > (int) this->num_bytes)
    return this;

  size_t len = bytelen (bytes);
  if (!len) return this;

  return string_insert_at_with_len (this, idx, bytes, len);
}

string *string_append_byte (string *this, char c) {
  int bts = this->mem_size - (this->num_bytes + 2);
  if (1 > bts) string_reallocate (this, 8);
  this->bytes[this->num_bytes++] = c;
  this->bytes[this->num_bytes] = '\0';
  return this;
}

string *string_insert_byte_at (string *this, char c, int idx) {
  char buf[2]; buf[0] = c; buf[1] = '\0';
  return string_insert_at_with_len (this, idx, buf, 1);
}

string *string_prepend_byte (string *this, char c) {
  int bts = this->mem_size - (this->num_bytes + 2);
  if (1 > bts) string_reallocate (this, 8);

  str_byte_move (this->bytes, this->num_bytes + 1, 1, 0, this->num_bytes);

  this->bytes[0] = c;
  this->bytes[++this->num_bytes] = '\0';
  return this;
}

string *string_append_with (string *this, const char *bytes) {
  return string_insert_at_with_len (this, this->num_bytes, bytes, bytelen (bytes));
}

string *string_append_with_len (string *this, const char *bytes,
                                                     size_t len) {
  return string_insert_at_with_len (this, this->num_bytes, bytes, len);
}

string *string_prepend_with (string *this, const char *bytes) {
  return string_insert_at_with (this, 0, bytes);
}

string *string_append_with_fmt (string *this, const char *fmt, ...) {
  size_t size = STRING_FORMAT_SIZE(fmt);
  char bytes[size + 1];
  va_list ap; va_start(ap, fmt);
  sys_vsnprintf (bytes, size + 1, fmt, ap);
  va_end(ap);

  return string_insert_at_with_len (this, this->num_bytes, bytes, size);
}

string *string_prepend_with_fmt (string *this, const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE(fmt);
  char bytes[len + 1];
  va_list ap; va_start(ap, fmt);
  sys_vsnprintf (bytes, len + 1, fmt, ap);
  va_end(ap);

  return string_insert_at_with_len (this, 0, bytes, len);
}

string *string_dup (string *this) {
  return string_new_with_len (this->bytes, this->num_bytes);
}

int string_delete_numbytes_at (string *this, int num, int idx) {
  if (num < 0) return NOTOK;
  if(!num) return OK;

  if (idx < 0 or idx >= (int) this->num_bytes or
      idx + num > (int) this->num_bytes)
    return EINDEX;

  if (idx + num != (int) this->num_bytes)
    str_byte_copy (this->bytes + idx, this->bytes + idx + num,
        this->num_bytes - (idx + num - 1));

  this->num_bytes -= num;
  this->bytes[this->num_bytes] = '\0';
  return OK;
}

string *string_replace_numbytes_at_with (
        string *this, int num, int idx, const char *bytes) {
  if (string_delete_numbytes_at (this, num, idx) != OK)
    return this;
  return string_insert_at_with (this, idx, bytes);
}

string *string_replace_with (string *this, const char *bytes) {
  string_clear (this);
  return string_append_with (this, bytes);
}

string *string_replace_with_len (string *this, const char *bytes,
                                                                  size_t len) {
  string_clear (this);
  return string_insert_at_with_len (this, 0, bytes, len);
}

string *string_replace_with_fmt (string *this, const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE(fmt);
  char bytes[len + 1];
  va_list ap; va_start(ap, fmt);
  sys_vsnprintf (bytes, len + 1, fmt, ap);
  va_end(ap);

  return string_replace_with_len (this, bytes, len);
}

string *string_append_utf8 (string *this, int code) {
  char buf[8];
  int len = utf8_character (code, buf, 8);
  if (len)
    string_append_with_len (this, buf, len);
  return this;
}

string *string_trim_end (string *this, char c) {
  char *sp = this->bytes + this->num_bytes - 1;

  while (1) {
    if (*sp-- != c) break;
    string_clear_at (this, -1);
    if (!this->num_bytes) break;
  }

  return this;
}

string *string_new_with_allocated (const char *allocated, size_t len, size_t size) {
  if (len > size) return NULL;

  string *s = Alloc (sizeof (string));
  s->bytes = (char *) allocated;
  s->num_bytes = len;
  s->mem_size = size;
  s->bytes[len] = '\0';
  return s;
}
