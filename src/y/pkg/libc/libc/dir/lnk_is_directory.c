// provides: int lnk_is_directory (const char *)
// requires: sys/stat.c
// requires: sys/stat.h

int lnk_is_directory (const char *dname) {
  struct stat st;
  if (-1 == stat (dname, &st)) return 0;
  return S_ISDIR (st.st_mode);
}
