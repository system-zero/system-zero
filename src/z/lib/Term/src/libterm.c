#define LIBRARY "Term"

#define REQUIRE_STD_DEFAULT_SOURCE

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_TERMIOS
#define REQUIRE_SYS_IOCTL
#define REQUIRE_STRING_TYPE DECLARE
#define REQUIRE_IO_TYPE     DECLARE
#define REQUIRE_TERM_TYPE   DONOT_DECLARE
#define REQUIRE_TERM_MACROS
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

static void term_release (term_t **thisp) {
  if (NULL is *thisp) return;
  free ((*thisp)->name);
  free (*thisp);
  *thisp = NULL;
}

static int term_sane_mode (term_t *this) {
  if (this->mode == 's') return OK;
  ifnot (FdReferToATerminal(this->in_fd)) return NOTOK;

  struct termios mode;
  while (NOTOK  == tcgetattr (this->in_fd, &mode))
    if (errno == EINTR) return NOTOK;

  mode.c_iflag |= (BRKINT|INLCR|ICRNL|IXON|ISTRIP);
  mode.c_iflag &= ~(IGNBRK|INLCR|IGNCR|IXOFF);
  mode.c_oflag |= (OPOST|ONLCR);
  mode.c_lflag |= (ECHO|ECHOE|ECHOK|ECHOCTL|ISIG|ICANON|IEXTEN);
  mode.c_lflag &= ~(ECHONL|NOFLSH|TOSTOP|ECHOPRT);
  mode.c_cc[VEOF] = 'D'^64; // splitvt
  mode.c_cc[VMIN] = 1;   /* 0 */
  mode.c_cc[VTIME] = 0;  /* 1 */

  while (NOTOK == tcsetattr (this->in_fd, TCSAFLUSH, &mode))
    if (errno == EINTR) return NOTOK;

  this->mode = 's';
  return OK;
}

static int term_orig_mode (term_t *this) {
  if (this->mode == 'o') return OK;
  ifnot (FdReferToATerminal(this->in_fd)) return NOTOK;

  while (NOTOK == tcsetattr (this->in_fd, TCSAFLUSH, &this->orig_mode))
    ifnot (errno == EINTR) return NOTOK;

  this->mode = 'o';

  return OK;
}

static int term_raw_mode (term_t *this) {
  if (this->mode == 'r') return OK;
  ifnot (FdReferToATerminal(this->in_fd)) return NOTOK;

  while (NOTOK == tcgetattr (this->in_fd, &this->orig_mode))
    if (errno == EINTR) return NOTOK;

  this->raw_mode = this->orig_mode;
  this->raw_mode.c_iflag &= ~(INLCR|ICRNL|IXON|ISTRIP);
  this->raw_mode.c_cflag |= (CS8);
  this->raw_mode.c_oflag &= ~(OPOST);
  this->raw_mode.c_lflag &= ~(ECHO|ISIG|ICANON|IEXTEN);
  this->raw_mode.c_lflag &= NOFLSH;
  this->raw_mode.c_cc[VEOF] = 1;
  this->raw_mode.c_cc[VMIN] = 0;   /* 1 */
  this->raw_mode.c_cc[VTIME] = 1;  /* 0 */

  while (NOTOK == tcsetattr (this->in_fd, TCSAFLUSH, &this->raw_mode))
    ifnot (errno == EINTR) return NOTOK;

  this->mode = 'r';
  return OK;
}

static void term_screen_bell (term_t *this) {
  TERM_SEND_ESC_SEQ (TERM_BELL);
}

static void term_screen_set_color (term_t *this, int color) {
  IO.fd.write (this->out_fd, TERM_MAKE_COLOR(color), 5);
}

static void term_screen_clear (term_t *this) {
  TERM_SEND_ESC_SEQ (TERM_SCREEN_CLEAR);
}

static void term_screen_save (term_t *this) {
  TERM_SEND_ESC_SEQ (TERM_SCREEN_SAVE);
}

static void term_screen_restore (term_t *this) {
  TERM_SEND_ESC_SEQ (TERM_SCROLL_RESET);
  TERM_SEND_ESC_SEQ (TERM_SCREEN_RESTORE);
}

static void term_screen_clear_eol (term_t *this) {
  TERM_SEND_ESC_SEQ (TERM_LINE_CLR_EOL);
}

static void term_cursor_hide (term_t *this) {
  TERM_SEND_ESC_SEQ (TERM_CURSOR_HIDE);
}

static void term_cursor_show (term_t *this) {
  TERM_SEND_ESC_SEQ (TERM_CURSOR_SHOW);
}

static void term_cursor_restore (term_t *this) {
  TERM_SEND_ESC_SEQ (TERM_CURSOR_RESTORE);
}

static void term_cursor_save (term_t *this) {
  TERM_SEND_ESC_SEQ (TERM_CURSOR_SAVE);
}

static int term_cursor_get_pos (term_t *this, int *row, int *col) {
  if (NOTOK == TERM_SEND_ESC_SEQ (TERM_GET_PTR_POS))
    return NOTOK;

  char buf[32];
  uint i = 0;
  int bts;
  while (i < sizeof (buf) - 1) {
    if (NOTOK == (bts = IO.fd.read (this->in_fd, buf + i, 1)) ||
         bts == 0)
      return NOTOK;

    if (buf[i] == 'R') break;
    i++;
  }

  buf[i] = '\0';

  if (buf[0] != ESCAPE_KEY || buf[1] != '[' ||
      2 != sscanf (buf + 2, "%d;%d", row, col))
    return NOTOK;

  return OK;
}

static void term_cursor_set_pos (term_t *this, int row, int col) {
  char ptr[32];
  snprintf (ptr, 32, TERM_GOTO_PTR_POS_FMT, row, col);
  IO.fd.write (this->out_fd, ptr, bytelen (ptr));
}

static void term_init_size (term_t *this, int *rows, int *cols) {
  struct winsize wsiz;

  do {
    if (OK == ioctl (this->out_fd, TIOCGWINSZ, &wsiz)) {
      this->num_rows = (int) wsiz.ws_row;
      this->num_cols = (int) wsiz.ws_col;
      *rows = this->num_rows; *cols = this->num_cols;
      return;
    }
  } while (errno == EINTR);

  int orig_row, orig_col;
  term_cursor_get_pos (this, &orig_row, &orig_col);

  TERM_SEND_ESC_SEQ (TERM_LAST_RIGHT_CORNER);
  term_cursor_get_pos (this, rows, cols);
  term_cursor_set_pos (this, orig_row, orig_col);
}

static int term_set_mode (term_t *this, char mode) {
  switch (mode) {
    case 'o': return term_orig_mode (this);
    case 's': return term_sane_mode (this);
    case 'r': return term_raw_mode (this);
  }
  return NOTOK;
}

static void term_set_state_bit (term_t *this, int bit) {
  this->state |= (bit);
}

static void term_unset_state_bit (term_t *this, int bit) {
  this->state &= ~(bit);
}

static int term_set (term_t *this) {
  if (NOTOK is term_set_mode (this, 'r')) return NOTOK;
  term_cursor_get_pos (this, &this->orig_curs_row_pos, &this->orig_curs_col_pos);
  term_init_size (this, &this->num_rows, &this->num_cols);

  ifnot (this->state & TERM_DONOT_SAVE_SCREEN)
    term_screen_save (this);

  ifnot (this->state & TERM_DONOT_CLEAR_SCREEN)
    term_screen_clear (this);

  this->is_initialized = 1;
  return OK;
}

static int term_reset (term_t *this) {
  ifnot (this->is_initialized) return OK;
  term_set_mode (this, 's');
  term_cursor_set_pos (this, this->orig_curs_row_pos, this->orig_curs_col_pos);

  ifnot (this->state & TERM_DONOT_RESTORE_SCREEN)
    term_screen_restore (this);

  this->is_initialized = 0;
  return OK;
}

static term_t *term_new (void) {
  term_t *this = Alloc (sizeof (term_t));
  this->num_rows = 24;
  this->num_cols = 78;
  this->out_fd = STDOUT_FILENO;
  this->in_fd = STDIN_FILENO;
  this->mode = 'o';
  this->state = 0;
  this->is_initialized = 0;
  return this;
}

static int *term_get_dim (term_t *this, int *dim) {
  dim[0] = this->num_rows;
  dim[1] = this->num_cols;
  return dim;
}

public term_T __init_term__ (void) {
  __INIT__ (io);
  __INIT__ (string);

  return (term_T) {
    .self = (term_self) {
      .new = term_new,
      .set = term_set,
      .reset = term_reset,
      .release = term_release,
      .set_mode = term_set_mode,
      .raw_mode =  term_raw_mode,
      .sane_mode = term_sane_mode,
      .orig_mode = term_orig_mode,
      .init_size = term_init_size,
      .set_state_bit = term_set_state_bit,
      .unset_state_bit = term_unset_state_bit,
      .get = (term_get_self) {
        .dim = term_get_dim
      },
      .screen = (term_screen_self) {
        .bell = term_screen_bell,
        .save = term_screen_save,
        .clear = term_screen_clear,
        .restore = term_screen_restore,
        .clear_eol = term_screen_clear_eol,
        .set_color = term_screen_set_color
      },
      .cursor = (term_cursor_self) {
        .hide = term_cursor_hide,
        .show = term_cursor_show,
        .save = term_cursor_save,
        .set_pos = term_cursor_set_pos,
        .get_pos = term_cursor_get_pos,
        .restore = term_cursor_restore
      }
    }
  };
}
