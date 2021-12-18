//#define APPLICATION "v"

#define REQUIRE_STDIO
#define REQUIRE_TERMIOS

#define REQUIRE_STRING_TYPE DONOT_DECLARE
#define REQUIRE_TERM_TYPE   DONOT_DECLARE
#define REQUIRE_V_TYPE      DECLARE

#include <z/cenv.h>

int main (int argc, const char **argv) {
  v_opts opts = VOpts(
    .argc = argc,
    .argv = argv
  );

  v_t *v = __init_v__ (&opts);

  if (NULL is v)
    exit (1);

  __V__ = v;

  int retval = V.main (v);

  __deinit_v__ (&v);

  exit (retval);
}
