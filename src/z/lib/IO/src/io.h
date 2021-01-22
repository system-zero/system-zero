#ifndef IO_H
#define IO_H

typedef utf8 (*IOGetkey) (int);

typedef struct io_fd_self {
  idx_t
    (*read) (int, char *, idx_t),
    (*write) (int, char *, idx_t);
} io_fd_self;

typedef struct io_self {
  io_fd_self fd;

  idx_t
    (*print) (const char *),
    (*print_fmt) (const char *, ...);

  utf8 (*getkey) (int);
  string_t *(*parse_escapes) (char *);
} io_self;

typedef struct io_T {
  io_self self;
} io_T;

public io_T __init_io__ (void);

#endif /* IO_H */
