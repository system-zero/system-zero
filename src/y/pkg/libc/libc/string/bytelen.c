// provides: size_t bytelen (const char *)

size_t bytelen (const char *str) {
  const char *sp = str;
  while (*sp) ++sp;
  return sp - str;
}
