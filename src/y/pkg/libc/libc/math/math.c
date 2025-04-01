// provides: float floorf (float)
// provides: double floor (double)
// provides: float ceilf (float)
// provides: double ceil (double)
// provides: float roundf (float)
// provides: double round (double)
// provides: float expf (float)
// provides: double exp (double)
// provides: float log2f (float)
// provides: double log2 (double)
// provides: float log10f (float)
// provides: double log10 (double)
// provides: float logf (float)
// provides: double log (double)
// provides: float powf (float, float)
// provides: double pow (double, double)
// provides: float sqrtf (float)
// provides: double sqrt (double)
// provides: float cbrtf (float)
// provides: double cbrt (double)
// provides: float sinf (float)
// provides: double sin (double)
// provides: float cosf (float)
// provides: double cos (double)
// provides: float tanf (float)
// provides: double tan (double)
// provides: float asinf (float)
// provides: double asin (double)
// provides: float acosf (float)
// provides: double acos (double)
// provides: float atanf (float)
// provides: double atan (double)
// provides: float atan2f (float, float)
// provides: double atan2 (double, double)
// provides: float fabsf (float)
// provides: double fabs (double)
// provides: float fminf (float, float)
// provides: double fmin (double, double)
// provides: float fmaxf (float, float)
// provides: double fmax (double, double)
// requires: math/math.h

/*
 * This file was written in 2023 by Martin Dvorak <jezek2@advel.cz>
 * You can download latest version at http://public-domain.advel.cz/
 *
 * To the extent possible under law, the author(s) have dedicated all
 * copyright and related and neighboring rights to this file to the
 * public domain worldwide. This software is distributed without any
 * warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication
 * along with this software. If not, see:
 * http://creativecommons.org/publicdomain/zero/1.0/ 
 */

static float ftrunc(float value)
{
   union {
      float f;
      uint32_t i;
   } u;
   int e;

   u.f = value;
   e = ((u.i >> 23) & 0xFF) - 127;

   if (e < 0) {
      u.i &= 0x80000000;
   }
   else if (e < 23) {
      u.i &= ~((1 << (23-e))-1);
   }

   return u.f;
}


static float ftrunc_up(float value)
{
   union {
      float f;
      uint32_t i;
   } u;
   uint32_t m;
   int e;

   u.f = value;
   e = ((u.i >> 23) & 0xFF) - 127;

   if (e < 0) {
      if (u.i & 0x7FFFFFFF) {
         return u.i & 0x80000000? -1.0f : 1.0f;
      }
      return u.i & 0x80000000? -0.0f : 0.0f;
   }
   else if (e < 23) {
      if (u.i & ((1 << (23-e))-1)) {
         m = (u.i & ((1 << 23)-1)) | (1 << 23);
         m &= ~((1 << (23-e))-1);
         m += 1 << (23-e);
         if (m & (1 << 24)) {
            m >>= 1;
            e++;
         }
         u.i = (u.i & 0x80000000) | ((e+127) << 23) | (m & ((1 << 23)-1));
      }
   }

   return u.f;
}


static double dtrunc(double value)
{
   union {
      double f;
      uint64_t i;
   } u;
   int e;

   u.f = value;
   e = ((u.i >> 52) & 0x7FF) - 1023;

   if (e < 0) {
      u.i &= 0x8000000000000000ULL;
   }
   else if (e < 52) {
      u.i &= ~((1ULL << (52-e))-1);
   }

   return u.f;
}


static double dtrunc_up(double value)
{
   union {
      double f;
      uint64_t i;
   } u;
   uint64_t m;
   int e;

   u.f = value;
   e = ((u.i >> 52) & 0x7FF) - 1023;

   if (e < 0) {
      if (u.i & 0x7FFFFFFFFFFFFFFFULL) {
         return u.i & 0x8000000000000000ULL? -1.0 : 1.0;
      }
      return u.i & 0x8000000000000000ULL? -0.0 : 0.0;
   }
   else if (e < 52) {
      if (u.i & ((1ULL << (52-e))-1)) {
         m = (u.i & ((1ULL << 52)-1)) | (1ULL << 52);
         m &= ~((1ULL << (52-e))-1);
         m += 1ULL << (52-e);
         if (m & (1ULL << 53)) {
            m >>= 1;
            e++;
         }
         u.i = (u.i & 0x8000000000000000ULL) | (((uint64_t)(e+1023)) << 52) | (m & ((1ULL << 52)-1));
      }
   }

   return u.f;
}


float floorf(float value)
{
   return value >= 0.0f? ftrunc(value) : ftrunc_up(value);
}


float ceilf(float value)
{
   return value >= 0.0f? ftrunc_up(value) : ftrunc(value);
}


float roundf(float value)
{
   return ftrunc(value >= 0.0f? value + 0.5f : value - 0.5f);
}


double floor(double value)
{
   return value >= 0.0f? dtrunc(value) : dtrunc_up(value);
}


double ceil(double value)
{
   return value >= 0.0f? dtrunc_up(value) : dtrunc(value);
}


double round(double value)
{
   return dtrunc(value >= 0.0f? value + 0.5f : value - 0.5f);
}


// https://en.wikipedia.org/wiki/Exponentiation_by_squaring#With_constant_auxiliary_memory

static double exp_sqr(double x, int n)
{
   double y = 1.0;

   if (n == 0) {
      return 1.0;
   }
   while (n > 1) {
      if (n & 1) {
         y *= x;
         x *= x;
      }
      else {
         x *= x;
      }
      n >>= 1;
   }
   return x * y;
}


// https://en.wikipedia.org/wiki/Exponential_function#Computation

static double exp_taylor(double x)
{
   double x2, x3, x4, x8;

   x2 = x*x;
   x3 = x2*x;
   x4 = x2*x2;
   x8 = x4*x4;

   return (
      1.0 + x +
      x2 * 0.5 +
      x3 * 0.16666666666666667 +
      x4 * 0.041666666666666667 +
      x4*x * 0.0083333333333333333 +
      x4*x2 * 0.0013888888888888889 +
      x4*x3 * 0.00019841269841269841 +
      x8 * 0.000024801587301587302 +
      x8*x * 0.0000027557319223985891 +
      x8*x2 * 0.00000027557319223985891 +
      x8*x3 * 0.000000025052108385441719
   );
}


float expf(float x)
{
   return exp(x);
}


double exp(double value)
{
   union {
      double f;
      uint64_t i;
   } u;
   double n, frac, result, taylor;
   int neg = 0;

   if (value < 0.0) {
      value = -value;
      neg = 1;
   }

   n = dtrunc(value);
   if (n > 709.0) {
      u.i = 0x7FFULL << 52; // inf
      return u.f;
   }
   frac = value - n;

   result = exp_sqr(2.7182818284590452, n);
   taylor = exp_taylor(frac * 0.25);
   taylor *= taylor;
   taylor *= taylor;
   result *= taylor;

   if (neg) {
      return 1.0 / result;
   }
   return result;
}


float log2f(float x)
{
   return log2(x);
}


// https://en.wikipedia.org/wiki/Binary_logarithm#Iterative_approximation

double log2(double value)
{
   union {
      double f;
      uint64_t i;
   } u;
   double m, tmp, result;
   int e, i, cnt;

   u.f = value;
   if (u.i >> 63) {
      u.i = (0x7FFULL << 52) | (1ULL << 51); // nan
      return u.f;
   }
   if (u.i == 0) {
      u.i = (0x7FFULL << 52) | (1ULL << 63); // -inf
      return u.f;
   }

   e = ((u.i >> 52) & 0x7FF) - 1023;
   u.i = (u.i & ((1ULL<<52)-1)) | (1023ULL << 52);
   m = u.f;

   result = e;
   e = 0;

   for (i=0; i<64; i++) {
      if (m == 1.0) break;
      cnt = 0;
      do {
         m *= m;
         cnt++;
      }
      while (m < 2.0);
      e -= cnt;
      if (e <= -1023) break;

      u.i = (uint64_t)(e + 1023) << 52;
      tmp = result + u.f;
      if (tmp == result) break;
      result = tmp;
      m *= 0.5;
   }

   return result;
}


float log10f(float x)
{
   return log10(x);
}


double log10(double value)
{
   return log2(value) * 0.30102999566398120;
}


float logf(float x)
{
   return log(x);
}


double log(double value)
{
   return log2(value) * 0.69314718055994531;
}


float powf(float x, float y)
{
   return pow(x, y);
}


double pow(double x, double y)
{
   if (x == 0.0) {
      return x;
   }
   if (y == 0.0) {
      return 1.0;
   }
   return exp(log(x) * y);
}


float sqrtf(float x)
{
   return sqrt(x);
}


double sqrt(double x)
{
   return pow(x, 0.5);
}


float cbrtf(float x)
{
   return cbrt(x);
}


double cbrt(double x)
{
   if (x < 0) {
      return -pow(-x, 0.33333333333333333);
   }
   return pow(x, 0.33333333333333333);
}


// https://en.wikipedia.org/wiki/Sine_and_cosine#Series_definitions

static double sin_taylor(double x)
{
   double x2, x3, x4, x5, x7, x8, x9;

   x2 = x*x;
   x3 = x2*x;
   x4 = x2*x2;
   x5 = x3*x2;
   x7 = x4*x3;
   x8 = x4*x4;
   x9 = x5*x4;

   return (
      x -
      x3 * 0.16666666666666667 +
      x5 * 0.0083333333333333333 -
      x7 * 0.00019841269841269841 +
      x9 * 0.0000027557319223985891 -
      x8*x3 * 0.000000025052108385441719 +
      x8*x5 * 0.00000000016059043836821615 -
      x8*x7 * 0.00000000000076471637318198165 +
      x9*x8 * 0.0000000000000028114572543455208 -
      x8*x8*x3 * 0.0000000000000000082206352466243297
   );
}


float sinf(float x)
{
   return sin(x);
}


double sin(double x)
{
   double tmp;
   int neg = 0, quadrant;

   if (x < 0.0) {
      x = -x;
      neg = 1;
   }

   tmp = dtrunc(x * 0.63661977236758134); // 1.0/(pi/2)
   x = x - tmp * 1.5707963267948966;
   tmp *= 0.25;
   quadrant = (tmp - dtrunc(tmp)) * 4.0;

   if (quadrant == 1 || quadrant == 3) {
      x = 1.5707963267948966 - x;
   }
   x = sin_taylor(x);
   if (quadrant == 2 || quadrant == 3) {
      x = -x;
   }
   if (neg) {
      x = -x;
   }
   return x;
}


float cosf(float x)
{
   return cos(x);
}


double cos(double x)
{
   return sin(x + 1.5707963267948966);
}


float tanf(float x)
{
   return tan(x);
}


double tan(double x)
{
   return sin(x) / cos(x);
}


// https://en.wikipedia.org/wiki/Inverse_trigonometric_functions#Infinite_series

static double asin_leibniz(double x)
{
   double x2, x3, x4, x5, x7, x8, x9, x11, x13, x15, x16, x17, x24;

   x2 = x*x;
   x3 = x2*x;
   x4 = x2*x2;
   x5 = x3*x2;
   x7 = x4*x3;
   x8 = x4*x4;
   x9 = x5*x4;
   x11 = x8*x3;
   x13 = x8*x5;
   x15 = x8*x7;
   x16 = x8*x8;
   x17 = x9*x8;
   x24 = x16*x8;

   return (
      x +
      x3 * 0.16666666666666667 +
      x5 * 0.075 +
      x7 * 0.044642857142857144 +
      x9 * 0.030381944444444444 +
      x11 * 0.022372159090909091 +
      x13 * 0.017352764423076923 +
      x15 * 0.01396484375 +
      x17 * 0.011551800896139706 +
      x16*x3 * 0.0097616095291940789 +
      x16*x5 * 0.0083903358096168155 +
      x16*x7 * 0.0073125258735988451 +
      x16*x9 * 0.0064472103118896484 +
      x16*x11 * 0.0057400376708419235 +
      x16*x13 * 0.0051533096823199042 +
      x16*x15 * 0.0046601434869150962 +
      x24*x9 * 0.0042409070936793631 +
      x24*x11 * 0.0038809645588376692 +
      x24*x13 * 0.0035692053938259345 +
      x24*x15 * 0.0032970595034734847
   );
}


float asinf(float x)
{
   return asin(x);
}


double asin(double x)
{
   union {
      double f;
      uint64_t i;
   } u;
   int neg = 0, invert = 0;

   if (x < 0.0) {
      x = -x;
      neg = 1;
   }
   if (x > 1.0) {
      u.i = (0x7FFULL << 52) | (1ULL << 51); // nan
      return u.f;
   }
   if (x > 0.5) {
      x = sqrt((1.0 - x) * 0.5);
      invert = 1;
   }
   x = asin_leibniz(x);
   if (invert) {
		x = 1.5707963267948966 - x * 2.0;
   }
   if (neg) {
      x = -x;
   }
   return x;
}


float acosf(float x)
{
   return acos(x);
}


double acos(double x)
{
   return 1.5707963267948966 - asin(x);
}


// https://en.wikipedia.org/wiki/Inverse_trigonometric_functions#Infinite_series

static double atan_leibniz(double x)
{
   double x2, x3, x4, x5, x7, x8, x9;

   x2 = x*x;
   x3 = x2*x;
   x4 = x2*x2;
   x5 = x3*x2;
   x7 = x4*x3;
   x8 = x4*x4;
   x9 = x5*x4;

   return (
      x -
      x3 * 0.33333333333333333 +
      x5 * 0.2 -
      x7 * 0.14285714285714286 +
      x9 * 0.11111111111111111 -
      x8*x3 * 0.090909090909090909 +
      x8*x5 * 0.076923076923076923 -
      x8*x7 * 0.066666666666666667 +
      x9*x8 * 0.058823529411764706 -
      x8*x8*x3 * 0.052631578947368421 +
      x8*x8*x5 * 0.047619047619047619 -
      x8*x8*x7 * 0.043478260869565217
   );
}


float atanf(float x)
{
   return atan(x);
}


// https://en.wikipedia.org/wiki/Inverse_trigonometric_functions#Arctangent_addition_formula

double atan(double x)
{
   double add;
   int neg = 0, invert = 0, adjust1 = 0, adjust2 = 0;

   if (x < 0.0) {
      x = -x;
      neg = 1;
   }
   if (x > 1.0) {
      x = 1.0 / x;
      invert = 1;
   }
   if (x > 0.5) {
      add = -0.54630248984379051; // tan(-0.5)
      x = (x + add) / (1.0 - x * add);
      adjust1 = 1;
   }
   if (x > 0.25) {
      add = -0.25534192122103627; // tan(-0.25)
      x = (x + add) / (1.0 - x * add);
      adjust2 = 1;
   }
   x = atan_leibniz(x);
   if (adjust2) {
      x += 0.25;
   }
   if (adjust1) {
      x += 0.5;
   }
   if (invert) {
      x = 1.5707963267948966 - x;
   }
   if (neg) {
      x = -x;
   }
   return x;
}


float atan2f(float y, float x)
{
   return atan2(y, x);
}

double atan2(double y, double x)
{
   union {
      double f;
      uint64_t i;
   } u;
   double angle;
   int ys, xs;

   u.f = y;
   ys = u.i >> 63;
   u.f = x;
   xs = u.i >> 63;

   if (x == 0.0 && y == 0.0) {
      if (xs) {
         return ys? -3.1415926535897932 : 3.1415926535897932;
      }
      else {
         return ys? -0.0 : 0.0;
      }
   }

   if (ys) {
      y = -y;
   }
   angle = atan(y / x);
   if (xs) {
      angle = 3.1415926535897932 + angle;
   }
   if (ys) {
      angle = -angle;
   }
   return angle;
}


float fabsf(float x)
{
   union {
      float f;
      uint32_t i;
   } u;

   u.f = x;
   u.i &= 0x7FFFFFFF;
   return u.f;
}


double fabs(double x)
{
   union {
      double f;
      uint64_t i;
   } u;

   u.f = x;
   u.i &= 0x7FFFFFFFFFFFFFFFULL;
   return u.f;
}


float fminf(float x, float y)
{
   return x < y? x : y;
}


double fmin(double x, double y)
{
   return x < y? x : y;
}


float fmaxf(float x, float y)
{
   return x > y? x : y;
}


double fmax(double x, double y)
{
   return x > y? x : y;
}
