#ifndef IO_H
#define IO_H

typedef struct io_fd_self {
  int (*read) (int, char *, idx_t);
  int (*write) (int, char *, idx_t);
} io_fd_self;

typedef struct io_self {
  io_fd_self fd;
} io_self;

typedef struct io_T {
  io_self self;
} io_T;

public io_T __init_io__ (void);

#endif /* IO_H */
