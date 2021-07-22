// some functions in this unit, are just to avoid circular dependencies
// some to use them for static targets

#define LIBRARY "OS"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT

#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_OS_TYPE       DONOT_DECLARE

#include <z/cenv.h>

#define GROUP_FILE "/etc/group"
#define PASSWD_FILE "/etc/passwd"

static char *os_get_grname (gid_t gid) {
  FILE *fp = fopen (GROUP_FILE, "r");
  if (NULL is fp)
    return NULL;

  size_t len = 0;
  char *line = NULL;
  ssize_t nread;

  char guid[32];
  Cstring.cp_fmt (guid, 32, "%d", gid);

  char *name = NULL;

  while (-1 isnot (nread = getline (&line, &len, fp))) {
    if (nread) {
      cstring_tok *ctok = Cstring.tokenize (NULL, line, ":", NULL, NULL);
      if (NULL is ctok)
        continue;

      if (ctok->num_tokens is 4) {
        if (Cstring.eq (ctok->tokens[2], guid)) {
          size_t tlen = strlen (ctok->tokens[0]);
          name = Alloc (tlen + 1);
          Cstring.cp (name, tlen + 1, ctok->tokens[0], tlen);
          Cstring.tok_release (ctok);
          break;
        }
      }

      Cstring.tok_release (ctok);
    }
  }


  ifnot (NULL is line)
    free (line);

  fclose (fp);
  return name;
}

static char *os_get_pwname (uid_t uid) {
  FILE *fp = fopen (PASSWD_FILE, "r");
  if (NULL is fp)
    return NULL;

  size_t len = 0;
  char *line = NULL;
  ssize_t nread;

  char suid[32];
  Cstring.cp_fmt (suid, 32, "%d", uid);

  char *name = NULL;

  while (-1 isnot (nread = getline (&line, &len, fp))) {
    if (nread) {
      cstring_tok *ctok = Cstring.tokenize (NULL, line, ":", NULL, NULL);
      if (NULL is ctok)
        continue;

      if (ctok->num_tokens is 7) {
        if (Cstring.eq (ctok->tokens[2], suid)) {
          size_t tlen = strlen (ctok->tokens[0]);
          name = Alloc (tlen + 1);
          Cstring.cp (name, tlen + 1, ctok->tokens[0], tlen);
          Cstring.tok_release (ctok);
          break;
        }
      }

      Cstring.tok_release (ctok);
    }
  }

  ifnot (NULL is line)
    free (line);

  fclose (fp);
  return name;
}

static char *os_get_pwdir (uid_t uid) {
  FILE *fp = fopen (PASSWD_FILE, "r");
  if (NULL is fp)
    return NULL;

  size_t len = 0;
  char *line = NULL;
  ssize_t nread;

  char suid[32];
  Cstring.cp_fmt (suid, 32, "%d", uid);

  char *name = NULL;

  while (-1 isnot (nread = getline (&line, &len, fp))) {
    if (nread) {
      cstring_tok *ctok = Cstring.tokenize (NULL, line, ":", NULL, NULL);
      if (NULL is ctok)
        continue;

      if (ctok->num_tokens is 7) {
        if (Cstring.eq (ctok->tokens[2], suid)) {
          size_t tlen = strlen (ctok->tokens[5]);
          name = Alloc (tlen + 1);
          Cstring.cp (name, tlen + 1, ctok->tokens[5], tlen);
          Cstring.tok_release (ctok);
          break;
        }
      }

      Cstring.tok_release (ctok);
    }
  }

  ifnot (NULL is line)
    free (line);

  fclose (fp);
  return name;
}


/* exposed here, but you may use it (normally) from the File library */
static char *os_mode_stat_to_string (char *mode_string, mode_t mode) {
  /* assumed at least 11 bytes */

  if      (S_ISREG(mode))  mode_string[0] = REG_CHAR;
  else if (S_ISDIR(mode))  mode_string[0] = DIR_CHAR;
  else if (S_ISLNK(mode))  mode_string[0] = LNK_CHAR;
  else if (S_ISCHR(mode))  mode_string[0] = CHR_CHAR;
  else if (S_ISBLK(mode))  mode_string[0] = BLK_CHAR;
  else if (S_ISFIFO(mode)) mode_string[0] = FIFO_CHAR;
  else if (S_ISSOCK(mode)) mode_string[0] = SOCK_CHAR;

  if (mode & S_IRUSR) mode_string[1] = 'r'; else mode_string[1] = '-';
  if (mode & S_IWUSR) mode_string[2] = 'w'; else mode_string[2] = '-';
  if (mode & S_IXUSR) mode_string[3] = 'x'; else mode_string[3] = '-';
  if (mode & S_ISUID) mode_string[3] = SUID_CHAR;

  if (mode & S_IRGRP) mode_string[4] = 'r'; else mode_string[4] = '-';
  if (mode & S_IWGRP) mode_string[5] = 'w'; else mode_string[5] = '-';
  if (mode & S_IXGRP) mode_string[6] = 'x'; else mode_string[6] = '-';
  if (mode & S_ISGID) mode_string[6] = SGID_CHAR;

  if (mode & S_IROTH) mode_string[7] = 'r'; else mode_string[7] = '-';
  if (mode & S_IWOTH) mode_string[8] = 'w'; else mode_string[8] = '-';
  if (mode & S_IXOTH) mode_string[9] = 'x'; else mode_string[9] = '-';
  if (mode & S_ISVTX) mode_string[9] = SVTX_CHAR;

  mode_string[10] = '\0';
  return mode_string;
}

public os_T __init_os__ () {
  __INIT__ (cstring);

  return (os_T) {
    .self = (os_self) {
      .get = (os_get_self) {
        .pwdir = os_get_pwdir,
        .pwname = os_get_pwname,
        .grname = os_get_grname
      },
      .mode = (os_mode_self) {
        .stat_to_string = os_mode_stat_to_string,
      }
    }
  };
}

public void __deinit_os__ (os_T **thisp) {
  (void) thisp;
  return;
}
