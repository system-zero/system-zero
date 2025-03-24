// provides: char *int_to_string (decimal_t *, int32_t)
// provides: char *uint_to_string (decimal_t *, uint32_t)
// provides: char *long_to_string (decimal_t *, long)
// provides: char *ulong_to_string (decimal_t *, ulong)
// provides: char *int64_to_string (decimal_t *, int64_t)
// provides: char *uint64_to_string (decimal_t *, uint64_t)
// provides: char *uint64_to_binary_string (decimal_t *, uint64_t)
// provides: char *uint64_to_octal_string (decimal_t *, uint64_t)
// provides: char *uint64_to_hex_string (decimal_t *, uint64_t)
// provides: char *ptr_to_string (decimal_t *, void *)
// provides: char *get_decimal_string (decimal_t *)
// requires: convert/decimal.h

static inline int decimal_prepend (decimal_t *dec, char c) {
  if (dec->size == DECIMAL_NUM_DIGITS) return -1;
  dec->digits[DECIMAL_NUM_DIGITS - (++dec->size)] = c;
  return 0;
}

inline char *get_decimal_string (decimal_t *dec) {
  return dec->digits + (DECIMAL_NUM_DIGITS - dec->size);
}

static char *unsigned64_to_string (decimal_t *dec, uint64_t u, int minus) {
  dec->size = 0;
  for (int i = 0; i < DECIMAL_NUM_DIGITS + 1; i++)
    dec->digits[i] = 0;

  switch (dec->base) {
    case 2 ... 36:
      break;

    default:
      return get_decimal_string (dec);
   }

  if (u == 0) {
    decimal_prepend (dec, '0');
    return get_decimal_string (dec);
  }

  const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

  while (u) {
    char digit = digits[(u % dec->base)];
    decimal_prepend (dec, digit);
    u /= dec->base;
  }

/* or
  uint64_t n = u;
  decimal_t de = {.base = 16, .size = 0};
  while (n != 0) {
    int r = (int) (n % (uint64_t) (de.base));
    char digit = (char) r + (r < 10 ? '0' : 'a' - 10);
    decimal_prepend (&de, digit);
    n /= (unsigned long long) (de.base);
  }
*/

  if (minus) decimal_prepend (dec, '-');

  return get_decimal_string (dec);
}

char *int_to_string (decimal_t *dec, int32_t i) {
  int minus = i < 0;
  return unsigned64_to_string (dec, minus ? -i : i, minus);
}

char *uint_to_string (decimal_t *dec, uint32_t u) {
  return unsigned64_to_string (dec, u, 0);
}

char *long_to_string (decimal_t *dec, long i) {
  return int_to_string (dec, i);
}

char *ulong_to_string (decimal_t *dec, ulong u) {
  return uint_to_string (dec, u);
}

char *uint64_to_string (decimal_t *dec, uint64_t u) {
  return unsigned64_to_string (dec, u, 0);
}

char *int64_to_string (decimal_t *dec, int64_t i) {
  int minus = i < 0;
  return unsigned64_to_string (dec, minus ? -i : i, minus);
}

char *uint64_to_octal_string (decimal_t *dec, uint64_t u) {
  int orig_base = dec->base;
  dec->base = 8;
  unsigned64_to_string (dec, u, 0);
  dec->base = orig_base;
  return get_decimal_string (dec);
}

char *uint64_to_hex_string (decimal_t *dec, uint64_t u) {
  int orig_base = dec->base;
  dec->base = 16;
  unsigned64_to_string (dec, u, 0);
  dec->base = orig_base;
  return get_decimal_string (dec);
}

char *ptr_to_string (decimal_t *dec, void *ptr) {
  uint64_to_hex_string (dec, (uint64_t) ptr);
  decimal_prepend (dec, 'x');
  decimal_prepend (dec, '0');
  return get_decimal_string (dec);
}

char *uint64_to_binary_string (decimal_t *dec, uint64_t u) {
  int orig_base = dec->base;
  dec->base = 2;
  unsigned64_to_string (dec, u, 0);
  dec->base = orig_base;
  return get_decimal_string (dec);
}
