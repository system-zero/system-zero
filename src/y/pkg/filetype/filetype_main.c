#include <stddef.h>
#include <z/filetype.h>
#include <stdio.h>

int main (int argc, char **argv) {
  if (argc == 1) return 1;

  char ft[MAXLEN_FILETYPE];
  filetype (argv[1], ft);
  fprintf (stdout, "%s: %s\n", argv[1], ft);
  return 0;
}
