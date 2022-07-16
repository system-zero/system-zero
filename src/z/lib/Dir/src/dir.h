#ifndef DIR_HDR
#define DIR_HDR

#define DIRWALK_MAX_DEPTH 1024
#define NOT_ALLOWED_IN_A_DIRECTORY_NAME "\t\n"

typedef struct dirwalk_t dirwalk_t;
typedef struct dir_opts  dir_opts;

typedef int (*DirStatFile_cb) (const char *, struct stat *);
typedef int (*DirProcessDir_cb) (dirwalk_t *, const char *, struct stat *);
typedef int (*DirProcessFile_cb) (dirwalk_t *, const char *, struct stat *);
typedef int (*DirWalkOnError_cb) (dirwalk_t *, const char *, const char *, int);
typedef int (*DirMake) (const char *, mode_t, dir_opts);
typedef int (*DirRm) (const char *, dir_opts);

typedef int (*PrintMsg_cb) (FILE *fp, const char *fmt, ...);
typedef       PrintMsg_cb   PrintErr_cb;
typedef       PrintMsg_cb   PrintWarn_cb;

typedef       PrintMsg_cb   DirPrintMsg_cb;
typedef       PrintMsg_cb   DirPrintErr_cb;
typedef       PrintMsg_cb   DirPrintWarn_cb;

/* TODO: add user defined objects */
struct dir_opts {
  int
    err,
    msg,
    warn;

  PrintErr_cb  err_cb;
  PrintMsg_cb  msg_cb;
  PrintWarn_cb warn_cb;
};

#define DirOpts(...) (dir_opts) { \
  .err = 1,                       \
  .msg = 0,                       \
  .warn = 0,                      \
  .err_cb = fprintf,              \
  .msg_cb = fprintf,              \
  .warn_cb = fprintf,             \
  __VA_ARGS__                     \
}

struct dirwalk_t {
  int
    depth,
    status,
    orig_depth;

  string_t *dir;
  Vstring_t *files;

  void *user_data;

  DirProcessDir_cb  process_dir;
  DirProcessFile_cb process_file;
  DirStatFile_cb    stat_file;
  DirWalkOnError_cb on_error;
};

#define DIRLIST_DONOT_CHECK_DIRECTORY (1 << 0)
#define DIRLIST_LONG_FORMAT           (1 << 1)
#define DIRLIST_LNK_IS_DIRECTORY      (1 << 2)

typedef struct dirlist_t dirlist_t;

struct dirlist_t {
  Vstring_t *list;
  char dir[PATH_MAX];
  void (*release) (dirlist_t *);
};

typedef struct dir_walk_self {
  dirwalk_t *(*new) (DirProcessDir_cb, DirProcessFile_cb);
  void (*release) (dirwalk_t **);
  int (*run) (dirwalk_t *, const char *);
} dir_walk_self;

typedef struct dir_self {
  dir_walk_self walk;

  char *(*current) (void);

  DirMake make;
  DirMake make_parents;
  DirRm   rm;
  DirRm   rm_parents;
  int
    (*is_directory) (const char *),
    (*lnk_is_directory) (const char *);

  dirlist_t *(*list) (const char *, int);

} dir_self;

typedef struct dir_T {
  dir_self self;
} dir_T;

public dir_T __init_dir__ (void);

#endif /* DIR_HDR */
