
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

  int   pid; // popen

} FILE;

FILE *sys_stdin;
FILE *sys_stdout;
FILE *sys_stderr;

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

#ifndef WITHOUT_STDIO_INTERFACE
#define tostderr(_fmt_, ...) sys_fprintf (sys_stderr, _fmt_, ##__VA_ARGS__)
#define tostdout(_fmt_, ...) sys_fprintf (sys_stdout, _fmt_, ##__VA_ARGS__)
#endif /* WITHOUT_STDIO_INTERFACE */

