typedef uint16_t    gid_t;
typedef uint16_t    mode_t;
typedef uint16_t    nlink_t;
typedef uint16_t    uid_t;

typedef int32_t     pid_t;

typedef uint32_t    uid32_t;
typedef uint32_t    gid32_t;
typedef uint32_t    dev_t;

#if __WORDSIZE == 64
  typedef int64_t  time_t;
#else
  typedef long int  time_t;
#endif

typedef long        fpos_t;
typedef long        useconds_t;

typedef signed long off_t;
typedef signed long ino_t;

typedef int64_t off64_t;
typedef uint64_t ino64_t;
