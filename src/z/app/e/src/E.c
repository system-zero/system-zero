#define APPLICATION "E"
#define APP_OPTS   "file"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SIGNAL
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT
#define REQUIRE_FCNTL
#define REQUIRE_TERMIOS

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_VSTRING_TYPE  DONOT_DECLARE
#define REQUIRE_DIR_TYPE      DONOT_DECLARE
#define REQUIRE_IMAP_TYPE     DONOT_DECLARE
#define REQUIRE_SMAP_TYPE     DONOT_DECLARE
#define REQUIRE_I_TYPE        DONOT_DECLARE
#define REQUIRE_PROC_TYPE     DONOT_DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_E_TYPE        DECLARE
#define REQUIRE_TERM_MACROS

#include <z/cenv.h>
#include <locale.h>

static E_T *__E__ = NULL;

#define E   __E__->self
#define Ed  __E__->__Ed__->self
#define Win __E__->__Ed__->__Win__.self
#define Buf __E__->__Ed__->__Buf__.self
#define Rline  __E__->__Ed__->__Rline__.self

public void sigwinch_handler (int sig) {
  signal (sig, sigwinch_handler);
  int cur_idx = E.get.current_idx (__E__);

  ed_t *ed = E.get.head (__E__);

  while (ed) {
    Ed.set.screen_size (ed, ScreenDimOpts());
    ifnot (OK is Ed.check_sanity (ed)) {
      __deinit_ed__ (&__E__);
      fprintf (stderr, "available lines are less than the required\n");
      exit (1);
    }

    win_t *w = Ed.get.win_head (ed);
    while (w) {
      Ed.readjust.win_size (ed, w);
      w = Ed.get.win_next (ed, w);
    }

    ifnot (E.get.next (__E__, ed))
      break;

    ed = E.set.next (__E__);
  }

  ed = E.set.current (__E__, cur_idx);
  win_t *w = Ed.get.current_win (ed);
#ifdef HAS_TEMPORARY_WORKAROUND
  buf_t * buf = Win.get.current_buf (w);
  Buf.draw (buf);
#else
  Win.draw (w);
#endif
}

public void sighup_handler (int sig) {
  (void) sig;
  ed_t *ed = E.get.current (__E__);
  buf_t *buf = Ed.get.current_buf (ed);

  E.set.state_bit (__E__, E_EXIT_ALL_FORCE);
  E.main (__E__, buf);

  __deinit_ed__ (&__E__);
  exit (0);
}

mutable public void __alloc_error_handler__ (int err, size_t size,
                           char *file, const char *func, int line) {
  fprintf (stderr, "MEMORY_ALLOCATION_ERROR\n");
  fprintf (stderr, "File: %s\nFunction: %s\nLine: %d\n", file, func, line);
  fprintf (stderr, "Size: %zd\n", size);

  if (err is INTEGEROVERFLOW_ERROR)
    fprintf (stderr, "Error: Integer Overflow Error\n");
  else
    fprintf (stderr, "Error: Not Enouch Memory\n");

  ifnot (NULL is __E__) __deinit_ed__ (&__E__);

  exit (1);
}

static string_t *parse_command (char *bytes) {
  string_t *com = String.new (256);
  char *sp = bytes;
  while (*sp) {
    if (*sp isnot ':')
      String.append_byte (com, *sp);
    else {
      if (*(sp+1) isnot ':')
        String.append_byte (com, *sp);
      else {
        String.append_byte (com, ' ');
        sp++;
      }
    }
    sp++;
  }

  return com;
}

int main (int argc, char **argv) {
  __INIT__ (term);
  __INIT__ (string);

  __INIT_APP__;

  int ifd = -1;

  ifnot (isatty (fileno (stdin))) {
    /* this looks sufficient but time will tell */
    ifd = dup (fileno (stdin));
    if (NULL is freopen ("/dev/tty", "r", stdin))
      return 1;
  }

  setlocale (LC_ALL, "");
  AllocErrorHandler = __alloc_error_handler__;

  if (NULL is (__E__ = __init_ed__ ("vedas")))
    return 1;

  char
    *load_file = NULL,
    *ftype = NULL,
    *backup_suffix = NULL,
    *exec_com = NULL;

  int
    exit = 0,
    exit_quick = 0,
    filetype = FTYPE_DEFAULT,
    autosave = 0,
    backupfile = 0,
    ispager = 0,
    linenr = 0,
    column = 1,
    num_win = 1;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_STRING(0, "ftype", &ftype, "set the file type", NULL, 0, 0),
    OPT_STRING(0, "backup-suffix", &backup_suffix, "backup suffix (default: ~)", NULL, 0, 0),
    OPT_STRING(0, "exec-com", &exec_com, "execute command", NULL, 0, 0),
    OPT_STRING(0, "load-file", &load_file, "eval file", NULL, 0, 0),
    OPT_INTEGER('+', "line-nr", &linenr, "start at line number", NULL, 0, SHORT_OPT_HAS_NO_DASH),
    OPT_INTEGER(0, "column", &column, "set pointer at column", NULL, 0, 0),
    OPT_INTEGER(0, "num-win", &num_win, "create new [num] windows", NULL, 0, 0),
    OPT_INTEGER(0, "autosave", &autosave, "interval time in minutes to autosave buffer", NULL, 0, 0),
    OPT_BOOLEAN(0, "backupfile", &backupfile, "backup file at the initial reading", NULL, 0, 0),
    OPT_BOOLEAN(0, "pager", &ispager, "behave as a pager", NULL, 0, 0),
    OPT_BOOLEAN(0, "exit", &exit, "exit", NULL, 0, 0),
    OPT_BOOLEAN(0, "exit-quick", &exit_quick, "exit when quiting current buffer", NULL, 0, 0),
    OPT_END()
  };

  PARSE_ARGS;

  if (argc is -1) goto theend;

  int term_flags = 0;

   /* minimal cooperation with libvwn */
  char *vwm_env_exists = getenv ("VWM");
  ifnot (NULL is vwm_env_exists) {
    term_t *term = E.get.term (__E__);
    term_flags = (TERM_DONOT_SAVE_SCREEN|TERM_DONOT_CLEAR_SCREEN|TERM_DONOT_RESTORE_SCREEN);
    Term.set_state_bit (term, term_flags);
  }

  ed_t *this = NULL;
  win_t *w = NULL;

  if (load_file isnot NULL and getuid ()) {
    retval = E.load_file (__E__, load_file);
    ifnot (OK is retval) {
      retval = 1;
      goto theend;
    }

    if (exit) goto theend;

    signal (SIGWINCH, sigwinch_handler);
    signal (SIGHUP, sighup_handler);

    this = E.get.current (__E__);
    ifnot (OK is Ed.check_sanity (this)) {
      retval = 1;
      goto theend;
    }

    if (exit_quick)
      Ed.set.exit_quick (this, 1);

    w = Ed.get.current_win (this);
    goto theloop;
  }

  num_win = (num_win < argc ? num_win : argc);

//      .init_cb = __init_ext__,
  this = E.new (__E__, EdOpts(
      .num_win = num_win,
      .term_flags = term_flags));

  if (NOTOK is Ed.check_sanity (this)) {
    retval = 1;
    goto theend;
  }

  filetype = Ed.syn.get_ftype_idx (this, ftype);

  w = Ed.get.current_win (this);

  if (0 is argc or ifd isnot -1) {
    /* just create a new empty buffer and append it to its
     * parent win_t to the frame zero */
    buf_t *buf = Win.buf.new (w, BufOpts (
        .ftype = filetype,
        .autosave = autosave,
        .flags = (ispager ? BUF_IS_PAGER : 0)));

    Win.append_buf (w, buf);

    /* check if input comes from stdin */
    if (ifd isnot -1) {
      FILE *fpin = fdopen (ifd, "r");
      fp_t fp = (fp_t) {.fp = fpin};
      Buf.read.from_fp (buf, NULL, &fp);
    }

  } else {
    int
      first_idx = Ed.get.current_win_idx (this),
      widx = first_idx,
      l_num_win = num_win;

    /* else create a new buffer for every file in the argvlist */
    for (int i = 0; i < argc; i++) {
      buf_t *buf = Win.buf.new (w, BufOpts(
          .fname = argv[i],
          .ftype = filetype,
          .at_frame = FIRST_FRAME,
          .at_linenr = linenr,
          .at_column = column,
          .backupfile = backupfile,
          .backup_suffix = backup_suffix,
          .autosave = autosave,
          .flags = (ispager ? BUF_IS_PAGER : 0)));

      Win.append_buf (w, buf);

      if (--l_num_win > 0)
        w = Ed.set.current_win (this, ++widx);
    }

    w = Ed.set.current_win (this, first_idx);
  }

  /* set the first indexed name in the argvlist, as current */
  Win.set.current_buf (w, 0, DRAW);

  signal (SIGWINCH, sigwinch_handler);
  signal (SIGHUP, sighup_handler);

  if (exec_com isnot NULL) {
    string_t *com = parse_command (exec_com);
    rline_t *rl = Ed.rline.new_with (this, com->bytes);
    buf_t *buf = Ed.get.current_buf (this);
    retval = Rline.exec (rl, &buf);
    String.release (com);
  }

  if (exit) {
    if (retval <= NOTOK) retval = 1;
    goto theend;
  }

  if (exit_quick)
    Ed.set.exit_quick (this, 1);

theloop:;
  for (;;) {
    buf_t *buf = Ed.get.current_buf (this);

    retval = E.main (__E__, buf);

    if (E.test.state_bit (__E__, E_EXIT))
      break;

    if (E.test.state_bit (__E__, E_SUSPENDED)) {
      if (E.get.num (__E__) is 1) {
        /* as an example, we simply create another independed instance */
        //this = E.new (__E__, EdOpts(.init_cb = __init_ext__));
        this = E.new (__E__, EdOpts());

        w = Ed.get.current_win (this);
        buf = Win.buf.new (w, BufOpts());
        Win.append_buf (w, buf);
        Win.set.current_buf (w, 0, DRAW);
      } else {
        /* else jump to the next or prev */
        int prev_idx = E.get.prev_idx (__E__);
        this = E.set.current (__E__, prev_idx);
        w = Ed.get.current_win (this);
      }

      continue;
    }

    break;
  }

theend:
  __deinit_ed__ (&__E__);
  return retval;
}
