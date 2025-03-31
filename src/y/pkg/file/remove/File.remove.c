#define REQUIRE_CRT
#define REQUIRE_MEM_SET
#define REQUIRE_FILE_REMOVE
#define REQUIRE_FORMAT
#define REQUIRE_STR_EQ

#include <libc.h>

#define NAME "__File.remove"
#define __USAGE "Usage: " NAME " [option]... FILE[s]\n" \
   "Remove FILE(s)\n\n" \
   "  --force       never prompt\n" \
   "  --interactive prompt before removing\n" \
   "  --recursive   remove directories and files recursively\n"\
   "  --verbose     be verbose\n" \
   "  -h,--help     show this message"

int main (int argc, char **argv) {
  int __argc = 0;
  char **__argv = argv;

  file_remove_opts opts = FileRemoveOpts();

  for (int i  = 1; i < argc; i++) {
    char *arg = argv[i];

    if (true == str_eq (arg, "--interactive")) {
      opts.interactive = 1;
      continue;
    }

    if (true == str_eq (arg, "--recursive")) {
      opts.recursive = 1;
      continue;
    }

    if (true == str_eq (arg, "--verbose")) {
      opts.verbose = VERBOSE_ON;
      continue;
    }

    if (true == str_eq (arg, "--force")) {
      opts.force = FORCE_ON;
      continue;
    }

    if (true == str_eq (arg, "--help") || true == str_eq (arg, "-h")) {
      tostderr ("%s\n", __USAGE);
      return 0;
    }

    if (str_eq (arg, "--")) {
      for (int n = i; n < argc; n++)
        __argv[__argc++] = argv[n];
      break;
    }

    if ('-' == *arg) {
      tostderr ("unknown argument %s\n", arg);
      return 1;
    }

    __argv[__argc++] = arg;
  }

  __argv[__argc] = NULL;

  if (0 == __argc) {
    tostderr ("%s\n", __USAGE);
    return 1;
  }

  for (int i = 0; i < __argc; i++) {
    int retval = file_remove (__argv[i], opts);

    if (-1 == retval)
      return 1;
  }

  return 0;
}
