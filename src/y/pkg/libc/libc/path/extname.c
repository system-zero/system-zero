// as: path_extname
// provides: char *path_extname (char *)
// comment: the resulted string is not allocated, as it is part of the given string argument

char *path_extname (char *name) {
  if (NULL == name) return name;

  char *sp = name;

  while (*sp) sp++;

  while (sp > name && (*(sp - 1) != '.')) --sp;

  if (sp == name)
    return (char *) "";

  sp--;

  return sp;
}
