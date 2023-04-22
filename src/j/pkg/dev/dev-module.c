#define REQUIRE_STD_MODULE
#define REQUIRE_Z_ENV
#define REQUIRE_DEV_BLKSIZE
#define REQUIRE_FD_INTERFACE
#define REQUIRE_STR_COPY
#define REQUIRE_STR_CAT
#define REQUIRE_BYTELEN
#define REQUIRE_UNISTD_H
#define REQUIRE_DECIMAL_TO_STRING

#include <libc.h>

MODULE(dev);

static VALUE _dev_blksize (la_t *this, VALUE v_dev) {
  ifnot (IS_STRING(v_dev)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *dev = AS_STRING_BYTES(v_dev);

  int print = GET_OPT_PRINT();

  int fd = sys_open (dev, O_RDONLY);
  if (-1 is fd) {
    char *err = errno_string (sys_errno);
    size_t len = bytelen (err);
    char errmsg[len+2];
    str_copy (errmsg, len + 1, err, len);
    sys_write (STDERR_FILENO, errmsg, len);
    sys_write (STDERR_FILENO, "\n", 1);
    return NOTOK_VALUE;
  }

  uint64_t size = dev_blksize (fd);
  sys_close (fd);

  if (print) {
    decimal_t dec;
    char *str = uint64_to_string (&dec, size);
    sys_write (STDOUT_FILENO, str, dec.size);
    sys_write (STDOUT_FILENO, "\n", 1);
    return OK_VALUE;
  }

  return NUMBER(size);
}

public int __init_dev_module__ (la_t *this) {
  __INIT_MODULE__(this);

  LaDefCFun lafuns[] = {
    {"dev_blksize", PTR(_dev_blksize), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Dev = {
      blksize : dev_blksize
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_dev_module__ (la_t *this) {
  (void) this;
}
