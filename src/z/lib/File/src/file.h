#ifndef FILE_HDR
#define FILE_HDR

typedef int (*FileReadLines_cb) (Vstring_t *, char *, size_t, int, void *);

typedef int (*FileInteractive) (const char *, const char *);

typedef struct file_copy_opts {
  int
    all,
    force,
    backup,
    update,
    verbose,
    preserve,
    maxdepth,
    curdepth,
    recursive,
    dereference,
    interactive;

  const char *backup_suffix;

  FILE
    *out_stream,
    *err_stream;

  FileInteractive on_interactive;

} file_copy_opts;

#define FileCopyOpts(...) (file_copy_opts) { \
  .all = OPT_NO_ALL,                         \
  .force = OPT_NO_FORCE,                     \
  .backup = OPT_NO_BACKUP,                   \
  .update = OPT_NO_UPDATE,                   \
  .verbose = OPT_VERBOSE_ON_ERROR,           \
  .preserve = OPT_NO_PRESERVE,               \
  .maxdepth = OPT_MAXDEPTH,                  \
  .curdepth = 0,                             \
  .recursive = OPT_NO_RECURSIVE,             \
  .dereference = OPT_NO_DEREFERENCE,         \
  .interactive = OPT_NO_INTERACTIVE,         \
  .backup_suffix = "~",                      \
  .out_stream = stdout,                      \
  .err_stream = stderr,                      \
  .on_interactive = NULL,                    \
  __VA_ARGS__}

typedef struct file_remove_opts {
  int
    force,
    verbose,
    maxdepth,
    curdepth,
    recursive,
    interactive;

  FILE
    *out_stream,
    *err_stream;

  FileInteractive on_interactive;

} file_remove_opts;

#define FileRemoveOpts(...) (file_remove_opts) { \
  .force = OPT_NO_FORCE,                         \
  .verbose = OPT_VERBOSE_ON_ERROR,               \
  .maxdepth = OPT_MAXDEPTH,                      \
  .curdepth = 0,                                 \
  .recursive = OPT_NO_RECURSIVE,                 \
  .interactive = OPT_NO_INTERACTIVE,             \
  .out_stream = stdout,                          \
  .err_stream = stderr,                          \
  .on_interactive = NULL,                        \
  __VA_ARGS__}

typedef struct file_rename_opts {
  int
    force,
    backup,
    verbose,
    interactive;

  const char *backup_suffix;

  FILE
    *out_stream,
    *err_stream;

  FileInteractive on_interactive;

} file_rename_opts;

#define FileRenameOpts(...) (file_rename_opts) { \
  .force = OPT_NO_FORCE,                     \
  .backup = OPT_NO_BACKUP,                   \
  .verbose = OPT_VERBOSE_ON_ERROR,           \
  .interactive = OPT_NO_INTERACTIVE,         \
  .backup_suffix = "~",                      \
  .out_stream = stdout,                      \
  .err_stream = stderr,                      \
  .on_interactive = NULL,                    \
  __VA_ARGS__}

#define FILE_TMPFNAME_UNLINK_FILE (1 << 0)
#define FILE_TMPFNAME_CLOSE_FD    (1 << 1)

typedef struct tmpfname_t {
  int fd;
  string *fname;
} tmpfname_t;

typedef struct file_tmpfname_self {
  tmpfname_t *(*new) (const char *, const char *);
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
    (*copy) (const char *, const char *, file_copy_opts),
    (*remove) (const char *, file_remove_opts),
    (*rename) (const char *, const char *, file_rename_opts),
    (*exists) (const char *),
    (*is_lnk) (const char *),
    (*is_reg) (const char *),
    (*is_elf) (const char *),
    (*is_rwx) (const char *),
    (*is_sock) (const char *),
    (*is_fifo) (const char *),
    (*is_readable) (const char *),
    (*is_writable) (const char *),
    (*is_executable) (const char *);

  size_t (*size) (const char *);

  ssize_t
    (*write) (char *, char *, ssize_t),
    (*append) (char *, char *, ssize_t);

  string_t
    *(*readlink) (const char *);

  Vstring_t
    *(*readlines) (char *, Vstring_t *, FileReadLines_cb, void *),
    *(*readlines_from_fp) (FILE *, Vstring_t *, FileReadLines_cb, void *);
} file_self;

typedef struct file_T {
  file_self self;
} file_T;

public file_T __init_file__ (void);

#endif /* FILE_HDR */
