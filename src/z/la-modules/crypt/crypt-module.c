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
#define REQUIRE_SHA256_TYPE   DECLARE
#define REQUIRE_SHA512_TYPE   DECLARE
#define REQUIRE_LA_TYPE       DECLARE

#include <z/cenv.h>

struct MdOpts {
  void
    *ctx,
    (*sum) (void *, uchar *),
    (*init) (void *),
    (*update) (void *, const uchar *, ulong);

   uchar *md;
   int md_len;
};

static VALUE crypt_sum (la_t *this, VALUE v_str, struct MdOpts opts) {
  ifnot (IS_STRING(v_str)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  string *str = AS_STRING(v_str);

  uchar *md = opts.md;
  void *ctx = opts.ctx;
  opts.init (ctx);
  opts.update (ctx, (uchar *) str->bytes, str->num_bytes);
  opts.sum (ctx, md);

  string *s = String.new (opts.md_len);
  for (int i = 0; i < opts.md_len; i++)
    String.append_with_fmt (s, "%02x", md[i]);

  return STRING(s);
}

static VALUE crypt_md5sum (la_t *this, VALUE v_str) {
  struct md5 ctx;
  uchar md[MD5_DIGEST_LENGTH];
  return crypt_sum (this, v_str, (struct MdOpts) {
    .ctx = &ctx,
    .sum = Md5.sum,
    .init = Md5.init,
    .update = Md5.update,
    .md = md,
    .md_len = MD5_DIGEST_LENGTH
   });
}

static VALUE crypt_sha256sum (la_t *this, VALUE v_str) {
  struct sha256 ctx;
  uchar md[SHA256_DIGEST_LENGTH];
  return crypt_sum (this, v_str, (struct MdOpts) {
    .ctx = &ctx,
    .sum = Sha256.sum,
    .init = Sha256.init,
    .update = Sha256.update,
    .md = md,
    .md_len = SHA256_DIGEST_LENGTH
   });
}

static VALUE crypt_sha512sum (la_t *this, VALUE v_str) {
  struct sha512 ctx;
  uchar md[SHA512_DIGEST_LENGTH];
  return crypt_sum (this, v_str, (struct MdOpts) {
    .ctx = &ctx,
    .sum = Sha512.sum,
    .init = Sha512.init,
    .update = Sha512.update,
    .md = md,
    .md_len = SHA512_DIGEST_LENGTH
   });
}

static VALUE crypt_sum_file (la_t *this, VALUE v_file, struct MdOpts opts) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);

  La.set.Errno (this, 0);

  int fd = open (file, O_RDONLY);
  if (-1 is fd) {
    La.set.Errno (this, errno);
    return NULL_VALUE;
  }

  ssize_t n;
  uchar buf[4096];

  void *ctx = opts.ctx;
  opts.init (ctx);

  while ((n = read (fd, buf, sizeof(buf))) > 0) {
    opts.update (ctx, buf, n);
    if (n < 0) {
      La.set.Errno (this, errno);
     return NULL_VALUE;
    }
  }

  close (fd);

  opts.sum (ctx, opts.md);

  string *s = String.new (opts.md_len);

  for (int i = 0; i < opts.md_len; i++)
    String.append_with_fmt (s, "%02x", opts.md[i]);

  return STRING(s);
}

static VALUE crypt_md5sum_file (la_t *this, VALUE v_file) {
  struct md5 ctx;
  uchar md[MD5_DIGEST_LENGTH];
  return crypt_sum_file (this, v_file, (struct MdOpts) {
    .ctx = &ctx,
    .sum = Md5.sum,
    .init = Md5.init,
    .update = Md5.update,
    .md = md,
    .md_len = MD5_DIGEST_LENGTH
   });
}

static VALUE crypt_sha256sum_file (la_t *this, VALUE v_file) {
  struct sha256 ctx;
  uchar md[SHA256_DIGEST_LENGTH];
  return crypt_sum_file (this, v_file, (struct MdOpts) {
    .ctx = &ctx,
    .sum = Sha256.sum,
    .init = Sha256.init,
    .update = Sha256.update,
    .md = md,
    .md_len = SHA256_DIGEST_LENGTH
   });
}

static VALUE crypt_sha512sum_file (la_t *this, VALUE v_file) {
  struct sha512 ctx;
  uchar md[SHA512_DIGEST_LENGTH];
  return crypt_sum_file (this, v_file, (struct MdOpts) {
    .ctx = &ctx,
    .sum = Sha512.sum,
    .init = Sha512.init,
    .update = Sha512.update,
    .md = md,
    .md_len = SHA512_DIGEST_LENGTH
   });
}

#define EvalString(...) #__VA_ARGS__

public int __init_crypt_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(md5);
  __INIT__(sha256);
  __INIT__(sha512);
  __INIT__(error);
  __INIT__(string);

  (void) vmapType;

  LaDefCFun lafuns[] = {
    { "crypt_md5sum",         PTR(crypt_md5sum), 1 },
    { "crypt_md5sum_file",    PTR(crypt_md5sum_file), 1},
    { "crypt_sha256sum",      PTR(crypt_sha256sum), 1 },
    { "crypt_sha256sum_file", PTR(crypt_sha256sum_file), 1},
    { "crypt_sha512sum",      PTR(crypt_sha512sum), 1 },
    { "crypt_sha512sum_file", PTR(crypt_sha512sum_file), 1},
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Crypt = {
      "md5sum"         : crypt_md5sum,
      "sha256sum"      : crypt_sha256sum,
      "sha512sum"      : crypt_sha512sum,
      "md5sum_file"    : crypt_md5sum_file,
      "sha256sum_file" : crypt_sha256sum_file,
      "sha512sum_file" : crypt_sha512sum_file
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