// provides: int format_to_string (char *, size_t, const char *, ...)
// provides: int format_to_fd (int, const char *, ...)
// provides: int vformat (FormatType *, va_list)
// provides: int vformat_to_fd (int, const char *, va_list)
// requires: string/bytelen.c
// requires: utf8/utf8_character.c
// requires: utf8/utf8_string_after_nth_char.c
// requires: unistd/write.c
// requires: convert/float_to_string.c
// requires: convert/decimal_to_string.c
// requires: convert/format.h

/* a minimal format function initially based on:
   https://github.com/tatetian/mini-snprintf

   This project has no license attached, and when I asked the developer, he
   gently said that is happy that someone found it usefull, and so do I.
   Many thanks.
 */

/*
   Syntax:
   - %d   int32_t
   - %ld  int64_t
   - %u   uint32_t
   - %lu  uint64_t
   - %b   uint64_t   in binary
   - %o   uint64_t   in octal
   - %x   uint64_t   in hex
   - %p   pointer    in hex
   - %f   float
   - %c   char       single byte in ascii range
   - %lc  int32_t    either a single byte or a sequence of bytes (utf8)
   - %s   char *
   - %ls  char *     with a precision stores upto 'n' characters. This might
                     be not a standard, but that is what it might makes sense.
   - %ns             (where 'n' is an int but not zero), to mean that the next
                     char * argument will be padded with spaces to the left or
                     to the right side, if is smaller than the given 'n' field
                     width.

   - %*s             likewise. Except that the field width value is taken by
                     the next int argument.
   - %m              errno_string/strerror (errno). Doesn't get any argument.
   - %#m             alternate form: errno_name/strerrorname_np (errno). It
                     doesn't get any argument.
   - %.n[s|f]        to mean that if the next char * argument or the result of
                     a float operation, is bigger than 'n' bytes specifier, at
                     most 'n' bytes will be written.
   - %.*s[s|f]       Likewise. Except that the precision value is taken by  the
                     next int argument.
  - %-n[specifier]   Left justification (default is right justificatiom).
   - %%              literal '%'.

   Alternate form:
   For '%#b'prepends '0b', for '%#o' prepends '0' and for '%#x' prepends '0x'.
   A '%p' directive  is always in this form.
   For '%#m' it uses the [sys_]errno symbolic name.

   Also, this implementation sends its byte to an output function.
   The default function writes the byte to its buffer argument.

   A function that outputs to a file descriptor it is also included in the
   interface, thus it can be used to output to std[out|err] file descriptor.
*/

#define RIGHT_JUST  1
#define LEFT_JUST   2

static int format_output_byte (FormatType *this, int c) {
  if (c == '\0') {
    this->bytes[this->num_bytes] = c;
    return 0;
  }

  if (this->num_bytes + 1 == this->mem_size) {
    this->error = FMT_OUT_OF_SPACE;
    return -1;
  }

  this->bytes[this->num_bytes++] = c;
  return 1;
}

static int next_directive (FormatType *this, DirectiveType *directive) {
  directive->size = directive->is_alternate =
  directive->width = directive->precision =
  directive->numchars = 0;
  directive->just = RIGHT_JUST;

  if (*this->fmtPtr == '\0') {
    directive->type = DT_EOF;
    return 0;
  }

  directive->size++;

  if (*this->fmtPtr != '%') {
    directive->type = DT_ORDINARY;
    this->fmtPtr++;

    while (*this->fmtPtr != '\0' && *this->fmtPtr != '%') {
      this->fmtPtr++;
      directive->size++;
    }

    return directive->size;
  }

  this->fmtPtr++;

  if (*this->fmtPtr == '%') {
    this->fmtPtr++;
    directive->type = DT_ESCAPED;
    return ++directive->size;
  }

  int awaitingspec = 0;

  int c;

  next:
  c = *this->fmtPtr++;

  again:
  switch (c) {
    case '\0':
      this->error = FMT_UNEXPECTED_EOF;
      return -1;

    case '#':
      if (awaitingspec) {
        this->error = FMT_UNEXPECTED_DIRECTIVE;
        return -1;
      }
      awaitingspec = 1;

      directive->is_alternate = 1;
      goto next;

    case '-':
      if (awaitingspec) {
        this->error = FMT_UNEXPECTED_DIRECTIVE;
        return -1;
      }
      awaitingspec = 1;

      directive->just = LEFT_JUST;
      goto next;

    case 's':
      directive->type = DT_STRING;
      return 0;

    case 'l':
      switch (*this->fmtPtr++) {
        case 'd':
          directive->type = DT_INT_64;
          return 0;

        case 'u':
          directive->type = DT_UINT_64;
          return 0;

        case 's':
          if (directive->precision == 0 && directive->width == 0) {
            directive->type = DT_STRING;
            return 0;
          }

          directive->type = DT_U8STRING;
          return 0;

        case 'c':
          directive->type = DT_U8CHAR;
          return 0;

        default:
          this->error = FMT_UNHANDLED_SPECIFIER;
          return -1;
      }

    case 'd':
      directive->type = DT_INT_32;
      return 0;

    case 'u':
      directive->type = DT_UINT_32;
      return 0;

    case 'p':
      directive->type = DT_PTR;
      return 0;

    case 'c':
      directive->type = DT_CHAR;
      return 0;

    case 'b':
      directive->type = DT_BINARY;
      return 0;

    case 'o':
      directive->type = DT_OCTAL;
      return 0;

    case 'x':
      directive->type = DT_HEX;
      return 0;

    case 'f':
      directive->type = DT_FLOAT;
      return 0;

    case 'm':
      directive->type = DT_ERRNO;
      return 0;

    case '.':
      if (awaitingspec > 1) {
        this->error = FMT_UNEXPECTED_DIRECTIVE;
        return -1;
      }
      awaitingspec = 2;

      c = *this->fmtPtr++;

      switch (c) {
        case '1' ... '9': {
          int d = c - '0';

          for (;;) {
            c = *this->fmtPtr++;
            if (c == 's' || (c == 'l' && *this->fmtPtr == 's') || c == 'f') {
              directive->precision = d;
              goto again;
            }

            if ('0' <= c && c <= '9') {
              d = (10 * d) + (c - '0');
              continue;
            }

            directive->type = DT_UNEXPECTED;
            return -1;
          }
        }

        case '*':
          switch (*this->fmtPtr++) {
            case 's':
              directive->type = DT_NSTRING;
              return 0;
          }

          __attribute__((fallthrough));

        default:
          this->error = FMT_UNHANDLED_SPECIFIER;
          return -1;
      }
      break;

    case '*':
      if (awaitingspec > 1) {
        this->error = FMT_UNEXPECTED_DIRECTIVE;
        return -1;
      }
      awaitingspec = 2;

      switch (*this->fmtPtr++) {
        case 's':
          directive->type = DT_FSTRING;
          return 0;

        case 'l':
          if (*this->fmtPtr++ == 's') {
            directive->type = DT_U8FSTRING;
            return 0;
          }

        __attribute__((fallthrough));

        default:
          this->error = FMT_UNHANDLED_SPECIFIER;
          return -1;
      }

    case '1' ... '9': {
      if (awaitingspec > 1) {
        this->error = FMT_UNEXPECTED_DIRECTIVE;
        return -1;
      }
      awaitingspec = 2;

      int d = c - '0';

      for (;;) {
        c = *this->fmtPtr++;
        switch (c) {
          case 's':
            directive->width = d;
            directive->type = DT_STRING;
            return 0;

          case 'l':
            if (*this->fmtPtr++ == 's') {
              directive->width = d;
              directive->type = DT_U8STRING;
              return 0;
            }

            directive->type = DT_UNEXPECTED;
            this->error = FMT_UNEXPECTED_DIRECTIVE;
            return -1;

          case '0'...'9':
            d = (10 * d) + (c - '0');
            continue;

          default:
            directive->type = DT_UNEXPECTED;
            this->error = FMT_UNEXPECTED_DIRECTIVE;
            return -1;
        }
      }

    }
    __attribute__((fallthrough));

    default:
      directive->type = DT_UNEXPECTED;
      this->error = FMT_UNEXPECTED_DIRECTIVE;
      return -1;
  }

  return 0;
}

int vformat (FormatType *this, va_list args) {
  if (this->bytes == NULL)
    this->mem_size = SIZE_MAX;

  if (this->mem_size == 0) return 0;

  if (this->mem_size == 1) {
    this->outputByte (this, '\0');
    return 0;
  }

  char bytebuf[8];
  char *str;
  size_t len;

  decimal_t dec = {.base = 10};

  DirectiveType directive;

  for (;;) {
    int r = next_directive (this, &directive);

    if (r == -1)
      return -1;

    int c = directive.type;

    switch (c) {
      case DT_EOF:
        goto theend;

      case DT_ORDINARY:
        str = this->fmtPtr - directive.size;
        len = directive.size;
        break;

      case DT_NSTRING:
        directive.precision = va_arg(args, int);
        __attribute__((fallthrough));

      case DT_STRING:
        str = (char *) va_arg(args, const char *);
        if (str == NULL)
          str = (char *) NULL_STRING;

        len = bytelen (str);

        if (directive.precision > 0 && len > directive.precision)
          len = directive.precision;

        break;

      case DT_FSTRING:
        directive.width = va_arg(args, int);

        str = (char*) va_arg(args, const char *);
        if (str == NULL)
          str = (char *) NULL_STRING;

        len = bytelen (str);
        break;

      case DT_U8FSTRING:
        directive.width = va_arg(args, int);
        __attribute__((fallthrough));

      case DT_U8STRING:
        str = (char *) va_arg(args, const char *);
        if (str == NULL)
          str = (char *) NULL_STRING;

        len = bytelen (str);

        if ((directive.precision > 0 && len < directive.precision) ||
             directive.precision == 0)
          directive.precision = len;

        char *sp = utf8_string_after_nth_char (str, directive.precision, &directive.numchars);
        len = sp - str;
        break;

      case DT_ESCAPED:
        str = (char *) "%";
        len = 1;
        break;

      case DT_INT_32: {
        int d;
        d = va_arg(args, int);
        str = int_to_string (&dec, d);
        len = dec.size;
        break;
      }

      case DT_INT_64: {
        int64_t i = va_arg(args, int64_t);
        str = int64_to_string (&dec, i);
        len = dec.size;
        break;
      }

      case DT_UINT_32: {
        unsigned int ui32 = va_arg(args, unsigned int);
        str = uint_to_string (&dec, ui32);
        len = dec.size;
        break;
      }

      case DT_UINT_64: {
        uint64_t u = va_arg(args, uint64_t);
        str = uint64_to_string (&dec, u);
        len = dec.size;
        break;
      }

      case DT_FLOAT: {
        double d = va_arg(args, double);

        if (directive.is_alternate)
          if (directive.precision == 0)
              directive.precision = 6;

        if (directive.precision > 16)
            directive.precision = 16;

        int is_neg = d < 0.;

        char *buf = dec.digits + is_neg;
        int n = float_to_string (buf, DECIMAL_NUM_DIGITS - is_neg, d, directive.precision);

        int i = 0;
        int j = n - 1;

        while (i < j) {
          char cc = buf[i];
          buf[i++] = buf[j];
          buf[j--] = cc;
        }

        buf -= is_neg;
        if (is_neg)
          *buf = '-';

        str = buf;
        len = n + is_neg;

        break;
      }

      case DT_BINARY: {
        uint64_t b = va_arg(args, uint64_t);
        str = uint64_to_binary_string (&dec, b);
        if (directive.is_alternate && *str != '0') {
          decimal_prepend (&dec, 'b');
          decimal_prepend (&dec, '0');
          str = get_decimal_string (&dec);
        }

        len = dec.size;
        break;
      }

      case DT_OCTAL: {
        uint64_t o = va_arg(args, uint64_t);
        str = uint64_to_octal_string (&dec, o);
        if (directive.is_alternate && *str != '0') {
          decimal_prepend (&dec, '0');
          str = get_decimal_string (&dec);
        }

        len = dec.size;
        break;
      }

      case DT_HEX: {
        uint64_t x = va_arg(args, uint64_t);
        str = uint64_to_hex_string (&dec, x);
        if (directive.is_alternate && *str != '0') {
          decimal_prepend (&dec, 'x');
          decimal_prepend (&dec, '0');
          str = get_decimal_string (&dec);
        }

        len = dec.size;
        break;
      }

      case DT_CHAR:
        *bytebuf = va_arg(args, int);
        str = bytebuf;
        len = 1;
        break;

      case DT_U8CHAR: {
        utf8 u =va_arg(args, int);
        len = utf8_character (u, bytebuf, 8);
        str = bytebuf;
        break;
      }

      case DT_PTR: {
        void *v = va_arg(args, void *);
        if (v == NULL) {
          str = (char *) NULL_STRING;
          len = bytelen (str);
        } else {
          str = ptr_to_string (&dec, v);
          len = dec.size;
        }
        break;
      }

      case DT_ERRNO:
        if (directive.is_alternate)
          str = errno_name (sys_errno);
        else
          str = errno_string (sys_errno);
        len = bytelen (str);
        break;

      case FMT_UNHANDLED_SPECIFIER:
      case DT_UNEXPECTED:
      default:
        this->outputByte (this, '\0');
        this->error = FMT_UNEXPECTED_DIRECTIVE;
        return -1;
    }

    if (directive.just == RIGHT_JUST && directive.width > 0 && len < directive.width) {
      if (directive.type == DT_U8STRING ||
          directive.type == DT_U8FSTRING) {
        if (directive.numchars < (int) directive.width)
          for (uint j = 0; j < directive.width - directive.numchars; j++)
            if (-1 == this->outputByte (this, ' '))
              return -1;
      } else {
        for (uint j = 0; j < directive.width - len; j++)
          if (-1 == this->outputByte (this, ' '))
            return -1;
      }
    }

    for (uint i = 0; i < len && *str; i++)
      if (-1 == this->outputByte (this, *str++))
        return -1;

    if (directive.just == LEFT_JUST && directive.width > 0 && len < directive.width) {
      if (directive.type == DT_U8STRING) {
        if (directive.numchars < (int) directive.width)
          for (uint j = 0; j < directive.width - directive.numchars; j++)
            if (-1 == this->outputByte (this, ' '))
              return -1;
      } else {
        for (uint j = 0; j < directive.width - len; j++)
          if (-1 == this->outputByte (this, ' '))
            return -1;
      }
    }
  }

theend:
  this->outputByte (this, '\0');
  return this->num_bytes;
}

int format_to_string (char *buf, size_t bufsize, const char *fmt, ...) {
  FormatType this = NewFormat (buf, bufsize, fmt);
  va_list ap;
  va_start(ap, fmt);
  int n = vformat (&this, ap);
  if (n < 0)
    tostderr ("error %d\n", this.error);
  va_end(ap);
  return n;
}

static int output_byte_to_fd (FormatType *fmt, int c) {
  if (c == '\0')
    return 0;

  char buf[1];
  buf[0] = c;

  int fd = *(int *) fmt->user_data;
  sys_write (fd, buf, 1);
  return 0;
}

int format_to_fd (int fd, const char *fmt, ...) {
  FormatType this = NewFormat (NULL, 0, fmt, .user_data = &fd,
      .outputByte = output_byte_to_fd);

  va_list ap;
  va_start(ap, fmt);
  int n = vformat (&this, ap);
  va_end(ap);

  return n;
}

int vformat_to_fd (int fd, const char *fmt, va_list ap) {
  FormatType this = NewFormat (NULL, 0, fmt, .user_data = &fd,
      .outputByte = output_byte_to_fd);

  return vformat (&this, ap);
}

/* test {
// num-tests: 6
#define REQUIRE_FORMAT
#define REQUIRE_STR_EQ
#define REQUIRE_BYTELEN
#define REQUIRE_ATOI
#define REQUIRE_STDIO

#include <libc.h>

#define FUNNAME "format_to_string()"

static int first_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "first_test, 2322 4d2 a α 100000000000 SUCCESS Success %\n";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%s, %o %x %c %lc %b %#m %m %%\n", __func__,
      1234, 1234, 97, 945, (uint64_t) 2048);
                   /* this cast is for clang */

  int eq = (n == (int) len);

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'first_test, 2322 4d2 a 100000000000 %%\n' got '%s'\n", a);

theend:
  return eq == 1 ? 0 : -1;
}

static int second_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "second_test";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%6s", __func__);

  int eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'second' got '%s'\n", a);
    goto theend;
  }

  n = format_to_string (a, 1024, "%*s", len, __func__);
  eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'second' got '%s'\n", a);

theend:
  return eq == 1 ? 0 : -1;
}

static int third_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "third";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%.5s",  __func__);

  int eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'third' got '%s'\n", a);
    goto theend;
  }

  n = format_to_string (a, 1024, "%.*s",  len, __func__);

  eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting 'third' got '%s'\n", a);

theend:
  return eq == 1 ? 0 : -1;
}

static int fourth_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "    fourth_test";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%15s",  __func__);

  int eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (0 == eq)
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%s' got '%s'\n", expected, a);

  const char *bexpected = "fourth_test    ";
  len = bytelen (bexpected);

  char b[1024];
  n = format_to_string (b, 1024, "%-15s",  __func__);

  eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (b, bexpected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%s' got '%s'\n", expected, a);

theend:
  return eq == 1 ? 0 : -1;
}

static int fifth_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "0b1111011";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%#b", 123);

  int eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (eq)
    tostdout ("\e[32m[OK]\e[m\n");
  else
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%s' got '%s'\n", expected, a);

theend:
  return eq == 1 ? 0 : -1;
}

static int sixth_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);
  const char *expected = "αabβγδ";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%.6ls", "αabβγδdςspπe");

  int eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (0 == eq)
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%s' got '%s'\n", expected, a);

  expected = "      αabβγδ";
  len = bytelen (expected);

  n = format_to_string (a, 1024, "%12ls", "αabβγδ");

  eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (0 == eq)
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%s' got '%s'\n", expected, a);

  n = format_to_string (a, 1024, "%*ls", 12, "αabβγδ");

  eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%d' bytes written, got '%d'\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (0 == eq)
    tostderr ("\e[31m[NOTOK]\e[m awaiting '%s' got '%s'\n", expected, a);
  else
    tostdout ("\e[32m[OK]\e[m\n");

theend:
  return eq == 1 ? 0 : -1;
}

static int seventh_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  const char *expected = "-123.456789100";
  size_t len = bytelen (expected);

  char a[1024];
  int n = format_to_string (a, 1024, "%.9f", -123.4567891);

  int eq = (n == (int) len);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n", len, n);
    goto theend;
  }

  eq = str_eq (a, expected);

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  const char *expectedInfN = "-INF";
  len = bytelen (expectedInfN);

  n = format_to_string (a, 1024, "%f", -INFINITY);

  eq = (n == (int) len);

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  eq = str_eq (a, expectedInfN);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  const char *expectedInf = "INF";
  len = bytelen (expectedInf);

  n = format_to_string (a, 1024, "%f", +INFINITY);

  eq = (n == (int) len);

  if (0 == eq) {
    tostderr ("\e[31m[SNOTOK]\e[m\n");
    goto theend;
  }

  eq = str_eq (a, expectedInf);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  const char *expectedNan = "NAN";
  len = bytelen (expectedNan);

  n = format_to_string (a, 1024, "%f", NAN);

  eq = (n == (int) len);

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  eq = str_eq (a, expectedNan);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  const char *expectedA = "0.9688";
  len = bytelen (expectedA);

  n = format_to_string (a, 1024, "%.4f",  0.96875);

  eq = (n == (int) len);

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  eq = str_eq (a, expectedA);
  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");

theend:
  return eq == 1 ? 0 : -1;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;
  int total = str_to_int (argv[1]);
  int failed = 0;

  total++;
  if (first_test (total) == -1) failed++;
  total++;
  if (second_test (total) == -1) failed++;
  total++;
  if (third_test (total) == -1) failed++;
  total++;
  if (fourth_test (total) == -1) failed++;
  total++;
  if (fifth_test (total) == -1) failed++;
  total++;
  if (sixth_test (total) == -1) failed++;
  total++;
  if (seventh_test (total) == -1) failed++;

  return failed;
}
} */
