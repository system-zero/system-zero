// provides: size_t cstring_cp_fmt (char *, size_t, const char *, ...)
// depends: stdio/vsnprintf.c
// depends: string/str_cp.c

size_t cstring_cp_fmt (char *dest, size_t dest_len, const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE(fmt);
  char bytes[len + 1];
  STRING_GET_FORMAT(bytes, len, fmt);
  return str_cp (dest, dest_len, bytes, len);
}
