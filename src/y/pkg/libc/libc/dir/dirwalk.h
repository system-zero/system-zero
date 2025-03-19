#define DIRWALK_MAX_DEPTH 1024
#define DIRWALK_MAXLEN_ERROR_MSG 256
#define DIRWALK_ERROR   -1
#define DIRWALK_SKIP     0
#define DIRWALK_PROCESS  1
#define DIRWALK_STOP     2

typedef struct dirwalk_t dirwalk_t;
typedef long (*DirStatFile_cb) (const char *, struct stat *);
typedef int (*DirProcessDir_cb) (dirwalk_t *, const char *, size_t, int);
typedef int (*DirProcessFile_cb) (dirwalk_t *, const char *, size_t, int);
typedef int (*DirWalkOnError_cb) (dirwalk_t *, const char *, const char *, int);

struct dirwalk_t {
  int
    depth,
    status,
    orig_depth;

  char error_msg[DIRWALK_MAXLEN_ERROR_MSG];

  string *dir;

  string **entries;
  uint num_entries;
  uint mem_entries;

  int *types;
  void *user_data;

  DirProcessDir_cb  process_dir;
  DirProcessFile_cb process_file;
  DirStatFile_cb    stat_file;
  DirWalkOnError_cb on_error;
};
