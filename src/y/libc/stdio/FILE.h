#define BUFSZ 1024
typedef struct FILE {
  int fd;
  int back;           /* pushback buffer */
  char *ibuf, *obuf;  /* input/output buffer */
  int isize, osize;   /* ibuf size */
  int ilen, olen;     /* length of data in buf */
  int iown, oown;     /* free the buffer when finished */
  int icur;           /* current position in ibuf */
  int ostat;
  int pid;
} FILE;
