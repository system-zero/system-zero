// provides: size_t utf8_len (const char *)

size_t utf8_len (const char *bytes) {
  char *sp = (char *) bytes;
  size_t len = 0;
  uchar c;

  while ((c = (uchar) *sp)) {
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

  return  len;
}
