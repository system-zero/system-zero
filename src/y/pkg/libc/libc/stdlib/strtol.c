// provides: long str_to_long (const char *, char **, int)
// requires: ctype/isspace.c
// requires: ctype/isalpha.c
// requires: ctype/isdigit.c
// requires: ctype/isupper.c
// comment: alias strtol

long str_to_long (const char *nptr, char **endptr, int base) {
  const char *s;
  long acc, cutoff;
  int c;
  int neg, any, cutlim;

  /*
   * Skip white space and pick up leading +/- sign if any.
   * If base is 0, allow 0x for hex and 0 for octal, else
   * assume decimal; if base is already 16, allow 0x.
   */

  s = nptr;

  do {
    c = (unsigned char) *s++;
  } while (isspace (c));

  if (c is '-') {
    neg = 1;
    c = *s++;
  } else {
    neg = 0;

    if (c is '+')
      c = *s++;
  }

  if ((base is 0 or base is 16) and c is '0' and (*s is 'x' or *s is 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }

  if (base is 0)
    base = c is '0' ? 8 : 10;

  /*
   * Compute the cutoff value between legal numbers and illegal
   * numbers.  That is the largest legal value, divided by the
   * base.  An input number that is greater than this value, if
   * followed by a legal input character, is too big.  One that
   * is equal to this value may be valid or not; the limit
   * between valid and invalid numbers is then based on the last
   * digit.  For instance, if the range for longs is
   * [-2147483648..2147483647] and the input base is 10,
   * cutoff will be set to 214748364 and cutlim to either
   * 7 (negis0) or 8 (negis1), meaning that if we have accumulated
   * a value > 214748364, or equal but the next digit is > 7 (or 8),
   * the number is too big, and we will return a range error.
   *
   * Set any if any `digits' consumed; make it negative to indicate
   * overflow.
   */

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
      if (acc < cutoff or (acc is cutoff and c > cutlim)) {
        any = -1;
        acc = LONG_MIN;
        sys_errno = ERANGE;
      } else {
        any = 1;
        acc *= base;
        acc -= c;
      }

    } else {
      if (acc > cutoff or (acc is cutoff and c > cutlim)) {
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

  if (endptr isnot 0)
    *endptr = (char *) (any ? s - 1 : nptr);

  return acc;
}
