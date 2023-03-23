// provides: char *str_chr (const char *, int)

char *str_chr (const char *s, int c) {
  const char *sp = s;
  while (*sp != c) {
    if (*sp == 0) return NULL;
    sp++;
  }

  return (char *) sp;
}
