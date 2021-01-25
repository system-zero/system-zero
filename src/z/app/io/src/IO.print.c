#define APPLICATION "IO.print"
#define APP_OPTS    "string"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE

#include <z/cenv.h>

static int IO_print (char *buf, int parse_escapes) {
  ifnot (parse_escapes)
    return IO.print (buf);

  string_t *out = IO.parse_escapes (buf);
  if (NULL is out)
    return NOTOK;

  idx_t nbytes = IO.print (out->bytes);
  String.release (out);
  return nbytes;
}

int main (int argc, char **argv) {
  __INIT__ (string);
  __INIT__ (cstring);

  __INIT_APP__;

  int parse_escapes = NULL is Cstring.bytes_in_str (argv[0], "raw");
  int println = Cstring.eq_n (argv[0], "IO.println", 10);

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  PARSE_ARGS;

  string_t *out = String.new (256);

  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    int maxlen = 4095;
    forever {
      char buf[maxlen + 1];
      idx_t nbytes = IO.fd.read (STDIN_FILENO, buf, maxlen);
      if (NOTOK is nbytes)
        goto theend;

      ifnot (nbytes) goto print;

      String.append_with_len (out, buf, nbytes);
     }
  }

  CHECK_ARGC;

  for (int i = 0; i < argc; i++) {
    if (i)
      String.append_byte (out, ' ');

    String.append_with (out, argv[i]);
  }

print:
  if (println) String.append_byte (out, '\n');
  retval = IO_print (out->bytes, parse_escapes);

theend:
  String.release (out);
  return (retval < 0 ? 1 : 0);
}
