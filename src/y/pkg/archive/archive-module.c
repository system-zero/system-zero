#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_FCNTL

#define REQUIRE_STD_MODULE
#define REQUIRE_DIR_TYPE    DECLARE
#define REQUIRE_FILE_TYPE   DECLARE
#define REQUIRE_STRING_TYPE DECLARE
#define REQUIRE_ERROR_TYPE  DECLARE

#include <z/cenv.h>
#include <libdeflate.h>
#include <z/filetype.h>

#include "microtar.h"

typedef struct archive_tar {
  char *archive;
  char *end;
  char *p;
  size_t size;
} archive_tar;

MODULE(archive);

typedef uint8_t u8;
typedef uint32_t u32;

#define MIN(a, b) ((a) <= (b) ? (a) : (b))

static int archive_tar_write_out (mtar_t *tar, mtar_header_t *h) {
  archive_tar *t = (archive_tar *) tar->stream;
  char *p;
  int fd;

  switch (h->type) {
    case MTAR_TDIR:
      return Dir.make (h->name, 0755, DirOpts());

    case MTAR_TREG:
    case MTAR_AREG:
      p = t->archive + tar->pos + 512;
      fd = open (h->name, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
      if (fd is -1) {
        fprintf (stderr, "%s: failed to open, %s\n", h->name, Error.errno_string (errno));
        return NOTOK;
      }

      size_t total = h->size;
      ssize_t written = 0;
      while (total isnot 0) {
        written = write (fd, p, h->size);
        if (written <= 0) {
          fprintf (stderr, "%s: [write] %s\n", h->name, Error.errno_string (errno));
          return NOTOK;
        }

        total -= written;
        p += written;
      }

      close (fd);
      return OK;

    case MTAR_TSYM:
      return symlink (h->linkName, h->name);
  }

  return OK;
}

static int mtar_seek_cb (mtar_t *tar, unsigned pos) {
  archive_tar *t = (archive_tar *) tar->stream;
  if (pos > t->size) return MTAR_ESEEKFAIL;
  t->p = t->archive;
  t->p += pos;
  tar->pos = pos;
  return MTAR_ESUCCESS;
}

static int mtar_read_cb (mtar_t *tar, void *data, unsigned size) {
  archive_tar *t = (archive_tar *) tar->stream;
  char *p = t->p;
  if (p + size > t->end) return MTAR_EREADFAIL;

  char *d = data;
  for (uint i = 0; i < size; i++)
    d[i] = *(p + i);

  return MTAR_ESUCCESS;
}

static int tar_extract (const char *archive, size_t size) {
  mtar_t tar;
  mtar_header_t h;
  memset (&tar, 0, sizeof (tar));

  tar.read = mtar_read_cb;
  tar.seek = mtar_seek_cb;

  archive_tar t = (archive_tar) {
    .archive = (char *) archive,
    .p = (char *) archive,
    .end = (char *) archive + size,
    .size = size
  };

  tar.stream = &t;

  int retval = mtar_read_header (&tar, &h);

  if (retval isnot MTAR_ESUCCESS) return NOTOK;

  while ((mtar_read_header (&tar, &h)) isnot MTAR_ENULLRECORD ) {
    if (NOTOK is archive_tar_write_out (&tar, &h))
      return NOTOK;

    if (MTAR_ESUCCESS isnot mtar_next(&tar)) {
      retval = NOTOK;
      break;
    }
  }

  return retval;
}

static unsigned int load_u32_gzip (const u8 *p) {
  return ((u32)p[0] << 0) | ((u32)p[1] << 8) |
    ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

static VALUE archive_extract_file (la_t *this, VALUE v_file) {
  ifnot (IS_STRING(v_file)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *file = AS_STRING_BYTES(v_file);

  VALUE r = NOTOK_VALUE;

  ifnot (File.exists (file)) {
    fprintf (stderr, "%s: doesn't exists", file);
    return r;
  }

  char ftbuf[MAXLEN_FILETYPE];
  int ftype = filetype (file, ftbuf);
  if (FILETYPE_GZIP isnot ftype) {
    fprintf (stderr, "%s: not a gzip file\n", file);
    return r;
  }

  FILE *fp = fopen (file, "r");
  if (NULL is fp) {
    fprintf (stderr, "%s: %s\n", file, Error.errno_string (errno));
    return r;
  }

  int fd = -1;
  char *out = NULL;
  struct libdeflate_decompressor *d = NULL;

  if (-1 is fseek (fp, 0, SEEK_END)) {
    fprintf (stderr, "%s: [fseek] %s\n", file, Error.errno_string (errno));
    fclose (fp);
    return r;
  }

  long flen = 0;
  flen = ftell (fp);

  if (-1 is flen) {
    fprintf (stderr, "%s: [ftell] %s\n", file, Error.errno_string (errno));
    fclose (fp);
    return r;
  }

  size_t len = (size_t) flen;

  unsigned char *in = Alloc (len + 1);

  if (-1 is fseek (fp, 0, SEEK_SET)) {
    fprintf (stderr, "%s: [fseek] %s\n", file, Error.errno_string (errno));
    goto theend;
  }

  size_t nread = fread (in, 1, len, fp);
  if (nread isnot (size_t) len) {
    fprintf (stderr, "%s: [fread] didn't read the whole data\n", file);
    goto theend;
  }
  in[len] = '\0';

  fclose (fp);
  fp = NULL;

  size_t out_len = load_u32_gzip (&in[len - 4]);
  if (out_len is 0) out_len = 1;

  size_t max_out_len;
  if ((size_t) len <= SIZE_MAX / 1032)
    max_out_len = len * 1032;
  else
    max_out_len = SIZE_MAX;

  size_t actual_in_nbytes;
  size_t actual_out_nbytes;
  enum libdeflate_result result;

  uchar *input = in;

  do {
    if (out is NULL) {
      out_len = MIN(out_len, max_out_len);

      out = Alloc (out_len);
    }

    d = libdeflate_alloc_decompressor ();

    if (d is NULL) {
      fprintf (stderr, "libdeflate_alloc_decompressor failed\n");
      goto theend;
    }

    result = libdeflate_gzip_decompress_ex (d,
      input, len, out, out_len,
      &actual_in_nbytes, &actual_out_nbytes);

    if (result is LIBDEFLATE_INSUFFICIENT_SPACE) {
      if (out_len >= max_out_len) {
        fprintf (stderr, "decompressed size %zd, is longer than maximum %zd\n", out_len, max_out_len);
        goto theend;
      }

      if (out_len * 2 <= out_len) {
        fprintf (stderr, "decompressed size %zd, might overflow\n", out_len);
        goto theend;
      }

      out_len *= 2;
      free (out);
      out = NULL;
      continue;
    }

    if (result isnot LIBDEFLATE_SUCCESS) {
      fprintf (stderr, "libdeflate_gzip_decompress_ex failed\n");
      goto theend;
    }

    if (actual_in_nbytes is 0 or
        actual_in_nbytes > (size_t) len or
        actual_out_nbytes > out_len) {
      fprintf (stderr, "libdeflate_gzip_decompress_ex read more than it should\n");
      goto theend;
    }

    input += actual_in_nbytes;
    len -= actual_in_nbytes;
  } while (len isnot 0);

  ftype = filetype_from_string (out, MTAR_BLCKSIZ, ftbuf);

  ifnot (FILETYPE_IS_TAR(ftype)) {
    fprintf (stderr, "%s: not a tar archive\n", file);
    goto theend;
  }

  r = INT(tar_extract (out, actual_out_nbytes));
  goto theend;

/* todo gunzip only */
#if 0
   char name[MAXLEN_PATH];
   char *p = out;

  for (int i = 0; i < MAXLEN_PATH; i++) {
    if (*p and *p isnot DIR_SEP) { name[i] = *p++; continue; }
    name[i] = '\0';
    break;
  }

  fd = open (name, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);

  if (fd is -1) {
    fprintf (stderr, "%s: %s\n", name, Error.errno_string (errno));
    goto theend;
  }

  size_t total = actual_out_nbytes;
  ssize_t written = 0;

  while (total isnot 0) {
    written = write (fd, p, actual_out_nbytes);
    if (written <= 0) {
      fprintf (stderr, "%s: [write] %s\n", file, Error.errno_string (errno));
      goto theend;
    }

    total -= written;
    p += written;
  }

  r = INT(actual_out_nbytes);
#endif

theend:
  ifnot (-1 is fd) close (fd);
  ifnot (NULL is out) free (out);
  ifnot (NULL is in) free (in);
  ifnot (NULL is fp) fclose (fp);
  ifnot (NULL is d) libdeflate_free_decompressor(d);
  return r;
}

#define EvalString(...) #__VA_ARGS__

public int __init_archive_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(dir);
  __INIT__(file);
  __INIT__(error);
  __INIT__(string);

  LaDefCFun lafuns[] = {
    { "archive_extract_file", PTR(archive_extract_file), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Archive = {
      extract : {
        file : archive_extract_file
      }
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_archive_module__ (la_t *this) {
  (void) this;
}
