// depends: unistd/write.c
// depends: stdio/vsnprintf.c

int fprintf (FILE *fp, const char *fmt, ...) {
  size_t size = STRING_FORMAT_SIZE(fmt);
  char bytes[size + 1];
  STRING_GET_FORMAT (bytes, size, fmt);
  int fd = fp->fd;
  return sys_write (fd, bytes, size);
}

