// requires: std/float.h

#if (DBL_MANT_DIG <= 24) && (DBL_MAX_EXP <= 128)
  typedef uint32_t double_bin;
  typedef signed char exponent_t;
#elif (DBL_MANT_DIG <= 53) && (DBL_MAX_EXP <= 1024)
  typedef uint64_t double_bin;
  typedef int32_t exponent_t;
#endif

typedef double_bin mantissa_t;

enum {
  DOUBLE_EXPONENT_MASK = DBL_MAX_EXP * 2 - 1,
  DOUBLE_EXPONENT_BIAS = DBL_MAX_EXP - 1,
  DOUBLE_MANTISSA_BITS = DBL_MANT_DIG - 1,
  DOUBLE_BIN_BITS = sizeof (double_bin) * CHAR_BIT,
  MANTISSA_BITS   = sizeof (mantissa_t) * CHAR_BIT,
  SHIFT_BITS = ((MANTISSA_BITS < DBL_MANT_DIG) ? MANTISSA_BITS : DBL_MANT_DIG) - 1
};

#ifndef INFINITY
#define INFINITY __builtin_inff ()
#endif

#ifndef NAN
#define NAN __builtin_nanf ("")
#endif
