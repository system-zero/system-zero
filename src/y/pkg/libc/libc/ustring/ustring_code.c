// provides: utf8 ustring_code_and_len (const char *, int *)
// provides: utf8 ustring_code (const char *)
// requires: ustring/ustring.h

utf8 ustring_code_and_len (const char *buf, int *len) {
  if (NULL is buf or 0 is *buf) return -1;

  int code = 0;
  *len = 0;

  int i = 0;
  do {
    code <<= 6;
    code += (uchar) buf[i++];
    (*len)++;
  } while (buf[i] and IS_UTF8 (buf[i]));

  code -= offsetsFromUTF8[*len - 1];
  return code;
}

utf8 ustring_code (const char *src) {
  int len = 0;
  return ustring_get_code (src, &len);
}
