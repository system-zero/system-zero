// provides: bool str_eq_n (const char *, const char *, size_t)
// requires: string/str_cmp_n.c

bool str_eq_n (const char *sa, const char *sb, size_t n) {
  return (str_cmp_n (sa, sb, n) == 0);
}
