// provides: int sys_vsnprintf (char *, size_t, const char *, va_list)
// provides: int sys_snprintf (char *, size_t, const char *, ...)
// provides: int str_format (fmtType *, char *, size_t, const char *, va_list)
// requires: string/bytelen.c
// requires: ctype/toupper.c
// requires: ctype/isspace.c
// requires: ctype/isdigit.c
// requires: string/fmt.h
// requires: string/vsnprintf.h

#define MAX_INTEGRAL_SIZE (99 + 1)
#define MAX_FRACTION_SIZE (29 + 1)
#define PRECISION (1.e-6)

#define ROUND_TO_PRECISION(d, p) \
  ((d < 0.) ? d - pow_10(-(p)->precision) * 0.5 : d + pow_10(-(p)->precision) * 0.5)

#define PUT_CHAR(__c__, __p__) (__p__)->output_char ((__p__), (__c__))

#define PUT_PLUS(d, p)                                  \
  if ((d) > 0 && (p)->align == ALIGN_RIGHT) {           \
    PUT_CHAR('+', p);                                   \
  }

#define PUT_SPACE(d, p)                                 \
  if ((p)->is_space && (d) > 0) {                       \
    PUT_CHAR(' ', p);                                   \
  }

#define PAD_RIGHT(p)                                    \
  if ((p)->width > 0 && (p)->align != ALIGN_LEFT) {     \
    for (; (p)->width > 0; (p)->width--) {              \
      PUT_CHAR((p)->pad, p);                            \
    }                                                   \
  }

#define PAD_LEFT(p)                                     \
  if ((p)->width > 0 && (p)->align == ALIGN_LEFT) {     \
    for (; (p)->width > 0; (p)->width--) {              \
      PUT_CHAR((p)->pad, p);                            \
    }                                                   \
  }

#define WIDTH_AND_PRECISION_ARGS(p)                     \
  if ((p)->is_star_w) {                                 \
    (p)->width = va_arg(args, int);                     \
  }                                                     \
  if ((p)->is_star_p) {                                 \
    (p)->precision = va_arg(args, int);                 \
  }

#define INTEGER_ARG(p, type, ll)                        \
  WIDTH_AND_PRECISION_ARGS(p);                          \
  if ((p)->a_long == INT_LEN_LONG_LONG) {               \
    ll = (long long)va_arg(args, type long long);       \
  } else if ((p)->a_long == INT_LEN_LONG) {             \
    ll = (long long)va_arg(args, type long);            \
  } else {                                              \
    type int a = va_arg(args, type int);                \
    if ((p)->a_long == INT_LEN_SHORT) {                 \
      ll = (type short)a;                               \
    } else if ((p)->a_long == INT_LEN_CHAR) {           \
      ll = (type char)a;                                \
    } else {                                            \
      ll = a;                                           \
    }                                                   \
  }

#define DOUBLE_ARG(p, d)                                \
  WIDTH_AND_PRECISION_ARGS(p);                          \
  if ((p)->precision == PRECISION_UNSET) {              \
    (p)->precision = 6;                                 \
  }                                                     \
  d = va_arg(args, double);

static inline void fmt_output_char (struct fmtType *p, int c) {
  if (p->counter >= p->ps_size) return; // ag: we are safe here as if buf is NULL
                                       // str_format() set ps_size it to SIZE_MAX
  if (p->ps isnot NULL) *p->ps++ = c;

  p->counter++;
}

static int strtoi (const char *a, int *res) {
  int i = 0;

  *res = 0;

  for (; a[i] != '\0' && isdigit (a[i]); i++) {
    *res = *res * 10 + (a[i] - '0');
  }

  return i;
}

static void inttoa (long long nr, int is_signed, int precision, int base,
    char *output, size_t output_size) {
  size_t i = 0, j;

  output_size--; /* for '\0' character */

  if (nr != 0) {
    unsigned long long n;

    if (is_signed && nr < 0) {
      n = (unsigned long long) -nr;
      output_size--; /* for '-' character */
    } else {
      n = (unsigned long long) nr;
    }

    while (n != 0 && i < output_size) {
      int r = (int) (n % (unsigned long long) (base));
      output[i++] = (char) r + (r < 10 ? '0' : 'a' - 10);
      n /= (unsigned long long) (base);
    }

    if (precision > 0) { /* precision defined ? */
      for (; i < (size_t) precision && i < output_size; i++) {
        output[i] = '0';
      }
    }

    /* put the sign ? */
    if (is_signed && nr < 0) {
      output[i++] = '-';
    }

    output[i] = '\0';

    /* reverse every thing */
    for (i--, j = 0; j < i; j++, i--) {
      char tmp = output[i];
      output[i] = output[j];
      output[j] = tmp;
    }
  } else {
    precision = precision < 0 ? 1 : precision;
    for (i = 0; i < (size_t) precision && i < output_size; i++) {
      output[i] = '0';
    }
    output[i] = '\0';
  }
}

static double pow_10 (int n) {
  int i = 1;
  double p = 1., m;

  if (n < 0) {
    n = -n;
    m = .1;
  } else {
    m = 10.;
  }

  for (; i <= n; i++) {
    p *= m;
  }

  return p;
}

static int log_10 (double r) {
  int i = 0;
  double result = 1.;

  if (r == 0.) {
    return 0;
  }

  if (r < 0.) {
    r = -r;
  }

  if (r < 1.) {
    for (; result >= r; i++) {
      result *= .1;
    }

    i = -i;
  } else {
    for (; result <= r; i++) {
      result *= 10.;
    }

    --i;
  }

  return i;
}

static double integral (double real, double *ip) {
  int log;
  double real_integral = 0.;

  /* equal to zero ? */
  if (real == 0.) {
    *ip = 0.;
    return 0.;
  }

  /* negative number ? */
  if (real < 0.) {
    real = -real;
  }

  /* a fraction ? */
  if (real < 1.) {
    *ip = 0.;
    return real;
  }

  /* the real work :-) */
  for (log = log_10 (real); log >= 0; log--) {
    double i = 0., p = pow_10 (log);
    double s = (real - real_integral) / p;
    for (; i + 1. <= s; i++) {}
    real_integral += i * p;
  }

  *ip = real_integral;
  return (real - real_integral);
}

static void floattoa (double nr, int precision,
    char *output_integral, size_t output_integral_size,
    char *output_fraction, size_t output_fraction_size) {

  size_t i, j;
  int is_negative = 0;
  double ip, fp; /* integer and fraction part */
  double fraction;

  /* taking care of the obvious case: 0.0 */
  if (nr == 0.) {
    output_integral[0] = output_fraction[0] = '0';
    output_integral[1] = output_fraction[1] = '\0';

    return;
  }

  /* for negative numbers */
  if (nr < 0.) {
    nr = -nr;
    is_negative = 1;
    output_integral_size--; /* sign consume one digit */
  }

  fraction = integral (nr, &ip);
  nr = ip;
  /* do the integral part */
  if (ip == 0.) {
    output_integral[0] = '0';
    i = 1;
  } else {
    for (i = 0; i < output_integral_size - 1 && nr != 0.; ++i) {
      nr /= 10;
      /* force to round */
      output_integral[i] = (char) ((integral (nr, &ip) + PRECISION) * 10) + '0';
      if (!isdigit (output_integral[i])) { /* bail out overflow !! */
        break;
      }
      nr = ip;
    }
  }

  /* Oh No !! out of bound, ho well fill it up ! */
  if (nr != 0.) {
    for (i = 0; i < output_integral_size - 1; ++i) {
      output_integral[i] = '9';
    }
  }

  /* put the sign ? */
  if (is_negative) {
    output_integral[i++] = '-';
  }

  output_integral[i] = '\0';

  /* reverse every thing */
  for (i--, j = 0; j < i; j++, i--) {
    char tmp = output_integral[i];
    output_integral[i] = output_integral[j];
    output_integral[j] = tmp;
  }

  /* the fractional part */
  for (i = 0, fp = fraction; precision > 0 && i < output_fraction_size - 1; i++, precision--) {
    output_fraction[i] = (char)(int) ((fp + PRECISION) * 10. + '0');
    if (!isdigit (output_fraction[i])) { /* underflow ? */
      break;
    }

    fp = (fp * 10.0) - (double)(long)((fp + PRECISION) * 10.);
  }
  output_fraction[i] = '\0';
}

static void decimal (struct fmtType *p, long long ll) {
  char nr[MAX_INTEGRAL_SIZE], *pnumber = nr;
  inttoa (ll, *p->pf == 'i' || *p->pf == 'd', p->precision, 10,
    nr, sizeof(nr));

  p->width -= bytelen (nr);
  PAD_RIGHT(p);

  PUT_PLUS(ll, p);
  PUT_SPACE(ll, p);

  for (; *pnumber != '\0'; pnumber++) {
    PUT_CHAR(*pnumber, p);
  }

  PAD_LEFT(p);
}

static void octal (struct fmtType *p, long long ll) {
  char nr[MAX_INTEGRAL_SIZE], *pnumber = nr;
  inttoa (ll, 0, p->precision, 8, nr, sizeof(nr));

  p->width -= bytelen (nr);
  PAD_RIGHT(p);

  if (p->is_square && *nr != '\0') { /* prefix '0' for octal */
    PUT_CHAR('0', p);
  }

  for (; *pnumber != '\0'; pnumber++) {
    PUT_CHAR(*pnumber, p);
  }

  PAD_LEFT(p);
}

static void hex (struct fmtType *p, long long ll) {
  char nr[MAX_INTEGRAL_SIZE], *pnumber = nr;
  inttoa (ll, 0, p->precision, 16, nr, sizeof(nr));

  p->width -= bytelen (nr);
  PAD_RIGHT(p);

  if (p->is_square && *nr != '\0') { /* prefix '0x' for hex */
    PUT_CHAR('0', p);
    PUT_CHAR(*p->pf == 'p' ? 'x' : *p->pf, p);
  }

  for (; *pnumber != '\0'; pnumber++) {
    PUT_CHAR((*p->pf == 'X' ? (char) toupper (*pnumber) : *pnumber), p);
  }

  PAD_LEFT(p);
}

static void strings (struct fmtType *p, const char *s) {
  int len = (int) bytelen (s);
  if (p->precision != PRECISION_UNSET && len > p->precision) { /* the smallest number */
    len = p->precision;
  }

  p->width -= len;

  PAD_RIGHT(p);

  for (; len-- > 0; s++) {
    PUT_CHAR(*s, p);
  }

  PAD_LEFT(p);
}

static void floating (struct fmtType *p, double d) {
  char integral[MAX_INTEGRAL_SIZE], *pintegral = integral;
  char fraction[MAX_FRACTION_SIZE], *pfraction = fraction;

  d = ROUND_TO_PRECISION(d, p);
  floattoa (d, p->precision,
    integral, sizeof (integral), fraction, sizeof (fraction));

  /* calculate the padding. 1 for the dot */
  if (d > 0. && p->align == ALIGN_RIGHT) {
    p->width -= 1;
  }
  p->width -= p->is_space + (int)bytelen(integral) + p->precision + 1;
  if (p->precision == 0) {
    p->width += 1;
  }

  PAD_RIGHT(p);
  PUT_PLUS(d, p);
  PUT_SPACE(d, p);

  for (; *pintegral != '\0'; pintegral++) {
    PUT_CHAR(*pintegral, p);
  }

  if (p->precision != 0 || p->is_square) { /* put the '.' */
    PUT_CHAR('.', p);
  }

  if (*p->pf == 'g' || *p->pf == 'G') { /* smash the trailing zeros */
    size_t i;
    for (i = bytelen (fraction); i > 0 && fraction[i - 1] == '0'; i--) {
      fraction[i - 1] = '\0';
    }
  }

  for (; *pfraction != '\0'; pfraction++) {
    PUT_CHAR(*pfraction, p);
  }

  PAD_LEFT(p);
}

static void exponent (struct fmtType *p, double d) {
  char integral[MAX_INTEGRAL_SIZE], *pintegral = integral;
  char fraction[MAX_FRACTION_SIZE], *pfraction = fraction;
  int log = log_10 (d);
  d /= pow_10 (log); /* get the Mantissa */
  d = ROUND_TO_PRECISION(d, p);

  floattoa (d, p->precision,
    integral, sizeof (integral), fraction, sizeof (fraction));
  /* 1 for unit, 1 for the '.', 1 for 'e|E',
   * 1 for '+|-', 2 for 'exp' */
  /* calculate how much padding need */
  if (d > 0. && p->align == ALIGN_RIGHT) {
    p->width -= 1;
  }
  p->width -= p->is_space + p->precision + 7;

  PAD_RIGHT(p);
  PUT_PLUS(d, p);
  PUT_SPACE(d, p);

  for (; *pintegral != '\0'; pintegral++) {
    PUT_CHAR(*pintegral, p);
  }

  if (p->precision != 0 || p->is_square) { /* the '.' */
    PUT_CHAR('.', p);
  }

  if (*p->pf == 'g' || *p->pf == 'G') { /* smash the trailing zeros */
    size_t i;
    for (i = bytelen (fraction); i > 0 && fraction[i - 1] == '0'; i--) {
      fraction[i - 1] = '\0';
    }
  }
  for (; *pfraction != '\0'; pfraction++) {
    PUT_CHAR(*pfraction, p);
  }

  if (*p->pf == 'g' || *p->pf == 'e') { /* the exponent put the 'e|E' */
    PUT_CHAR('e', p);
  } else {
    PUT_CHAR('E', p);
  }

  if (log >= 0) { /* the sign of the exp */
    PUT_CHAR('+', p);
  }

  inttoa (log, 1, 2, 10, integral, sizeof (integral));
  for (pintegral = integral; *pintegral != '\0'; pintegral++) { /* exponent */
    PUT_CHAR(*pintegral, p);
  }

  PAD_LEFT(p);
}

static void conv_flags (struct fmtType *p) {
  p->width = WIDTH_UNSET;
  p->precision = PRECISION_UNSET;
  p->is_star_w = p->is_star_p = 0;
  p->is_square = p->is_space = 0;
  p->a_long = INT_LEN_DEFAULT;
  p->align = ALIGN_UNSET;
  p->pad = ' ';
  p->is_dot = 0;

  for (; p != NULL && p->pf != NULL; p->pf++) {
    switch (*p->pf) {
      case ' ':
        p->is_space = 1;
        break;

      case '#':
        p->is_square = 1;
        break;

      case '*':
        if (p->width == WIDTH_UNSET) {
          p->width = 1;
          p->is_star_w = 1;
        } else {
          p->precision = 1;
          p->is_star_p = 1;
        }
        break;

      case '+':
        p->align = ALIGN_RIGHT;
        break;

      case '-':
        p->align = ALIGN_LEFT;
        break;

      case '.':
        if (p->width == WIDTH_UNSET) {
          p->width = 0;
        }
        p->is_dot = 1;
        break;

      case '0':
        p->pad = '0';
        if (p->is_dot) {
          p->precision = 0;
        }
        break;

      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': /* get all the digits */
        p->pf += strtoi (p->pf,
          p->width == WIDTH_UNSET ? &p->width : &p->precision) - 1;
        break;

      case '%':
        return;

      default:
        p->pf--; /* went to far go back */
        return;
    }
  }
}

__attribute__((format(printf, 4, 0)))
int str_format (struct fmtType *p, char *buf, size_t bufsize, const char *format, va_list args) {

  /* calculate only size of output string */
  if (buf == NULL) {
    bufsize = SIZE_MAX;
  /* sanity check, the string must be > 1 */
  } else if (bufsize < 1) {
    return -1;
  }

  p->ps_size = bufsize - 1; /* leave room for '\0' */
  p->ps = buf;
  p->pf = format;
  p->counter = 0;

  for (; *p->pf != '\0' && (p->counter < p->ps_size); p->pf++) {
    if (*p->pf == '%') { /* we got a magic % cookie */
      int is_continue = 1;
      conv_flags (p); /* initialise format flags */
      while (*p->pf != '\0' && is_continue) {
        switch (*(++p->pf)) {
          case '\0': /* a NULL here ? ? bail out */
            PUT_CHAR('%', p);
            if (p->ps != NULL) {
              *p->ps = '\0';
            }
            return (int) p->counter;

          case 'f':
          case 'F': { /* decimal floating point */
            double d;
            DOUBLE_ARG(p, d);
            floating (p, d);
            is_continue = 0;
            break;
          }

          case 'e':
          case 'E': { /* scientific (exponential) floating point */
            double d;
            DOUBLE_ARG(p, d);
            exponent (p, d);
            is_continue = 0;
            break;
          }

          case 'g':
          case 'G': { /* scientific or decimal floating point */
            int log;
            double d;
            DOUBLE_ARG(p, d);
            log = log_10 (d);
            /* use decimal floating point (%f / %F) if exponent is in the range
               [-4,precision] exclusively else use scientific floating
               point (%e / %E) */
            if (-4 < log && log < p->precision) {
              floating (p, d);
            } else {
              exponent (p, d);
            }
            is_continue = 0;
            break;
          }

          case 'u': { /* unsigned decimal integer */
            long long ll;
            INTEGER_ARG(p, unsigned, ll);
            decimal (p, ll);
            is_continue = 0;
            break;
          }

          case 'i':
          case 'd': { /* signed decimal integer */
            long long ll;
            INTEGER_ARG(p, signed, ll);
            decimal (p, ll);
            is_continue = 0;
            break;
          }

          case 'o': { /* octal (always unsigned) */
            long long ll;
            INTEGER_ARG(p, unsigned, ll);
            octal (p, ll);
            is_continue = 0;
            break;
          }

          case 'x':
          case 'X': { /* hexadecimal (always unsigned) */
            long long ll;
            INTEGER_ARG(p, unsigned, ll);
            hex (p, ll);
            is_continue = 0;
            break;
          }

          case 'c': { /* single character */
            int i = va_arg(args, int);
            PUT_CHAR((char)i, p);
            is_continue = 0;
            break;
          }

          case 's': /* string of characters */
            WIDTH_AND_PRECISION_ARGS(p);
            strings (p, va_arg(args, char *));
            is_continue = 0;
            break;

          case 'p': { /* pointer */
            void *v = va_arg(args, void *);
            p->is_square = 1;
            if (v == NULL) {
              strings (p, "(nil)");
            } else {
              hex (p, (intptr_t) v);
            }
            is_continue = 0;
            break;
          }

          case 'n': /* what's the count ? */
            *(va_arg(args, int *)) = (int)p->counter;
            is_continue = 0;
            break;

          case 'l': /* long or long long */
            if (p->a_long == INT_LEN_LONG) {
              p->a_long = INT_LEN_LONG_LONG;
            } else {
              p->a_long = INT_LEN_LONG;
            }
            break;

          case 'h': /* short or char */
            if (p->a_long == INT_LEN_SHORT) {
              p->a_long = INT_LEN_CHAR;
            } else {
              p->a_long = INT_LEN_SHORT;
            }
            break;

          case '%': /* nothing just % */
            PUT_CHAR('%', p);
            is_continue = 0;
            break;

          case '#':
          case ' ':
          case '+':
          case '*':
          case '-':
          case '.':
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            conv_flags (p);
            break;

          default:
            /* is this an error ? maybe bail out */
            PUT_CHAR('%', p);
            is_continue = 0;
            break;
        } /* end switch */
      } /* end of while */
    } else { /* not % */
      PUT_CHAR(*p->pf, p); /* add the char the string */
    }
  }

  if (p->ps != NULL) {
    *p->ps = '\0'; /* the end ye ! */
  }

  return (int) p->counter;
}

int sys_vsnprintf (char *buf, size_t bufsize, const char *fmt, va_list args) {
  struct fmtType s;
  s.output_char = fmt_output_char;
  return str_format (&s, buf, bufsize, fmt, args);
}

int sys_snprintf (char *buf, size_t bufsize, const char *fmt, ...) {
  fmtType s;
  s.output_char = fmt_output_char;

  va_list ap;
  va_start(ap, fmt);
  int n = str_format (&s, buf, bufsize, fmt, ap);
  va_end(ap);

  return n;
}
