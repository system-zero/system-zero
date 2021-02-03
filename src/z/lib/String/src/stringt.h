#ifndef STRING_H
#define STRING_H

typedef struct string_t {
  size_t  num_bytes;
  size_t  mem_size;
    char *bytes;
} string_t;

typedef struct string_self {
  void
     (*clear) (string_t *),
     (*release) (string_t *),
     (*clear_at) (string_t *, int);

  string_t
    *(*new) (size_t),
    *(*dup) (string_t *),
    *(*reallocate) (string_t *, size_t),
    *(*new_with) (const char *),
    *(*new_with_len) (const char *, size_t),
    *(*new_with_fmt) (const char *, ...),
    *(*append_byte) (string_t *, char),
    *(*prepend_byte) (string_t *, char),
    *(*append_with) (string_t *, const char *),
    *(*append_with_fmt) (string_t *, const char *, ...),
    *(*append_with_len) (string_t *, const char *, size_t),
    *(*prepend_with) (string_t *, const char *),
    *(*prepend_with_fmt) (string_t *, const char *, ...),
    *(*insert_at_with) (string_t *, int, const char *),
    *(*insert_at_with_len) (string_t *, int, const char *, size_t),
    *(*insert_byte_at) (string_t *, char c, int),
    *(*replace_with) (string_t *, char *),
    *(*replace_with_len) (string_t *, const char *, size_t),
    *(*replace_with_fmt) (string_t *, const char *, ...),
    *(*trim_end) (string_t *, char c),
    *(*replace_numbytes_at_with) (string_t *, int, int, const char *);

  int (*delete_numbytes_at) (string_t *, int, int);
} string_self;

typedef struct string_T {
  string_self self;
} string_T;

public string_T __init_string__ (void);

#endif /* STRING_H */
