
#define BUFSIZ 1024

#define _IONBF 0
#define _IOLBF 1
#define _IOFBF 2

typedef struct _FILE {
  int   fd;
  int   bufferingType;

  int   out_bufsize;
  int   out_buflen;
  char *out_buf;

  int   in_bufsize;
  int   in_buflen;
  char *in_buf;
  int   in_cur;
  int   in_prevbyte;

  void *user_data;
} FILE;

FILE *sys_stdin;
FILE *sys_stdout;
FILE *sys_stderr;
