#ifndef FILE_HDR
#define FILE_HDR

typedef int (*FileReadLines_cb) (Vstring_t *, char *, size_t, int, void *);

#define FILE_TMPFNAME_UNLINK_FILE (1 << 0)
#define FILE_TMPFNAME_CLOSE_FD    (1 << 1)

typedef struct tmpfname_t {
  int fd;
  string_t *fname;
} tmpfname_t;

typedef struct file_tmpfname_self {
  tmpfname_t *(*new) (char *, char *);
  void (*release) (tmpfname_t *, int);
} file_tmpfname_self;

typedef struct file_mode_self {
  mode_t (*from_octal_string) (char *);
  char  *(*stat_to_string) (char *, mode_t);
} file_mode_self;

typedef struct file_self {
  file_tmpfname_self tmpfname;
  file_mode_self mode;

  int
    (*exists) (const char *);

  int
    (*is_lnk) (const char *),
    (*is_reg) (const char *),
    (*is_elf) (const char *),
    (*is_rwx) (const char *),
    (*is_sock) (const char *),
    (*is_readable) (const char *),
    (*is_writable) (const char *),
    (*is_executable) (const char *);

  size_t (*size) (const char *);

  ssize_t
    (*write) (char *, char *, ssize_t),
    (*append) (char *, char *, ssize_t);

  string_t *(*readlink) (const char *);

  Vstring_t
    *(*readlines) (char *, Vstring_t *, FileReadLines_cb, void *),
    *(*readlines_from_fp) (FILE *, Vstring_t *, FileReadLines_cb, void *);
} file_self;

typedef struct file_T {
  file_self self;
} file_T;

public file_T __init_file__ (void);

#endif /* FILE_HDR */
