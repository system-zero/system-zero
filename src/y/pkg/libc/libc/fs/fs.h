/* only what is really needed for now (from <linux/fs.h>)
 * we may need to adjust for all archs */

#ifndef _IOC_READ
#define _IOC_READ       2U
#endif

#define _IOC_NRBITS     8
#define _IOC_TYPEBITS   8

#ifndef _IOC_SIZEBITS
#define _IOC_SIZEBITS   14
#endif

#define _IOC_NRSHIFT    0
#define _IOC_TYPESHIFT  (_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT  (_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT   (_IOC_SIZESHIFT+_IOC_SIZEBITS)

#define _IOC(dir,type,nr,size) \
  (((dir)  << _IOC_DIRSHIFT) | \
   ((type) << _IOC_TYPESHIFT) | \
   ((nr)   << _IOC_NRSHIFT) | \
   ((size) << _IOC_SIZESHIFT))

#define _IOC_TYPECHECK(t) (sizeof(t))

#define _IOR(type,nr,size)  _IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))

#define BLKGETSIZE64 _IOR(0x12,114,size_t)  /* return device size in bytes (u64 *arg) */

/* sys_rename* flags */
#define RENAME_NOREPLACE  (1 << 0)  /* Don't overwrite target */
#define RENAME_EXCHANGE   (1 << 1)  /* Exchange source and dest */
#define RENAME_WHITEOUT   (1 << 2)  /* Whiteout source */
