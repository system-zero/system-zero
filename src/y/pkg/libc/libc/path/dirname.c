// provides: char *path_dirname (const char *)
// requires: stdlib/alloc.c
// requires: string/bytelen.c
// requires: string/str_copy.c

char *path_dirname (const char *name) {
  char *dname = NULL;

  if (NULL == name)
    goto return_dot;

  size_t len = bytelen (name);

  if (0 == len) {
return_dot:
    dname = Alloc (2);
    dname[0] = '.'; dname[1] = '\0';
    return dname;
  }

  char *sep = (char *) name + len - 1;

  /* trailing slashes */
  while (sep != name) {
    if (IS_NOT_DIR_SEP (*sep))
      break;

    sep--;
  }

  /* first found */
  while (sep != name) {
    if (IS_DIR_SEP (*sep))
      break;
    sep--;
  }

  if (sep - 1 == name)
    goto theend;

  /* trim again */
  while (sep != name) {
    if (IS_NOT_DIR_SEP (*sep))
      break;
    sep--;
  }

  if (sep == name) {
    dname = Alloc (2);
    dname[0] = (IS_DIR_SEP (*name)) ? DIR_SEP : '.';
    dname[1] = '\0';
    return dname;
  }

theend:
  len = sep - name + 1;
  dname = Alloc (len + 1);
  str_copy (dname, len + 1, name, len);

  return dname;
}
