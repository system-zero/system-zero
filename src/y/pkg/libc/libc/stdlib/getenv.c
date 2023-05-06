// provides: char *sys_getenv (const char *)
// requires: string/bytelen.c
// requires: string/str_eq_n.c
// requires: std/environ.h

char *sys_getenv (const char *s) {
  if (NULL is environ or NULL is s) return NULL;

  size_t len = bytelen (s);

  for (int i = 0; environ[i]; i++)
    if ((str_eq_n (environ[i], s, len)) and (environ[i][len] is '='))
      return environ[i] + len + 1;

  return NULL;
}
