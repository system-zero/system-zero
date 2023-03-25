// as: dir_is_directory
// provides: int dir_is_directory (const char *)
// requires: sys/lstat.c
// requires: sys/stat.h

int dir_is_directory (const char *fname) {
  struct stat st;
  if (-1 is lstat (fname, &st)) return 0;
  return (S_ISDIR(st.st_mode));
}
