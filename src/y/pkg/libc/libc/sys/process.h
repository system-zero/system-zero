typedef struct Proc {
  int exitval;
  pid_t pid;

  char **argv;
  char **envp;

  uchar read_stdout;
  uchar print_stdout;
  int stdout_fd;
  string *stdout_string;

  char *redir_stdout_fname;
  int   redir_stdout_perm;
  int   redir_stdout_flags;
  int   redir_stdout_fd;

  uchar read_stderr;
  uchar print_stderr;
  int   stderr_fd;
  string *stderr_string;

  char *redir_stderr_fname;
  int   redir_stderr_perm;
  int   redir_stderr_flags;
  int   redir_stderr_fd;

  string *stdin_string;

  uchar set_sid;
  uchar set_pgid;
} Proc;

#define NewProc(...) (Proc) {        \
  .exitval = -1,                     \
  .pid = -1,                         \
  .argv = NULL,                      \
  .envp = NULL,                      \
  .read_stdout = 0,                  \
  .print_stdout = 0,                 \
  .stdout_fd = -1,                   \
  .stdout_string = NULL,             \
  .redir_stdout_fname = NULL,        \
  .redir_stdout_perm = 0700,         \
  .redir_stdout_flags = O_CREAT|O_TRUNC|O_WRONLY, \
  .redir_stdout_fd = -1,             \
  .read_stderr = 0,                  \
  .print_stderr = 0,                 \
  .stderr_fd = -1,                   \
  .stderr_string = NULL,             \
  .redir_stderr_fname = NULL,        \
  .redir_stderr_perm = 0700,         \
  .redir_stderr_flags = O_CREAT|O_TRUNC|O_WRONLY, \
  .redir_stderr_fd = -1,             \
  .stdin_string = NULL,              \
  .set_sid = 1,                      \
  .set_pgid = 1,                     \
  __VA_ARGS__ }
