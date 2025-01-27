// as: filecopy
typedef struct file_copy_opts file_copy_opts;
typedef int (*FileInteractive) (const char *, const char *,  file_copy_opts);

struct file_copy_opts {
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

  int
    out_fd,
    err_fd;

  FileInteractive on_interactive;
};

#define FILECOPY_NO_VERBOSE       0
#define FILECOPY_VERBOSE_ON_ERROR 1
#define FILECOPY_VERBOSE          2
#define FILECOPY_VERBOSE_EXTRA    3
#define FILECOPY_NO_PRESERVE      0
#define FILECOPY_PRESERVE         1
#define FILECOPY_PRESERVE_OWNER   2
#define FILECOPY_MAXDEPTH         1024

#define FILECOPY_DEST_EXISTS      -2

#define FileCopyOpts(...) (file_copy_opts) { \
  .all = false,                              \
  .force = false,                            \
  .backup = false,                           \
  .update = false,                           \
  .verbose = FILECOPY_VERBOSE_ON_ERROR,      \
  .preserve = FILECOPY_NO_PRESERVE,          \
  .maxdepth = FILECOPY_MAXDEPTH,             \
  .curdepth = 0,                             \
  .recursive = false,                        \
  .dereference = false,                      \
  .interactive = false,                      \
  .backup_suffix = "~",                      \
  .out_fd = 1,                               \
  .err_fd = 2,                               \
  .on_interactive = NULL,                    \
  __VA_ARGS__}
