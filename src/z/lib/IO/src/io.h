#ifndef IO_HDR
#define IO_HDR

typedef utf8 (*IOGetkey) (int);

typedef struct io_fd_self {
  idx_t
    (*read) (int, char *, idx_t),
    (*write) (int, char *, idx_t);
} io_fd_self;

typedef struct io_err_self {
  idx_t
    (*print) (const char *),
    (*print_fmt) (const char *, ...);
} io_err_self;

typedef struct io_out_self {
  idx_t
    (*print) (const char *),
    (*print_fmt) (const char *, ...);
} io_out_self;

typedef struct io_input_self {
  utf8 (*getkey) (int);
} io_input_self;

typedef struct io_self {
  io_fd_self fd;
  io_err_self err;
  io_out_self out;
  io_input_self input;

  string_t *(*parse_escapes) (char *);
} io_self;

typedef struct io_T {
  io_self self;
} io_T;

public io_T __init_io__ (void);

#endif /* IO_HDR */
