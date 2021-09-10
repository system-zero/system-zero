#define REQUIRE_STDIO
#define REQUIRE_STDARG
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT
#define REQUIRE_UNISTD
#define REQUIRE_FCNTL

#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_ERROR_TYPE    DECLARE
#define REQUIRE_MD5_TYPE      DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

static VALUE crypt_md5sum (la_t *this, VALUE v_str) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *str = AS_STRING(v_str);

  uchar md[MD5_DIGEST_LENGTH];
  struct md5 ctx;
  Md5.init (&ctx);
  Md5.update (&ctx, (uchar *) str->bytes, str->num_bytes);
  Md5.sum (&ctx, md);
  string *s = String.new (MD5_DIGEST_LENGTH);

  for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++)
    String.append_with_fmt (s, "%02x", md[i]);
  return STRING(s);
}

static VALUE crypt_md5sum_file (la_t *this, VALUE v_file) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);

  La.set.Errno (this, 0);

  int fd = open (file, O_RDONLY);
  if (-1 is fd) {
    La.set.Errno (this, errno);
    return NULL_VALUE;
  }

  struct md5 ctx;
  Md5.init (&ctx);

  ssize_t n;
  uchar buf[4096];

  while ((n = read (fd, buf, sizeof(buf))) > 0) {
    Md5.update (&ctx, buf, n);
    if (n < 0) {
      La.set.Errno (this, errno);
     return NULL_VALUE;
    }
  }

  close (fd);


  uchar md[MD5_DIGEST_LENGTH];
  Md5.sum (&ctx, md);

  string *s = String.new (MD5_DIGEST_LENGTH);

  for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++)
    String.append_with_fmt (s, "%02x", md[i]);

  String.append_with_fmt (s, "  %s", file);

  return STRING(s);
}

#define EvalString(...) #__VA_ARGS__

public int __init_crypt_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(error);
  __INIT__(string);
  __INIT__(md5);

  (void) vmapType;

  LaDefCFun lafuns[] = {
    { "crypt_md5sum",   PTR(crypt_md5sum), 1 },
    { "crypt_md5sum_file", PTR(crypt_md5sum_file), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Crypt = {
      "md5sum" : crypt_md5sum,
      "md5sum_file" : crypt_md5sum_file
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_crypt_module__ (la_t *this) {
  (void) this;
  return;
}
