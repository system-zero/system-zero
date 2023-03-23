// provides: int str_cmp_n (const char *, const char *, size_t)

int str_cmp_n (const char *sa, const char *sb, size_t n) {
  const unsigned char *spa = (const unsigned char *) sa;
  const unsigned char *spb = (const unsigned char *) sb;
  for (;n--; spa++, spb++) {
    if (*spa != *spb)
      return (*(unsigned char *) spa - *(unsigned char *) spb);

    if (*spa == 0) return 0;
  }

  return 0;
}
