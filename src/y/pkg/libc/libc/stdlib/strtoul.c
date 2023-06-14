// provides: unsigned long str_to_unsigned_long (const char *, char **, int)
// requires: ctype/isspace.c
// requires: ctype/isalpha.c
// requires: ctype/isdigit.c
// requires: ctype/isupper.c

unsigned long str_to_unsigned_long (const char *nptr, char **endptr, int base) {
  const char *s;
  unsigned long acc, cutoff;
  int c;
  int neg = 0, any, cutlim;

  s = nptr;

  do {
    c = (unsigned char) *s++;
  } while (isspace (c));

  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+')
    c = *s++;

  if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }

  if (base == 0)
    base = c == '0' ? 8 : 10;

  cutoff = (unsigned long) ULONG_MAX / (unsigned long) base;
  cutlim = (unsigned long) ULONG_MAX % (unsigned long) base;

  for (acc = 0, any = 0; ; c = (unsigned char) *s++) {
    if (isdigit (c))
      c -= '0';
    else if (isalpha (c))
      c -= isupper (c) ? 'A' - 10 : 'a' - 10;
    else
      break;

    if (c >= base)
      break;

    if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }

  if (any < 0) {
    acc = ULONG_MAX;
    sys_errno = ERANGE;
  } else if (neg)
    acc = -acc;

  if (endptr != 0)
    *endptr = (char *) (any ? s - 1 : nptr);

  return acc;
}
