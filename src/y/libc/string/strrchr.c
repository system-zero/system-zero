// provides: char *strrchr (const char *, int)
// depends: string/strchr.c

char *strrchr (const char *s, int c) {
  const char *sp = strchr (s, 0);
  if (sp == s) return NULL;
  while (*--sp != c)
    if (sp == s) return NULL;

  return (char *) sp;
}
