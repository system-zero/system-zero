// provides: char *utf8_string_after_nth_char (const char *, int, int *)
// requires: utf8/utf8_charlen.c
// requires: utf8/utf8_width.c

#ifndef DEFAULT_TABWIDTH
#define DEFAULT_TABWIDTH 8
#endif

char *utf8_string_after_nth_char (const char *buf, int nth, int *numchars) {
   *numchars = 0;
  int len;

  char *sp = (char *) buf;

  while (*sp && *numchars < nth) {
    len = utf8_charlen (*sp);
    *numchars += utf8_width (sp, DEFAULT_TABWIDTH);
    sp += len;
  }

  return sp;
}
