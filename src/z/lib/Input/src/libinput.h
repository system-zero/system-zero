#ifndef LIBINPUT_H
#define LIBINPUT_H

#ifndef BACKSPACE_KEY
#define BACKSPACE_KEY   010
#endif

#ifndef ESCAPE_KEY
#define ESCAPE_KEY      033
#endif

#ifndef ARROW_DOWN_KEY
#define ARROW_DOWN_KEY  0402
#endif

#ifndef ARROW_UP_KEY
#define ARROW_UP_KEY    0403
#endif

#ifndef ARROW_LEFT_KEY
#define ARROW_LEFT_KEY  0404
#endif

#ifndef ARROW_RIGHT_KEY
#define ARROW_RIGHT_KEY 0405
#endif

#ifndef HOME_KEY
#define HOME_KEY        0406
#endif

#ifndef FN_KEY
#define FN_KEY(x)       (x + 0410)
#endif

#ifndef DELETE_KEY
#define DELETE_KEY      0512
#endif

#ifndef INSERT_KEY
#define INSERT_KEY      0513
#endif

#ifndef PAGE_DOWN_KEY
#define PAGE_DOWN_KEY   0522
#endif

#ifndef PAGE_UP_KEY
#define PAGE_UP_KEY     0523
#endif

#ifndef END_KEY
#define END_KEY         0550
#endif

#ifndef CTRL
#define CTRL(X) (X & 037)
#endif

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
