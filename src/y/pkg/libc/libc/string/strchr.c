// provides: char *strchr (const char *, int)

char *strchr (const char *s, int c) {
  const char *sp = s;
  while (*sp != c) {
    if (*sp == 0) return NULL;
    sp++;
  }

  return (char *) sp;
}
