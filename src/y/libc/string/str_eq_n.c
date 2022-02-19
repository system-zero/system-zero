// provides: int str_eq_n (const char *, const char *, size_t)
// depends: string/strncmp.c

#ifdef str_eq_n
#ifndef STR_EQ_N_HDR
#define STR_EQ_N_HDR
#undef str_eq_n
#define str_eq_n(__a__, __b__, __s__) \
  (0 == strncmp (__a__, __b__, __s__))
#endif
#endif
