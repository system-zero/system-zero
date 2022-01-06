// APPLICATION "zs"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_TIME
#define REQUIRE_SYS_STAT
#define REQUIRE_SIGNAL

#define REQUIRE_SH_TYPE      DECLARE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_SYS_TYPE     DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_PATH_TYPE    DECLARE
#define REQUIRE_RLINE_TYPE   DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

#define DEFAULT_ROOT_PROMPT "\033[31m$\033[m "
#define DEFAULT_USER_PROMPT "\033[32m$\033[m "

#define MAXLEN_DIR          4095
#define MAXLEN_COMMAND      255
#define MAXLEN_COMMAND_LINE 8191
#define MAXLEN_HINT         63

#define ZS_COMMAND_HAS_NO_FILENAME_ARG (1 << 0)

#define ZS_RLINE_ARG_IS_COMMAND        1
#define ZS_RLINE_ARG_IS_ARG            2
#define ZS_RLINE_ARG_IS_DIRECTORY      3
#define ZS_RLINE_ARG_IS_FILENAME       4
#define ZS_RLINE_ARG_IS_LAST_COMPONENT 5

#define MAXNUM_LAST_COMPONENTS 20

typedef struct LastComp LastComp;
struct LastComp {
  char *lastcomp;
  LastComp *next;
};

typedef struct Command Command;
struct Command {
  char *name;
  int flags;
  Command *next;
};

typedef struct zs_t {
  Command *command_head;
  LastComp *lastcomp_head;
  int numLastComp;

  char command[MAXLEN_COMMAND + 1];
  char hint[MAXLEN_HINT + 1];

  int
    arg_type,
    num_items,
    last_retval,
    exit_val;

  string
    *arg,
    *comdir;

  rline_t *rline;
} zs_t;

static void zs_completion (const char *buf, int curpos, rlineCompletions *lc, void *userdata) {
  zs_t *zs = (zs_t *) userdata;
  rline_t *rline = zs->rline;
  Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);

  zs->num_items = 0;
  string *arg = zs->arg;
  String.clear (arg);

  dirlist_t *dlist = NULL;
  char *dirname = NULL;

  const char *lptr = buf + curpos;
  size_t lptrlen = bytelen (lptr);

  size_t buflen = bytelen (buf) - lptrlen;

  char ptrbuf[buflen + 1];

  if (buf[0] is '\0') {
    Command *it = zs->command_head;

    while (it) {
      char *name = it->name;
      while (*name) {
        String.append_byte (arg, *name);
        if (*name++ is '.') break;
      }

      zs->num_items++;
      Rline.add_completion (rline, lc, arg->bytes, -1);
      zs->arg_type = ZS_RLINE_ARG_IS_COMMAND;
      while (it->next) {
        if (Cstring.eq_n (arg->bytes, it->next->name, arg->num_bytes)) {
          it = it->next;
          continue;
        }
        break;
      }

      String.clear (arg);
      it = it->next;
    }

    goto theend;
  }

  if (buf[buflen-1] is '.') { // adjust code to count if it is really a command and not a hidden file
    Command *it = zs->command_head;

    while (it) {
      if (Cstring.eq_n (it->name, buf, buflen)) {
        zs->num_items++;
        String.replace_with (arg, it->name);
        if (buf + buflen -1 is lptr) {
          lptr++; lptrlen--;
        }

        String.append_with_len (arg, lptr, lptrlen);
        Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes - lptrlen);
        zs->arg_type = ZS_RLINE_ARG_IS_COMMAND;
      }
      it = it->next;
    }

    goto theend;
  }

  char *ptr = (char *) lptr;
  int is_arg = 0;
  int arglen = 0;

  while (ptr isnot buf) {
    if (*(ptr - 1) is ' ')
      break;
    ptr--;
    arglen++;
  }

  if (ptr isnot buf and *ptr is '-') {
    buflen = ptr - buf;
    is_arg = 1;
  }

  char *com = zs->command;
  int comlen = 0;
  char *sp = (char *) buf;
  while (*sp is ' ') sp++;
  if (sp is buf + buflen)
    goto theend;

  while (*sp) {
    if (*sp is ' ') break;
    if (comlen is MAXLEN_COMMAND)
      goto theend;

    com[comlen++] = *sp;
    sp++;
  }
  com[comlen] = '\0';

  if (0 is is_arg and comlen) {
    Command *it = zs->command_head;
    while (it) {
      if (Cstring.eq (it->name, com)) {
        if (it->flags & ZS_COMMAND_HAS_NO_FILENAME_ARG) {
          is_arg = 1;
          buflen = ptr - buf;
        }

        break;
      }
      it = it->next;
    }
  }

  if (is_arg) {
    int is_long = *(ptr + 1) is '-';

    size_t filelen = zs->comdir->num_bytes + 1 + comlen + sizeof ("/args");
    char file[filelen + 1];
    Cstring.cp_fmt (file, filelen + 1, "%s/%s/args", zs->comdir->bytes, com);

    if (-1 is access (file, F_OK|R_OK))
      goto theend;

    FILE *fp = fopen (file, "r");
    if (NULL is fp) goto theend;

    char *args = NULL;
    size_t argslen;
    ssize_t nread;
    String.append_with_len (arg, buf, buflen);

    while (-1 isnot (nread = getline (&args, &argslen, fp))) {
      args[nread - 1] = '\0';
      sp = args;
      while (*sp is ' ') sp++;
      while (*sp) {
        if (*sp is ' ' or *sp is '\n') break;
        if (*sp is '-' and (sp is args or *(sp - 1) is ' ')) {
          if (*(sp + 1) isnot '-') {
            if (is_long) {
              sp += 2;
              continue;
            }

            String.append_byte (arg, *sp++);
            continue;
          } else {
            String.append_with_len (arg, "--", 2);
            sp += 2;
            continue;
          }
        }

        if (*sp is ',') {
          if (arg->num_bytes > buflen) {
            if (Cstring.eq_n (ptr, arg->bytes + buflen, arglen)) {
              zs->num_items++;
              String.append_with_len (arg, lptr, lptrlen);
              Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes - lptrlen);
              zs->arg_type = ZS_RLINE_ARG_IS_ARG;
            }

            String.clear_at (arg, buflen);
          }

          sp++;
          continue;
        }

        String.append_byte (arg, *sp++);
      }

      if (arg->num_bytes > buflen) {
        if (Cstring.eq_n (ptr, arg->bytes + buflen, arglen)) {
          zs->num_items++;
          String.append_with_len (arg, lptr, lptrlen);
          Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes - lptrlen);
          zs->arg_type = ZS_RLINE_ARG_IS_ARG;
        }

        String.clear_at (arg, buflen);
      }
    }

    fclose (fp);
    ifnot (NULL is args)
      free (args);

    goto theend;
  }

  ptr = (char *) buf + buflen;
  int ptrlen = buflen;

  int is_filename = 0;
  while (ptrlen--) {
    char c = *(ptr-1);
    if (('0' <= c and c <= '9') or
        ('a' <= c and c <= 'z') or
        ('A' <= c and c <= 'Z') or
        c is '_' or c is '.' or c is '-') {
      ptr--;
      continue;
    }

    if (c is '/') {
      is_filename = 1;
      ptr--;
      continue;
    }
    break;
  }

  // half command
  if (-1 is ptrlen) {
    Command *it = zs->command_head;
    sp = (char *) buf;

    while (*sp) {
      if (*sp is '.') {
        sp++;
        while (it) {
          if (Cstring.eq_n (it->name, buf, buflen)) {
            zs->num_items++;
            String.replace_with (arg, it->name);
            if (sp - 1 is lptr) {
              lptr++; lptrlen--;
            }

            String.append_with_len (arg, lptr, lptrlen);
            Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes - lptrlen);
            zs->arg_type = ZS_RLINE_ARG_IS_COMMAND;
          }

          it = it->next;
        }

        goto theend;
      }
      sp++;
    }

    while (it) {
      char *name = it->name;
      if (Cstring.eq_n (name, buf, buflen)) {
        while (*name) {
          String.append_byte (arg, *name);
          if (*name++ is '.') break;
        }

        zs->num_items++;
        Rline.add_completion (rline, lc, arg->bytes, -1);
        zs->arg_type = ZS_RLINE_ARG_IS_COMMAND;

        while (it->next) {
          if (Cstring.eq_n (arg->bytes, it->next->name, arg->num_bytes)) {
            it = it->next;
            continue;
          }
          break;
        }

        String.clear (arg);
      }

      it = it->next;
    }

    goto theend;
  }

  size_t diff = buflen - ptrlen - 1;
  buflen = (buflen - diff);
  ptrlen = diff;

  String.replace_with_len (arg, buf, buflen);

  Cstring.cp (ptrbuf, ptrlen + 1, ptr, ptrlen);

  dirname = Path.dirname (ptrbuf);
  char *basename = Path.basename (ptrbuf);

  is_filename = 0;

  if (Dir.is_directory (ptrbuf) or (File.is_lnk (ptrbuf) and Dir.lnk_is_directory (ptrbuf))) {
    if (ptrbuf[ptrlen-1] isnot DIR_SEP) {
      zs->num_items++;
      String.append_with_fmt (arg, "%s%c", ptrbuf, DIR_SEP);
      String.append_with_len (arg, lptr, lptrlen);
      Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes - lptrlen);
      zs->arg_type = ZS_RLINE_ARG_IS_DIRECTORY;
      goto theend;
    }

    is_filename = 1;
    dlist  = Dir.list (ptrbuf, DIRLIST_LNK_IS_DIRECTORY);
  } else ifnot (ptrlen) {
get_current: {}
    char *cwd = Dir.current ();
    if (NULL is cwd) return;
    dlist = Dir.list (cwd, 0);
    free (cwd);
  } else {
    if (0 is Dir.is_directory (dirname) and (
        0 is File.is_lnk (dirname) and Dir.lnk_is_directory (dirname)))
      goto get_current;

    dlist = Dir.list (dirname, DIRLIST_LNK_IS_DIRECTORY);
  }

  if (NULL is dlist) goto theend;

  vstring_t *it = dlist->list->head;

  size_t dirlen = bytelen (dirname);
  size_t bname_len = bytelen (basename);

  if (1 is dirlen and *dirname is '.' and 0 is bname_len + is_filename) {
    while (it) {
      if (it->data->bytes[0] isnot '.') {
        zs->num_items++;
        String.append_with_len (arg, it->data->bytes, it->data->num_bytes);
        String.append_with_len (arg, lptr, lptrlen);
        Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes - lptrlen);
        zs->arg_type = ZS_RLINE_ARG_IS_FILENAME;
        String.clear_at (arg, buflen);
      }
      it = it->next;
    }
  } else {
    while (it) {
      if (is_filename) {
        zs->num_items++;
        String.append_with_fmt (arg, "%s%s%s", ptrbuf,
            (ptrbuf[ptrlen-1] is DIR_SEP ? "" : DIR_SEP_STR), it->data->bytes);
        String.append_with_len (arg, lptr, lptrlen);
        Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes - lptrlen);
        zs->arg_type = ZS_RLINE_ARG_IS_FILENAME;
        String.clear_at (arg, buflen);

      } else if (Cstring.eq_n (it->data->bytes, basename, bname_len)) {
        zs->num_items++;
        if (dirlen is 1 and *dirname is '.')
          String.append_with_len (arg, it->data->bytes, it->data->num_bytes);
        else
          String.append_with_fmt (arg, "%s%s%s", dirname,
            (dirname[dirlen-1] is DIR_SEP ? "" : DIR_SEP_STR), it->data->bytes);

        String.append_with_len (arg, lptr, lptrlen);
        Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes - lptrlen);
        zs->arg_type = ZS_RLINE_ARG_IS_FILENAME;
        String.clear_at (arg, buflen);
      }

      it = it->next;
    }
  }

theend:
  ifnot (NULL is dlist) dlist->release (dlist);
  ifnot (NULL is dirname) free (dirname);
}

static int zs_on_input (const char *buf, string *prevLine, int *ch, int curpos, rlineCompletions *lc, void *userdata) {
  zs_t *zs = (zs_t *) userdata;
  rline_t *rline = zs->rline;

  int c = *ch;

  // complete command
  if (0 is curpos and buf[0] is '\0') {
    if (('A' <= c and c <= 'Z')) { // or ('a' <= c and c <= 'z') or c is '_') {
      char newbuf[2]; newbuf[0] = c; newbuf[1] = '\0';
      String.replace_with_len (prevLine, newbuf, 1);
      zs_completion (newbuf, 1, lc, userdata);
      ifnot (zs->num_items) return -1;
      *ch = 0;
      return 0;
    }

    return c;
  }

  // escape: (maybe vi mode?)
  if (c is 033)
    return 033;

  // CTRL('/') or CTRL('-') instead of ALT('.')
  if (c is 037 and zs->numLastComp) {
    zs->num_items = 0;
    Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);

    const char *ptr = buf + curpos;
    size_t ptrlen = bytelen (ptr);

    string *arg = zs->arg;
    size_t len = bytelen (buf);
    len -= ptrlen;

    String.replace_with_len (arg, buf, len);

    LastComp *it = zs->lastcomp_head;
    while (it) {
      zs->num_items++;
      size_t llen = bytelen (it->lastcomp);
      String.append_with_len (arg, it->lastcomp, llen);
      String.append_with_len (arg, ptr, ptrlen);
      Rline.add_completion (rline, lc, arg->bytes, curpos + llen);
      zs->arg_type = ZS_RLINE_ARG_IS_LAST_COMPONENT;
      String.clear_at (arg, len);
      it = it->next;
    }

    ifnot (zs->num_items) return -1;

    return 0;
  }

  if (c is '~') {
    if (curpos > 0) {
      if (buf[curpos - 1] is ' ') {
        zs->num_items = 1;
        Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);
        string *arg = zs->arg;

        const char *ptr = buf + curpos;
        size_t ptrlen = bytelen (ptr);
        size_t len = bytelen (buf) - ptrlen;

        String.replace_with_len (arg, buf, len);
        char *home = Sys.get.env_value ("HOME");
        size_t homlen = bytelen (home);
        String.append_with_len (arg, home, homlen);
        String.append_byte (arg, DIR_SEP);
        String.append_with_len (arg, ptr, ptrlen);
        Rline.add_completion (rline, lc, arg->bytes, curpos + homlen + 1);
        zs->arg_type = ZS_RLINE_ARG_IS_DIRECTORY;
        String.clear_at (arg, len);
        return 0;
      }
    }
  }

  return -1;
}

static int zs_accept_one_item (const char *buf, void *userdata) {
  zs_t *zs = (zs_t *) userdata;
  (void) zs; (void) buf;

//  if (zs->arg_type is ZS_RLINE_ARG_IS_COMMAND)
//    if ('a' <= *buf and *buf <= 'z')
//      return -1;

  return 1;
}

static void zs_on_carriage_return (const char *buf, void *userdata) {
  zs_t *zs = (zs_t *) userdata;

  size_t len = bytelen (buf);
  ifnot (len) return;

  char *ptr = (char *) buf + len;
  while (*(ptr - 1) is ' ') ptr--;
  char *end = ptr;

  while (*(ptr - 1) isnot ' ') {
    ptr--;
    if (ptr is buf)
      break;
  }

  len = end - ptr;
  ifnot (len) return;

  LastComp *lastcomp = Alloc (sizeof (LastComp));
  lastcomp->lastcomp = Cstring.dup (ptr, len);

  zs->numLastComp++;

  if (zs->lastcomp_head is NULL) {
      zs->lastcomp_head = lastcomp;
      zs->lastcomp_head->next = NULL;
  } else {
    lastcomp->next = zs->lastcomp_head;
    zs->lastcomp_head = lastcomp;
    if (zs->numLastComp > MAXNUM_LAST_COMPONENTS) {
      zs->numLastComp--;
      LastComp *it = zs->lastcomp_head;
      int idx = 0;
      while (++idx < MAXNUM_LAST_COMPONENTS) it = it->next;
      free (it->next->lastcomp);
      free (it->next);
      it->next = NULL;
    }
  }
}

static char *zs_hints (const char *buf, int *color, int *bold, void *userdata) {
  (void) buf;
  zs_t *zs = (zs_t *) userdata;
  if (zs->num_items > 1) {
    Cstring.cp_fmt (zs->hint, MAXLEN_HINT + 1, " [%d items]", zs->num_items);
    *color = 33; *bold = 1;
    zs->num_items = 0;
    return zs->hint;
  }

  return NULL;
}

static void init_rline_commands (zs_t *this) {
  Command *comit;
  Command *head = Alloc (sizeof (Command));
  head->name = Cstring.dup ("exit", 4);
  comit = head;

  Command *next = Alloc (sizeof (Command));
  next->name = Cstring.dup ("cd", 2);
  comit->next = next;
  comit = next;

  next = Alloc (sizeof (Command));
  next->name = Cstring.dup ("pwd", 3);
  comit->next = next;
  comit = next;

  char *flags = NULL;
  dirlist_t *dlist = NULL; // silence clang
  dlist = Dir.list (this->comdir->bytes, 0);
  if (NULL is dlist) goto theend;

  size_t flagslen = 0;

  vstring_t *it = dlist->list->head;
  while (it) {
    if (it->data->bytes[it->data->num_bytes - 1] isnot DIR_SEP) {
      it = it->next;
      continue;
    }

    String.clear_at (it->data, -1);

    next = Alloc (sizeof (Command));

    next->name = Cstring.dup (it->data->bytes, it->data->num_bytes);

    size_t filelen = this->comdir->num_bytes + 1 + it->data->num_bytes + sizeof ("/flags");
    char file[filelen + 1];
    Cstring.cp_fmt (file, filelen + 1, "%s/%s/flags", this->comdir->bytes, it->data->bytes);

    next->flags = 0;

    if (0 is access (file, F_OK|R_OK)) {
      FILE *fp = fopen (file, "r");
      if (NULL isnot fp) {
        ssize_t nread = getline (&flags, &flagslen, fp);
        if (-1 isnot nread) {
          flags[nread - 1] = '\0';
          next->flags = atoi (flags);
        }
        fclose (fp);
      }
    }

    comit->next = next;
    comit = next;
    it = it->next;
  }

theend:
  ifnot (NULL is flags) free (flags);
  ifnot (NULL is dlist) dlist->release (dlist);
  comit->next = NULL;
  this->command_head = head;
}

static void deinit_commands (Command *it) {
  while (it) {
    Command *tmp = it->next;
    free (it->name);
    free (it);
    it = tmp;
  }
}

static void deinit_lastcomp (LastComp *it) {
  while (it) {
    LastComp *tmp = it->next;
    free (it->lastcomp);
    free (it);
    it = tmp;
  }
}

static zs_t *zs_init_rline (void) {
  zs_t *zs = Alloc (sizeof (zs_t));
  rline_t *this = Rline.new ();

  char *datadir = Sys.get.env_value ("DATADIR");
  zs->comdir = String.new_with_fmt ("%s/zs/commands", datadir);
  size_t len = bytelen (datadir) + sizeof ("/zs/.zs_history");
  char histfile[len + 1];
  Cstring.cp_fmt (histfile, len + 1, "%s/zs/.zs_history", datadir);

  Rline.history.set.file (this, histfile);
  Rline.history.load (this);

  zs->arg = String.new (32);

  ifnot (getuid ())
    Rline.set.prompt (this, DEFAULT_ROOT_PROMPT);
  else
    Rline.set.prompt (this, DEFAULT_USER_PROMPT);

  init_rline_commands (zs);

  Rline.set.hints_cb (this, zs_hints, zs); // UNUSED
  Rline.set.completion_cb (this, zs_completion, zs);
  Rline.set.on_input_cb (this, zs_on_input);
  Rline.set.on_carriage_return_cb (this, zs_on_carriage_return);
  Rline.set.accept_one_item_cb (this, zs_accept_one_item);

  zs->rline = this;

  zs->lastcomp_head = NULL;
  zs->numLastComp = 0;
  return zs;
}

static int zs_interactive (sh_t *this) {
  int retval = OK;
  char *line;

  zs_t *zs = zs_init_rline ();

  rline_t *rline = zs->rline;

  for (;;) {
    errno = 0;
    line = Rline.edit (rline);
    if (line is NULL) {
      if (errno is EAGAIN)
        continue;
       break;
    }

    char *save_line = line;

    signal (SIGINT, SIG_IGN);
    retval = Sh.exec (this, line);
    signal (SIGINT, SIG_DFL);
    zs->last_retval = retval;
    line = save_line;

    Rline.history.add (rline, line);
    free (line);
    Sh.release_list (this);

    if (Sh.should_exit (this)) break;
  }

  Rline.history.save (rline);
  Rline.history.release (rline);
  Rline.release (rline);
  String.release (zs->comdir);
  String.release (zs->arg);
  deinit_commands (zs->command_head);
  deinit_lastcomp (zs->lastcomp_head);
  free (zs);

  return retval;
}

int main (int argc, char **argv) {
  __INIT__ (io);
  __INIT__ (sh);
  __INIT__ (sys);
  __INIT__ (dir);
  __INIT__ (path);
  __INIT__ (file);
  __INIT__ (error);
  __INIT__ (rline);
  __INIT__ (string);
  __INIT__ (vstring);
  __INIT__ (cstring);

  Sys.init_environment (SysEnvOpts());

  char dir[MAXLEN_DIR + 1]; dir[0] = '\0';
  char fname[MAXLEN_DIR + 1]; fname[0] = '\0';
  char command[MAXLEN_COMMAND_LINE + 1]; command[0] = '\0';

  int retval = 0;

  argc--; argv++;

  sh_t *this = NULL;
  char *curdir = NULL;
  int is_command = 0;
  size_t comlen = 0;
  size_t fnamelen = 0;

  for (int i = 0; i < argc; i++) {
    ifnot (is_command) {
      if (Cstring.eq_n (argv[i], "--chdir=", 8)) {
        size_t dirlen = bytelen (argv[i] + 8);
        if (dirlen > MAXLEN_DIR) {
          Stderr.print_fmt ("--chdir=%s, path name is too long", argv[i] + 8);
          retval = 1;
          goto theend;
        }

        Cstring.cp (dir, MAXLEN_DIR + 1, argv[i] + 8, dirlen);
        continue;
      }

      if (Cstring.eq (argv[i], "-c")) {
        is_command = 1;
        continue;
      }

      fnamelen = bytelen (argv[i]);
      if (fnamelen > MAXLEN_DIR) {
        Stderr.print_fmt ("%s, path name is too long", argv[i]);
        retval = 1;
        goto theend;
      }

      Cstring.cp (fname, MAXLEN_DIR + 1, argv[i], fnamelen);
      break;
    }

    if (comlen) {
      Cstring.cat (command, MAXLEN_COMMAND_LINE + 1, " ");
      comlen++;
    }

    comlen += bytelen (argv[i]);
    if (comlen > MAXLEN_COMMAND_LINE) {
      Stderr.print_fmt ("comands are too long too fit");
      retval = 1;
      goto theend;
    }

    Cstring.cat (command, MAXLEN_COMMAND_LINE + 1, argv[i]);
  }

  this = Sh.new ();

  curdir = Dir.current ();
  if (NULL is curdir) {
    Stderr.print ("cannot determinate current working directory\n");
    retval = 1;
    goto theend;
  }

  if (dir[0]) {
    if (chdir (dir) is -1) {
      Stderr.print_fmt ("%s, can not change to this directory, %s\n",
          dir, Error.errno_string (errno));
      retval = 1;
      goto theend;
    }

    setenv ("PWD", dir, 1);
  } else
    setenv ("PWD", curdir, 1);

  ifnot (comlen + fnamelen) {
    retval = zs_interactive (this);
    goto get_exit_val;
  }

  if (comlen) {
    retval = Sh.exec (this, command);
    goto get_exit_val;
  }

  retval = Sh.exec_file (this, fname);

  get_exit_val:
  if (retval is NOTOK)
    retval = 1;
  else
    retval = Sh.get.exit_val (this);

theend:
  ifnot (NULL is curdir)
    free (curdir);

  __deinit_sys__ ();

  ifnot (NULL is this)
    Sh.release (this);

  return retval;
}
