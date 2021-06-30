#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ERROR_C_FILE "liberror.c"
#define ERROR_H_FILE "error.h"

#define __BASE_ERROR__        -10000
#define EINDEX                (__BASE_ERROR__ - 1)
#define EINTEGEROVERFLOW      (__BASE_ERROR__ - 2)
#define ECANNOTGETCWD         (__BASE_ERROR__ - 3)

#define __LAST_ERROR__        (ECANNOTGETCWD - 1)

#define EINDEX_STR            "Index is out of range"
#define EINDEX_NAME           "EINDEX"
#define EINTEGEROVERFLOW_STR  "Integer overflow"
#define EINTEGEROVERFLOW_NAME "EINTEGEROVERFLOW"
#define ECANNOTGETCWD_STR     "Can not get current directory"
#define ECANNOTGETCWD_NAME    "ECANNOTGETCWD"
#define EUNKNOWN_NAME         "EUNKNOWN"
#define EUNKNOWN_STR          "Unknown error"

#if defined (__linux__) || defined (__unix__)
int main (void) {
  char cmd[] =
    "printf \"#include <errno.h>\" | cpp -dD | sed -n '/#define E/p' | sed "
    "'s/^\\(.*\\)\\s\\(.*\\)\\s\\(.*\\)$/#ifndef \\2\\n  \\1 \\2 \\3\\n#endif\\n/'";

  FILE *fp_source = popen (cmd, "r");
  if (NULL == fp_source) {
    fprintf (stderr, "%s\n", strerror (errno));
    return 1;
  }

  FILE *fp_dest = fopen (ERROR_C_FILE, "w");
  if (NULL == fp_dest) {
    fprintf (stderr, "%s\n", strerror (errno));
    return 1;
  }

  fprintf (fp_dest,
      "#define REQUIRE_ERROR_TYPE DONOT_DECLARE\n\n"
      "#include <z/cenv.h>\n\n"
      "#ifndef __BASE_ERROR__\n"
      "#define __BASE_ERROR__   %d\n"
      "#endif\n\n"
      "#define __LAST_ERROR__   %d\n\n"
      "static const struct sys_error_list_type {\n"
      "  const char *name;\n"
      "  const char *msg;\n"
      "  int errnum;\n"
    "} sys_error_list[] = {\n"
    "  { \"SUCCESS\", \"%s\", 0},\n",
    __BASE_ERROR__,
    __LAST_ERROR__,
    strerror (0));

  char name[128];
  char *buf = NULL;
  size_t len;
  ssize_t nread;
  int num_items;
  int err;
  int prev_err = 0;
  int num_err = 0;

  while (-1 != (nread = getline (&buf, &len, fp_source))) {
    if (nread < 2) continue;
    if (0 == strncmp (buf, "#ifndef", 7)) continue;
    if (0 == strncmp (buf, "#endif", 6)) continue;
    num_items = sscanf (buf, "  #define %s %d\n", name, &err);
    if (2 != num_items) {    // this is not going to work anyway
#if 0                        // so just document it
      if ((sp = strstr (buf, "ENOTSUP")) != NULL)
        fprintf (fp_dest, "  { \"ENOTSUP\", \"%s\", %d},\n", strerror(ENOTSUP), ENOTSUP);
#endif
      continue;
    }

    if (prev_err != err - 1) {
      fprintf (fp_dest, "  { \"%s\", \"%s\", %d},\n", EUNKNOWN_NAME, EUNKNOWN_STR, err - 1);
      num_err++;
    }

    prev_err = err;
    num_err++;

    fprintf (fp_dest, "  { \"%s\", \"%s\", %d},\n", name, strerror(err), err);
  }

  num_err++;
  fprintf (fp_dest, "  { \"%s\", \"%s\", %d},\n", EUNKNOWN_NAME, EUNKNOWN_STR, num_err + 1);
  fprintf (fp_dest, "  { \"%s\", \"%s\", %d},\n", EINDEX_NAME, EINDEX_STR, EINDEX);
  fprintf (fp_dest, "  { \"%s\", \"%s\", %d},\n", EINTEGEROVERFLOW_NAME, EINTEGEROVERFLOW_STR, EINTEGEROVERFLOW);
  fprintf (fp_dest, "  { \"%s\", \"%s\", %d},\n", ECANNOTGETCWD_NAME, ECANNOTGETCWD_STR, ECANNOTGETCWD);
  fprintf (fp_dest, "};\n\n");

  fprintf (fp_dest,
    "static int sys_last_error = %d;\n\n", num_err);

  fprintf (fp_dest,
    "static int get_error_num (int errnum) {\n"
     "  if (__BASE_ERROR__ > errnum && errnum > __LAST_ERROR__)\n"
     "    return (sys_last_error + (__BASE_ERROR__ - errnum));\n\n"
     "  if (errnum > sys_last_error || errnum < 0)\n"
     "    return EUNKNOWN;\n\n"
     "  return errnum;\n"
     "}\n\n"
    "static char *errno_name (int errnum) {\n"
     "  errnum = get_error_num (errnum);\n"
     "  return (char *) sys_error_list[errnum].name;\n"
     "}\n\n"
    "static char *errno_name_s (int errnum, char *buf, size_t buflen) {\n"
     "  char *name = errno_name (errnum);\n\n"
     "  size_t len = bytelen (name);\n\n"
     "  size_t idx = 0;\n"
     "  for (;idx < len && idx < buflen - 1; idx++) {\n"
     "    buf[idx] = name[idx];\n"
     "  }\n\n"
     "  buf[idx] = '\\0';\n\n"
     "  return buf;\n"
     "}\n\n"
    "static char *errno_string (int errnum) {\n"
     "  errnum = get_error_num (errnum);\n"
     "  return (char *) sys_error_list[errnum].msg;\n"
     "}\n\n"
    "static char *errno_string_s (int errnum, char *buf, size_t buflen) {\n"
     "  char *msg = errno_string (errnum);\n\n"
     "  size_t len = bytelen (msg);\n\n"
     "  size_t idx = 0;\n"
     "  for (;idx < len && idx < buflen - 1; idx++) {\n"
     "    buf[idx] = msg[idx];\n"
     "  }\n\n"
     "  buf[idx] = '\\0';\n\n"
     "  return buf;\n"
     "}\n\n"
     "static int error_exists (int errnum) {\n"
     "  errnum = get_error_num (errnum);\n"
     "  return (errnum != EUNKNOWN);\n"
     "}\n\n");

  fprintf (fp_dest,
    "public error_T __init_error__ (void) {\n"
    "  return (error_T) {\n"
    "    .self = (error_self) {\n"
    "      .errno_string = errno_string,\n"
    "      .errno_string_s = errno_string_s,\n"
    "      .errno_name = errno_name,\n"
    "      .errno_name_s = errno_name_s,\n"
    "      .exists = error_exists\n"
    "    }\n"
    "  };\n"
    "}\n");

  fclose (fp_source);
  fclose (fp_dest);

  fp_dest = fopen (ERROR_H_FILE, "w");
  if (NULL == fp_dest) {
    fprintf (stderr, "%s\n", strerror (errno));
    return 1;
  }

  fprintf (fp_dest,
    "#ifndef ERROR_HDR\n"
    "#define ERROR_HDR\n\n"
    "#define EUNKNOWN %d\n\n"
    "typedef struct error_self {\n"
    "  char\n"
    "    *(*errno_name) (int),\n"
    "    *(*errno_name_s) (int, char *, size_t),\n"
    "    *(*errno_string) (int),\n"
    "    *(*errno_string_s) (int, char *, size_t);\n\n"
    "  int (*exists) (int);\n\n"
    "} error_self;\n\n"
    "typedef struct error_T {\n"
    "  error_self self;\n"
    "} error_T;\n\n"
    "public error_T __init_error__ (void);\n\n"
    "#endif /* ERROR_HDR */\n",
    num_err);

  fclose (fp_dest);

  return 0;
}
#else
int main (void) {
  fprintf (stderr, "this functionality has been implemented only for Linux platform\n");
  return 1;
}
#endif
