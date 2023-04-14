// provides: FILE *sys_fopen (const char *, const char *)
// provides: int sys_fprintf (FILE *, const char *, ...)
// provides: int sys_fclose  (FILE *)
// provides: int sys_fflush  (FILE *)
// provides: size_t sys_fwrite (const void *, size_t, size_t, FILE *)
// provides: size_t sys_fread (void *, size_t, size_t, FILE *)
// provides: int fileptr_init_output (FILE *, size_t)
// provides: int fileptr_init_input  (FILE *, size_t)
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
// requires: sys/stat.h

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
  int flags = 0;

  switch (*mode) {
    case 'w': flags |= O_WRONLY|O_CREAT|O_TRUNC; break;
    case 'a': flags |= O_WRONLY|O_CREAT|O_APPEND; break;
    case 'r': flags |= O_RDONLY; break;

    default:
      sys_errno = EINVAL;
      return NULL;
  }

  switch (*(mode + 1)) {
    case '\0': break;
    case '+' : flags |= O_RDWR; break;

    default:
      sys_errno = EINVAL;
      return NULL;
  }

  fp = Alloc (sizeof (FILE));

  mem_set(fp, 0, sizeof (FILE)); // zero everything

  int modebits = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH; // man fopen()

  fp->fd = sys_open3 (path, flags, modebits);

  if (fp->fd < 0) {
    Release(fp);
    return NULL;
  }

  fp->bufferingType = _IOFBF;

  return fp;
}

int sys_fflush (FILE *fp) {
  if (fp->fd < 0) return 0;

  // we may want to make sure that this is an uncovered error by checking errno 
  // probably with a higher interface than sys_write(), but elsewhere not here
  if (sys_write (fp->fd, fp->out_buf, fp->out_buflen) isnot fp->out_buflen)
    return EOF;

  fp->out_buflen = 0;

  return 0;
}

int sys_fclose (FILE *fp) {
  sys_fflush (fp); // flush first anything in buffer

  int ret = sys_close (fp->fd);

  ifnot (NULL is fp->out_buf) Release (fp->out_buf);
  ifnot (NULL is fp->in_buf)  Release (fp->in_buf);

  Release (fp);

  return ret;
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

int fileptr_init_output (FILE *fp, size_t size) {
  if (fp->out_bufsize > 0) return 0;

  if (size < 1) {
    sys_errno = EINVAL;
    return -1;
  }

  fp->out_buflen = 0;
  fp->out_bufsize = size;
  fp->out_buf = Alloc (fp->out_bufsize);

  return (NULL is fp->out_buf ? -1 : 0); // normally never reached here on alloc failure
}

int fileptr_init_input (FILE *fp, size_t size) {
  if (fp->in_bufsize > 0) return 0;

  if (size < 1) {
    sys_errno = EINVAL;
    return -1;
  }

  fp->in_cur = 0;
  fp->in_buflen = 0;
  fp->in_bufsize = size;
  fp->in_buf = Alloc (fp->out_bufsize);

  return (NULL is fp->in_buf ? -1 : 0);
}

int sys_fprintf (FILE *fp, const char *fmt, ...) {
  if (NULL is fp or fp->fd < 0) return -1;

  if (-1 is fileptr_init_output (fp, BUFSIZ)) return -1;

  fmtType s;
  s.output_char = (fp->bufferingType is _IONBF ? fileptrWriteUnbuffered : fileptrWriteBuffered);
  s.user_data = fp;

  va_list ap;
  va_start(ap, fmt);
  int n = str_format (&s, NULL, 0, fmt, ap);
  va_end(ap);

  sys_fflush (fp); // flush unconditionally

  return n;
}

size_t sys_fwrite (const void *v, size_t size, size_t nmemb, FILE *fp) {
  if (NULL is fp or fp->fd < 0) return -1;

  if (-1 is fileptr_init_output (fp, BUFSIZ)) return -1;

  uchar *s = (uchar *) v;
  size_t i = nmemb * size;

  while (i-- > 0)
    if (fileptr_append_or_flush (fp, *s++) is EOF)
      return nmemb * size - i - 1;

  return nmemb * size;
}

static int fileptr_input_char (FILE *fp) {
  if (fp->in_cur is fp->in_buflen) {
    int n = sys_read (fp->fd, fp->in_buf, fp->in_bufsize);
    if (n <= 0) return EOF;

    fp->in_buflen = n;
    fp->in_cur = 0;
  }

  return fp->in_cur < fp->in_buflen ? (unsigned char) fp->in_buf[fp->in_cur++] : EOF;
}

size_t sys_fread (void *v, size_t size, size_t nmemb, FILE *fp) {
  if (NULL is fp or fp->fd < 0) return 0;

  if (-1 is fileptr_init_input (fp, BUFSIZ)) return 0;

  char *s = v;

  size_t n = nmemb * size;

  while (n-- > 0) {
    *s = fileptr_input_char (fp);
    if (*s++ is EOF)
      return (nmemb * size) - n - 1;
  }

  return nmemb * size;
}

