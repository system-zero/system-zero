// provides: size_t str_copy_fmt (char *, size_t, const char *, ...)
// requires: string/vsnprintf.c
// requires: string/str_copy.c

size_t str_copy_fmt (char *dest, size_t dest_len, const char *fmt, ...) {
  int len = STRING_FORMAT_SIZE(fmt);
  char bytes[len + 1];

  va_list ap;
  va_start(ap, fmt);
  int r = sys_vsnprintf (bytes, len + 1, fmt, ap);
  va_end(ap);

  /* since is a variable length thing, truncation might occurs
   * so we might want to do something about this
   */
  if (r isnot len) len = r;

  return str_copy (dest, dest_len, bytes, len);
}
