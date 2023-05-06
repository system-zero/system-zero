// provides: int utf8_get_nth_character (const char *, char *, size_t,  int)
// requires: utf8/utf8_character.c

int utf8_get_nth_character (const char *bytes, char *buf, size_t bufsize,  int n) {
  if (bufsize < 5) return -1;

  size_t len = 0;
  char *sp = (char *) bytes;
  uchar c;

  while ((c = (uchar) *sp)) && n < len) {
    if (c < 0x80)
      sp++;
    else if ((c & 0xe0) == 0xc0)
      sp += 2;
    else if ((c & 0xf0) == 0xe0)
      sp += 3;
    else
      sp += 4;

    len++;
  }

  if (n > len) {
    buf[0] = '\0';
    return 0;
  }

  return utf8_character (sp, buf, bufsize);
}
