// provides: string *string_append_with_len (string *, const char *, size_t)
// requires: string/string.h
// requires: string/string_insert_at_with_len.c

string *string_append_with_len (string *this, const char *bytes, size_t len) {
  return string_insert_at_with_len (this, this->num_bytes, bytes, len);
}
