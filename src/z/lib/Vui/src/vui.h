#ifndef VUI_HDR
#define VUI_HDR

typedef struct vui_prop vui_prop;
typedef struct menu_t menu_t;

typedef int (*MenuProcessList_cb) (menu_t *);

#define MENU_NUM_USER_DATA 2
#define READLINE_MENU_USER_DATA_IDX  2

#define MENU_INIT              (1 << 0)
#define MENU_QUIT              (1 << 1)
#define MENU_INSERT            (1 << 2)
#define MENU_REINIT_LIST       (1 << 3)
#define MENU_FINALIZE          (1 << 4)
#define MENU_DONE              (1 << 5)
#define MENU_LIST_IS_ALLOCATED (1 << 6)
#define MENU_REDO              (1 << 7)

struct menu_t {
  char pat[MAXLEN_PATTERN + 1];

  utf8 c;

  int
    fd,
    state,
    patlen,
    retval,
    last_col,
    row_pos,
    col_pos,
    next_key,
    num_cols,
    num_rows,
    last_row,
    first_col,
    first_row,
    prompt_row,
    space_selects,
    min_first_row,
    orig_num_rows,
    orig_first_row,
    return_if_one_item,
    clear_and_continue_on_backspace;

  string_t  *header;
  Vstring_t *list;

  video_t *video;
  term_t  *term;

  MenuProcessList_cb process_list;
  IOGetkey getch;
  void *user_data[MENU_NUM_USER_DATA];
};

typedef struct menu_opts {
  int fd;
  int first_row;
  int last_row;
  int prompt_row;
  int first_col;
  int num_cols;
  int space_selects;
  int return_if_one_item;
  int clear_and_continue_on_backspace;
  int next_key;
  char *pat;
  size_t patlen;
  video_t *video;
  term_t  *term;
  MenuProcessList_cb process_list_cb;
  IOGetkey getch_cb;
  void *user_data_first;
  void *user_data_second;
} menu_opts;

#define MenuOpts(...) (menu_opts) {     \
  .fd = STDOUT_FILENO,                  \
  .first_row = 1,                       \
  .last_row  = 24,                      \
  .prompt_row = 23,                     \
  .first_col = 1,                       \
  .num_cols = 78,                       \
  .space_selects = 1,                   \
  .return_if_one_item = 1,              \
  .clear_and_continue_on_backspace = 0, \
  .next_key = '\t',                     \
  .pat = NULL,                          \
  .patlen = 0,                          \
  .video = NULL,                        \
  .term = NULL,                         \
  .process_list_cb = NULL,              \
  .getch_cb = IO.getkey,                \
  .user_data_first = NULL,              \
  .user_data_second = NULL,             \
  __VA_ARGS__                           \
}

typedef struct vui_menu_self {
  menu_t *(*new) (menu_opts);

  void
    (*clear) (menu_t *),
    (*release) (menu_t *),
    (*release_list) (menu_t *);

  char *(*create) (menu_t *);
} vui_menu_self;

typedef struct vui_self {
  vui_menu_self menu;
} vui_self;

typedef struct vui_T {
  vui_self self;
  vui_prop *prop;
} vui_T;

public vui_T __init_vui__ (void);
public void  __deinit_vui__ (vui_T *);

#endif /* VUI_HDR */
