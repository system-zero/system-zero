#define FILEREMOVE_NO_VERBOSE       0

typedef int (*FileRemoveInteractive) (const char *, const char *, void *);

typedef struct file_remove_opts {
  int
    force,
    verbose,
    maxdepth,
    curdepth,
    recursive,
    interactive;

  int out_fd,
      err_fd;

  FileRemoveInteractive on_interactive;

} file_remove_opts;

#ifndef OPT_NO_FORCE
#define OPT_NO_FORCE       0
#endif

#ifndef OPT_VERBOSE_ON_ERROR
#define OPT_VERBOSE_ON_ERROR 1
#endif

#ifndef OPT_MAXDEPTH
#define OPT_MAXDEPTH 1024
#endif

#ifndef OPT_NO_RECURSIVE
#define OPT_NO_RECURSIVE   0
#endif

#ifndef OPT_NO_INTERACTIVE
#define OPT_NO_INTERACTIVE 0
#endif

#define FileRemoveOpts(...) (file_remove_opts) { \
  .force = OPT_NO_FORCE,                         \
  .verbose = OPT_VERBOSE_ON_ERROR,               \
  .maxdepth = OPT_MAXDEPTH,                      \
  .curdepth = 0,                                 \
  .recursive = OPT_NO_RECURSIVE,                 \
  .interactive = OPT_NO_INTERACTIVE,             \
  .out_fd = 1,                                   \
  .err_fd = 2,                                   \
  .on_interactive = NULL,                        \
  __VA_ARGS__}
