#define APPLICATION "Sys.chroot"
#define APP_OPTS    "directory command"

#define STD_POSIX_C_SOURCE_HDR
#define STD_XOPEN_SOURCE_HDR

#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_SYS_TYPE     DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_VSTRING_TYPE DONOT_DECLARE

#include <z/cenv.h>

static int sys_exec (char **argv) {
  errno = 0;

  execvp (argv[0], argv);

  if (errno) {
    Stderr.print_fmt ("%s: %s\n", argv[0], Error.errno_string (errno));
    return -1;
  }

  return 0;
}

static int sys_chroot (char *dir) {
  if (-1 is chroot (dir)) {
    Stderr.print_fmt ("%s: %s\n", dir, Error.errno_string (errno));
    return 1;
  }

  if (-1 is chdir ("/")) {
    Stderr.print_fmt ("%s\n", Error.errno_string (errno));
    return 1;
  }

  return 0;
}

int main (int argc, char **argv) {
  __INIT__ (sys);
  __INIT__ (file);
  __INIT__ (error);
  __INIT__ (string);

  Sys.init_environment (SysEnvOpts());

  __INIT_APP__;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  PARSE_ARGS;

  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    string_t *input = String.new (64);
    int maxlen = 4095;

    forever {
      char buf[maxlen + 1];
      idx_t nbytes = IO.fd.read (STDIN_FILENO, buf, maxlen);
      if (NOTOK is nbytes) {
        String.release (input);
        return 1;
      }

      ifnot (nbytes) break;

      String.append_with_len (input, buf, nbytes);
     }

     retval = sys_chroot (input->bytes);

     String.release (input);

     if (retval isnot OK)
       return 1;

     goto execute_command;
  }

  CHECK_ARGC;

  retval = sys_chroot (argv[0]);

  argv++;

  if (retval isnot OK)
    return 1;

execute_command:
  if (NULL is argv[0]) {
    string_t *sh = Sys.which ("sh", NULL);
    if (NULL is sh)
      return ENOENT;

    argv[0] = sh->bytes;
    retval = sys_exec (argv);
    String.release (sh);
    return retval;
  }

  return sys_exec (argv);
}
