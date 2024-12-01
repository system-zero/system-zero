// provides: string *string_alloc_bytes (string *, size_t)
// provides: string *string_realloc_bytes (string *, size_t)
// requires: string/string.h

static inline size_t string_align (size_t size) {
  return ((size + (8 - 1)) & ~(8 - 1));
}

string *string_alloc_bytes (string *this, size_t size) {
  size_t sz = (size <= 0 ? MIN_STRING_SIZE : string_align (size));
  this->bytes = Alloc (sz);
  this->mem_size = sz;
  this->num_bytes = 0;
  return this;
}

/* this is not like realloc(), as len here is the extra size */
string *string_realloc_bytes (string *this, size_t len) {
  size_t sz = string_align (this->mem_size + len + 1);
  this->bytes = Realloc (this->bytes, sz);
  this->mem_size = sz;
  return this;
}
