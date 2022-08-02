// provides: int hexstring_to_string (unsigned char *, size_t, const char *, size_t)

static int hval (int c) {
  switch (c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
  }

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
  if (l & 1) {
    *(dest++) = hval (*hex);

    hex++;
    l++;
  }

  while (*hex) {
    c = hval (*hex);
    hex++;
    c = c << 4;
    c |= hval(*hex);
    hex++;
    *dest = c;
    dest++;
  }

  *dest = '\0';
  return l / 2;
}
