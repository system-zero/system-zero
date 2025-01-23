#define DIRWALK_MAX_DEPTH 1024
#define DIRWALK_MAXLEN_ERROR_MSG 256

typedef struct dirwalk_t dirwalk_t;
typedef long (*DirStatFile_cb) (const char *, struct stat *);
typedef int (*DirProcessDir_cb) (dirwalk_t *, const char *, size_t, struct stat *);
typedef int (*DirProcessFile_cb) (dirwalk_t *, const char *, size_t, struct stat *);
typedef int (*DirWalkOnError_cb) (dirwalk_t *, const char *, const char *, int);

struct dirwalk_t {
  int
    depth,
    status,
    realpath,
    orig_depth;

  char error_msg[DIRWALK_MAXLEN_ERROR_MSG];

  string *dir;

  string **entries;
  uint num_entries;
  uint mem_entries;

  void *user_data;

  DirProcessDir_cb  process_dir;
  DirProcessFile_cb process_file;
  DirStatFile_cb    stat_file;
  DirWalkOnError_cb on_error;
};
