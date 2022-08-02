// provides: int string_to_hexstring (char *, size_t, const char *, size_t)

/* a bit modified raw2hex() from https://github.com/rofl0r/libulz */
int string_to_hexstring (char *dest, size_t dest_len, const char *src, size_t src_len) {
  if (dest_len < (src_len * 2) + 1) {
    sys_errno = ENOTENOUGHSPACE;
    return -1;
  }

  const char *sp = src;
  static const char hextab[] = "0123456789abcdef";

  size_t idx = 0;
  while (*sp && src_len--) {
    dest[idx++] = hextab[0xf & (*sp >> 4)];
    dest[idx++] = hextab[0xf & *sp];
    sp++;
  }

  dest[idx] = '\0';
  return idx;
}
