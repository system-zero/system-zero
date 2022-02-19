// provides: size_t str_byte_cp (char *, const char *, size_t)

size_t str_byte_cp (char *dest, const char *src, size_t nelem) {
  const char *sp = src;
  size_t len = 0;

  while (len < nelem && *sp) {
    dest[len] = *sp++;
    len++;
  }

  return len;
}
