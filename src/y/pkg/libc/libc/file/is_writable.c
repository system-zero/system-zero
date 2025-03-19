// as: file_is_writable
// provides: bool file_is_writable (const char *)
// requires: unistd/access.c
// requires: unistd/unistd.h

bool file_is_writable (const char *fname) {
  return (0 == sys_access (fname, W_OK));
}
