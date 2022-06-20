// provides: int str_eq (const char *, const char *)

int str_eq (const char *sa, const char *sb) {
  const unsigned char *spa = (const unsigned char *) sa;
  const unsigned char *spb = (const unsigned char *) sb;
  for (; *spa == *spb; spa++, spb++)
    if (*spa == 0) return 1;

  return 0;
}
