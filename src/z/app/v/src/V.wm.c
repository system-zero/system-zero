#define APPLICATION "V.wm"
#define APP_OPTS "command"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_TYPES
#define REQUIRE_TERMIOS

#define REQUIRE_STRING_TYPE   DONOT_DECLARE
#define REQUIRE_VSTRING_TYPE  DONOT_DECLARE
#define REQUIRE_VUI_TYPE      DONOT_DECLARE
#define REQUIRE_VIDEO_TYPE    DONOT_DECLARE
#define REQUIRE_READLINE_TYPE DONOT_DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_VWM_TYPE      DECLARE

#include <z/cenv.h>

static vwm_t *init () {
  __VWM__ = __init_vwm__ ();
  return __VWM__;
}

int main (int argc, char **argv) {
  __INIT__ (term);
  __INIT_APP__;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_BOOLEAN(0, "version", &version, "show version", NULL, 0, 0),
    OPT_END()
  };

  argparse_flags |= ARGPARSE_DONOT_EXIT_ON_UNKNOWN;

  PARSE_ARGS;

  char *vargv[MAX_ARGS]; memset (vargv, 0, MAX_ARGS * sizeof (char *));
  int vargc = 1;

  if (argc) {
    int i = 0;
    for (; i < argc; i++) {
      vargv[i] = argv[i];
    }
    vargv[i] = NULL;
    vargc = argc;
  } else {
    vargv[0] = SHELL;
    vargv[1] = NULL;
  }

  vwm_t *this = init ();

  term_t *term =  Vwm.get.term (this);

  Term.raw_mode (term);

  int rows, cols;
  Term.init_size (term, &rows, &cols);

  Vwm.set.size (this, rows, cols, 1);

  vwm_win *win = Vwm.new.win (this, NULL, WinOpts (
    .num_rows =rows,
    .num_cols = cols,
    .num_frames = 1,
    .max_frames = 3));

  vwm_frame *frame = Vwin.get.frame_at (win, 0);
  Vframe.set.argv (frame, vargc, vargv);
  Vframe.set.log (frame, NULL, 1);

  Term.screen.save (term);
  Term.screen.clear (term);

  retval = Vwm.main (this);

  Term.screen.restore (term);

  __deinit_vwm__ (&this);

  return retval;
}
