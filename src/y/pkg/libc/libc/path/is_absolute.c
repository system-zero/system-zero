// provides: int path_is_absolute (const char *)

int path_is_absolute (const char *path) {
  return IS_DIR_ABS(path);
}
