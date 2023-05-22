// provides: int str_cmp (const char *, const char *)

int str_cmp (const char *sa, const char *sb) {
  const uchar *spa = (const uchar *) sa;
  const uchar *spb = (const uchar *) sb;

  for (;; spa++, spb++) {
    if (*spa != *spb)
      return (*(uchar *) spa - *(uchar *) spb);

    if (*spa == 0) return 0;
  }

  return 0;
}
