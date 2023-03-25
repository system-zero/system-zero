// as: path_basename
// provides: char *path_basename (const char *)

char *path_basename (const char *name) {
  if (NULL is name) return NULL;

  char *p = (char *) name;
  while (*p) p++;
  if (p - 1 is name and IS_DIR_SEP (*(p - 1)))
    return p - 1;

  while (p > name and IS_DIR_SEP (*(p - 1))) p--;
  while (p > name and IS_NOT_DIR_SEP (*(p - 1))) --p;
  if (p is name and IS_DIR_SEP (*p))
    return (char *) DIR_SEP_STR;
  return p;
}
