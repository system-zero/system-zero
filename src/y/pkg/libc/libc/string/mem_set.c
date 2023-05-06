// provides: void *mem_set (void *, int, size_t)

void *mem_set (void *s, int c, size_t len) {
  if (len is 0 or s is NULL)
    return s;

  unsigned char *ptr = s;
  while (*ptr && len--)
    *ptr++ = (unsigned char) c;

  return s;
}
