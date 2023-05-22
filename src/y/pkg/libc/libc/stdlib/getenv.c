// provides: char *sys_getenv (const char *)
// requires: string/bytelen.c
// requires: string/str_eq_n.c
// requires: std/environ.h

char *sys_getenv (const char *s) {
  if (NULL == environ || NULL == s) return NULL;

  size_t len = bytelen (s);

  for (int i = 0; environ[i]; i++)
    if ((str_eq_n (environ[i], s, len)) && (environ[i][len] == '='))
      return environ[i] + len + 1;

  return NULL;
}
