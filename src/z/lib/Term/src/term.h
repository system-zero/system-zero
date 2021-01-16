#ifndef TERM_HDR
#define TERM_HDR

typedef struct term_t {
  int is_initialized;

  char
     mode,
    *name;

  int
    state,
    in_fd,
    out_fd,
    orig_curs_row_pos,
    orig_curs_col_pos,
    num_rows,
    num_cols;

  struct termios
    orig_mode,
    raw_mode;

} term_t;


typedef struct term_cursor_self {
  void
    (*hide) (term_t *),
    (*show) (term_t *),
    (*save) (term_t *),
    (*set_pos) (term_t *, int, int),
    (*restore) (term_t *);

 int
   (*get_pos) (term_t *, int *, int *);
} term_cursor_self;

typedef struct term_screen_self {
  void
    (*bell)    (term_t *),
    (*save)    (term_t *),
    (*clear)   (term_t *),
    (*restore) (term_t *),
    (*clear_eol) (term_t *),
    (*set_color) (term_t *, int);
} term_screen_self;

typedef struct term_self {
  term_screen_self screen;
  term_cursor_self cursor;

  void
    (*release)   (term_t **),
    (*init_size) (term_t *, int *, int *),
    (*set_state_bit) (term_t *, int),
    (*unset_state_bit) (term_t *, int);

  int
    (*set) (term_t *),
    (*reset) (term_t *),
    (*set_mode) (term_t *, char),
    (*raw_mode)  (term_t *),
    (*orig_mode) (term_t *),
    (*sane_mode) (term_t *);

  term_t *(*new) (void);
} term_self;

typedef struct term_T {
  term_self self;
} term_T;

public term_T __init_term__ (void);

#endif /* TERM_HDR */
