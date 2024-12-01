// provides: int str_to_int (const char *)
// provides: int32_t str_to_i32 (const char *)
// provides: int64_t str_to_i64 (const char *)

int str_to_int (const char *s) {
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

int32_t str_to_i32 (const char *s) {
  return str_to_int (s);
}

int64_t str_to_i64 (const char *s) {
  int has_neg_sign = 0;

  switch (*s) {
    case '-': has_neg_sign = 1;
    // fallthrough
    case '+': s++;
    // fallthrough
    default:
      break;
  }

  int64_t val = 0;
  while ('0' <= *s && *s <= '9')
    val = (10 * val) + (*s++ - '0');

  if (has_neg_sign) val = -val;
  return val;
}
