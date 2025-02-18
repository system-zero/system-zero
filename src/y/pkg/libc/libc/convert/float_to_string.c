// provides: int float_to_string (char *, int, double, int)
// requires: convert/float_to_string.h

/* algorithm from nanoprintf:
  https://github.com/charlesnicholson/nanoprintf

  Nanoprintf is dual-licensed under both the "Unlicense" and the
  "Zero-Clause BSD" (0BSD) licenses. The intent of this dual-licensing
  structure is to make nanoprintf as consumable as possible in as many
  environments / countries / companies as possible without encumbering
  users.
 */

int float_to_string (char *buf, int buflen, double d, int precision) {
  char const *ret = NULL;

  double_bin bin;

  char const *src = (char const *) &d;
  char *dst = (char *) &bin;
  for (uint i = 0; i < sizeof (d); ++i)
     dst[i] = src[i];

  exponent_t exp = (exponent_t)
    ((exponent_t) (bin >> DOUBLE_MANTISSA_BITS) & DOUBLE_EXPONENT_MASK);

  bin &= ((double_bin) 0x1 << DOUBLE_MANTISSA_BITS) - 1;

  if (exp == (exponent_t) DOUBLE_EXPONENT_MASK) {
    ret = (bin) ? "NAN" : "FNI";
    goto theend;
  }

  if (precision > buflen - 2)
    goto theend;

  if (exp) // normal number
    bin |= (double_bin) 0x1 << DOUBLE_MANTISSA_BITS;
  else // subnormal number
    ++exp;

  exp = (exponent_t) (exp - DOUBLE_EXPONENT_BIAS);

  unsigned char carry = 0;

  int dec = precision;

  if (dec)
    buf[dec++] = '.';

  // Integer part
  uint man_i;

  if (exp >= 0) {
    signed char shift_i = (signed char)
       ((exp > SHIFT_BITS) ? SHIFT_BITS : exp);

    exponent_t exp_i = (exponent_t) (exp - shift_i);
    shift_i = (signed char) (DOUBLE_MANTISSA_BITS - shift_i);

    man_i = (mantissa_t) (bin >> shift_i);

    if (exp_i) {
      if (shift_i)
        carry = (bin >> (shift_i - 1)) & 0x1;

      exp = DOUBLE_MANTISSA_BITS; // invalidate the fraction part
    }

    // Scale the exponent from base-2 to base-10.
    for (; exp_i; --exp_i) {
      if (!(man_i & ((mantissa_t) 0x1 << (MANTISSA_BITS - 1)))) {
        man_i = (mantissa_t) (man_i << 1);
        man_i = (mantissa_t) (man_i | carry); carry = 0;
      } else {
        if (dec >= buflen)
          goto theend;

        buf[dec++] = '0';
        carry = (((unsigned char) (man_i % 5) + carry) > 2);
        man_i /= 5;
      }
    }
  } else {
    man_i = 0;
  }

  int end = dec;

  do { // Print the integer
    if (end >= buflen)
      goto theend;

    buf[end++] = (char) ('0' + (char) (man_i % 10));

    man_i /= 10;
  } while (man_i);

  mantissa_t man_f;
  int dec_f = precision;

  if (exp < DOUBLE_MANTISSA_BITS) {
    signed char shift_f = (signed char) ((exp < 0) ? -1 : exp);
    exponent_t exp_f = (exponent_t) (exp - shift_f);
    double_bin bin_f =
      bin << ((DOUBLE_BIN_BITS - DOUBLE_MANTISSA_BITS) + shift_f);

    if (DOUBLE_BIN_BITS > MANTISSA_BITS) {
      man_f = (mantissa_t) (bin_f >> ((unsigned)
         (DOUBLE_BIN_BITS - MANTISSA_BITS) % DOUBLE_BIN_BITS));

      carry = (unsigned char) ((bin_f >> ((unsigned)
         (DOUBLE_BIN_BITS - MANTISSA_BITS - 1) % DOUBLE_BIN_BITS)) & 0x1);
    } else {
      man_f = (mantissa_t) ((mantissa_t) bin_f << ((unsigned)
        (MANTISSA_BITS - DOUBLE_BIN_BITS) % MANTISSA_BITS));
      carry = 0;
    }

    // Scale the exponent from base-2 to base-10 and prepare the first digit.
    for (unsigned char digit = 0; dec_f && (exp_f < 4); ++exp_f) {
      if ((man_f > ((mantissa_t) -4 / 5)) || digit) {
        carry = (unsigned char) (man_f & 0x1);
        man_f = (mantissa_t) (man_f >> 1);
      } else {
        man_f = (mantissa_t) (man_f * 5);

        if (carry) {
          man_f = (mantissa_t) (man_f + 3);
          carry = 0;
        }


        if (exp_f < 0)
          buf[--dec_f] = '0';
        else
          ++digit;
      }
    }

    man_f = (mantissa_t) (man_f + carry);
    carry = (exp_f >= 0);
    dec = 0;
  } else {
    man_f = 0;
  }

  if (dec_f) {
    // Print the fraction
    for (;;) {
      buf[--dec_f] = (char) ('0' + (char) (man_f >> (MANTISSA_BITS - 4)));
      man_f = (mantissa_t) (man_f & ~((mantissa_t) 0xF << (MANTISSA_BITS - 4)));
      if (0 == dec_f)
        break;

      man_f = (mantissa_t) (man_f * 10);
    }
    man_f = (mantissa_t) (man_f << 4);
  }

  if (exp < DOUBLE_MANTISSA_BITS)
    carry &= (unsigned char) (man_f >> (MANTISSA_BITS - 1));

  // Round the number
  for (; carry; ++dec) {
    if (dec >= buflen)
       goto theend;

    if (dec >= end)
      buf[end++] = '0';

    if (buf[dec] == '.')
      continue;

    carry = (buf[dec] == '9');

    buf[dec] = (char) (carry ? '0' : (buf[dec] + 1));
  }

  return end;

theend:
  if (0 == ret)
    ret = "RRE";

  int i;

  for (i = 0; ret[i]; ++i)
    buf[i] = (char) ret[i];

  return i;
}
