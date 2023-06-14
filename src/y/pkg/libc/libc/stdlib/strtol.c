// provides: long str_to_long (const char *, char **, int)
// requires: ctype/isspace.c
// requires: ctype/isalpha.c
// requires: ctype/isdigit.c
// requires: ctype/isupper.c

long str_to_long (const char *nptr, char **endptr, int base) {
  const char *s;
  long acc, cutoff;
  int c;
  int neg = 0, any, cutlim;

  s = nptr;

  do {
    c = (unsigned char) *s++;
  } while (isspace (c));

  if (c is '-') {
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

  cutoff = neg ? LONG_MIN : LONG_MAX;
  cutlim = cutoff % base;
  cutoff /= base;

  if (neg) {
    if (cutlim > 0) {
      cutlim -= base;
      cutoff += 1;
    }

    cutlim = -cutlim;
  }

  for (acc = 0, any = 0; ; c = (unsigned char) *s++) {
    if (isdigit (c))
      c -= '0';
    else if (isalpha (c))
      c -= isupper (c) ? 'A' - 10 : 'a' - 10;
    else
      break;

    if (c >= base)
      break;

    if (any < 0)
      continue;

    if (neg) {
      if (acc < cutoff || (acc == cutoff && c > cutlim)) {
        any = -1;
        acc = LONG_MIN;
        sys_errno = ERANGE;
      } else {
        any = 1;
        acc *= base;
        acc -= c;
      }

    } else {
      if (acc > cutoff || (acc == cutoff && c > cutlim)) {
        any = -1;
        acc = LONG_MAX;
        sys_errno = ERANGE;
      } else {
        any = 1;
        acc *= base;
        acc += c;
      }
    }
  }

  if (endptr != 0)
    *endptr = (char *) (any ? s - 1 : nptr);

  return acc;
}
