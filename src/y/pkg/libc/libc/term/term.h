#define TERM_ITALIC                 "\033[3m"
#define TERM_ITALIC_LEN             4
#define TERM_INVERTED               "\033[7m"
#define TERM_INVERTED_LEN           4
#define TERM_LAST_RIGHT_CORNER      "\033[999C\033[999B"
#define TERM_LAST_RIGHT_CORNER_LEN  12
#define TERM_FIRST_LEFT_CORNER      "\033[H"
#define TERM_FIRST_LEFT_CORNER_LEN  3
#define TERM_GET_PTR_POS            "\033[6n"
#define TERM_GET_PTR_POS_LEN        4
#define TERM_SCREEN_SAVE            "\033[?47h"
#define TERM_SCREEN_SAVE_LEN        6
#define TERM_SCREEN_RESTORE        "\033[?47l"
#define TERM_SCREEN_RESTORE_LEN     6
#define TERM_SCREEN_CLEAR           "\033[2J"
#define TERM_SCREEN_CLEAR_LEN       4
#define TERM_SCROLL_RESET           "\033[r"
#define TERM_SCROLL_RESET_LEN       3
#define TERM_GOTO_PTR_POS_FMT       "\033[%d;%dH"
#define TERM_CURSOR_HIDE            "\033[?25l"
#define TERM_CURSOR_HIDE_LEN        6
#define TERM_CURSOR_SHOW            "\033[?25h"
#define TERM_CURSOR_SHOW_LEN        6
#define TERM_CURSOR_RESTORE         "\0338"
#define TERM_CURSOR_RESTORE_LEN     2
#define TERM_CURSOR_SAVE            "\0337"
#define TERM_CURSOR_SAVE_LEN        2
#define TERM_AUTOWRAP_ON            "\033[?7h"
#define TERM_AUTOWRAP_ON_LEN        5
#define TERM_AUTOWRAP_OFF           "\033[?7l"
#define TERM_AUTOWRAP_OFF_LEN       5
#define TERM_BELL                   "\033[7"
#define TERM_BELL_LEN               3
#define TERM_NEXT_BOL              "\033E"
#define TERM_NEXT_BOL_LEN           2
#define TERM_SCROLL_REGION_FMT      "\033[%d;%dr"
#define TERM_COLOR_RESET            "\033[m"
#define TERM_COLOR_RESET_LEN        3
#define TERM_SET_COLOR_FMT          "\033[%dm"
#define TERM_SET_COLOR_FMT_LEN      5
#define TERM_LINE_CLR_EOL           "\033[2K"
#define TERM_LINE_CLR_EOL_LEN       4

#define COLOR_RED         31
#define COLOR_GREEN       32
#define COLOR_YELLOW      33
#define COLOR_BLUE        34
#define COLOR_MAGENTA     35
#define COLOR_CYAN        36
#define COLOR_WHITE       37
#define COLOR_FG_NORMAL   39
#define COLOR_BG_NORMAL   49

#define COLOR_BOX         COLOR_YELLOW

#define SEND_ESC_SEQ(_fd_, _seq_) sys_write ((_fd_), _seq_, _seq_ ## _LEN)

#ifndef BACKSPACE_KEY
#define BACKSPACE_KEY   010
#endif

#ifndef ESCAPE_KEY
#define ESCAPE_KEY      033
#endif

#ifndef ARROW_DOWN_KEY
#define ARROW_DOWN_KEY  0402
#endif

#ifndef ARROW_UP_KEY
#define ARROW_UP_KEY    0403
#endif

#ifndef ARROW_LEFT_KEY
#define ARROW_LEFT_KEY  0404
#endif

#ifndef ARROW_RIGHT_KEY
#define ARROW_RIGHT_KEY 0405
#endif

#ifndef HOME_KEY
#define HOME_KEY        0406
#endif

#ifndef FN_KEY
#define FN_KEY(x)       (x + 0410)
#endif

#ifndef DELETE_KEY
#define DELETE_KEY      0512
#endif

#ifndef INSERT_KEY
#define INSERT_KEY      0513
#endif

#ifndef PAGE_DOWN_KEY
#define PAGE_DOWN_KEY   0522
#endif

#ifndef PAGE_UP_KEY
#define PAGE_UP_KEY     0523
#endif

#ifndef END_KEY
#define END_KEY         0550
#endif

#ifndef CTRL
#define CTRL(X) (X & 037)
#endif

#ifndef MODE_KEY
#define MODE_KEY  CTRL('\\')
#endif

typedef struct {
  int is_initialized;

  char
     mode,
    *name;

  int
    state,
    in_fd,
    out_fd,
    orig_curs_row_pos,
    orig_curs_col_pos,
    num_rows,
    num_cols;

  struct termios
    orig_mode,
    raw_mode;

} term_t;
