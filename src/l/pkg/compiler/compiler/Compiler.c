#define REQUIRE_ALLOC
#define REQUIRE_STDIO
#define REQUIRE_STRING
#define REQUIRE_STR_EQ
#define REQUIRE_STR_COPY
#define REQUIRE_STR_COPY_FMT
#define REQUIRE_READFILE
#define REQUIRE_MAP

#include <libc.h>
#include <lex.h>
#include <compiler.h>

int main (int argc, char **argv) {
  int to_json = 0;
  int flags = 0;
  int to_lang = 0;

  flags |= COMPILER_DEBUG;

  int nargs = argc - 1;
  for (int i = 1; i < argc; i++) {
    if (str_eq (argv[i], "-h") || str_eq (argv[i], "--help")) {
      sys_fprintf(sys_stderr, "%s: [[--to-json], [--no-debug], [--to-lang]] file name for evaluation\n", argv[0]);
      return 1;
    }

    if (str_eq (argv[i], "--to-json")) {
      if (i == argc - 1) {
        sys_fprintf(sys_stderr, "%s: [[--to-json], [--no-debug], [--to-lang]] file name for evaluation\n", argv[0]);
        return 1;
      }

      nargs--;
      to_json = 1;
      continue;
    }

    if (str_eq (argv[i], "--no-debug")) {
      if (i == argc - 1) {
        sys_fprintf(sys_stderr, "%s: [[--to-json], [--no-debug], [--to-lang]] file name for evaluation\n", argv[0]);
        return 1;
      }

      nargs--;
      flags &= ~COMPILER_DEBUG;
      flags |= COMPILER_NO_DEBUG;
      continue;
    }

    if (str_eq (argv[i], "--to-lang")) {
      if (i == argc - 1) {
        sys_fprintf(sys_stderr, "%s: [[--to-json], [--no-debug], [--to-lang]] file name for evaluation\n", argv[0]);
        return 1;
      }

      nargs--;
      to_lang = 1;
    }
  }

  if (nargs == 0) {
    sys_fprintf(sys_stderr, "%s: [[--to-json], [--no-debug], [--to-lang]] file name for evaluation\n", argv[0]);
    return 1;
  }

  readfile_t rf = {.file = argv[argc - 1]};
  if (-1 == readfile_u (&rf))
    return 1;

  compiler_parse_string (rf.bytes, CompilerOpts (
    .flags = flags, .to_json = to_json, .to_lang = to_lang));
  return 0;
}
