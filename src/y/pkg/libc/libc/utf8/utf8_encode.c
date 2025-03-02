// provides: utf8_char *utf8_encode (utf8_char *, const char *, int)
// requires: utf8/utf8.h
// requires: utf8/utf8_width.c

utf8_char *utf8_encode (utf8_char *chr, const char *bytes, int tabwidth) {
  char *sp = (char *) bytes;
  uchar c = (uchar) *sp;

  if (0 == c) {
    chr->len = chr->code = chr->width = 0;
    chr->buf[0] = '\0';
    return chr;
  }

  chr->code = c;
  chr->width = chr->len = 1;
  chr->buf[0] = *sp;

  if (c < 0x80) {
    if (chr->code == '\t')
      chr->width += (tabwidth - 1);
    chr->buf[1] = '\0';
    return chr;
  }

  chr->buf[1] = *++sp;
  chr->len++;
  chr->code <<= 6;
  chr->code += (uchar) *sp;

  if ((c & 0xe0) == 0xc0) {
    chr->code -= offsetsFromUTF8[1];
    chr->width += (utf8_char_width (chr->code) - 1);
    chr->buf[2] = '\0';
    return chr;
  }

  chr->buf[2] = *++sp;
  chr->len++;
  chr->code <<= 6;
  chr->code += (uchar) *sp;

  if ((c & 0xf0) == 0xe0) {
    chr->code -= offsetsFromUTF8[2];
    chr->width += (utf8_char_width (chr->code) - 1);
    chr->buf[3] = '\0';
    return chr;
  }

  chr->buf[3] = *++sp;
  chr->buf[4] = '\0';
  chr->len++;
  chr->code <<= 6;
  chr->code += (uchar) *sp;
  chr->code -= offsetsFromUTF8[3];
  chr->width += (utf8_char_width (chr->code) - 1);

  return chr;
}
