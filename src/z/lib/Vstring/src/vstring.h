#ifndef VSTRING_HDR
#define VSTRING_HDR

typedef struct vstring_t vstring_t;

struct vstring_t {
  string_t *data;
  vstring_t *next;
  vstring_t *prev;
};

typedef struct Vstring_t {
  vstring_t *head;
  vstring_t *tail;
  vstring_t *current;
        int  cur_idx;
        int  num_items;
} Vstring_t;

typedef struct vstring_current_self {
  void
    (*append_with) (Vstring_t *, char *),
    (*prepend_with) (Vstring_t *, char *),
    (*append_with_len) (Vstring_t *, char *, size_t);
} vstring_current_self;

typedef struct vstring_get_self {
  size_t (*size) (Vstring_t *);
} vstring_get_self;

typedef struct vstring_add_self {
  vstring_t *(*sort_and_uniq) (Vstring_t *, char *bytes);
} vstring_add_self;

typedef struct vstring_to_self {
  char *(*cstring) (Vstring_t *, int);
} vstring_to_self;

typedef struct vstring_self {
  vstring_current_self current;
  vstring_add_self add;
  vstring_to_self to;
  vstring_get_self get;

  Vstring_t
    *(*new) (void),
    *(*dup) (Vstring_t *);

  vstring_t *(*new_item) (void);

  void
    (*release_item) (vstring_t *);

  void
    (*clear) (Vstring_t *),
    (*append) (Vstring_t *, vstring_t *),
    (*release) (Vstring_t *),
    (*remove_at) (Vstring_t *, int),
    (*append_uniq) (Vstring_t *, char *),
    (*append_with) (Vstring_t *, char *),
    (*prepend_with) (Vstring_t *, char *),
    (*append_with_len) (Vstring_t *, char *, size_t),
    (*append_with_fmt) (Vstring_t *, char *, ...);

  char **(*shallow_copy) (Vstring_t *, char **);

  vstring_t *(*pop_at) (Vstring_t *, int);

  string_t
    *(*join) (Vstring_t *, char *),
    *(*join_allocated) (Vstring_t *, char *, string_t *);
} vstring_self;

typedef struct vstring_T {
  vstring_self self;
} vstring_T;

public vstring_T __init_vstring__ (void);

#endif /* VSTRING_HDR */
