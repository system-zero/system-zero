// provides: utf8 utf8_code_and_len (const char *, int *)
// provides: utf8 utf8_code (const char *)
// requires: utf8/utf8.h

utf8 utf8_code_and_len (const char *buf, int *len) {
  if (NULL == buf) return -1;

  int code = 0;
  *len = 0;

  int n = 0;

  do {
    code <<= 6;
    code += (uchar) buf[n++];
    (*len)++;
  } while (buf[n] && IS_UTF8 (buf[n]));

  code -= offsetsFromUTF8[*len - 1];
  return code;
}

utf8 utf8_code (const char *src) {
  int len = 0;
  return utf8_code_and_len (src, &len);
}
