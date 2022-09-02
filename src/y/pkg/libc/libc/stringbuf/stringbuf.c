// provides: stringbuf *string_append_byte (stringbuf *, char)
// provides: void string_release (stringbuf *)
// provides: void string_clear (stringbuf *)
// provides: void string_clear_at (stringbuf *, int)
// provides: stringbuf *string_new (size_t)
// provides: stringbuf *string_new_with_len (const char *, size_t)
// provides: stringbuf *string_new_with (const char *)
// provides: stringbuf *string_new_with_fmt (const char *, ...)
// provides: stringbuf *string_insert_at_with_len (stringbuf *, int, const char *, size_t)
// provides: stringbuf *string_insert_at_with (stringbuf *, int, const char *)
// provides: stringbuf *string_append_byte (stringbuf *, char)
// provides: stringbuf *string_insert_byte_at (stringbuf *, char, int)
// provides: stringbuf *string_prepend_byte (stringbuf *, char)
// provides: stringbuf *string_append_with (stringbuf *, const char *)
// provides: stringbuf *string_append_with_len (stringbuf *, const char *, size_t)
// provides: stringbuf *string_prepend_with (stringbuf *, const char *)
// provides: stringbuf *string_append_with_fmt (stringbuf *, const char *, ...)
// provides: stringbuf *string_prepend_with_fmt (stringbuf *, const char *, ...)
// provides: stringbuf *string_dup (stringbuf *)
// provides: int string_delete_numbytes_at (stringbuf *, int, int)
// provides: stringbuf *string_replace_numbytes_at_with (stringbuf *, int, int, const char *)
// provides: stringbuf *string_replace_with (stringbuf *, const char *)
// provides: stringbuf *string_replace_with_len (stringbuf *, const char *, size_t)
// provides: stringbuf *string_replace_with_fmt (stringbuf *, const char *, ...)
// requires: stringbuf/stringbuf.h
// requires: string/str_copy.c
// requires: string/str_byte_copy.c
// requires: string/str_byte_move.c

void string_release (stringbuf *this) {
  if (this is NULL) return;
  if (this->mem_size) free (this->bytes);
  free (this);
}

void string_clear (stringbuf *this) {
  ifnot (this->num_bytes) return;
  this->bytes[0] = '\0';
  this->num_bytes = 0;
}

void string_clear_at (stringbuf *this, int idx) {
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
static stringbuf *string_reallocate (stringbuf *this, size_t len) {
  size_t sz = string_align (this->mem_size + len + 1);
  this->bytes = Realloc (this->bytes, sz);
  this->mem_size = sz;
  return this;
}

stringbuf *string_new (size_t len) {
  stringbuf *s = Alloc (sizeof (stringbuf));
  size_t sz = (len <= 0 ? 8 : string_align (len));
  s->bytes = Alloc (sz);
  s->mem_size = sz;
  s->num_bytes = 0;
  s->bytes[0] = '\0';
  return s;
}

stringbuf *string_new_with_len (const char *bytes, size_t len) {
  stringbuf *new = Alloc (sizeof (stringbuf));
  size_t sz = string_align (len + 1);
  char *buf = Alloc (sz);
  len = str_copy (buf, sz, bytes, len);
  new->bytes = buf;
  new->num_bytes = len;
  new->mem_size = sz;
  return new;
}

stringbuf *string_new_with (const char *bytes) {
  size_t len = (NULL is bytes ? 0 : bytelen (bytes));
  return string_new_with_len (bytes, len); /* this succeeds even if bytes is NULL */
}

stringbuf *string_new_with_fmt (const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len+1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return string_new_with_len (bytes, len);
}

stringbuf *string_insert_at_with_len (stringbuf *this,
                     int idx, const char *bytes, size_t len) {
  size_t bts = this->num_bytes + len;
  if (bts >= this->mem_size) string_reallocate (this, bts - this->mem_size + 1);

  if (idx is (int) this->num_bytes) {
    str_byte_copy (this->bytes + this->num_bytes, bytes, len);
  } else {
    str_byte_move (this->bytes, this->mem_size - 1, idx + len, idx, this->num_bytes - idx);
    str_byte_copy (this->bytes + idx, bytes, len);
  }

  this->num_bytes += len;
  this->bytes[this->num_bytes] = '\0';
  return this;
}

stringbuf *string_insert_at_with (stringbuf *this, int idx, const char *bytes) {
  if (0 > idx) idx = this->num_bytes + idx + 1;
  if (idx < 0 or idx > (int) this->num_bytes)
    return this;

  size_t len = bytelen (bytes);
  ifnot (len) return this;

  return string_insert_at_with_len (this, idx, bytes, len);
}

stringbuf *string_append_byte (stringbuf *this, char c) {
  int bts = this->mem_size - (this->num_bytes + 2);
  if (1 > bts) string_reallocate (this, 8);
  this->bytes[this->num_bytes++] = c;
  this->bytes[this->num_bytes] = '\0';
  return this;
}

stringbuf *string_insert_byte_at (stringbuf *this, char c, int idx) {
  char buf[2]; buf[0] = c; buf[1] = '\0';
  return string_insert_at_with_len (this, idx, buf, 1);
}

stringbuf *string_prepend_byte (stringbuf *this, char c) {
  int bts = this->mem_size - (this->num_bytes + 2);
  if (1 > bts) string_reallocate (this, 8);

  str_byte_move (this->bytes, this->num_bytes + 1, 1, 0, this->num_bytes);

  this->bytes[0] = c;
  this->bytes[++this->num_bytes] = '\0';
  return this;
}

stringbuf *string_append_with (stringbuf *this, const char *bytes) {
  return string_insert_at_with_len (this, this->num_bytes, bytes, bytelen (bytes));
}

stringbuf *string_append_with_len (stringbuf *this, const char *bytes,
                                                                 size_t len) {
  return string_insert_at_with_len (this, this->num_bytes, bytes, len);
}

stringbuf *string_prepend_with (stringbuf *this, const char *bytes) {
  return string_insert_at_with (this, 0, bytes);
}

stringbuf *string_append_with_fmt (stringbuf *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return string_insert_at_with_len (this, this->num_bytes, bytes, len);
}

stringbuf *string_prepend_with_fmt (stringbuf *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return string_insert_at_with_len (this, 0, bytes, len);
}

stringbuf *string_dup (stringbuf *this) {
  return string_new_with_len (this->bytes, this->num_bytes);
}

int string_delete_numbytes_at (stringbuf *this, int num, int idx) {
  if (num < 0) return NOTOK;
  ifnot (num) return OK;

  if (idx < 0 or idx >= (int) this->num_bytes or
      idx + num > (int) this->num_bytes)
    return EINDEX;

  if (idx + num isnot (int) this->num_bytes)
    str_byte_copy (this->bytes + idx, this->bytes + idx + num,
        this->num_bytes - (idx + num - 1));

  this->num_bytes -= num;
  this->bytes[this->num_bytes] = '\0';
  return OK;
}

stringbuf *string_replace_numbytes_at_with (
        stringbuf *this, int num, int idx, const char *bytes) {
  if (string_delete_numbytes_at (this, num, idx) isnot OK)
    return this;
  return string_insert_at_with (this, idx, bytes);
}

stringbuf *string_replace_with (stringbuf *this, const char *bytes) {
  string_clear (this);
  return string_append_with (this, bytes);
}

stringbuf *string_replace_with_len (stringbuf *this, const char *bytes,
                                                                  size_t len) {
  string_clear (this);
  return string_insert_at_with_len (this, 0, bytes, len);
}

stringbuf *string_replace_with_fmt (stringbuf *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  return string_replace_with_len (this, bytes, len);
}
