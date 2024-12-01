// provides: int format_to_string (char *, size_t, const char *, ...)
// provides: int format_to_fd (int, const char *, ...)
// provides: int vformat (FormatType *, va_list)
// requires: string/bytelen.c
// requires: unistd/write.c
// requires: convert/decimal_to_string.c
// requires: convert/format.h

/* a minimal format function
   initially based on: https://github.com/tatetian/mini-snprintf
   added support for the following specifiers:
     'c', 'b', 'x', 'o', 'p', '.*s'

   also, this implementation sends its byte to an output function.
   The default function writes the byte to its buffer argument.

   A function that outputs to a file descriptor it is also included
   in the interface, thus it can be used to output to streams.
*/

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
  return 0;
}

static int next_directive (FormatType *this, DirectiveType *directive) {
  directive->size = 0;

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
  int c = *this->fmtPtr++;

  switch (c) {
    case '\0':
      this->error = FMT_UNEXPECTED_EOF;
      return -1;

    case '%':
      directive->type = DT_ESCAPED;
      return ++directive->size;

    case 's':
      directive->type = DT_STRING;
      return ++directive->size;

    case 'l':
      directive->size++;

      switch (*this->fmtPtr++) {
        case 'd':
          directive->type = DT_INT_64;
          return ++directive->size;

        case 'u':
          directive->type = DT_UINT_64;
          return ++directive->size;

        default:
          this->error = FMT_UNHANDLED_SPECIFIER;
          return -1;
      }

    case 'd':
      directive->type = DT_INT_32;
      return ++directive->size;

    case 'u':
      directive->type = DT_UINT_32;
      return ++directive->size;

    case 'p':
      directive->type = DT_PTR;
      return ++directive->size;

    case 'c':
      directive->type = DT_BYTE;
      return ++directive->size;

    case 'b':
      directive->type = DT_BINARY;
      return ++directive->size;

    case 'o':
      directive->type = DT_OCTAL;
      return ++directive->size;

    case 'x':
      directive->type = DT_HEX;
      return ++directive->size;

    case '.':
      directive->size++;

      switch (*this->fmtPtr++) {
        case '*':
          directive->size++;
          switch (*this->fmtPtr++) {
            case 's':
              directive->type = DT_NSTRING;
              return ++directive->size;

            default:
              break;
          }

         // fallthrough
        default:
          this->error = FMT_UNHANDLED_SPECIFIER;
          return -1;
      }
      break;

    default:
      directive->type = DT_UNEXPECTED;
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

  char bytebuf[1];
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

      case DT_STRING:
        str = (char*) va_arg(args, const char *);
        if (str == NULL)
          str = (char *) NULL_STRING;

        len = bytelen (str);
        break;

      case DT_NSTRING:
        len = va_arg(args, int);

        str = (char*) va_arg(args, const char *);
        if (str == NULL)
          str = (char *) NULL_STRING;

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

      case DT_BINARY: {
        uint64_t b = va_arg(args, uint64_t);
        str = uint64_to_binary_string (&dec, b);
        len = dec.size;
        break;
      }

      case DT_OCTAL: {
        uint64_t o = va_arg(args, uint64_t);
        str = uint64_to_octal_string (&dec, o);
        len = dec.size;
        break;
      }

      case DT_HEX: {
        uint64_t x = va_arg(args, uint64_t);
        str = uint64_to_hex_string (&dec, x);
        len = dec.size;
        break;
      }

      case DT_BYTE:
        *bytebuf = va_arg(args, int);
        str = bytebuf;
        len = 1;
        break;

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

      default:
        this->error = FMT_UNEXPECTED_DIRECTIVE;
        return -1;
    }

    for (size_t i = 0; i < len; i++) {
      if (-1 == this->outputByte (this, *str++)) {
        this->outputByte (this, '\0');
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
