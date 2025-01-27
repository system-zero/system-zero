// as: filesize
// provides: size_t file_size (const char *)

size_t file_size (const char *fname) {
  struct stat st;
  if (-1 == sys_stat (fname, &st))
    return 0;

  return st.st_size;
}
