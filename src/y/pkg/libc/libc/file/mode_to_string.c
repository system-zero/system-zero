// as: file_mode_to_string
// provides: char *file_mode_to_string (char *, mode_t)
// requires: sys/stat.h

#define REG_CHAR   '-'
#define BLK_CHAR   'b'
#define CHR_CHAR   'c'
#define DIR_CHAR   'd'
#define LNK_CHAR   'l'
#define FIFO_CHAR  'p'
#define SOCK_CHAR  's'
#define SUID_CHAR  's'
#define SGID_CHAR  'g'
#define SVTX_CHAR  't'

char *file_mode_to_string (char *mode_string, mode_t mode) {
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
