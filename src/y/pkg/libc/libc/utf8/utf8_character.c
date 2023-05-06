// provides: int utf8_character (utf8, char *, size_t)

int utf8_character (utf8 c, char *buf, size_t bufsize) {
  if (bufsize < 5)
    return -1;

  if (!c) {
    buf[0] = '\0';
    return 0;
  }

  int len = 1;

  if (c < 0x80) {
    buf[0] = (char) c;
  } else if (c < 0x800) {
    buf[0] = (c >> 6) | 0xC0;
    buf[1] = (c & 0x3F) | 0x80;
    len++;
  } else if (c < 0x10000) {
    buf[0] = (c >> 12) | 0xE0;
    buf[1] = ((c >> 6) & 0x3F) | 0x80;
    buf[2] = (c & 0x3F) | 0x80;
    len += 2;
  } else if (c < 0x110000) {
    buf[0] = (c >> 18) | 0xF0;
    buf[1] = ((c >> 12) & 0x3F) | 0x80;
    buf[2] = ((c >> 6) & 0x3F) | 0x80;
    buf[3] = (c & 0x3F) | 0x80;
    len += 3;
  } else {
    return -1;
  }

  buf[len] = '\0';
  return len;
}
