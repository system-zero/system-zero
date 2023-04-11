// provides: FILE *sys_fopen (const char *, const char *)
// provides: int sys_fprintf (FILE *, const char *, ...)
// provides: int sys_fclose  (FILE *)
// provides: int sys_fflush  (FILE *)
// provides: size_t sys_fwrite (const void *, size_t, size_t, FILE *)
// provides: size_t sys_fread (void *, size_t, size_t, FILE *)
// requires: stdio/stdio.h
// requires: string/str_chr.c
// requires: string/mem_set.c
// requires: string/fmt.h
// requires: string/vsnprintf.c
// requires: stdlib/alloc.c
// requires: unistd/write.c
// requires: unistd/read.c
// requires: unistd/fcntl.h
// requires: unistd/open.c
// requires: unistd/close.c

static char
   _ibuf[BUFSIZ],
   _obuf[BUFSIZ],
   _ebuf[BUFSIZ];

static FILE _stdin  = {0,      0,      0, 0,  NULL, BUFSIZ, 0, _ibuf, 0};
static FILE _stdout = {1, _IOLBF, BUFSIZ, 0, _obuf,      0, 0,  NULL, 0};
static FILE _stderr = {2, _IONBF, BUFSIZ, 0, _ebuf,      0, 0,  NULL, 0};

FILE *sys_stdin  = &_stdin;
FILE *sys_stdout = &_stdout;
FILE *sys_stderr = &_stderr;

FILE *sys_fopen (const char *path, const char *mode) {
  FILE *fp;
  int flags;

  if (str_chr(mode, '+'))
    flags = O_RDWR;
  else
    flags = *mode == 'r' ? O_RDONLY : O_WRONLY;

  if (*mode != 'r') flags |= O_CREAT;
  if (*mode == 'w') flags |= O_TRUNC;
  if (*mode == 'a') flags |= O_APPEND;

  fp = Alloc (sizeof (FILE));
  mem_set(fp, 0, sizeof (FILE));

  fp->fd = sys_open3 (path, flags, 0600);

  if (fp->fd < 0) {
    Release(fp);
    return NULL;
  }

  fp->out_buflen = 0;
  fp->out_bufsize = BUFSIZ;
  fp->out_buf = Alloc (fp->out_bufsize);

  fp->in_bufsize = BUFSIZ;
  fp->in_buflen = 0;
  fp->in_cur = 0;
  fp->in_buf = Alloc (fp->in_bufsize);

  fp->bufferingType = _IOFBF;

  return fp;
}

int sys_fclose (FILE *fp) {
  int ret = sys_close (fp->fd);

  Release (fp->out_buf);
  Release (fp->in_buf);
  Release (fp);

  return ret;
}

int sys_fflush (FILE *fp) {
  if (fp->fd < 0) return 0;

  // we may want to make sure that this is an uncovered error by checking errno 
  // probably a higher interface but elsewhere not here
  if (sys_write (fp->fd, fp->out_buf, fp->out_buflen) isnot fp->out_buflen)
    return EOF;

  fp->out_buflen = 0;

  return 0;
}

static int fileptr_append_or_flush (FILE *fp, int c) {
  int not_flush = fp->out_bufsize - fp->out_buflen;

  if (not_flush) {
    fp->out_buf[fp->out_buflen++] = c;
    not_flush--;
  }

  if (c is '\n' or 0 is not_flush)
    if (sys_fflush (fp)) return EOF;

  return 0; // so to be able to set the counter properly, assuming fflush succeeded 
}

static void fileptrWriteBuffered (fmtType *p, int c) {
  FILE *fp = (FILE *) p->user_data;
  int r = fileptr_append_or_flush (fp, c);
  p->counter += (r isnot EOF);
}

static void fileptrWriteUnbuffered (fmtType *p, int c) {
  FILE *fp = (FILE *) p->user_data;

  // if (fp->fd < 0) return; let it fail since sys_write returns -1

  fp->out_buf[0] = c;

  p->counter += (sys_write (fp->fd, fp->out_buf, 1) isnot EOF);
}

int sys_fprintf (FILE *fp, const char *fmt, ...) {
  fmtType s;
  s.output_char = (fp->bufferingType is _IONBF ? fileptrWriteUnbuffered : fileptrWriteBuffered);
  s.user_data = fp;

  va_list ap;
  va_start(ap, fmt);
  int n = str_format (&s, NULL, 0, fmt, ap);
  va_end(ap);

  sys_fflush (fp);

  return n;
}

size_t sys_fwrite (const void *v, size_t size, size_t nmemb, FILE *fp) {
  uchar *s = (uchar *) v;
  size_t i = nmemb * size;

  while (i-- > 0)
    if (fileptr_append_or_flush (fp, *s++) is EOF)
      return nmemb * size - i - 1;

  return nmemb * size;
}


static int fileptr_input_char (FILE *fp) {
  if (fp->fd < 0) return EOF;

  if (fp->in_cur is fp->in_buflen) {
    int n = sys_read (fp->fd, fp->in_buf, fp->in_bufsize);
    if (n <= 0) return EOF;

    fp->in_buflen = n;
    fp->in_cur = 0;
  }

  return fp->in_cur < fp->in_buflen ? (unsigned char) fp->in_buf[fp->in_cur++] : EOF;
}


size_t sys_fread (void *v, size_t size, size_t nmemb, FILE *fp) {
  char *s = v;

  size_t n = nmemb * size;

  while (n-- > 0) {
    *s = fileptr_input_char (fp);
    if (*s++ is EOF)
      return (nmemb * size) - n - 1;
  }

  return nmemb * size;
}

