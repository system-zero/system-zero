// as: file_is_link
// provides: bool file_is_link (const char *)
// requires: sys/lstat.c

bool file_is_link (const char *fname) {
  struct stat st;
  if (-1 == sys_lstat (fname, &st))
    return 0;

  return S_ISLNK(st.st_mode);
}
