
/* from sys/mount.h and linux/mount.h */

#define MS_RDONLY       (1 << 0)   /* Mount read-only */
#define MS_NOSUID       (1 << 1)   /* Ignore suid and sgid bits */
#define MS_NODEV        (1 << 2)   /* Disallow access to device special files */
#define MS_NOEXEC       (1 << 3)   /* Disallow program execution */
#define MS_SYNCHRONOUS  (1 << 4)   /* Writes are synced at once */
#define MS_REMOUNT      (1 << 5)   /* Alter flags of a mounted FS */
#define MS_MANDLOCK     (1 << 6)   /* Allow mandatory locks on an FS */
#define MS_DIRSYNC      (1 << 7)   /* Directory modifications are synchronous */
#define MS_NOSYMFOLLOW  (1 << 8)   /* Do not follow symlinks */
#define MS_NOATIME      (1 << 10)  /* Do not update access times. */
#define MS_NODIRATIME   (1 << 11)  /* Do not update directory access times */
#define MS_BIND         (1 << 12)
#define MS_MOVE         (1 << 13)
#define MS_REC          (1 << 14)

#define MS_SILENT       (1 << 15)
#define MS_POSIXACL     (1 << 16)  /* VFS does not apply the umask */
#define MS_UNBINDABLE   (1 << 17)  /* change to unbindable */
#define MS_PRIVATE      (1 << 18)  /* change to private */
#define MS_SLAVE        (1 << 19)  /* change to slave */
#define MS_SHARED       (1 << 20)  /* change to shared */
#define MS_RELATIME     (1 << 21)  /* Update atime relative to mtime/ctime. */
#define MS_KERNMOUNT    (1 << 22)  /* this is a kern_mount call */
#define MS_I_VERSION    (1 << 23)  /* Update inode I_version field */
#define MS_STRICTATIME  (1 << 24)  /* Always perform atime updates */
#define MS_LAZYTIME     (1 << 25)  /* Update the on-disk [acm]times lazily */
#define MS_ACTIVE       (1 << 30)
#define MS_NOUSER       (1 << 31)


#define MNT_FORCE       (1 << 0)   /* Force unmounting.  */
#define MNT_DETACH      (1 << 1)   /* Just detach from the tree.  */
#define MNT_EXPIRE      (1 << 2)   /* Mark for expiry.  */
#define UMOUNT_NOFOLLOW (1 << 3)   /* Don't follow symlink on umount.  */
