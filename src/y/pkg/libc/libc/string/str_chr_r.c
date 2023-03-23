// provides: char *str_chr_r (const char *, int)
// requires: string/str_chr.c

char *str_chr_r (const char *s, int c) {
  const char *sp = str_chr (s, 0);
  if (sp == s) return NULL;
  while (*--sp != c)
    if (sp == s) return NULL;

  return (char *) sp;
}
