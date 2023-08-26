#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <xsel.h>

#ifndef Alloc
#define Alloc malloc
#endif

#ifndef Realloc
#define Realloc realloc
#endif

#define DO_PUT 1
#define DO_GET 2

int main (int argc, char **argv) {
  int action = DO_PUT;
  xsel_opts_t opts = XselOpts();

  for (int n = 1; n < argc; n++) {
    char *arg = argv[n];

    if (strcmp ("-i", arg) == 0 ||
        strcmp ("--input", arg) == 0) {
      action = DO_PUT;
      continue;
    }

    if (strcmp ("-o", arg) == 0 ||
        strcmp ("--output", arg) == 0) {
      action = DO_GET;
      continue;
    }

    int is_short_sel = (0 == strcmp ("-s", arg));
    int is_selection = (0 == strncmp ("--selection=", arg, 12));

    if (is_short_sel || is_selection) {
      if (is_short_sel) {
        if (n + 1 == argc) {
          fprintf (stderr, "%s: requires an option, either primary or clipboard\n", arg);
          exit (1);
        }

        arg = argv[++n];
      } else
        arg = strchr (arg, '=');

      if (strcmp (arg, "primary") == 0)
        opts.selection = XSEL_PRIMARY;
      else if (strcmp (arg, "clipboard") == 0)
        opts.selection = XSEL_CLIPBOARD;
      else {
        fprintf (stderr, "%s argunent accepts either primary or clipboard\n",
           (is_short_sel ? "-s" : "--selection="));
        exit (1);
      }

      continue;
    }

    if (0 == strcmp (arg, "--rm-last-nl")) {
      opts.rm_last_nl = 1;
      continue;
    }

    fprintf (stderr, "%s: unknown argunent\n", arg);
    exit (1);
  }

  if (action == DO_PUT) {

    if (isatty (STDIN_FILENO))
      exit (1);

    size_t len = 1024;
    char *buf = Alloc (len);
    size_t total = 0;
    int chunk = 512;

    while (1) {
      int nread = read (STDIN_FILENO, buf + total, chunk);
      if (0 >= nread)
        break;

      total += nread;

      if (total + chunk >= len) {
         len += chunk + 1;
         buf = Realloc (buf, len);
         if (NULL == buf)
           exit (1);
      }
    }

    if (0 == total)
      exit (1);

    buf[total] = '\0';

    xsel_put (buf, opts);
    exit (0);
  }

  char *xsel = xsel_get (opts);
  if (xsel)
    fprintf (stdout, xsel);

  exit (0);
}
