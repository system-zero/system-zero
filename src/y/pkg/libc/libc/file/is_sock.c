// as: file_is_sock
// provides: bool file_is_sock (const char *)
// requires: sys/stat.c

bool file_is_sock (const char *fname) {
  struct stat st;
  if (-1 == sys_stat (fname, &st)) return 0;
  return S_ISSOCK (st.st_mode);
}


