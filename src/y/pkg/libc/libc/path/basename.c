// provides: char *path_basename (const char *)
// comment: the resulted string is not allocated, as it is part of the given string argument

char *path_basename (const char *name) {
  if (NULL == name) return NULL;

  char *p = (char *) name;

  while (*p) p++;

  if (p - 1 == name && IS_DIR_SEP (*(p - 1)))
    return p - 1;

  while (p > name && IS_DIR_SEP (*(p - 1))) p--;

  while (p > name && IS_NOT_DIR_SEP (*(p - 1))) --p;

  if (p == name && IS_DIR_SEP (*p))
    return (char *) DIR_SEP_STR;

  return p;
}
