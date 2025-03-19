// as: file_is_reg
// provides: bool file_is_reg (const char *)
// requires: sys/stat.c

bool file_is_reg (const char *fname) {
  struct stat st;
  if (-1 == sys_stat (fname, &st))
    return -1;

  return S_ISREG (st.st_mode);
}
