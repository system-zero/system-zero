#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_FCNTL
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT

#define REQUIRE_STD_MODULE
#define REQUIRE_IO_TYPE       DECLARE

#include <z/cenv.h>

MODULE(io)

static VALUE io_fd_isatty (la_t *this, VALUE v_fd) {
  ifnot (IS_FILEDES(v_fd)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file descriptor");
  return INT(isatty (AS_FILEDES(v_fd)));
}

static VALUE io_fd_read (la_t *this, VALUE v_fd) {
  ifnot (IS_FILEDES(v_fd)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file descriptor");
  int fd = AS_FILEDES(v_fd);
  int len = 1024;
  char buf[len];
  buf[0] = '\0';
  string *s = String.new (128);
  int bts = 0;

  La.set.Errno (this, 0);

  while (1) {
    bts = IO.fd.read (fd, buf, len);
    if (NOTOK is bts) {
      String.release (s);
      La.set.Errno (this, errno);
      return NULL_VALUE;
    }

    String.append_with_len (s, buf, bts);
    if (bts < len) break;
  }

  return STRING(s);
}

static VALUE io_fd_write (la_t *this, VALUE v_fd, VALUE v_buf) {
  ifnot (IS_FILEDES(v_fd)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file descriptor");
  ifnot (IS_STRING(v_buf)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *s = AS_STRING(v_buf);

  La.set.Errno (this, 0);

  int retval = IO.fd.write (AS_FILEDES(v_fd), s->bytes, s->num_bytes);
  if (retval is -1)
    La.set.Errno (this, errno);

  return INT(retval);
}

static VALUE io_close (la_t *this, VALUE v_fd) {
  (void) this;
  ifnot (IS_FILEDES(v_fd)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a file descriptor");
  int fd = AS_FILEDES(v_fd);
  close (fd);
  return OK_VALUE;
}

static VALUE io_open (la_t *this, VALUE v_fname, VALUE v_flags, VALUE v_mode) {
  ifnot (IS_STRING(v_fname)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  ifnot (IS_INT(v_flags)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer flaags argument");

  char *fname = AS_STRING_BYTES(v_fname);
  int flags = AS_INT(v_flags);

  int mode = 0;

  if (flags & O_CREAT) {
    ifnot (IS_INT(v_mode)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer as mode argument");
    mode = AS_INT(v_mode);
  }

  La.set.Errno (this, 0);

  int fd;
  if (flags & O_CREAT)
    fd = open (fname, flags, mode);
  else
    fd = open (fname, flags);

  if (fd is -1) {
    La.set.Errno (this, errno);
    return NULL_VALUE;
  }

  VALUE v = OBJECT(fd);
  object *o = La.object.new (io_close, NULL, "FdType", v);
  v = FILEDES(o);
  return v;
}

public int __init_io_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(io);

  LaDefCFun lafuns[] = {
    { "io_open",      PTR(io_open), 3 },
    { "io_fd_isatty", PTR(io_fd_isatty), 1 },
    { "io_fd_read",   PTR(io_fd_read), 1 },
    { "io_fd_write",  PTR(io_fd_write), 2 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  /* minimum? */
  if (La.def_std (this, "O_RDONLY", INTEGER_TYPE, INT(O_RDONLY), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_WRONLY", INTEGER_TYPE, INT(O_WRONLY), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_RDWR", INTEGER_TYPE, INT(O_RDWR), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_CREAT", INTEGER_TYPE, INT(O_CREAT), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_APPEND", INTEGER_TYPE, INT(O_APPEND), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_TRUNC", INTEGER_TYPE, INT(O_TRUNC), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_NOCTTY", INTEGER_TYPE, INT(O_NOCTTY), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_CLOEXEC", INTEGER_TYPE, INT(O_CLOEXEC), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_DIRECTORY", INTEGER_TYPE, INT(O_DIRECTORY), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_NONBLOCK", INTEGER_TYPE, INT(O_NONBLOCK), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_EXCL", INTEGER_TYPE, INT(O_EXCL), 1)) return LA_NOTOK;
  if (La.def_std (this, "O_NOFOLLOW", INTEGER_TYPE, INT(O_NOFOLLOW), 1)) return LA_NOTOK;

  #ifdef O_LARGEFILE
  if (La.def_std (this, "O_LARGEFILE", INTEGER_TYPE, INT(O_LARGEFILE), 1)) return LA_NOTOK;
  #endif
  #ifdef O_SYNC
  if (La.def_std (this, "O_SYNC", INTEGER_TYPE, INT(O_SYNC), 1)) return LA_NOTOK;
  #endif
  #ifdef O_ASYNC
  if (La.def_std (this, "O_ASYNC", INTEGER_TYPE, INT(O_ASYNC), 1)) return LA_NOTOK;
  #endif

  /* could be already redefined at file */
  if (La.def_std (this, "S_IRWXU", INTEGER_TYPE, INT(S_IRWXU), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IRUSR", INTEGER_TYPE, INT(S_IRUSR), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IWUSR", INTEGER_TYPE, INT(S_IWUSR), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IXUSR", INTEGER_TYPE, INT(S_IXUSR), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IRWXG", INTEGER_TYPE, INT(S_IRWXG), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IRGRP", INTEGER_TYPE, INT(S_IRGRP), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IWGRP", INTEGER_TYPE, INT(S_IWGRP), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IXGRP", INTEGER_TYPE, INT(S_IXGRP), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IRWXO", INTEGER_TYPE, INT(S_IRWXO), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IROTH", INTEGER_TYPE, INT(S_IROTH), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IWOTH", INTEGER_TYPE, INT(S_IWOTH), -1)) return LA_NOTOK;
  if (La.def_std (this, "S_IXOTH", INTEGER_TYPE, INT(S_IXOTH), -1)) return LA_NOTOK;

  const char evalString[] = EvalString (
    public var Io = {
      open : io_open,
      fd : {
        read : io_fd_read,
        write : io_fd_write,
        isatty : io_fd_isatty,
      }
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_io_module__ (la_t *this) {
  (void) this;
  return;
}
