// provides: char *getenv (const char *)
// depends: string/strlen.c
// depends: string/str_eq_n.c
// depends: std/NULL.h
// depends: std/environ.h

char *getenv (const char *s) {
  if (!environ || !s) return NULL;

  size_t len = strlen(s);

  for (int i = 0; environ[i]; i++)
    if ((str_eq_n (environ[i], s, len)) && (environ[i][len] == '='))
      return environ[i] + len + 1;

  return NULL;
}
