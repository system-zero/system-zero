// as: path_basename_sans_extname
// provides: char *path_basename_sans_extname (char *)
// requires: path/basename.c
// requires: stdlib/alloc.c
// requires: string/str_copy.c

char *path_basename_sans_extname (char *name) {
  char *bsnm = path_basename (name);

  if (NULL == bsnm) return NULL;

  char *sp = bsnm;
  int found = 0;

  do {
    if (0 == *sp) break;

    while (*(sp + 1)) sp++;

    while (sp > bsnm) {
      if (*sp == '.') {
        found = 1;
        sp--;
        break;
      }

      sp--;
    }

  } while (0);

  size_t len = 0;

  if (0 == found)
    len = bsnm - name;
  else
    len = sp - bsnm + (*sp != '.');

  char *buf = Alloc (len + 1);
  str_copy (buf, len + 1, bsnm, len);
  return buf;
}
