// as: file_exists
// provides: int file_exists (const char *)
// requires: sys/stat.h
// requires: sys/lstat.c

int file_exists (const char *file) {
  struct stat st;
  if (-1 is lstat (file, &st)) return 0;
  return 1;
}
