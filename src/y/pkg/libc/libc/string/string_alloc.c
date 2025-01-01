// provides: string *string_alloc_bytes (string *, size_t)
// provides: string *string_realloc_bytes (string *, size_t)
// requires: string/string.h

/* this is handled by Alloc */
//#if 0
static inline size_t string_align (size_t size) {
  return ((size + (8 - 1)) & ~(8 - 1));
}
//#endif

string *string_alloc_bytes (string *this, size_t size) {
  // size_t sz = (size == 0 ? MIN_STRING_SIZE : string_align (size));
  size_t sz = (size == 0 ? MIN_STRING_SIZE : size);
  this->bytes = Alloc (sz);
  // use extra space (usually it is the case)
  this->mem_size = mem_get_actual_size (this->bytes);
  //this->mem_size = sz;
  this->num_bytes = 0;
  return this;
}

/* this is not like realloc(), as size here is the extra size */
string *string_realloc_bytes (string *this, size_t size) {
  //size_t sz = string_align (this->mem_size + size + 1);
  size_t sz = this->mem_size + size + 1;
  this->bytes = Realloc (this->bytes, sz);
  // likewise with alloc
  this->mem_size = mem_get_actual_size (this->bytes);
  //this->mem_size = sz;
  return this;
}
