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
// requires: string/mem_set.c
// requires: convert/decimal.h

static void decimal_prepend (decimal_t *dec, char c) {
  dec->digits[DECIMAL_NUM_DIGITS - (++dec->size)] = c;
}

static char *unsigned64_to_string (decimal_t *dec, uint64_t u, int minus) {
  dec->size = 0;
  mem_set (dec->digits, 0, DECIMAL_NUM_DIGITS + 1);

  switch (dec->base) {
    case 2 ... 36:
      break;

    default:
      return dec->digits + (DECIMAL_NUM_DIGITS - dec->size);
   }

  if (u == 0) {
    decimal_prepend (dec, '0');
    return dec->digits + (DECIMAL_NUM_DIGITS - dec->size);
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

  return dec->digits + (DECIMAL_NUM_DIGITS - dec->size);
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
  //decimal_prepend (dec, '0');
  dec->base = orig_base;
  return dec->digits + (DECIMAL_NUM_DIGITS - dec->size);
}

char *ptr_to_string (decimal_t *dec, void *ptr) {
  int orig_base = dec->base;
  dec->base = 16;
  unsigned64_to_string (dec, (uint64_t) ptr, 0);
  decimal_prepend (dec, 'x');
  decimal_prepend (dec, '0');
  dec->base = orig_base;
  return dec->digits + (DECIMAL_NUM_DIGITS - dec->size);
}

char *uint64_to_hex_string (decimal_t *dec, uint64_t u) {
  int orig_base = dec->base;
  dec->base = 16;
  unsigned64_to_string (dec, u, 0);
  dec->base = orig_base;
  return dec->digits + (DECIMAL_NUM_DIGITS - dec->size);
}

char *uint64_to_binary_string (decimal_t *dec, uint64_t u) {
  int orig_base = dec->base;
  dec->base = 2;
  unsigned64_to_string (dec, u, 0);
  dec->base = orig_base;
  return dec->digits + (DECIMAL_NUM_DIGITS - dec->size);
}
