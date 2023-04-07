// provides: long sys_mount (const char *, const char *, const char *, ulong, const void *)
// provides: long sys_umount (const char *, int)
// requires: sys/mount.h

long sys_mount (const char *source, const char *target,
                const char *fstype, ulong flags, const void *data) {
  return syscall5 (NR_mount, (long) source, (long) target,
                             (long) fstype, flags, (long) data);
}

/* Force unmounting if FLAGS is set to MNT_FORCE.  */
long sys_umount (const char *target, int flags) {
  return syscall2 (NR_umount2, (long) target, flags);
}
