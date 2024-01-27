// provides: void *mem_set (void *, int, size_t)
// provides: void *mem_set_all (void *, int, size_t)

void *mem_set (void *s, int c, size_t len) {
  if (len == 0 || s == NULL)
    return s;

  unsigned char *ptr = s;
  while (*ptr && len--)
    *ptr++ = (unsigned char) c;

  return s;
}

void *mem_set_all (void *s, int c, size_t len) {
  if (len == 0 || s == NULL)
    return s;

  unsigned char *ptr = s;
  while (len--)
    *ptr++ = (unsigned char) c;

  return s;
}
