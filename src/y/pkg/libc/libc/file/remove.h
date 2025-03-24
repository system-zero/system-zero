#define FORCE_ON        true
#define FORCE_OFF       false

#define VERBOSE_OFF     true
#define VERBOSE_ON      false

#define RECURSIVE_ON    true
#define RECURSIVE_OFF   false

#define INTERACTIVE_ON  true
#define INTERACTIVE_OFF false

#ifndef OPT_MAXDEPTH
#define OPT_MAXDEPTH 1024
#endif

typedef int (*FileRemoveInteractive) (const char *, const char *, void *);

typedef struct file_remove_opts {
  bool
    force,
    verbose,
    verbose_on_error,
    recursive,
    interactive;

  int
    maxdepth,
    curdepth;

  int
    out_fd,
    err_fd;

  FileRemoveInteractive on_interactive;

} file_remove_opts;

#define FileRemoveOpts(...) (file_remove_opts) { \
  .force = FORCE_OFF,                            \
  .verbose = VERBOSE_OFF,                        \
  .verbose_on_error = VERBOSE_ON,                \
  .recursive = RECURSIVE_OFF,                    \
  .interactive = INTERACTIVE_OFF,                \
  .maxdepth = OPT_MAXDEPTH,                      \
  .curdepth = 0,                                 \
  .out_fd = 1,                                   \
  .err_fd = 2,                                   \
  .on_interactive = NULL,                        \
  __VA_ARGS__}
