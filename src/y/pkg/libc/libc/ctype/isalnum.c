// provides: int isalnum (int)
// requires: ctype/isalpha.c
// requires: ctype/isdigit.c

int isalnum (int c) {
  return isalpha (c) || isdigit (c);
}
