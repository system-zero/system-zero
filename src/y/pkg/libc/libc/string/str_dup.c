// provides: char *str_dup (const char *src, size_t len)
// requires: string/str_new.c

char *str_dup (const char *src, size_t len) {
  return str_new_with_len (src, len);
}
