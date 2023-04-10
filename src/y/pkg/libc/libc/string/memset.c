// provides: void *memset (void *, int, size_t)

void *memset (void *s, int c, size_t len) {
  if (len <= 0)
    return s;

  unsigned char *ptr = s;
  while (*ptr && len--)
    *ptr++ = (unsigned char) c;

  return s;
}
