// provides: int atoi (const char *)

int atoi (const char *s) {
  int has_neg_sign = 0;

  switch (*s) {
    case '-': has_neg_sign = 1;
    // fallthrough
    case '+': s++;
    // fallthrough
    default:
      break;
  }

  int val = 0;
  while ('0' <= *s && *s <= '9')
    val = (10 * val) + (*s++ - '0');

  if (has_neg_sign) val = -val;
  return val;
}
