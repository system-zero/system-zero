#ifndef LIBINPUT_H
#define LIBINPUT_H

typedef struct input_self {
  utf8 (*getkey) (int);
  int
    (*raw_mode) (int),
    (*orig_mode) (int);
} input_self;

typedef struct input_T {
  input_self self;
} input_T;

public input_T __init_input__ (void);

#endif /* LIBINPUT_H */
