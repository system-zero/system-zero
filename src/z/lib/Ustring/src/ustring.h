#ifndef USTRING_HDR
#define USTRING_HDR

#ifndef TO_LOWER
#define TO_LOWER 0
#endif

#ifndef TO_UPPER
#define TO_UPPER 1
#endif

typedef struct ustring_t ustring_t;
struct ustring_t {
  utf8 code;
  char buf[5];
  int
    len,
    width;

  ustring_t
    *next,
    *prev;
};

typedef struct Ustring_t {
  ustring_t *head;
  ustring_t *tail;
  ustring_t *current;
      int  cur_idx;
      int  num_items;
      int  len;
} Ustring_t;

typedef struct ustring_get_self {
  utf8
    (*code) (char *),
    (*code_at) (char *, size_t, int, int *),
    (*nth_character_code) (char *, size_t, int);

  int (*num_characters) (char *, size_t);
} ustring_get_self;

typedef struct ustring_self {
  ustring_get_self get;

  Ustring_t *(*new) (void);
  void
    (*release) (Ustring_t *),
    (*release_members) (Ustring_t *);

  ustring_t *(*encode) (Ustring_t *, const char *, size_t, int, int, int);

  char
     *(*character) (utf8, char *, int *),
     *(*at_nth_character) (char *, size_t, int);

  int
    (*width) (char *, int),
    (*charlen) (uchar),
    (*is_lower) (utf8),
    (*is_upper) (utf8),
    (*char_num) (char *, int),
    (*swap_case) (char *, char *, size_t),
    (*change_case) (char *, char *, size_t, int),
    (*is_nth_character_at) (char *, size_t, int);

  size_t (*validate) (unsigned char *, size_t, char **, int *);

  utf8
    (*to_lower) (utf8),
    (*to_upper) (utf8);
} ustring_self;

typedef struct ustring_T {
  ustring_self self;
} ustring_T;

public ustring_T __init_ustring__ (void);

#endif /* USTRING_HDR */
