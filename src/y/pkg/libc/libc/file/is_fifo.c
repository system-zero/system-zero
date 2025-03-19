// as: file_is_fifo
// provides: bool file_is_fifo (const char *)
// requires: sys/stat.c

bool file_is_fifo (const char *fname) {
  struct stat st;
  if (-1 == sys_stat (fname, &st)) return 0;
  return S_ISFIFO (st.st_mode);
}
