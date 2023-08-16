// provides: char *str_new_with_fmt (const char *, ...)
// requires: string/vsnprintf.c
// requires: stdlib/alloc.c

char *str_new_with_fmt (const char *fmt, ...) {
  size_t len = STRING_FORMAT_SIZE(fmt);
  char *bytes =  Alloc (len + 1);
  va_list ap; va_start(ap, fmt);
  sys_vsnprintf (bytes, len + 1, fmt, ap);
  va_end(ap);

  return bytes;
}
