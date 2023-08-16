// provides: char *str_new_with_len (const char *, size_t)
// provides: char *str_new_with (const char *)
// requires: stdlib/alloc.c
// requires: string/str_copy.c
// requires: string/bytelen.c

char *str_new_with_len (const char *source, size_t len) {
  char *new = Alloc (len + 1);
  str_copy (new, len + 1, source, len);
  return new;
}

char *str_new_with (const char *source) {
  return str_new_with_len (source, bytelen (source));
}
