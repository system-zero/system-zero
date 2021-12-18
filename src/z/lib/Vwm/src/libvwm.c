/* this was pulled from: https://github.com/agathoklisx/libv
 * the escape mechanism algorithm was derived from the splitvt utility
 * by Sam Lantinga, Splitvt 1.6.6  3/11/2006 (slouken at devolution dot com)
 * which is licensed with GPL2, and it is included within this directory
 */

#define LIBRARY "Vwm"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_SYS_IOCTL
#define REQUIRE_SYS_SELECT
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT
#define REQUIRE_SYS_WAIT
#define REQUIRE_SYS_MMAN
#define REQUIRE_PTY
#define REQUIRE_FCNTL
#define REQUIRE_TERMIOS
#define REQUIRE_TIME
#define REQUIRE_DIRENT
#define REQUIRE_SIGNAL

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE  DECLARE
#define REQUIRE_USTRING_TYPE  DECLARE
#define REQUIRE_DIR_TYPE      DECLARE
#define REQUIRE_PATH_TYPE     DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_READLINE_TYPE DECLARE
#define REQUIRE_VIDEO_TYPE    DECLARE
#define REQUIRE_SYS_TYPE      DECLARE
#define REQUIRE_ERROR_TYPE    DECLARE
#define REQUIRE_VUI_TYPE      DECLARE
#define REQUIRE_VWM_TYPE      DONOT_DECLARE
#define REQUIRE_LIST_MACROS
#define REQUIRE_TERM_MACROS
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

static  vwm_T  *__VWM__ = NULL;

#define $myprop this->prop
#define $my(__v__) $myprop->__v__
#define self(__f__, ...) this->self.__f__ (this, ##__VA_ARGS__)

#define Vwm    vwm->self
#define Vframe this->frame
#define Vwin this->win

#define MIN_ROWS 2
#define MAX_TTYNAME 1024
#define MAX_PARAMS  12
#define MAX_SEQ_LEN 32

#define TABWIDTH    8

#define V_STR_FMT_LEN(len_, fmt_, ...)                                \
({                                                                    \
  char buf_[len_];                                                    \
  snprintf (buf_, len_, fmt_, __VA_ARGS__);                           \
  buf_;                                                               \
})

#define NORMAL          0x00
#define BOLD            0x01
#define UNDERLINE       0x02
#define BLINK           0x04
#define REVERSE         0x08
#define ITALIC          0x10
#define SELECTED        0xF0

#define NCHARSETS       2
#define G0              0
#define G1              1
#define UK_CHARSET      0x01
#define US_CHARSET      0x02
#define GRAPHICS        0x04

#define COLOR_RED_STR   "\033[31m"
#define COLOR_GREEN_STR "\033[32m"

#define COLOR_FOCUS     COLOR_GREEN_STR
#define COLOR_UNFOCUS   COLOR_RED_STR

enum vt_keystate {
  norm,
  appl
};

typedef string_t *(*FrameProcessChar_cb) (vwm_frame *, string_t *, int);

struct vwm_frame {
  char
    **argv,
    mb_buf[8],
    tty_name[1024];

  uchar
    charset[2],
    textattr,
    saved_textattr;

  int
    fd,
    argc,
    logfd,
    state,
    status,
    mb_len,
    mb_curlen,
    col_pos,
    row_pos,
    new_rows,
    num_rows,
    num_cols,
    last_row,
    first_row,
    first_col,
    scroll_first_row,
    param_idx,
    at_frame,
    is_visible,
    remove_log,
    saved_row_pos,
    saved_col_pos,
    old_attribute,
    **colors,
    **videomem,
    *tabstops,
    *esc_param,
    *cur_param;

  utf8 mb_code;

  enum vt_keystate key_state;

  pid_t pid;

  string_t
    *logfile,
    *render;

  FrameProcessOutput_cb process_output_cb;
  FrameProcessChar_cb   process_char_cb;
  FrameUnimplemented_cb unimplemented_cb;
  FrameAtFork_cb        at_fork_cb;

  vwm_t   *root;
  vwm_win *parent;

  vwm_win_self   win;
  vwm_frame_self self;

  vwm_frame
    *next,
    *prev;
};

struct vwm_win {
  char
    *name;

  string_t
    *render,
    *separators_buf;

  int
    saved_row,
    saved_col,
    cur_row,
    cur_col,
    num_rows,
    num_cols,
    first_row,
    first_col,
    last_row,
    max_frames,
    num_visible_frames,
    draw_separators,
    num_separators,
    is_initialized;

  vwm_frame
    *head,
    *current,
    *tail,
    *last_frame;

  int
    cur_idx,
    num_items;

  vwm_t *parent;

  vwm_win
    *next,
    *prev;

  vwm_win_self self;
  vwm_frame_self frame;
};

struct vwm_prop {
  term_t  *term;

  char mode_key;

  string_t
    *shell,
    *editor,
    *tmpdir,
    *datadir,
    *shared_str,
    *default_app,
    *history_file,
    *sequences_fname,
    *unimplemented_fname;

  FILE
    *sequences_fp,
    *unimplemented_fp;

  int
    state,
    num_cols,
    num_rows,
    name_gen,
    shared_int,
    need_resize,
    first_column;

  uint modes;

  vwm_win
    *head,
    *tail,
    *current,
    *last_win;

  int
    cur_idx,
    num_items;

  video_t *video;
  readline_com_t **commands;
  int num_commands;

  readline_hist_t *readline_history;

  void *user_data[NUM_OBJECTS];

  VwmReadline_cb readline_cb;
  VwmEditFile_cb edit_file_cb;

  int num_at_exit_cbs;
  VwmAtExit_cb *at_exit_cbs;

  int num_process_input_cbs;
  ProcessInput_cb *process_input_cbs;
};

static const utf8 offsetsFromUTF8[6] = {
  0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static void vwm_sigwinch_handler (int sig);

static void fd_set_size (int fd, int rows, int cols) {
  struct winsize wsiz;
  wsiz.ws_row = rows;
  wsiz.ws_col = cols;
  wsiz.ws_xpixel = 0;
  wsiz.ws_ypixel = 0;
  ioctl (fd, TIOCSWINSZ, &wsiz);
}

static term_t *vwm_new_term (vwm_t *this) {
  ifnot (NULL is $my(term)) return $my(term);

  term_t *term = Term.new ();

  char *term_name = Sys.get.env_value ("TERM");

  if (Cstring.eq_n (term_name, "linux", 5))
    term->name = Cstring.dup ("linux", 5);
  else if (Cstring.eq_n (term_name, "xterm", 5))
    term->name = Cstring.dup ("xterm", 5);
  else if (Cstring.eq_n (term_name, "rxvt-unicode", 12))
    term->name = Cstring.dup ("xterm", 5);
  else
    term->name = Cstring.dup (term_name, bytelen (term_name));

  $my(term) = term;

  return term;
}

static void vwm_set_process_input_cb (vwm_t *this, ProcessInput_cb cb) {
  if (NULL is cb) return;

  $my(num_process_input_cbs)++;

  ifnot ($my(num_process_input_cbs) - 1)
    $my(process_input_cbs) = Alloc (sizeof (ProcessInput_cb));
  else
    $my(process_input_cbs) = Realloc ($my(process_input_cbs), sizeof (ProcessInput_cb));

  $my(process_input_cbs)[$my(num_process_input_cbs) -1] = cb;
}

static void vwm_set_at_exit_cb (vwm_t *this, VwmAtExit_cb cb) {
  if (NULL is cb) return;

  $my(num_at_exit_cbs)++;

  ifnot ($my(num_at_exit_cbs) - 1)
    $my(at_exit_cbs) = Alloc (sizeof (VwmAtExit_cb));
  else
    $my(at_exit_cbs) = Realloc ($my(at_exit_cbs), sizeof (VwmAtExit_cb) * $my(num_at_exit_cbs));

  $my(at_exit_cbs)[$my(num_at_exit_cbs) -1] = cb;
}

static void vwm_set_size (vwm_t *this, int rows, int cols, int first_col) {
  $my(num_rows) = rows;
  $my(num_cols) = cols;
  $my(first_column) = first_col;
}

static void vwm_set_term  (vwm_t *this, term_t *term) {
  $my(term) = term;
}

static void vwm_set_state (vwm_t *this, int state) {
  $my(state) = state;
}

static void vwm_set_editor (vwm_t *this, char *editor) {
  if (NULL is editor) return;
  size_t len = bytelen (editor);
  ifnot (len) return;
  String.clear ($my(editor));
  String.append_with_len ($my(editor), editor, len);
}

static void vwm_set_default_app (vwm_t *this, char *app) {
  if (NULL is app) return;
  size_t len = bytelen (app);
  ifnot (len) return;
  String.clear ($my(default_app));
  String.append_with_len ($my(default_app), app, len);
}

static void vwm_set_object (vwm_t *this, void *object, int idx) {
  if (idx >= NUM_OBJECTS or idx < 0) return;
  $my(user_data)[idx] = object;
}

static void vwm_set_readline_cb (vwm_t *this, VwmReadline_cb cb) {
  $my(readline_cb) = cb;
}

static void vwm_set_edit_file_cb (vwm_t *this, VwmEditFile_cb cb) {
  $my(edit_file_cb) = cb;
}

static void vwm_set_shell (vwm_t *this, char *shell) {
  if (NULL is shell) return;
  size_t len = bytelen (shell);
  ifnot (len) return;
  String.clear ($my(shell));
  String.append_with_len ($my(shell), shell, len);
}

static void vwm_set_debug_unimplemented (vwm_t *this, char *fname) {
  self(unset.debug.unimplemented);

  if (NULL is fname) {
    char *tmpdir = self(get.tmpdir);
    tmpfname_t *t = File.tmpfname.new (tmpdir,
        V_STR_FMT_LEN (64, "%d_unimplemented", getpid ()));

    if (-1 is t->fd) return;

    $my(unimplemented_fp) = fdopen (t->fd, "w+");
    $my(unimplemented_fname) = String.dup (t->fname);

    File.tmpfname.release (t, 0);

    return;
  }

  $my(unimplemented_fname) = String.new_with (fname);
  $my(unimplemented_fp) = fopen (fname, "w");
}

static void vwm_unset_debug_unimplemented (vwm_t *this) {
  if (NULL is $my(unimplemented_fp)) return;
  fclose ($my(unimplemented_fp));
  $my(unimplemented_fp) = NULL;
  String.release ($my(unimplemented_fname));
}

static void vwm_set_debug_sequences (vwm_t *this, char *fname) {
  self(unset.debug.sequences);

  if (NULL is fname) {
    char *tmpdir = self(get.tmpdir);
    tmpfname_t *t = File.tmpfname.new (tmpdir,
        V_STR_FMT_LEN (64, "%d_sequences", getpid ()));

    if (-1 is t->fd) return;

    $my(sequences_fp) = fdopen (t->fd, "w+");
    $my(sequences_fname) = String.dup (t->fname);

    File.tmpfname.release (t, 0);

    return;
  }

  $my(sequences_fname) = String.new_with (fname);
  $my(sequences_fp) = fopen (fname, "w");
}

static void vwm_unset_debug_sequences (vwm_t *this) {
  if (NULL is $my(sequences_fp)) return;
  fclose ($my(sequences_fp));
  $my(sequences_fp) = NULL;
  String.release ($my(sequences_fname));
}

static void vwm_unset_tmpdir (vwm_t *this) {
  String.release ($my(tmpdir));
  $my(tmpdir) = NULL;
}

static int vwm_set_tmpdir (vwm_t *this, char *dir, size_t len) {
  if (NULL is $my(tmpdir))
    $my(tmpdir) = String.new (32);

  int num_tries = 0;

do_dir:
  String.clear ($my(tmpdir));

  if (++num_tries is 3)
    goto theerror;

  if (NULL is dir or dir[0] is '\0') {
    if (num_tries is 1)
      String.append_with ($my(tmpdir), TMPDIR);
    else
      String.append_with ($my(tmpdir), "/tmp");
  } else
    String.append_with_len ($my(tmpdir), dir, len);

  String.append_byte ($my(tmpdir), '/');
  String.append_with ($my(tmpdir), V_STR_FMT_LEN (64, "%d-vwm_tmpdir", getpid ()));

  if (-1 is access ($my(tmpdir)->bytes, F_OK)) {
    if (-1 is Dir.make_parents ($my(tmpdir)->bytes, S_IRWXU, DirOpts(.err = 0)))
      goto do_dir;
  } else {
    ifnot (Dir.is_directory ($my(tmpdir)->bytes))
      goto theerror;

    if (-1 is access ($my(tmpdir)->bytes, W_OK|R_OK|X_OK))
      goto theerror;
  }

  return OK;

theerror:
  self(unset.tmpdir);
  return NOTOK;
}

static void vwm_unset_datadir (vwm_t *this) {
  String.release ($my(datadir));
  $my(datadir) = NULL;
}

static int vwm_set_datadir (vwm_t *this, char *dir, size_t len) {
  if (NULL is $my(datadir))
    $my(datadir) = String.new (32);

  int num_tries = 0;

do_dir:
  String.clear ($my(datadir));

  if (++num_tries is 3)
    goto theerror;

  if (NULL is dir or dir[0] is '\0') {
    if (num_tries is 1)
      String.append_with ($my(datadir), DATADIR);
    else
      String.append_with ($my(datadir), Sys.get.env_value ("HOME"));

    String.append_with ($my(datadir), "/v");
  } else
    String.append_with_len ($my(datadir), dir, len);

  if (-1 is access ($my(datadir)->bytes, F_OK)) {
    if (-1 is Dir.make_parents ($my(datadir)->bytes, S_IRWXU, DirOpts(.err = 0)))
      goto do_dir;
  } else {
    ifnot (Dir.is_directory ($my(datadir)->bytes))
      goto theerror;

    if (-1 is access ($my(datadir)->bytes, W_OK|R_OK|X_OK))
      goto theerror;
  }

  return OK;

theerror:
  self(unset.datadir);
  return NOTOK;
}

static void vwm_set_history_file (vwm_t *this, char *file) {
  ifnot (Sys.get.env_value_as_int ("UID")) return;

  if (NULL is $my(history_file))
    $my(history_file) = String.new (16);

  if (NULL is file or *file is '\0') {
    if ($my(datadir) isnot NULL) {
      String.replace_with_fmt ($my(history_file), "%s/.readline_hist_%s",
          $my(datadir)->bytes, Sys.get.env_value ("USERNAME"));
    } else
      String.replace_with_fmt ($my(history_file), "%s/.readline_hist_%s",
          Sys.get.env_value ("HOME"), Sys.get.env_value ("USERNAME"));
  } else
    String.replace_with ($my(history_file), file);
}

static void vwm_history_add_lines (vwm_t *this, Vstring_t *hist) {
  if (NULL is hist) return;

  vstring_t *it = hist->head;
  while (it) {
    char *sp = Cstring.trim.end (it->data->bytes, '\n');
    sp = Cstring.trim.end (sp, ' ');
    readline_t *rl = Readline.new (this, $my(term), Input.getkey, $my(num_rows) - 1,
        1, $my(num_cols), $my(video));
    rl->history = $my(readline_history);
    Readline.insert_with_len (rl, it->data->bytes, bytelen (sp));
    Readline.history.push (rl);
    it = it->next;
  }
}

static void vwm_history_read (vwm_t *this) {
  ifnot (Sys.get.env_value_as_int ("UID")) return;

  Vstring_t *lines = NULL;

  if (File.exists ($my(history_file)->bytes)) {
    lines = File.readlines ($my(history_file)->bytes, NULL, NULL, NULL);
    vwm_history_add_lines (this, lines);
  }

  Vstring.release (lines);
}

static void vwm_history_write (vwm_t *this) {
  ifnot (Sys.get.env_value_as_int ("UID")) return;

  FILE *fp = fopen ($my(history_file)->bytes, "w");
  if (NULL is fp) return;

  readline_hist_item_t *hrl = $my(readline_history)->tail;
  while (hrl) {
    string_t *line = Vstring.join (hrl->data->line, "");
    fprintf (fp, "%s\n", line->bytes);
    String.release (line);
    hrl = hrl->prev;
  }

  fclose (fp);
}

static utf8 vwm_getkey (vwm_t *this, int infd) {
  (void) this;
  return Input.getkey (infd);
}

static void readline_realloc_command_arg (readline_com_t *rlcom, int num) {
  int orig_num = rlcom->num_args;
  rlcom->num_args = num;
  rlcom->args = Realloc (rlcom->args, sizeof (char *) * (rlcom->num_args + 1));
  for (int i = orig_num; i <= num; i++)
    rlcom->args[i] = NULL;
}

static void vwm_readline_append_command_arg (vwm_t *this, const char *com, const char *argname, size_t len) {
  if (len <= 0) len = bytelen (argname);

  int i = 0;
  while (i < $my(num_commands)) {
    if (Cstring.eq ($my(commands)[i]->com, com)) {
      int idx = 0;
      int found = 0;
      while (idx < $my(commands)[i]->num_args) {
        if (NULL is $my(commands)[i]->args[idx]) {
          $my(commands)[i]->args[idx] = Cstring.dup (argname, len);
          found = 1;
          break;
        }
        idx++;
      }

      ifnot (found) {
        readline_realloc_command_arg ($my(commands)[i], $my(commands)[i]->num_args + 1);
        $my(commands)[i]->args[$my(commands)[i]->num_args - 1] = Cstring.dup (argname, len);
      }

      return;
    }
    i++;
  }
}

static void vwm_readline_append_command (vwm_t *this, const char *command,
                       size_t command_len, int num_args) {
  int len = $my(num_commands) + 1;

  ifnot ($my(num_commands))
    $my(commands) = Alloc (sizeof (readline_com_t) * (2));
  else
    $my(commands) = Realloc ($my(commands), sizeof (readline_com_t) * (len + 1));

  int idx = $my(num_commands);

  $my(commands)[idx] = Alloc (sizeof (readline_com_t));
  $my(commands)[idx]->com = Alloc (command_len + 1);
  Cstring.cp ($my(commands)[idx]->com, command_len + 1, command, command_len);

  ifnot (num_args)
    $my(commands)[idx]->args = NULL;
  else {
    $my(commands)[idx]->args = Alloc (sizeof (char *) * ((int) num_args + 1));
    for (int k = 0; k <= num_args; k++)
      $my(commands)[idx]->args[k] = NULL;
  }

  $my(commands)[idx]->num_args = num_args;
  $my(commands)[idx + 1] = NULL;
  $my(num_commands)++;
}

static void vwm_deinit_commands (vwm_t *this) {
  if (NULL is $my(commands)) return;

  int i = 0;
  while ($my(commands)[i]) {
    free ($my(commands)[i]->com);
    if ($my(commands)[i]->args isnot NULL) {
      int j = 0;
      while ($my(commands)[i]->args[j] isnot NULL)
        free ($my(commands)[i]->args[j++]);
      free ($my(commands)[i]->args);
      }

    free ($my(commands)[i]);
    i++;
  }

  free ($my(commands));
  $my(commands) = NULL;
  $my(num_commands) = 0;
}

static void vwm_init_commands (vwm_t *this) {
  if ($my(num_commands)) return;

  vwm_readline_append_command     (this, "quit", 4, 0);

  vwm_readline_append_command     (this, "win_draw", 8, 0);

  vwm_readline_append_command     (this, "frame_clear", 11, 0);
  vwm_readline_append_command_arg (this, "frame_clear", "--clear-log=", 12);
  vwm_readline_append_command_arg (this, "frame_clear", "--clear-video-mem=", 18);

  vwm_readline_append_command     (this, "frame_delete", 12, 0);

  vwm_readline_append_command     (this, "split_and_fork", 14, 0);
  vwm_readline_append_command_arg (this, "split_and_fork", "--command={", 11);

  vwm_readline_append_command     (this, "set", 3, 0);
  vwm_readline_append_command_arg (this, "set", "--log-file=", 11);

  vwm_readline_append_command     (this, "win_new", 7, 0);
  vwm_readline_append_command_arg (this, "win_new", "--num-frames=", 13);
  vwm_readline_append_command_arg (this, "win_new", "--command={", 11);
  vwm_readline_append_command_arg (this, "win_new", "--focus=", 8);
  vwm_readline_append_command_arg (this, "win_new", "--draw=", 7);
}

static vwm_win *vwm_set_current_at (vwm_t *this, int idx) {
  vwm_win *cur_win = $my(current);
  if (EINDEX isnot DListSetCurrent ($myprop, idx)) {
    if (NULL isnot cur_win)
      $my(last_win) = cur_win;
  }

  return $my(current);
}

static void vwm_set_mode_key (vwm_t *this, char c) {
  $my(mode_key) = c;
}

static char vwm_get_mode_key (vwm_t *this) {
  return $my(mode_key);
}

static readline_com_t **vwm_get_commands (vwm_t *this, int *num_commands) {
  vwm_init_commands (this);
  *num_commands = $my(num_commands);
  return $my(commands);
}

static int vwm_get_lines (vwm_t *this) {
  return $my(term)->num_rows;
}

static int vwm_get_columns (vwm_t *this) {
  return $my(term)->num_cols;
}

static void *vwm_get_user_data_at (vwm_t *this, int idx) {
  if (idx >= NUM_OBJECTS or idx < 0) return NULL;
  return $my(user_data)[idx];
}

static char *vwm_get_tmpdir (vwm_t *this) {
  ifnot (NULL is $my(tmpdir))
    return $my(tmpdir)->bytes;

  return (char *) TMPDIR;
}

static int vwm_get_num_wins (vwm_t *this) {
  return $my(num_items);
}

static int vwm_get_win_idx (vwm_t *this, vwm_win *win) {
  int idx = DListGetIdx ($myprop, vwm_win, win);
  if (idx is EINDEX)
    return NOTOK;

  return idx;
}

static vwm_win *vwm_get_win_at (vwm_t *this, int idx) {
  return DListGetAt ($myprop, vwm_win, idx);
}

static vwm_win *vwm_get_current_win (vwm_t *this) {
  return $my(current);
}

static int vwm_get_current_win_idx (vwm_t *this) {
  return $my(cur_idx);
}

static vwm_frame *vwm_get_current_frame (vwm_t *this) {
  return $my(current)->current;
}

static term_t *vwm_get_term (vwm_t *this) {
  return $my(term);
}

static int vwm_get_state (vwm_t *this) {
  return $my(state);
}

static char *vwm_get_shell (vwm_t *this) {
  return $my(shell)->bytes;
}

static char *vwm_get_editor (vwm_t *this) {
  return $my(editor)->bytes;
}

static char *vwm_get_default_app (vwm_t *this) {
  return $my(default_app)->bytes;
}

static void vwm_release_readline_history (vwm_t *this) {
  readline_hist_t *hrl = $my(readline_history);
  readline_hist_item_t *it = hrl->head;
  while (it isnot NULL) {
    readline_hist_item_t *tmp = it->next;
    Readline.release (it->data);
    free (it);
    it = tmp;
  }

  free (hrl);
}

static void frame_release_info (vframe_info *finfo) {
  if (NULL is finfo) return;
  free (finfo);
  finfo = NULL;
}

static void win_release_info (vwin_info *winfo) {
  if (NULL is winfo) return;

  for (int fidx = 0; fidx < winfo->num_frames; fidx++)
    frame_release_info (winfo->frames[fidx++]);

  free (winfo->frames);
  free (winfo);
  winfo = NULL;
}

static void vwm_release_info (vwm_t *this, vwm_info **vinfop) {
  (void) this;
  if (*vinfop is NULL) return;

  vwm_info *vinfo = *vinfop;

  for (int widx = 0; widx < vinfo->num_win; widx++)
    win_release_info (vinfo->wins[widx++]);

  free (vinfo->wins);
  free (vinfo);
  *vinfop = NULL;
}

static vframe_info *frame_get_info (vwm_frame *this) {
  vframe_info *finfo = Alloc (sizeof (vframe_info));
  finfo->pid = this->pid;
  finfo->first_row = this->first_row;
  finfo->num_rows = this->num_rows;
  finfo->last_row = this->first_row + this->num_rows - 1;
  finfo->is_visible = this->is_visible;
  finfo->is_current = this->parent->current is this;
  finfo->at_frame = (this->is_visible ? this->at_frame : -1);
  finfo->logfile = (NULL is this->logfile ? (char *) "" : this->logfile->bytes);

  int arg = 0;
  for (; arg < this->argc; arg++)
    finfo->argv[arg] = this->argv[arg];
  finfo->argv[arg] = NULL;

  return finfo;
}

static vwin_info *win_get_info (vwm_win *this) {
  vwm_t *vwm = this->parent;

  vwin_info *winfo = Alloc (sizeof (vwin_info));
  winfo->name = this->name;
  winfo->num_rows = this->num_rows;
  winfo->num_cols = this->num_cols;
  winfo->num_visible_frames = self(get.num_visible_frames);
  winfo->num_frames = this->num_items;
  winfo->cur_frame_idx = this->cur_idx;
  winfo->is_current = Vwm.get.current_win (vwm) is this;

  winfo->frames = Alloc (sizeof (vframe_info) * this->num_items);
  vwm_frame *frame = this->head;
  int fidx = 0;

  while (frame and fidx < this->num_items) {
    winfo->frames[fidx++] = frame_get_info (frame);
    frame = frame->next;
  }

  return winfo;
}

static vwm_info *vwm_get_info (vwm_t *this) {
  vwm_info *vinfo = Alloc (sizeof (vwm_info));
  vinfo->pid = getpid ();
  vinfo->num_win = $my(num_items);
  vinfo->cur_win_idx = $my(cur_idx);
  vinfo->sequences_fname = (NULL is $my(sequences_fname) ? (char *) "" :
      $my(sequences_fname)->bytes);
  vinfo->unimplemented_fname = (NULL is $my(unimplemented_fname) ? (char *) "" :
      $my(unimplemented_fname)->bytes);

  vinfo->wins = Alloc (sizeof (vwin_info *) * $my(num_items));
  vwm_win *win = $my(head);
  int idx = 0;
  while (win and idx < vinfo->num_win) {
    vinfo->wins[idx++] = win_get_info (win);
    win = win->next;
  }

  return vinfo;
}

static vwm_win *vwm_pop_win_at (vwm_t *this, int idx) {
  return DListPopAt ($myprop, vwm_win, idx);
}

static int **vwm_alloc_ints (int rows, int cols, int val) {
  int **obj = Alloc (rows * sizeof (int *));

  for (int i = 0; i < rows; i++) {
    obj[i] = Alloc (sizeof (int *) * cols);

    for (int j = 0; j < cols; j++)
      obj[i][j] = val;
 }

 return obj;
}

static int vt_video_line_to_str (int *line, char *buf, int len) {
  int idx = 0;
  utf8 c;

  for (int i = 0; i < len; i++) {
    c = line[i];

    ifnot (c) continue;

    if ((c & 0xFF) < 0x80)
      buf[idx++] = c & 0xFF;
    else if (c < 0x800) {
      buf[idx++] = (c >> 6) | 0xC0;
      buf[idx++] = (c & 0x3F) | 0x80;
    } else if (c < 0x10000) {
      buf[idx++] = (c >> 12) | 0xE0;
      buf[idx++] = ((c >> 6) & 0x3F) | 0x80;
      buf[idx++] = (c & 0x3F) | 0x80;
    } else if (c < 0x110000) {
      buf[idx++] = (c >> 18) | 0xF0;
      buf[idx++] = ((c >> 12) & 0x3F) | 0x80;
      buf[idx++] = ((c >> 6) & 0x3F) | 0x80;
      buf[idx++] = (c & 0x3F) | 0x80;
    } else {
      continue;
    }
  }

  buf[idx++] = '\n';

  buf[idx] = '\0';

  return idx;
}

static void vt_write (char *buf, FILE *fp) {
  fprintf (fp, "%s", buf);
  fflush (fp);
}

static string_t *vt_insline (string_t *buf, int num) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%dL", num));
}

static string_t *vt_insertchar (string_t *buf, int numcols) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%d@", numcols));
}

static string_t *vt_savecursor (string_t *buf) {
  return String.append_with_len (buf, "\0337", 2);
}

static string_t *vt_restcursor (string_t *buf) {
  return String.append_with_len (buf, "\0338", 2);
}

static string_t *vt_clreol (string_t *buf) {
  return String.append_with_len (buf, "\033[K", 3);
}

static string_t *vt_clrbgl (string_t *buf) {
  return String.append_with_len (buf, "\033[1K", 4);
}

static string_t *vt_clrline (string_t *buf) {
  return String.append_with_len (buf, "\033[2K", 4);
}

static string_t *vt_delunder (string_t *buf, int num) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%dP", num));
}

static string_t *vt_delline (string_t *buf, int num) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%dM", num));
}

static string_t *vt_attr_reset (string_t *buf) {
  return String.append_with_len (buf, "\033[m", 3);
}

static string_t *vt_reverse (string_t *buf, int on) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%sm", (on ? "7" : "27")));
}

static string_t *vt_underline (string_t *buf, int on) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%sm", (on ? "4" : "24")));
}

static string_t *vt_bold (string_t *buf, int on) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%sm", (on ? "1" : "22")));
}

static string_t *vt_italic (string_t *buf, int on) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%sm", (on ? "3" : "23")));
}

static string_t *vt_blink (string_t *buf, int on) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%sm", (on ? "5" : "25")));
}

static string_t *vt_bell (string_t *buf) {
  return String.append_with_len (buf, "\007", 1);
}

static string_t *vt_setfg (string_t *buf, int color) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%d;1m", color));
}

static string_t *vt_setbg (string_t *buf, int color) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%d;1m", color));
}

static string_t *vt_left (string_t *buf, int count) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%dD", count));
}

static string_t *vt_right (string_t *buf, int count) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%dC", count));
}

static string_t *vt_up (string_t *buf, int numrows) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%dA", numrows));
}

static string_t *vt_down (string_t *buf, int numrows) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%dB", numrows));
}

static string_t *vt_irm (string_t *buf) {
  return String.append_with_len (buf, "\033[4l", 4);
}

static string_t *vt_revscroll (string_t *buf) {
  return String.append_with_len (buf, "\033M", 2);
}

static string_t *vt_setscroll (string_t *buf, int first, int last) {
  if (0 is first and 0 is last)
    return String.append_with_len (buf, "\033[r", 3);
  else
    return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%d;%dr", first, last));
}

static string_t *vt_goto (string_t *buf, int row, int col) {
  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%d;%dH", row, col));
}

static string_t *vt_attr_check (string_t *buf, int pixel, int lastattr, uchar *currattr) {
  uchar
    simplepixel,
    lastpixel,
    change,
    selected,
    reversed;

 /* Set the simplepixel REVERSE bit if SELECTED ^ REVERSE */
  simplepixel = ((pixel  >> 8) & (~SELECTED)  & (~REVERSE));
  selected    = (((pixel >> 8) & (~SELECTED)) ? 1 : 0);
  reversed    = (((pixel >> 8) & (~REVERSE))  ? 1 : 0);

  if (selected ^ reversed)
    simplepixel |= REVERSE;

  /* Set the lastpixel REVERSE bit if SELECTED ^ REVERSE */
  lastpixel = ((lastattr  >> 8) & (~SELECTED)  & (~REVERSE));
  selected  = (((lastattr >> 8) & (~SELECTED)) ? 1 : 0);
  reversed  = (((lastattr >> 8) & (~REVERSE))  ? 1 : 0);

  if (selected ^ reversed)
    lastpixel |= REVERSE;

 /* Thanks to Dan Dorough for the XOR code */
checkchange:
  change = (lastpixel ^ simplepixel);
  if (change) {
    if (change & REVERSE) {
      if ((*currattr) & REVERSE) {
#define GOTO_HACK          /* vt_reverse (0) doesn't work on xterms? */
#ifdef  GOTO_HACK          /* This goto hack resets all current attributes */
        vt_attr_reset (buf);
        *currattr &= ~REVERSE;
        simplepixel = 0;
        lastpixel &= (~REVERSE);
        goto checkchange;
#else
        vt_reverse (buf, 0);
        *currattr &= ~REVERSE;
#endif
      } else {
        vt_reverse (buf, 1);
        *currattr |= REVERSE;
      }
    }

    if (change & BOLD) {
      if ((*currattr) & BOLD) {
        vt_bold (buf, 0);
        *currattr &= ~BOLD;
      } else {
        vt_bold (buf, 1);
        *currattr |= BOLD;
      }
    }

    if (change & ITALIC) {
      if ((*currattr) & ITALIC) {
        vt_italic (buf, 0);
        *currattr &= ~ITALIC;
      } else {
        vt_italic (buf, 1);
        *currattr |= ITALIC;
      }
    }

    if (change & UNDERLINE) {
      if ((*currattr) & UNDERLINE) {
        vt_underline (buf, 0);
        *currattr &= ~UNDERLINE;
      } else {
        vt_underline (buf, 1);
        *currattr |= UNDERLINE;
      }
    }

    if (change & BLINK) {
      if ((*currattr) & BLINK) {
        vt_blink (buf, 0);
        *currattr &= ~BLINK;
      } else {
        vt_blink (buf, 1);
        *currattr |= BLINK;
      }
    }
  }

  return buf;
}

static string_t *vt_attr_set (string_t *buf, int textattr) {
  vt_attr_reset (buf);

  if (textattr & BOLD)
    vt_bold (buf, 1);

  if (textattr & UNDERLINE)
    vt_underline (buf, 1);

  if (textattr & BLINK)
    vt_blink (buf, 1);

  if (textattr & REVERSE)
    vt_reverse (buf, 1);

  if (textattr & ITALIC)
    vt_italic (buf, 1);

  return buf;
}

static void vt_video_add (vwm_frame *frame, utf8 c) {
  frame->videomem[frame->row_pos - 1][frame->col_pos - 1] = c;
  frame->videomem[frame->row_pos - 1][frame->col_pos - 1] |=
      (((int) frame->textattr) << 8);
}

static void vt_video_erase (vwm_frame *frame, int x1, int x2, int y1, int y2) {
  int i, j;

  for (i = x1 - 1; i < x2; ++i)
    for (j = y1 - 1; j < y2; ++j) {
      frame->videomem[i][j] = 0;
      frame->colors  [i][j] = COLOR_FG_NORMAL;
    }
}

static void vt_frame_video_rshift (vwm_frame *frame, int numcols) {
  for (int i = frame->num_cols - 1; i > frame->col_pos - 1; --i) {
    if (i - numcols >= 0) {
      frame->videomem[frame->row_pos-1][i] = frame->videomem[frame->row_pos-1][i-numcols];
      frame->colors[frame->row_pos-1][i] = frame->colors[frame->row_pos-1][i-numcols];
    } else {
      frame->videomem[frame->row_pos-1][i] = 0;
      frame->colors [frame->row_pos-1][i] = COLOR_FG_NORMAL;
    }
  }
}

static string_t *vt_frame_ech (vwm_frame *frame, string_t *buf, int num_cols) {
  for (int i = 0; i + frame->col_pos <= frame->num_cols and i < num_cols; i++) {
    frame->videomem[frame->row_pos-1][frame->col_pos - i - 1] = 0;
    frame->colors[frame->row_pos-1][frame->col_pos - i - 1] = COLOR_FG_NORMAL;
  }

  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%dX", num_cols));
}

/*
static string_t *vt_frame_cha (vwm_frame *frame, string_t *buf, int param) {
  if (param < 2)
    frame->col_pos = 1;
  else {
    if (param > frame->num_cols)
      param = frame->num_cols;
    frame->col_pos = param;
  }

  return String.append_with(buf, V_STR_FMT_LEN (MAX_SEQ_LEN, "\033[%dG", param));
}
*/

static void vt_frame_video_scroll (vwm_frame *frame, int numlines) {
  int *tmpvideo;
  int *tmpcolors;
  int n;

  for (int i = 0; i < numlines; i++) {
    tmpvideo = frame->videomem[frame->scroll_first_row - 1];
    tmpcolors = frame->colors[frame->scroll_first_row - 1];

    ifnot (NULL is frame->logfile) {
      char buf[(frame->num_cols * 3) + 2];
      int len = vt_video_line_to_str (tmpvideo, buf, frame->num_cols);
      IO.fd.write (frame->logfd, buf, len);
    }

    for (int j = 0; j < frame->num_cols; j++) {
      tmpvideo[j] = 0;
      tmpcolors[j] = COLOR_FG_NORMAL;
    }

    for (n = frame->scroll_first_row - 1; n < frame->last_row - 1; n++) {
      frame->videomem[n] = frame->videomem[n + 1];
      frame->colors[n] = frame->colors[n + 1];
    }

    frame->videomem[n] = tmpvideo;
    frame->colors[n] = tmpcolors;
  }
}

static void vt_frame_video_scroll_back (vwm_frame *frame, int numlines) {
  if (frame->row_pos < frame->scroll_first_row)
    return;

  int n;
  int *tmpvideo;
  int *tmpcolors;

  for (int i = 0; i < numlines; i++) {
    tmpvideo = frame->videomem[frame->last_row - 1];
    tmpcolors = frame->colors[frame->last_row - 1];

    for (int j = 0; j < frame->num_cols; j++) {
      tmpvideo[j] = 0;
      tmpcolors[j] = COLOR_FG_NORMAL;
    }

   for (n = frame->last_row - 1; n > frame->scroll_first_row - 1; --n) {
      frame->videomem[n] = frame->videomem[n - 1];
      frame->colors[n] = frame->colors[n - 1];
    }

    frame->videomem[n] = tmpvideo;
    frame->colors[n] = tmpcolors;
  }
}

static string_t *vt_frame_attr_set (vwm_frame *frame, string_t *buf) {
  uchar on = NORMAL;
  vt_attr_reset (buf);
  return vt_attr_check (buf, 0, frame->textattr, &on);
}

static string_t *vt_append (vwm_frame *frame, string_t *buf, utf8 c) {
  if (frame->col_pos > frame->num_cols) {
    if (frame->row_pos < frame->last_row)
      frame->row_pos++;
    else
      vt_frame_video_scroll (frame, 1);

    String.append_with(buf, "\r\n");
    frame->col_pos = 1;
  }

  vt_video_add (frame, c);

  if (frame->mb_len)
    String.append_with_len (buf, frame->mb_buf, frame->mb_len);
  else
    String.append_byte (buf, c);

  frame->col_pos++;
  return buf;
}

static string_t *vt_keystate_print (string_t *buf, int application) {
  if (application)
    return String.append_with(buf, "\033=\033[?1h");

  return String.append_with(buf, "\033>\033[?1l");
}

static string_t *vt_altcharset (string_t *buf, int charset, int type) {
  switch (type) {
    case UK_CHARSET:
      String.append_with_len (buf, V_STR_FMT_LEN (4, "\033%cA", (charset is G0 ? '(' : ')')), 3);
      break;

    case US_CHARSET:
      String.append_with_len (buf, V_STR_FMT_LEN (4, "\033%cB", (charset is G0 ? '(' : ')')), 3);
      break;

    case GRAPHICS:
      String.append_with_len (buf, V_STR_FMT_LEN (4, "\033%c0", (charset is G0 ? '(' : ')')), 3);
      break;

    default:  break;
  }
  return buf;
}

static string_t *vt_esc_scan (vwm_frame *, string_t *, int);

static void vt_frame_esc_set (vwm_frame *frame) {
  frame->process_char_cb = vt_esc_scan;

  for (int i = 0; i < MAX_PARAMS; i++)
    frame->esc_param[i] = 0;

  frame->param_idx = 0;
  frame->cur_param = &frame->esc_param[frame->param_idx];
}

static void frame_reset (vwm_frame *frame) {
  frame->row_pos = 1;
  frame->col_pos = 1;
  frame->saved_row_pos = 1;
  frame->saved_col_pos = 1;
  frame->scroll_first_row = 1;
  frame->last_row = frame->num_rows;
  frame->key_state = norm;
  frame->textattr = NORMAL;
  frame->saved_textattr = NORMAL;
  frame->charset[G0] = US_CHARSET;
  frame->charset[G1] = US_CHARSET;
  vt_frame_esc_set (frame);
}

static string_t *vt_esc_brace_q (vwm_frame *frame, string_t *buf, int c) {
  if (IS_DIGIT (c)) {
    *frame->cur_param *= 10;
    *frame->cur_param += (c - '0');
    return buf;
  }

  /* Return inside the switch to prevent reset_esc() */
  switch (c) {
    case '\030': /* Processed as escape cancel */
    case '\032': vt_frame_esc_set (frame);
    return buf;

    case 'h': /* Set modes */
      switch (frame->esc_param[0]) {
        case 1: /* Cursorkeys in application mode */
          frame->key_state = appl;
          vt_keystate_print (buf, frame->key_state);
          break;

        case 7:
          String.append_with_len (buf, TERM_AUTOWRAP_ON, TERM_AUTOWRAP_ON_LEN);
          break;

        case 25:
          String.append_with_len (buf, TERM_CURSOR_SHOW, TERM_CURSOR_SHOW_LEN);
          break;

        case 47:
          String.append_with_len (buf, TERM_SCREEN_SAVE, TERM_SCREEN_SAVE_LEN);
          break;

        case 2: /* Set ansi mode */
        case 3: /* 132 char/row */
        case 4: /* Set jump scroll */
        case 5: /* Set reverse screen */
        case 6: /* Set relative coordinates */
        case 8: /* Set auto repeat on */
        default:
          frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
          break;
      }
      break;

  case 'l': /* Reset modes */
    switch (frame->esc_param[0]) {
      case 1: /* Cursorkeys in normal mode */
        frame->key_state = norm;
        vt_keystate_print (buf, frame->key_state);
        break;

        case 7:
          String.append_with_len (buf, TERM_AUTOWRAP_OFF, TERM_AUTOWRAP_OFF_LEN);
          break;

      case 25:
        String.append_with_len (buf, TERM_CURSOR_HIDE, TERM_CURSOR_HIDE_LEN);
        break;

      case 47:
        String.append_with_len (buf, TERM_SCREEN_RESTORE, TERM_SCREEN_RESTORE_LEN);
        break;

      case 2: /* Set VT52 mode */
      case 3:
      case 4: /* Set smooth scroll */
      case 5: /* Set non-reversed (normal) screen */
      case 6: /* Set absolute coordinates */
      case 8: /* Set auto repeat off */
      default:
        frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
        break;
    }
    break;

    default:
      break;
   }

  vt_frame_esc_set (frame);
  return buf;
}

static string_t *vt_esc_lparen (vwm_frame *frame, string_t *buf, int c) {
  /* Return inside the switch to prevent reset_esc() */
  switch (c) {
    case '\030': /* Processed as escape cancel */
    case '\032':
      vt_frame_esc_set (frame);
      break;

    /* Select character sets */
    case 'A': /* UK as G0 */
      frame->charset[G0] = UK_CHARSET;
      vt_altcharset (buf, G0, UK_CHARSET);
      break;

    case 'B': /* US as G0 */
      frame->charset[G0] = US_CHARSET;
      vt_altcharset (buf, G0, US_CHARSET);
      break;

    case '0': /* Special character set as G0 */
      frame->charset[G0] = GRAPHICS;
      vt_altcharset (buf, G0, GRAPHICS);
      break;

    case '1': /* Alternate ROM as G0 */
    case '2': /* Alternate ROM special character set as G0 */
    default:
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      break;
  }

  vt_frame_esc_set (frame);
  return buf;
}

static string_t *vt_esc_rparen (vwm_frame *frame, string_t *buf, int c) {
  switch (c) {
    case '\030': /* Processed as escape cancel */
    case '\032':
      break;

    /* Select character sets */
    case 'A':
      frame->charset[G1] = UK_CHARSET;
      vt_altcharset (buf, G1, UK_CHARSET);
      break;

    case 'B':
      frame->charset[G1] = US_CHARSET;
      vt_altcharset (buf, G1, US_CHARSET);
      break;

    case '0':
      frame->charset[G1] = GRAPHICS;
      vt_altcharset (buf, G1, GRAPHICS);
      break;

    case '1': /* Alternate ROM as G1 */
    case '2': /* Alternate ROM special character set as G1 */
    default:
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      break;
  }

  vt_frame_esc_set (frame);
  return buf;
}

static string_t *vt_esc_pound (vwm_frame *frame, string_t *buf, int c) {
  switch (c)   /* Line attributes not supported */ {
    case '3':  /* Double height (top half) */
    case '4':  /* Double height (bottom half) */
    case '5':  /* Single width, single height */
    case '6':  /* Double width */
    default:
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      vt_frame_esc_set (frame);
      break;
  }

  return buf;
}

static string_t *vt_process_m (vwm_frame *frame, string_t *buf, int c) {
  int idx;

  switch (c) {
    case 0: /* Turn all attributes off */
      frame->textattr = NORMAL;
      vt_attr_reset (buf);
      idx = frame->num_cols - frame->col_pos - 1;
      if (0 <= idx)
        for (int i = 0; i < idx; i++)
          frame->colors[frame->row_pos -1][frame->col_pos - 1 + i] = COLOR_FG_NORMAL;
      break;

    case 1:
      frame->textattr |= BOLD;
      vt_bold (buf, 1);
      break;

    case 2: /* Half brightness */
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      break;

    case 3:
      frame->textattr |= ITALIC;
      vt_italic (buf, 1);
      break;

    case 4:
      frame->textattr |= UNDERLINE;
      vt_underline (buf, 1);
      break;

    case 5:
      frame->textattr |= BLINK;
      vt_blink (buf, 1);
      break;

    case 7:
      frame->textattr |= REVERSE;
      vt_reverse (buf, 1);
      break;

    case 21: /* Normal brightness */
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      break;

    case 22:
      frame->textattr &= ~BOLD;
      vt_bold (buf, 0);
      break;

    case 23:
      frame->textattr &= ~ITALIC;
      vt_italic (buf, 0);
      break;

    case 24:
      frame->textattr &= ~UNDERLINE;
      vt_underline (buf, 0);
      break;

    case 25:
      frame->textattr &= ~BLINK;
      vt_blink (buf, 0);
      break;

    case 27:
      frame->textattr &= ~REVERSE;
      vt_reverse (buf, 0);
      break;

    case 39:
      c = 30;

    // fall through
    case 30 ... 37:
      vt_setfg (buf, c);
      idx = frame->num_cols - frame->col_pos + 1;
      if (0 < idx)
        for (int i = 0; i < idx; i++)
          frame->colors[frame->row_pos - 1][frame->col_pos - 1 + i] = c;

      break;

    case 49:
      c = 47;

    // fall through
    case 40 ... 47:
      vt_setbg (buf, c);
      break;

    default:
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      break;
  }

  return buf;
}

static string_t *vt_esc_brace (vwm_frame *frame, string_t *buf, int c) {
  int
    i,
    newx,
    newy;

  char reply[128];

  if (IS_DIGIT (c)) {
    *frame->cur_param *= 10;
    *frame->cur_param += (c - '0');
    return buf;
  }

   /* Return inside the switch to prevent reset_esc() */
  switch (c) {
    case '\030': /* Processed as escape cancel */
    case '\032':
      break;

    case '?': /* Format should be \E[?<n> */
      if (*frame->cur_param) {

frame->unimplemented_cb (frame, "brace why", c, frame->esc_param[0]);
        vt_frame_esc_set (frame);
      } else {
        frame->process_char_cb = vt_esc_brace_q;
      }

      return buf;

    case ';':
      if (frame->param_idx + 1 < MAX_PARAMS)
        frame->cur_param = &frame->esc_param[++frame->param_idx];
      return buf;

    case 'h': /* Set modes */
      switch (frame->esc_param[0]) {
        case 2:  /* Lock keyboard */
        case 4:  /* Character insert mode */
        case 12: /* Local echo on */
        case 20: /* <Return> = CR */
        default:
          frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
          break;
      }
      break;

    case 'l': /* Reset modes */
      switch (frame->esc_param[0]) {
        case 4:  /* Character overstrike mode */
          vt_irm (buf); /* (ADDITION - unverified) */
          break;

        case 2:  /* Unlock keyboard */
        case 12: /* Local echo off */
        case 20: /* <Return> = CR-LF */
        default:
          frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
          break;
      }
      break;

    case 'r': /* Set scroll region */
      if (!frame->esc_param[0] and !frame->esc_param[1]) {
        frame->scroll_first_row = 1;
        frame->last_row = frame->num_rows;
      } else {
        /* Check parameters: VERY important. :) */
        if (frame->esc_param[0] < 1) /* Not needed */
          frame->scroll_first_row = 1;
        else
          frame->scroll_first_row = frame->esc_param[0];

        if (frame->esc_param[1] > frame->num_rows)
          frame->last_row = frame->num_rows;
        else
           frame->last_row = frame->esc_param[1];

        if (frame->scroll_first_row > frame->last_row) {
          /* Reset scroll region */
          frame->scroll_first_row = 1;
          frame->last_row = frame->num_rows;
        }
      }

      frame->row_pos = 1;
      frame->col_pos = 1;

      vt_setscroll (buf, frame->scroll_first_row + frame->first_row - 1,
        frame->last_row + frame->first_row - 1);
      vt_goto (buf, frame->row_pos + frame->first_row - 1, 1);
      break;

    case 'A': /* Cursor UP */
      if (frame->row_pos is frame->first_row)
        break;

      ifnot (frame->esc_param[0])
        frame->esc_param[0] = 1;

      newx = (frame->row_pos - frame->esc_param[0]);

      if (newx > frame->scroll_first_row) {
        frame->row_pos = newx;
        vt_up (buf, frame->esc_param[0]);
      } else {
        frame->row_pos = frame->scroll_first_row;
        vt_goto (buf, frame->row_pos + frame->first_row - 1,
          frame->col_pos);
      }
      break;

    case 'B': /* Cursor DOWN */
      if (frame->row_pos is frame->last_row)
        break;

      ifnot (frame->esc_param[0])
        frame->esc_param[0] = 1;

      newx = frame->row_pos + frame->esc_param[0];

      if (newx <= frame->last_row) {
        frame->row_pos = newx;
        vt_down (buf, frame->esc_param[0]);
      } else {
        frame->row_pos = frame->last_row;
        vt_goto (buf, frame->row_pos + frame->first_row - 1,
          frame->col_pos);
      }
      break;

    case 'C': /* Cursor RIGHT */
      if (frame->col_pos is frame->num_cols)
        break;

      ifnot (frame->esc_param[0])
        frame->esc_param[0] = 1;

      newy = (frame->col_pos + frame->esc_param[0]);

      if (newy < frame->num_cols) {
        frame->col_pos = newy;

        vt_right (buf, frame->esc_param[0]);
      } else {
        frame->col_pos = frame->num_cols;
        vt_goto (buf, frame->row_pos + frame->first_row - 1,
          frame->col_pos);
      }
      break;

    case 'D': /* Cursor LEFT */
      if (frame->col_pos is 1)
        break;

      ifnot (frame->esc_param[0])
        frame->esc_param[0] = 1;

      newy = (frame->col_pos - frame->esc_param[0]);

      if (newy > 1) {
        frame->col_pos = newy;
        vt_left (buf, frame->esc_param[0]);
      } else {
        frame->col_pos = 1;
        String.append_with(buf, "\r");
      }

      break;

    case 'G': { /* (hpa - horizontal) (ADDITION) */
      int col = frame->esc_param[0];
      int row = frame->esc_param[1];
      if (row <= 1) row = frame->row_pos;
      frame->esc_param[0] = row;
      frame->esc_param[1] = col;
    }
      break;

    case 'd': /* (vpa - HVP) (ADDITION) */
      frame->unimplemented_cb (frame, "ADDI param[0]", c, frame->esc_param[0]);
      frame->unimplemented_cb (frame, "ADDI param[1]", c, frame->esc_param[1]);
      frame->unimplemented_cb (frame, "ADDI row_pos", c, frame->row_pos);
      frame->unimplemented_cb (frame, "ADDI col_pos", c, frame->col_pos);
      break;

    case 'f':
    case 'H': /* Move cursor to coordinates */
      ifnot (frame->esc_param[0])
        frame->esc_param[0] = 1;

      ifnot (frame->esc_param[1])
        frame->esc_param[1] = 1;

      if ((frame->row_pos = frame->esc_param[0]) >
          frame->num_rows)
        frame->row_pos = frame->num_rows;

      if ((frame->col_pos = frame->esc_param[1]) >
          frame->num_cols)
        frame->col_pos = frame->num_cols;

      vt_goto (buf, frame->row_pos + frame->first_row - 1,
        frame->col_pos);
      break;

    case 'g': /* Clear tabstops */
      switch (frame->esc_param[0]) {
        case 0: /* Clear a tabstop */
          frame->tabstops[frame->col_pos-1] = 0;
          break;

        case 3: /* Clear all tabstops */
          for (newy = 0; newy < frame->num_cols; ++newy)
            frame->tabstops[newy] = 0;
          break;

        default:
          frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
          break;
      }
      break;

    case 'm': /* Set terminal attributes */
      vt_process_m (frame, buf, frame->esc_param[0]);
      for (i = 1; frame->esc_param[i] and i < MAX_PARAMS; i++)
        vt_process_m (frame, buf, frame->esc_param[i]);
      break;

    case 'J': /* Clear screen */
      switch (frame->esc_param[0]) {
        case 0: /* Clear from cursor down */
          vt_video_erase (frame, frame->row_pos,
            frame->num_rows, 1, frame->num_cols);

          newx = frame->row_pos;
          vt_savecursor (buf);
          String.append_with(buf, "\r");

          while (newx++ < frame->num_rows) {
            vt_clreol (buf);
            String.append_with(buf, "\n");
          }

          vt_clreol (buf);
          vt_restcursor (buf);
          break;

        case 1: /* Clear from cursor up */
          vt_video_erase (frame, 1, frame->row_pos,
            1, frame->num_cols);

          newx = frame->row_pos;
          vt_savecursor (buf);
          String.append_with(buf, "\r");

          while (--newx > 0) {
            vt_clreol (buf);
            vt_up (buf, 1);
          }

          vt_clreol (buf);
          vt_restcursor (buf);
          break;

        case 2: /* Clear whole screen */
          vt_video_erase (frame, 1, frame->num_rows,
            1, frame->num_cols);

          vt_goto (buf, frame->first_row + 1 - 1, 1);
          frame->row_pos = 1;
          frame->col_pos = 1;
          newx = frame->row_pos;
          vt_savecursor (buf);
          String.append_with(buf, "\r");

          while (newx++ < frame->num_rows) {
            vt_clreol (buf);
            String.append_with(buf, "\n");
          }

          vt_clreol (buf);
          vt_restcursor (buf);
          break;

        default:
          frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
          break;
      }
      break;

    case 'K': /* Clear line */
      switch (frame->esc_param[0]) {
        case 0: /* Clear to end of line */
          vt_video_erase (frame, frame->row_pos,
            frame->row_pos, frame->col_pos, frame->num_cols);

          vt_clreol (buf);
        break;

        case 1: /* Clear to beginning of line */
          vt_video_erase (frame, frame->row_pos,
            frame->row_pos, 1, frame->col_pos);

          vt_clrbgl (buf);
          break;

        case 2: /* Clear whole line */
          vt_video_erase (frame, frame->row_pos,
            frame->row_pos, 1, frame->num_cols);

          vt_clrline (buf);
          break;
        }
      break;

    case 'P': /* Delete under cursor */
      vt_video_erase (frame, frame->row_pos,
        frame->row_pos, frame->col_pos, frame->col_pos);

      vt_delunder (buf, frame->esc_param[0]);
      break;

    case 'M': /* Delete lines */
      vt_frame_video_scroll_back (frame, 1);
      vt_delline (buf, frame->esc_param[0]);
      break;

    case 'L': /* Insert lines */
      vt_insline (buf, frame->esc_param[0]);
      break;

    case '@': /* Insert characters */
      ifnot (frame->esc_param[0])
        frame->esc_param[0] = 1;

      vt_insertchar (buf, frame->esc_param[0]);
      vt_frame_video_rshift (frame, frame->esc_param[0]);
      break;

    case 'i': /* Printing */
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      break;

    case 'n': /* Device status request */
      switch (frame->esc_param[0]) {
        case 5: /* Status report request */
          /* Say we're just fine. */
          write (frame->fd, "\033[0n", 4);
          break;

        case 6: /* Cursor position request */
          sprintf (reply, "\033[%d;%dR", frame->row_pos,
              frame->col_pos);

          write (frame->fd, reply, bytelen (reply));
          break;
        }
        break;

    case 'c': /* Request terminal identification string_t */
      /* Respond with "I am a vt102" */
      write (frame->fd, "\033[?6c", 5);
      break;

    case 'X': /* (ECH) Erase param chars (ADDITION) */
      vt_frame_ech (frame, buf, frame->esc_param[0]);
      break;

    //case 'G':
       /* (CHA) Cursor to column param (ADDITION) */
     // vt_frame_cha (frame, buf, frame->esc_param[0]);
     // break;

    default:
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      break;
  }

  vt_frame_esc_set (frame);
  return buf;
}

static string_t *vt_esc_e (vwm_frame *frame, string_t *buf, int c) {
  /* Return inside the switch to prevent reset_esc() */
  switch (c) {
    case '\030': /* Processed as escape cancel */
    case '\032':
      break;

    case '[':
      frame->process_char_cb = vt_esc_brace;
      return buf;

    case '(':
      frame->process_char_cb = vt_esc_lparen;
      return buf;

    case ')':
      frame->process_char_cb = vt_esc_rparen;
      return buf;

    case '#':
      frame->process_char_cb = vt_esc_pound;
      return buf;

    case 'D': /* Cursor down with scroll up at margin */
      if (frame->row_pos < frame->last_row)
        frame->row_pos++;
      else
        vt_frame_video_scroll (frame, 1);

      String.append_with(buf, "\n");
      break;

    case 'M': /* Reverse scroll (move up; scroll at top) */
      if (frame->row_pos > frame->scroll_first_row)
        --frame->row_pos;
      else
        vt_frame_video_scroll_back (frame, 1);

      vt_revscroll (buf);
      break;

    case 'E': /* Next line (CR-LF) */
      if (frame->row_pos < frame->last_row)
        frame->row_pos++;
      else
        vt_frame_video_scroll (frame, 1);

      frame->col_pos = 1;
      String.append_with(buf, "\r\n");
      break;

    case '7': /* Save cursor and attribute */
      frame->saved_row_pos = frame->row_pos;
      frame->saved_col_pos = frame->col_pos;
      frame->saved_textattr = frame->textattr;
      break;

    case '8': /* Restore saved cursor and attribute */
      frame->row_pos = frame->saved_row_pos;
      frame->col_pos = frame->col_pos;
      if (frame->row_pos > frame->num_rows)
        frame->row_pos = frame->num_rows;

      if (frame->col_pos > frame->num_cols)
        frame->col_pos = frame->num_cols;

      vt_goto (buf, frame->row_pos + frame->first_row - 1,
        frame->col_pos);

      frame->textattr = frame->saved_textattr;
      vt_frame_attr_set (frame, buf);
      break;

    case '=': /* Set application keypad mode */
      frame->key_state = appl;
      vt_keystate_print (buf, frame->key_state);
      break;

    case '>': /* Set numeric keypad mode */
      frame->key_state = norm;
      vt_keystate_print (buf, frame->key_state);
      break;

    case 'N': /* Select charset G2 for one character */
    case 'O': /* Select charset G3 for one character */
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      break;

    case 'H': /* Set horizontal tab */
      frame->tabstops[frame->col_pos - 1] = 1;
      break;

    case 'Z': /* Request terminal identification string_t */
      /* Respond with "I am a vt102" */
      write (frame->fd, "\033[?6c", 5);
      break;

    case 'c': /* Terminal reset */
      frame_reset (frame);
      break;

    default:
      frame->unimplemented_cb (frame, __func__, c, frame->esc_param[0]);
      break;
  }

  vt_frame_esc_set (frame);
  return buf;
}

static string_t *vt_esc_scan (vwm_frame *frame, string_t *buf, int c) {
  switch (c) {
    case '\000': /* NULL (fill character) */
      break;

    case '\003': /* EXT  (half duplex turnaround) */
    case '\004': /* EOT  (can be disconnect char) */
    case '\005': /* ENQ  (generate answerback) */
      String.append_byte (buf, c);
      break;

    case '\007': /* BEL  (sound terminal bell) */
      vt_bell (buf);
      break;

    case '\b': /* Backspace; move left one character */
      ifnot (1 is frame->col_pos) {
        --frame->col_pos;
        vt_left (buf, frame->esc_param[0]);
      }
      break;

    case '\t': /* Tab.  Handle with direct motion (Buggy) */
      {
        int i = frame->col_pos;
        do {
          ++frame->col_pos;
        } while (0 is frame->tabstops[frame->col_pos - 1]
            and (frame->col_pos < frame->num_cols));

        vt_right (buf, frame->col_pos - i);
      }
      break;

    case '\013': /* Processed as linefeeds */
    case '\014': /* Don't let the cursor move below winow or scrolling region */
    case '\n':
      if (frame->row_pos < frame->last_row)
        frame->row_pos++;
      else
        vt_frame_video_scroll (frame, 1);

      String.append_with(buf, "\n");
      break;

    case '\r': /* Move cursor to left margin */
      frame->col_pos = 1;
      String.append_with(buf, "\r");
      break;

    case '\016': /* S0 (selects G1 charset) */
    case '\017': /* S1 (selects G0 charset) */
    case '\021': /* XON (continue transmission) */
    case '\022': /* XOFF (stop transmission) */
      String.append_byte (buf, c);
      break;

    case '\030': /* Processed as escape cancel */
    case '\032':
      vt_frame_esc_set (frame);
      break;

    case '\033':
      frame->process_char_cb = vt_esc_e;
      return buf;

    default:
      if (c >= 0x80 or frame->mb_len) {
        if (frame->mb_len > 0) {
          frame->mb_buf[frame->mb_curlen++] = c;
          frame->mb_code <<= 6;
          frame->mb_code += c;

          if (frame->mb_curlen isnot frame->mb_len)
            return buf;

          frame->mb_code -= offsetsFromUTF8[frame->mb_len-1];

          vt_append (frame, buf, frame->mb_code);
          frame->mb_buf[0] = '\0';
          frame->mb_curlen = frame->mb_len = frame->mb_code = 0;
          return buf;
        } else {
          frame->mb_code = c;
          frame->mb_len = ({
            uchar uc = 0;
            if ((c & 0xe0) is 0xc0)
              uc = 2;
            else if ((c & 0xf0) is 0xe0)
              uc = 3;
            else if ((c & 0xf8) is 0xf0)
              uc = 4;
            else
              uc = -1;

            uc;
            });
          frame->mb_buf[0] = c;
          frame->mb_curlen = 1;
          return buf;
        }
      } else
        vt_append (frame, buf, c);
  }

  return buf;
}

static void vt_video_add_log_lines (vwm_frame *this) {
  struct stat st;
  if (-1 is this->logfd or -1 is fstat (this->logfd, &st))
    return;

  long size = st.st_size;

  char *mbuf = mmap (0, size, PROT_READ, MAP_SHARED, this->logfd, 0);

  if (NULL is mbuf) return;

  char *buf = mbuf + size - 1;

  int lines = this->num_rows;

  for (int i = 0; i < lines; i++)
    for (int j = 0; j < this->num_cols; j++)
      this->videomem[i][j] = 0;

  while (lines isnot 0 and size) {
    char b[BUFSIZE];
    char c;
    int rbts = 0;
    while (--size) {
      c = *--buf;

      if (c is '\n') break;

      ifnot (c) continue;
      b[rbts++] = c;
    }

    b[rbts] = '\0';

    int blen = bytelen (b);

    char nbuf[blen + 1];
    for (int i = 0; i < blen; i++)
      nbuf[i] = b[blen - i - 1];

    nbuf[blen] = '\0';

    int idx = 0;
    int len = 0;
    for (int i = 0; i < this->num_cols; i++) {
      if (idx >= blen) break;

      this->videomem[lines-1][i] = Ustring.get.code_at (nbuf, blen, idx, &len);
      idx += len;
      //   (int) ustring_to_code (nbuf, &idx);
    }

    lines--;
  }

  ftruncate (this->logfd, size);
  lseek (this->logfd, size, SEEK_SET);
  munmap (0, st.st_size);
}

static void frame_on_resize (vwm_frame *this, int rows, int cols) {
  int **videomem = vwm_alloc_ints (rows, cols, 0);
  int **colors = vwm_alloc_ints (rows, cols, COLOR_FG_NORMAL);
  int row_pos = 0;
  int i, j, nj, ni;

  int last_row = this->num_rows;
  if (rows < last_row)
    while (last_row > rows and 0 is this->videomem[last_row-1][0])
      last_row--;

  for (i = last_row, ni = rows; i and ni; i--, ni--) {
    if (this->row_pos is i)
      if ((row_pos = i + (this->num_rows - last_row) + rows - this->num_rows) < 1)
        row_pos = 1;

    for (j = 0, nj = 0; (j < this->num_cols) and (nj < cols); j++, nj++) {
      videomem[ni-1][nj] = this->videomem[i-1][j];
      colors[ni-1][nj] = this->colors[i-1][j];
    }
  }

  ifnot (row_pos) /* We never reached the old cursor */
    row_pos = 1;

  this->row_pos = row_pos;
  this->col_pos = (this->col_pos > cols ? cols : this->col_pos);

  for (i = 0; i < this->num_rows; i++)
    free (this->videomem[i]);
  free (this->videomem);

  for (i = 0; i < this->num_rows; i++)
    free (this->colors[i]);
  free (this->colors);

  this->videomem = videomem;
  this->colors = colors;
}

static void win_set_frame (vwm_win *this, vwm_frame *frame) {
  String.clear (frame->render);

  vt_setscroll (frame->render, frame->scroll_first_row + frame->first_row - 1,
      frame->last_row + frame->first_row - 1);

  if (this->draw_separators) {
    this->draw_separators = 0;
    String.append_with_len (frame->render, this->separators_buf->bytes, this->separators_buf->num_bytes);
  }

  vt_goto (frame->render, frame->row_pos + frame->first_row - 1, frame->col_pos);

  ifnot (NULL is this->last_frame) {
    ifnot (frame->key_state is this->last_frame->key_state)
      vt_keystate_print (frame->render, frame->key_state);

    ifnot (frame->textattr is this->last_frame->textattr)
      vt_attr_set (frame->render, frame->textattr);

    for (int i = 0; i < NCHARSETS; i++)
      if (frame->charset[i] isnot this->last_frame->charset[i])
        vt_altcharset (frame->render, i, frame->charset[i]);
  }

  vt_write (frame->render->bytes, stdout);
}

static void frame_process_output (vwm_frame *this, char *buf, int len) {
  this->process_output_cb (this, buf, len);
}

#ifndef DEBUG
static void frame_process_output_cb (vwm_frame *this, char *buf, int len) {
  String.clear (this->render);

  while (len--)
    this->process_char_cb (this, this->render, (uchar) *buf++);

  vt_write (this->render->bytes, stdout);
}
#else
static void frame_process_output_cb (vwm_frame *this, char *buf, int len) {
  String.clear (this->render);

  FILE *fout = this->root->prop->sequences_fp;

  fprintf (fout, "\n%s\n\n", buf);

  char seq_buf[MAX_SEQ_LEN + 1];
  int seq_idx = 1;
  seq_buf[0] = '\0';

  while (len--) {

    if (this->process_char_cb is vt_esc_scan) {
      ifnot (seq_idx) goto proceed;

      fprintf (fout, "ESC %s\n", seq_buf);

      seq_idx = 0;
      memset (seq_buf, 0, MAX_SEQ_LEN+1);
    } else
      seq_buf[seq_idx++] = *buf;

proceed:
    this->process_char_cb (this, this->render, (uchar) *buf++);
  }

  if (seq_idx)
    fprintf (fout, "ESC %s\n", seq_buf);

  fflush (fout);

  vt_write (this->render->bytes, stdout);
}
#endif /* DEBUG */

static void argv_release (char **argv, int *argc) {
  for (int i = 0; i <= *argc; i++) free (argv[i]);
  free (argv);
  *argc = 0;
  argv = NULL;
}

static void frame_release_argv (vwm_frame *this) {
  if (NULL is this->argv) return;
  argv_release (this->argv, &this->argc);
}

static char **parse_command (const char *command, int *argc) {
  char *sp = (char *) command;
  char *tokbeg;
  size_t len;

  *argc = 0;
  char **argv = Alloc (sizeof (char *));

  while (*sp) {
    while (*sp and *sp is ' ') sp++;
    ifnot (*sp) break;

    tokbeg = sp;

    if (*sp is '"') {
      sp++;
      tokbeg++;

parse_quoted:
      while (*sp and *sp isnot '"') sp++;
      ifnot (*sp) goto theerror;
      if (*(sp - 1) is '\\') goto parse_quoted;
      len = (size_t) (sp - tokbeg);
      sp++;
      goto add_arg;
    }

    while (*sp and *sp isnot ' ') sp++;

    len = (size_t) (sp - tokbeg);

add_arg:
    *argc += 1;
    argv = Realloc (argv, sizeof (char *) * ((*argc) + 1));
    argv[*argc - 1] = Alloc (len + 1);
    Cstring.cp (argv[*argc - 1], len + 1, tokbeg, len);

    ifnot (*sp) break;
    sp++;
  }

  argv[*argc] = (char *) NULL;

  return argv;

theerror:
  argv_release (argv, argc);
  return NULL;
}

static void frame_set_command (vwm_frame *this, const char *command) {
  if (NULL is command or 0 is bytelen (command))
    return;

  self(release_argv);
  this->argv = parse_command (command, &this->argc);
}

static void frame_set_visibility (vwm_frame *this, int visibility) {
  if (this->is_visible) {
    ifnot (visibility) {
      this->parent->num_visible_frames--;
      this->parent->num_separators--;
    }
  } else {
    if (visibility) {
      this->parent->num_visible_frames++;
      this->parent->num_separators++;
    }
  }

  this->is_visible = (0 isnot visibility);
}

static void frame_set_argv (vwm_frame *this, int argc, const char **argv) {
  if (argc <= 0) return;

  self(release_argv);

  this->argv = Alloc (sizeof (char *) * (argc + 1));
  for (int i = 0; i < argc; i++) {
    size_t len = bytelen (argv[i]);
    this->argv[i] = Alloc (len + 1);
    Cstring.cp (this->argv[i], len + 1, argv[i], len);
  }

  this->argv[argc] = NULL;
  this->argc = argc;
}

static void frame_set_fd (vwm_frame *this, int fd) {
  this->fd = fd;
}

static int frame_get_fd (vwm_frame *this) {
  return this->fd;
}

static pid_t frame_get_pid (vwm_frame *this) {
  return this->pid;
}

static int frame_get_visibility (vwm_frame *this) {
  return this->is_visible;
}

static int frame_get_remove_log (vwm_frame *this) {
  return this->remove_log;
}

static int frame_get_argc (vwm_frame *this) {
  return this->argc;
}

static char **frame_get_argv (vwm_frame *this) {
  return this->argv;
}

static vwm_win *frame_get_parent (vwm_frame *this) {
  return this->parent;
}

static vwm_t *frame_get_root (vwm_frame *this) {
  return this->root;
}

static int frame_get_num_rows (vwm_frame *this) {
  return this->num_rows;
}

static int frame_get_logfd (vwm_frame *this) {
  return this->logfd;
}

static char *frame_get_logfile (vwm_frame *this) {
  if (NULL is this->logfile) return NULL;
  return this->logfile->bytes;
}

static void frame_clear (vwm_frame *this, int state) {
  if (NULL is this) return;

  string_t *render = this->render;

  String.clear (render);
  vt_goto (render, this->first_row, 1);

  for (int i = 0; i < this->num_rows; i++) {
    for (int j = 0; j < this->num_cols; j++) {
      if (state & VFRAME_CLEAR_VIDEO_MEM) {
        this->videomem[i][j] = ' ';
        this->colors[i][j] = COLOR_FG_NORMAL;
      }

      String.append_byte (render, ' ');
    }

    String.append_with(render, "\r\n");
  }

  // clear the last newline, otherwise it scrolls one line more
  String.clear_at (render, -1); // this is visible when there is one frame

  if (state & VFRAME_CLEAR_LOG)
    if (this->logfd isnot -1)
      ftruncate (this->logfd, 0);

  vt_write (render->bytes, stdout);
}

static int frame_check_pid (vwm_frame *this) {
  if (NULL is this or -1 is this->pid) return -1;

  ifnot (0 is waitpid (this->pid, &this->status, WNOHANG)) {
    this->pid = -1;
    this->fd = -1;
    int state = (VFRAME_CLEAR_VIDEO_MEM|
      (this->logfd isnot -1 ?
        (this->remove_log ? VFRAME_CLEAR_LOG : 0) :
        0));
    self(clear, state);
    return 0;
  }

  return this->pid;
}

static int frame_kill_proc (vwm_frame *this) {
  if (this is NULL or this->pid is -1) return -1;

  int state = (VFRAME_CLEAR_VIDEO_MEM|
    (this->logfd isnot -1 ?
      (this->remove_log ? VFRAME_CLEAR_LOG : 0) :
      0));
  self(clear, state);

  kill (this->pid, SIGHUP);
  waitpid (this->pid, NULL, 0);
  this->pid = -1;
  this->fd = -1;
  return 0;
}

static FrameProcessOutput_cb frame_set_process_output_cb (vwm_frame *this, FrameProcessOutput_cb cb) {
  FrameProcessOutput_cb prev = this->process_output_cb;
  this->process_output_cb = cb;
  return prev;
}

static FrameAtFork_cb frame_set_at_fork_cb (vwm_frame *this, FrameAtFork_cb cb) {
  FrameAtFork_cb prev = this->at_fork_cb;
  this->at_fork_cb = cb;
  return prev;
}

static void frame_set_unimplemented_cb (vwm_frame *this, FrameUnimplemented_cb cb) {
  this->unimplemented_cb = cb;
}

static int frame_set_log (vwm_frame *this, const char *fname, int remove_log) {
  self(release_log);

  if (NULL is fname or fname[0] is '\0') {
    if (NULL is this->root->prop->tmpdir)
      return NOTOK;

    tmpfname_t *t = File.tmpfname.new (this->root->prop->tmpdir->bytes, "libvwm");

    if (-1 is t->fd)
      return NOTOK;

    this->logfd = t->fd;
    this->logfile = String.dup (t->fname);
    this->remove_log = remove_log;

    File.tmpfname.release (t, 0);

    return this->logfd;
  }

  this->logfd = open (fname, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);

  if (this->logfd is NOTOK) return NOTOK;

  if (-1 is fchmod (this->logfd, 0600)) return NOTOK;

  this->logfile = String.new_with (fname);
  this->remove_log = remove_log;
  return this->logfd;
}

static int frame_at_fork_default_cb (vwm_frame *this, vwm_t *root, vwm_win *parent) {
  (void) this; (void) parent; (void) root;
  return 1;
}

static void frame_unimplemented_default_cb (vwm_frame *this, const char *fun, int c, int param) {
  if (this->root->prop->unimplemented_fp isnot NULL) {
    fprintf (this->root->prop->unimplemented_fp, "|%s| %c %d| param: %d\n", fun, c, c, param);
    fflush  (this->root->prop->unimplemented_fp);
  }
}

static int win_insert_frame_at (vwm_win *this, vwm_frame *frame, int idx) {
  return DListInsertAt (this, frame, idx);
}

static int win_append_frame (vwm_win *this, vwm_frame *frame) {
  return DListAppend (this, frame);
}

static vwm_frame *win_init_frame (vwm_win *this, frame_opts opts) {
  vwm_frame *frame = Alloc (sizeof (vwm_frame));

  frame->parent = (opts.parent isnot NULL ? opts.parent : this);

  if (frame->parent isnot NULL) {
    frame->root = frame->parent->parent;
    frame->win =  frame->parent->self;
    frame->self = frame->parent->frame;
  }

  frame->pid = opts.pid;
  frame->fd = opts.fd;
  frame->at_frame = opts.at_frame;
  frame->logfile = NULL;
  frame->remove_log = opts.remove_log;
  frame->is_visible = opts.is_visible;
  frame->num_rows = opts.num_rows;
  frame->first_row = opts.first_row;

  frame->num_cols = (opts.num_cols isnot -1 ? opts.num_cols :
    (NULL is this ? 78 : this->num_cols));
  frame->first_col = (opts.first_col isnot -1 ? opts.first_col :
    (NULL is this ? 1 : this->first_col));

  ifnot (NULL is opts.argv)
    Vframe.set.argv (frame, opts.argc, opts.argv);
  else
    if (NULL isnot opts.command)
      Vframe.set.command (frame, opts.command);

  frame->logfd = -1;

  if (opts.enable_log)
    Vframe.set.log (frame, opts.logfile, frame->remove_log);

  frame->mb_buf[0] = '\0';
  frame->mb_curlen = frame->mb_len = frame->mb_code = 0;
  frame->render = String.new (2048);
  frame->state = 0;

  frame->process_output_cb = (NULL is opts.process_output_cb ?
      frame_process_output_cb : opts.process_output_cb);

  frame->at_fork_cb = (NULL is opts.at_fork_cb ?
      frame_at_fork_default_cb : opts.at_fork_cb);

  frame->unimplemented_cb = frame_unimplemented_default_cb;

  frame->videomem = vwm_alloc_ints (frame->num_rows, frame->num_cols, 0);
  frame->colors = vwm_alloc_ints (frame->num_rows, frame->num_cols, COLOR_FG_NORMAL);
  frame->esc_param = Alloc (sizeof (int) * MAX_PARAMS);
  for (int i = 0; i < MAX_PARAMS; i++) frame->esc_param[i] = 0;
  frame->tabstops = Alloc (sizeof (int) * frame->num_cols);
  for (int i = 0; i < frame->num_cols; i++) {
    ifnot ((int) i % TABWIDTH)
      frame->tabstops[i] = 1;
    else
      frame->tabstops[i] = 0;
  }

  Vframe.reset (frame);

  if (opts.create_fd)
    Vframe.create_fd (frame);

  if (opts.fork and frame->argc)
    Vframe.fork (frame);

  return frame;
}

static vwm_frame *win_new_frame (vwm_win *this, frame_opts opts) {
  vwm_frame *frame = self(init_frame, opts);

  if (frame->at_frame < 0 or frame->at_frame > this->num_items)
    self(append_frame, frame);
  else
    self(insert_frame_at, frame, frame->at_frame - 1);

  if (frame->is_visible) {
    this->num_visible_frames++;
    this->num_separators++;

    int at = 1;
    vwm_frame *it = this->head;
    while (it) {
      if (it is frame) break;
      at += (it->is_visible isnot 0);
      it = it->next;
    }

    frame->at_frame = at;

  }

  return frame;
}

static vwm_frame *win_pop_frame_at (vwm_win *this, int idx) {
  return DListPopAt (this, vwm_frame, idx);
}

static vwm_frame *win_add_frame (vwm_win *this, int argc, const char **argv, int draw) {
  int num_frames = self(get.num_visible_frames);

  if (num_frames is this->max_frames) return NULL;

  int frame_rows = 0;
  int mod = self(frame_rows, num_frames + 1, &frame_rows);
  int
    num_rows = frame_rows + mod,
    first_row = this->first_row;

  vwm_frame *frame = this->head;
  while (frame) {
    ifnot (frame->is_visible) goto next_frame;

    frame->new_rows = num_rows;
    first_row += num_rows + 1;
    num_rows = frame_rows;

    next_frame: frame = frame->next;
  }

  frame = self(new_frame, FrameOpts (
      .num_rows = num_rows,
      .first_row = first_row,
      .argc = argc,
      .argv = argv));

  frame->new_rows = num_rows;

  self(set.current_at, this->num_items - 1);
  self(on_resize, draw);
  return frame;
}

static int win_frame_rows (vwm_win *this, int num_frames, int *frame_rows) {
  int avail_rows = this->num_rows - (num_frames - 1);
  *frame_rows = avail_rows / num_frames;
  return avail_rows % num_frames;
}

static int win_delete_frame (vwm_win *this, vwm_frame *frame, int draw) {
  ifnot (this->num_items)
    return OK;

  if (1 is this->num_items) {
    Vframe.kill_proc (frame);;
    self(release_frame_at, 0);
  } else if (0 is frame->is_visible) {
    int idx = self(get.frame_idx, frame);
    Vframe.kill_proc (frame);
    self(release_frame_at, idx);
  } else {
    int is_last_frame = this->last_frame is frame;
    int idx = self(get.frame_idx, frame);
    Vframe.kill_proc (frame);
    self(release_frame_at, idx);

    int num_frames = self(get.num_visible_frames);
    ifnot (num_frames) return OK;

    int frame_rows = 0;
    int mod = self(frame_rows, num_frames, &frame_rows);
    int
      num_rows = frame_rows + mod,
      first_row = this->first_row;

    frame = this->head;
    while (frame) {
      ifnot (frame->is_visible) goto next_frame;

      frame->new_rows = num_rows;
      first_row += num_rows + 1;
      num_rows = frame_rows;

      next_frame: frame = frame->next;
    }

    if (is_last_frame or this->current is this->last_frame) {
      if (1 is num_frames)
        this->last_frame = this->current;
      else {
        frame = this->head;
        while (frame) {
          if (frame isnot this->current) {
            this->last_frame = frame;
            break;
          }

          frame = frame->next;
        }
      }
    }


    self(on_resize, draw);
  }

  return OK;
}

static void frame_release_log (vwm_frame *this) {
  if (NULL is this->logfile) return;

  if (this->remove_log)
    unlink (this->logfile->bytes);

  String.release (this->logfile);
  this->logfile = NULL;

  close (this->logfd);
  this->logfd = -1;
}

static void win_release_frame_at (vwm_win *this, int idx) {
  vwm_frame *frame = self(pop_frame_at, idx);

  if (NULL is frame) return;

  if (frame->is_visible) {
    this->num_separators--;
    this->num_visible_frames--;

    vwm_frame *it = this->head;
    while (it) {
      if (it->is_visible)
        if (frame->at_frame < it->at_frame)
          it->at_frame--;

      it = it->next;
    }
  }

  Vframe.release_log (frame);

  for (int i = 0; i < frame->num_rows; i++)
    free (frame->videomem[i]);
  free (frame->videomem);

  for (int i = 0; i < frame->num_rows; i++)
    free (frame->colors[i]);
  free (frame->colors);

  free (frame->tabstops);
  free (frame->esc_param);

  Vframe.release_argv (frame);
  String.release (frame->render);

  ifnot (-1 is frame->pid) {
    kill (frame->pid, SIGHUP);
    waitpid (frame->pid, NULL, 0);
  }

  free (frame);
}

static void vwm_make_separator (string_t *render, const char *color, int cells, int row, int col) {
  vt_goto (render, row, col);
  String.append_with(render, color);
  for (int i = 0; i < cells; i++)
    String.append_with_len (render, "—", 3);

  vt_attr_reset (render);
}

static int win_set_separators (vwm_win *this, int draw) {
  String.clear (this->separators_buf);

  ifnot (this->num_separators) return NOTOK;

  vwm_frame *prev = this->head;
  vwm_frame *frame = this->head->next;
  while (prev->is_visible is 0) {
    prev = frame;
    frame = frame->next;
  }

  int num = 0;

  while (num < this->num_separators) {
    ifnot (frame->is_visible) goto next_frame;

    num++;
    vwm_make_separator (this->separators_buf,
       (prev is this->current ? COLOR_FOCUS : COLOR_UNFOCUS),
        frame->num_cols, prev->first_row + prev->last_row, frame->first_col);

    prev = frame;
    next_frame: frame = frame->next;
  }

  if (DRAW is draw)
    vt_write (this->separators_buf->bytes, stdout);

  return OK;
}

static vwm_frame *win_set_current_at (vwm_win *this, int idx) {
  DListSetCurrent (this, idx);
  return this->current;
}

static vwm_frame *win_set_frame_as_current (vwm_win *this, vwm_frame *frame) {
  int idx = DListGetIdx (this, vwm_frame, frame);
  return win_set_current_at (this, idx);
}

static void win_draw (vwm_win *this) {
  char buf[8];

  int
    len = 0,
    oldattr = 0,
    oldclr = COLOR_FG_NORMAL,
    clr = COLOR_FG_NORMAL;

  uchar on = NORMAL;
  utf8 chr = 0;

  string_t *render = this->render;
  String.clear (render);
  String.append_with(render, TERM_SCREEN_CLEAR);
  vt_setscroll (render, 0, 0);
  vt_attr_reset (render);
  vt_setbg (render, COLOR_BG_NORMAL);
  vt_setfg (render, COLOR_FG_NORMAL);
  vwm_frame *frame = this->head;
  while (frame) {
    ifnot (frame->is_visible) goto next_frame;

    vt_goto (render, frame->first_row, 1);

    for (int i = 0; i < frame->num_rows; i++) {
      for (int j = 0; j < frame->num_cols; j++) {
        chr = frame->videomem[i][j];
        clr = frame->colors[i][j];

        ifnot (clr is oldclr) {
          vt_setfg (render, clr);
          oldclr = clr;
        }

        if (chr & 0xFF) {
          vt_attr_check (render, chr & 0xFF, oldattr, &on);
          oldattr = (chr & 0xFF);

          if (oldattr >= 0x80) {
            Ustring.character (chr, buf, &len);
            String.append_with_len (render, buf, len);
          } else
            String.append_byte (render, chr & 0xFF);
        } else {
          oldattr = 0;

          ifnot (on is NORMAL) {
            vt_attr_reset (render);
            on = NORMAL;
          }

          String.append_byte (render, ' ');
        }
      }

      String.append_with(render, "\r\n");
    }

    // clear the last newline, otherwise it scrolls one line more
    String.clear_at (render, -1); // this is visible when there is one frame

    /*  un-needed?
    vt_setscroll (render, frame->scroll_first_row + frame->first_row - 1,
        frame->last_row + frame->first_row - 1);
    vt_goto (render, frame->row_pos + frame->first_row - 1, frame->col_pos);
    */

    next_frame: frame = frame->next;
  }

  self(set.separators, DONOT_DRAW);
  String.append_with_len (render, this->separators_buf->bytes, this->separators_buf->num_bytes);

  frame = this->current;
  vt_goto (render, frame->row_pos + frame->first_row - 1, frame->col_pos);

  vt_write (render->bytes, stdout);
}

static void win_on_resize (vwm_win *this, int draw) {
  int frow = 1;
  vwm_frame *frame = this->head;
  while (frame) {
    ifnot (frame->is_visible) goto next_frame;

    Vframe.on_resize (frame, frame->new_rows, frame->num_cols);
    frame->num_rows = frame->new_rows;
    frame->last_row = frame->num_rows;
    frame->first_row = frow;
    frow += frame->num_rows + 1;
    if (frame->argv and frame->pid isnot -1) {
      struct winsize ws = {.ws_row = frame->num_rows, .ws_col = frame->num_cols};
      ioctl (frame->fd, TIOCSWINSZ, &ws);
      kill (frame->pid, SIGWINCH);
    }

    next_frame: frame = frame->next;
  }

  if (draw)
    self(draw);
}

static int win_min_rows (vwm_win *this) {
  int num_frames = self(get.num_visible_frames);
  return this->num_rows - num_frames -
      this->num_separators - (num_frames * 2);
}

static void win_frame_increase_size (vwm_win *this, vwm_frame *frame, int param, int draw) {
  if (this->num_items is 1)
    return;

  int min_rows = win_min_rows (this);

  ifnot (param) param = 1;
  if (param > min_rows) param = min_rows;

  int num_lines;
  int mod;
  int num_frames = self(get.num_visible_frames);

  if (param is 1 or param is num_frames - 1) {
    num_lines = 1;
    mod = 0;
  } else if (param > num_frames - 1) {
    num_lines = param / (num_frames - 1);
    mod = param % (num_frames - 1);
  } else {
    num_lines = (num_frames - 1) / param;
    mod = (num_frames - 1) % param;
  }

  int orig_param = param;

  vwm_frame *fr = this->head;
  while (fr) {
    fr->new_rows = fr->num_rows;
    fr = fr->next;
  }

  fr = this->head;
  int iter = 1;
  while (fr and param and iter++ < ((num_frames - 1) * 3)) {
    if (frame is fr or frame->is_visible is 0)
      goto next_frame;

    int num = num_lines;
    while (fr->new_rows > 2 and num--) {
      fr->new_rows = fr->new_rows - 1;
      param--;
    }

    if (fr->new_rows is 2)
      goto next_frame;

    if (mod) {
      fr->new_rows--;
      param--;
      mod--;
    }

    next_frame:
      if (fr->next is NULL)
        fr = this->head;
      else
        fr = fr->next;
  }

  frame->new_rows = frame->new_rows + (orig_param - param);

  self(on_resize, draw);
}

static void win_frame_decrease_size (vwm_win *this, vwm_frame *frame, int param, int draw) {
  if (1 is this->num_items or frame->num_rows <= MIN_ROWS)
    return;

  if (frame->num_rows - param <= 2)
    param = frame->num_rows - 2;

  if (0 >= param)
    param = 1;

  int num_lines;
  int mod;
  int num_frames = self(get.num_visible_frames);

  if (param is 1 or param is num_frames - 1) {
    num_lines = 1;
    mod = 0;
  } else if (param > num_frames - 1) {
    num_lines = param / (num_frames - 1);
    mod = param % (num_frames - 1);
  } else {
    num_lines = (num_frames - 1) / param;
    mod = (num_frames - 1) % param;
  }

  vwm_frame *fr = this->head;
  while (fr) {
    fr->new_rows = fr->num_rows;
    fr = fr->next;
  }

  int orig_param = param;

  fr = this->head;
  int iter = 1;
  while (fr and param and iter++ < ((this->num_items - 1) * 3)) {
    if (frame is fr or frame->is_visible is 0)
      goto next_frame;

    fr->new_rows = fr->num_rows + num_lines;

    param -= num_lines;

    if (mod) {
      fr->new_rows++;
      param--;
      mod--;
    }

  next_frame:
    if (fr->next is NULL)
      fr = this->head;
    else
      fr = fr->next;
  }

  frame->new_rows = frame->new_rows - (orig_param - param);

  self(on_resize, draw);
}

static void win_frame_set_size (vwm_win *this, vwm_frame *frame, int param, int draw) {
  if (param is frame->num_rows or 0 >= param)
    return;

  if (param > frame->num_rows)
    win_frame_increase_size (this, frame, param - frame->num_rows, draw);
  else
    win_frame_decrease_size (this, frame, frame->num_rows - param, draw);
}

static vwm_frame *win_frame_change (vwm_win *this, vwm_frame *frame, int dir, int draw) {
  int num_frames = self(get.num_visible_frames);
  if ((NULL is frame->next and NULL is frame->prev) or 1 is num_frames)
    return frame;

  int idx = -1;

  vwm_frame *lframe = frame;

again:
  if (dir is DOWN_POS) {
    if (NULL is lframe->next) {
      idx = 0;
      while (lframe) {
        if (lframe->is_visible) break;
        idx++;
        lframe = lframe->next;
      }
    } else {
      if (lframe->next->is_visible) {
        idx = self(get.frame_idx, lframe->next);
      } else {
        lframe = lframe->next;
        goto again;
      }
    }
  } else {
    if (NULL is lframe->prev) {
      idx = this->num_items - 1;
      while (lframe) {
        if (lframe->is_visible) break;
        idx--;
        lframe = lframe->prev;
      }
    } else {
      if (lframe->prev->is_visible) {
        idx = self(get.frame_idx, lframe->prev);
      } else {
        lframe = lframe->prev;
        goto again;
      }
    }
  }

  this->last_frame = frame;

  self(set.current_at, idx);

  if (OK is self(set.separators, draw))
    if (draw is DONOT_DRAW)
      this->draw_separators = 1;

  return this->current;
}

static void vwm_change_win (vwm_t *this, vwm_win *win, int dir, int draw) {
  if ($my(num_items) is 1)
    return;

  int idx = -1;
  switch (dir) {
    case LAST_POS:
      if ($my(last_win) is win)
        return;

      idx = self(get.win_idx, $my(last_win));
      break;

    case NEXT_POS:
      ifnot (NULL is win->next)
        idx = self(get.win_idx, win->next);
      else
        idx = 0;
      break;

    case PREV_POS:
      ifnot (NULL is win->prev)
        idx = self(get.win_idx, win->prev);
      else
        idx = $my(num_items) - 1;
      break;

    default:
      idx = dir;
      if (idx >= $my(num_items) or idx < 0) return;
      break;
  }

  $my(last_win) = win;

  win = self(set.current_at, idx);

  Term.screen.clear ($my(term));

  ifnot (win->is_initialized) {
    vwm_frame *frame = win->head;
    while (frame) {
      if (frame->argv is NULL)
        Vframe.set.command (frame, $my(default_app)->bytes);

      Vframe.fork (frame);

      frame = frame->next;
    }
  }

  if (draw) {
    if (win->is_initialized)
      Vwin.draw (win);
    else
      Vwin.set.separators (win, DRAW);
  } else
    Vwin.set.separators (win, DONOT_DRAW);

  win->is_initialized = 1;
}

static int vwm_default_edit_file_cb (vwm_t *this, vwm_frame *frame, char *file, void *object) {
  (void) object; (void) frame;

  size_t len = $my(editor)->num_bytes + bytelen (file);
  char command[len + 2];
  snprintf (command, len + 2, "%s %s", $my(editor)->bytes, file);
  int argc = 0;
  char **argv = parse_command (command, &argc);
  int retval = self(spawn, argv);
  argv_release (argv, &argc);
  return retval;
}

static void frame_reopen_log (vwm_frame *this) {
  if (this->logfile is NULL or 0 is this->logfile->num_bytes)
    return;

  if (-1 isnot this->logfd) close (this->logfd);

  this->logfd = open (this->logfile->bytes, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
}

static int frame_edit_log (vwm_frame *frame) {
  if (frame->logfile is NULL)
    return NOTOK;

  vwm_win *win = frame->parent;
  vwm_t *this = win->parent;

  int len;

  for (int i = 0; i < frame->num_rows; i++) {
    char buf[(frame->num_cols * 3) + 2];
    len = vt_video_line_to_str (frame->videomem[i], buf, frame->num_cols);
    write (frame->logfd, buf, len);
  }

  $my(edit_file_cb) (this, frame, frame->logfile->bytes, $my(user_data)[E_OBJECT]);

  vt_video_add_log_lines (frame);
  Vwin.draw (win);
  return OK;
}

static char *vwm_name_gen (const int *name_gen, const char *prefix, size_t prelen) {
  char *ng = (char *) name_gen;
  size_t num = (*name_gen / 26) + prelen;
  char *name = Alloc (num * sizeof (char *) + 1);
  size_t i = 0;
  for (; i < prelen; i++) name[i] = prefix[i];
  for (; i < num; i++) name[i] = 'a' + ((*ng)++ % 26);
  name[num] = '\0';
  return name;
}

static int win_get_num_frames (vwm_win *this) {
  return this->num_items;
}

static int win_get_max_frames (vwm_win *this) {
  return this->max_frames;
}

static int win_get_num_visible_frames (vwm_win *this) {
  return this->num_visible_frames;
}

static vwm_frame *win_get_current_frame (vwm_win *this) {
  return this->current;
}

static int win_get_current_frame_idx (vwm_win *this) {
  return this->cur_idx;
}

static char *win_get_name (vwm_win *this) {
  return this->name;
}

static vwm_frame *win_get_frame_at (vwm_win *this, int idx) {
  return DListGetAt (this, vwm_frame, idx);
}

static int win_get_frame_idx (vwm_win *this, vwm_frame *frame) {
  int idx = DListGetIdx (this, vwm_frame, frame);
  if (idx is EINDEX)
    return NOTOK;

  return idx;
}

static int vwm_append_win (vwm_t *this, vwm_win *win) {
  return DListAppend ($myprop, win);
}

static vwm_win *vwm_new_win (vwm_t *this, char *name, win_opts opts) {
  vwm_win *win = Alloc (sizeof (vwm_win));
  self(append_win, win);

  win->parent = this;
  win->self = this->win;
  win->frame = this->frame;

  if (NULL is name)
    win->name = vwm_name_gen (&$my(name_gen), "win:", 4);
  else
    win->name = strdup (name);

  int num_frames = opts.num_frames;

  win->max_frames = opts.max_frames;
  win->first_row = opts.first_row;
  win->first_col = opts.first_col;

  win->num_rows = opts.num_rows;
  win->num_cols = opts.num_cols;

  if (num_frames > win->max_frames) num_frames = win->max_frames;
  if (num_frames < 0) num_frames = 1;

  if (win->num_rows >= $my(num_rows))
    win->num_rows = $my(num_rows);

  win->num_visible_frames = 0;
  win->num_separators = -1;

  win->separators_buf = String.new ((win->num_rows * win->num_cols) + 32);
  win->render = String.new (4096);
  win->last_row = win->num_rows;

  if (win->first_col <= 0) win->first_col = 1;
  if (win->first_row <= 0) win->first_row = 1;
  if (win->first_row >= win->num_rows - (num_frames - 1) + num_frames)
    win->first_row = win->num_rows - (num_frames - 1) - num_frames;

  win->cur_row = win->first_row;
  win->cur_col = win->first_col;

  win->num_items = 0;
  win->cur_idx = -1;
  win->head = win->current = win->tail = NULL;

  int frame_rows = 0;
  int mod = Vwin.frame_rows (win, num_frames, &frame_rows);

  int
    num_rows = frame_rows + mod,
    first_row = win->first_row;

  for (int i = 0; i < num_frames; i++) {
    frame_opts fr_opts;

    if (i < WIN_OPTS_MAX_FRAMES)
      fr_opts = opts.frame_opts[i];
    else
      fr_opts = FrameOpts ();

    fr_opts.num_rows = num_rows;
    fr_opts.first_row = first_row;

    Vwin.new_frame (win, fr_opts);

    first_row += num_rows + 1;
    num_rows = frame_rows;
  }

  win->last_frame = win->head;

  if (opts.focus or opts.draw) {
    win = self(set.current_at, $my(num_items) - 1);
    Vwin.draw (win);
  }

  return win;
}

static void vwm_release_win (vwm_t *this, vwm_win *win) {
  int idx = self(get.win_idx, win);
  if (idx is NOTOK) return;

  vwm_win *w = self(pop_win_at, idx);

  int len = w->num_items;
  for (int i = 0; i < len; i++)
    Vwin.release_frame_at (w, 0);

  String.release (win->separators_buf);
  String.release (win->render);

  if ($my(last_win) is w and $my(num_items) isnot 0) {
    if ($my(num_items) is 1)
      $my(last_win) = $my(current);
    else {
      vwm_win *lw = $my(head);
      while (lw) {
        if (lw isnot $my(current)) {
          $my(last_win) = lw;
          break;
        }

        lw = lw->next;
      }
    }
  }

  free (w->name);
  free (w);
}

static int vwm_spawn (vwm_t *this, char **argv) {
  int status = NOTOK;
  pid_t pid;

  Term.orig_mode ($my(term));

  if (-1 is (pid = fork ())) goto theend;

  ifnot (pid) {
    char lrows[4], lcols[4];
    snprintf (lrows, 4, "%d", $my(num_rows));
    snprintf (lcols, 4, "%d", $my(num_cols));

    setenv ("TERM", $my(term)->name, 1);
    setenv ("LINES", lrows, 1);
    setenv ("COLUMNS", lcols, 1);

    execvp (argv[0], argv);
    Stderr.print ("execvp failed\n");
    _exit (1);
  }

  if (-1 is waitpid (pid, &status, 0)) {
    status = -1;
    goto theend;
  }

  ifnot (WIFEXITED (status)) {
    status = -1;
    Stderr.print_fmt ("Failed to invoke %s\n", argv[0]);
    goto theend;
  }

  ifnot (status is WEXITSTATUS (status))
    Stderr.print_fmt ("Proc %s terminated with exit status: %d", argv[0], status);

theend:
  Term.raw_mode ($my(term));
  return status;
}

static int frame_create_fd (vwm_frame *frame) {
  if (frame->fd isnot -1) return frame->fd;

  int fd = -1;
  if (-1 is (fd = posix_openpt (O_RDWR|O_NOCTTY|O_CLOEXEC))) goto theerror;
  if (-1 is grantpt (fd)) goto theerror;
  if (-1 is unlockpt (fd)) goto theerror;
  char *name = ptsname (fd); if (NULL is name) goto theerror;
  Cstring.cp (frame->tty_name, MAX_TTYNAME, name, MAX_TTYNAME - 1);

  frame->fd = fd;
  return fd;

theerror:
  if (fd isnot -1)
    close (fd);

  return -1;
}

static pid_t frame_fork (vwm_frame *frame) {
  if (frame->pid isnot -1)
    return frame->pid;

  char pid[8]; snprintf (pid, sizeof (pid), "%d", getpid ());

  vwm_t *this = frame->parent->parent;

  signal (SIGWINCH, SIG_IGN);

  frame->pid = -1;

  int fd = -1;

  if (frame->fd is -1) {
    if (-1 is (fd = posix_openpt (O_RDWR|O_NOCTTY|O_CLOEXEC))) goto theerror;
    if (-1 is grantpt (fd)) goto theerror;
    if (-1 is unlockpt (fd)) goto theerror;
    char *name = ptsname (fd); if (NULL is name) goto theerror;

    Cstring.cp (frame->tty_name, MAX_TTYNAME, name, MAX_TTYNAME - 1);
  } else
    fd = frame->fd;

  frame->fd = fd;

  if (-1 is (frame->pid = fork ())) goto theerror;

  ifnot (frame->pid) {
    int slave_fd;

    setsid ();

    if (-1 is (slave_fd = open (frame->tty_name, O_RDWR|O_CLOEXEC|O_NOCTTY)))
      goto theerror;

    ioctl (slave_fd, TIOCSCTTY, 0);

    dup2 (slave_fd, 0);
    dup2 (slave_fd, 1);
    dup2 (slave_fd, 2);

    fd_set_size (slave_fd, frame->num_rows, frame->num_cols);

    close (slave_fd);
    close (fd);

    int maxfd;
#ifdef OPEN_MAX
    maxfd = OPEN_MAX;
#else
    maxfd = sysconf (_SC_OPEN_MAX);
#endif

    for (int fda = 3; fda < maxfd; fda++)
      if (close (fda) is -1 and errno is EBADF)
        break;

    sigset_t emptyset;
    sigemptyset (&emptyset);
    sigprocmask (SIG_SETMASK, &emptyset, NULL);

    char rows[4]; snprintf (rows, 4, "%d", frame->num_rows);
    char cols[4]; snprintf (cols, 4, "%d", frame->num_cols);
    setenv ("TERM",  $my(term)->name, 1);
    setenv ("LINES", rows, 1);
    setenv ("COLUMNS", cols, 1);
    setenv ("VWM", pid, 1);

    frame->pid = getpid ();

    int retval = frame->at_fork_cb (frame, this, frame->parent);

    if (retval is NOTOK)
      goto theerror;

    ifnot (retval)
      return 0;

    if (retval is 1) {
      execvp (frame->argv[0], frame->argv);

      Stderr.print_fmt ("execvp() failed for command: '%s'\n", frame->argv[0]);
      _exit (1);
    }
  }

  goto theend;

theerror:
  ifnot (-1 is frame->pid) {
    kill (frame->pid, SIGHUP);
    waitpid (frame->pid, NULL, 0);
  }

  frame->pid = -1;
  frame->fd = -1;

  ifnot (-1 is fd) close (fd);

theend:
  signal (SIGWINCH, vwm_sigwinch_handler);
  return frame->pid;
}

static void vwm_sigwinch_handler (int sig) {
  signal (sig, vwm_sigwinch_handler);
  vwm_t *this = __VWM__;
  $my(need_resize) = 1;
}

static void vwm_handle_sigwinch (vwm_t *this) {
  int rows; int cols;
  Term.init_size ($my(term), &rows, &cols);
  self(set.size, rows, cols, 1);

  vwm_win *win = $my(head);
  while (win) {
    win->num_rows = $my(num_rows);
    win->num_cols = $my(num_cols);
    win->last_row = win->num_rows;

    int frame_rows = 0;
    int num_frames = Vwin.get.num_visible_frames (win);
    int mod = Vwin.frame_rows (win, num_frames, &frame_rows);

    int
      num_rows = frame_rows + mod,
      first_row = win->first_row;

    vwm_frame *frame = win->head;
    while (frame) {
      Vframe.on_resize (frame, num_rows, win->num_cols);

      frame->first_row = first_row;
      frame->num_rows = num_rows;
      frame->last_row = frame->num_rows;

      if (frame->argv and frame->pid isnot -1) {
        struct winsize ws = {.ws_row = frame->num_rows, .ws_col = frame->num_cols};
        ioctl (frame->fd, TIOCSWINSZ, &ws);
        kill (frame->pid, SIGWINCH);
      }

      if (frame->is_visible) {
        first_row += num_rows + 1;
        num_rows = frame_rows;
      }

      frame = frame->next;
    }
    win = win->next;
  }

  Video.release ($my(video));
  $my(video) = Video.new (STDOUT_FILENO, $my(num_rows), $my(num_cols), 1, 1);

  win = $my(current);

  Vwin.draw (win);
  $my(need_resize) = 0;
}

static void vwm_exit_signal (int sig) {
  __deinit_vwm__ (&__VWM__);
  exit (sig);
}

static int vwm_main (vwm_t *this) {
  ifnot ($my(num_items)) return OK;

  if (NULL is $my(current)) {
    $my(current) = $my(head);
    $my(cur_idx) = 0;
  }

  if ($my(num_items) > 1) {
    if (NULL isnot $my(current)->prev)
      $my(last_win) = $my(current)->prev;
    else
      $my(last_win) = $my(current)->next;
  }

  setbuf (stdin, NULL);

  signal (SIGHUP,   vwm_exit_signal);
  signal (SIGINT,   vwm_exit_signal);
  signal (SIGQUIT,  vwm_exit_signal);
  signal (SIGTERM,  vwm_exit_signal);
  signal (SIGSEGV,  vwm_exit_signal);
  signal (SIGBUS,   vwm_exit_signal);
  signal (SIGWINCH, vwm_sigwinch_handler);

  fd_set read_mask;
  struct timeval *tv = NULL;

  char
    input_buf[MAX_CHAR_LEN],
    output_buf[BUFSIZE];

  int
    maxfd,
    numready,
    output_len,
    retval = NOTOK;

  vwm_win *win = $my(current);

  Vwin.set.separators (win, DRAW);

  vwm_frame *frame = win->head;

  while (frame) {
    if (frame->argv is NULL)
      Vframe.set.command (frame, $my(default_app)->bytes);

    if (-1 is Vframe.fork (frame)) {
      vwm_frame *tmp = frame->next;
      Vwin.delete_frame (win, frame, DONOT_DRAW);
      frame = tmp;
      continue;
    }

    frame = frame->next;
  }

  win->is_initialized = 1;

  forever {
    win = $my(current);

    if (NULL is win) {
      retval = OK;
      break;
    }

    check_length:

    ifnot (Vwin.get.num_visible_frames (win)) { // at_no_length_cb
      retval = OK;
      if (1 isnot $my(num_items))
        self(change_win, win, PREV_POS, DRAW);

      self(release_win, win);
      continue;
    }

    if ($my(need_resize))
      vwm_handle_sigwinch (this);

    Vwin.set.frame (win, win->current);

    maxfd = 1;

    FD_ZERO (&read_mask);
    FD_SET (STDIN_FILENO, &read_mask);

    frame = win->head;
    int num_frames = 0;
    while (frame) {
      ifnot (frame->is_visible) goto frame_next;

      if (frame->pid isnot -1) {
        if (0 is Vframe.check_pid (frame)) {
          vwm_frame *tmp = frame->next;
          Vwin.delete_frame (win, frame, DRAW);
          frame = tmp;
          continue;
        }
      }

      if (frame->fd isnot -1) {
        FD_SET (frame->fd, &read_mask);
        num_frames++;

        if (maxfd <= frame->fd)
          maxfd = frame->fd + 1;
      }

frame_next:
      frame = frame->next;
    }

    ifnot (num_frames) goto check_length;

    if (0 >= (numready = select (maxfd, &read_mask, NULL, NULL, tv))) {
      switch (errno) {
        case EIO:
        case EINTR:
        default:
          break;
      }

      continue;
    }

    frame = win->current;

    for (int i = 0; i < MAX_CHAR_LEN; i++) input_buf[i] = '\0';

    if (FD_ISSET (STDIN_FILENO, &read_mask)) {
      if (0 < IO.fd.read (STDIN_FILENO, input_buf, 1)) {
        if (VWM_QUIT is self(process_input, win, frame, input_buf)) {
          retval = OK;
          break;
        }
      }
    }

    win = $my(current);

    frame = win->head;
    while (frame) {
      if (frame->fd is -1 or 0 is frame->is_visible)
        goto next_frame;

      if (FD_ISSET (frame->fd, &read_mask)) {
        output_buf[0] = '\0';
        if (0 > (output_len = read (frame->fd, output_buf, BUFSIZE))) {
          switch (errno) {
            case EIO:
            default:
              if (-1 isnot frame->pid) {
                if (0 is Vframe.check_pid (frame)) {
                  Vwin.delete_frame (win, frame, DRAW);
                  goto check_length;
                }
              }

              goto next_frame;
          }
        }

        output_buf[output_len] = '\0';

        Vwin.set.frame (win, frame);

        frame->process_output_cb (frame, output_buf, output_len);
      }

      next_frame:
        frame = frame->next;
    }
  }

  if (retval is 1 or retval is OK or retval is VWM_QUIT) return OK;

  return NOTOK;
}

static void vwm_win_to_video (vwm_t *this, vwm_win *win) {
  if (NULL is $my(video))
    $my(video) = Video.new (STDOUT_FILENO, $my(num_rows), $my(num_cols), 1, 1);

  int video_row = 0;
  vwm_frame *frame = win->head;

  int num_separators = win->num_separators;

  while (frame) {
    ifnot (frame->is_visible) goto next_frame;

    for (int i = 0; i < frame->num_rows; i++) {
      char buf[(frame->num_cols * 3) + 2];
      int len = vt_video_line_to_str (frame->videomem[i], buf, frame->num_cols);
      if (buf[len - 1] is '\n')
        buf[len - 1] = '\0';
      Video.set.row_with ($my(video), video_row++, buf);
    }

    if (num_separators) {
      Video.set.row_with ($my(video), video_row++, win->separators_buf->bytes);
      num_separators--;
    }

    next_frame: frame = frame->next;
  }
}

static int readline_complete_filename  (menu_t *menu) {
  vwm_t *this = menu->user_data[0];
  char dir[PATH_MAX];
  int joinpath;

  if (menu->state & MENU_FINALIZE) goto finalize;

  dir[0] = '\0';
  joinpath = 0;

  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  char *sp = (menu->patlen is 0) ? NULL : menu->pat + menu->patlen - 1;
  char *end = sp;

  if (NULL is sp) {
    char *cwd = Dir.current ();
    Cstring.cp (dir, PATH_MAX, cwd, PATH_MAX - 1);
    free (cwd);
    end = NULL;
    goto getlist;
  }

  if ('~' is menu->pat[0]) {
    if (menu->patlen is 1) {
      end = NULL;
    } else {
      end = menu->pat + 1;
      if (*end is DIR_SEP) {
        if (*(end + 1))
          end++;
        else
          end = NULL;
      }
    }

    char *home = Sys.get.env_value ("HOME");
    Cstring.cp (dir, PATH_MAX, home, bytelen (home));

    joinpath = 1;
    goto getlist;
  }

  if (Dir.is_directory (menu->pat) and bytelen (Path.basename (menu->pat)) > 1) {
    Cstring.cp (dir, PATH_MAX, menu->pat, menu->patlen);
    end = NULL;
    joinpath = 1;
    goto getlist;
  }

  if (sp is menu->pat) {
   if (*sp is DIR_SEP) {
      dir[0] = *sp; dir[1] = '\0';
      joinpath = 1;
      end = NULL;
    } else {
      char *cwd = Dir.current ();
      Cstring.cp (dir, PATH_MAX, cwd, PATH_MAX - 1);
      free (cwd);
      end = sp;
    }

    goto getlist;
  }

  if (*sp is DIR_SEP) {
    Cstring.cp (dir, PATH_MAX, menu->pat, menu->patlen);
    end = NULL;
    joinpath = 1;
    goto getlist;
  }

  while (sp > menu->pat and *(sp - 1) isnot DIR_SEP) sp--;
  if (sp is menu->pat) {
    end = sp;
    char *cwd = Dir.current ();
    Cstring.cp (dir, PATH_MAX, cwd, PATH_MAX - 1);
    free (cwd);
    goto getlist;
  }

  end = sp;
  sp = menu->pat;
  int i = 0;
  while (sp < end) dir[i++] = *sp++;
  dir[i] = '\0';
  joinpath = 1;

getlist:;
  int endlen = (NULL is end) ? 0 : bytelen (end);
  dirlist_t *dlist = Dir.list (dir, 0);

  if (NULL is dlist) {
    menu->state |= MENU_QUIT;
    return NOTOK;
  }

  Vstring_t *vs = Vstring.new ();
  vstring_t *it = dlist->list->head;

  $my(shared_int) = joinpath;
  String.replace_with ($my(shared_str), dir);

  while (it) {
    if (end is NULL or (Cstring.eq_n (it->data->bytes, end, endlen))) {
      Vstring.add.sort_and_uniq (vs, it->data->bytes);
    }

    it = it->next;
  }

  dlist->release (dlist);

  menu->list = vs;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  String.replace_with (menu->header, menu->pat);

  return OK;

finalize:
  menu->state &= ~MENU_FINALIZE;

  if ($my(shared_int)) {
    if ($my(shared_str)->bytes[$my(shared_str)->num_bytes - 1] is DIR_SEP)
      String.clear_at ($my(shared_str), $my(shared_str)->num_bytes - 1);

    int len = menu->patlen + $my(shared_str)->num_bytes + 1;
    char tmp[len + 1];
    snprintf (tmp, len + 1, "%s%c%s", $my(shared_str)->bytes, DIR_SEP, menu->pat);
    String.replace_with ($my(shared_str), tmp);
  } else
    String.replace_with ($my(shared_str), menu->pat);

  if (Dir.is_directory ($my(shared_str)->bytes))
    menu->state |= MENU_REDO;
  else
    menu->state |= MENU_DONE;

  return OK;
}

static int readline_complete_arg (menu_t *menu) {
  vwm_t *this = menu->user_data[0];
  readline_com_t **commands = menu->user_data[2];

  int com = $my(shared_int);
  char *line = $my(shared_str)->bytes;

  if (commands[com]->args is NULL) {
    return NOTOK;
  }

  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  Vstring_t *args = Vstring.new ();

  int i = 0;

  ifnot (menu->patlen) {
    while (commands[com]->args[i])
      Vstring.add.sort_and_uniq (args, commands[com]->args[i++]);
  } else {
    while (commands[com]->args[i]) {
      if (Cstring.eq_n (commands[com]->args[i], menu->pat, menu->patlen))
        if (NULL is Cstring.bytes_in_str (line, commands[com]->args[i]) or
        	Cstring.eq (commands[com]->args[i], "--fname=") or
        	Cstring.eq_n (commands[com]->args[i], "--command=", 10))
          Vstring.add.sort_and_uniq (args, commands[com]->args[i]);
      i++;
    }
  }

  ifnot (args->num_items) {
    menu->state |= MENU_QUIT;
    Vstring.release (args);
    return NOTOK;
  }

  menu->list = args;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  String.replace_with (menu->header, menu->pat);

  return OK;
}

static int readline_complete_command (menu_t *menu) {
  readline_com_t **commands = menu->user_data[2];
  //int num_commands = *(int *) menu->user_data[3];

  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  Vstring_t *coms = Vstring.new ();
  int i = 0;

  ifnot (menu->patlen) {
    while (commands[i])
      Vstring.add.sort_and_uniq (coms, commands[i++]->com);
  } else {
    while (commands[i]) {
      ifnot (Cstring.cmp_n (commands[i]->com, menu->pat, menu->patlen))
        Vstring.add.sort_and_uniq (coms, commands[i]->com);
      i++;
    }
  }

  ifnot (coms->num_items) {
    menu->state |= MENU_QUIT;
    Vstring.release (coms);
    return NOTOK;
  }

  menu->list = coms;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  String.replace_with (menu->header, menu->pat);

  return OK;
}

static int readline_tab_completion (readline_t *rl) {
  vwm_t *this = (vwm_t *) rl->user_data[0];

  ifnot (rl->line->num_items) return READLINE_OK;

  int retval = READLINE_OK;
  vwm_win *win = rl->user_data[1];
  (void) win;

  string_t *currline = NULL;  // otherwise segfaults on certain conditions
redo:;
  currline = Vstring.join (rl->line, "");
  char *sp = currline->bytes + rl->line->cur_idx;
  char *cur = sp;

  while (sp > currline->bytes and *(sp - 1) isnot ' ') sp--;
  int fidx = sp - currline->bytes;
  size_t tok_stacklen = (cur - sp) + 1;
  char tok[tok_stacklen];
  int toklen = 0;
  while (sp < cur) tok[toklen++] = *sp++;
  tok[toklen] = '\0';

  int orig_len = toklen;
  int type = 0;

  if (fidx is 0) {
    type |= READLINE_TOK_COMMAND;
  } else {
    Readline.parse_command (rl);

    $my(shared_int) = rl->com;
    String.replace_with ($my(shared_str), currline->bytes);

    if (rl->com isnot READLINE_NO_COMMAND) {
      if (Cstring.eq_n (tok, "--fname=", 8)) {
        type |= READLINE_TOK_ARG_FILENAME;
        int len = 8 + (tok[8] is '"');
        char tmp[toklen - len + 1];
        int i;
        for (i = len; i < toklen; i++) tmp[i-len] = tok[i];
        tmp[i-len] = '\0';
        Cstring.cp (tok, tok_stacklen, tmp, i-len);
        toklen = i-len;
      } else if (tok[0] is '-') {
        type |= READLINE_TOK_ARG;
        ifnot (NULL is Cstring.byte.in_str (tok, '='))
          type |= READLINE_TOK_ARG_OPTION;
      } else {
        type |= READLINE_TOK_ARG_FILENAME;
      }
    }
  }

  String.release (currline);

  ifnot (type) return retval;

  int (*process_list) (menu_t *) = NULL;

  if (type & READLINE_TOK_ARG_FILENAME)
    process_list = readline_complete_filename;
  else if (type & READLINE_TOK_COMMAND)
    process_list = readline_complete_command;
  else if (type & READLINE_TOK_ARG)
    process_list = readline_complete_arg;

  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = this,
    .user_data_second = win,
    .user_data_third = rl->commands,
    .user_data_fourth = &rl->num_commands,
    .video = $my(video),
    .term = $my(term),
    .first_row = $my(num_rows) - 2,
    .last_row  = $my(num_rows) - 2,
    .first_col = 0,
    .prompt_row = $my(num_rows) - 1,
    .state = MENU_INIT,
    .process_list_cb = process_list,
    .pat = tok,
    .patlen = toklen));

  if ((retval = menu->retval) is NOTOK) goto theend;

  if (type & READLINE_TOK_ARG_FILENAME)
    menu->clear_and_continue_on_backspace = 1;

  menu->return_if_one_item = 1;

  char *item;
  for (;;) {
    item = Menu.create (menu);

    if (NULL is item) goto theend;
    if (menu->state & MENU_QUIT) break;
   // if (type & READLINE_TOK_ARG and Cstring.eq_n ("--command=", item, 10))
   //    Msg.send (this, COLOR_WARNING, "--command argument should be enclosed in a pair of '{}' braces");

    if (type & READLINE_TOK_COMMAND or type & READLINE_TOK_ARG) break;

    menu->patlen = bytelen (item);
    Cstring.cp (menu->pat, MAXLEN_PATTERN, item, menu->patlen);

    if (type & READLINE_TOK_ARG_FILENAME) menu->state |= MENU_FINALIZE;

    if (menu->process_list (menu) is NOTOK) goto theend;

    if (menu->state & (MENU_REDO|MENU_DONE)) break;
    if (menu->state & MENU_QUIT) goto theend;
  }

  if (type & READLINE_TOK_ARG_FILENAME) {
    ifnot (menu->state & MENU_REDO) {
   //   if (rl->com isnot VED_COM_READ_SHELL and rl->com isnot VED_COM_SHELL) {

        String.prepend_with ($my(shared_str), "--fname=\"");
        String.append_byte ($my(shared_str), '"');
      }

    item = $my(shared_str)->bytes;
  }

  ifnot (type & READLINE_TOK_ARG_OPTION) {
    DListSetCurrent (rl->line, fidx);
    int lidx = fidx + orig_len;
    while (fidx++ < lidx) {
      vstring_t *tmp = DListPopCurrent (rl->line, vstring_t);
      String.release (tmp->data);
      free (tmp);
    }
  }

  Readline.insert_with_len (rl, item, bytelen (item));

  if (menu->state & MENU_REDO) {
    Menu.release (menu);
    goto redo;
  }

theend:
  Menu.release (menu);
  return READLINE_OK;
}

static string_t *filter_readline (string_t *);
static string_t *filter_readline (string_t *line) {
  char *sp = Cstring.bytes_in_str (line->bytes, "--fname=");
  if (NULL is sp) return line;
  String.delete_numbytes_at (line, 8, sp - line->bytes);
  return filter_readline (line);
}

static readline_t *vwm_readline_edit (vwm_t *this, vwm_win *win,
  vwm_frame *frame, readline_com_t **commands, int num_commands) {

  vwm_win_to_video (this, win);

  readline_t *rl = Readline.new (this, $my(term), Input.getkey, $my(num_rows) - 1,
      1, $my(num_cols), $my(video));

  rl->commands = commands;
  rl->num_commands = num_commands;
  rl->first_chars[0] = '~';
  rl->first_chars[1] = '`';
  rl->first_chars[2] = '@';
  rl->first_chars[3] = '!';
  rl->first_chars_len = 4;
  rl->trigger_first_char_completion = 1;
  rl->user_data[1] = win;
  rl->user_data[2] = frame;
  rl->history = $my(readline_history);

  rl->tab_completion = readline_tab_completion;
  //rl->last_component_push = readline_last_component_push_cb;
  //rl->last_component = $my(rl_last_component);

  Readline.edit (rl);

  if (rl->c is '\r')
    Readline.parse (rl);

  return rl;
}

static int vwm_command_mode (vwm_t *this, vwm_win *win, vwm_frame *frame) {
  string_t *com = NULL;
  int retval = NOTOK;
  int win_changed = 0;
  readline_t *rl;

  VwmReadlineArgs args = VwmReadline();

  if ($my(readline_cb) isnot NULL) {
    args = $my(readline_cb) (args);
    retval = args.retval;
    com = args.com;
    win_changed = args.win_changed;
    rl = args.rl;

    if (args.state & VWM_READLINE_END) goto theend;
    if (args.state & VWM_READLINE_PARSE) goto parse;
  }

  vwm_init_commands (this);

  rl = vwm_readline_edit (this, win, frame, $my(commands), $my(num_commands));

  if (rl->c isnot '\r') goto theend;

  com = Readline.get.command (rl);

parse:
  if (Cstring.eq (com->bytes, "win_new")) {
    string_t *a_draw = Readline.get.anytype_arg (rl, "draw");
    string_t *a_focus = Readline.get.anytype_arg (rl, "focus");
    string_t *a_num_frames = Readline.get.anytype_arg (rl, "num-frames");
    Vstring_t *a_commands  = Readline.get.anytype_args (rl, "command");

    int draw  = (NULL is a_draw  ? 1 : atoi (a_draw->bytes));
    int focus = (NULL is a_focus ? 1 : atoi (a_focus->bytes));
    int num_frames = (NULL is a_num_frames ? 1 : atoi (a_num_frames->bytes));
    if (num_frames is 0 or num_frames > MAX_FRAMES) num_frames = 1;

    char *command = self(get.default_app);

    win_opts w_opts = WinOpts (
        .num_rows = self(get.lines),
        .num_cols = self(get.columns),
        .num_frames = num_frames,
        .max_frames = MAX_FRAMES,
        .draw = draw,
        .focus = focus);

    if (NULL is a_commands) {
      for (int i = 0; i < num_frames; i++)
        w_opts.frame_opts[i].command = command;
    } else {
      int num = 0;
      vstring_t *it = a_commands->head;
      while (it and num < num_frames) {
        w_opts.frame_opts[num++].command = filter_readline (it->data)->bytes;
        it = it->next;
      }

      while (num < num_frames)
        w_opts.frame_opts[num++].command = command;
    }

    self(new.win, NULL, w_opts);

    Vstring.release (a_commands);

    win_changed = 1;
    retval = OK;
    goto theend;

   } else if (Cstring.eq (com->bytes, "quit")) {
    $my(state) |= VWM_QUIT;
    retval = VWM_QUIT;
    goto theend;

  } else if (Cstring.eq (com->bytes, "frame_delete")) {
    Vwin.delete_frame (win, frame, DRAW);
    retval = OK;
    goto theend;

  } else if (Cstring.eq (com->bytes, "frame_clear")) {
    $my(state) |= (VFRAME_CLEAR_VIDEO_MEM|VFRAME_CLEAR_LOG);
    string_t *clear_log = Readline.get.anytype_arg (rl, "clear-log");
    string_t *clear_mem = Readline.get.anytype_arg (rl, "clear-video-mem");

    ifnot (NULL is clear_log)
      if (atoi (clear_log->bytes) is 0)
        $my(state) &= ~VFRAME_CLEAR_LOG;

    ifnot (NULL is clear_mem)
      if (atoi (clear_log->bytes) is 0)
        $my(state) &= ~VFRAME_CLEAR_VIDEO_MEM;

    Vframe.clear (frame, $my(state));

    retval = OK;
    goto theend;

  } else if (Cstring.eq (com->bytes, "split_and_fork")) {
    vwm_frame *n_frame = Vwin.add_frame (win, 0, NULL, DRAW);
    if (NULL is n_frame) goto theend;

    string_t *command = Readline.get.anytype_arg (rl, "command");
    if (NULL is command) {
      Vframe.set.command (n_frame, self(get.default_app));
    } else {
      command = filter_readline (command);
      Vframe.set.command (n_frame, command->bytes);
    }

    Vframe.fork (n_frame);

    retval = OK;
    goto theend;

  } else if (Cstring.eq (com->bytes, "set")) {
    string_t *log_file = Readline.get.anytype_arg (rl, "log-file");
    if (NULL is log_file) goto theend;

    int set_log = atoi (log_file->bytes);
    if (set_log)
      Vframe.set.log (frame, NULL, 1);
    else
      Vframe.release_log (frame);

    retval = OK;
    goto theend;

  } else if (Cstring.eq (com->bytes, "win_draw")) {
    Vwin.draw (win);
    retval = OK;
    goto theend;
  }

theend:
  if (OK is retval)
    Readline.history.push (rl);
  else
    Readline.release (rl);

  String.release (com);

  ifnot (win_changed)
    Video.draw.rows_from_to ($my(video), $my(num_rows) - 1, $my(num_rows));

  return retval;
}

static int vwm_process_input (vwm_t *this, vwm_win *win, vwm_frame *frame, char *input_buf) {
  if (input_buf[0] isnot $my(mode_key)) {
    if (-1 isnot frame->fd)
      IO.fd.write (frame->fd, input_buf, 1);
    return OK;
  }

  int param = 0;

  utf8 c;

getc_again:
  c = self(getkey, STDIN_FILENO);

  if (NOTOK is c) return OK;

  if (c is $my(mode_key)) {
    input_buf[0] = $my(mode_key); input_buf[1] = '\0';
    IO.fd.write (frame->fd, input_buf, 1);
    return OK;
  }

  for (int i = 0; i < $my(num_process_input_cbs); i++) {
    int retval = $my(process_input_cbs)[i] (this, win, frame, c);
    if (retval isnot VWM_NO_COMMAND) {
      if (retval is VWM_PROCESS_INPUT_RETURN)
        return OK;
      else if (retval is VWM_QUIT)
        return VWM_QUIT;

      break;
    }
  }

  switch (c) {
    case ESCAPE_KEY:
      break;

    case ':': {
        int retval = vwm_command_mode (this, win, frame);
        if (retval is VWM_QUIT or ($my(state) & VWM_QUIT))
          return VWM_QUIT;
      }
      break;

    case 'q':
      return VWM_QUIT;

    case '!':
    case 'c':
      if (frame->pid isnot -1)
        break;

      if (c is '!')
        Vframe.set.command (frame, $my(shell)->bytes);
      else
        if (NULL is frame->argv)
          Vframe.set.command (frame, $my(default_app)->bytes);

      Vframe.fork (frame);
      break;

    case 'n': {
        ifnot (param)
          param = 1;
        else
          if (param > MAX_FRAMES)
            param = MAX_FRAMES;

        win_opts w_opts = WinOpts (
            .num_rows = $my(num_rows),
            .num_cols = $my(num_cols),
            .num_frames = param,
            .draw = 1,
            .focus = 1);

        for (int i = 0; i < param; i++)
          w_opts.frame_opts[i].command = $my(default_app)->bytes;

        win = self(new.win, NULL, w_opts);
        }
      break;

    case 'K':
      if (0 is Vframe.kill_proc (frame))
        Vwin.delete_frame (win, frame, DRAW);
      break;

    case 'd':
      Vwin.delete_frame (win, frame, DRAW);
      break;

    case CTRL('l'):
      Vwin.draw (win);
      break;

    case 's': {
          vwm_frame *fr = Vwin.add_frame (win, 0, NULL, DRAW);
          ifnot (NULL is fr) {
            Vframe.set.command (fr, $my(default_app)->bytes);
            Vframe.fork (fr);
          }
        }
      break;

    case 'S': {
        utf8 w = self(getkey, STDIN_FILENO);
        char *command = $my(default_app)->bytes;

        switch (w) {
          case '!':
            command = $my(shell)->bytes;
            break;

          case 'c':
            command = $my(default_app)->bytes;
            break;

          case 'e':
            command = $my(editor)->bytes;
            break;

          default:
            return OK;
        }

        vwm_frame *fr = Vwin.add_frame (win, 0, NULL, DRAW);
        ifnot (NULL is fr) {
          Vframe.set.command (fr, command);
          Vframe.fork (fr);
        }
      }

      break;

    case PAGE_UP_KEY:
    case 'E':
      Vframe.edit_log (frame);
      break;

    case 'j':
    case 'k':
    case 'w':
    case ARROW_DOWN_KEY:
    case ARROW_UP_KEY:
      Vwin.frame.change (win, frame, (
        c is 'w' or c is 'j' or c is ARROW_DOWN_KEY) ? DOWN_POS : UP_POS, DONOT_DRAW);
      break;

    case 'h':
    case 'l':
    case '`':
    case ARROW_LEFT_KEY:
    case ARROW_RIGHT_KEY:
      self(change_win, win,
          (c is ARROW_RIGHT_KEY or c is 'l') ? NEXT_POS :
          (c is '`') ? LAST_POS : PREV_POS, DRAW);
      break;

    case '+':
      Vwin.frame.increase_size (win, frame, param, DRAW);
      break;

    case '-':
      Vwin.frame.decrease_size (win, frame, param, DRAW);
      break;

    case '=':
      Vwin.frame.set_size (win, frame, param, DRAW);
      break;

    case FN_KEY(1) ... FN_KEY(12): {
        int idx = c - FN_KEY(1);
        if (NULL is self(get.win_at, idx)) break;
        self(change_win, win, idx, DRAW);
      }
      break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      param *= 10;
      param += (c - '0');
      goto getc_again;
  }

  return OK;
}

mutable public void __alloc_error_handler__ (int err, size_t size,
                     const char *file, const char *func, int line);
mutable public void __alloc_error_handler__ (int err, size_t size,
                     const char *file, const char *func, int line) {
  Stderr.print ("MEMORY_ALLOCATION_ERROR\n");
  Stderr.print_fmt ("File: %s\nFunction: %s\nLine: %d\n", file, func, line);
  Stderr.print_fmt ("Size: %zd\n", size);
  Stderr.print_fmt ("%s\n", Error.errno_string (err));

  __deinit_vwm__ (&__VWM__);

  exit (1);
}

public vwm_t *__init_vwm__ (void) {
  __INIT__ (io);
  __INIT__ (dir);
  __INIT__ (sys);
  __INIT__ (vui);
  __INIT__ (term);
  __INIT__ (path);
  __INIT__ (file);
  __INIT__ (error);
  __INIT__ (video);
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (vstring);
  __INIT__ (ustring);
  __INIT__ (readline);

  Sys.init_environment (SysEnvOpts(.termname = "vt100"));

  AllocErrorHandler = __alloc_error_handler__;

  vwm_t *this = Alloc (sizeof (vwm_t));
  vwm_prop *prop = Alloc (sizeof (vwm_prop));

  *this =  (vwm_t) {
    .self = (vwm_self) {
      .main = vwm_main,
      .spawn = vwm_spawn,
      .getkey = vwm_getkey,
      .pop_win_at = vwm_pop_win_at,
      .change_win = vwm_change_win,
      .append_win = vwm_append_win,
      .release_win = vwm_release_win,
      .release_info = vwm_release_info,
      .process_input = vwm_process_input,
      .get = (vwm_get_self) {
        .term = vwm_get_term,
        .info = vwm_get_info,
        .shell = vwm_get_shell,
        .state = vwm_get_state,
        .lines = vwm_get_lines,
        .editor = vwm_get_editor,
        .tmpdir = vwm_get_tmpdir,
        .win_at = vwm_get_win_at,
        .win_idx = vwm_get_win_idx,
        .columns = vwm_get_columns,
        .num_wins = vwm_get_num_wins,
        .mode_key = vwm_get_mode_key,
        .commands = vwm_get_commands,
        .current_win = vwm_get_current_win,
        .default_app = vwm_get_default_app,
        .user_data_at = vwm_get_user_data_at,
        .current_frame = vwm_get_current_frame,
        .current_win_idx = vwm_get_current_win_idx
      },
      .set = (vwm_set_self) {
        .size = vwm_set_size,
        .term = vwm_set_term,
        .state = vwm_set_state,
        .shell =  vwm_set_shell,
        .editor = vwm_set_editor,
        .tmpdir = vwm_set_tmpdir,
        .object = vwm_set_object,
        .datadir = vwm_set_datadir,
        .mode_key = vwm_set_mode_key,
        .at_exit_cb = vwm_set_at_exit_cb,
        .current_at = vwm_set_current_at,
        .default_app = vwm_set_default_app,
        .readline_cb = vwm_set_readline_cb,
        .history_file = vwm_set_history_file,
        .edit_file_cb = vwm_set_edit_file_cb,
        .process_input_cb = vwm_set_process_input_cb,
        .debug = (vwm_set_debug_self) {
          .sequences = vwm_set_debug_sequences,
          .unimplemented = vwm_set_debug_unimplemented
        },
      },
      .unset = (vwm_unset_self) {
        .tmpdir = vwm_unset_tmpdir,
        .datadir = vwm_unset_datadir,
        .debug = (vwm_unset_debug_self) {
          .sequences = vwm_unset_debug_sequences,
          .unimplemented = vwm_unset_debug_unimplemented
        }
      },
      .new = (vwm_new_self) {
        .win = vwm_new_win,
        .term = vwm_new_term
      },
      .readline = (vwm_readline_self) {
        .edit = vwm_readline_edit,
        .append_command = vwm_readline_append_command,
        .append_command_arg = vwm_readline_append_command_arg
      },
      .history = (vwm_history_self) {
        .read = vwm_history_read,
        .write = vwm_history_write
      }
    },
    .win = (vwm_win_self) {
      .draw = win_draw,
      .on_resize = win_on_resize,
      .new_frame = win_new_frame,
      .add_frame = win_add_frame,
      .init_frame = win_init_frame,
      .frame_rows = win_frame_rows,
      .append_frame = win_append_frame,
      .delete_frame = win_delete_frame,
      .pop_frame_at = win_pop_frame_at,
      .release_info = win_release_info,
      .insert_frame_at = win_insert_frame_at,
      .release_frame_at = win_release_frame_at,
      .set = (vwm_win_set_self) {
        .frame = win_set_frame,
        .current_at = win_set_current_at,
        .separators = win_set_separators,
        .frame_as_current = win_set_frame_as_current
      },
      .get = (vwm_win_get_self) {
        .info = win_get_info,
        .name = win_get_name,
        .frame_at = win_get_frame_at,
        .frame_idx = win_get_frame_idx,
        .num_frames = win_get_num_frames,
        .max_frames = win_get_max_frames,
        .current_frame = win_get_current_frame,
        .current_frame_idx = win_get_current_frame_idx,
        .num_visible_frames = win_get_num_visible_frames,
      },
      .frame = (vwm_win_frame_self) {
        .change = win_frame_change,
        .set_size = win_frame_set_size,
        .increase_size = win_frame_increase_size,
        .decrease_size = win_frame_decrease_size
      }
    },
    .frame = (vwm_frame_self) {
      .fork = frame_fork,
      .clear = frame_clear,
      .reset = frame_reset,
      .edit_log = frame_edit_log,
      .check_pid = frame_check_pid,
      .create_fd = frame_create_fd,
      .on_resize = frame_on_resize,
      .kill_proc = frame_kill_proc,
      .reopen_log = frame_reopen_log,
      .release_log = frame_release_log,
      .release_argv = frame_release_argv,
      .release_info = frame_release_info,
      .process_output = frame_process_output,
      .get = (vwm_frame_get_self) {
        .fd = frame_get_fd,
        .pid = frame_get_pid,
        .info = frame_get_info,
        .argc = frame_get_argc,
        .argv = frame_get_argv,
        .root = frame_get_root,
        .logfd = frame_get_logfd,
        .parent = frame_get_parent,
        .logfile = frame_get_logfile,
        .num_rows = frame_get_num_rows,
        .remove_log = frame_get_remove_log,
        .visibility = frame_get_visibility
      },
      .set = (vwm_frame_set_self) {
        .fd = frame_set_fd,
        .log = frame_set_log,
        .argv = frame_set_argv,
        .command = frame_set_command,
        .visibility = frame_set_visibility,
        .at_fork_cb = frame_set_at_fork_cb,
        .unimplemented_cb = frame_set_unimplemented_cb,
        .process_output_cb = frame_set_process_output_cb
      }
    },
    .prop = prop
  };

  $my(tmpdir) = NULL;

  $my(editor) = String.new_with (EDITOR);
  $my(shell) = String.new_with (SHELL);
  $my(default_app) = String.new_with (DEFAULT_APP);
  $my(shared_str) = String.new (16);

  $my(mode_key) = MODE_KEY;

  $my(num_items) = 0;
  $my(shared_int) = 0;
  $my(cur_idx) = -1;
  $my(head) = $my(tail) = $my(current) = NULL;
  $my(name_gen) = ('z' - 'a') + 1;
  $my(num_at_exit_cbs) = 0;
  $my(process_input_cbs) = 0;
  $my(user_data)[E_OBJECT] = NULL;

  self(new.term);

  self(set.edit_file_cb, vwm_default_edit_file_cb);
  self(set.tmpdir, NULL, 0);
  self(set.datadir, NULL, 0);
  self(set.history_file, NULL);

  $my(num_commands) = 0;
  $my(readline_cb) = NULL;
  $my(readline_history) = Alloc (sizeof (readline_hist_t));
  $my(readline_history)->history_idx = 0;
  self(history.read);

  $my(sequences_fp) = NULL;
  $my(sequences_fname) = NULL;
  $my(unimplemented_fp) = NULL;
  $my(unimplemented_fname) = NULL;

#ifdef DEBUG
  self(set.debug.sequences, NULL);
  self(set.debug.unimplemented, NULL);
#endif

  __VWM__ = this;
  return this;
}

public void __deinit_vwm__ (vwm_t **thisp) {
  if (NULL == *thisp) return;

  vwm_t *this = *thisp;

  Term.orig_mode ($my(term));
  Term.release (&$my(term));

  vwm_win *win = $my(head);
  while (win) {
    vwm_win *tmp = win->next;
    self(release_win, win);
    win = tmp;
  }

  self(history.write);

  vwm_release_readline_history (this);
  vwm_deinit_commands (this);
  String.release ($my(shared_str));

  __deinit_vui__ (&vuiType);

  ifnot (NULL is $my(video))
    Video.release ($my(video));

  for (int i = 0; i < $my(num_at_exit_cbs); i++)
    $my(at_exit_cbs)[i] (this);

  if ($my(num_at_exit_cbs))
    free ($my(at_exit_cbs));

  if ($my(num_process_input_cbs))
    free ($my(process_input_cbs));

  self(unset.debug.sequences);
  self(unset.debug.unimplemented);
  self(unset.tmpdir);
  self(unset.datadir);

  String.release ($my(history_file));
  String.release ($my(editor));
  String.release ($my(shell));
  String.release ($my(default_app));

  free (this->prop);
  free (this);

  __deinit_sys__ ();

  *thisp = NULL;
}
