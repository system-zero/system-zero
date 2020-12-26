#ifndef DIR_H
#define DIR_H

typedef struct dirwalk_t dirwalk_t;

typedef int  (*DirProcessDir_cb) (dirwalk_t *, char *, struct stat *);
typedef int  (*DirProcessFile_cb) (dirwalk_t *, char *, struct stat *);
typedef int  (*DirStatFile_cb) (const char *, struct stat *);

struct dirwalk_t {
  string_t *dir;

  Vstring_t *files;

  int
    orig_depth,
    depth,
    status;

  void *object;

  DirProcessDir_cb process_dir;
  DirProcessFile_cb process_file;
  DirStatFile_cb stat_file;
};

#define DIRLIST_DONOT_CHECK_DIRECTORY (1 << 0)

typedef struct dirlist_t dirlist_t;

struct dirlist_t {
  Vstring_t *list;
  char dir[PATH_MAX];
  void (*free) (dirlist_t *);
};

typedef struct dir_walk_self {
  dirwalk_t *(*new) (DirProcessDir_cb, DirProcessFile_cb);
  void (*free) (dirwalk_t **);
  int (*run) (dirwalk_t *, char *);
} dir_walk_self;

typedef struct dir_self {
  dir_walk_self walk;

  dirlist_t *(*list) (char *, int);
  char *(*current) (void);
  int (*is_directory) (char *);
} dir_self;

typedef struct dir_T {
  dir_self self;
} dir_T;

public dir_T __init_dir__ (void);

#endif /* DIR_H */
