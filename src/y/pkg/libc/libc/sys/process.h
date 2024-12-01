typedef struct Proc {
  int retval;
  pid_t pid;

  char **argv;
  char **envp;

  int read_stdout;
  int print_stdout;
  int stdout_fd;
  FILE *stdout_fp;
  string stdout_string;
  char redir_stdout_mode[4];
  char *redir_stdout;

  int read_stderr;
  int print_stderr;
  int stderr_fd;
  FILE *stderr_fp;
  string stderr_string;
  char redir_stderr_mode[4];
  char *redir_stderr;

  int set_sid;
  int set_pgid;
} Proc;

#define NewProc(...) (Proc) {        \
  .retval = -1,                      \
  .pid = -1,                         \
  .argv = NULL,                      \
  .envp = NULL,                      \
  .read_stdout = 0,                  \
  .print_stdout = 0,                 \
  .stdout_fd = -1,                   \
  .stdout_fp = NULL,                 \
  .stdout_string = (string) {.bytes = NULL, .mem_size = 0, .num_bytes = 0}, \
  .redir_stdout_mode = "w",          \
  .redir_stdout = NULL,              \
  .read_stderr = 0,                  \
  .print_stderr = 0,                 \
  .stderr_fd = -1,                   \
  .stderr_fp = NULL,                 \
  .stderr_string = (string) {.bytes = NULL, .mem_size = 0, .num_bytes = 0}, \
  .redir_stderr_mode = "w",          \
  .redir_stderr = NULL,              \
  .set_sid = 1,                      \
  .set_pgid = 1,                     \
  __VA_ARGS__ }
