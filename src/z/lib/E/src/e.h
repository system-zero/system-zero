#ifndef LIBVED_H
#define LIBVED_H

#define MYNAME "veda"
#define ED_INSTANCES 252

#define MIN_ROWS 8
#define MIN_COLS  2

#define MAX_FRAMES 3

#define DEFAULT_SHIFTWIDTH 0
#define DEFAULT_PROMPT_CHAR ':'
#define DEFAULT_ON_EMPTY_LINE_STRING "~"

#define DEFAULT_CLOCK CLOCK_REALTIME

#ifndef RLINE_HISTORY_NUM_ENTRIES
#define RLINE_HISTORY_NUM_ENTRIES 20
#endif

#define RLINE_LAST_COMPONENT_NUM_ENTRIES 10

#ifndef UNDO_NUM_ENTRIES
#define UNDO_NUM_ENTRIES 40
#endif

#ifndef TABWIDTH
#define TABWIDTH 8
#endif

#ifndef AUTOCHDIR
#define AUTOCHDIR 1
#endif

#ifndef CLEAR_BLANKLINES
#define CLEAR_BLANKLINES 1
#endif

#ifndef TAB_ON_INSERT_MODE_INDENTS
#define TAB_ON_INSERT_MODE_INDENTS 0
#endif

#ifndef C_TAB_ON_INSERT_MODE_INDENTS
#define C_TAB_ON_INSERT_MODE_INDENTS 1
#endif

#ifndef C_DEFAULT_SHIFTWIDTH
#define C_DEFAULT_SHIFTWIDTH 2
#endif

#ifndef CARRIAGE_RETURN_ON_NORMAL_IS_LIKE_INSERT_MODE
#define CARRIAGE_RETURN_ON_NORMAL_IS_LIKE_INSERT_MODE 1
#endif

#ifndef SPACE_ON_NORMAL_IS_LIKE_INSERT_MODE
#define SPACE_ON_NORMAL_IS_LIKE_INSERT_MODE 1
#endif

#ifndef SMALL_E_ON_NORMAL_GOES_INSERT_MODE
#define SMALL_E_ON_NORMAL_GOES_INSERT_MODE 1
#endif

#ifndef BACKSPACE_ON_FIRST_IDX_REMOVE_TRAILING_SPACES
#define BACKSPACE_ON_FIRST_IDX_REMOVE_TRAILING_SPACES 1
#endif

#ifndef BACKSPACE_ON_NORMAL_IS_LIKE_INSERT_MODE
#define BACKSPACE_ON_NORMAL_IS_LIKE_INSERT_MODE 1
#endif

#ifndef BACKSPACE_ON_NORMAL_GOES_UP
#define BACKSPACE_ON_NORMAL_GOES_UP 1
#else
#if (BACKSPACE_ON_NORMAL_GOES_UP == 1)
#undef  BACKSPACE_ON_FIRST_IDX_REMOVE_TRAILING_SPACES
#define BACKSPACE_ON_FIRST_IDX_REMOVE_TRAILING_SPACES 0
#endif
#endif

#ifndef BACKSPACE_ON_INSERT_GOES_UP_AND_JOIN
#define BACKSPACE_ON_INSERT_GOES_UP_AND_JOIN 1
#endif

#ifndef NUM_SYNTAXES
#define NUM_SYNTAXES 32
#endif

#ifndef MAX_BACKTRACK_LINES_FOR_ML_COMMENTS
#define MAX_BACKTRACK_LINES_FOR_ML_COMMENTS 24
#endif

#define DIRWALK_MAX_DEPTH 1024
#define MAX_SCREEN_ROWS    256

#define MAXLEN_MODE        16
#define MAXLEN_FTYPE_NAME  16
#define MAXLEN_WORD_ACTION 512
#define MAXLEN_COM         512
#define MAXLEN_PAT         PATH_MAX

#define MAXLEN_ED_NAME MAXLEN_NAME
#define OUTPUT_FD STDOUT_FILENO

#define Notword ".,?/+*-=~%<>[](){}\\'\";"
#define Notword_len 22
#define Notfname "|][\""
#define Notfname_len 4

#define IsAlsoAHex(c)    (((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define IsAlsoANumber(c) ((c) == '.' || (c) == 'x' || IsAlsoAHex (c))

#define NO_GLOBAL 0
#define GLOBAL    1

#define NO_FORCE 0
#define FORCE    1

#define NO_INTERACTIVE 0
#define INTERACTIVE    1

#define NOT_AT_EOF     0
#define AT_EOF         1

#define NO_COUNT_SPECIAL 0
#define COUNT_SPECIAL    1

#define DONOT_OPEN_FILE_IFNOT_EXISTS 0
#define OPEN_FILE_IFNOT_EXISTS       1

#define DONOT_REOPEN_FILE_IF_LOADED 0
#define REOPEN_FILE_IF_LOADED       1

#define SHARED_ALLOCATION 0
#define NEW_ALLOCATION    1

#define DONOT_ABORT_ON_ESCAPE 0
#define ABORT_ON_ESCAPE       1

#define VERBOSE_OFF 0
#define VERBOSE_ON  1

#define DONOT_DRAW 0
#define DRAW       1

#define DONOT_APPEND 0
#define APPEND       1

#define DONOT_CLEAR  0
#define CLEAR        1

#define DONOR_REOPEN 0
#define REOPEN       1

#define X_PRIMARY     0
#define X_CLIPBOARD   1

#define DONOT_REDIRECT 0
#define REDIRECT       1

#define NULL_CALLBACK_FN  NULL

#define DEFAULT_ORDER  0
#define REVERSE_ORDER -1
#define NORMAL_ORDER DEFAULT_ORDER

#define LEFT_DIRECTION   0
#define RIGHT_DIRECTION -1

#define DONOT_ADJUST_COL 0
#define ADJUST_COL 1

#define AT_CURRENT_FRAME -1

#define NO_CB_FN NULL

#define NO_COMMAND 0
#define NO_OPTION 0

#define NO_NL  0
#define ADD_NL 1

#define NO  0
#define YES 1

#define RLINE_HISTORY  0
#define SEARCH_HISTORY 1

#define STRCHOP_NOTOK NOTOK
#define STRCHOP_OK OK
#define STRCHOP_RETURN (OK + 1)

#define LINEWISE 1
#define CHARWISE 2

#define TO_LOWER 0
#define TO_UPPER 1

#define DELETE_LINE  1
#define REPLACE_LINE 2
#define INSERT_LINE  3

#define HL_STRINGS_NO 0
#define HL_STRINGS 1

#define HL_NUMBERS_NO 0
#define HL_NUMBERS 1

#define NOT       (0 << 0)
#define UNSET     NOT

#define MSG_SET_RESET       (1 << 0)
#define MSG_SET_APPEND      (1 << 1)
#define MSG_SET_OPEN        (1 << 2)
#define MSG_SET_CLOSE       (1 << 3)
#define MSG_SET_DRAW        (1 << 4)
#define MSG_SET_COLOR       (1 << 5)
#define MSG_SET_TO_MSG_BUF  (1 << 6)
#define MSG_SET_TO_MSG_LINE (1 << 7)

#define FILE_IS_REGULAR     (1 << 0)
#define FILE_IS_RDONLY      (1 << 1)
#define FILE_EXISTS         (1 << 2)
#define FILE_IS_READABLE    (1 << 3)
#define FILE_IS_WRITABLE    (1 << 4)
#define BUF_IS_MODIFIED     (1 << 5)
#define BUF_IS_VISIBLE      (1 << 6)
#define BUF_IS_RDONLY       (1 << 7)
#define BUF_IS_PAGER        (1 << 8)
#define BUF_IS_SPECIAL      (1 << 9)
#define BUF_FORCE_REOPEN    (1 << 10)
#define PTR_IS_AT_EOL       (1 << 12)
#define BUF_LW_RESELECT     (1 << 13)

#define ED_SUSPENDED        (1 << 0)
#define ED_EXIT             (1 << 1)
#define ED_EXIT_ALL         (1 << 2)
#define ED_EXIT_ALL_FORCE   (1 << 3)
#define ED_PAUSE            (1 << 4)
#define ED_NEW              (1 << 5)
#define ED_NEXT             (1 << 6)
#define ED_PREV             (1 << 7)
#define ED_PREV_FOCUSED     (1 << 8)

#define ED_INIT_OPT_LOAD_HISTORY (1 << 0)

#define E_SUSPENDED                (1 << 0)
#define E_EXIT                     (1 << 1)
#define E_EXIT_ALL                 (1 << 2)
#define E_EXIT_ALL_FORCE           (1 << 3)
#define E_PAUSE                    (1 << 4)
#define E_DONOT_RESTORE_TERM_STATE (1 << 5)
#define E_DONOT_CHANGE_FOCUS       (1 << 6)

#define IDX_OUT_OF_BOUNDS_ERROR_STATE  (1 << 0)

#define LAST_INSTANCE_ERROR_STATE      (1 << 0)
#define ARG_IDX_IS_CUR_IDX_ERROR_STATE (1 << 1)

#define FTYPE_DEFAULT 0

#define UNNAMED         "[No Name]"
#define BACKUP_SUFFIX   "~"

#define FIRST_FRAME  0
#define SECOND_FRAME 1
#define THIRD_FRAME  2

#define LAST_ARG_KEY    037

#define COLOR_SU          COLOR_RED
#define COLOR_BOX         COLOR_YELLOW
#define COLOR_MSG         COLOR_YELLOW
#define COLOR_ERROR       COLOR_RED
#define COLOR_PROMPT      COLOR_YELLOW
#define COLOR_NORMAL      COLOR_FG_NORMAL
#define COLOR_MENU_BG     COLOR_RED
#define COLOR_TOPLINE     COLOR_YELLOW
#define COLOR_DIVIDER     COLOR_MAGENTA
#define COLOR_WARNING     COLOR_MAGENTA
#define COLOR_SUCCESS     COLOR_GREEN
#define COLOR_TOPLINE     COLOR_YELLOW
#define COLOR_WARNING     COLOR_MAGENTA
#define COLOR_MENU_SEL    COLOR_GREEN
#define COLOR_STATUSLINE  COLOR_BLUE
#define COLOR_MENU_HEADER COLOR_CYAN

#define HL_NORMAL         COLOR_NORMAL
#define HL_VISUAL         COLOR_CYAN
#define HL_IDENTIFIER     COLOR_BLUE
#define HL_KEYWORD        COLOR_MAGENTA
#define HL_OPERATOR       COLOR_MAGENTA
#define HL_FUNCTION       COLOR_MAGENTA
#define HL_VARIABLE       COLOR_BLUE
#define HL_TYPE           COLOR_BLUE
#define HL_DEFINITION     COLOR_BLUE
#define HL_COMMENT        COLOR_YELLOW
#define HL_NUMBER         COLOR_MAGENTA
#define HL_STRING_DELIM   COLOR_GREEN
#define HL_STRING         COLOR_YELLOW
#define HL_TRAILING_WS    COLOR_RED
#define HL_TAB            COLOR_CYAN
#define HL_ERROR          COLOR_RED
#define HL_QUOTE          COLOR_YELLOW
#define HL_QUOTE_1        COLOR_BLUE
#define HL_QUOTE_2        COLOR_CYAN
#define HL_TXT            COLOR_WHITE

#define COLOR_CHARS  "IKCONSDFVTMEQ><"
// I: identifier, K: keyword, C: comment, O: operator, N: number, S: string
// D:_delimiter F: function   V: variable, T: type,  M: macro,
// E: error, Q: quote, >: qoute1, <: quote_2  

#define NO_OFFSET  0

/* #define RESET_ERRNO errno = 0   mostly as an indicator and reminder */

#define ZERO_FLAGS 0

#define RL_ERROR                             -20
#define RL_NO_COMMAND                        -21
#define RL_ARG_AWAITING_STRING_OPTION_ERROR  -22
#define RL_ARGUMENT_MISSING_ERROR            -23
#define RL_UNTERMINATED_QUOTED_STRING_ERROR  -24
#define RL_UNRECOGNIZED_OPTION               -25

#define RL_ARG_FILENAME    (1 << 0)
#define RL_ARG_RANGE       (1 << 1)
#define RL_ARG_GLOBAL      (1 << 2)
#define RL_ARG_PATTERN     (1 << 3)
#define RL_ARG_SUB         (1 << 4)
#define RL_ARG_INTERACTIVE (1 << 5)
#define RL_ARG_APPEND      (1 << 6)
#define RL_ARG_BUFNAME     (1 << 7)
#define RL_ARG_VERBOSE     (1 << 8)
#define RL_ARG_ANYTYPE     (1 << 9)
#define RL_ARG_RECURSIVE   (1 << 10)

#define RL_OK (1 << 0)
#define RL_CONTINUE (1 << 1)
#define RL_BREAK (1 << 2)
#define RL_PROCESS_CHAR (1 << 3)
#define RL_INSERT_CHAR (1 << 4)
#define RL_CLEAR (1 << 5)
#define RL_WRITE (1 << 6)
#define RL_IS_VISIBLE (1 << 7)
#define RL_CURSOR_HIDE (1 << 8)
#define RL_CLEAR_FREE_LINE (1 << 9)
#define RL_POST_PROCESS (1 << 10)
#define RL_SET_POS (1 << 11)
#define RL_EXEC (1 << 12)
#define RL_FIRST_CHAR_COMPLETION (1 << 13)

#define RL_OPT_HAS_TAB_COMPLETION (1 << 0)
#define RL_OPT_HAS_HISTORY_COMPLETION (1 << 1)
#define RL_OPT_RETURN_AFTER_TAB_COMPLETION (1 << 2)

enum {
  NO_CALLBACK_FUNCTION = -4,
  RLINE_NO_COMMAND = -3,
  ERROR = -2,
  NOTHING_TODO = -1,
  DONE = 0,
  IGNORE_BLOCK,
  EXIT_THIS,
  EXIT_ALL,
  EXIT_ALL_FORCE,
  WIN_EXIT,
  BUF_QUIT,
  NEWCHAR,
};

typedef ptrdiff_t idx_t;
typedef ptrdiff_t msize_t;

#define STR_FMT(fmt_, ...)                                            \
({                                                                    \
  char buf_[MAXLEN_LINE];                                             \
  snprintf (buf_, MAXLEN_LINE, fmt_, __VA_ARGS__);                    \
  buf_;                                                               \
})

#define __Me__  Me
#define __this__ this
#define __thisp__ thisp
#define __prop__ prop
#define __self__ self
#define __root__ root
#define __parent__ parent
#define __current__ current
#define $myprop __this__->__prop__
#define $my(__p__) __this__->__prop__->__p__
#define $self(__f__) $myprop->__Me__->self.__f__
#define $selfp(__f__) (*__thisp__)->prop->__Me__->self.__f__
#define self(__f__, ...) $self(__f__)(__this__, ##__VA_ARGS__)
#define selfp(__f__, ...) $selfp(__f__)(__thisp__, ##__VA_ARGS__)
#define My(__C__) $my(__C__)->self
//#define $from(__v__) __v__->__prop__
#define $from(__v__, __p__) __v__->__prop__->__p__
#define $mycur(__v__) __this__->__current__->__v__
#define $myparents(__p__) __this__->__prop__->__parent__->__prop__->__p__
#define $myroots(__p__) __this__->__prop__->__root__->__prop__->__p__

typedef struct fp_t fp_t;
typedef struct reg_t reg_t;
typedef struct Reg_t Reg_t;
typedef struct arg_t arg_t;
typedef struct row_t row_t;
typedef struct dim_t dim_t;
typedef struct syn_t syn_t;
typedef struct undo_t undo_t;
typedef struct hist_t hist_t;
typedef struct menu_t menu_t;
typedef struct mark_t mark_t;
typedef struct video_t video_t;
typedef struct rline_t rline_t;
typedef struct ftype_t ftype_t;
typedef struct search_t search_t;
typedef struct Search_t Search_t;
typedef struct action_t action_t;
typedef struct Action_t Action_t;
typedef struct balanced_t balanced_t;
typedef struct histitem_t histitem_t;
typedef struct h_rlineitem_t h_rlineitem_t;
typedef struct buf_t buf_t;
typedef struct bufinfo_t bufinfo_t;
typedef struct buf_prop buf_prop;
typedef struct bufiter_t bufiter_t;

typedef struct win_t win_t;
typedef struct wininfo_t wininfo_t;
typedef struct win_prop win_prop;

typedef struct ed_t ed_t;
typedef struct edinfo_t edinfo_t;
typedef struct ed_prop ed_prop;
typedef struct ed_T ed_T;

typedef struct video_T video_T;

typedef struct E_prop E_prop;
typedef struct E_self E_self;
typedef struct E_T E_T;

typedef struct ed_opts ed_opts;

/* this might make things harder for the reader, because hides details, but if
 * something is gonna change in future, if it's not just a signle change it is
 * certainly (easier) searchable */

typedef int  (*Rline_cb) (buf_t **, rline_t *, utf8);
typedef int  (*StrChop_cb) (Vstring_t *, char *, void *);
typedef int  (*RlineAtBeg_cb) (rline_t **);
typedef int  (*RlineAtEnd_cb) (rline_t **);
typedef int  (*RlineTabCompletion_cb) (rline_t *);
typedef int  (*MenuProcessList_cb) (menu_t *);
typedef int  (*VisualLwMode_cb) (buf_t **, int, int, Vstring_t *, utf8, char *);
typedef int  (*VisualCwMode_cb) (buf_t **, int, int, string_t *, utf8, char *);
typedef int  (*WordActions_cb)  (buf_t **, int, int, bufiter_t *, char *, utf8, char *);
typedef int  (*LineMode_cb)     (buf_t **, utf8, char *, char *, size_t);
typedef int  (*FileActions_cb)  (buf_t **, utf8, char *);
typedef int  (*BufNormalBeg_cb) (buf_t **, utf8, int, int);
typedef int  (*BufNormalEnd_cb) (buf_t **, utf8, int, int);
typedef int  (*BufNormalOng_cb) (buf_t **, int);
typedef char *(*FtypeOpenFnameUnderCursor_cb) (char *, size_t, size_t);
typedef dim_t **(*WinDimCalc_cb) (win_t *, int, int, int, int);
typedef string_t *(*FtypeAutoIndent_cb) (buf_t *, row_t *);
typedef int (*Balanced_cb) (buf_t **, int, int);
typedef int (*ExprRegister_cb) (ed_t *, buf_t *, int);
typedef void (*EAtExit_cb) (void);
typedef void (*EdAtExit_cb) (ed_t *);
typedef void (*EdAtInit_cb) (ed_t *, ed_opts);

/* in between */
typedef void (*Record_cb) (ed_t *, char *);
typedef int  (*IRecord_cb) (ed_t *, Vstring_t *);
typedef char *(*InitRecord_cb) (ed_t *);

#define NULL_REF NULL

/* do not change order */
struct syn_t {
  char
    *filetype,
    **filenames,
    **extensions,
    **shebangs,
    **keywords,
    *operators,
    *singleline_comment,
    *multiline_comment_start,
    *multiline_comment_end,
    *multiline_comment_continuation;

  int
    hl_strings,
    hl_numbers;

  char *(*parse) (buf_t *, char *, int, int, row_t *);
  ftype_t *(*init) (buf_t *);

  int state;

  size_t
     multiline_comment_continuation_len,
    *keywords_len,
    *keywords_colors;

  char *balanced_pairs;
};

struct ftype_t {
  char
    name[MAXLEN_FTYPE_NAME];

  string_t *on_emptyline;

  int
    shiftwidth,
    tabwidth,
    autochdir,
    tab_indents,
    clear_blanklines,
    cr_on_normal_is_like_insert_mode,
    backspace_on_normal_is_like_insert_mode,
    backspace_on_normal_goes_up,
    backspace_on_insert_goes_up_and_join,
    backspace_on_first_idx_remove_trailing_spaces,
    space_on_normal_is_like_insert_mode,
    small_e_on_normal_goes_insert_mode,
    read_from_shell;

  FtypeAutoIndent_cb autoindent;
  FtypeOpenFnameUnderCursor_cb on_open_fname_under_cursor;
  Balanced_cb balanced;
};

struct fp_t {
  FILE   *fp;
  size_t  num_bytes;

  int
    fd,
    error;
};

struct bufiter_t {
  int
    idx,
    col_idx;

  size_t num_lines;
  row_t *row;
  string_t *line;
};

struct balanced_t {
  char bytes[512];
  int  linenr[512];
  int  last_idx;
  int  has_opening_string;
};

struct bufinfo_t {
  char
    *fname,
    *cwd,
    *parents_name;

  int
    at_frame,
    cur_idx,
    is_writable;

  size_t
    num_bytes,
    num_lines;
};

struct wininfo_t {
  char
    *name,
    *parents_name,
    *cur_buf,
    **buf_names;

  int
    num_frames,
    cur_idx;

  size_t
    num_items;
};

struct edinfo_t {
  char
    *name,
    *cur_win,
    **win_names,
    **special_win_names;

  int
    cur_idx;

  size_t
    num_special_win,
    num_items;
};


typedef struct buf_opts {
   win_t *win;

   char
     *fname,
     *backup_suffix;

   int
     flags,
     ftype,
     at_frame,
     at_linenr,
     at_column,
     backupfile;

   long autosave;
} buf_opts;

#define BufOpts(...) (buf_opts) { \
  .at_frame = 0,                  \
  .at_linenr = 1,                 \
  .at_column = 1,                 \
  .ftype = FTYPE_DEFAULT,         \
  .autosave = 0,                  \
  .backupfile = 0,                \
  .backup_suffix = BACKUP_SUFFIX, \
  .flags = 0,                     \
  .fname = UNNAMED,               \
   __VA_ARGS__}

struct ed_opts {
  int
    first_row,
    first_col,
    num_rows,
    num_cols,
    flags,
    num_win,
    term_flags;

  char
    *hs_file,
    *hrl_file;

  EdAtInit_cb init_cb;
};

#define EdOpts(...) (ed_opts) { \
  .first_row = 1,               \
  .first_col = 1,               \
  .num_rows = 0,                \
  .num_cols = 0,                \
  .flags = 0,                   \
  .num_win = 1,                 \
  .term_flags = 0,              \
  .init_cb = NULL,              \
  .hs_file = NULL,              \
  .hrl_file = NULL,             \
  __VA_ARGS__}

#define FtypeOpts(...) (ftype_t) {               \
  .name = "",                                    \
  .on_emptyline = NULL,                          \
  .shiftwidth = DEFAULT_SHIFTWIDTH,              \
  .tabwidth = TABWIDTH,                          \
  .autochdir = AUTOCHDIR,                        \
  .tab_indents = TAB_ON_INSERT_MODE_INDENTS,     \
  .clear_blanklines = CLEAR_BLANKLINES,          \
  .read_from_shell = 1,                          \
  .autoindent = NULL,                            \
  .on_open_fname_under_cursor = NULL,            \
  .balanced = NULL,                              \
  .cr_on_normal_is_like_insert_mode = CARRIAGE_RETURN_ON_NORMAL_IS_LIKE_INSERT_MODE,  \
  .backspace_on_normal_is_like_insert_mode = BACKSPACE_ON_NORMAL_IS_LIKE_INSERT_MODE,  \
  .backspace_on_normal_goes_up = BACKSPACE_ON_NORMAL_GOES_UP, \
  .backspace_on_insert_goes_up_and_join = BACKSPACE_ON_INSERT_GOES_UP_AND_JOIN, \
  .backspace_on_first_idx_remove_trailing_spaces = BACKSPACE_ON_FIRST_IDX_REMOVE_TRAILING_SPACES,  \
  .space_on_normal_is_like_insert_mode = SPACE_ON_NORMAL_IS_LIKE_INSERT_MODE,  \
  .small_e_on_normal_goes_insert_mode = SMALL_E_ON_NORMAL_GOES_INSERT_MODE,  \
  __VA_ARGS__ }

typedef struct screen_dim_opts {
  int
    first_row,
    first_col,
    last_row,
    num_rows,
    num_cols,
    init_term;
} screen_dim_opts;

#define ScreenDimOpts(...) (screen_dim_opts) { \
  .first_row = 1,                              \
  .first_col = 1,                              \
  .last_row = 0,                               \
  .num_rows = 0,                               \
  .num_cols = 0,                               \
  .init_term = 1,                              \
  __VA_ARGS__ }

typedef struct video_set_self {
  void (*row_with) (video_t *, int, char *);
} video_set_self;

typedef struct video_draw_self {
  void
    (*row_at) (video_t *, int),
    (*all) (video_t *);

  int
    (*bytes) (video_t *, char *, size_t);
} video_draw_self;

typedef struct video_self {
  video_set_self set;
  video_draw_self draw;

  video_t
    *(*new) (int, int, int, int, int);

  void
    (*free) (video_t *),
    (*flush) (video_t *, string_t *);
} video_self;

typedef struct video_T {
  video_self self;
} video_T;

typedef struct rline_set_self {
  void
    (*line) (rline_t *, char *, size_t),
    (*opts) (rline_t *, int),
    (*state) (rline_t *, int),
    (*opts_bit) (rline_t *, int),
    (*state_bit) (rline_t *, int),
    (*visibility) (rline_t *, int),
    (*prompt_char) (rline_t *, char),
    (*user_object) (rline_t *, void *);
} rline_set_self;

typedef struct rline_get_self {
  string_t
     *(*line) (rline_t *),
     *(*command) (rline_t *),
     *(*anytype_arg) (rline_t *, char *);

  Vstring_t *(*anytype_args) (rline_t *, char *);

  arg_t *(*arg) (rline_t *, int);

  int
    (*opts) (rline_t *),
    (*state) (rline_t *),
    (*buf_range) (rline_t *, buf_t *, int *);

  void *(*user_object) (rline_t *);

  Vstring_t *(*arg_fnames) (rline_t *, int);
} rline_get_self;

typedef struct rline_arg_self {
  int (*exists) (rline_t *, char *);
} rline_arg_self;

typedef struct rline_history_self {
  void (*push) (rline_t *);
} rline_history_self;

typedef struct rline_self {
  rline_get_self get;
  rline_set_self set;
  rline_arg_self arg;
  rline_history_self history;

  rline_t
    *(*edit) (rline_t *),
    *(*parse) (rline_t *, buf_t *);

  void
    (*free) (rline_t *),
    (*clear) (rline_t *),
    (*write_and_break) (rline_t *);

  int
    (*exec) (rline_t *, buf_t **);

} rline_self;

typedef struct rline_T {
  rline_self self;
} rline_T;

typedef struct msg_self {
  void
    (*write) (ed_t *, char *),
    (*write_fmt) (ed_t *, char *, ...),
    (*set) (ed_t *, int, int, char *, size_t),
    (*set_fmt) (ed_t *, int, int, char *, ...),
    (*line) (ed_t *, int, char *, ...),
    (*send) (ed_t *, int, char *),
    (*send_fmt) (ed_t *, int, char *, ...),
    (*error) (ed_t *, char *, ...);

  char *(*fmt) (ed_t *, int, ...);
} msg_self;

typedef struct msg_T {
  msg_self self;
} msg_T;

typedef struct error_self {
  char *(*string) (ed_t *, int);
} error_self;

typedef struct error_T {
  error_self self;
} error_T;

typedef struct buf_iter_self {
  void (*free) (buf_t *, bufiter_t *);

  bufiter_t
    *(*new)  (buf_t *, int),
    *(*next) (buf_t *, bufiter_t *);
} buf_iter_self;

typedef struct bufget_row_self {
  row_t
    *(*current) (buf_t *),
    *(*at) (buf_t *, int);

  string_t
     *(*current_bytes) (buf_t *),
     *(*bytes_at) (buf_t *, int);

  int
    (*col_idx) (buf_t *, row_t *);
} bufget_row_self;

typedef struct bufget_prop_self {
  int (*tabwidth) (buf_t *);
} bufget_prop_self;

typedef struct bufget_info_self {
  bufinfo_t *(*as_type) (buf_t *);
} bufget_info_self;

typedef struct buf_get_self {
  bufget_row_self row;
  bufget_prop_self prop;
  bufget_info_self info;

  char
    *(*contents) (buf_t *, int),
    *(*basename) (buf_t *),
    *(*fname) (buf_t *),
    *(*ftype_name) (buf_t *),
    *(*info_string) (buf_t *),
    *(*current_word) (buf_t *, char *, char *, int, int *, int *);

  size_t
    (*size) (buf_t *),
    (*num_lines) (buf_t *);

  string_t *(*shared_str) (buf_t *);

  row_t *(*line_at) (buf_t *, int);

  int
    (*flags) (buf_t *),
    (*current_col_idx) (buf_t *),
    (*current_row_idx) (buf_t *),
    (*current_video_row) (buf_t *),
    (*current_video_col) (buf_t *);

  win_t *(*my_parent) (buf_t *);
  ed_t  *(*my_root) (buf_t *);
} buf_get_self;

typedef struct bufset_as_self {
  void
    (*pager) (buf_t *),
    (*unnamed) (buf_t *),
    (*non_existant) (buf_t *);
} bufset_as_self;

typedef struct bufset_row_self {
  int (*idx) (buf_t *, int, int, int);
} bufset_row_self;

typedef struct bufset_normal_self {
  void
    (*at_beg_cb) (buf_t *, BufNormalBeg_cb),
    (*at_end_cb) (buf_t *, BufNormalEnd_cb);
} bufset_normal_self;

typedef struct buf_set_self {
  bufset_as_self as;
  bufset_row_self row;
  bufset_normal_self normal;

  int (*fname) (buf_t *, char *);

  void
    (*mode) (buf_t *, char *),
    (*ftype) (buf_t *, int),
    (*backup) (buf_t *, int, char *),
    (*modified) (buf_t *),
    (*autosave) (buf_t *, long),
    (*autochdir) (buf_t *, int),
    (*on_emptyline) (buf_t *, char *),
    (*video_first_row) (buf_t *, int),
    (*show_statusline) (buf_t *, int);
} buf_set_self;

typedef struct buf_syn_self {
  ftype_t *(*init) (buf_t *);
     char *(*parser) (buf_t *, char *, int, int, row_t *);
} buf_syn_self;

typedef struct buf_ftype_self {
  void (*free) (buf_t *);

  ftype_t
     *(*init) (buf_t *, int, FtypeAutoIndent_cb),
     *(*set)  (buf_t *, int, ftype_t);
} buf_ftype_self;

typedef struct buf_to_self {
  void (*video) (buf_t *);
} buf_to_self;

typedef struct buf_isit_self {
  int (*special_type) (buf_t *);
} buf_isit_self;

typedef struct buf_current_self {
  int
    (*set) (buf_t *, int);

  row_t
    *(*pop) (buf_t *),
    *(*delete) (buf_t *),
    *(*prepend) (buf_t *, row_t *),
    *(*append) (buf_t *, row_t *),
    *(*append_with) (buf_t *, char *),
    *(*append_with_len) (buf_t *, char *, size_t),
    *(*prepend_with) (buf_t *, char *),
    *(*replace_with) (buf_t *, char *);
} buf_current_self;

typedef struct buf_free_self {
  void
     (*line) (buf_t *),
     (*info) (buf_t *, bufinfo_t **),
     (*row) (buf_t *, row_t *),
     (*rows) (buf_t *);
} buf_free_self;

typedef struct buf_row_self {
  row_t
    *(*new_with) (buf_t *, const char *),
    *(*new_with_len) (buf_t *, const char *, size_t);
} buf_row_self;

typedef struct buf_read_self {
  ssize_t  (*fname) (buf_t *);
  int (*from_fp) (buf_t *, FILE *, fp_t *);
} buf_read_self;

typedef struct buf_Action_self {
  Action_t *(*new) (buf_t *);
  void
    (*free) (buf_t *, Action_t *),
    (*set_with) (buf_t *, Action_t *, int, int, char *, size_t),
    (*set_with_current) (buf_t *, Action_t *, int);
} buf_Action_self;

typedef struct buf_action_self {
  action_t
    *(*new) (buf_t *),
    *(*new_with) (buf_t *, int, int, char *, size_t);

  void
    (*free) (buf_t *, action_t *);
} buf_action_self;

typedef struct buf_undo_self {
  void
    (*free) (buf_t *),
    (*init) (buf_t *),
    (*push) (buf_t *, Action_t *),
    (*clear) (buf_t *);

  int
    (*exec) (buf_t *, utf8),
    (*insert) (buf_t *, Action_t *, action_t *),
    (*delete_line) (buf_t *, Action_t *, action_t *),
    (*replace_line) (buf_t *, Action_t *, action_t *);

  Action_t
    *(*pop) (buf_t *this);
} buf_undo_self;

typedef struct buf_redo_self {
  void
    (*push) (buf_t *, Action_t *);

  Action_t
    *(*pop) (buf_t *this);
} buf_redo_self;

typedef struct bufnormal_visual_self {
  int
    (*bw) (buf_t *),
    (*lw) (buf_t **),
    (*cw) (buf_t **);
} bufnormal_visual_self;

typedef struct bufnormal_handle_self {
  int
    (*g) (buf_t **, int),
    (*G) (buf_t *, int),
    (*d) (buf_t *, int, int),
    (*c) (buf_t **, int, int),
    (*W) (buf_t **),
    (*F) (buf_t **),
    (*L) (buf_t **),
    (*comma) (buf_t **),
    (*ctrl_w) (buf_t **);
} bufnormal_handle_self;

typedef struct buf_normal_self {
  bufnormal_handle_self handle;
  bufnormal_visual_self visual;

  int
    (*up) (buf_t *, int, int, int),
    (*bol) (buf_t *, int),
    (*eol) (buf_t *, int),
    (*bof) (buf_t *, int),
    (*eof) (buf_t *, int),
    (*put) (buf_t *, int, utf8),
    (*join) (buf_t *, int),
    (*yank) (buf_t *, int, int),
    (*Yank) (buf_t *, int, int),
    (*down) (buf_t *, int, int, int),
    (*left) (buf_t *, int, int),
    (*right) (buf_t *, int, int),
    (*delete) (buf_t *, int, int, int),
    (*noblnk) (buf_t *),
    (*page_up) (buf_t *, int, int),
    (*end_word) (buf_t *, int, int, int),
    (*page_down) (buf_t *, int, int),
    (*delete_eol) (buf_t *, int, int),
    (*change_case) (buf_t *),
    (*goto_linenr) (buf_t *, int, int),
    (*replace_character) (buf_t *),
    (*replace_character_with) (buf_t *, utf8);
} buf_normal_self;

typedef struct buf_insert_self {
  int
    (*mode) (buf_t **, utf8, char *),
    (*string) (buf_t *, char *, size_t, int),
    (*new_line) (buf_t **, utf8);
} buf_insert_self;

typedef struct buf_delete_self {
  int
    (*word) (buf_t *, int),
    (*line) (buf_t *, int, int);
} buf_delete_self;

typedef struct buf_jump_self {
  void (*push) (buf_t *, mark_t *);
  int (*to) (buf_t *, int);
} buf_jump_self;

typedef struct buf_jumps_self {
  void
     (*free) (buf_t *),
     (*init) (buf_t *);
} buf_jumps_self;

typedef struct buf_mark_self {
  int
    (*set) (buf_t *, int),
    (*jump) (buf_t *),
    (*restore) (buf_t *, mark_t *);
} buf_mark_self;

typedef struct buf_adjust_self {
  void
    (*view) (buf_t *),
    (*marks) (buf_t *, int, int, int);

  int (*col) (buf_t *, int, int);
} buf_adjust_self;

typedef struct buf_self {
  buf_current_self current;
  buf_to_self to;
  buf_set_self set;
  buf_get_self get;
  buf_syn_self syn;
  buf_row_self row;
  buf_isit_self isit;
  buf_free_self free;
  buf_undo_self undo;
  buf_redo_self redo;
  buf_read_self read;
  buf_iter_self iter;
  buf_mark_self mark;
  buf_jump_self jump;
  buf_jumps_self jumps;
  buf_ftype_self ftype;
  buf_Action_self Action;
  buf_action_self action;
  buf_normal_self normal;
  buf_delete_self delete;
  buf_insert_self insert;
  buf_adjust_self adjust;

  void
    (*draw) (buf_t *),
    (*flush) (buf_t *),
    (*clear) (buf_t *),
    (*draw_current_row) (buf_t *);

  char
    *(*info) (buf_t *);

  int
    (*rline) (buf_t **, rline_t *),
    (*write) (buf_t *, int),
    (*search) (buf_t *, char, char *, utf8),
    (*indent) (buf_t *, int, utf8),
    (*substitute) (buf_t *, char *, char *, int, int, int, int),
    (*backupfile) (buf_t *);

  ssize_t (*init_fname) (buf_t *, char *);

  row_t *(*append_with) (buf_t *, char *);
  string_t *(*input_box) (buf_t *, int, int, int, char *);
} buf_self;

typedef struct buf_T {
  buf_self self;
} buf_T;

typedef struct win_adjust_self {
  void (*buf_dim) (win_t *);
} win_adjust_self;

typedef struct win_set_self {
  void
    (*previous_idx) (win_t *,  int),
    (*dim) (win_t *, dim_t *, int, int, int, int),
    (*num_frames) (win_t *, int),
    (*min_rows) (win_t *, int),
    (*has_dividers) (win_t *, int),
    (*video_dividers) (win_t *);

  buf_t *(*current_buf) (win_t*, int, int);
} win_set_self;

typedef struct winget_info_self {
  wininfo_t *(*as_type) (win_t *);
} winget_info_self;

typedef struct win_get_self {
  winget_info_self info;

  buf_t
    *(*current_buf) (win_t *),
    *(*buf_head) (win_t *),
    *(*buf_by_idx) (win_t *, int),
    *(*buf_next) (win_t *, buf_t *),
    *(*buf_by_name) (win_t *, const char *, int *);

  int
    (*num_cols) (win_t *),
    (*num_rows) (win_t *),
    (*num_buf) (win_t *),
    (*current_buf_idx) (win_t *);
} win_get_self;

typedef struct win_pop_self {
  int (*current_buf) (win_t *);
} win_pop_self;

typedef struct win_buf_self {
  buf_t
    *(*init) (win_t *, int, int),
    *(*new) (win_t *, buf_opts);
} win_buf_self;

typedef struct win_frame_self {
  buf_t
    *(*change) (win_t *, int, int);
} win_frame_self;

typedef struct win_isit_self {
  int (*special_type) (win_t *);
} win_isit_self;

typedef struct win_self {
  win_set_self set;
  win_get_self get;
  win_isit_self isit;
  win_pop_self pop;
  win_adjust_self adjust;
  win_buf_self buf;
  win_frame_self frame;

  void
    (*free_info) (win_t *, wininfo_t **),
    (*draw) (win_t *);

  int
    (*edit_fname)    (win_t *, buf_t **, char *, int, int, int, int),
    (*append_buf)    (win_t *, buf_t *),
    (*prepend_buf)   (win_t *, buf_t *),
    (*insert_buf_at) (win_t *, buf_t *, int);

  dim_t
     **(*dim_calc) (win_t *);
} win_self;

typedef struct win_T {
  win_self self;
} win_T;

typedef struct edget_info_self {
  edinfo_t *(*as_type) (ed_t *);
} edget_info_self;

typedef struct ed_get_self {
  ed_t *(*self) (ed_t *);

  edget_info_self info;

  buf_t
    *(*bufname) (ed_t *, char *),
    *(*current_buf) (ed_t *),
    *(*scratch_buf) (ed_t *);

  int
    (*state) (ed_t *),
    (*num_win) (ed_t *, int),
    (*min_rows) (ed_t *),
    (*current_win_idx) (ed_t *),
    (*num_special_win) (ed_t *),
    (*num_rline_commands) (ed_t *);

  win_t
    *(*current_win) (ed_t *),
    *(*win_head) (ed_t *),
    *(*win_next) (ed_t *, win_t *),
    *(*win_by_idx) (ed_t *, int),
    *(*win_by_name) (ed_t *, char *, int *);

  term_t *(*term) (ed_t *);

  string_t *(*topline) (ed_t *);

  video_t *(*video) (ed_t *);

  void *(*callback_fun) (ed_t *, char *);
} ed_get_self;

typedef struct ed_unset_self {
  void
    (*state_bit) (ed_t *, int);
} ed_unset_self;

typedef struct ed_test_self {
  int
    (*state_bit) (ed_t *, int);
} ed_test_self;

typedef struct ed_set_self {
  void
    (*state) (ed_t *, int),
    (*state_bit) (ed_t *, int),
    (*exit_quick) (ed_t *, int),
    (*screen_size) (ed_t *, screen_dim_opts),
    (*topline) (ed_t *, buf_t *),
    (*rline_cb) (ed_t *, Rline_cb),
    (*lang_map) (ed_t *, int[][26]),
    (*record_cb) (ed_t *, Record_cb),
    (*at_exit_cb) (ed_t *, EdAtExit_cb),
    (*i_record_cb) (ed_t *, IRecord_cb),
    (*expr_reg_cb) (ed_t *, ExprRegister_cb),
    (*init_record_cb) (ed_t *, InitRecord_cb),
    (*on_normal_g_cb)  (ed_t *, BufNormalOng_cb),
    (*word_actions) (ed_t *, utf8 *, int, char *, WordActions_cb),
    (*cw_mode_actions) (ed_t *, utf8 *, int, char *, VisualCwMode_cb),
    (*lw_mode_actions) (ed_t *, utf8 *, int, char *, VisualLwMode_cb),
    (*line_mode_actions) (ed_t *, utf8 *, int, char *, LineMode_cb),
    (*file_mode_actions) (ed_t *, utf8 *, int, char *, FileActions_cb);

  win_t *(*current_win) (ed_t *, int);
  dim_t *(*dim) (ed_t *, dim_t *, int, int, int, int);
} ed_set_self;

typedef struct ed_syn_self {
  void (*append) (ed_t *, syn_t);
  int  (*get_ftype_idx) (ed_t *, char *);
} ed_syn_self;

typedef struct ed_reg_self {
  Reg_t
     *(*set) (ed_t *, int, int, char *, int),
     *(*setidx) (ed_t *, int, int, char *, int);
} ed_reg_self;

typedef struct ed_append_self {
  int (*win) (ed_t *, win_t *);

  void
    (*message_fmt) (ed_t *, char *, ...),
    (*message) (ed_t *, char *),
    (*toscratch_fmt) (ed_t *, int, char *, ...),
    (*toscratch) (ed_t *, int, char *),
    (*command_arg) (ed_t *, char *, char *, size_t),
    (*rline_commands) (ed_t *, char **, int, int[], int[]),
    (*rline_command) (ed_t *, char *, int, int);
} ed_append_self;

typedef struct ed_readjust_self {
  void (*win_size) (ed_t *, win_t *);
} ed_readjust_self;

typedef struct ed_rline_self {
  rline_t
    *(*new) (ed_t *),
    *(*new_with) (ed_t *, char *);
} ed_rline_self;

typedef struct ed_buf_self {
  int (*change) (ed_t  *, buf_t **, char *, char *);
  buf_t *(*get) (ed_t  *, char *, char *);
} ed_buf_self;

typedef struct ed_win_self {
  win_t
    *(*new) (ed_t *, char *, int),
    *(*init) (ed_t *, char *, WinDimCalc_cb),
    *(*new_special) (ed_t *, char *, int);
  int (*change) (ed_t *, buf_t **, int, char *, int, int);
} ed_win_self;

typedef struct ed_menu_self {
   void (*free) (ed_t *, menu_t *);
   menu_t *(*new) (ed_t *, buf_t *, MenuProcessList_cb);
   char *(*create) (ed_t *, menu_t *);
} ed_menu_self;

typedef struct ed_sh_self {
  int (*popen) (ed_t *, buf_t *, char *, int, int, ProcRead_cb);
} ed_sh_self;

typedef struct edhistory_set_self {
  string_t
    *(*rline_file) (ed_t *, char *),
    *(*search_file) (ed_t *, char *);
} edhistory_set_self;

typedef struct edhistory_get_self {
  string_t
    *(*rline_file) (ed_t *),
    *(*search_file) (ed_t *);
} edhistory_get_self;

typedef struct ed_history_self {
  edhistory_set_self set;
  edhistory_get_self get;

  void
    (*read) (ed_t *),
    (*write) (ed_t *),
    (*add) (ed_t *, char *, size_t),
    (*add_lines) (ed_t *, Vstring_t *, int);
} ed_history_self;

typedef struct ed_draw_self {
  void (*current_win) (ed_t *);
} ed_draw_self;

typedef struct ed_self {
  ed_sh_self sh;
  ed_buf_self buf;
  ed_win_self win;
  ed_set_self set;
  ed_get_self get;
  ed_syn_self syn;
  ed_reg_self reg;
  ed_test_self test;
  ed_menu_self menu;
  ed_draw_self draw;
  ed_unset_self unset;
  ed_rline_self rline;
  ed_append_self append;
  ed_history_self history;
  ed_readjust_self readjust;

  void
    (*free) (ed_t *),
    (*free_info) (ed_t *, edinfo_t **),
    (*record) (ed_t *, char *, ...),
    (*suspend) (ed_t *),
    (*resume) (ed_t *),
    (*deinit_commands) (ed_t *);

  int
    (*check_sanity) (ed_t *),
    (*quit) (ed_t *, int, int),
    (*delete) (ed_t *, ed_T *, int, int),
    (*scratch) (ed_t *, buf_t **, int),
    (*messages) (ed_t *, buf_t **, int);

  utf8 (*question) (ed_t *, char *, utf8 *, int);

  dim_t
     **(*dim_calc) (ed_t *, int, int, int, int),
     **(*dims_init) (ed_t *, int);
} ed_self;

typedef struct ed_T {
  ed_prop *prop;
  ed_self  self;
  buf_T __Buf__;
  win_T __Win__;
  i_T __I__;

  msg_T __Msg__;
  rline_T __Rline__;
  video_T __Video__;
  error_T __Error__;
} ed_T;

typedef struct E_unset_self {
  void
    (*state_bit) (E_T *, int);
} E_unset_self ;

typedef struct E_test_self {
  int
    (*state_bit) (E_T *, int);
} E_test_self;

typedef struct E_set_self {
  void
    (*state) (E_T *, int),
    (*state_bit) (E_T *, int),
    (*save_image) (E_T *, int),
    (*image_name) (E_T *, char *),
    (*image_file) (E_T *, char *),
    (*at_exit_cb) (E_T *, EAtExit_cb),
    (*at_init_cb) (E_T *, EdAtInit_cb),
    (*persistent_layout) (E_T *, int);

  int (*i_dir) (E_T *, char *);

  ed_t
    *(*next) (E_T *),
    *(*prev) (E_T *),
    *(*current) (E_T *, int);
} E_set_self;

typedef struct E_get_self {
  ed_t
    *(*head) (E_T *),
    *(*current) (E_T *),
    *(*next) (E_T *, ed_t *);

  int
    (*num) (E_T *),
    (*idx) (E_T *, ed_t *),
    (*state) (E_T *),
    (*prev_idx) (E_T *),
    (*error_state) (E_T *),
    (*current_idx) (E_T *);

  term_t *(*term) (E_T *);

  string_t *(*env) (E_T *, char *);

  IDefineFuns_cb (*i_define_funs_cb) (E_T *);

  i_T *(*iclass) (E_T *);
} E_get_self;

typedef struct E_self {
  E_get_self get;
  E_set_self set;
  E_test_self test;
  E_unset_self unset;

  ed_t
    *(*init) (E_T *, ed_opts),
    *(*new) (E_T *, ed_opts);

  int
    (*main) (E_T *, buf_t *),
    (*delete) (E_T *, int, int),
    (*exit_all) (E_T *),
    (*load_file) (E_T *, char *),
    (*save_image) (E_T *, char *);

  string_t *(*create_image) (E_T *);
} E_self;

typedef struct this_T {
  void *self;
  void *prop;
  E_T *__E__;
} this_T;

typedef struct E_T {
  E_self self;
  E_prop *prop;
  ed_T *__Ed__;
  this_T *__This__;
} E_T;

public E_T *__init_ed__ (char *);
public void __deinit_ed__ (E_T **);

public mutable size_t tostderr (char *);
public mutable size_t tostdout (char *);

#endif /* LIBVED_H */
