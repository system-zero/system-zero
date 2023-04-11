// provides: size_t str_copy_fmt (char *, size_t, const char *, ...)
// requires: string/vsnprintf.c
// requires: string/str_copy.c

size_t str_copy_fmt (char *dest, size_t dest_len, const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE(fmt);
  char bytes[len + 1];
  size_t r = snprintf (bytes, len, fmt, VA_ARGS);
  /* since is a variable length thing, truncation might occurs
   * so we might want to do something about this
   */
  (void) r;
  return str_copy (dest, dest_len, bytes, len);
}
