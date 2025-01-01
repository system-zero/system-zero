#define DT_UNKNOWN  0
#define DT_FIFO     1
#define DT_CHR      2
#define DT_DIR      4
#define DT_BLK      6
#define DT_REG      8
#define DT_LNK     10
#define DT_SOCK    12
#define DT_WHT     14

struct dirent { // dirent64
  ino64_t        d_ino;       /* 64-bit inode number */
  off64_t        d_off;       /* 64-bit offset to next structure */
  unsigned short d_reclen;    /* Size of this dirent */
  unsigned char  d_type;      /* File type */
  char           d_name[256]; /* Filename (null-terminated) */
};

struct _DIR {
  int fd;
  int buf_pos;
  int buf_end;
  char buf[2048];
};

typedef struct _DIR DIR;
