// provides: int str_eq_n (const char *, const char *, size_t)
// requires: string/strncmp.c

int str_eq_n (const char *sa, const char *sb, size_t n) {
  return (strncmp (sa, sb, n) == 0);
}
// end

#if 0
#ifdef str_eq_n
#ifndef STR_EQ_N_HDR
#define STR_EQ_N_HDR
#undef str_eq_n

#define str_eq_n(__a__, __b__, __s__) \
  (0 == strncmp (__a__, __b__, __s__))
#endif
#endif
#endif
