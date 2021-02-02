
#define MAX_COUNT_DIGITS 8

#define VED_WIN_NORMAL_TYPE  0
#define VED_WIN_SPECIAL_TYPE 1

#define NORMAL_MODE     "normal"
#define INSERT_MODE     "insert"
#define VISUAL_MODE_LW  "visual lw"
#define VISUAL_MODE_CW  "visual cw"
#define VISUAL_MODE_BW  "visual bw"

#define VED_SCRATCH_WIN "scratch"
#define VED_SCRATCH_BUF "[scratch]"
#define VED_MSG_WIN     "message"
#define VED_MSG_BUF     "[messages]"
#define VED_SEARCH_WIN  "search"
#define VED_SEARCH_BUF  "[search]"
#define VED_DIFF_WIN    "diff"
#define VED_DIFF_BUF    "[diff]"

#define CASE_A ",[]()+-:;}{<>_"
#define CASE_B ".][)(-+;:{}><-"

#define MENU_INIT (1 << 0)
#define MENU_QUIT (1 << 1)
#define MENU_INSERT (1 << 2)
#define MENU_REINIT_LIST (1 << 3)
#define MENU_FINALIZE (1 << 4)
#define MENU_DONE (1 << 5)
#define MENU_LIST_IS_ALLOCATED (1 << 6)
#define MENU_REDO (1 << 7)

#define RL_TOK_COMMAND (1 << 0)
#define RL_TOK_ARG (1 << 1)
#define RL_TOK_ARG_SHORT (1 << 2)
#define RL_TOK_ARG_LONG (1 << 3)
#define RL_TOK_ARG_OPTION (1 << 4)
#define RL_TOK_ARG_FILENAME (1 << 5)

#define ACCEPT_TAB_WHEN_INSERT (1 << 0)

#define WIN_NUM_FRAMES(w_) w_->prop->num_frames
#define WIN_LAST_FRAME(w_) WIN_NUM_FRAMES(w_) - 1
#define WIN_CUR_FRAME(w_) w_->prop->cur_frame

/* buf is already open 
  instances {...} */

#define VUNDO_RESET (1 << 0)

enum {
  VED_COM_BUF_BACKUP = 0,
  VED_COM_BUF_CHANGE_NEXT,
  VED_COM_BUF_CHANGE_NEXT_ALIAS,
  VED_COM_BUF_CHANGE_PREV_FOCUSED,
  VED_COM_BUF_CHANGE_PREV_FOCUSED_ALIAS,
  VED_COM_BUF_CHANGE_PREV,
  VED_COM_BUF_CHANGE_PREV_ALIAS,
  VED_COM_BUF_CHECK_BALANCED,
  VED_COM_BUF_DELETE_FORCE,
  VED_COM_BUF_DELETE_FORCE_ALIAS,
  VED_COM_BUF_DELETE,
  VED_COM_BUF_DELETE_ALIAS,
  VED_COM_BUF_CHANGE,
  VED_COM_BUF_CHANGE_ALIAS,
  VED_COM_BUF_SET,
  VED_COM_DIFF_BUF,
  VED_COM_DIFF,
  VED_COM_EDIT_FORCE,
  VED_COM_EDIT_FORCE_ALIAS,
  VED_COM_EDIT,
  VED_COM_EDIT_ALIAS,
  VED_COM_EDIT_IMAGE,
  VED_COM_EDNEW,
  VED_COM_ENEW,
  VED_COM_EDNEXT,
  VED_COM_EDPREV,
  VED_COM_EDPREV_FOCUSED,
  VED_COM_ETAIL,
  VED_COM_GREP,
  VED_COM_MESSAGES,
  VED_COM_QUIT_FORCE,
  VED_COM_QUIT_FORCE_ALIAS,
  VED_COM_QUIT,
  VED_COM_QUIT_ALIAS,
  VED_COM_READ,
  VED_COM_READ_ALIAS,
  VED_COM_READ_SHELL,
  VED_COM_SCRATCH,
  VED_COM_REDRAW,
  VED_COM_SEARCHES,
  VED_COM_SHELL,
  VED_COM_SPLIT,
  VED_COM_SUBSTITUTE,
  VED_COM_SUBSTITUTE_WHOLE_FILE_AS_RANGE,
  VED_COM_SUBSTITUTE_ALIAS,
  VED_COM_SAVE_IMAGE,
  VED_COM_TEST_KEY,
  VED_COM_VALIDATE_UTF8,
  VED_COM_WIN_CHANGE_NEXT,
  VED_COM_WIN_CHANGE_NEXT_ALIAS,
  VED_COM_WIN_CHANGE_PREV_FOCUSED,
  VED_COM_WIN_CHANGE_PREV_FOCUSED_ALIAS,
  VED_COM_WIN_CHANGE_PREV,
  VED_COM_WIN_CHANGE_PREV_ALIAS,
  VED_COM_WRITE_FORCE,
  VED_COM_WRITE_FORCE_ALIAS,
  VED_COM_WRITE_FORCE_FORCE,
  VED_COM_WRITE_FORCE_FORCE_ALIAS,
  VED_COM_WRITE,
  VED_COM_WRITE_ALIAS,
  VED_COM_WRITE_QUIT_FORCE,
  VED_COM_WRITE_QUIT,
  VED_COM_END,
};

#define NUM_RECORDS    3

#define MARKS          "`abcdghjklqwertyuiopzxcvbnm1234567890"
#define NUM_MARKS      37
#define MARK_UNNAMED   0

/* this produce wrong results when compiled with tcc
  char *r = strchr (REGISTERS, register); r - REGISTERS;
 */

#define REGISTERS \
"\"/:%*+=abcdghjklqwertyuiopzxsvbnm1234567890ABCDGHJKLQWERTYUIOPZXSVBNM^`_-\n"
#define NUM_REGISTERS  74

#define REG_CURWORD_CHR '^'
#define REG_SHARED_CHR  '`'

enum {
  REG_UNNAMED = 0,
  REG_SEARCH,
  REG_PROMPT,
  REG_FNAME,
  REG_STAR,
  REG_PLUS,
  REG_EXPR,
  REG_CURWORD = NUM_REGISTERS - 5,
  REG_SHARED,
  REG_BLACKHOLE,
  REG_INTERNAL,
  REG_RDONLY
};

#define SYS_ERRORS "\
1:  EPERM     Operation not permitted,\
2:  ENOENT    No such file or directory,\
3:  ESRCH     No such process,\
4:  EINTR     Interrupted system call,\
5:  EIO       I/O error,\
6:  ENXIO     No such device or address,\
7:  E2BIG     Argument list too long,\
8:  ENOEXEC   Exec format error,\
9:  EBADF     Bad file number,\
10: ECHILD    No child processes,\
11: EAGAIN    Try again,\
12: ENOMEM    Out of memory,\
13: EACCES    Permission denied,\
14: EFAULT    Bad address,\
15: ENOTBLK   Block device required,\
16: EBUSY     Device or resource busy,\
17: EEXIST    File exists,\
18: EXDEV     Cross-device link,\
19: ENODEV    No such device,\
20: ENOTDIR   Not a directory,\
21: EISDIR    Is a directory,\
22: EINVAL    Invalid argument,\
23: ENFILE    File table overflow,\
24: EMFILE    Too many open files,\
25: ENOTTY    Not a typewriter,\
26: ETXTBSY   Text file busy,\
27: EFBIG     File too large,\
28: ENOSPC    No space left on device,\
29: ESPIPE    Illegal seek,\
30: EROFS     Read-only file system,\
31: EMLINK    Too many links,\
32: EPIPE     Broken pipe,\
33: EDOM      Math argument out of domain of func,\
34: ERANGE    Math result not representable,\
35: EDEADLK   Resource deadlock would occur,\
36: ENAMETOOLONG File name too long,\
37: ENOLCK    No record locks available,\
38: ENOSYS    Invalid system call number,\
39: ENOTEMPTY Directory not empty,\
40: ELOOP     Too many symbolic links encountered"

#define ED_ERRORS "\
-3: RE_UNBALANCHED_BRACKETS_ERROR Unbalanced brackets in the pattern,\
-22: RL_ARG_AWAITING_STRING_OPTION_ERROR Awaiting a string after =,\
-23: RL_ARGUMENT_MISSING_ERROR Awaiting argument after dash,\
-24: RL_UNTERMINATED_QUOTED_STRING_ERROR Quoted String is unterminated,\
-25: RL_UNRECOGNIZED_OPTION Unrecognized option,\
-1000: INDEX_ERROR Index is out of range,\
-1001: NULL_PTR_ERROR NULL Pointer,\
-1002: INTEGEROVERFLOW_ERROR Integer overflow"

enum {
  MSG_FILE_EXISTS_AND_NO_FORCE = 1,
  MSG_FILE_EXISTS_BUT_IS_NOT_A_REGULAR_FILE,
  MSG_FILE_EXISTS_BUT_IS_AN_OBJECT_FILE,
  MSG_FILE_EXISTS_AND_IS_A_DIRECTORY,
  MSG_FILE_IS_NOT_READABLE,
  MSG_FILE_IS_LOADED_IN_ANOTHER_BUFFER,
  MSG_FILE_REMOVED_FROM_FILESYSTEM,
  MSG_FILE_HAS_BEEN_MODIFIED,
  MSG_BUF_IS_READ_ONLY,
  MSG_ON_WRITE_BUF_ISNOT_MODIFIED_AND_NO_FORCE,
  MSG_ON_BD_IS_MODIFIED_AND_NO_FORCE,
  MSG_CAN_NOT_WRITE_AN_UNNAMED_BUFFER,
  MSG_CAN_NOT_DETERMINATE_CURRENT_DIR
};

#define ED_MSGS_FMT "\
1:file %s: exists, use w! to overwrite.\
2:file %s: exists but is not a regular file.\
3:file %s: exists but is an object (elf) file.\
4:file %s: exists and is a directory.\
5:file %s: is not readable.\
6:file %s: is loaded in another buffer.\
7:[Warning]%s: removed from filesystem since last operation.\
8:[Warning]%s: has been modified since last operation.\
9:buffer is read only.\
10:buffer has not been modified, use w! to force.\
11:buffer has been modified, use bd! to delete it without writing.\
12:can not write an unnamed buffer.\
13:can not get current directory!!!."

#define MSG_ERRNO(errno__) \
  Msg.error ($my(root), Error.string ($my(root), errno__))

#define VED_MSG_ERROR(err__, ...) \
  Msg.error ($my(root), Msg.fmt ($my(root), err__, ##__VA_ARGS__))

#define MSG(fmt, ...) \
  Msg.set_fmt ($my(root), COLOR_NORMAL, MSG_SET_DRAW, fmt, ##__VA_ARGS__)

#define MSG_ERROR(fmt, ...) \
  Msg.error ($my(root), fmt, ##__VA_ARGS__)

#define MY_PROPERTIES            \
  dim_t *dim;                    \
  video_t *video;                \
    int  num_items

#define MY_CLASSES(__me__)       \
  E_T *__E__;                    \
  __me__ ## _T *Me;              \
  msg_T *__Msg__;                \
  error_T *__Error__

typedef struct menu_t {
  char pat[MAXLEN_PAT];

  int
    patlen,
    fd,
    next_key,
    num_cols,
    num_rows,
    first_row,
    last_row,
    first_col,
    last_col,
    row_pos,
    col_pos,
    min_first_row,
    space_selects,
    return_if_one_item,
    clear_and_continue_on_backspace,
    state,
    orig_first_row,
    orig_num_rows,
    retval;

  utf8 c;

  buf_t *this;

  string_t *header;
  Vstring_t  *list;
  video_t *cur_video;

  int (*process_list) (menu_t *);
} menu_t;

typedef struct reg_t {
  string_t *data;
  reg_t *next;
  reg_t *prev;
} reg_t;

typedef struct Reg_t {
  reg_t *head;
  char reg;
  int  type;
  int  cur_col_idx;
  int  first_col_idx;
  int  col_pos;
} Reg_t;

typedef struct mark_t {
  char mark;
  int
    idx,
    cur_idx,
    cur_col_idx,
    first_col_idx,
    row_pos,
    col_pos,
    video_first_row_idx;

  row_t *video_first_row;
} mark_t;

typedef struct jump_t jump_t;
struct jump_t {
  jump_t *next;
  mark_t *mark;
};

typedef struct Jump_t {
  jump_t *head;
  int num_items;
  int cur_idx;
  int old_idx;
} Jump_t;

typedef struct action_t {
  action_t *next;
  action_t *prev;

  int
    num_bytes,
    idx,
    cur_idx,
    cur_col_idx,
    first_col_idx,
    row_pos,
    col_pos,
    video_first_row_idx;

  row_t *video_first_row;

  char
    type,
    *bytes;

  string_t *__bytes;
} action_t;

typedef struct Action_t {
  action_t *head;

  Action_t *next;
  Action_t *prev;
} Action_t;

typedef struct undo_t {
  Action_t *head;
  Action_t *current;
  Action_t *tail;
       int  num_items;
       int  cur_idx;
       int  state;
} undo_t;

typedef struct vis_t {
  int
    fidx,
    lidx,
    orig_idx;

  char  *(*orig_syn_parser) (buf_t *, char *, int, int, row_t *);
} vis_t;

typedef struct search_t {
  int    idx;
  row_t *row;
  search_t *next;
} search_t;

typedef struct Search_t {
  search_t *head;

  string_t *pat;
  row_t *row;
  int
    idx,
    cur_idx,
    col,
    found,
    dir;

  char
    *prefix,
    *match,
    *end;
} Search_t;

typedef struct histitem_t {
  string_t *data;
  histitem_t *next;
  histitem_t *prev;
} histitem_t;

typedef struct h_search_t {
  histitem_t *head;
  histitem_t *tail;
         int  num_items;
         int  cur_idx;
} h_search_t;

typedef struct hist_t {
  h_search_t *search;
  readline_hist_t  *readline;
} hist_t;

typedef struct dim_t {
  int
    first_row,
    last_row,
    num_rows,
    first_col,
    last_col,
    num_cols;
} dim_t;

typedef struct buf_t {
  row_t  *head;
  row_t  *tail;
  row_t  *current;
    int   cur_idx;
    int   num_items;

  buf_prop *prop;

  BufNormalBeg_cb on_normal_beg;
  BufNormalEnd_cb on_normal_end;
  BufNormalOng_cb on_normal_g;

  buf_t  *next;
  buf_t  *prev;
} buf_t;

typedef struct win_t {
  win_prop *prop;

  buf_t *head;
  buf_t *tail;
  buf_t *current;
    int  cur_idx;
    int  prev_idx;
    int  num_items;

  WinDimCalc_cb dim_calc;

  win_t *next;
  win_t *prev;
} win_t;

typedef struct ed_t {
  win_t *head;
  win_t *tail;
  win_t *current;
    int  cur_idx;
    int  prev_idx;
    int  num_items;

  int name_gen;

  ed_prop *prop;

  ed_t *next;
  ed_t *prev;
} ed_t;

typedef struct row_t {
  string_t *data;

  int
    first_col_idx,
    cur_col_idx;

   row_t *next;
   row_t *prev;
} row_t;

typedef struct buf_prop {
  MY_PROPERTIES;
  MY_CLASSES (buf);

  ed_T  *__Ed__;
  win_T *__Win__;
  i_T   *__I__;

  ed_t  *root;
  win_t *parent;

  char
    *fname,
    *basename,
    *extname,
    *cwd,
    *backupfile,
     backup_suffix[8],
     mode[MAXLEN_MODE];

  int
    fd,
    state,
    nth_ptr_pos,
    prev_nth_ptr_pos,
    prev_num_items,
    at_frame,
    video_first_row_idx,
    is_sticked,
    flags,
    cur_video_row,
    cur_video_col,
    statusline_row,
    show_statusline,
    enable_writing,
    *msg_row_ptr,
    *prompt_row_ptr;

  long
    saved_sec,
    autosave;

  string_t
    *cur_insert,
    *last_insert,
    *statusline,
    *promptline;

  term_t    *term_ptr;
  row_t     *video_first_row;
  syn_t     *syn;
  ftype_t   *ftype;
  Reg_t     *regs;
  hist_t    *history;
  mark_t     marks[NUM_MARKS];
  vis_t      vis[2];
  vis_t      lw_vis_prev[1];
  Ustring_t *line;
  Jump_t    *jumps;
  undo_t
    *undo,
    *redo;

  struct stat st;

  int shared_int;
  string_t *shared_str;
} buf_prop;

typedef struct win_prop {
  char *name;

  int
    flags,
    state,
    type;

  MY_PROPERTIES;
  MY_CLASSES (win);

  buf_T *__Buf__;
  ed_T  *__Ed__;
  i_T   *__I__;

  int
    has_promptline,
    has_msgline,
    has_topline,
    min_rows,
    has_dividers,
    min_frames,
    max_frames,
    num_frames,
    cur_frame;

  ed_t  *parent;
  dim_t **frames_dim;
} win_prop;

typedef struct venv_t {
  pid_t pid;
  uid_t uid;
  gid_t gid;

  string_t
    *user_name,
    *group_name,
    *term_name,
    *my_dir,
    *home_dir,
    *tmp_dir,
    *data_dir,
    *diff_exec,
    *xclip_exec,
    *path,
    *display,
    *env_str,
    *i_dir;
} venv_t;

typedef struct ed_prop {
  char
    *name,
    *saved_cwd;

  E_T *root;
  E_self E;

  MY_PROPERTIES;
  MY_CLASSES (ed);

  buf_T *__Buf__;
  win_T *__Win__;
  i_T   *__I__;

  venv_t *env;

  int
    state,
    exit_quick,
    has_promptline,
    has_msgline,
    has_topline,
    has_ed_readline_commands,
    max_wins,
    max_num_hist_entries,
    max_num_undo_entries,
    num_commands,
    num_special_win,
    topline_row,
    prompt_row,
    msg_row,
    msg_send,
    msg_numchars,
    msg_tabwidth;

  string_t
    *hs_file,
    *hrl_file,
    *last_insert,
    *msgline,
    *topline,
    *ed_str;

  Vstring_t *rl_last_component;
  term_t *term;
  hist_t *history;
  Reg_t regs[NUM_REGISTERS];
  readline_com_t **commands;

  Ustring_t *uline;

  int lmap[2][26];

  int
    lw_mode_chars_len,
    cw_mode_chars_len,
    line_mode_chars_len,
    file_mode_chars_len,
    word_actions_chars_len;

  char
    *lw_mode_actions,
    *cw_mode_actions,
    *bw_mode_actions,
    *line_mode_actions,
    *file_mode_actions;

  utf8
    *lw_mode_chars,
    *cw_mode_chars,
    *line_mode_chars,
    *file_mode_chars,
    *word_actions_chars;

  Vstring_t *word_actions;
  WordActions_cb *word_actions_cb;

  int num_line_mode_cbs;
  LineMode_cb *line_mode_cbs;

  int num_file_mode_cbs;
  FileActions_cb *file_mode_cbs;

  int num_lw_mode_cbs;
  VisualLwMode_cb *lw_mode_cbs;

  int num_cw_mode_cbs;
  VisualCwMode_cb *cw_mode_cbs;

  int num_on_normal_g_cbs;
  BufNormalOng_cb *on_normal_g_cbs;

  int num_readline_cbs;
  Readline_cb *readline_cbs;

  int num_syntaxes;
  syn_t syntaxes[NUM_SYNTAXES];

  int num_expr_reg_cbs;
  ExprRegister_cb *expr_reg_cbs;

  int num_at_exit_cbs;
  EdAtExit_cb *at_exit_cbs;

  int repeat_mode;
  int record;
  int record_idx;

  ssize_t record_header_len;

  Vstring_t    *records[NUM_RECORDS + 1];

  Record_cb     record_cb;
  IRecord_cb    i_record_cb;
  InitRecord_cb init_record_cb;
} ed_prop;

typedef struct E_prop {
  char
    name[MAXLEN_ED_NAME],
    *image_name,
    *image_file;

  int
    persistent_layout,
    save_image,
    state,
    error_state,
    name_gen;

  E_T *Me;
  ed_t *head;
  ed_t *tail;
  ed_t *current;
   int  cur_idx;
   int  orig_num_items;
   int  num_items;
   int  prev_idx;

  ed_self __Ed__;
  Reg_t shared_reg[1];

  IDefineFuns_cb i_define_funs_cb;

  int num_at_exit_cbs;
  EAtExit_cb *at_exit_cbs;

  EdAtInit_cb at_init_cb;
} E_prop;

#undef MY_CLASSES
#undef MY_PROPERTIES

private int  win_edit_fname (win_t *, buf_t **, char *, int, int, int, int);

private int  buf_normal_eof (buf_t *, int);
private int  buf_normal_left (buf_t *, int, int);
private int  buf_normal_right (buf_t *, int, int);
private utf8 buf_quest (buf_t *, char *, utf8 *, int);
private int  buf_normal_down (buf_t *, int, int, int);
private int  buf_normal_goto_linenr (buf_t *, int, int);
private int  buf_substitute (buf_t *, char *, char *, int, int, int, int);
private int  buf_write_to_fname (buf_t *, char *, int, int, int, int, int);

private int  buf_change (buf_t **, int);
private int  buf_split (buf_t **, char *);
private int  buf_readline (buf_t **, readline_t *);
private int  buf_normal_visual_lw (buf_t **);
private int  buf_enew_fname (buf_t **, char *);
private int  buf_insert (buf_t **, utf8, char *);
private int  buf_change_bufname (buf_t **, char *);
private int  buf_insert_complete_filename (buf_t **);
private int  buf_grep_on_normal (buf_t **, utf8, int, int);
private int  buf_open_fname_under_cursor (buf_t **, int, int, int, int);

private void ed_resume (ed_t *);
private void ed_suspend (ed_t *);
private void ed_record (ed_t *, char *, ...);
private int  ed_win_change (ed_t *, buf_t **, int, char *, int, int);
private readline_t *ed_readline_new (ed_t *);

private Vstring_t  *cstring_chop (char *, char, Vstring_t *, StrChop_cb, void *);

/* this code belongs to? */
static const utf8 offsetsFromUTF8[6] = {
  0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL
};
/* the only reference found from the last research,
 * was at the julia programming language sources,
 * (this code and the functions that make use of it,
 * is atleast 4/5 years old, lying (during a non network season))
 */

#define ONE_PAGE ($my(dim->num_rows) - 1)
#define isnotutf8(c) IS_UTF8 (c) == 0
#define isnotatty(fd__) (0 == isatty ((fd__)))
#define IsNotDirSep(c) (c != DIR_SEP)

#define debug_append(fmt, ...)                            \
({                                                        \
  char *file_ = STR_FMT ("/tmp/%s.debug", __func__);      \
  FILE *fp_ = fopen (file_, "a+");                        \
  if (fp_ isnot NULL) {                                   \
    fprintf (fp_, (fmt), ## __VA_ARGS__);                 \
    fclose (fp_);                                         \
  }                                                       \
})

#define utf8_code(s_)                                     \
({                                                        \
  int code = 0; int i_ = 0; int sz = 0;                   \
  do {code <<= 6; code += (uchar) s_[i_++]; sz++;}        \
  while (s_[i_] and IS_UTF8(s_[i_]));                     \
                                                          \
  code -= offsetsFromUTF8[sz-1];                          \
  code;                                                   \
})

#define CUR_UTF8_CODE                                     \
({                                                        \
  char *s_ = $mycur(data)->bytes + $mycur(cur_col_idx);   \
  int code = 0; int i_ = 0; int sz = 0;                   \
  do {code <<= 6; code += (uchar) s_[i_++]; sz++;}        \
  while (s_[i_] and IS_UTF8(s_[i_]));                     \
                                                          \
  code -= offsetsFromUTF8[sz-1];                          \
  code;                                                   \
})

#define BUF_GET_NUMBER(intbuf_, idx_)                     \
({                                                        \
  utf8 cc__;                                              \
  while ((idx_) < 8) {                                    \
    cc__ = IO.getkey (STDIN_FILENO);                      \
                                                          \
    if (IS_DIGIT (cc__))                                  \
      (intbuf_)[(idx_)++] = cc__;                         \
    else                                                  \
      break;                                              \
  }                                                       \
  cc__;                                                   \
})

#define BUF_GET_AS_NUMBER(has_pop_pup, frow, fcol, lcol, prefix)          \
({                                                                        \
  utf8 cc__;                                                              \
  int nr = 0;                                                             \
  string_t *ibuf = NULL, *sbuf = NULL;                                    \
  if (has_pop_pup) {                                                      \
     ibuf = String.new_with (""); sbuf = String.new_with ("");            \
  }                                                                       \
  while (1) {                                                             \
    if (has_pop_pup) {                                                    \
      String.replace_with_fmt (sbuf, "%s %s", prefix, ibuf->bytes);       \
      Video.paint_rows_with ($my(video), frow, fcol, lcol, sbuf->bytes);  \
      SEND_ESC_SEQ ($my(video)->fd, TERM_CURSOR_HIDE);                    \
    }                                                                     \
    cc__ = IO.getkey (STDIN_FILENO);                                      \
                                                                          \
    if (IS_DIGIT (cc__)) {                                                \
      nr = (10 * nr) + (cc__ - '0');                                      \
      if (has_pop_pup) String.append_byte (ibuf, cc__);                   \
    } else                                                                \
      break;                                                              \
  }                                                                       \
  if (has_pop_pup) {                                                      \
    String.release (ibuf); String.release (sbuf);                               \
    Video.resume_painted_rows ($my(video));                               \
    SEND_ESC_SEQ ($my(video)->fd, TERM_CURSOR_SHOW);                      \
  }                                                                       \
  nr;                                                                     \
})

#define IS_MODE(mode__) Cstring.eq ($my(mode), (mode__))

#define HAS_THIS_LINE_A_TRAILING_NEW_LINE \
({$mycur(data)->bytes[$mycur(data)->num_bytes - 1] is '\n';})

#define RM_TRAILING_NEW_LINE                                        \
  if ($mycur(data)->bytes[$mycur(data)->num_bytes - 1] is '\n' or   \
      $mycur(data)->bytes[$mycur(data)->num_bytes - 1] is 0)        \
     String.clear_at ($mycur(data), $mycur(data)->num_bytes - 1)

#define ADD_TRAILING_NEW_LINE                                        \
  if ($mycur(data)->bytes[$mycur(data)->num_bytes - 1] isnot '\n')   \
    String.append_with ($mycur(data), "\n")

#define stack_free(list, type)                                      \
do {                                                                \
  type *item = (list)->head;                                        \
  while (item != NULL) {                                            \
    type *tmp = item->next;                                         \
    free (item);                                                    \
    item = tmp;                                                     \
  }                                                                 \
} while (0)

#define stack_append(list, type, node)                              \
({                                                                  \
  type *item = (node);                                              \
  while (item and item->next) item = item->next;                    \
  item->next = NULL;                                                \
  item = (list)->head;                                              \
  if (item == NULL) {                                               \
    (list)->head = (node);                                          \
    (list)->head->prev = NULL;                                      \
  } else {                                                          \
    while (item->next != NULL) item = item->next;                   \
    (node)->prev = item;                                            \
    item->next = (node);                                            \
  }                                                                 \
  (list);                                                           \
})

#define stack_push(list, node)                                      \
({                                                                  \
  if ((list)->head == NULL) {                                       \
    (list)->head = (node);                                          \
    (list)->head->next = NULL;                                      \
  } else {                                                          \
    (node)->next = (list)->head;                                    \
    (list)->head = (node);                                          \
  }                                                                 \
                                                                    \
 (list);                                                            \
})

#define stack_pop(list_, type_)                                     \
({                                                                  \
  type_ *node_ = (list_)->head;                                     \
  if (node_ != NULL)                                                \
    (list_)->head = (list_)->head->next;                            \
                                                                    \
  node_;                                                            \
})

#define stack_pop_tail(list_, type_)                                \
({                                                                  \
  type_ *node_ = (list_)->head;                                     \
  type_ *tmp_ = NULL;                                               \
  while (node_->next) {                                             \
    tmp_ = node_;                                                   \
    node_ = node_->next;                                            \
  }                                                                 \
  if (tmp_) tmp_->next = NULL;                                      \
  node_;                                                            \
})

#define list_push(list, node)                                       \
({                                                                  \
  if ((list)->head == NULL) {                                       \
    (list)->head = (node);                                          \
    (list)->tail = (node);                                          \
    (list)->head->next = NULL;                                      \
    (list)->head->prev = NULL;                                      \
  } else {                                                          \
    (list)->head->prev = (node);                                    \
    (node)->next = (list)->head;                                    \
    (list)->head = (node);                                          \
  }                                                                 \
                                                                    \
  (list)->num_items++;                                              \
  list;                                                             \
})

#define current_list_prepend(list, node)                            \
({                                                                  \
  if ((list)->current is NULL) {                                    \
    (list)->head = (node);                                          \
    (list)->tail = (node);                                          \
    (list)->cur_idx = 0;                                            \
    (list)->current = (list)->head;                                 \
  } else {                                                          \
    if ((list)->cur_idx == 0) {                                     \
      (list)->head->prev = (node);                                  \
      (node)->next = (list)->head;                                  \
      (list)->head = (node);                                        \
      (list)->current = (list)->head;                               \
    } else {                                                        \
      (list)->current->prev->next = (node);                         \
      (list)->current->prev->next->next = (list)->current;          \
      (list)->current->prev->next->prev = (list)->current->prev;    \
      (list)->current->prev = (list)->current->prev->next;          \
      (list)->current = (list)->current->prev;                      \
    }                                                               \
  }                                                                 \
                                                                    \
  (list)->num_items++;                                              \
  (list)->current;                                                  \
})

#define current_list_append(list, node)                             \
({                                                                  \
  if ((list)->current is NULL) {                                    \
    (list)->head = (node);                                          \
    (list)->tail = (node);                                          \
    (list)->cur_idx = 0;                                            \
    (list)->current = (list)->head;                                 \
  } else {                                                          \
    if ((list)->cur_idx is (list)->num_items - 1) {                 \
      (list)->current->next = (node);                               \
      (node)->prev = (list)->current;                               \
      (list)->current = (node);                                     \
      (node)->next = NULL;                                          \
      (list)->cur_idx++;                                            \
      (list)->tail = (node);                                        \
    } else {                                                        \
      (node)->next = (list)->current->next;                         \
      (list)->current->next = (node);                               \
      (node)->prev = (list)->current;                               \
      (node)->next->prev = (node);                                  \
      (list)->current = (node);                                     \
      (list)->cur_idx++;                                            \
    }                                                               \
  }                                                                 \
                                                                    \
  (list)->num_items++;                                              \
  (list)->current;                                                  \
})

#define list_append(list, node)                                     \
({                                                                  \
  if ((list)->head is NULL) {                                       \
    (list)->head = (node);                                          \
    (list)->tail = (node);                                          \
    (list)->cur_idx = 0;                                            \
    (list)->current = (list)->head;                                 \
  } else {                                                          \
    (list)->tail->next = (node);                                    \
    (node)->prev = (list)->tail;                                    \
    (node)->next = NULL;                                            \
    (list)->tail = (node);                                          \
  }                                                                 \
                                                                    \
  (list)->num_items++;                                              \
})

#define list_pop_tail(list, type)                                   \
({                                                                  \
type *node = NULL;                                                  \
do {                                                                \
  if ((list)->tail is NULL) break;                                  \
  node = (list)->tail;                                              \
  (list)->tail->prev->next = NULL;                                  \
  (list)->tail = (list)->tail->prev;                                \
  (list)->num_items--;                                              \
} while (0);                                                        \
  node;                                                             \
})

#define list_pop_at(list, type, idx_)                               \
({                                                                  \
  int cur_idx = (list)->cur_idx;                                    \
  int __idx__ = current_list_set (list, idx_);                      \
  type *cnode = NULL;                                               \
  do {                                                              \
    if (__idx__ is INDEX_ERROR) break;                              \
    cnode = current_list_pop (list, type);                          \
    if (cur_idx is __idx__) break;                                  \
    if (cur_idx > __idx__) cur_idx--;                               \
    current_list_set (list, cur_idx);                               \
  } while (0);                                                      \
  cnode;                                                            \
})

#define current_list_pop(list, type)                                \
({                                                                  \
type *node = NULL;                                                  \
do {                                                                \
  if ((list)->current is NULL) break;                               \
  node = (list)->current;                                           \
  if (1 is (list)->num_items) {                                     \
    (list)->head = NULL;                                            \
    (list)->tail = NULL;                                            \
    (list)->current = NULL;                                         \
    break;                                                          \
  }                                                                 \
  if (0 is (list)->cur_idx) {                                       \
    (list)->current = (list)->current->next;                        \
    (list)->current->prev = NULL;                                   \
    (list)->head = (list)->current;                                 \
    break;                                                          \
  }                                                                 \
  if ((list)->cur_idx is (list)->num_items - 1) {                   \
    (list)->current = (list)->current->prev;                        \
    (list)->current->next = NULL;                                   \
    (list)->cur_idx--;                                              \
    (list)->tail = (list)->current;                                 \
    break;                                                          \
  }                                                                 \
  (list)->current->next->prev = (list)->current->prev;              \
  (list)->current->prev->next = (list)->current->next;              \
  (list)->current = (list)->current->next;                          \
} while (0);                                                        \
  if (node isnot NULL) (list)->num_items--;                         \
  node;                                                             \
})

#define current_list_set(list, idx_)                                \
({                                                                  \
  int idx__ = idx_;                                                 \
  do {                                                              \
    if (0 > idx__) idx__ += (list)->num_items;                      \
    if (idx__ < 0 or idx__ >= (list)->num_items) {                  \
      idx__ = INDEX_ERROR;                                          \
      break;                                                        \
    }                                                               \
    if (idx__ is (list)->cur_idx) break;                            \
    int idx___ = (list)->cur_idx;                                   \
    (list)->cur_idx = idx__;                                        \
    if (idx___ < idx__)                                             \
      while (idx___++ < idx__)                                      \
        (list)->current = (list)->current->next;                    \
    else                                                            \
      while (idx___-- > idx__)                                      \
        (list)->current = (list)->current->prev;                    \
  } while (0);                                                      \
  idx__;                                                            \
})

#define list_get_at(list_, type_, idx_)                             \
({                                                                  \
  type_ *node = NULL;                                               \
  int idx__ = idx_;                                                 \
  do {                                                              \
    if (0 > idx__) idx__ += (list_)->num_items;                     \
    if (idx__ < 0 or idx__ >= (list_)->num_items) {                 \
      idx__ = INDEX_ERROR;                                          \
      break;                                                        \
    }                                                               \
    if ((list_)->num_items / 2 < idx__) {                           \
      node = (list_)->head;                                         \
      while (idx__--)                                               \
        node = node->next;                                          \
    } else {                                                        \
      node = (list_)->tail;                                         \
      while (idx__++ < (list_)->num_items - 1)                      \
        node = node->prev;                                          \
    }                                                               \
  } while (0);                                                      \
  node;                                                             \
})

#define ED_BYTES_TO_READLINE(rl_, bytes, len)                                \
do {                                                                      \
  char *sp_ = (bytes);                                                    \
  for (int i__ = 0; i__ < (len); i__++) {                                 \
    int clen = Ustring.charlen ((bytes)[i__]);                            \
    (rl_)->state |= (READLINE_INSERT_CHAR|READLINE_BREAK);                \
    (rl_)->c = UTF8_CODE (sp_);                                           \
    Readline.edit ((rl_));                                                \
    i__ += clen - 1;                                                      \
    sp_ += clen;                                                          \
    }                                                                     \
} while (0)

#define READLINE_ED_USER_DATA_IDX   0
#define READLINE_BUF_USER_DATA_IDX  1
#define READLINE_MENU_USER_DATA_IDX 2

#define Root  My(__E__)
#define Ed My(__Ed__)
#define Win My(__Win__)
#define Buf My(__Buf__)

#define Msg My(__Msg__)
#define Error My(__Error__)

#define Screen termType.self.screen
#define Cursor termType.self.cursor

#define $OurRoot  $my(__E__)
#define $OurRoots(__p__) $my(__E__)->prop->__p__
