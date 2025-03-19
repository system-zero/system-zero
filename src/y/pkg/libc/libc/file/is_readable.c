// as: file_is_readable
// provides: bool file_is_readable (const char *)
// requires: unistd/access.c

bool file_is_readable (const char *fname) {
  return (0 == sys_access (fname, R_OK));
}
