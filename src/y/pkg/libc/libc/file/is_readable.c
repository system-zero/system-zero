// provides: int file_is_readable (const char *)
// requires: unistd/access.c

int file_is_readable (const char *fname) {
  return (0 == sys_access (fname, R_OK));
}
