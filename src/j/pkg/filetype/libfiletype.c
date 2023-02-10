#define REQUIRE_STRCAT
#define REQUIRE_STRLEN
#define REQUIRE_STR_EQ_N
#define REQUIRE_STR_COPY
#define REQUIRE_LSTAT
#define REQUIRE_FCNTL_H
#define REQUIRE_OPEN
#define REQUIRE_READ
#define REQUIRE_ACCESS

#include <libc.h>

#include <z/filetype.h>

#define TAR_HEADER_LEN 512
#define TAR_NAME_LEN   100

typedef struct {
  char name[TAR_NAME_LEN];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char type;
  char linkName[TAR_NAME_LEN];
  char _padding[255];
} tarHeader;

static int is_tar (unsigned char *buf) {
  if (*buf == '\0') return 0;

  tarHeader *h = (tarHeader *) buf;

  int sum = 0;
  unsigned char *p = buf;
  unsigned char *end = p + sizeof (*h);

  while (p < end) sum += *p++;

  for (size_t i = 0; i < sizeof (h->checksum); i++)
    sum -= h->checksum[i];
  sum += ' ' * sizeof(h->checksum);

  size_t clen = sizeof (h->checksum);
  char *checksum = h->checksum;

  while (*checksum == ' ' || *checksum == '\0') {
    if (clen-- == 0) return 0;
    checksum++;
  }

  int r = 0;
  char c;
  while (clen > 0) {
    c = *checksum;
    if (c == '\0') break;
    if ('0' > c || c > '7') return 0;
    r = 8 * r + (c - '0');
    checksum++;
    clen--;
  }

  return r == sum;
}

static int shebang (char *buf, char type[MAXLEN_FILETYPE]) {
  char *sp = buf + 2;

  if (str_eq_n (sp, "/bin/sh", 7)) {
    str_copy (type, MAXLEN_FILETYPE, "POSIX sh script", MAXLEN_FILETYPE - 1);
    return FILETYPE_POSIX_SH;
  }

  if (str_eq_n (sp, "/bin/zsh", 8)) {
    str_copy (type, MAXLEN_FILETYPE, "zsh script", MAXLEN_FILETYPE - 1);
    return FILETYPE_ZSH;
  }

  if (str_eq_n (sp, "/bin/bash", 9)) {
    str_copy (type, MAXLEN_FILETYPE, "bash shell script", MAXLEN_FILETYPE - 1);
    return FILETYPE_BASH;
  }

  if (str_eq_n (sp, "/bin/env ", 9) ||
      str_eq_n (sp, "/usr/bin/env ", 13))
    sp = strchr (sp, ' ') + 1;

  int i = 0;
  while (*sp != '\n' && i < MAXLEN_FILETYPE)
    type[i++] = *sp++;
  type[i] = '\0';

  str_cat (type,  MAXLEN_FILETYPE, " script");
  return FILETYPE_SCRIPT;
}

int filetype_from_string (const char *buf, size_t len, char type[MAXLEN_FILETYPE]) {
  type[0] = '\0';
  if (NULL == buf) return FILETYPE_ERROR;

  if (0 == len) return FILETYPE_EMPTY_FILE;

  if (buf[0] == '#' && buf[1] == '!')
    return shebang ((char *) buf, type);

  if (str_eq_n (buf, "BZh", 3)) {
    str_copy (type, MAXLEN_FILETYPE, "bzip archive", MAXLEN_FILETYPE - 1);
    return FILETYPE_BZIP;
  }

  if (buf[0] == 0x1f && len > 1) {
    if ((unsigned char) buf[1] == 0x9d) {
      str_copy (type, MAXLEN_FILETYPE, "compressed file", MAXLEN_FILETYPE - 1);
      return FILETYPE_COMPRESSED;
    }

    if ((unsigned char) buf[1] == 0x8b) {
      str_copy (type, MAXLEN_FILETYPE, "gzip archive", MAXLEN_FILETYPE - 1);
      return FILETYPE_GZIP;
    }
  }

  if (str_eq_n (buf, "PK", 2) && (len > 3 && buf[2] == 0x03 && buf[3] == 0x04)) {
    str_copy (type, MAXLEN_FILETYPE, "zip archive", MAXLEN_FILETYPE - 1);
    return FILETYPE_ZIP;
  }

  if (len > 5 &&
      (unsigned char) buf[0] == 0xfd && buf[1] == 0x37 && buf[2] == 0x7a &&
      buf[3] == 0x58 && buf[4] == 0x5a && buf[5] == 0x00) {
    str_copy (type, MAXLEN_FILETYPE, "xz archive", MAXLEN_FILETYPE - 1);
    return FILETYPE_XZ;
  }

  if (len > 263 &&
      buf[257] == 0x75 && buf[258] == 0x73 && buf[259] == 0x74 &&
      buf[260] == 0x61 && buf[261] == 0x72) {
    if (buf[262] == 0x00 && buf[263] == 0x30 && buf[264] == 0x30) {
      str_copy (type, MAXLEN_FILETYPE, "POSIX tar archive", MAXLEN_FILETYPE - 1);
      return FILETYPE_TAR_POSIX;
    }

    if (buf[262] == 0x20 && buf[263] == 0x20 && buf[264] == 0x00) {
      str_copy (type, MAXLEN_FILETYPE, "POSIX tar archive (GNU)", MAXLEN_FILETYPE - 1);
      return FILETYPE_TAR_POSIX_GNU;
    }
  }

  if (len == TAR_HEADER_LEN && is_tar ((unsigned char *) buf)) {
    str_copy (type, MAXLEN_FILETYPE, "tar archive", MAXLEN_FILETYPE - 1);
    return FILETYPE_TAR;
  }

  if (str_eq_n (buf + 1, "ELF", 3) && buf[0] == 0x7f) {
    str_copy (type, MAXLEN_FILETYPE, "dynamically compiled elf object", MAXLEN_FILETYPE - 1);
    return FILETYPE_ELF_OBJECT;
  }

  if (str_eq_n (buf, "!<arch>", 7)) {
    str_copy (type, MAXLEN_FILETYPE, "statically compiled object", MAXLEN_FILETYPE - 1);
    return FILETYPE_STATIC_OBJECT;
  }

  if (len > 3 &&
      buf[0] == 0x00 && buf[1] == 'a' && buf[2] == 's' &&
      buf[3] == 'm') {
    str_copy (type, MAXLEN_FILETYPE, "web assembly binary", MAXLEN_FILETYPE - 1);
    return FILETYPE_WEB_ASSEMBLY_BIN;
  }

  if (str_eq_n (buf, "Cr24", 4)) {
    str_copy (type, MAXLEN_FILETYPE, "chrome extension or packaged app", MAXLEN_FILETYPE - 1);
    return FILETYPE_CHROME_EXTENSION;
  }

  if (buf[0] == 0x1b && str_eq_n (buf + 1, "Lua", 3)) {
    str_copy (type, MAXLEN_FILETYPE, "Lua bytecode", MAXLEN_FILETYPE - 1);
    return FILETYPE_LUA_BYTECODE;
  }

  if (str_eq_n (buf, "%!PS", 4)) {
    str_copy (type, MAXLEN_FILETYPE, "postscript", MAXLEN_FILETYPE - 1);
    return FILETYPE_PS;
  }

  if (str_eq_n (buf, "%PDF-", 5)) {
    str_copy (type, MAXLEN_FILETYPE, "pdf document", MAXLEN_FILETYPE - 1);
    return FILETYPE_PDF;
  }

  if (str_eq_n (buf, "ID3", 3)) {
    str_copy (type, MAXLEN_FILETYPE, "mp3 with ID3v2 container", MAXLEN_FILETYPE - 1);
    return FILETYPE_MP3_WITH_ID3V2;
  }

  if (str_eq_n (buf, "fLaC", 4)) {
    str_copy (type, MAXLEN_FILETYPE, "free losless audio codec (flac)", MAXLEN_FILETYPE - 1);
    return FILETYPE_FLAC;
  }

  if (str_eq_n (buf, "OggS", 4)) {
    str_copy (type, MAXLEN_FILETYPE, "ogg media", MAXLEN_FILETYPE - 1);
    return FILETYPE_OGG;
  }

  if (len > 3 &&
      buf[0] == 0x1a && buf[1] == 0x45 && (unsigned char) buf[2] == 0xdf &&
      (unsigned char) buf[3] == 0xa3) {
    str_copy (type, MAXLEN_FILETYPE, "matroska container / webm media", MAXLEN_FILETYPE - 1);
    return FILETYPE_MATROSKA_WEBM;
  }

  if (len > 11 && str_eq_n (buf + 4, "ftypisom", 8)) {
    str_copy (type, MAXLEN_FILETYPE, "mp4 (MPEG-4)", MAXLEN_FILETYPE - 1);
    return FILETYPE_MP4;
  }

  if (str_eq_n (buf, "GIF87a", 6)) {
    str_copy (type, MAXLEN_FILETYPE, "gif image", MAXLEN_FILETYPE - 1);
    return FILETYPE_GIF;
  }

  if ((unsigned char) buf[0] == 0x89 && (len > 5 &&
      str_eq_n (buf + 1, "PNG", 3) && buf[4] == 0x0d && buf[5] == 0x0a)) {
    str_copy (type, MAXLEN_FILETYPE, "png image", MAXLEN_FILETYPE - 1);
    return FILETYPE_PNG;
  }

  if ((unsigned char) buf[0] == 0xff && (len > 3 &&
      (unsigned char) buf[1] == 0xd8 && (unsigned char) buf[2] == 0xff &&
      (unsigned char) buf[3] == 0xe0)) {
    str_copy (type, MAXLEN_FILETYPE, "jpeg image", MAXLEN_FILETYPE - 1);
    return FILETYPE_JPEG;
  }

  if (len > 4 &&
      buf[0] == 0x00 && buf[1] == 0x01 && buf[2] == 0x00 &&
      buf[3] == 0x00 && buf[4] == 0x00) {
    str_copy (type, MAXLEN_FILETYPE, "ttf (TrueType font)", MAXLEN_FILETYPE -1);
    return FILETYPE_TTF;
  }

  str_copy (type, MAXLEN_FILETYPE, "text", MAXLEN_FILETYPE - 1);
  return FILETYPE_TXT;
}

int filetype (const char *file, char type[MAXLEN_FILETYPE]) {
  type[0] = '\0';

  if (NULL == file) return FILETYPE_ERROR;

  struct stat st;
  if (-1 == lstat (file, &st)) {
    str_copy (type, MAXLEN_FILETYPE, "non-existent", MAXLEN_FILETYPE - 1);
    return FILETYPE_NON_EXISTANT;
  }

  if (S_ISDIR(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "directory", MAXLEN_FILETYPE - 1);
    return FILETYPE_DIRECTORY;
  }

  if (S_ISCHR(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "character device", MAXLEN_FILETYPE - 1);
    return FILETYPE_CHAR_DEVICE;
  }

  if (S_ISBLK(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "block device", MAXLEN_FILETYPE - 1);
    return FILETYPE_BLOCK_DEVICE;
  }

  if (S_ISFIFO(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "fifo (named pipe)", MAXLEN_FILETYPE - 1);
    return FILETYPE_NAMED_PIPE;
  }

  if (S_ISLNK(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "symbolic link", MAXLEN_FILETYPE - 1);
    return FILETYPE_SYMLINK;
  }

  if (S_ISSOCK(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "socket", MAXLEN_FILETYPE - 1);
    return FILETYPE_SOCKET;
  }

  int fd = open (file, O_RDONLY);
  if (-1 == fd) return FILETYPE_ERROR;

  char buf[512];
  size_t nread = sys_read (fd, buf, 512);

  if (0 >= nread) {
    str_copy (type, MAXLEN_FILETYPE, "empty file", MAXLEN_FILETYPE - 1);
    return FILETYPE_EMPTY_FILE;
  }

  buf[nread] = '\0';

  int r = filetype_from_string (buf, nread, type);

  if (0 == access (file, X_OK))
    str_cat (type, MAXLEN_FILETYPE, " executable");

  return r;
}
