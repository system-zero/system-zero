// provides: size_t strlen (const char *)

size_t strlen (const char *str) {
  const char *sp = str;
  while (*sp) ++sp;
  return sp - str;
}

#ifndef bytelen
#define bytelen strlen
#endif
