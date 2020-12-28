#include <zc.h>
#include <stdio.h>
#include <unistd.h>

#include <libstring.h>
#include <libvstring.h>
#include <libargparse.h>
#include <libio.h>
#include <libfile.h>

static  file_T FileT;
#define File   FileT.self

static  io_T IoT;
#define Io   IoT.self

static  argparse_T ArgparseT;
#define Argparse   ArgparseT.self

#ifndef APP_NAME
#define APP_NAME "File.size"
#endif

#define OPTS "filename"
static const char *const usage[] = {
  APP_NAME " " OPTS,
  NULL,
};

int main (int argc, char **argv) {
  IoT = __init_io__ ();
  FileT = __init_file__ ();
  ArgparseT = __init_argparse__ ();

  int retval = 0;
  int version = 0;
  size_t size = 0;

  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    char filename[MAXLEN_PATH];
    if (NOTOK is Io.fd.read (STDIN_FILENO, filename, MAXLEN_PATH))
      retval = 1;
    else
      size = File.size (filename);

    goto print;
  }

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  argparse_t argparser;
  Argparse.init (&argparser, options, usage, 0);
  argc = Argparse.exec (&argparser, argc, (const char **) argv);

  if (version) {
    fprintf (stderr, "version: %s\n", VERSION_STRING);
    return 1;
  }

  ifnot (argc) {
    Argparse.print_usage (&argparser);
    return 1;
  }

  size = File.size (argv[0]);

print:
  fprintf (stdout, "%zd\n", size);

  return retval;
}
