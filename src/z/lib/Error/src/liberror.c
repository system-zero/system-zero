#define REQUIRE_ERROR_TYPE DONOT_DECLARE

#include <z/cenv.h>

#ifndef __BASE_ERROR__
#define __BASE_ERROR__   -10000
#endif

#define __LAST_ERROR__   -10005

static const struct sys_error_list_type {
  const char *name;
  const char *msg;
  int errnum;
} sys_error_list[] = {
  { "SUCCESS", "Success", 0},
  { "EPERM", "Operation not permitted", 1},
  { "ENOENT", "No such file or directory", 2},
  { "ESRCH", "No such process", 3},
  { "EINTR", "Interrupted system call", 4},
  { "EIO", "Input/output error", 5},
  { "ENXIO", "No such device or address", 6},
  { "E2BIG", "Argument list too long", 7},
  { "ENOEXEC", "Exec format error", 8},
  { "EBADF", "Bad file descriptor", 9},
  { "ECHILD", "No child processes", 10},
  { "EAGAIN", "Resource temporarily unavailable", 11},
  { "ENOMEM", "Cannot allocate memory", 12},
  { "EACCES", "Permission denied", 13},
  { "EFAULT", "Bad address", 14},
  { "ENOTBLK", "Block device required", 15},
  { "EBUSY", "Device or resource busy", 16},
  { "EEXIST", "File exists", 17},
  { "EXDEV", "Invalid cross-device link", 18},
  { "ENODEV", "No such device", 19},
  { "ENOTDIR", "Not a directory", 20},
  { "EISDIR", "Is a directory", 21},
  { "EINVAL", "Invalid argument", 22},
  { "ENFILE", "Too many open files in system", 23},
  { "EMFILE", "Too many open files", 24},
  { "ENOTTY", "Inappropriate ioctl for device", 25},
  { "ETXTBSY", "Text file busy", 26},
  { "EFBIG", "File too large", 27},
  { "ENOSPC", "No space left on device", 28},
  { "ESPIPE", "Illegal seek", 29},
  { "EROFS", "Read-only file system", 30},
  { "EMLINK", "Too many links", 31},
  { "EPIPE", "Broken pipe", 32},
  { "EDOM", "Numerical argument out of domain", 33},
  { "ERANGE", "Numerical result out of range", 34},
  { "EDEADLK", "Resource deadlock avoided", 35},
  { "ENAMETOOLONG", "File name too long", 36},
  { "ENOLCK", "No locks available", 37},
  { "ENOSYS", "Function not implemented", 38},
  { "ENOTEMPTY", "Directory not empty", 39},
  { "ELOOP", "Too many levels of symbolic links", 40},
  { "EUNKNOWN", "Unknown error", 41},
  { "ENOMSG", "No message of desired type", 42},
  { "EIDRM", "Identifier removed", 43},
  { "ECHRNG", "Channel number out of range", 44},
  { "EL2NSYNC", "Level 2 not synchronized", 45},
  { "EL3HLT", "Level 3 halted", 46},
  { "EL3RST", "Level 3 reset", 47},
  { "ELNRNG", "Link number out of range", 48},
  { "EUNATCH", "Protocol driver not attached", 49},
  { "ENOCSI", "No CSI structure available", 50},
  { "EL2HLT", "Level 2 halted", 51},
  { "EBADE", "Invalid exchange", 52},
  { "EBADR", "Invalid request descriptor", 53},
  { "EXFULL", "Exchange full", 54},
  { "ENOANO", "No anode", 55},
  { "EBADRQC", "Invalid request code", 56},
  { "EBADSLT", "Invalid slot", 57},
  { "EUNKNOWN", "Unknown error", 58},
  { "EBFONT", "Bad font file format", 59},
  { "ENOSTR", "Device not a stream", 60},
  { "ENODATA", "No data available", 61},
  { "ETIME", "Timer expired", 62},
  { "ENOSR", "Out of streams resources", 63},
  { "ENONET", "Machine is not on the network", 64},
  { "ENOPKG", "Package not installed", 65},
  { "EREMOTE", "Object is remote", 66},
  { "ENOLINK", "Link has been severed", 67},
  { "EADV", "Advertise error", 68},
  { "ESRMNT", "Srmount error", 69},
  { "ECOMM", "Communication error on send", 70},
  { "EPROTO", "Protocol error", 71},
  { "EMULTIHOP", "Multihop attempted", 72},
  { "EDOTDOT", "RFS specific error", 73},
  { "EBADMSG", "Bad message", 74},
  { "EOVERFLOW", "Value too large for defined data type", 75},
  { "ENOTUNIQ", "Name not unique on network", 76},
  { "EBADFD", "File descriptor in bad state", 77},
  { "EREMCHG", "Remote address changed", 78},
  { "ELIBACC", "Can not access a needed shared library", 79},
  { "ELIBBAD", "Accessing a corrupted shared library", 80},
  { "ELIBSCN", ".lib section in a.out corrupted", 81},
  { "ELIBMAX", "Attempting to link in too many shared libraries", 82},
  { "ELIBEXEC", "Cannot exec a shared library directly", 83},
  { "EILSEQ", "Invalid or incomplete multibyte or wide character", 84},
  { "ERESTART", "Interrupted system call should be restarted", 85},
  { "ESTRPIPE", "Streams pipe error", 86},
  { "EUSERS", "Too many users", 87},
  { "ENOTSOCK", "Socket operation on non-socket", 88},
  { "EDESTADDRREQ", "Destination address required", 89},
  { "EMSGSIZE", "Message too long", 90},
  { "EPROTOTYPE", "Protocol wrong type for socket", 91},
  { "ENOPROTOOPT", "Protocol not available", 92},
  { "EPROTONOSUPPORT", "Protocol not supported", 93},
  { "ESOCKTNOSUPPORT", "Socket type not supported", 94},
  { "EOPNOTSUPP", "Operation not supported", 95},
  { "EPFNOSUPPORT", "Protocol family not supported", 96},
  { "EAFNOSUPPORT", "Address family not supported by protocol", 97},
  { "EADDRINUSE", "Address already in use", 98},
  { "EADDRNOTAVAIL", "Cannot assign requested address", 99},
  { "ENETDOWN", "Network is down", 100},
  { "ENETUNREACH", "Network is unreachable", 101},
  { "ENETRESET", "Network dropped connection on reset", 102},
  { "ECONNABORTED", "Software caused connection abort", 103},
  { "ECONNRESET", "Connection reset by peer", 104},
  { "ENOBUFS", "No buffer space available", 105},
  { "EISCONN", "Transport endpoint is already connected", 106},
  { "ENOTCONN", "Transport endpoint is not connected", 107},
  { "ESHUTDOWN", "Cannot send after transport endpoint shutdown", 108},
  { "ETOOMANYREFS", "Too many references: cannot splice", 109},
  { "ETIMEDOUT", "Connection timed out", 110},
  { "ECONNREFUSED", "Connection refused", 111},
  { "EHOSTDOWN", "Host is down", 112},
  { "EHOSTUNREACH", "No route to host", 113},
  { "EALREADY", "Operation already in progress", 114},
  { "EINPROGRESS", "Operation now in progress", 115},
  { "ESTALE", "Stale file handle", 116},
  { "EUCLEAN", "Structure needs cleaning", 117},
  { "ENOTNAM", "Not a XENIX named type file", 118},
  { "ENAVAIL", "No XENIX semaphores available", 119},
  { "EISNAM", "Is a named type file", 120},
  { "EREMOTEIO", "Remote I/O error", 121},
  { "EDQUOT", "Disk quota exceeded", 122},
  { "ENOMEDIUM", "No medium found", 123},
  { "EMEDIUMTYPE", "Wrong medium type", 124},
  { "ECANCELED", "Operation canceled", 125},
  { "ENOKEY", "Required key not available", 126},
  { "EKEYEXPIRED", "Key has expired", 127},
  { "EKEYREVOKED", "Key has been revoked", 128},
  { "EKEYREJECTED", "Key was rejected by service", 129},
  { "EOWNERDEAD", "Owner died", 130},
  { "ENOTRECOVERABLE", "State not recoverable", 131},
  { "ERFKILL", "Operation not possible due to RF-kill", 132},
  { "EHWPOISON", "Memory page has hardware error", 133},
  { "EUNKNOWN", "Unknown error", 135},
  { "EINDEX", "Index is out of range", -10001},
  { "EINTEGEROVERFLOW", "Integer overflow", -10002},
  { "ENOTENOUGHSPACE", "Not enough space", -10003},
  { "ECANNOTGETCWD", "Can not get current directory", -10004},
};

static int sys_last_error = 134;

static int get_error_num (int errnum) {
  if (__BASE_ERROR__ > errnum && errnum > __LAST_ERROR__)
    return (sys_last_error + (__BASE_ERROR__ - errnum));

  if (errnum > sys_last_error || errnum < 0)
    return EUNKNOWN;

  return errnum;
}

static char *errno_name (int errnum) {
  errnum = get_error_num (errnum);
  return (char *) sys_error_list[errnum].name;
}

static char *errno_name_s (int errnum, char *buf, size_t buflen) {
  char *name = errno_name (errnum);

  size_t len = bytelen (name);

  size_t idx = 0;
  for (;idx < len && idx < buflen - 1; idx++) {
    buf[idx] = name[idx];
  }

  buf[idx] = '\0';

  return buf;
}

static char *errno_string (int errnum) {
  errnum = get_error_num (errnum);
  return (char *) sys_error_list[errnum].msg;
}

static char *errno_string_s (int errnum, char *buf, size_t buflen) {
  char *msg = errno_string (errnum);

  size_t len = bytelen (msg);

  size_t idx = 0;
  for (;idx < len && idx < buflen - 1; idx++) {
    buf[idx] = msg[idx];
  }

  buf[idx] = '\0';

  return buf;
}

static int error_exists (int errnum) {
  errnum = get_error_num (errnum);
  return (errnum != EUNKNOWN);
}

public error_T __init_error__ (void) {
  return (error_T) {
    .self = (error_self) {
      .errno_string = errno_string,
      .errno_string_s = errno_string_s,
      .errno_name = errno_name,
      .errno_name_s = errno_name_s,
      .exists = error_exists
    }
  };
}
