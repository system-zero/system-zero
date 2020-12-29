#include <zc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include <libstring.h>
#include <libvstring.h>
#include <libargparse.h>
#include <libio.h>
#include <libfile.h>
#include <libdir.h>

static  io_T IoT;
#define Io   IoT.self

static  dir_T DirT;
#define Dir   DirT.self

static  file_T FileT;
#define File   FileT.self

static  argparse_T ArgparseT;
#define Argparse   ArgparseT.self

#ifndef APP_NAME
#define APP_NAME "Dir.make"
#endif

#define OPTS "directory-name"
static const char *const usage[] = {
  APP_NAME " " OPTS,
  NULL,
};

static void dir_make_print (char *dname,  int retval, int verbose) {
  ifnot (verbose) return;
  if (retval isnot OK) return;

  fprintf (stdout, "%s: created directory '%s'\n", APP_NAME, dname);
}

int main (int argc, char **argv) {
  IoT = __init_io__ ();
  DirT = __init_dir__ ();
  FileT = __init_file__ ();
  ArgparseT = __init_argparse__ ();

  int retval = 0;
  int version = 0;
  int verbose = 0;
  char *mode_string = NULL;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_STRING('m', "mode", &mode_string, "set file mode in octal", NULL, 0, 0),
    OPT_BOOLEAN('v', "verbose", &verbose, "print a message when creating a directory", NULL, 0, 0),
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

  mode_t mode = 0777 & ~umask (0);
  mode_t m = File.mode.from_octal_string (mode_string);

  ifnot (m) {
    fprintf (stderr, APP_NAME ": not a valid mode %s\n", mode_string);
    return 1;
  }

  mode = (mode & 0) | m;

  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    char dname[MAXLEN_PATH];
    if (NOTOK is Io.fd.read (STDIN_FILENO, dname, MAXLEN_PATH))
      retval = 1;
    else
      retval = Dir.make (dname, mode);

    dir_make_print (dname, retval, verbose);

    goto theend;
  }

  ifnot (argc) {
    Argparse.print_usage (&argparser);
    return 1;
  }

  retval = Dir.make (argv[0], mode);
  dir_make_print (argv[0], retval, verbose);

theend:
  return (retval < 0 ? 1 : 0);
}
