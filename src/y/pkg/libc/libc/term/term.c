// provides: void term_release (term_t **)
// provides: int term_sane_mode (term_t *)
// provides: int term_orig_mode (term_t *)
// provides: int term_raw_mode (term_t *)
// provides: void term_screen_bell (term_t *)
// provides: void term_screen_set_color (term_t *, int)
// provides: void term_screen_clear (term_t *)
// provides: void term_screen_save (term_t *)
// provides: void term_screen_restore (term_t *)
// provides: void term_screen_clear_eol (term_t *)
// provides: void term_cursor_hide (term_t *)
// provides: void term_cursor_show (term_t *)
// provides: void term_cursor_restore (term_t *)
// provides: void term_cursor_save (term_t *)
// provides: int term_cursor_get_pos (term_t *, int *, int *)
// provides: void term_cursor_set_pos (term_t *, int, int)
// provides: void term_init_size (term_t *, int *, int *)
// provides: int term_set_mode (term_t *, char)
// provides: void term_set_state_bit (term_t *, int)
// provides: void term_unset_state_bit (term_t *, int)
// provides: int term_set (term_t *)
// provides: int term_reset (term_t *)
// provides: int *term_get_dim (term_t *, int *)
// provides: term_t *term_new (void)
// requires: unistd/termios.h
// requires: unistd/isatty.c
// requires: unistd/tcgetattr.c
// requires: unistd/tcsetattr.c
// requires: unistd/write.c
// requires: unistd/read.c
// requires: stdlib/atoi.c
// requires: string/bytelen.c
// requires: string/vsnprintf.c
// requires: sys/ioctl.c
// requires: utf8/utf8.h
// requires: utf8/utf8_charlen.c
// requires: term/term.h

#define TERM_DONOT_SAVE_SCREEN    (1 << 0)
#define TERM_DONOT_CLEAR_SCREEN   (1 << 1)
#define TERM_DONOT_RESTORE_SCREEN (1 << 2)

void term_release (term_t **thisp) {
  if (NULL == *thisp)
    return;
  Release ((*thisp)->name);
  Release (*thisp);
  *thisp = NULL;
}

int term_sane_mode (term_t *this) {
  if (this->mode == 's')
    return 0;

  ifnot (sys_isatty (this->in_fd))
    return -1;

  struct termios mode;
  while (-1  == sys_tcgetattr (this->in_fd, &mode))
    if (sys_errno == EINTR)
      return -1;

  mode.c_iflag |= (BRKINT|INLCR|ICRNL|IXON|ISTRIP);
  mode.c_iflag &= ~(IGNBRK|INLCR|IGNCR|IXOFF);
  mode.c_oflag |= (OPOST|ONLCR);
  mode.c_lflag |= (ECHO|ECHOE|ECHOK|ECHOCTL|ISIG|ICANON|IEXTEN);
  mode.c_lflag &= ~(ECHONL|NOFLSH|TOSTOP|ECHOPRT);
  mode.c_cc[VEOF] = 'D'^64; // splitvt
  mode.c_cc[VMIN] = 1;   /* 0 */
  mode.c_cc[VTIME] = 0;  /* 1 */

  while (-1 == sys_tcsetattr (this->in_fd, TCSAFLUSH, &mode))
    if (sys_errno == EINTR)
      return -1;

  this->mode = 's';
  return 0;
}

int term_orig_mode (term_t *this) {
  if (this->mode == 'o')
    return 0;

  ifnot (sys_isatty (this->in_fd))
    return -1;

  while (-1 == sys_tcsetattr (this->in_fd, TCSAFLUSH, &this->orig_mode))
    ifnot (sys_errno == EINTR)
      return -1;

  this->mode = 'o';

  return 0;
}

int term_raw_mode (term_t *this) {
  if (this->mode == 'r')
    return 0;

  ifnot (sys_isatty (this->in_fd))
    return -1;

  while (-1 == sys_tcgetattr (this->in_fd, &this->orig_mode))
    if (sys_errno == EINTR)
      return -1;

  this->raw_mode = this->orig_mode;
  this->raw_mode.c_iflag &= ~(INLCR|ICRNL|IXON|ISTRIP);
  this->raw_mode.c_cflag |= (CS8);
  this->raw_mode.c_oflag &= ~(OPOST);
  this->raw_mode.c_lflag &= ~(ECHO|ISIG|ICANON|IEXTEN);
  this->raw_mode.c_lflag &= NOFLSH;
  this->raw_mode.c_cc[VEOF] = 1;
  this->raw_mode.c_cc[VMIN] = 0;   /* 1 */
  this->raw_mode.c_cc[VTIME] = 1;  /* 0 */

  while (-1 == sys_tcsetattr (this->in_fd, TCSAFLUSH, &this->raw_mode))
    ifnot (sys_errno == EINTR)
      return -1;

  this->mode = 'r';
  return OK;
}

void term_screen_bell (term_t *this) {
  SEND_ESC_SEQ (this->out_fd, TERM_BELL);
}

void term_screen_set_color (term_t *this, int color) {
  char buf[8];
  int len = sys_snprintf (buf, 8, TERM_SET_COLOR_FMT, color);
  sys_write (this->out_fd, buf, len);
}

void term_screen_clear (term_t *this) {
  SEND_ESC_SEQ (this->out_fd, TERM_SCREEN_CLEAR);
}

void term_screen_save (term_t *this) {
  SEND_ESC_SEQ (this->out_fd, TERM_SCREEN_SAVE);
}

void term_screen_restore (term_t *this) {
  SEND_ESC_SEQ (this->out_fd, TERM_SCROLL_RESET);
  SEND_ESC_SEQ (this->out_fd, TERM_SCREEN_RESTORE);
}

void term_screen_clear_eol (term_t *this) {
  SEND_ESC_SEQ (this->out_fd, TERM_LINE_CLR_EOL);
}

void term_cursor_hide (term_t *this) {
  SEND_ESC_SEQ (this->out_fd, TERM_CURSOR_HIDE);
}

void term_cursor_show (term_t *this) {
  SEND_ESC_SEQ (this->out_fd, TERM_CURSOR_SHOW);
}

void term_cursor_restore (term_t *this) {
  SEND_ESC_SEQ (this->out_fd, TERM_CURSOR_RESTORE);
}

void term_cursor_save (term_t *this) {
  SEND_ESC_SEQ (this->out_fd, TERM_CURSOR_SAVE);
}

int term_cursor_get_pos (term_t *this, int *row, int *col) {
  if (-1 == SEND_ESC_SEQ (this->out_fd, TERM_GET_PTR_POS))
    return -1;

  char buf[32];
  uint i = 0;
  int bts;
  while (i < sizeof (buf) - 1) {
    if (-1 == (bts = sys_read (this->in_fd, buf + i, 1)) || bts == 0)
      return -1;

    if (buf[i] == 'R') break;
    i++;
  }

  buf[i] = '\0';

  if (buf[0] != ESCAPE_KEY || buf[1] != '[')
    return -1;

  char *sp = buf + 2;
  int r = 0;
  while (*sp && ('0' < *sp && *sp < '9'))
    r = (10 * r) + (*sp++ - '0');

  if (*sp++ != ';')
    return -1;

  int c = 0;
  while (*sp && ('0' < *sp && *sp < '9'))
    c = (10 * c) + (*sp++ - '0');

  if (*sp != '\0')
    return -1;

  *row = r;
  *col = c;

  return 0;
}

void term_cursor_set_pos (term_t *this, int row, int col) {
  char ptr[32];
  sys_snprintf (ptr, 32, TERM_GOTO_PTR_POS_FMT, row, col);
  sys_write (this->out_fd, ptr, bytelen (ptr));
}

void term_init_size (term_t *this, int *rows, int *cols) {
  struct winsize wsiz;

  do {
    if (OK == sys_ioctl (this->out_fd, TIOCGWINSZ, &wsiz)) {
      this->num_rows = (int) wsiz.ws_row;
      this->num_cols = (int) wsiz.ws_col;
      *rows = this->num_rows;
      *cols = this->num_cols;
      return;
    }
  } while (sys_errno == EINTR);

  int orig_row, orig_col;
  term_cursor_get_pos (this, &orig_row, &orig_col);

  SEND_ESC_SEQ (this->out_fd, TERM_LAST_RIGHT_CORNER);
  term_cursor_get_pos (this, rows, cols);
  term_cursor_set_pos (this, orig_row, orig_col);
}

int term_set_mode (term_t *this, char mode) {
  switch (mode) {
    case 'o': return term_orig_mode (this);
    case 's': return term_sane_mode (this);
    case 'r': return term_raw_mode (this);
  }

  return -1;
}

void term_set_state_bit (term_t *this, int bit) {
  this->state |= (bit);
}

void term_unset_state_bit (term_t *this, int bit) {
  this->state &= ~(bit);
}

int term_set (term_t *this) {
  if (-1 is term_set_mode (this, 'r'))
    return -1;
  term_cursor_get_pos (this, &this->orig_curs_row_pos, &this->orig_curs_col_pos);
  term_init_size (this, &this->num_rows, &this->num_cols);

  ifnot (this->state & TERM_DONOT_SAVE_SCREEN)
    term_screen_save (this);

  ifnot (this->state & TERM_DONOT_CLEAR_SCREEN)
    term_screen_clear (this);

  this->is_initialized = 1;
  return 0;
}

int term_reset (term_t *this) {
  ifnot (this->is_initialized)
    return 0;

  term_set_mode (this, 's');
  term_cursor_set_pos (this, this->orig_curs_row_pos, this->orig_curs_col_pos);

  ifnot (this->state & TERM_DONOT_RESTORE_SCREEN)
    term_screen_restore (this);

  this->is_initialized = 0;
  return 0;
}

int *term_get_dim (term_t *this, int *dim) {
  dim[0] = this->num_rows;
  dim[1] = this->num_cols;
  return dim;
}

term_t *term_new (void) {
  term_t *this = Alloc (sizeof (term_t));
  this->num_rows = 24;
  this->num_cols = 78;
  this->out_fd = 1;
  this->in_fd = 0;
  this->mode = 'o';
  this->state = 0;
  this->is_initialized = 0;
  return this;
}
