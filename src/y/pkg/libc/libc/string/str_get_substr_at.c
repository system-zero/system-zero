// provides: char *str_get_substr_at (char *, size_t, char *, size_t, size_t)

char *str_get_substr_at (char *dest, size_t len, char *src, size_t src_len, size_t idx) {
  if (src_len < idx + len) {
    dest[0] = '\0';
    return NULL;
  }

  for (size_t i = 0; i < len; i++)
    dest[i] = src[i + idx];

  dest[len] = '\0';
  return dest;
}
