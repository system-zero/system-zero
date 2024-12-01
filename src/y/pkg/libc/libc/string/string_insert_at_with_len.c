// provides: string *string_insert_at_with_len (string *, int, const char *, size_t)
// requires: string/string.h
// requires: string/string_alloc.c
// requires: string/str_byte_copy.c
// requires: string/str_byte_move.c

string *string_insert_at_with_len (string *this,
         int idx, const char *bytes, size_t len) {
  size_t bts = this->num_bytes + len;
  if (bts >= this->mem_size) string_realloc_bytes (this, bts - this->mem_size + 1);

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
