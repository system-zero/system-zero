#define NULL_STRING "(nil)"

typedef enum {
  DT_EOF = -1,
  DT_ORDINARY,
  DT_STRING,
  DT_NSTRING,
  DT_INT_32,
  DT_INT_64,
  DT_UINT_32,
  DT_UINT_64,
  DT_BYTE,
  DT_BINARY,
  DT_OCTAL,
  DT_HEX,
  DT_PTR,
  DT_ESCAPED,
  DT_UNEXPECTED
} Directive;

typedef struct DirectiveType {
  Directive type;
  int is_alternate;
  uint width;
  uint precision;
  size_t size;
} DirectiveType;

typedef struct FormatType FormatType;

typedef int (*formatOutputByte) (FormatType *, int c);

#define FMT_OUT_OF_SPACE         -2
#define FMT_UNEXPECTED_EOF       -3
#define FMT_UNHANDLED_SPECIFIER  -4
#define FMT_UNEXPECTED_DIRECTIVE -5

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

