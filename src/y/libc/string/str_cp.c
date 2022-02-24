// provides: size_t str_cp (char *, size_t, const char *, size_t)
// requires: string/str_byte_cp.c

size_t str_cp (char *dest, size_t dest_len, const char *src, size_t nelem) {
  size_t num = (nelem > (dest_len - 1) ? dest_len - 1 : nelem);
  size_t len = (NULL == src ? 0 : str_byte_cp (dest, src, num));
  dest[len] = '\0';
  return len;
}
