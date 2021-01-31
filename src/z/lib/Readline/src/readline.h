#ifndef READLINE_HDR
#define READLINE_HDR

#define READLINE_HISTORY_NUM_ENTRIES 20
#define READLINE_LAST_COMPONENT_NUM_ENTRIES 10

#define READLINE_PROMPT_CHAR ':'
#define COLOR_PROMPT COLOR_YELLOW

#define READLINE_NUM_USER_DATA 3
#define READLINE_MAXLEN_COM    512

#define READLINE_ERROR                             -20
#define READLINE_NO_COMMAND                        -21
#define READLINE_ARG_AWAITING_STRING_OPTION_ERROR  -22
#define READLINE_ARGUMENT_MISSING_ERROR            -23
#define READLINE_UNTERMINATED_QUOTED_STRING_ERROR  -24
#define READLINE_UNRECOGNIZED_OPTION               -25

#define READLINE_ARG_FILENAME                     (1 << 0)
#define READLINE_ARG_RANGE                        (1 << 1)
#define READLINE_ARG_GLOBAL                       (1 << 2)
#define READLINE_ARG_PATTERN                      (1 << 3)
#define READLINE_ARG_SUB                          (1 << 4)
#define READLINE_ARG_INTERACTIVE                  (1 << 5)
#define READLINE_ARG_APPEND                       (1 << 6)
#define READLINE_ARG_BUFNAME                      (1 << 7)
#define READLINE_ARG_VERBOSE                      (1 << 8)
#define READLINE_ARG_ANYTYPE                      (1 << 9)
#define READLINE_ARG_RECURSIVE                    (1 << 10)

#define READLINE_OK                               (1 << 0)
#define READLINE_CONTINUE                         (1 << 1)
#define READLINE_BREAK                            (1 << 2)
#define READLINE_PROCESS_CHAR                     (1 << 3)
#define READLINE_INSERT_CHAR                      (1 << 4)
#define READLINE_CLEAR                            (1 << 5)
#define READLINE_WRITE                            (1 << 6)
#define READLINE_IS_VISIBLE                       (1 << 7)
#define READLINE_CURSOR_HIDE                      (1 << 8)
#define READLINE_CLEAR_FREE_LINE                  (1 << 9)
#define READLINE_POST_PROCESS                     (1 << 10)
#define READLINE_SET_POS                          (1 << 11)
#define READLINE_EXEC                             (1 << 12)
#define READLINE_FIRST_CHAR_COMPLETION            (1 << 13)

#define READLINE_OPT_HAS_TAB_COMPLETION           (1 << 0)
#define READLINE_OPT_HAS_HISTORY_COMPLETION       (1 << 1)
#define READLINE_OPT_RETURN_AFTER_TAB_COMPLETION  (1 << 2)

#define READLINE_LAST_ARG_KEY    037

#define READLINE_TOK_COMMAND      (1 << 0)
#define READLINE_TOK_ARG          (1 << 1)
#define READLINE_TOK_ARG_SHORT    (1 << 2)
#define READLINE_TOK_ARG_LONG     (1 << 3)
#define READLINE_TOK_ARG_OPTION   (1 << 4)
#define READLINE_TOK_ARG_FILENAME (1 << 5)

typedef struct readline_hist_item_t readline_hist_item_t;
typedef struct readline_hist_t readline_hist_t;
typedef struct readline_arg_t readline_arg_t;
typedef struct readline_com_t readline_com_t;
typedef struct readline_t readline_t;

typedef utf8 (*InputGetch_cb) (int);
typedef int  (*ReadlineExec_cb) (readline_t *);
typedef int  (*ReadlineAtBeg_cb) (readline_t **);
typedef int  (*ReadlineAtEnd_cb) (readline_t **);
typedef int  (*ReadlineTabCompletion_cb) (readline_t *);
typedef int  (*ReadlineCalcColumns_cb) (readline_t *rl, int);
typedef void (*ReadlineLastComponentPush_cb) (readline_t *rl);

struct readline_hist_item_t {
  readline_hist_item_t *next;
  readline_hist_item_t *prev;
  readline_t *data;
};

struct readline_hist_t {
  readline_hist_item_t *head;
  readline_hist_item_t *tail;
  readline_hist_item_t *current;
  int num_items;
  int cur_idx;
  int history_idx;
};

struct readline_arg_t {
  int type;
  string_t
    *argname,
    *argval;

  readline_arg_t *next;
  readline_arg_t *prev;
};

struct readline_com_t {
  char *com;
  char **args;
  int  num_args;
};

struct readline_t {
  char prompt_char;

  int
    com,
    opts,
    rows,
    state,
    range[2],
    row_pos,
    num_cols,
    num_rows,
    first_row,
    first_col,
    prompt_row,
    trigger_first_char_completion;

  utf8
    c,
    first_chars[8];

  int first_chars_len;

  term_t *term;

  int fd;

  video_t *video;

  Vstring_t *line;

  readline_arg_t *head;
  readline_arg_t *tail;
  readline_arg_t *current;
  int cur_idx;
  int num_items;

  string_t *render;

  readline_com_t **commands;
  int commands_len;

  Vstring_t *last_component;
  readline_hist_t  *history;
  int max_num_hist_entries;

  void *object;
  void *user_data[READLINE_NUM_USER_DATA];

  InputGetch_cb getch;
  ReadlineExec_cb exec;
  ReadlineAtBeg_cb at_beg;
  ReadlineAtEnd_cb at_end;
  ReadlineCalcColumns_cb calc_columns;
  ReadlineTabCompletion_cb tab_completion;
  ReadlineLastComponentPush_cb last_component_push;
};

typedef struct readline_set_self {
  void
    (*line) (readline_t *, char *, size_t),
    (*opts) (readline_t *, int),
    (*state) (readline_t *, int),
    (*opts_bit) (readline_t *, int),
    (*state_bit) (readline_t *, int),
    (*visibility) (readline_t *, int),
    (*prompt_char) (readline_t *, char);
} readline_set_self;

typedef struct readline_get_self {
  string_t
     *(*line) (readline_t *),
     *(*command) (readline_t *),
     *(*anytype_arg) (readline_t *, char *);

  Vstring_t *(*anytype_args) (readline_t *, char *);

  readline_arg_t *(*arg) (readline_t *, int);

  int
    (*opts) (readline_t *),
    (*state) (readline_t *);

  Vstring_t *(*arg_fnames) (readline_t *, int);
} readline_get_self;

typedef struct readline_arg_self {
  int (*exists) (readline_t *, char *);
} readline_arg_self;

typedef struct readline_history_self {
  void (*push) (readline_t *);
} readline_history_self;

typedef struct readline_self {
  readline_get_self get;
  readline_set_self set;
  readline_arg_self arg;
  readline_history_self history;

  readline_t
    *(*new) (void *, term_t *, InputGetch_cb, int, int, int, video_t *),
    *(*edit) (readline_t *),
    *(*parse) (readline_t *);

  void
    (*clear) (readline_t *),
    (*release) (readline_t *),
    (*write_and_break) (readline_t *),
    (*last_component_push) (readline_t *);

  int (*exec) (readline_t *);

  vstring_t *(*parse_command) (readline_t *);
} readline_self;

typedef struct readline_T {
  readline_self self;
}readline_T;

public readline_T __init_readline__ (void);
#endif /* READLINE_HDR */
