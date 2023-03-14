#define REQUIRE_STDIO
#define REQUIRE_TIME

#define REQUIRE_STD_MODULE
#define REQUIRE_OS_TYPE      DECLARE
#define REQUIRE_RE_TYPE      DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_PATH_TYPE    DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_LIST_MACROS

#include <z/cenv.h>

MODULE(find);

#define EXECUTABLE -1
#define DIR_TYPE  (1 << 0)
#define REG_TYPE  (1 << 1)
#define BLK_TYPE  (1 << 2)
#define CHR_TYPE  (1 << 3)
#define LNK_TYPE  (1 << 4)
#define FIFO_TYPE (1 << 5)
#define SOCK_TYPE (1 << 6)

#define SORT_MTIME  1
#define SORT_CTIME  2
#define SORT_ATIME  3
#define SORT_SIZE   4
#define SORT_STRING 5

#define GET_OPT_TOSTDOUT() ({                                               \
  VALUE _v_tostdout = La.get.qualifier (this, "tostdout", INT(0));          \
  ifnot (IS_INT(_v_tostdout))                                               \
    THROW(LA_ERR_TYPE_MISMATCH, "tostdout, awaiting an integer qualifier"); \
  AS_INT(_v_tostdout);                                                      \
})

#define GET_OPT_REALPATH() ({                                               \
  VALUE _v_realpath = La.get.qualifier (this, "realpath", INT(1));          \
  ifnot (IS_INT(_v_realpath))                                               \
    THROW(LA_ERR_TYPE_MISMATCH, "realpath, awaiting an integer qualifier"); \
  AS_INT(_v_realpath);                                                      \
})

typedef struct find_t {
  int type;
  int tostdout;
  int show_hidden;
  int append_indicator;
  int long_format;
  int initialdir_isdot;
  int sort_type;
  int reverse_order;
  int match_uid;
  uid_t uid;
  int match_gid;
  gid_t gid;
  unsigned long *sorted_long;
  char **sorted_string;
  re_t *re;
  Vstring_t *files;
} find_t;

typedef struct sorted {
  vstring_t *v;
  unsigned long s;
  char *bname;
} sorted;

static int cstring_cmp (const char *sa, const char *sb) {
  const uchar *spa = (const uchar *) sa;
  const uchar *spb = (const uchar *) sb;
  for (;; spa++, spb++) {
    if (*spa != *spb)
      // return (*(uchar *) spa - *(uchar *) spb);
      return (*(uchar *) spb - *(uchar *) spa); // allign with the sort long types
      /* not sure why we had to do this, but that's the way it is */
    if (*spa == 0) return 0;
  }

  return 0;
}

static int cmp_string (const void *a, const void *b) {
  const sorted *sa = a, *sb = b;
  return cstring_cmp (sa->bname, sb->bname);
}

static int cmp_long (const void *a, const void *b) {
  const sorted *sa = a, *sb = b;
  if (sa->s is sb->s) return 0;
  return sa->s > sb->s;
}

static ArrayType *find_sort (find_t *find) {
  int num = find->files->num_items;
  sorted v[num];
  int idx = 0;

  vstring_t *it = find->files->head;
  while (it) {
    sorted o;
    o.v = it;
    if (find->sort_type is SORT_STRING)
      o.bname = find->sorted_string[idx];
    else
      o.s = find->sorted_long[idx];

    v[idx++] = o;
    it = it->next;
  }

  if (find->sort_type is SORT_STRING)
    qsort (&v, num, sizeof(sorted), cmp_string);
  else
    qsort (&v, num, sizeof(sorted), cmp_long);

  ArrayType *array = ARRAY_INIT_WITH_LEN(STRING_TYPE, num);
  string **ar = (string **) AS_ARRAY(array->value);

  if (find->reverse_order) {
    for (int i = 0; i < num; i++)
      ar[i] = v[i].v->data;
  } else {
    for (int i = num - 1, j = 0; i >= 0; i--, j++)
      ar[j] = v[i].v->data;
  }

  return array;
}

static void append_sorted_type (find_t *find, struct stat *st, const char *o) {
  int num = find->files->num_items;

  if (find->sort_type is SORT_STRING) {
    if (find->sorted_string is NULL)
      find->sorted_string = Alloc (1 * sizeof (char *));
    else
      find->sorted_string = Realloc (find->sorted_string, ((size_t) num) * sizeof (char *));
  } else {
    if (find->sorted_long is NULL)
      find->sorted_long = Alloc (1 * sizeof (unsigned long));
    else
      find->sorted_long = Realloc (find->sorted_long, ((size_t) num) * sizeof (unsigned long));
  }

  switch (find->sort_type) {
    case SORT_STRING:
      find->sorted_string[num-1] = (char *) o;
      break;

    case SORT_MTIME:
      find->sorted_long[num-1] = st->st_mtime;
      break;

    case SORT_ATIME:
      find->sorted_long[num-1] = st->st_atime;
      break;

    case SORT_CTIME:
      find->sorted_long[num-1] = st->st_ctime;
      break;

    case SORT_SIZE:
      find->sorted_long[num-1] = st->st_size;
      break;
  }
}

static char *path_basename (char *name, size_t len) {
  char *p = name + (len);
  if (p - 1 is name and IS_DIR_SEP (*(p - 1)))
    return p - 1;

  while (p > name and IS_DIR_SEP (*(p - 1))) p--;
  while (p > name and IS_NOT_DIR_SEP (*(p - 1))) --p;
  if (p is name and IS_DIR_SEP (*p))
    return (char *) DIR_SEP_STR;
  return p;
}

static string *long_format (const char *o, size_t len, struct stat *st, int islnk, char **f) {
  char buf[12]; File.mode.stat_to_string (buf, st->st_mode);
  char *name = OS.get.pwname (st->st_uid);
  char *gnam = OS.get.grname (st->st_gid);
  char buftm[32];
  struct tm *tm = localtime (&st->st_mtim.tv_sec);
  strftime (buftm, sizeof (buftm), "%b %d %H:%M", tm);

  string *s = String.new_with_fmt ("%s %4ld %-6.6s %-6.6s %10lu %s %s", buf,
    (long) st->st_nlink, name, gnam, (unsigned long) st->st_size, buftm, o);

  free (name);
  free (gnam);

  if (islnk) {
    string *sl = File.readlink (o);
    if (sl isnot NULL) {
      String.append_with_len (s, " -> ", 4);
      String.append_with_len (s, sl->bytes, sl->num_bytes);
      struct stat stl;
      ifnot (stat (sl->bytes, &stl))
        if (stl.st_mode & S_IXUSR or stl.st_mode & S_IXGRP or stl.st_mode & S_IXOTH)
          String.append_byte (s, '*');
      len += sl->num_bytes + 4;
      String.release (sl);
    }
  }

  *f = s->bytes + (s->num_bytes - len);

  return s;
}

static VALUE ls_file (find_t *find, const char *f) {
  char *file = (char *) f;
  struct stat st;
  if (-1 is lstat (file, &st)) {
    fprintf (stderr, "%s: %s\n", file, Error.errno_string (errno));
    return NULL_VALUE;
  }

  if (-1 isnot find->match_uid and st.st_uid isnot find->uid)
    return NULL_VALUE;
  if (-1 isnot find->match_gid and st.st_gid isnot find->gid)
    return NULL_VALUE;


  size_t len = bytelen (file);

  char *bname = path_basename (file, len);

  char indicator = '\0';

  do {
    if (find->type is EXECUTABLE) {
      if (st.st_mode & S_IXUSR or st.st_mode & S_IXGRP or st.st_mode & S_IXOTH) {
        indicator = '*';
        break;
      } else
        return NULL_VALUE;
    }

    if (find->type & REG_TYPE  and S_ISREG(st.st_mode))  { indicator = 'f'; break; }
    if (find->type & LNK_TYPE  and S_ISLNK(st.st_mode))  { indicator = '@'; break; }
    if (find->type & FIFO_TYPE and S_ISFIFO(st.st_mode)) { indicator = '|'; break; }
    if (find->type & SOCK_TYPE and S_ISSOCK(st.st_mode)) { indicator = '='; break; }
    if (find->type & BLK_TYPE  and S_ISBLK(st.st_mode)) break;
    if (find->type & CHR_TYPE  and S_ISCHR(st.st_mode)) break;
    return NULL_VALUE;
  } while (0);

  ifnot (NULL is find->re)
    ifnot (Re.exec (find->re, bname,  len - (bname - file)) >= 0)
      return NULL_VALUE;

  string *s = NULL;
  char *tmp = NULL;

  ifnot (find->long_format) {
    s = String.new_with (file);
    tmp = s->bytes;
  } else
    s = long_format (file, len, &st, indicator is '@', &tmp);

  if (find->append_indicator) {
    switch (indicator) {
      case '\0': break;

      case 'f':
        if (st.st_mode & S_IXUSR or st.st_mode & S_IXGRP or st.st_mode & S_IXOTH)
          String.append_byte (s, '*');
        break;

      case '@':
        if (find->long_format) break;
        // fall through

      default:
        String.append_byte (s, indicator);
    }
  }

  if (find->tostdout) {
    fprintf (stdout, "%s\n", s->bytes);
    String.release (s);
    return TRUE_VALUE;
  }

  return STRING(s);
}

static int process_file (dirwalk_t *dw, const char *f, struct stat *st) {
  find_t *find = dw->user_data;
  if (-1 isnot find->match_uid and st->st_uid isnot find->uid) return 0;
  if (-1 isnot find->match_gid and st->st_gid isnot find->gid) return 0;

  char *file = (char *) f;

  if (find->initialdir_isdot and *f is '.' and *(f + 1) is DIR_SEP)
    file += 2;

  size_t len = bytelen (file);

  char *bname = path_basename (file, len);

  if (*bname is '.' and 0 is find->show_hidden) return 0;

  char indicator = '\0';

  do {
    if (find->type is EXECUTABLE) {
      if (st->st_mode & S_IXUSR or st->st_mode & S_IXGRP or st->st_mode & S_IXOTH) {
        indicator = '*';
        break;
      } else
        return 0;
    }

    if (find->type & REG_TYPE  and S_ISREG(st->st_mode))  { indicator = 'f'; break; }
    if (find->type & LNK_TYPE  and S_ISLNK(st->st_mode))  { indicator = '@'; break; }
    if (find->type & FIFO_TYPE and S_ISFIFO(st->st_mode)) { indicator = '|'; break; }
    if (find->type & SOCK_TYPE and S_ISSOCK(st->st_mode)) { indicator = '='; break; }
    if (find->type & BLK_TYPE  and S_ISBLK(st->st_mode)) break;
    if (find->type & CHR_TYPE  and S_ISCHR(st->st_mode)) break;
    return 0;
  } while (0);

  vstring_t *v = NULL;

  if (NULL is find->re)
    v = Vstring.new_item ();
  else if (Re.exec (find->re, bname,  len - (bname - file)) >= 0)
    v = Vstring.new_item ();
  else return 0;

  char *tmp = NULL;

  ifnot (find->long_format) {
    v->data = String.new_with (file);
    tmp = v->data->bytes;
  } else
    v->data = long_format (file, len, st, indicator is '@', &tmp);

  if (find->append_indicator) {
    switch (indicator) {
      case '\0': break;

      case 'f':
        if (st->st_mode & S_IXUSR or st->st_mode & S_IXGRP or st->st_mode & S_IXOTH)
          String.append_byte (v->data, '*');
        break;

      case '@':
        if (find->long_format) break;
        // fall through

      default:
        String.append_byte (v->data, indicator);
    }
  }

  if (find->tostdout) {
    fprintf (stdout, "%s\n", v->data->bytes);
    Vstring.release_item (v);
  } else {
    DListAppend (find->files, v);
    append_sorted_type (find, st, tmp);
  }

  return 0;
}

static int process_dir (dirwalk_t *dw, const char *d, struct stat *st) {
  find_t *find = dw->user_data;

  if (-1 isnot find->match_uid and st->st_uid isnot find->uid) return 1;
  if (-1 isnot find->match_gid and st->st_gid isnot find->gid) return 1;
  if (find->type is EXECUTABLE or 0 is (find->type & DIR_TYPE)) return 1;

  char *dir = (char *) d;
  if (find->initialdir_isdot and *d is '.' and *(d + 1) is DIR_SEP)
    dir += 2;

  size_t len = bytelen (dir);
  char *bname  = path_basename (dir, len);

  if (*bname is '.' and 0 is find->show_hidden) return 1;

  vstring_t *v = NULL;

  if (NULL is find->re)
    v = Vstring.new_item ();
  else if (Re.exec (find->re, bname, len - (bname - dir)) >= 0)
    v = Vstring.new_item ();
  else
    return 1;

  char *tmp = NULL;

  ifnot (find->long_format) {
    v->data = String.new_with (dir);
    tmp = v->data->bytes;
  } else
    v->data = long_format (dir, len, st, 0, &tmp);

  if (find->append_indicator) String.append_byte (v->data, '/');

  if (find->tostdout) {
    fprintf (stdout, "%s\n", v->data->bytes);
    Vstring.release_item (v);
  } else {
    DListAppend (find->files, v);
    append_sorted_type (find, st, tmp);
  }

  return 1;
}

static VALUE find_dir (la_t *this, VALUE v_o, VALUE v_type) {
  ifnot (IS_STRING(v_o)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string");
  char *type = NULL;

  if (IS_STRING(v_type))
    type = AS_STRING_BYTES(v_type);
  else ifnot (IS_NULL(v_type))
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null");

  char *dir = AS_STRING_BYTES(v_o);

  int sort_type = 0;
  int max_depth = GET_OPT_MAX_DEPTH();
  int append_indicator = GET_OPT_APPEND_INDICATOR();
  int show_hidden = GET_OPT_HIDDEN();
  int long_format = GET_OPT_LONG_FORMAT();
  int reverse = GET_OPT_REVERSE();
  int sort_by_mtime = GET_OPT_SORT_BY_MTIME();
  int sort_by_atime = GET_OPT_SORT_BY_ATIME();
  int sort_by_ctime = GET_OPT_SORT_BY_CTIME();
  int sort_by_size  = GET_OPT_SORT_BY_SIZE();
  int only_executables = GET_OPT_EXECUTABLE();
  int tostdout = GET_OPT_TOSTDOUT();
  int match_uid = GET_OPT_UID();
  int match_gid = GET_OPT_GID();
  int realpath =  GET_OPT_REALPATH();

  if (sort_by_mtime) sort_type = SORT_MTIME;
  if (sort_by_atime) {
    if (sort_type) {fprintf (stderr, "already set a sorted option\n"); return NULL_VALUE;}
    sort_type = SORT_ATIME;
  }

  if (sort_by_ctime) {
    if (sort_type) {fprintf (stderr, "already set a sorted option\n"); return NULL_VALUE;}
    sort_type = SORT_CTIME;
  }

  if (sort_by_size) {
    if (sort_type) {fprintf (stderr, "already set a sorted option\n"); return NULL_VALUE;}
    sort_type = SORT_SIZE;
  }

  ifnot (sort_type) sort_type = SORT_STRING;

  int initialdir_isdot = ('.' is *dir and *(dir + 1) is '\0');
  char *pat = GET_OPT_PAT();

  int types = 0;

  if (only_executables) {
    types = EXECUTABLE;
  } else if (type is NULL) {
    types |= (DIR_TYPE|REG_TYPE|BLK_TYPE|CHR_TYPE|LNK_TYPE|FIFO_TYPE|SOCK_TYPE);
  } else {
    while (*type) {
      switch (*type) {
        case 'd' : types |= DIR_TYPE;  break;
        case 'f' : types |= REG_TYPE;  break;
        case 'b' : types |= BLK_TYPE;  break;
        case 'c' : types |= CHR_TYPE;  break;
        case 'l' : types |= LNK_TYPE;  break;
        case 'p' : types |= FIFO_TYPE; break;
        case 's' : types |= SOCK_TYPE; break;
        case ',' :
          if (*(type + 1) is '\0') {
            fprintf (stderr, "awaiting a char after a comma\n");
            return NULL_VALUE;
          }
          break;

        default:
          fprintf (stderr, "awaiting one of 'dfbcpls' characters\n");
          return NULL_VALUE;
      }

      type++;
    }
  }

  re_t *re = NULL;
  if (pat isnot NULL)
    re = Re.new (pat, 0, RE_MAX_NUM_CAPTURES, Re.compile);

  find_t find = (find_t) {
    .type = types,
    .tostdout = tostdout,
    .show_hidden = show_hidden,
    .append_indicator = append_indicator,
    .long_format = long_format,
    .initialdir_isdot = initialdir_isdot,
    .sort_type = sort_type,
    .reverse_order = reverse,
    .match_uid = match_uid,
    .match_gid = match_gid,
    .uid = (match_uid isnot -1 ? match_uid : 0),
    .gid = (match_gid isnot -1 ? match_gid : 0),
    .re = re,
    .files = NULL,
    .sorted_long = NULL,
    .sorted_string = NULL
  };

  ifnot (Dir.is_directory (dir))
    return ls_file (&find, dir);

  ifnot (tostdout)
    find.files = Vstring.new ();

  if (max_depth < 0) max_depth = DIRWALK_MAX_DEPTH;

  dirwalk_t *dw = Dir.walk.new (process_dir, process_file);
  dw->user_data = &find;
  dw->depth = max_depth;
  dw->files = NULL;
  dw->stat_file = lstat;
  dw->realpath = realpath;

  Dir.walk.run (dw, dir);

  if (NULL isnot re) Re.release (re);
  Dir.walk.release (&dw);

  if (tostdout) return INT(0);

  ArrayType *array = NULL;
  array = find_sort (&find);

  if (find.sorted_string isnot NULL) free (find.sorted_string);
  if (find.sorted_long   isnot NULL) free (find.sorted_long);

  vstring_t *it = find.files->head;
  while (it) { vstring_t *tmp = it->next; free (it); it = tmp; }
  free (find.files);

  return ARRAY(array);
}

public int __init_find_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(re);
  __INIT__(os);
  __INIT__(dir);
  __INIT__(file);
  __INIT__(path);
  __INIT__(error);
  __INIT__(string);
  __INIT__(vstring);

  LaDefCFun lafuns[] = {
    { "find_dir", PTR(find_dir), 2 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Find = {
      dir : find_dir
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_find_module__ (la_t *this) {
  (void) this;
}
