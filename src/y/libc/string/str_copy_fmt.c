// provides: size_t str_copy_fmt (char *, size_t, const char *, ...)
// requires: stdio/vsnprintf.c
// requires: string/str_copy.c

size_t str_cp_fmt (char *dest, size_t dest_len, const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE(fmt);
  char bytes[len + 1];
  STRING_GET_FORMAT(bytes, len, fmt);
  return str_copy (dest, dest_len, bytes, len);
}
