// provides: void string_release_bytes (string *)
// requires: stdlib/alloc.c
// requires: string/string.h

void string_release_bytes (string *this) {
  if (this->mem_size)
    Release (this->bytes);
  this->mem_size = this->num_bytes = 0;
}
