// as: file_is_rwx
// provides: bool file_is_rwx (const char *)
// requires: unistd/access.c

bool file_is_rwx (const char *fname) {
  return (0 == sys_access (fname, R_OK|W_OK|X_OK));
}
