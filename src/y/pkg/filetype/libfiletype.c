#define REQUIRE_STRCAT
#define REQUIRE_STRLEN
#define REQUIRE_STR_EQ_N
#define REQUIRE_STR_COPY
#define REQUIRE_LSTAT
#define REQUIRE_FCNTL_H
#define REQUIRE_OPEN
#define REQUIRE_READ
#define REQUIRE_ACCESS

#include "../../generatedLibc/libc.c"

#include <z/filetype.h>

static char *shebang (char *buf, char type[MAXLEN_FILETYPE]) {
  char *sp = buf + 2;

  if (str_eq_n (sp, "/bin/sh", 7)) {
    str_copy (type, MAXLEN_FILETYPE, "POSIX sh script", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (str_eq_n (sp, "/bin/zsh", 8)) {
    str_copy (type, MAXLEN_FILETYPE, "zsh script", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (str_eq_n (sp, "/bin/bash", 9)) {
    str_copy (type, MAXLEN_FILETYPE, "bash shell script", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (str_eq_n (sp, "/bin/env ", 9) ||
      str_eq_n (sp, "/usr/bin/env ", 13))
    sp = strchr (sp, ' ') + 1;

  int i = 0;
  while (*sp != '\n' && i < MAXLEN_FILETYPE)
    type[i++] = *sp++;
  type[i] = '\0';

  str_cat (type,  MAXLEN_FILETYPE, " script");
  return type;
}

char *filetype (const char *file, char type[MAXLEN_FILETYPE]) {
  type[0] = '\0';

  if (NULL == file)
    return NULL;

  struct stat st;
  if (-1 == lstat (file, &st)) {
    str_copy (type, MAXLEN_FILETYPE, "non-existent", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (S_ISDIR(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "directory", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (S_ISCHR(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "character device", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (S_ISBLK(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "block device", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (S_ISFIFO(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "fifo (named pipe)", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (S_ISLNK(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "symbolic link", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (S_ISSOCK(st.st_mode)) {
    str_copy (type, MAXLEN_FILETYPE, "socket", MAXLEN_FILETYPE - 1);
    return type;
  }

  int fd = open (file, O_RDONLY);
  if (-1 == fd) return NULL;

  char buf[512];
  size_t nread = sys_read (fd, buf, 512);

  if (0 >= nread) {
    str_copy (type, MAXLEN_FILETYPE, "empty file", MAXLEN_FILETYPE - 1);
    return type;
  }

  if (buf[0] == '#' && buf[1] == '!') {
    shebang (buf, type);
    goto is_an_executable;
  }

  if (str_eq_n (buf + 1, "ELF", 3) && buf[0] == 0x7f)
    str_copy (type, MAXLEN_FILETYPE, "dynamically compiled elf object", MAXLEN_FILETYPE - 1);
  else if (str_eq_n (buf, "BZh", 3)) {
    str_copy (type, MAXLEN_FILETYPE, "bzip archive", MAXLEN_FILETYPE - 1);
    return type;
  } else if (buf[0] == 0x1f && nread > 1) {
    if ((unsigned char) buf[1] == 0x9d) {
      str_copy (type, MAXLEN_FILETYPE, "compressed file", MAXLEN_FILETYPE -1);
      return type;
    } else if ((unsigned char) buf[1] == 0x8b) {
      str_copy (type, MAXLEN_FILETYPE, "gzip archive", MAXLEN_FILETYPE -1);
      return type;
    }
  } else if (str_eq_n (buf, "PK", 2) &&
            (nread > 3 && buf[2] == 0x03 && buf[3] == 0x04)) {
    str_copy (type, MAXLEN_FILETYPE, "zip archive", MAXLEN_FILETYPE -1);
    return type;
  } else if (nread > 5 &&
      (unsigned char) buf[0] == 0xfd && buf[1] == 0x37 && buf[2] == 0x7a &&
      buf[3] == 0x58 && buf[4] == 0x5a && buf[5] == 0x00) {
    str_copy (type, MAXLEN_FILETYPE, "xz archive", MAXLEN_FILETYPE -1);
    return type;
  } else if (nread > 263 &&
      buf[257] == 0x75 && buf[258] == 0x73 && buf[259] == 0x74 &&
      buf[260] == 0x61 && buf[261] == 0x72 && buf[262] == 0x20 &&
      buf[263] == 0x20 && buf[264] == 0x00) {
    str_copy (type, MAXLEN_FILETYPE, "tar archive", MAXLEN_FILETYPE -1);
    return type;
  } else if (str_eq_n (buf, "ID3", 3)) {
    str_copy (type, MAXLEN_FILETYPE, "mp3 with ID3v2 container", MAXLEN_FILETYPE -1);
    return type;
  } else if (str_eq_n (buf, "%!PS", 4))
    str_copy (type, MAXLEN_FILETYPE, "postscript", MAXLEN_FILETYPE -1);
  else if (str_eq_n (buf, "Cr24", 4))
    str_copy (type, MAXLEN_FILETYPE, "chrome extension or packaged app", MAXLEN_FILETYPE -1);
  else if (str_eq_n (buf, "fLaC", 4))
    str_copy (type, MAXLEN_FILETYPE, "free losless audio codec (flac)", MAXLEN_FILETYPE -1);
  else if (str_eq_n (buf, "OggS", 4))
    str_copy (type, MAXLEN_FILETYPE, "ogg media", MAXLEN_FILETYPE -1);
  else if (str_eq_n (buf, "%PDF-", 5))
    str_copy (type, MAXLEN_FILETYPE, "pdf document", MAXLEN_FILETYPE -1);
  else if (str_eq_n (buf, "GIF87a", 6))
    str_copy (type, MAXLEN_FILETYPE, "gif image", MAXLEN_FILETYPE -1);
  else if (str_eq_n (buf, "!<arch>", 7))
    str_copy (type, MAXLEN_FILETYPE, "statically compiled object", MAXLEN_FILETYPE -1);
  else if ((unsigned char) buf[0] == 0x89 && (nread > 5 &&
      str_eq_n (buf + 1, "PNG", 3) && buf[4] == 0x0d && buf[5] == 0x0a))
    str_copy (type, MAXLEN_FILETYPE, "png image", MAXLEN_FILETYPE -1);
  else if ((unsigned char) buf[0] == 0xff && (nread > 3 &&
      (unsigned char) buf[1] == 0xd8 && (unsigned char) buf[2] == 0xff &&
      (unsigned char) buf[3] == 0xe0))
    str_copy (type, MAXLEN_FILETYPE, "jpg image", MAXLEN_FILETYPE -1);
  else if (nread > 3 &&
      buf[0] == 0x1a && buf[1] == 0x45 && (unsigned char) buf[2] == 0xdf &&
      (unsigned char) buf[3] == 0xa3)
    str_copy (type, MAXLEN_FILETYPE, "matroska container / webm media", MAXLEN_FILETYPE -1);
  else if (nread > 3 &&
      buf[0] == 0x00 && buf[1] == 'a' && buf[2] == 's' &&
      buf[3] == 'm')
    str_copy (type, MAXLEN_FILETYPE, "web assembly binary", MAXLEN_FILETYPE -1);
  else if (nread > 11 && str_eq_n (buf + 4, "ftypisom", 8))
    str_copy (type, MAXLEN_FILETYPE, "mp4 (MPEG-4)", MAXLEN_FILETYPE -1);
  else if (buf[0] == 0x1b && str_eq_n (buf + 1, "Lua", 3))
    str_copy (type, MAXLEN_FILETYPE, "Lua bytecode", MAXLEN_FILETYPE -1);
  else if (nread > 4 &&
      buf[0] == 0x00 && buf[1] == 0x01 && buf[2] == 0x00 &&
      buf[3] == 0x00 && buf[4] == 0x00)
    str_copy (type, MAXLEN_FILETYPE, "ttf (TrueType font)", MAXLEN_FILETYPE -1);
  else
    str_copy (type, MAXLEN_FILETYPE, "text", MAXLEN_FILETYPE -1);

is_an_executable:
  if (0 == access (file, X_OK))
    str_cat (type, MAXLEN_FILETYPE, " executable");

  return type;
}
