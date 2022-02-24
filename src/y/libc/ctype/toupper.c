// provides: int toupper (int)
// requires: ctype/islower.c

int toupper (int c) {
  if (islower (c))
    return c & 0x5f;

  return c;
}
