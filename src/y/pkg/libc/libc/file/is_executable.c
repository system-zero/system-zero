// as: file_is_executable
// provides: bool file_is_executable (const char *)
// requires: unistd/access.c

bool file_is_executable (const char *fname) {
  return (0 == sys_access (fname, F_OK|X_OK));
}
