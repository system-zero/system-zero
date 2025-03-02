#ifndef NULL_STRING
#define NULL_STRING "(nil)"
#endif

#define FMT_OUT_OF_SPACE         -2
#define FMT_UNEXPECTED_EOF       -3
#define FMT_UNHANDLED_SPECIFIER  -4
#define FMT_UNEXPECTED_DIRECTIVE -5

typedef enum {
  DT_EOF = -1,
  DT_ORDINARY,
  DT_STRING,
  DT_NSTRING,
  DT_FSTRING,
  DT_U8STRING,
  DT_U8FSTRING,
  DT_INT_32,
  DT_INT_64,
  DT_UINT_32,
  DT_UINT_64,
  DT_FLOAT,
  DT_CHAR,
  DT_U8CHAR,
  DT_BINARY,
  DT_OCTAL,
  DT_HEX,
  DT_PTR,
  DT_ERRNO,
  DT_ESCAPED,
  DT_UNEXPECTED
} Directive;

typedef struct DirectiveType {
  Directive type;
  char is_alternate;
  char just;
  uint width;
  uint precision;
   int numchars;
  uint size;
} DirectiveType;

typedef struct FormatType FormatType;

typedef int (*formatOutputByte) (FormatType *, int c);

struct FormatType {
  char *bytes;
  char *bytesPtr;
  const char *fmt;
  char *fmtPtr;
  size_t num_bytes;
  size_t mem_size;
  DirectiveType *directive;
  formatOutputByte outputByte;
  int error;
  void *user_data;
};

#define NewFormat(__bytes__, __buf_len__, __fmt__, ...) (FormatType) { \
  .bytes = __bytes__, .bytesPtr = __bytes__,                           \
  .fmt = __fmt__, .fmtPtr = (char *) __fmt__,                          \
  .num_bytes = 0, .mem_size = __buf_len__,                             \
  .directive = NULL, .outputByte = format_output_byte,                 \
  .error = 0, .user_data = NULL, __VA_ARGS__}

#ifndef tostdout
#define tostdout(fmt, ...) format_to_fd (1, fmt, ##__VA_ARGS__)
#endif

#ifndef tostderr
#define tostderr(fmt, ...) format_to_fd (2, fmt, ##__VA_ARGS__)
#endif

