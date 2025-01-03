#define O_RDONLY    00000
#define O_WRONLY    00001
#define O_RDWR      00002
#define O_ACCMODE   00003
#define O_CREAT     00100
#define O_EXCL      00200
#define O_NOCTTY    00400
#define O_TRUNC     01000
#define O_APPEND    02000
#define O_NONBLOCK  04000
#define O_NDELAY    O_NONBLOCK
#define O_SYNC      0010000
#define FASYNC      0020000
#define O_DIRECT    0040000
#define O_LARGEFILE 0100000
#define O_DIRECTORY 0200000
#define O_NOFOLLOW  0400000
#define O_NOATIME   01000000
#define O_CLOEXEC   02000000
#define O_PATH      010000000
#define O_TMPFILE   020200000

#define F_DUPFD     0
#define F_GETFD     1
#define F_SETFD     2
#define F_GETFL     3
#define F_SETFL     4
#define F_GETLK     5
#define F_SETLK     6
#define F_SETLKW    7
#define F_SETOWN    8
#define F_GETOWN    9
#define F_SETSIG    10
#define F_GETSIG    11

#define FD_CLOEXEC  1

#define F_RDLCK     0
#define F_WRLCK     1
#define F_UNLCK     2

/* note from /usr/include/fcntl.h (glibc)
   The constants AT_REMOVEDIR and AT_EACCESS have the same value.  AT_EACCESS
   is meaningful only to faccessat, while AT_REMOVEDIR is meaningful only to
   unlinkat.  The two functions do completely different things and therefore,
   the flags can be allowed to overlap.  For example, passing AT_REMOVEDIR to
   faccessat would be undefined behavior and thus treating it equivalent to
   AT_EACCESS is valid undefined behavior.  */
#define AT_FDCWD            -100
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR        0x200
#define AT_SYMLINK_FOLLOW   0x400
#define AT_EACCESS          0x200
