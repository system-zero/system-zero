// provides: size_t strlen (const char *)
// provides: #define bytelen strlen

size_t strlen (const char *str) {
  const char *sp = str;
  while (*sp) ++sp;
  return sp - str;
}
