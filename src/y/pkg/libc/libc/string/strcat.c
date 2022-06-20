// provides: size_t str_cat (char *, size_t, const char *)
// provides: #define strcat str_cat
// requires: string/strchr.c

size_t str_cat (char *dest, size_t dest_sz, const char *src) {
  char *sp = strchr (dest, 0);
  size_t len = sp - dest;
  size_t i = 0;

  // do not change it for *src - it is confirmed that doesn't provide the expected
  while (src[i] && i + len < dest_sz - 1) *sp++ = src[i++];
  *sp = '\0';
  return len + i;
}
