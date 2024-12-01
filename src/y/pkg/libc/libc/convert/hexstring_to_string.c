// provides: int hexstring_to_string (unsigned char *, size_t, const char *, size_t)

static int hval (int c) {
  switch (c) {
    case '0' ... '9':
      return c - '0';

    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
  }

  sys_errno = EINVAL;

  return -1;
}

/* a bit modified hex2raw() from https://github.com/rofl0r/libulz */
int hexstring_to_string (unsigned char *dest, size_t dest_len, const char *hex, size_t src_len) {
  if (dest_len < (src_len / 2) + 1) {
    sys_errno = ENOTENOUGHSPACE;
    return -1;
  }

  int l = (int) src_len;

  int c;
  int h;

  if (l & 1) {
    c = hval (*hex);
    if (-1 == c)
      return -1;

    *(dest++) = c;

    l++;
  }

  while (*hex) {
    c = hval (*(hex++));
    if (-1 == c)
      return -1;

    c = c << 4;
    h = hval(*(hex++));
    if (-1 == h)
      return -1;

    c |= h;
    *(dest++) = c;
  }

  *dest = '\0';
  return l / 2;
}
