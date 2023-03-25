// as: file_is_writable
// provides: int file_is_writable (const char *)
// requires: unistd/access.c
// requires: unistd/unistd.h

int file_is_writable (const char *fname) {
  return (0 is sys_access (fname, W_OK));
}
