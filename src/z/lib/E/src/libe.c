#ifndef __MACH__
#define REQUIRE_STD_DEFAULT_SOURCE
#else
#define _DARWIN_C_SOURCE
// https://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
#include <sys/time.h>
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif
#endif /* __MACH__ */

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_SYS_STAT
#define REQUIRE_SYS_IOCTL
#define REQUIRE_SYS_SELECT
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_PARAM
#define REQUIRE_PWD
#define REQUIRE_GRP
#define REQUIRE_FCNTL
#define REQUIRE_TERMIOS
#define REQUIRE_TIME
#define REQUIRE_DIRENT

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_USTRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE  DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_PATH_TYPE     DECLARE
#define REQUIRE_DIR_TYPE      DECLARE
#define REQUIRE_LA_TYPE       DECLARE
#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_RE_TYPE       DECLARE
#define REQUIRE_SYS_TYPE      DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define REQUIRE_IMAP_TYPE     DECLARE
#define REQUIRE_SMAP_TYPE     DECLARE
#define REQUIRE_PROC_TYPE     DECLARE
#define REQUIRE_SPELL_TYPE    DECLARE
#define REQUIRE_VIDEO_TYPE    DECLARE
#define REQUIRE_VUI_TYPE      DECLARE
#define REQUIRE_READLINE_TYPE DECLARE
#define REQUIRE_ERROR_TYPE    DECLARE
#define REQUIRE_TERM_MACROS
#define REQUIRE_KEYS_MACROS
#define REQUIRE_LIST_MACROS

#include <z/cenv.h>

#include "e.h"
#include "__e.h"

static la_T *__LAPTR__ = NULL;

/* this replaced Cstring.tok() below */
static Vstring_t *cstring_chop (const char *buf, char tok, Vstring_t *tokstr,
                                     StrChop_cb cb, void *obj) {
  Vstring_t *ts = tokstr;
  int ts_isnull = (NULL is ts);
  if (ts_isnull) ts = Vstring.new ();

  char *sp = (char *) buf;
  char *p = sp;

  int end = 0;
  for (;;) {
    if (end) break;
    ifnot (*sp) {
      end = 1;
      goto tokenize;
    }

    if (*sp is tok) {
tokenize:;
      size_t len = sp - p;
      /* ifnot (len) {
         sp++; p = sp;
         continue;
      } when commented, this broke once the code */

      char s[len + 1];
      Cstring.cp (s, len + 1, p, len);

      ifnot (NULL is cb) {
        int retval;
        if (STRCHOP_NOTOK is (retval = cb (ts, s, obj))) {
          if (ts_isnull) Vstring.release (ts); /* this might bring issues (not */
          return NULL;                   /* with current code though) */
        }

        if (retval is STRCHOP_RETURN)
          return ts;
      }
      else
        Vstring.current.append_with (ts, s);

      sp++;
      p = sp;
      continue;
    }

    sp++;
  }

  return ts;
}

/* unused (based on strtok(), but i do not want functions with static state) */
/* static Vstring_t  *Cstring.tok (char *buf, char *tok, Vstring_t *tokstr,
 *     void (*cb) (Vstring_t *, char *, void *), void *obj) {
 *   Vstring_t *ts = tokstr;
 *   if (NULL is ts) ts = Vstring.new ();
 *
 *   char *src = Cstring.dup (buf, bytelen (buf));
 *   char *sp = strtok (src, tok);
 *   while (sp) {
 *     ifnot (NULL is cb)
 *       cb (ts, sp, obj);
 *     else
 *       Vstring.current.append_with (ts, sp);
 *     sp = strtok (NULL, tok);
 *   }
 *
 *   free (src);
 *   return ts;
 * }
 */

static void readline_last_component_push_cb (readline_t *rl) {
  ed_t *this = (ed_t *) rl->user_data[READLINE_ED_USER_DATA_IDX];
  Vstring.current.prepend_with ($my(rl_last_component), rl->tail->argval->bytes);

  if ($my(rl_last_component)->num_items > READLINE_LAST_COMPONENT_NUM_ENTRIES) {
    vstring_t *item = DListPopTail ($my(rl_last_component), vstring_t);
    String.release (item->data);
    free (item);
  }
}

static void ed_menu_release (ed_t *ed, menu_t *this) {
  (void) ed;
  Menu.release (this);
}

static menu_t *ed_menu_new (ed_t *this, buf_t *buf, MenuProcessList_cb cb) {
  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = this,
    .user_data_second = buf,
    .video = $my(video),
    .term = $my(term),
    .first_row = $my(video)->row_pos,
    .last_row = $my(prompt_row) - 2,
    .first_col = $my(video)->col_pos,
    .prompt_row = $my(prompt_row),
    .process_list_cb = cb,
    .pat = NULL,
    .patlen = 0));

  return menu;
}

static string_t *buf_input_box (buf_t *this, int row, int col,
                            int abort_on_escape, char *buf) {
  string_t *str = NULL;
  readline_t *rl = Readline.new ($my(root), $my(term_ptr), Input.getkey, row,
      col, $my(dim)->num_cols - col + 1, $my(video));

  rl->opts &= ~READLINE_OPT_HAS_HISTORY_COMPLETION;
  rl->opts &= ~READLINE_OPT_HAS_TAB_COMPLETION;
  rl->prompt_char = 0;

  rl->on_write = readline_on_write;
  rl->expr_reg = readline_expr_reg;

  ifnot (NULL is buf)
    Readline.insert_with_len (rl, buf, bytelen (buf));

  utf8 c;
  for (;;) {
    c = Readline.edit (rl)->c;
    switch (c) {
      case ESCAPE_KEY:
        if (abort_on_escape) {
          str = String.new (1);
          goto theend;
        }
        //continue;
        // fall through
      case '\r': str = Readline.get.line (rl); goto theend;
    }
  }

theend:
  String.clear_at (str, -1);
  Readline.release (rl);
  return str;
}

static dim_t *dim_set (dim_t *dim, int f_row, int l_row,
                                    int f_col, int l_col) {
  if (NULL is dim)
    dim = Alloc (sizeof (dim_t));

  dim->first_row = f_row;
  dim->last_row = l_row;
  dim->num_rows = l_row - f_row + 1;
  dim->first_col = f_col;
  dim->last_col = l_col;
  dim->num_cols = l_col - f_col + 1;
  return dim;
}

static dim_t **ed_dims_init (ed_t *this, int num_frames) {
  (void) this;
  return Alloc (sizeof (dim_t *) * num_frames);
}

static dim_t *ed_dim_new (ed_t *this, int f_row, int l_row, int f_col, int l_col) {
  (void) this;
  dim_t *dim = Alloc (sizeof (dim_t));
  return dim_set (dim, f_row, l_row, f_col, l_col);
}

static dim_t **ed_dim_calc (ed_t *this, int num_rows, int num_frames,
                                     int min_rows, int has_dividers) {
  int reserved = $my(has_topline) + $my(has_msgline) + $my(has_promptline);
  int dividers = has_dividers ? num_frames - 1 : 0;
  int rows = (num_frames * min_rows) + dividers + reserved;

  if (num_rows < rows) {
    Term.reset ($my(term));
    Stderr.print ("Available LINES are less than the required to be functional\n");
    exit (1);
  }

  rows = (num_rows - dividers - reserved) / num_frames;
  int mod = (num_rows - dividers - reserved) % num_frames;

  dim_t **dims = self(dims_init, num_frames);

  for (int i = 0; i < num_frames; i++) {
    dims[i] = Alloc (sizeof (dim_t));
    dims[i]->num_rows = rows + (i is 0 ? mod : 0);
    dims[i]->first_row = i is 0
      ? 1 + $my(has_topline)
      : dims[i-1]->last_row + has_dividers + 1;
    dims[i]->last_row = dims[i]->first_row + dims[i]->num_rows - 1;
    dims[i]->first_col = $my(dim)->first_col;
    dims[i]->last_col  = $my(dim)->last_col;
    dims[i]->num_cols  = $my(dim)->num_cols;
  }

  return dims;
}

/* bad code, this is the case where the only thing you can do is to
 * pray to got the things right, very fragile code */
#define state_cp(v__, a__)                                 \
  (v__)->video_first_row = (a__)->video_first_row;         \
  (v__)->video_first_row_idx = (a__)->video_first_row_idx; \
  (v__)->row_pos = (a__)->row_pos;                         \
  (v__)->col_pos = (a__)->col_pos;                         \
  (v__)->cur_col_idx = (a__)->cur_col_idx;                 \
  (v__)->first_col_idx = (a__)->first_col_idx;             \
  (v__)->cur_idx = (a__)->cur_idx;                         \
  (v__)->idx = (a__)->idx

#define state_set(v__)                                   \
  (v__)->video_first_row = $my(video_first_row);         \
  (v__)->video_first_row_idx = $my(video_first_row_idx); \
  (v__)->row_pos = $my(cur_video_row);                   \
  (v__)->col_pos = $my(cur_video_col);                   \
  (v__)->cur_col_idx = $mycur(cur_col_idx);              \
  (v__)->first_col_idx = $mycur(first_col_idx);          \
  (v__)->cur_idx = this->cur_idx

#define state_restore(s__)                               \
  $mycur(first_col_idx) = (s__)->first_col_idx;          \
  $mycur(cur_col_idx) = (s__)->cur_col_idx;              \
  $my(video)->row_pos = (s__)->row_pos;                  \
  $my(video)->col_pos = (s__)->col_pos;                  \
  $my(video_first_row_idx) = (s__)->video_first_row_idx; \
  $my(video_first_row) = (s__)->video_first_row;         \
  $my(cur_video_row) = (s__)->row_pos;                   \
  $my(cur_video_col) = (s__)->col_pos

static int buf_mark_restore (buf_t *this, mark_t *mark) {
  if (mark->video_first_row is NULL) return NOTHING_TODO;
  if (mark->cur_idx is this->cur_idx) return NOTHING_TODO;
  if (mark->cur_idx >= this->num_items) return NOTHING_TODO;

  self(current.set, mark->cur_idx);
  state_restore (mark);

  if ($mycur(first_col_idx) or $mycur(cur_col_idx) >= (int) $mycur(data)->num_bytes) {
    $mycur(first_col_idx) = $mycur(cur_col_idx) = 0;
    $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;
  }

  return DONE;
}

static ustring_t *buf_get_line_nth (Ustring_t *line, int idx) {
  line->current = line->head;
  int i = 0;
  while (line->current) {
    if (i is idx or (i + (line->current->len - 1) is idx))
      return line->current;

    i += line->current->len;
    line->current = line->current->next;
  }

  if (i is idx or (i + (line->tail->len - 1) is idx))
    return line->tail;
  return NULL;
}

static void buf_adjust_view (buf_t *this) {
  $my(video_first_row) = this->current;
  $my(video_first_row_idx) = this->cur_idx;
  int num = (ONE_PAGE / 2);

  while ($my(video_first_row_idx) and num) {
    $my(video_first_row_idx)--;
    num--;
    $my(video_first_row) = $my(video_first_row)->prev;
  }

  $mycur(first_col_idx) = $mycur(cur_col_idx) = 0;

  $my(video)->row_pos = $my(cur_video_row) =
      $my(dim)->first_row + ((ONE_PAGE / 2) - num);
  $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;
}

static void buf_adjust_marks (buf_t *this, int type, int fidx, int lidx) {
  for (int m = 0; m < NUM_MARKS; m++) {
    mark_t *mark = &$my(marks)[m];
    if (mark->video_first_row is NULL) continue;
    if (mark->cur_idx < fidx) continue;
    if (fidx <= mark->cur_idx and mark->cur_idx <= lidx) {
      mark->video_first_row = NULL;
      continue;
    }

    if (type is DELETE_LINE)
      if (fidx is 0 and mark->video_first_row_idx is 0)
        mark->video_first_row = this->head;

    int lcount = lidx - fidx + (type is DELETE_LINE);
    int idx = this->cur_idx;

    mark->video_first_row_idx = idx;
    mark->video_first_row = this->current;

    if (type is DELETE_LINE)
      mark->cur_idx -= lcount;
    else
      mark->cur_idx += lcount;

    while (idx++ < mark->cur_idx) {
      mark->video_first_row_idx++;
      mark->video_first_row = mark->video_first_row->next;
    }

    mark->row_pos = $my(dim)->first_row;

    idx = 5 < $my(dim)->num_rows ? 5 : $my(dim)->num_rows;
    while (mark->video_first_row_idx > 0 and idx--) {
      if (NULL is mark->video_first_row or NULL is mark->video_first_row->prev)
        break;
      mark->video_first_row = mark->video_first_row->prev;
      mark->video_first_row_idx--;
      mark->row_pos++;
    }
  }
}

static int buf_adjust_col (buf_t *this, int nth, int isatend) {
  if (this->current is NULL) return 1;

  Ustring.encode ($my(line), $mycur(data)->bytes, $mycur(data)->num_bytes,
      CLEAR, $my(ftype)->tabwidth, $mycur(cur_col_idx));

  int hasno_len = ($mycur(data)->num_bytes is 0 or NULL is $my(line));
  if (hasno_len) isatend = 0;
  if (0 is isatend and (hasno_len or nth <= 1 or
      (int) $mycur(data)->num_bytes is $my(line)->head->len)) {
    $mycur(cur_col_idx) = $mycur(first_col_idx) = 0;
    $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;

    if ($mycur(data)->num_bytes)
      $my(video)->col_pos = $my(cur_video_col) = $my(cur_video_col) +
          (0 is IS_MODE (INSERT_MODE) ? $my(line)->head->width - 1 : 0);

    return $my(video)->col_pos;
  }

  ustring_t *it;
  int clen = 0;
  /* some heuristics, this can never be perfect, unless a specific keybinding|set */
  if (isatend and
      ($my(prev_nth_ptr_pos) > $my(line)->num_items or
      ($my(line)->num_items < ($my(prev_nth_ptr_pos) + 20) and
       $my(prev_num_items) > 1))) {
    clen = $my(line)->tail->len;
    $mycur(cur_col_idx) = $mycur(data)->num_bytes - clen;
  } else {
    int idx = 0;
    int num = 1;
    it = $my(line)->head;
    while (it and (idx < (int) $mycur(data)->num_bytes and num < nth)) {
      clen = it->len;
      idx += clen;
      it = it->next;
      num++;
    }

    if (idx >= (int) $mycur(data)->num_bytes)
      $mycur(cur_col_idx) = $mycur(data)->num_bytes - clen;
    else
      $mycur(cur_col_idx) = idx - (num < nth ? clen : 0);
  }

  it = buf_get_line_nth ($my(line), $mycur(cur_col_idx));
  int cur_width = it->width;
  int col_pos = cur_width; /* cur_col_idx char */;
  int idx = $mycur(cur_col_idx);

  it = it->prev;
  while (it and idx and (col_pos < $my(dim)->num_cols)) {
    idx -= it->len;
    col_pos += it->width;
    it = it->prev;
  }

  $mycur(first_col_idx) = idx;
  $my(video)->col_pos = $my(cur_video_col) = col_pos -
       (IS_MODE (INSERT_MODE) ? (cur_width - 1) : 0);

  return $my(video)->col_pos;
}

#define undo_set(act, type__)                            \
  (act)->type = (type__);                                \
  state_set(act)

#define undo_restore(act)                                \
  state_restore(act)

static void buf_undo_init (buf_t *this) {
  if (NULL is $my(undo)) $my(undo) = Alloc (sizeof (undo_t));
  if (NULL is $my(redo)) $my(redo) = Alloc (sizeof (undo_t));
}

static Action_t *buf_undo_pop (buf_t *this) {
  return DListPopCurrent ($my(undo), Action_t);
}

static Action_t *buf_redo_pop (buf_t *this) {
  if ($my(redo)->head is NULL) return NULL;
  return DListPopCurrent ($my(redo), Action_t);
}

static void __buf_redo_clear__ (buf_t *this) {
  if ($my(redo)->head is NULL) return;

  Action_t *action = self(redo.pop);
  while (action) {
    self(Action.release, action);
    action = self(redo.pop);
  }

  $my(redo)->num_items = 0; $my(redo)->cur_idx = 0;
  $my(redo)->head = $my(redo)->tail = $my(redo)->current = NULL;
}

static void __buf_undo_clear__ (buf_t *this) {
  if ($my(undo)->head is NULL) return;

  Action_t *action = self(undo.pop);
  while (action isnot NULL) {
    self(Action.release, action);
    action = self(undo.pop);
  }
  $my(undo)->num_items = 0; $my(undo)->cur_idx = 0;
  $my(undo)->head = $my(undo)->tail = $my(undo)->current = NULL;
}

static void buf_undo_clear (buf_t *this) {
  __buf_undo_clear__ (this);
  __buf_redo_clear__ (this);
}

static void buf_undo_push (buf_t *this, Action_t *action) {
  if ($my(undo)->num_items > $myroots(max_num_undo_entries)) {
    Action_t *tmp = DListPopTail ($my(undo), Action_t);
    self(Action.release, tmp);
  }

  ifnot ($my(undo)->state & VUNDO_RESET)
    __buf_redo_clear__ (this);
  else
    $my(undo)->state &= ~VUNDO_RESET;

  DListPrependCurrent ($my(undo), action);
}

static void buf_redo_push (buf_t *this, Action_t *action) {
  if ($my(redo)->num_items > $myroots(max_num_undo_entries)) {
    Action_t *tmp = DListPopTail ($my(redo), Action_t);
    self(Action.release, tmp);
  }

  DListPrependCurrent ($my(redo), action);
}

static int buf_undo_insert (buf_t *this, Action_t *redoact, action_t *act) {
  ifnot (this->num_items) return DONE;

  action_t *ract = self(action.new);
  self(current.set, act->idx);
  self(adjust.view);
  ract->type = DELETE_LINE;
  undo_set (ract, DELETE_LINE);
  ract->idx = this->cur_idx;
  ract->bytes = Cstring.dup ($mycur(data)->bytes, $mycur(data)->num_bytes);
  ListStackPush (redoact, ract);

  self(current.delete);

  if (this->num_items) {
    self(current.set, act->cur_idx);
    self(adjust.marks, DELETE_LINE, act->idx, act->idx);
    undo_restore (act);
  }

  return DONE;
}

static int buf_undo_delete_line (buf_t *this, Action_t *redoact, action_t *act) {
  action_t *ract = self(action.new);
  row_t *row = self(row.new_with, act->bytes);

  if (this->num_items) {
    if (act->idx >= this->num_items) {
      self(current.set, this->num_items - 1);
      self(adjust.view);
      undo_set (ract, INSERT_LINE);
      self(current.append, row);
      ract->idx = this->cur_idx;
    } else {
      self(current.set, act->idx);
      self(adjust.view);
      undo_set (ract, INSERT_LINE);
      self(current.prepend, row);
      ract->idx = this->cur_idx;
    }

    ListStackPush (redoact, ract);
    self(adjust.marks, INSERT_LINE, act->idx, act->idx + 1);
    undo_restore (act);

  } else {
    this->head = row;
    this->tail = row;
    this->cur_idx = 0;
    this->current = this->head;
    this->num_items = 1;
    self(adjust.view);
    undo_set (ract, INSERT_LINE);
    ListStackPush (redoact, ract);
  }

  return DONE;
}

static int buf_undo_replace_line (buf_t *this, Action_t *redoact, action_t *act) {
  self(current.set, act->idx);

  action_t *ract = self(action.new);

  self(set.row.idx, act->idx, act->row_pos - $my(dim)->first_row, act->col_pos);
  undo_set (ract, REPLACE_LINE);
  ract->idx = this->cur_idx;
  ract->bytes = Cstring.dup ($mycur(data)->bytes, $mycur(data)->num_bytes);
  ListStackPush (redoact, ract);

  String.replace_with ($mycur(data), act->bytes);

  undo_restore (act);
  return DONE;
}

/* ATTENTION */
/* generally speaking the undo/redo basic functionality seems to be
 * working. what is not working always perfect, is the state of the
 * screen, i think on redoing'it, so this has a very serious bug */
static int buf_undo_exec (buf_t *this, utf8 com) {
  Action_t *Action = NULL;
  if (com is 'u')
    Action = self(undo.pop);
  else
    Action = self(redo.pop);

  if (NULL is Action) return NOTHING_TODO;

  action_t *action = ListStackPop (Action, action_t);

  Action_t *Redoact = self(Action.new);

  while (action) {
    if (action->type is DELETE_LINE)
      self(undo.delete_line, Redoact, action);
    else
      if (action->type is REPLACE_LINE)
        self(undo.replace_line, Redoact, action);
      else
        self(undo.insert, Redoact, action);

    self(action.release, action);
    action = ListStackPop (Action, action_t);
  }

  if (com is 'u')
    self(redo.push, Redoact);
  else {
    $my(undo)->state |= VUNDO_RESET;
    self(undo.push, Redoact);
  }

  free (Action);
  $my(flags) |= BUF_IS_MODIFIED;
  self(draw);
  return DONE;
}

static Action_t *buf_Action_new (buf_t *this) {
  (void) this;
  return Alloc (sizeof (Action_t));
}

static void buf_Action_release (buf_t *this, Action_t *Action) {
  (void) this;
  action_t *action = ListStackPop (Action, action_t);
  while (action) {
    self(action.release, action);
    action = ListStackPop (Action, action_t);
  }

  free (Action);
}

static void buf_Action_set_with_current (buf_t *this, Action_t *Action, int type) {
  action_t *action = self(action.new_with, type, this->cur_idx,
      $mycur(data)->bytes, $mycur(data)->num_bytes);
  ListStackPush (Action, action);
}

static void buf_Action_set_with (buf_t *this, Action_t *Action,
                     int type, int idx, char *bytes, size_t len) {
  action_t *action = self(action.new);
  undo_set (action, type);
  action->idx = ((idx < 0 or idx >= this->num_items) ? this->cur_idx : idx);
  if (NULL is bytes)
    action->bytes = Cstring.dup ($mycur(data)->bytes, $mycur(data)->num_bytes);
  else
    action->bytes = Cstring.dup (bytes, len);

  ListStackPush (Action, action);
}

static action_t *buf_action_new (buf_t *this) {
  (void) this;
  return Alloc (sizeof (action_t));
}

static action_t *buf_action_new_with (buf_t *this, int type, int idx, char *bytes, size_t len) {
  action_t *action = self(action.new);
  undo_set (action, type);
  action->idx = idx;
  action->num_bytes = len;
  action->bytes = Cstring.dup (bytes, len);
  return action;
}

static void buf_action_release (buf_t *this, action_t *action) {
  (void) this;
  if (NULL is action) return;
  ifnot (NULL is action->bytes)
    free (action->bytes);
  free (action);
}

/* a highlight theme derived from tte editor, fork of kilo editor,
 * adjusted and enhanced for the environment
 * written by itself (the very first lines) (iirc somehow after the middle days
 * of February of 2019) 
 */

static char *buf_syn_parser (buf_t *, char *, int, int, row_t *);
static ftype_t *buf_syn_init (buf_t *);
static ftype_t *buf_syn_init_c (buf_t *);
static ftype_t *buf_syn_init_lai (buf_t *);

const char *default_extensions[] = {".txt", NULL};

const char *c_extensions[] = {".c", ".h", ".cpp", ".hpp", ".cc", NULL};
const char *c_keywords[] = {
    "is I", "isnot I", "or I", "and I", "if I", "for I", "return I", "else I",
    "ifnot I", "NULL K", "self I", "this V",
    "OK K", "NOTOK K", "char T", "int T",
    "free F",
    "switch I", "while I", "break I", "continue I", "do I", "default I", "goto I",
    "case I",  "$my V",
    "static I", "#include M", "struct T", "typedef I",
    "uint T", "size_t T", "idx_t T",
    "utf8 T", "uchar T", "sizeof T", "void T",
    "#define M", "#endif M", "#error M", "#ifdef M", "#ifndef M", "#undef M", "#if M", "#else I", "#elif I",
    "union T", "const T", "theend I", "theerror E",
    "Alloc T", "Realloc T",
    "forever I" "loop I",
    "STDIN_FILENO K", "STDOUT_FILENO K", "STDERR_FILENO K",
    "inline I",
    "$myroots V", "$myparents V",
    "$OurRoot V", "$OurRoots V", "public I", "private I",  "mutable I",
    "enum T", "bool T", "long T", "ulong T", "double T", "float T", "unsigned T",
    "selfp I","My V", "$mycur V", "$myprop V", "thisp V", "$from V",
    "extern I", "signed T", "volatile T", "register T", "auto T",
    NULL
};

const char c_singleline_comment[] = "//";
const char c_multiline_comment_start[] = "/*";
const char c_multiline_comment_end[] = "*/";
const char c_multiline_comment_continuation[] = " * ";
const char c_operators[] = "+:?-%*^><=/|&~.()[]{}!";
const char c_balanced_pairs[] = "[](){}";

const char *lai_extensions[] = {".lai", ".build", NULL};
const char *lai_shebangs[] = {
  "#!/bin/env La", "#!/bin/env La-shared",
  "#!/bin/env La-static", "#!La-static",
  "#!/bin/env L", "#!L-static", NULL
};

const char lai_operators[] = "+-%*^><=/|& .(){}![]:$";

const char *lai_keywords[] = {
  "var V", "if I", "ifnot I", "else I", "func I", "is I", "isnot I",
  "orelse I", "then I", "and I", "end I", "or I", "this V", "return I",
  "for I", "while I", "break I", "continue I", "times I", "time I",
  "in I", "as I", "append I", "println F", "format F",
  "null T", "ok T", "notok T", "true T", "false T", "callback I",
  "array V", "map V", "string T", "integer T", "number T", "list T",
  "stdout V", "stderr V", "stdin V",
  "qualifier F", "qualifiers F", "qualifier_exists F",
  "lambda F", "loop I", "forever I", "do I",
  "Type V", "New V", "override V",
  "public V", "private V", "import F", "include F", "loadfile F", "evalfile F",
  "typeof F", "typeofArray F", "typeAsString F", "typeArrayAsString F",
  "len F", "print F", "const V", "__argv V", "__argc V", "__file__ V",
  "__importpath V", "__loadpath V",
   NULL
};

const char lai_singleline_comment[] = "#";
const char lai_multiline_comment_start[] = "##[";
const char lai_multiline_comment_end[] = "##]";
const char lai_multiline_comment_continuation[] = " # ";

const char *NULL_ARRAY[] = {NULL};

syn_t HL_DB[] = {
   {
     "txt", NULL_ARRAY, default_extensions, NULL_ARRAY,
     NULL, NULL,
     NULL, NULL, NULL, NULL,
     HL_STRINGS_NO, HL_NUMBERS_NO, buf_syn_parser, buf_syn_init,
     0, 0, NULL, NULL, c_balanced_pairs,
  },
  {
    "c", NULL_ARRAY, c_extensions, NULL_ARRAY,
    c_keywords, c_operators,
    c_singleline_comment, c_multiline_comment_start, c_multiline_comment_end,
    c_multiline_comment_continuation,
    HL_STRINGS, HL_NUMBERS,
    buf_syn_parser, buf_syn_init_c, 0, 0, NULL, NULL, c_balanced_pairs,
  },
  {
    "lai", NULL_ARRAY, lai_extensions, lai_shebangs, lai_keywords, lai_operators,
    lai_singleline_comment, lai_multiline_comment_start, lai_multiline_comment_end,
    lai_multiline_comment_continuation,
    HL_STRINGS, HL_NUMBERS,
    buf_syn_parser, buf_syn_init_lai, 0, 0, NULL, NULL, c_balanced_pairs
  }
};

#define IsSeparator(c)                          \
  ((c) is ' ' or (c) is '\t' or (c) is '\0' or  \
   Cstring.byte.in_str (",.()+-/=*~%<>[]:;}@", (c)) isnot NULL)

#define IGNORE(c) ((c) > '~' || (c) <= ' ')

#define ADD_COLORED_CHAR(_c, _clr) String.append_with_fmt ($my(shared_str), \
  TERM_SET_COLOR_FMT "%c" TERM_COLOR_RESET,(_clr), (_c))

#define ADD_INVERTED_CHAR(_c, _clr) String.append_with_fmt ($my(shared_str), \
  TERM_INVERTED "%s%c" TERM_COLOR_RESET, TERM_MAKE_COLOR ((_clr)), (_c))

#define SYN_HAS_OPEN_COMMENT       (1 << 0)
#define SYN_HAS_SINGLELINE_COMMENT (1 << 1)
#define SYN_HAS_MULTILINE_COMMENT  (1 << 2)

static int buf_syn_has_mlcmnt (buf_t *this, row_t *row) {
  int found = 0;
  row_t *it = row->prev;

  for (int i = 0; i < MAX_BACKTRACK_LINES_FOR_ML_COMMENTS and it; i++) {
    ifnot (NULL is strstr (it->data->bytes, $my(syn)->multiline_comment_end))
      break;

    char *sp = strstr (it->data->bytes, $my(syn)->multiline_comment_start);

    if (NULL isnot sp)
      if (sp is it->data->bytes or
          it->data->bytes[sp - it->data->bytes - 1] is ' ' or
          it->data->bytes[sp - it->data->bytes - 1] is '\t') {
        found = 1;
        break;
      }

    ifnot (NULL is $my(syn)->multiline_comment_continuation)
      if (Cstring.eq_n (it->data->bytes, $my(syn)->multiline_comment_continuation,
      	$my(syn)->multiline_comment_continuation_len)) {
        found = 1;
        break;
      }

    it = it->prev;
  }

  return found;
}

#define IsAlsoAHex(c)    (((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define IsAlsoANumber(c) ((c) == '.' || (c) == 'x' || IsAlsoAHex (c))

/* Sorry but the highlight system is ridiculously simple (word by word), but
 * is fast and works for me in C at least */
static char *buf_syn_parser (buf_t *this, char *line, int len, int index, row_t *row) {
  (void) index;

  ifnot (len) return line;

  /* make sure to reset, in the case the last character on previous line
   * is in e.g., a string. Do it here instead with an "if" later on */
  String.replace_with_len ($my(shared_str), TERM_COLOR_RESET, TERM_COLOR_RESET_LEN);

  char *m_cmnt_p = NULL;
  int m_cmnt_idx = -1;
  int has_mlcmnt  = 0;

  ifnot (NULL is $my(syn)->multiline_comment_start) {
    m_cmnt_p = strstr (line, $my(syn)->multiline_comment_start);
    m_cmnt_idx = (NULL is m_cmnt_p ? -1 : m_cmnt_p - line);

    if (m_cmnt_idx > 0)
      if (line[m_cmnt_idx-1] isnot ' ' and line[m_cmnt_idx-1] isnot '\t') {
        m_cmnt_idx = -1;
        m_cmnt_p = NULL;
      }

    if (m_cmnt_idx is -1) has_mlcmnt = buf_syn_has_mlcmnt (this, row);
  }

  char *s_cmnt_p = NULL;
  int s_cmnt_idx = -1;

  if (m_cmnt_idx is -1)
    ifnot (NULL is $my(syn)->singleline_comment) {
      s_cmnt_p = strstr (line, $my(syn)->singleline_comment);
      s_cmnt_idx = (NULL is s_cmnt_p ? -1 : s_cmnt_p - line);
    }

  uchar c;
  int idx = 0;

  for (idx = 0; idx < len; idx++) {
    c = line[idx];
    goto parse_char;

parse_comment:
    if ($my(syn)->state & SYN_HAS_OPEN_COMMENT) {
      $my(syn)->state &= ~SYN_HAS_OPEN_COMMENT;
      int diff = len;
      String.append_with_fmt ($my(shared_str), "%s%s", TERM_MAKE_COLOR (HL_COMMENT), TERM_ITALIC);

      if ($my(syn)->state & SYN_HAS_MULTILINE_COMMENT) {
        $my(syn)->state &= ~SYN_HAS_MULTILINE_COMMENT;
        char *sp = strstr (line + idx, $my(syn)->multiline_comment_end);
        if (sp is NULL) {
          while (idx < len)
            String.append_byte ($my(shared_str), line[idx++]);

          String.append_with ($my(shared_str), TERM_COLOR_RESET);
          goto theend;
        }

        diff = idx + (sp - (line + idx)) + (int) bytelen ($my(syn)->multiline_comment_end);

      } else
        $my(syn)->state &= ~SYN_HAS_SINGLELINE_COMMENT;

      while (idx < diff) String.append_byte ($my(shared_str), line[idx++]);
      String.append_with ($my(shared_str), TERM_COLOR_RESET);
      if (idx is len) goto theend;
      c = line[idx];
    }

parse_char:
    while (IGNORE (c)) {
      if (c is ' ') {
        if (idx + 1 is (int) row->data->num_bytes) {
          ADD_INVERTED_CHAR (' ', HL_TRAILING_WS);
        } else
          String.append_byte ($my(shared_str), c);
      } else
        if (c is '\t') {
          for (int i = 0; i < $my(ftype)->tabwidth; i++)
            String.append_byte ($my(shared_str), ' ');
        } else
          if ((c < ' ' and (c isnot 0 and c isnot 0x0a)) or c is 0x7f) {
            ADD_INVERTED_CHAR ('?', HL_ERROR);
          }
          else {
            String.append_byte ($my(shared_str),  c);
          }

      if (++idx is len) goto theend;
      c = line[idx];
    }

    if (has_mlcmnt or idx is m_cmnt_idx) {
      has_mlcmnt = 0;
      $my(syn)->state |= (SYN_HAS_OPEN_COMMENT|SYN_HAS_MULTILINE_COMMENT);
      goto parse_comment;
    }

    if (idx is s_cmnt_idx)
      if (s_cmnt_idx is 0 or line[s_cmnt_idx-1] is ' ' or line[s_cmnt_idx-1] is '\t') {
        $my(syn)->state |= (SYN_HAS_OPEN_COMMENT|SYN_HAS_SINGLELINE_COMMENT);
        goto parse_comment;
      }

    ifnot (NULL is $my(syn->operators)) {
      int lidx = idx;
      while (NULL isnot Cstring.byte.in_str ($my(syn)->operators, c)) {
        ADD_COLORED_CHAR (c, HL_OPERATOR);
        if (++idx is len) goto theend;
        c = line[idx];
      }

      if (idx isnot lidx) goto parse_char;
    }

    if ($my(syn)->hl_strings)
      if (c is '"' or c is '\'') {
        String.append_with_fmt ($my(shared_str),
          TERM_SET_COLOR_FMT "%c" TERM_SET_COLOR_FMT "%s",
            HL_STRING_DELIM, c, HL_STRING, TERM_ITALIC);
        char openc = c;
        while (++idx < len) {
          char prevc = c;
          c = line[idx];
          if (c is openc and (prevc isnot '\\' or (prevc is '\\' and line[idx-2] is '\\'))) {
            ADD_COLORED_CHAR (c, HL_STRING_DELIM);
            goto next_char;
          }

          String.append_byte ($my(shared_str),  c);
        }

        goto theend;
      }

    if ($my(syn)->hl_numbers)
      if (IS_DIGIT (c)) {
        ADD_COLORED_CHAR (c, HL_NUMBER);
        while (++idx < len) {
          c = line[idx];
          if (0 is IS_DIGIT (c) and 0 is IsAlsoANumber (c))
            goto parse_char;

          ADD_COLORED_CHAR (c, HL_NUMBER);
        }

        goto theend;
      }

    if (NULL isnot $my(syn)->keywords and (idx is 0 or IsSeparator (line[idx-1])))
      for (int j = 0; $my(syn)->keywords[j] isnot NULL; j++) {
        int kw_len = $my(syn)->keywords_len[j];
        // int is_glob = $my(syn)->keywords[j][kw_len] is '*';if (is_glob) kw_len--;

        if (Cstring.eq_n (&line[idx], $my(syn)->keywords[j], kw_len) and
           IsSeparator (line[idx + kw_len])) {
         // (0 is is_glob ? IsSeparator (line[idx + kw_len]) : 1)) {
          int color = $my(syn)->keywords_colors[j];
          String.append_with ($my(shared_str), TERM_MAKE_COLOR (color));

          for (int i = 0; i < kw_len; i++)
            String.append_byte ($my(shared_str), line[idx+i]);

          idx += (kw_len - 1);

          // ifnot (is_glob) {
          String.append_with ($my(shared_str), TERM_COLOR_RESET);
          goto next_char;
          // }
          /*
          idx++;
          while (idx < len) String.append_byte ($my(shared_str), line[idx++]);
          String.append_with ($my(shared_str), TERM_COLOR_RESET);
          goto theend;
          */
        }
      }

    String.append_byte ($my(shared_str),  c);

next_char:;
  }

theend:
  Cstring.cp (line, MAXLEN_LINE, $my(shared_str)->bytes, $my(shared_str)->num_bytes);
  return line;
}

static void balanced_push (balanced_t *this, char obj, int idx) {
  this->bytes[++this->last_idx] = obj;
  this->linenr[this->last_idx] = idx;
}

static char balanced_pop (balanced_t *this) {
  char c = this->bytes[this->last_idx];
  this->bytes[this->last_idx--] = '\0';
  return c;
}

static int balanced_check_obj (char *pair, char ca, char cb) {
  if (ca is *pair) return (cb is *(pair + 1) ? OK : NOTOK);
  return NOTOK;
}

static int buf_balanced_obj (buf_t **thisp, int first_idx, int last_idx) {
  int retval = NOTOK;
  buf_t *this = *thisp;

  if ($my(syn)->balanced_pairs is NULL) return NOTOK;

  balanced_t balanced = (balanced_t) {
    .last_idx = -1,
    .has_opening_string = 0
  };

  int idx = first_idx;

theloop:
  while (idx <= last_idx) {
    string_t *data = self(get.row.bytes_at, idx++);
    if (data is NULL) break;

    for (size_t i = 0; i < data->num_bytes; i++) {
      char c = data->bytes[i];

      ifnot (balanced.has_opening_string) {
        ifnot (NULL is $my(syn)->singleline_comment) {
          if (c is $my(syn)->singleline_comment[0]) {
            if ((bytelen ($my(syn)->singleline_comment) is 1) or
                ((data->num_bytes - 1 > i and data->bytes[i+1] is
                 $my(syn)->singleline_comment[1]))) {
              goto theloop;
            }
          }
        }
      }

      if (c is '"') {
        if ((i isnot 0 and data->bytes[i-1] is '\\' and balanced.has_opening_string)
          or ((i isnot 0 and data->bytes[i-1] is '\'') and
           ((data->num_bytes - 1 > i and data->bytes[i+1] is '\''))))
         continue;

          if (balanced.has_opening_string)
            balanced.has_opening_string = 0;
          else
            balanced.has_opening_string = 1;

        continue;
      }

      if (balanced.has_opening_string) continue;

      char *sp = Cstring.byte.in_str ($my(syn)->balanced_pairs, c);
      ifnot (NULL is sp) {
        if (i isnot 0 and data->bytes[i-1] is '\'' and
          (data->num_bytes - 1 > i and data->bytes[i+1] is '\'')) continue;

        ifnot ((sp - $my(syn)->balanced_pairs) % 2) {
          balanced_push (&balanced, c, idx);
          continue;
        }

        if (balanced.last_idx is -1) {
          Msg.write_fmt ($my(root), "no opening objects to match close object |%c| at: "
              "%d line number", c, idx);
          retval = NOTOK;
          goto theend;
        }

        char last_p = balanced_pop (&balanced);

        if (NOTOK is balanced_check_obj (sp - 1, last_p, c)) {
          Msg.write_fmt ($my(root), "%c opened at %d but closed object is %c at %d",
             last_p, balanced.linenr[balanced.last_idx + 1], c, idx + 1);
          goto theend;
        }
      }

      continue;
    }
  }

  if (balanced.last_idx isnot -1) {
     char last_p = balanced_pop (&balanced);

     Msg.write_fmt ($my(root), "%c opened at %d and didn't found it's closed pair", last_p,
         balanced.linenr[balanced.last_idx + 1]);
     retval = NOTOK;
  } else
    retval = OK;

theend:
  if (NOTOK is retval)
    Ed.messages ($my(root), thisp, NOT_AT_EOF);
  else
    Msg.line ($my(root), COLOR_NORMAL, "pair objects looks symmetrical");

  return retval;
}

static int buf_balanced_lw_mode_cb (buf_t **thisp, int fidx, int lidx, Vstring_t *vstr, utf8 c, char *action) {
  (void) vstr; (void) c; (void) action;
  if (c is 'b')
    return buf_balanced_obj (thisp, fidx, lidx);
  return NO_CALLBACK_FUNCTION;
}

static int i_syntax_error_to_ed (la_t *__la__, const char *msg) {
  E_T *e = La.get.user_data (__la__);
  ed_t *this = e->prop->current;
  char *ptr = La.get.eval_str (__la__);
  Msg.write_fmt (this, "\nSYNTAX ERROR: %s\nbefore:\n%s\n", msg, ptr);
  return LA_ERR_SYNTAX;
}

static int buf_interpret (buf_t **thisp, char *malloced) {
  buf_t *this = *thisp;
  char *str = malloced;

  la_t *in = La.get.current ($my(__LA__));
  ifnot (in)
    in = La.init_instance ($my(__LA__), LaOpts (
      .define_funs_cb = $OurRoots (la_define_funs_cb),
      .syntax_error = i_syntax_error_to_ed));

  La.set.user_data (in, $OurRoot);

  Term.reset ($my(term_ptr));

  int retval = La.eval_string (in, str);

  free (malloced);

  Term.set_mode ($my(term_ptr), 'r');
  Input.getkey (STDIN_FILENO);
  Term.set ($my(term_ptr));

  if (retval is LA_ERR_SYNTAX)
    Ed.messages ($my(root), thisp, AT_EOF);

  this = *thisp;

  Win.draw ($my(parent));

  if (retval isnot OK or retval isnot NOTOK)
    retval = NOTOK;
  return retval;
}

static int buf_eval_expression (buf_t **thisp, int fidx, int lidx, string_t *str, utf8 c, char *action) {
  (void) fidx; (void) lidx; (void) action;

  if (c isnot '$') return NO_CALLBACK_FUNCTION;

  ifnot (str->num_bytes) return NOTOK;

  buf_t *this = *thisp;
  ed_t *ed = $my(root);

  la_t *la = La.get.current ($my(__LA__));
  ifnot (la)
    la = La.init_instance ($my(__LA__), LaOpts (
      .define_funs_cb = $OurRoots (la_define_funs_cb),
      .syntax_error = i_syntax_error_to_ed));

  La.set.user_data (la, $OurRoot);

  if (str->bytes[0] isnot '(')
    String.prepend_byte (str, '(');

  if (str->bytes[str->num_bytes - 1] isnot ')')
    String.append_byte (str, ')');

  VALUE v;
  int retval = La.eval_expr (la, str->bytes, &v);

  if (retval isnot LA_OK) {
    Ed.messages (ed, thisp, NOT_AT_EOF);
    return NOTOK;
  }

  char buf[256]; buf[0] = '\0';

  switch (v.type) {
    case NUMBER_TYPE:
      snprintf (buf, 256, "%f", AS_NUMBER(v));
      break;

    case STRING_TYPE:
      Ed.reg.set (ed, '$', CHARWISE, AS_STRING_BYTES(v), NORMAL_ORDER);
      Msg.send_fmt (ed, COLOR_NORMAL, "Result =  %s (stored to '$' register)",
          AS_STRING_BYTES(v));
      return OK;

    default:
      snprintf (buf, 256, "%zd", AS_INT(v));
  }

  Ed.reg.set (ed, '$', CHARWISE, buf, NORMAL_ORDER);
  Msg.send_fmt (ed, COLOR_NORMAL, "Result =  %s (stored to '$' register)", buf);

  return OK;
}

static int buf_evaluate_lw_mode_cb (buf_t **thisp, int fidx, int lidx, Vstring_t *vstr, utf8 c, char *action) {
  (void) fidx; (void) lidx; (void) action;

  if (c isnot '@') return NO_CALLBACK_FUNCTION;

  buf_t *this = *thisp;
  ifnot ($OurRoots(uid))
    return NO_CALLBACK_FUNCTION;

  char *str = Vstring.to.cstring (vstr, ADD_NL);
  return buf_interpret (thisp, str);
}

static string_t *buf_ftype_autoindent (buf_t *this, row_t *row) {
  (void) row;
  $my(shared_int) = 0; // needed by the caller
  char s[$my(shared_int) + 1];
  for (int i = 0; i < $my(shared_int); i++) s[i] = ' ';
  s[$my(shared_int)] = '\0';
  String.replace_with ($my(shared_str), s);
  return $my(shared_str);
}

static string_t *buf_autoindent_c (buf_t *this, row_t *row) {
  do {
    if (row->data->num_bytes < 2 or row->data->bytes[0] is '}') {
      $my(shared_int) = 0;
      break;
    }

    int ws = 0;
    char *line = row->data->bytes;
    while (*line and *line++ is ' ') ws++;
    if (ws is (int) row->data->num_bytes) {
      $my(shared_int) = 0;
      break;
    }

    int len = row->data->num_bytes - ws - 2;
    if (len > 0) while (len--) line++;
    if (*line is '\n') line--;
    if (*line is ';' or *line is ',')
      $my(shared_int) = ws;
    else
      $my(shared_int) = ws + $my(ftype)->shiftwidth;
  } while (0);

  char s[$my(shared_int) + 1];
  for (int i = 0; i < $my(shared_int) ; i++) s[i] = ' ';
  s[$my(shared_int)] = '\0';
  String.replace_with ($my(shared_str), s);
  return $my(shared_str);
}

static ftype_t *__ftype_new__ (syn_t *syn) {
  ftype_t *this = Alloc (sizeof (ftype_t));
  Cstring.cp (this->name, MAXLEN_FTYPE_NAME, syn->filetype, MAXLEN_FTYPE_NAME - 1);
  return this;
}

static void buf_ftype_release (buf_t *this) {
  if (this is NULL or $myprop is NULL or $my(ftype) is NULL) return;
  String.release ($my(ftype)->on_emptyline);
  free ($my(ftype));
  $my(ftype) = NULL;
}

static int ed_syn_get_ftype_idx (ed_t *this, const char *name) {
  if (NULL is name) return FTYPE_DEFAULT;

  for (int i = 0; i < $my(num_syntaxes); i++) {
    if (Cstring.eq ($my(syntaxes)[i].filetype, name))
      return i;
  }

  return FTYPE_DEFAULT;
}

static ftype_t *__ftype_set__ (ftype_t *this, ftype_t q) {
  this->autochdir = q.autochdir;
  this->shiftwidth = q.shiftwidth;
  this->tabwidth = q.tabwidth;
  this->clear_blanklines = q.clear_blanklines;
  this->tab_indents = q.tab_indents;
  this->cr_on_normal_is_like_insert_mode = q.cr_on_normal_is_like_insert_mode;
  this->backspace_on_normal_is_like_insert_mode = q.backspace_on_normal_is_like_insert_mode;
  this->backspace_on_normal_goes_up = q.backspace_on_normal_goes_up;
  this->backspace_on_insert_goes_up_and_join = q.backspace_on_insert_goes_up_and_join;
  this->backspace_on_first_idx_remove_trailing_spaces = q.backspace_on_first_idx_remove_trailing_spaces;
  this->small_e_on_normal_goes_insert_mode = q.small_e_on_normal_goes_insert_mode;
  this->space_on_normal_is_like_insert_mode = q.space_on_normal_is_like_insert_mode;
  this->read_from_shell = q.read_from_shell;

  this->autoindent = (NULL is q.autoindent ? buf_ftype_autoindent : q.autoindent);
  this->on_open_fname_under_cursor = q.on_open_fname_under_cursor;
  this->balanced = q.balanced;

  if (NULL is q.on_emptyline) {
    if (NULL is this->on_emptyline)
      this->on_emptyline = String.new_with (DEFAULT_ON_EMPTY_LINE_STRING);
  } else
    this->on_emptyline = q.on_emptyline;

  return this;
}

static ftype_t *buf_ftype_init (buf_t *this, int ftype, FtypeAutoIndent_cb indent_cb) {
  if (ftype >= $myroots(num_syntaxes) or ftype < 0) ftype = 0;
  $my(syn) = &$myroots(syntaxes)[ftype];
  return __ftype_set__ (__ftype_new__ ($my(syn)),
    FtypeOpts (.autoindent = indent_cb));
}

static ftype_t *buf_ftype_set (buf_t *this, int ftype, ftype_t q) {
  if (ftype >= $myroots(num_syntaxes) or ftype < 0) ftype = 0;
  $my(syn) = &$myroots(syntaxes)[ftype];
  if (NULL is $my(ftype))
    $my(ftype) = __ftype_new__ ($my(syn));

  return __ftype_set__ ($my(ftype), q);
}

static int readline_parse_arg_buf_range (readline_t *rl, readline_arg_t *arg, buf_t *this) {
  if (arg is NULL) {
    rl->range[0] = rl->range[1] = this->cur_idx;
    return OK;
  }

  if (arg->argval->num_bytes is 1) {
    if (arg->argval->bytes[0] is '%') {
      rl->range[0] = 0; rl->range[1] = this->num_items - 1;
      return OK;
    }

    if (arg->argval->bytes[0] is '.') {
      rl->range[0] = rl->range[1] = this->cur_idx;
      return OK;
    }

    if ('0' < arg->argval->bytes[0] and arg->argval->bytes[0] <= '9') {
      rl->range[0] = rl->range[1] = (arg->argval->bytes[0] - '0') - 1;
      if (rl->range[0] >= this->num_items) return NOTOK;
      return OK;
    }

    return NOTOK;
  }

  char *sp = Cstring.byte.in_str (arg->argval->bytes, ',');

  if (NULL is sp) {
    sp = arg->argval->bytes;

    int num = 0;
    int idx = 0;
    while ('0' <= *sp and *sp <= '9' and idx++ <= MAX_COUNT_DIGITS)
      num = (10 * num) + (*sp++ - '0');

    if (*sp isnot 0) return NOTOK;
    rl->range[0] = rl->range[1] = num - 1;
    if (rl->range[0] >= this->num_items or rl->range[0] < 0) return NOTOK;
    return OK;
  }

  int diff = sp - arg->argval->bytes;
  sp++;
  do {
    if (*sp is '.') {
      if (*(sp + 1) isnot 0) return NOTOK;
      rl->range[1] = this->cur_idx;
      break;
    }

    if (*sp is '$') {
      if (*(sp + 1) isnot 0) return NOTOK;
      rl->range[1] = this->num_items - 1;
      break;
    }

    if (*sp > '9' or *sp < '0') return NOTOK;
    int num = 0;
    int idx = 0;
    while ('0' <= *sp and *sp <= '9' and idx++ <= MAX_COUNT_DIGITS)
      num = (10 * num) + (*sp++ - '0');

    if (*sp isnot 0) return NOTOK;
    rl->range[1] = num - 1;
  } while (0);

  String.clear_at (arg->argval, diff);
  ifnot (arg->argval->num_bytes) return NOTOK;
  sp = arg->argval->bytes;

  loop (1) {
    if (*sp is '.') {
      if (*(sp + 1) isnot 0) return NOTOK;
      rl->range[0] = this->cur_idx;
      break;
    }

    if (*sp > '9' or *sp < '0') return NOTOK;
    int num = 0;
    int idx = 0;
    while ('0' <= *sp and *sp <= '9' and idx++ <= MAX_COUNT_DIGITS)
      num = (10 * num) + (*sp++ - '0');

    if (*sp isnot 0) return NOTOK;
    rl->range[0] = num - 1;
  } while (0);

  if (rl->range[0] < 0) return NOTOK;
  if (rl->range[0] > rl->range[1]) return NOTOK;
  if (rl->range[1] >= this->num_items) return NOTOK;
  return OK;
}

static int readline_get_buf_range (readline_t  *rl, buf_t *this, int *range) {
  readline_arg_t *arg = Readline.get.arg (rl, READLINE_ARG_RANGE);
  if (NULL is arg or (NOTOK is readline_parse_arg_buf_range (rl, arg, this))) {
    range[0] = -1; range[1] = -1;
    return NOTOK;
  }

  range[0] = rl->range[0];
  range[1] = rl->range[1];

  return OK;
}

/* this retval pointer provides information, since the callee resets retval at the
 * beginning of its function. The early return here is actually a goto theend there */
static int buf_com_substitute (buf_t *this, readline_t *rl, int *retval) {
  readline_arg_t *pat = Readline.get.arg (rl, READLINE_ARG_PATTERN);
  readline_arg_t *sub = Readline.get.arg (rl, READLINE_ARG_SUB);
  readline_arg_t *global = Readline.get.arg (rl, READLINE_ARG_GLOBAL);
  readline_arg_t *interactive = Readline.get.arg (rl, READLINE_ARG_INTERACTIVE);
  readline_arg_t *range = Readline.get.arg (rl, READLINE_ARG_RANGE);

  if (NULL is range and rl->com is VED_COM_SUBSTITUTE_WHOLE_FILE_AS_RANGE) {
    rl->range[0] = 0; rl->range[1] = this->num_items - 1;
  } else
    if (NOTOK is readline_parse_arg_buf_range (rl, range, this))
      return *retval;

  if (Readline.arg.exists (rl, "remove-tabs")) {
    int shiftwidth = $my(ftype)->shiftwidth;
    string_t *sw = Readline.get.anytype_arg (rl, "shiftwidth");
    ifnot (NULL is sw) shiftwidth = atoi (sw->bytes);
    char subst[shiftwidth];
    for (int i = 0; i < shiftwidth; i++) subst[i] = ' ';
      subst[shiftwidth] = '\0';
    return buf_substitute (this, "\t", subst, GLOBAL, interactive isnot NULL, rl->range[0], rl->range[1]);
  }

  if (Readline.arg.exists (rl, "remove-doseol")) {
    return buf_substitute (this, "\x0d", "", GLOBAL, interactive isnot NULL, rl->range[0], rl->range[1]);
  }

  if (Readline.arg.exists (rl, "interpret-ctrl-backspace")) {
    return buf_substitute (this, ".\x08", "", GLOBAL, interactive isnot NULL, rl->range[0], rl->range[1]);
  }

  if (pat is NULL or sub is NULL) return *retval;

  *retval = buf_substitute (this, pat->argval->bytes, sub->argval->bytes,
     global isnot NULL, interactive isnot NULL, rl->range[0], rl->range[1]);

  return *retval;
}

static int buf_com_set (buf_t *this, readline_t *rl) {
  int draw = 0;

  string_t *arg = Readline.get.anytype_arg (rl, "ftype");
  ifnot (NULL is arg) {
    int idx = Ed.syn.get_ftype_idx ($my(root), arg->bytes);
    syn_t syn = $myroots(syntaxes)[idx];
    ifnot (Cstring.eq (syn.filetype, $my(ftype)->name)) {
      self(ftype.release);
      $my(ftype) = syn.init (this);
      Cstring.cp ($my(ftype)->name, MAXLEN_FTYPE_NAME, $my(syn)->filetype, MAXLEN_FTYPE_NAME - 1);
      draw = 1;
    }
  }

  arg = Readline.get.anytype_arg (rl, "tabwidth");
  ifnot (NULL is arg) {
    $my(ftype)->tabwidth = atoi (arg->bytes);
    self(normal.bol, DONOT_DRAW);
    draw = 1;
  }

  arg = Readline.get.anytype_arg (rl, "shiftwidth");
  ifnot (NULL is arg) {
    $my(ftype)->shiftwidth = atoi (arg->bytes);
    self(normal.bol, DONOT_DRAW);
    draw = 1;
  }

  arg = Readline.get.anytype_arg (rl, "autosave");
  ifnot (NULL is arg) {
    long minutes = atol (arg->bytes);
    if (minutes)
      self(set.autosave, minutes);
  }

  arg = Readline.get.anytype_arg (rl, "save-image");
  ifnot (NULL is arg)
    Root.set.save_image ($OurRoot, atoi (arg->bytes));

  arg = Readline.get.anytype_arg (rl, "image-file");
  ifnot (NULL is arg)
    Root.set.image_file ($OurRoot, arg->bytes);

  arg = Readline.get.anytype_arg (rl, "image-name");
  ifnot (NULL is arg)
    Root.set.image_name ($OurRoot, arg->bytes);

  arg = Readline.get.anytype_arg (rl, "persistent-layout");
  ifnot (NULL is arg)
    Root.set.persistent_layout ($OurRoot, atoi (arg->bytes));

  arg = Readline.get.anytype_arg (rl, "lang-mode");
  ifnot (NULL is arg)
    Ed.set.lang_mode ($my(root), arg->bytes);

  if (Readline.arg.exists (rl, "backupfile")) {
    arg = Readline.get.anytype_arg (rl, "backup-suffix");
    self(set.backup, 1, (NULL is arg ? BACKUP_SUFFIX : arg->bytes));
  }

  if (Readline.arg.exists (rl, "no-backupfile"))
    ifnot (NULL is $my(backupfile)) {
      free ($my(backupfile));
      $my(backupfile) = NULL;
    }

  if (Readline.arg.exists (rl, "enable-writing"))
    $my(enable_writing) = 1;

  arg = Readline.get.anytype_arg (rl, "save-on-exit");
  ifnot (NULL is arg)
    self(set.save_on_exit, atoi (arg->bytes));

  if (draw) self(draw);

  return OK;
}

static char *ftype_on_open_fname_under_cursor_c (char *fname,
                                size_t len, size_t stack_size) {
  if (len < 8 or (*fname isnot '<' and fname[len-1] isnot '>'))
    return fname;

  char incl_dir[] = {"/usr/include"};
  size_t tlen = len + bytelen (incl_dir) + 1 - 2; // + / - <>

  if (tlen + 1 > stack_size) return fname;

  char t[tlen + 1];
  snprintf (t, tlen + 1, "%s/%s", incl_dir, fname + 1);
  for (size_t i = 0; i < tlen; i++) fname[i] = t[i];
  fname[tlen] = '\0';

  return fname;
}

static ftype_t *buf_syn_init_c (buf_t *this) {
  int idx = Ed.syn.get_ftype_idx ($my(root), "c");
  return self(ftype.set, idx, FtypeOpts (
    .autoindent = buf_autoindent_c,
    .shiftwidth = C_DEFAULT_SHIFTWIDTH,
    .tab_indents = C_TAB_ON_INSERT_MODE_INDENTS,
    .on_open_fname_under_cursor = ftype_on_open_fname_under_cursor_c,
    .balanced = buf_balanced_obj
    ));
}

static ftype_t *buf_syn_init_lai (buf_t *this) {
  int idx = Ed.syn.get_ftype_idx ($my(root), "lai");
  return self(ftype.set, idx, FtypeOpts (
      .autoindent = buf_autoindent_c,
      .shiftwidth = C_DEFAULT_SHIFTWIDTH,
      .tab_indents = C_TAB_ON_INSERT_MODE_INDENTS,
      .balanced = buf_balanced_obj
      ));
}

static ftype_t *buf_syn_init (buf_t *this) {
  return self(ftype.init, FTYPE_DEFAULT, buf_ftype_autoindent);
}

static void buf_set_ftype (buf_t *this, int ftype) {
  if (FTYPE_DEFAULT < ftype and ftype < $myroots(num_syntaxes)) {
    $my(ftype) = $myroots(syntaxes)[ftype].init (this);
    return;
  }

  for (int i = 0; i < $myroots(num_syntaxes); i++) {
    int j = 0;
    while ($myroots(syntaxes)[i].filenames[j])
      if (Cstring.eq ($myroots(syntaxes)[i].filenames[j++], $my(basename))) {
        $my(ftype) = $myroots(syntaxes)[i].init (this);
        return;
      }

    if (NULL is $my(extname)) continue;

    j = 0;
    while ($myroots(syntaxes)[i].extensions[j])
      if (Cstring.eq ($myroots(syntaxes)[i].extensions[j++], $my(extname))) {
        $my(ftype) = $myroots(syntaxes)[i].init (this);
        return;
      }

    if (NULL is this->head or this->head->data->num_bytes < 2) continue;

    j = 0;
    while ($myroots(syntaxes)[i].shebangs[j]) {
      if (Cstring.eq_n ($myroots(syntaxes)[i].shebangs[j], this->head->data->bytes,
          bytelen ($myroots(syntaxes)[i].shebangs[j]))) {
        $my(ftype) = $myroots(syntaxes)[i].init (this);
        return;
      }

      j++; /* gcc complains (and probably for a right) if j++ at the end of the
            * conditional expression (even if it is right) */
     }
  }

  $my(ftype) = self(syn.init);
}

static void buf_set_modified (buf_t *this) {
  $my(flags) |= BUF_IS_MODIFIED;
}

static row_t *buf_row_new_with (buf_t *this, const char *bytes) {
  (void) this;
  row_t *row = Alloc (sizeof (row_t));
  string_t *data = String.new_with (bytes);
  row->data = data;
  return row;
}

static row_t *buf_row_new_with_len (buf_t *this, const char *bytes, size_t len) {
  (void) this;
  row_t *row = Alloc (sizeof (row_t));
  string_t *data = String.new_with_len (bytes, len);
  row->data = data;
  return row;
}

static int buf_get_row_col_idx (buf_t *this, row_t *row) {
  (void) this;
  return row->cur_col_idx;
}

static row_t *buf_get_row_at (buf_t *this, int idx) {
  return DListGetAt (this, row_t, idx);
}

static row_t *buf_get_row_current (buf_t *this) {
  return this->current;
}

static int buf_get_current_row_idx (buf_t *this) {
  return this->cur_idx;
}

static int buf_get_current_col_idx (buf_t *this) {
  return $mycur(cur_col_idx);
}

static string_t *buf_get_row_current_bytes (buf_t *this) {
  return $mycur(data);
}

static string_t *buf_get_row_bytes_at (buf_t *this, int idx) {
  (void) this;
  row_t *row = self(get.row.at, idx);
  if (NULL is row) return NULL;
  return row->data;
}

static void buf_release_row (buf_t *this, row_t *row) {
  (void) this;
  if (row is NULL) return;
  String.release (row->data);
  free (row);
}

static void buf_release_line (buf_t *this) {
  if (this is NULL or $myprop is NULL or $my(line) is NULL) return;
  Ustring.release_members ($my(line));
  free ($my(line));
}

static void buf_jumps_release (buf_t *this) {
  jump_t *jump = $my(jumps)->head;
  while (jump) {
    jump_t *tmp = jump->next;
    free (jump->mark);
    free (jump);
    jump = tmp;
  }

  free ($my(jumps));
}

static void buf_jumps_init (buf_t *this) {
  if (NULL is $my(jumps)) {
    $my(jumps) = Alloc (sizeof (Jump_t));
    $my(jumps)->old_idx = -1;
  }
}

static void buf_jump_push (buf_t *this, mark_t *mark) {
  jump_t *jump = Alloc (sizeof (jump_t));
  mark_t *lmark = Alloc (sizeof (mark_t));
  state_cp (lmark, mark);
  jump->mark = lmark;

  if ($my(jumps)->num_items >= 20) {
    jump_t *tmp = ListStackPopTail ($my(jumps), jump_t);
    free (tmp->mark);
    free (tmp);
    $my(jumps)->num_items--;
  }

  ListStackPush ($my(jumps), jump);
  $my(jumps)->num_items++;
  $my(jumps)->cur_idx = 0;
}

static int mark_get_idx (int c) {
  char marks[] = MARKS; /* this is for tcc */
  char *m = Cstring.byte.in_str (marks, c);
  if (NULL is m) return -1;
  return m - marks;
}

static int buf_mark_set (buf_t *this, int mark) {
  if (mark < 0) {
    mark = mark_get_idx (Input.getkey (STDIN_FILENO));
    if (-1 is mark) return NOTHING_TODO;
  }

  state_set (&$my(marks)[mark]);
  $my(marks)[mark].cur_idx = this->cur_idx;

  if (mark isnot MARK_UNNAMED)
    MSG("set [%c] mark", MARKS[mark]);

  self(jump.push, &$my(marks)[mark]);
  return DONE;
}

static int buf_mark_jump (buf_t *this) {
  int c = mark_get_idx (Input.getkey (STDIN_FILENO));
  if (-1 is c) return NOTHING_TODO;

  mark_t *mark = &$my(marks)[c];
  mark_t t;  state_set (&t);  t.cur_idx = this->cur_idx;

  if (NOTHING_TODO is self(mark.restore, mark))
    return NOTHING_TODO;

  $my(marks)[MARK_UNNAMED] = t;

  self(draw);
  return DONE;
}

static int buf_jump_to (buf_t *this, int dir) {
  ifnot ($my(jumps)->num_items) return NOTHING_TODO;

  jump_t *jump = $my(jumps)->head;
  if (dir is LEFT_DIRECTION) {
    for (int i = 0; i + 1 < $my(jumps)->cur_idx; i++)
      jump = jump->next;
    $my(jumps)->old_idx = $my(jumps)->cur_idx;
    if ($my(jumps)->cur_idx) $my(jumps)->cur_idx--;
    goto theend;
  }

  if ($my(jumps)->cur_idx + 1 is $my(jumps)->num_items and
      $my(jumps)->old_idx + 1 is $my(jumps)->num_items)
    return NOTHING_TODO;

  for (int i = 0; i < $my(jumps)->cur_idx; i++)
    jump = jump->next;

  if ($my(jumps)->cur_idx is 0) { // and $my(jumps)->num_items is 1) {
    mark_t m; state_set (&m); m.cur_idx = this->cur_idx;
    self(jump.push, &m);
  }

  $my(jumps)->old_idx = $my(jumps)->cur_idx;
  if ($my(jumps)->cur_idx + 1 isnot $my(jumps)->num_items)
    $my(jumps)->cur_idx++;

theend:
  state_set (&$my(marks)[MARK_UNNAMED]);
  $my(marks)[MARK_UNNAMED].cur_idx = this->cur_idx;

  self(mark.restore, jump->mark);
  self(draw);
  return DONE;
}

static void buf_iter_release (buf_t *unused, bufiter_t *this) {
  (void) unused;
  if (NULL is this) return;
  free (this);
}

static bufiter_t *buf_iter_new (buf_t *this, int start_idx) {
  bufiter_t *it = Alloc (sizeof (bufiter_t));
  if (start_idx < 0) start_idx = this->cur_idx;

  it->row = self(get.row.at, start_idx);
  it->num_lines = this->num_items - start_idx;
  if (it->row) {
    it->line = it->row->data;
    it->idx = start_idx;
    it->col_idx = it->row->cur_col_idx;
  } else {
    it->line = NULL;
    it->idx = -1;
  }
  return it;
}

static bufiter_t *buf_iter_next (buf_t *unused, bufiter_t *this) {
  (void) unused;
  if (this->row) this->row = this->row->next;
  if (this->row) {
    this->line = this->row->data;
    this->idx++;
    this->num_lines--;
    this->col_idx = this->row->cur_col_idx;
  } else this->line = NULL;
  return this;
}

static void buf_undo_release (buf_t *this) {
  self(undo.clear);
  free ($my(undo));
  free ($my(redo));
}

static int __env_check_directory__ (const char *dir, const char *dir_descr,
               int exit_on_error, int exit_on_warning, int warn) {
  int retval = OK;

  if (NULL is dir) {
    Stderr.print_fmt ("Fatal Error: NULL (%s) directory argument\n", dir_descr);
    retval = 1;
    goto theend;
  }

  int fexists = File.exists (dir);

  ifnot (fexists)
    if (NOTOK is Dir.make_parents (dir, S_IRWXU, DirOpts())) {
      Stderr.print_fmt ("Fatal Error: Cannot create %s directory\n", dir);
      retval = errno;
      goto theend;
    }

  if (-1 is access (dir, R_OK)) {
    Stderr.print_fmt ("Fatal Error: %s, (%s) directory, Is Not Readable\n", dir_descr, dir);
    retval = errno;
    goto theend;
  }

  if (-1 is access (dir, W_OK)) {
    Stderr.print_fmt ("Fatal Error: %s, (%s) directory, Is Not Writable\n", dir_descr, dir);
    retval = errno;
    goto theend;
  }

  if (-1 is access (dir, X_OK)) {
    Stderr.print_fmt ("Fatal Error: %s, (%s) directory, Has Not Execution Bits\n", dir_descr, dir);
    retval = errno;
    goto theend;
  }

  struct stat st;
  if (-1 is stat (dir, &st)) {
    Stderr.print_fmt ("Fatal Error: %s, (%s) directory, Can not stat()\n", dir_descr, dir);
    retval = errno;
    goto theend;
  }

  ifnot (S_ISDIR (st.st_mode)) {
    Stderr.print_fmt ("Fatal Error: %s, (%s) directory, Is Not A Directory\n", dir_descr, dir);
    retval = errno;
    goto theend;
  }

  if (warn) {
    char mode_string[12];
    File.mode.stat_to_string (mode_string, st.st_mode);
    ifnot (Cstring.eq (mode_string, "drwx------")) {
      Stderr.print_fmt ("Warning: (%s) directory |%s| permissions is not 0700 or drwx------\n",
         dir_descr, dir);

      if (exit_on_warning) {
        retval = 1;
        goto theend;
      }
    }
  }

theend:
  if (retval isnot OK)
    if (exit_on_error) exit (retval);

  return retval;
}

static void venv_new (ed_T *this) {
  $OurRoots(uid) = (uid_t) atoi (Sys.get.env_value ("UID"));
  $OurRoots(pid) = (pid_t) atoi (Sys.get.env_value ("PID"));

  if ($OurRoots(uid))
    __env_check_directory__ (Sys.get.env_value ("HOME"), "home directory", 1, 0, 0);

#ifndef LIBE_DIR
  Sys.set.env_as (STR_FMT ("%s/.libe", Sys.get.env_value ("HOME"), "E_DIR", 1);
#else
  Sys.set.env_as (LIBE_DIR, "E_DIR", 1);
#endif
 char *e_dir = Sys.get.env_value ("E_DIR");
  if ($OurRoots(uid))
    __env_check_directory__ (e_dir, "libe directory", 1, 0, 0);

#ifndef LIBE_TMPDIR
  Sys.set.env_as (STR_FMT ("%s/tmp", e_dir), "E_TMPDIR", 1);
#else
  Sys.set.env_as (LIBE_TMPDIR, "E_TMPDIR", 1);
#endif
  char *tmp_dir = Sys.get.env_value ("E_TMPDIR");
  if ($OurRoots(uid))
    __env_check_directory__ (tmp_dir, "editor temp directory", 1, 1, 0);

#ifndef LIBE_DATADIR
  Sys.set.env_as (STR_FMT ("%s/data", e_dir), "E_DATADIR", 1);
#else
  Sys.set.env_as (LIBE_DATADIR, "E_DATADIR", 1);
#endif
  char *data_dir = Sys.get.env_value ("E_DATADIR");
  if ($OurRoots(uid))
    __env_check_directory__ (data_dir, "editor data directory", 1, 1, 0);

  Sys.set.env_as (STR_FMT ("%s/e", data_dir), "LA_DIR", 1);
  if ($OurRoots(uid)) {
    char *i_dir = Sys.get.env_value ("LA_DIR");
    __env_check_directory__ (i_dir, "integrated interpreter directory", 1, 1, 0);

    size_t len = bytelen (i_dir) + 8;
    char scripts[len + 1];
    Cstring.cp_fmt (scripts, len + 1, "%s/scripts", i_dir);
    __env_check_directory__ (scripts, "integrated interpreter scripts directory", 1, 1, 0);
  }
}

static void history_release (hist_t **hist) {
  if (NULL is hist) return;

  h_search_t *hs = (*hist)->search;
  if (NULL isnot hs) {
    histitem_t *hitem = hs->head;

    while (hitem isnot NULL) {
      histitem_t *tmp = hitem->next;
      String.release (hitem->data);
      free (hitem);
      hitem = tmp;
    }
  }

  readline_hist_t *hrl = (*hist)->readline;
  readline_hist_item_t *it = hrl->head;
  while (it isnot NULL) {
    readline_hist_item_t *tmp = it->next;
    Readline.release (it->data);
    free (it);
    it = tmp;
  }

  free (hrl);
  free (hs); (*hist)->search = NULL;
  free (*hist); *hist = NULL;
}

static void buf_set_video_first_row (buf_t *this, int idx) {
  if (idx >= this->num_items or idx < 0 or idx is $my(video_first_row_idx))
    return;

  if ($my(video_first_row_idx) < 0) {
    $my(video_first_row) = this->head;
    $my(video_first_row_idx) = 0;
  }

  int num;

  if (idx < $my(video_first_row_idx)) {
    num = $my(video_first_row_idx) - idx;
    loop (num) $my(video_first_row) = $my(video_first_row)->prev;
    $my(video_first_row_idx) -= num;
  } else {
    num = idx - $my(video_first_row_idx);
    loop (num) $my(video_first_row) = $my(video_first_row)->next;
    $my(video_first_row_idx) += num;
   }
}

static row_t *buf_current_prepend (buf_t *this, row_t *row) {
  return DListPrependCurrent (this, row);
}

static row_t *buf_current_append (buf_t *this, row_t *row) {
  return DListAppendCurrent (this, row);
}

static row_t *buf_append_with (buf_t *this, char *bytes) {
  row_t *row = self(row.new_with, bytes);
  int cur_idx = this->cur_idx;
  self(current.set, this->num_items - 1);
  DListAppendCurrent (this, row);
  self(current.set, cur_idx);
  return row;
}

static row_t *buf_current_prepend_with(buf_t *this, const char *bytes) {
  row_t *row = self(row.new_with, bytes);
  return DListPrependCurrent (this, row);
}

static row_t *buf_current_append_with (buf_t *this, const char *bytes) {
  row_t *row = self(row.new_with, bytes);
  return DListAppendCurrent (this, row);
}

static row_t *buf_current_append_with_len (buf_t *this, const char *bytes, size_t len) {
  row_t *row = self(row.new_with_len, bytes, len);
  return DListAppendCurrent (this, row);
}

static row_t *buf_current_replace_with (buf_t *this, const char *bytes) {
  String.replace_with (this->current->data, bytes);
  return this->current;
}

static row_t *__buf_current_delete (buf_t *this, row_t **row) {
  if (this->current is NULL) return NULL;
  *row = this->current;

  if (this->num_items is 1) {
    this->current = NULL;
    this->head = NULL;
    this->tail = NULL;
    this->cur_idx = -1;
    goto theend;
  }

  if (this->cur_idx is 0) {
    this->current = this->current->next;
    this->current->prev = NULL;
    this->head = this->current;
    goto theend;
  }

  if (this->cur_idx + 1 is this->num_items) {
    this->cur_idx--;
    this->current = this->current->prev;
    this->current->next = NULL;
    this->tail = this->current;
    goto theend;
  }

  this->current->prev->next = this->current->next;
  this->current->next->prev = this->current->prev;
  this->current = this->current->next;

theend:
  this->num_items--;
  return this->current;
}

static row_t *buf_current_delete (buf_t *this) {
  row_t *row = NULL;
  __buf_current_delete (this, &row);

  if (row isnot NULL) self(release.row, row);

  return this->current;
}

static row_t *buf_current_pop (buf_t *this) {
  row_t *row = NULL;
  __buf_current_delete (this, &row);
  return row;
}

static row_t *buf_current_pop_next (buf_t *this) {
  if (this->current->next is NULL) return NULL;

  this->current = this->current->next;
  this->cur_idx++;
  int islastidx = this->cur_idx + 1 == this->num_items;
  row_t *row;
  __buf_current_delete (this, &row);

  ifnot (islastidx) {
    this->current = this->current->prev;
    this->cur_idx--;
  }

  return row;
}

static char *buf_get_contents (buf_t *this, int addnl) {
  size_t len = self(get.size) - (addnl ? 0 : this->num_items);
  char *buf = Alloc (len + 1);

  row_t *it = this->head;
  size_t offset = 0;

  while (it) {
    Cstring.byte.cp (buf + offset, it->data->bytes, it->data->num_bytes);
    offset += it->data->num_bytes;
    if (addnl)
      buf[offset++] = '\n';
    it = it->next;
  }

  buf[len] = '\0';

  return buf;
}

static win_t *buf_get_my_parent (buf_t *this) {
  return $my(parent);
}

static ed_t *buf_get_my_root (buf_t *this) {
  return $my(root);
}

static char *buf_get_fname (buf_t *this) {
  return $my(fname);
}

static char *buf_get_ftype_name (buf_t *this) {
  return $my(ftype)->name;
}

static char *buf_get_basename (buf_t *this) {
  return $my(basename);
}

static string_t *buf_get_shared_str (buf_t *this) {
  return $my(shared_str);
}

static int buf_get_flags (buf_t *this) {
  return $my(flags);
}

static int buf_get_current_video_row (buf_t *this) {
  return $my(cur_video_row);
}

static int buf_get_current_video_col (buf_t *this) {
  return $my(cur_video_col);
}

static size_t buf_get_num_lines (buf_t *this) {
  return this->num_items;
}

static int buf_get_prop_tabwidth (buf_t *this) {
  return $my(ftype)->tabwidth;
}

static int buf_isit_special_type (buf_t *this) {
  return ($my(flags) & BUF_IS_SPECIAL);
}

static int buf_set_row_idx (buf_t *this, int idx, int ofs, int col) {
  if (idx < 0) idx = 0;

  int index = idx;
  do {
    idx = DListSetCurrent (this, index);

    if (idx is EINDEX) {
      index--;
      continue;
    }

    break;
  } while (1);

  /* ATTENTION: the following doesn't do what you may think it will do */
  // do {
  //   idx = DListSetCurrent (this, index);
  //   if (idx is EINDEX) {
  //     index--;
  //     continue;
  //   }
  // } while (0);
  /* as the continue statement doesn't really continues the loop */

  $my(video)->row_pos = $my(cur_video_row) = $my(dim)->first_row;
  $my(video_first_row) = this->current;
  $my(video_first_row_idx) = idx;

  for (int i = 0; i < ofs and idx-- > 0; i++) {
    $my(video_first_row_idx)--;
    $my(video_first_row) = $my(video_first_row)->prev;
    $my(video)->row_pos = $my(cur_video_row) =
        $my(cur_video_row) + 1;
  }

  if (col > (int) $mycur(data)->num_bytes or col < 1)
    col = 1;
  $my(video)->col_pos = $my(cur_video_col) = col;
  return this->cur_idx;
}

static int buf_current_set (buf_t *this, int idx) {
  return DListSetCurrent (this, idx);
}

static void buf_set_mode (buf_t *this, const char *mode) {
  Cstring.cp ($my(mode), MAXLEN_MODE, mode, MAXLEN_MODE - 1);
}

static void buf_set_save_on_exit (buf_t *this, int val) {
  if (val)
    $my(flags) |= BUF_SAVE_ON_EXIT;
  else
    $my(flags) &= ~BUF_SAVE_ON_EXIT;
}

static void buf_set_autosave (buf_t *this, long minutes) {
  if (minutes <= 0) {
    $my(autosave) = $my(saved_sec) = 0;
    return;
  }

  if (minutes > (60 * 24)) minutes = (60 * 24);
  $my(autosave) = minutes * 60;
  ifnot ($my(saved_sec))
    $my(saved_sec) = Sys.get.clock_sec (DEFAULT_CLOCK);
}

static void buf_set_on_emptyline (buf_t *this, const char *str) {
  if (NULL is str) return;
  String.replace_with ($my(ftype)->on_emptyline, str);
}

static void buf_set_autochdir (buf_t *this, int val) {
  $my(ftype)->autochdir = (val isnot 0);
}

static void buf_set_backup (buf_t *this, int backup, const char *suffix) {
  if (backup <= 0
      or NULL isnot $my(backupfile)
      or NULL is suffix
      or ($my(flags) & BUF_IS_SPECIAL)
      or Cstring.eq ($my(fname), UNNAMED))
    return;

  size_t len = bytelen (suffix);
  ifnot (len) return;
  if (len > 7) len = 7;
  for (size_t i = 0; i < len; i++) $my(backup_suffix)[i] = suffix[i];
  $my(backup_suffix)[len] = '\0';

  char *dname = Path.dirname ($my(fname));
  size_t dname_len = bytelen (dname);

  size_t baselen = bytelen ($my(basename));
  size_t suffixlen = bytelen ($my(backup_suffix));
  size_t backuplen = 2 + dname_len + baselen + suffixlen;

  $my(backupfile) = Alloc (backuplen + 1);

  size_t i = 0;
  for (; i < dname_len; i++) $my(backupfile)[i] = dname[i];
  free (dname);

  $my(backupfile)[i++] = '/'; $my(backupfile)[i++] = '.';

  for (size_t j = 0; j < baselen; j++, i++) $my(backupfile)[i] = $my(basename)[j];
  for (size_t j = 0; j < suffixlen; i++, j++) $my(backupfile)[i] = $my(backup_suffix)[j];
  $my(backupfile)[backuplen] = '\0';
}

static void buf_set_normal_beg_cb (buf_t *this, BufNormalBeg_cb cb) {
  this->on_normal_beg = cb;
}

static void buf_set_normal_end_cb (buf_t *this, BufNormalEnd_cb cb) {
  this->on_normal_end = cb;
}

static void buf_set_show_statusline (buf_t *this, int val) {
  $my(show_statusline) = val;
}

static void *mem_should_realloc (void *obj, size_t allocated, size_t len) {
  if (len > allocated) return Realloc (obj, len);
  return obj;
}

static void buf_set_as_non_existant (buf_t *this) {
  $my(basename) = $my(fname); $my(extname) = NULL;
  $my(cwd) = Dir.current ();
  $my(flags) &= ~FILE_EXISTS;
}

static void buf_set_as_unnamed (buf_t *this) {
  size_t len = bytelen (UNNAMED);
  /* static size_t len = bytelen (UNNAMED); fails on tcc with:
   * error: initializer element is not constant
   */
  $my(fname) = mem_should_realloc ($my(fname), PATH_MAX + 1, len + 1);
  Cstring.cp ($my(fname), len + 1, UNNAMED, len);
  self(set.as.non_existant);
}

static void buf_set_as_pager (buf_t *this) {
  $my(flags) &= BUF_IS_PAGER;
}

static int buf_set_fname (buf_t *this, const char *filename) {
  int is_null = (NULL is filename);
  int is_unnamed = (is_null ? 0 : Cstring.eq (filename, UNNAMED));
  size_t len = ((is_null or is_unnamed) ? 0 : bytelen (filename));

  if (is_null or 0 is len or is_unnamed) {
    buf_set_as_unnamed (this);
    return OK;
  }

  string_t *fname = $my(shared_str);
  String.replace_with_len (fname, filename, len);

  /* normalize */
  for (size_t i = 0; i < len; i++) {
    if (fname->bytes[i] isnot DIR_SEP) continue;
    i++;
    while (i < len and fname->bytes[i] is DIR_SEP) {
      String.delete_numbytes_at (fname, 1, i);
      len--;
    }
  }
  if (len > 1 and fname->bytes[len-1] is DIR_SEP) {
    String.clear_at (fname, len-1);
    len--;
  }

  if ($my(flags) & BUF_IS_SPECIAL) {
    $my(fname) = mem_should_realloc ($my(fname), PATH_MAX + 1, len + 1);
    Cstring.cp ($my(fname), len + 1, fname->bytes, len);
    self(set.as.non_existant);
    return OK;
    /* this stays as a reference as tcc segfault'ed, when jumping to the label
     * (the old code had fname as char * that it should be freed and there
     * was a clean-up goto)
     * The reason was that the variable (fname) was considered uninitialized,
     * when it wasn't, and it should do here the goto's job
     * free (fname);
     * return retval;
     */
  }

  int fname_exists = File.exists (fname->bytes);
  int is_abs = IS_DIR_ABS (fname->bytes);

  if (fname_exists) {
    ifnot (File.is_reg (fname->bytes)) {
      VED_MSG_ERROR(MSG_FILE_EXISTS_BUT_IS_NOT_A_REGULAR_FILE, fname->bytes);
      buf_set_as_unnamed (this);
      return NOTOK;
    }

    if (File.is_elf (fname->bytes)) {
      VED_MSG_ERROR(MSG_FILE_EXISTS_BUT_IS_AN_OBJECT_FILE, fname->bytes);
      buf_set_as_unnamed (this);
      return NOTOK;
    }

    $my(flags) |= FILE_EXISTS;

    ifnot (is_abs)
      goto concat_with_cwd;
    else {
      $my(fname) = mem_should_realloc ($my(fname), PATH_MAX + 1, len + 1);
      Cstring.cp ($my(fname), len + 1, fname->bytes, len);
    }
  } else {
    $my(flags) &= ~FILE_EXISTS;
    if (is_abs) {
      $my(fname) = mem_should_realloc ($my(fname), PATH_MAX + 1, len + 1);
      Cstring.cp ($my(fname), len + 1, fname->bytes, len);
    } else {
concat_with_cwd:;
      char *cwd = Dir.current ();
      len += bytelen (cwd) + 1;
      char tmp[len + 1]; snprintf (tmp, len + 1, "%s/%s", cwd, fname->bytes);
      $my(fname) = mem_should_realloc ($my(fname), PATH_MAX + 1, len + 1);
      /* $my(fname) = realpath (tmp, NULL); aborts with invalid argument on tcc */
      Path.real (tmp, $my(fname));
      free (cwd);
    }
  }

  buf_t *buf = Ed.get.bufname ($my(root), $my(fname));
  if (buf isnot NULL) {
    VED_MSG_ERROR(MSG_FILE_IS_LOADED_IN_ANOTHER_BUFFER, $my(fname));
    $my(flags) |= BUF_IS_RDONLY;
  }

  if ($my(flags) & FILE_EXISTS) stat ($my(fname), &$my(st));

  $my(basename) = Path.basename ($my(fname));
  $my(extname) = Path.extname ($my(fname));
  $my(cwd) = Path.dirname ($my(fname));

  return OK;
}

static int buf_on_no_length (buf_t *this) {
  buf_current_append_with (this, " ");
  String.clear_at (this->current->data, 0);
  return OK;
}

static ssize_t ed_readline_from_fp (char **line, size_t *len, FILE *fp) {
  ssize_t nread;
  if (-1 is (nread = getline (line, len, fp))) return -1;
  if (nread and ((*line)[nread - 1] is '\n' or (*line)[nread - 1] is '\r')) {
    (*line)[nread - 1] = '\0';
    nread--;
  }

  return nread;
}

static int buf_com_backupfile (buf_t *this) {
  // RECENT WORDS
  if (NULL is $my(backupfile)) return NOTHING_TODO;

  if (File.exists ($my(backupfile))) {
    utf8 chars[] = {'y', 'Y', 'n', 'N'};
    utf8 c =  buf_quest (this, STR_FMT ("backup file: %s exists\noverride? [yYnN]",
        $my(backupfile)), chars, ARRLEN(chars));

      switch (c) {
        case 'n': case 'N': return NOTHING_TODO;
      }
  }

  return buf_write_to_fname (this, $my(backupfile), DONOT_APPEND, 0,
      this->num_items - 1, FORCE, VERBOSE_OFF);
}

static int buf_com_edit_image (buf_t **thisp, readline_t *rl) {
  (void) rl;
  buf_t *this = *thisp;
  if (NULL is $OurRoots(image_file))
    return NOTHING_TODO;

  return Win.edit_fname ($my(parent), thisp, $OurRoots(image_file),
     $myparents(cur_frame), NO_FORCE, DRAW, REOPEN);
}

static int buf_com_save_image (buf_t *this, readline_t *rl) {
  char *fn = NULL;
  string_t *fn_arg = Readline.get.anytype_arg (rl, "as");

  ifnot (NULL is fn_arg)
    fn = fn_arg->bytes;
  else ifnot (NULL is $OurRoots(image_name))
    fn = $OurRoots(image_name);
  else
    fn = Path.basename (self(get.basename));

  return Root.save_image ($OurRoot, fn);
}

static ssize_t buf_read_fname (buf_t *this) {
  if ($my(fname) is NULL or Cstring.eq ($my(fname), UNNAMED)) return NOTOK;

  FILE *fp = fopen ($my(fname), "r");

  if (fp is NULL) {
     if (EACCES is errno) {
      if (File.exists ($my(fname))) {
        $my(flags) |= FILE_EXISTS;
        $my(flags) &= ~(FILE_IS_READABLE|FILE_IS_WRITABLE);
        VED_MSG_ERROR(MSG_FILE_IS_NOT_READABLE, $my(fname));
        return NOTOK;
      } else
       return OK;

    } else {
      MSG_ERRNO(errno);
      return NOTOK;
    }
  }

  fstat (fileno (fp), &$my(st));
  $my(flags) |= (FILE_EXISTS|FILE_IS_READABLE);

  if (OK is access ($my(fname), W_OK))
    $my(flags) |= FILE_IS_WRITABLE;
  else
    $my(flags) &= ~FILE_IS_WRITABLE;

  $my(flags) |= S_ISREG ($my(st).st_mode);

  size_t t_len = 0;

  ifnot ($my(flags) & FILE_IS_REGULAR) {
    t_len = NOTOK;
    goto theend;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t nread;

  while (-1 isnot (nread = ed_readline_from_fp (&line, &len, fp))) {
    buf_current_append_with (this, line);
    t_len += nread;
  }

  if (line isnot NULL) free (line);

  /* actually this might be called only on :e! (and in that case we do not want it)
   * self(backupfile);
   */

theend:
  fclose (fp);
  return t_len;
}

static ssize_t buf_init_fname (buf_t *this, const char *fname) {
  ssize_t retval = buf_set_fname (this, fname);
//  if (NOTOK is retval) return NOTOK;

  if ($my(flags) & FILE_EXISTS) {
    retval = self(read.fname);

    ifnot (this->num_items)
      retval = buf_on_no_length (this);
  } else
    retval = buf_on_no_length (this);

  $my(video_first_row_idx) = 0;
  $my(video_first_row) = this->head;
  $my(video)->row_pos = $my(cur_video_row) = $my(dim)->first_row;
  $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;

  return retval;
}

static void win_adjust_buf_dim (win_t *w) {
  buf_t *this = w->head;

  while (this) {
    $my(video) = $myroots(video);
    $my(dim) = $from(w, frames_dim)[$my(at_frame)];
    $my(statusline_row) = $my(dim)->last_row;
    $my(video_first_row_idx) = this->cur_idx;
    $my(video_first_row) = this->current;
    $my(cur_video_row) = $my(dim)->first_row;
    for (int i = 0; i < 4 and $my(video_first_row_idx) and
        i < $my(dim)->num_rows - 1; i++) {
      $my(video_first_row_idx)--;
      $my(video_first_row) = $my(video_first_row)->prev;
      $my(cur_video_row)++;
    }

    this = this->next;
  }
}

static void win_set_has_dividers (win_t *this, int val) {
  $my(has_dividers) = val;
}

static void win_set_video_dividers (win_t *this) {
  ifnot ($my(has_dividers)) return;
  if ($my(num_frames) - 1 < 1) return;

  int len = $my(dim)->num_cols + TERM_SET_COLOR_FMT_LEN + TERM_COLOR_RESET_LEN;
  char line[len + 1];
  snprintf (line, TERM_SET_COLOR_FMT_LEN + 1, TERM_SET_COLOR_FMT, COLOR_DIVIDER);
  for (int i = 0; i < $my(dim)->num_cols; i++)
    line[i + TERM_SET_COLOR_FMT_LEN] = '_';

  snprintf (line + TERM_SET_COLOR_FMT_LEN + $my(dim)->num_cols, len, "%s",
      TERM_COLOR_RESET);

  line[len+1] = '\0';
   for (int i = 0; i < $my(num_frames) - 1; i++) {
     Video.set.row_with ($my(video), $my(frames_dim)[i]->last_row, line);
  }
}

static int win_add_frame (win_t *this) {
  if ($my(num_frames) is $my(max_frames)) return NOTHING_TODO;

  for (int i = 0; i < $my(num_frames); i++) free ($my(frames_dim)[i]);
  free ($my(frames_dim));

  $my(num_frames)++;
  self(dim_calc);
  self(adjust.buf_dim);
  self(set.video_dividers);
  return DONE;
}

static int win_delete_frame (win_t *this, int idx) {
  if ($my(num_frames) is 1) return NOTHING_TODO;
  if ($my(num_frames) is $my(min_frames)) return NOTHING_TODO;

  for (int i = 0; i < $my(num_frames); i++) free ($my(frames_dim)[i]);
  free ($my(frames_dim));

  $my(num_frames)--;
  self(dim_calc);

  buf_t *it = this->head;
  while (it isnot NULL) {
    if ($from(it, at_frame) > $my(num_frames) - 1)
        $from(it, at_frame) = $my(num_frames) - 1;
    else
      if ($from(it, at_frame) >= idx)
        $from(it, at_frame)--;

    it = it->next;
  }

  if ($my(cur_frame) >= $my(num_frames))
    $my(cur_frame) = $my(num_frames) - 1;

  self(adjust.buf_dim);
  self(set.video_dividers);
   return DONE;
}

static buf_t *win_frame_change (win_t* w, int frame, int draw) {
  if (frame < FIRST_FRAME or frame > WIN_LAST_FRAME(w)) return NULL;
  int idx = 0;
  buf_t *this = w->head;
  while (this) {
    if ($my(at_frame) is frame and $my(flags) & BUF_IS_VISIBLE) {
      Win.set.current_buf ($my(parent), idx, draw);
      WIN_CUR_FRAME(w) = frame;
      return w->current;
    }

    idx++;
    this = this->next;
  }

  return NULL;
}

static int buf_on_normal_beg (buf_t **thisp, utf8 com, int count, int regidx) {
  (void) thisp; (void) com; (void) count; (void) regidx;
  return 0;
}

static int buf_on_normal_end (buf_t **thisp, utf8 com, int count, int regidx) {
  (void) thisp; (void) com; (void) count; (void) regidx;
  return 0;
}

static void buf_release_rows (buf_t *this) {
  row_t *row = this->head;
  while (row) {
    row_t *next = row->next;
    self(release.row, row);
    row = next;
  }
}

static void buf_release (buf_t *this) {
  if (this is NULL) return;

  self(release.rows);

  if ($myprop is NULL) return;

  if ($my(fname) isnot NULL) free ($my(fname));

  free ($my(cwd));

  ifnot (NULL is $my(backupfile))
    free ($my(backupfile));

  String.release ($my(statusline));
  String.release ($my(shared_str));
  String.release ($my(cur_insert));

  self(release.line);
  self(ftype.release);
  self(undo.release);
  self(jumps.release);

  free ($myprop);
  free (this);
}

static size_t buf_get_size (buf_t *this) {
  size_t size = 0;
  row_t *it = this->head;
  while (it) {
    size += it->data->num_bytes + 1;
    it = it->next;
  }

  return size;
}

static void buf_release_info (buf_t *this, bufinfo_t **info) {
  (void) this;
  if (NULL is *info) return;
  free ((*info)->fname);
  free ((*info)->cwd);
  free ((*info)->parents_name);
  free (*info);
  *info = NULL;
}

static bufinfo_t *buf_get_info_as_type (buf_t *this) {
  bufinfo_t *info = Alloc (sizeof (bufinfo_t));
  info->fname = Cstring.dup ($my(fname), bytelen ($my(fname)));
  info->cwd = Cstring.dup ($my(cwd), bytelen ($my(cwd)));
  info->parents_name = Cstring.dup ($myparents(name), bytelen ($myparents(name)));
  info->at_frame = $my(at_frame);
  info->cur_idx = this->cur_idx;
  info->is_writable = (($my(flags) & FILE_IS_WRITABLE) ? 1 : 0);
  info->num_bytes = self(get.size);
  info->num_lines = self(get.num_lines);
  info->autosave = $my(autosave);
  info->save_on_exit = (($my(flags) & BUF_SAVE_ON_EXIT) ? 1 : 0);
  return info;
}

static void win_release_info (win_t *this, wininfo_t **info) {
  (void) this;
  if (NULL is *info) return;
  free ((*info)->name);
  free ((*info)->parents_name);
  free ((*info)->cur_buf);

  for (size_t i = 0; i < (*info)->num_items; i++)
    free ((*info)->buf_names[i]);
  free ((*info)->buf_names);

  free (*info);
  *info = NULL;
}

static wininfo_t *win_get_info_as_type (win_t *this) {
  wininfo_t *info = Alloc (sizeof (wininfo_t));
  info->name = Cstring.dup ($my(name), bytelen ($my(name)));
  info->parents_name = Cstring.dup ($myparents(name), bytelen ($myparents(name)));
  info->num_frames = $my(num_frames);
  info->cur_idx = this->cur_idx;
  info->num_items = this->num_items;
  info->buf_names = Alloc (sizeof (char *) * (info->num_items));
  buf_t *it = this->head;
  int idx = 0;
  while (it) {
    size_t len = bytelen ($from(it, fname));
    if (it is this->current)
      info->cur_buf = Cstring.dup ($from(it, fname), len);

    info->buf_names[idx++] = Cstring.dup ($from(it, fname), len);

    it = it->next;
  }

  return info;
}

static void ed_release_info (ed_t *this, edinfo_t **info) {
  (void) this;
  if (NULL is *info) return;
  free ((*info)->name);
  free ((*info)->cur_win);
  for (size_t i = 0; i < (*info)->num_items; i++)
    free ((*info)->win_names[i]);
  free ((*info)->win_names);

  for (size_t i = 0; i < (*info)->num_special_win; i++)
    free ((*info)->special_win_names[i]);
  free ((*info)->special_win_names);

  free (*info);
  *info = NULL;
}

static edinfo_t *ed_get_info_as_type (ed_t *this) {
  edinfo_t *info = Alloc (sizeof (edinfo_t));
  info->num_special_win = self(get.num_special_win);
  info->name = Cstring.dup ($my(name), bytelen ($my(name)));
  info->num_items = this->num_items - info->num_special_win;
  info->win_names = Alloc (sizeof (char *) * (info->num_items));
  info->special_win_names = Alloc (sizeof (char *) * (info->num_special_win));
  win_t *it = this->head;
  int idx = 0;
  int sp_idx = 0;
  while (it) {
    size_t len = bytelen ($from(it, name));
    if (it is this->current)
      info->cur_win = Cstring.dup ($from(it, name), len);

    if ($from(it, type) is VED_WIN_NORMAL_TYPE)
      info->win_names[idx++] = Cstring.dup ($from(it, name), len);
    else
      info->special_win_names[sp_idx++] = Cstring.dup ($from(it, name), len);

    it = it->next;
  }

  info->cur_idx = this->cur_idx;
  return info;
}

static void E_release_info (E_T *this, Einfo_t **info) {
  (void) this;
  if (NULL is *info) return;
  free ((*info)->image_name);
  free ((*info)->image_file);
  free (*info);
  *info = NULL;
}

static Einfo_t *E_get_info_as_type (E_T *this) {
  Einfo_t *info = Alloc (sizeof (Einfo_t));
  ifnot (NULL is $my(image_name))
    info->image_name = Cstring.dup ($my(image_name), bytelen ($my(image_name)));
  else
    info->image_name = Cstring.dup ("", 0);

  ifnot (NULL is $my(image_file))
    info->image_file = Cstring.dup ($my(image_file), bytelen ($my(image_file)));
  else
    info->image_file = Cstring.dup ("", 0);

  info->num_items = $my(num_items);
  info->cur_idx = $my(cur_idx);
  return info;
}

static buf_t *win_buf_init (win_t *w, int at_frame, int flags) {
  buf_t *this = Alloc (sizeof (buf_t));
  $myprop = Alloc (sizeof (buf_prop));

  $my(parent)  = w;
  $my(Me)          = $myparents(__Buf__);
  $my(root)        = $myparents(parent);
  $my(__Ed__)      = $myparents(__Ed__);
  $my(__Win__)     = $myparents(Me);
  $my(__LA__)      = $myparents(__LA__);
  $my(__E__)       = $myparents(__E__);
  $my(__Msg__)     = $myparents(__Msg__);
  $my(__EError__)  = $myparents(__EError__);

  $my(term_ptr) = $myroots(term);
  $my(msg_row_ptr) = &$myroots(msg_row);
  $my(prompt_row_ptr) = &$myroots(prompt_row);
  $my(history) = $myroots(history);
  $my(last_insert) = $myroots(last_insert);
  $my(regs) = &$myroots(regs)[0];
  $my(video) = $myroots(video);

  self(undo.init);
  self(jumps.init);

  $my(shared_str) = String.new (128);
  $my(statusline) = String.new (64);
  $my(cur_insert) = String.new (128);

  $my(line) = Ustring.new ();

  $my(flags) = flags;
  $my(flags) &= ~BUF_IS_MODIFIED;
  $my(flags) &= ~BUF_IS_VISIBLE;

  $my(enable_writing) = 1;

  self(set.mode, NORMAL_MODE);

  $my(fname) = Alloc (PATH_MAX + 1);

  for (int i = 0; i < NUM_MARKS; i++)
    $my(marks)[i] = (mark_t) {.mark = MARKS[i], .video_first_row = NULL};

  $my(at_frame) = at_frame;
  $my(dim) = $myparents(frames_dim)[$my(at_frame)];
  $my(statusline_row) = $my(dim)->last_row;
  $my(show_statusline) = 1;

  $my(autosave) = 0;
  $my(backupfile) = NULL;
  $my(backup_suffix)[0] = '~'; $my(backup_suffix)[1] = '\0';

  $my(lw_vis_prev)[0].fidx = -1;
  $my(lw_vis_prev)[0].lidx = -1;

  this->on_normal_beg = buf_on_normal_beg;
  this->on_normal_end = buf_on_normal_end;

  return this;
}

static buf_t *win_buf_new (win_t *win, buf_opts opts) {
  buf_t *this = win_buf_init (win, opts.at_frame, opts.flags);

  self(init_fname, opts.fname);
  self(set.ftype, opts.ftype);
  self(set.row.idx, 0, NO_OFFSET, 1);
  self(set.autosave, opts.autosave);
  self(set.save_on_exit, opts.save_on_exit);
  self(set.backup, opts.backupfile,
    (NULL is opts.backup_suffix ? BACKUP_SUFFIX : opts.backup_suffix));
  self(backupfile);

  buf_normal_goto_linenr (this, opts.at_linenr, DONOT_DRAW);
  buf_normal_right (this, opts.at_column - 1, DONOT_DRAW);

  return this;
}

static int win_isit_special_type (win_t *this) {
  return ($my(type) is VED_WIN_SPECIAL_TYPE);
}

static void win_set_min_rows (win_t *this, int rows) {
  $my(min_rows) = rows;
}

static void win_set_num_frames (win_t *this, int num_frames) {
  if (num_frames > $my(max_frames)) return;
  $my(num_frames) = num_frames;
}

static void win_set_previous_idx (win_t *this, int idx) {
  if (idx < 0 or idx > this->num_items - 1) return;
  this->prev_idx = idx;
}

static buf_t *win_set_current_buf (win_t *w, int idx, int draw) {
  buf_t *that = w->current;
  int cur_idx = w->cur_idx;
  int cur_frame = $from(w, cur_frame);
  buf_t *this = NULL;

  if (idx is cur_idx) {
    this = that;
    goto change;
  }

  int lidx = DListSetCurrent (w, idx);

  if (lidx is EINDEX) return NULL;

  if (cur_idx isnot lidx) w->prev_idx = cur_idx;

  this = w->current;

  if (cur_frame is $my(at_frame))
    $from(that, flags) &= ~BUF_IS_VISIBLE;
  else
    $from(w, cur_frame) = $my(at_frame);

change:
  $my(flags) |= BUF_IS_VISIBLE;
  $my(video)->row_pos = $my(cur_video_row);
  $my(video)->col_pos = $my(cur_video_col);

  if ($my(ftype)->autochdir)
    if (-1 is chdir ($my(cwd)))
      MSG_ERRNO(errno);

  if ($my(flags) & FILE_EXISTS) {
    struct stat st;
    if (-1 is stat ($my(fname), &st)) {
      VED_MSG_ERROR(MSG_FILE_REMOVED_FROM_FILESYSTEM, $my(fname));
    } else {
#if defined(__MACH__)
      if ($my(st).st_mtimespec.tv_sec isnot st.st_mtimespec.tv_sec)
#else
      if ($my(st).st_mtim.tv_sec isnot st.st_mtim.tv_sec)
#endif
        VED_MSG_ERROR(MSG_FILE_HAS_BEEN_MODIFIED, $my(fname));
    }
  }

  if (draw) self(draw);

  return this;
}

static buf_t *win_get_current_buf (win_t *w) {
  return w->current;
}

static int win_get_current_buf_idx (win_t *w) {
  return w->cur_idx;
}

static buf_t *win_get_buf_by_idx (win_t *w, int idx) {
  if (w is NULL) return NULL;

  if (0 > idx) idx += w->num_items;

  if (idx < 0 || idx >= w->num_items) return NULL;

  buf_t *this = w->head;
  loop (idx) this = this->next;

  return this;
}

static buf_t *win_get_buf_head (win_t *this) {
  return this->head;
}

static buf_t *win_get_buf_next (win_t *w, buf_t *this) {
  (void) w;
  return this->next;
}

static buf_t *win_get_buf_by_name (win_t *w, const char *fname, int *idx) {
  if (w is NULL or fname is NULL) return NULL;
  *idx = 0;
  buf_t *this = w->head;
  while (this) {
    if ($my(fname) isnot NULL)
      if (Cstring.eq ($my(fname), fname)) return this;

    this = this->next;
    *idx += 1;
  }

  return NULL;
}

static int win_get_num_buf (win_t *w) {
  return w->num_items;
}

static int win_get_num_cols (win_t *this) {
  return $my(dim)->num_cols;
}

static int win_append_buf (win_t *this, buf_t *buf) {
  DListAppendCurrent (this, buf);
  return this->cur_idx;
}

static int win_pop_current_buf (win_t *this) {
  ifnot (this->num_items) return NOTOK;
  if ($from(this->current, flags) & BUF_IS_SPECIAL) return NOTOK;
  int prev_idx = this->prev_idx;
  buf_t *tmp = DListPopCurrent (this, buf_t);
  buf_release (tmp);
  if (this->num_items is 1)
    this->prev_idx = this->cur_idx;
  else
    if (prev_idx >= this->cur_idx) this->prev_idx--;
  return this->cur_idx;
}

static void win_draw (win_t *w) {
  buf_t *this = w->head;

  char line[$from(w, dim->num_cols) + 1];
  for (int i = 0; i < $from(w, dim->num_cols); i++) line[i] = ' ';
  line[$from(w, dim->num_cols)] = '\0';

//  if (w->num_items is 0) return;

  for (int i = $from(w, dim->first_row) - 1; i < $from(w, dim->last_row); i++)
    Video.set.row_with ($from(w, video), i, line);

  Win.set.video_dividers ($my(parent));

  while (this) {
    if ($my(flags) & BUF_IS_VISIBLE) {
      if (this is w->current)
        Ed.set.topline ($my(root), this);

      self(to.video);
    }
    this = this->next;
  }

  this = w->head;

  Video.draw.all ($my(video));
}

static void ed_draw_current_win (ed_t *this) {
  win_draw (this->current);
}

static void win_release (win_t *this) {
  buf_t *buf = this->head;
  buf_t *next;

  while (buf) {
    next = buf->next;
    buf_release (buf);
    buf = next;
  }

  if ($myprop isnot NULL) {
    free ($my(name));

    if ($my(dim) isnot NULL)
      free ($my(dim));

    if ($my(frames_dim) isnot NULL) {
      for (int i = 0; i < $my(num_frames); i++) free ($my(frames_dim)[i]);

      free ($my(frames_dim));
    }

    free ($myprop);
  }

 free (this);
}

static dim_t **win_dim_calc_cb (win_t *this, int num_rows, int num_frames,
                                            int min_rows, int has_dividers) {
  return ed_dim_calc ($my(parent), num_rows, num_frames, min_rows, has_dividers);
}

static dim_t **win_dim_calc (win_t *this) {
  $my(frames_dim) = this->dim_calc (this, $my(dim)->num_rows, $my(num_frames),
     $my(min_rows), $my(has_dividers));
  return $my(frames_dim);
}

static char *ed_name_gen (int *name_gen, const char *prefix, size_t prelen) {
  size_t num = (*name_gen / 26) + prelen;
  char *name = Alloc (num * sizeof (char *) + 1);
  size_t i = 0;
  for (; i < prelen; i++) name[i] = prefix[i];
  for (; i < num; i++) name[i] = 'a' + ((*name_gen)++ % 26);
  name[num] = '\0';
  return name;
}

static win_t *ed_win_init (ed_t *ed, const char *name, WinDimCalc_cb dim_calc_cb) {
  win_t *this = Alloc (sizeof (win_t));
  $myprop = Alloc (sizeof (win_prop));

  if (NULL is name) {
    $my(name) = ed_name_gen (&ed->name_gen, "win:", 4);
  } else
    $my(name) = Cstring.dup (name, bytelen (name));

  $my(parent) = ed;

  $my(Me)          = &$myparents(Me)->__Win__;
  $my(__Ed__)      = $myparents(Me);
  $my(__Buf__)     = &$myparents(Me)->__Buf__;
  $my(__LA__)      = $myparents(__LA__);
  $my(__E__)       = $myparents(__E__);
  $my(__Msg__)     = $myparents(__Msg__);
  $my(__EError__)  = $myparents(__EError__);

  $my(video) = $myparents(video);
  $my(min_rows) = 1;
  $my(has_topline) = $myparents(has_topline);
  $my(has_msgline) = $myparents(has_msgline);
  $my(has_promptline) = $myparents(has_promptline);
  $my(type) = VED_WIN_NORMAL_TYPE;
  $my(has_dividers) = 1;

  $my(dim) = ed_dim_new (ed,
      $myparents(dim)->first_row,
      $myparents(dim)->num_rows,
      $myparents(dim)->first_col,
      $myparents(dim)->num_cols);

  $my(cur_frame) = 0;
  $my(min_frames) = 1;
  $my(num_frames) = 1;
  $my(max_frames) = MAX_FRAMES;

  this->dim_calc = (NULL isnot dim_calc_cb ? dim_calc_cb : win_dim_calc_cb);
  return this;
}

static win_t *ed_win_new (ed_t *ed, const char *name, int num_frames) {
  win_t *this = ed_win_init (ed, name, win_dim_calc_cb);
  $my(num_frames) = num_frames;
  self(dim_calc);
  self(set.video_dividers);
  return this;
}

static win_t *ed_win_new_special (ed_t *ed, const char *name, int num_frames) {
  win_t *this = ed_win_new (ed, name, num_frames);
  $my(type) = VED_WIN_SPECIAL_TYPE;
  return this;
}

static void ed_win_readjust_size (ed_t *ed, win_t *this) {
  ifnot (NULL is $my(dim)) {
    free ($my(dim));
    $my(dim) = NULL;
  }

  $my(dim) = ed_dim_new (ed,
    $myparents(dim)->first_row, $myparents(dim)->num_rows,
    $myparents(dim)->first_col, $myparents(dim)->num_cols);

  ifnot (NULL is $my(frames_dim)) {
    for (int i = 0; i < $my(num_frames); i++) free ($my(frames_dim)[i]);
    free ($my(frames_dim));
    $my(frames_dim) = NULL;
  }

  $my(video) = $myparents(video);

  self(dim_calc);
  self(adjust.buf_dim);
  self(set.video_dividers);

  $my(video)->row_pos = $from(this->current, cur_video_row);
  $my(video)->col_pos = $from(this->current, cur_video_col);
  if (this is $my(parent)->current) {
    Video.set.row_with ($my(video), $myparents(prompt_row), " ");
    Video.set.row_with ($my(video), $myparents(msg_row), " ");
    self(draw);
  }
}

static void ed_check_msg_status (ed_t *this) {
  if ($my(msg_send) is 1)
    $my(msg_send)++;
  else if (2 is $my(msg_send)) {
    Video.set.row_with ($my(video), $my(msg_row) - 1, " ");
    Video.draw.row_at ($my(video), $my(msg_row));
    $my(msg_send) = 0;
  }
}

static buf_t *ed_buf_get (ed_t *this, const char *wname, const char *bname) {
  int idx;
  win_t *w = self(get.win_by_name, wname, &idx);
  ifnot (w) return NULL;
  return Win.get.buf_by_name (w, bname, &idx);
}

static void ed_tty_screen (ed_t *this) {
  term_t *term = $my(term);

  Term.reset (term);
  Term.set_mode (term, 'r');
  Input.getkey (STDIN_FILENO);
  Term.set (term);

  win_t *w = self(get.current_win);
  int idx = Win.get.current_buf_idx (w);
  Win.set.current_buf (w, idx, DONOT_DRAW);
  Win.draw (w);
}

static int ed_buf_change (ed_t *this, buf_t **thisp, const char *wname, const char *bname) {
  self(win.change, thisp, NO_COMMAND, wname, NO_OPTION, NO_FORCE);
  return buf_change_bufname (thisp, bname);
}

static buf_t *ed_special_buf (ed_t *this, const char *wname, const char *bname,
    int num_frames, int at_frame) {
  int idx;
  win_t *w = self(get.win_by_name, wname, &idx);
  if (NULL is w) {
    w = self(win.new_special, wname, num_frames);
    self(append.win, w);
  }

  buf_t *buf = Win.get.buf_by_name (w, bname, &idx);
  if (NULL is buf) {
      buf = Win.buf.new (w, BufOpts (
          .fname = bname,
          .at_frame = at_frame,
          .flags = BUF_IS_PAGER|BUF_IS_RDONLY|BUF_IS_SPECIAL));

    Win.append_buf (w, buf);
  }

  return buf;
}

static buf_t *ed_msg_buf (ed_t *this) {
  return ed_special_buf (this, VED_MSG_WIN, VED_MSG_BUF, 1, 0);
}

static buf_t *ed_diff_buf (ed_t *this) {
  return ed_special_buf (this, VED_DIFF_WIN, VED_DIFF_BUF, 1, 0);
}

static buf_t *ed_search_buf (ed_t *ed) {
  buf_t *this = ed_special_buf (ed, VED_SEARCH_WIN, VED_SEARCH_BUF, 2, 1);
  this->on_normal_beg = buf_grep_on_normal;
  $my(is_sticked) = 1;
  $myparents(cur_frame) = 1;
  Win.set.current_buf ($my(parent), 0, DONOT_DRAW);
  return this;
}

static buf_t *ed_get_scratch_buf (ed_t *this) {
  return ed_special_buf (this, VED_SCRATCH_WIN, VED_SCRATCH_BUF, 1, 0);
}

static void ed_append_toscratch (ed_t *this, int clear_first, const char *bytes) {
  buf_t *buf = self(buf.get, VED_SCRATCH_WIN, VED_SCRATCH_BUF);
  if (NULL is buf)
    buf = self(get.scratch_buf);

  if (clear_first) Buf.clear (buf);

  Vstring_t *lines = cstring_chop (bytes, '\n', NULL, NO_CB_FN, NULL);
  vstring_t *it = lines->head;

  int ifclear = 0;
  while (it) {
    if (clear_first and 0 is ifclear++)
      Buf.current.replace_with (buf, it->data->bytes);
    else
      Buf.append_with (buf, it->data->bytes);
    it = it->next;
  }

  Vstring.release (lines);
}

static void ed_append_toscratch_fmt (ed_t *this, int clear_first, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  ed_append_toscratch (this, clear_first, bytes);
}

static int ed_scratch (ed_t *this, buf_t **bufp, int at_eof) {
  ifnot (Cstring.eq ($from((*bufp), fname), VED_SCRATCH_BUF)) {
    self(buf.change, bufp, VED_SCRATCH_WIN, VED_SCRATCH_BUF);
    ifnot (Cstring.eq ($from((*bufp), fname), VED_SCRATCH_BUF)) {
      self(get.scratch_buf);
      self(buf.change, bufp, VED_SCRATCH_WIN, VED_SCRATCH_BUF);
    }
  }

  if (at_eof) Buf.normal.eof (*bufp, DRAW);
  else Buf.draw (*bufp);

  return DONE;
}

static int ed_messages (ed_t *this, buf_t **bufp, int at_eof) {
  self(buf.change, bufp, VED_MSG_WIN, VED_MSG_BUF);
  ifnot (Cstring.eq ($from((*bufp), fname), VED_MSG_BUF))
    return NOTHING_TODO;

  if (at_eof) Buf.normal.eof (*bufp, DRAW);
  else Buf.draw (*bufp);

  return DONE;
}

static int ed_append_message_cb (Vstring_t *str, char *tok, void *obj) {
  (void) str;
  buf_t *this = (buf_t *) obj;
  self(append_with, tok);
  return OK;
}

static void ed_append_message (ed_t *this, const char *msg) {
  buf_t *buf = self(buf.get, VED_MSG_WIN, VED_MSG_BUF);
  ifnot (buf) return;
  Vstring_t unused;
  cstring_chop (msg, '\n', &unused, ed_append_message_cb, (void *) buf);
}

static void ed_append_message_fmt (ed_t *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  ed_append_message (this, bytes);
}

static char *ed_msg_fmt (ed_t *this, int msgid, ...) {
  char efmt[MAXLEN_ERR_MSG]; efmt[0] = '%'; efmt[1] = 's'; efmt[2] = '\0';
  char pat[16]; snprintf (pat, 16, "%d:", msgid);
  char *sp = strstr (ED_MSGS_FMT, pat);
  if (sp isnot NULL) {
    int i;
    for (i = 0; i < (int) bytelen (pat); i++) sp++;
    for (i = 0; *sp and *sp isnot '.'; sp++) efmt[i++] = *sp;
    efmt[i] = '\0';
  }

  size_t len = ({
    int size = 0;
    va_list ap; va_start(ap, msgid);
    size = vsnprintf (NULL, size, efmt, ap);
    va_end(ap);
    size;
  });

  char bytes[len + 1];
  va_list ap;
  va_start(ap, msgid);
  vsnprintf (bytes, sizeof (bytes), efmt, ap);
  va_end(ap);

  String.replace_with_len ($my(ed_str), bytes, len);
  return $my(ed_str)->bytes;
}

static int ed_fmt_string_with_numchars (ed_t *this, string_t *dest,
   int clear_dest, const char *src, size_t src_len, int tabwidth, int num_cols) {

  int numchars = 0;
  if (clear_dest) String.clear (dest);

  if (src_len <= 0) src_len = bytelen (src);

  ifnot (src_len) return numchars;

  ustring_t *it = Ustring.encode ($my(uline), src, src_len, CLEAR, tabwidth, 0);

  ustring_t *tmp = it;

  while (it and numchars < num_cols) {
    if (Cstring.eq (it->buf, "\t")) {
      for (int i = 0; i < it->width; i++)
        String.append_byte (dest, ' ');
    } else
      String.append_with (dest, it->buf);

    numchars += it->width;
    tmp = it;
    it = it->next;
  }

  it = tmp;
  while (it and numchars > num_cols) {
    if (Cstring.eq (it->buf, "\t")) {
      for (int i = 0; i < it->width; i++) {
        if (numchars-- is num_cols)
          return numchars;
        String.clear_at (dest, dest->num_bytes - 1);
      }

      goto thenext_it;
    }

    String.clear_at (dest, (dest->num_bytes - it->len));
    numchars -= it->width;

thenext_it:
    it = it->prev;
  }

  return numchars;
}

static void ed_msg_write (ed_t *this, const char *msg) {
  self(append.message, msg);
}

static void ed_msg_write_fmt (ed_t *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  ed_msg_write (this, bytes);
}

static void ed_msg_set (ed_t *this, int color, int msg_flags, const char *msg,
                                                        size_t maybe_len) {
  int wt_msg = (msg_flags & MSG_SET_TO_MSG_BUF or
               (msg_flags & MSG_SET_TO_MSG_LINE) is UNSET);
  if (wt_msg)
    self(append.message, msg);

  int clear = (msg_flags & MSG_SET_RESET or
              (msg_flags & MSG_SET_APPEND) is UNSET);
  if (clear)
    $my(msg_numchars) = 0;

  $my(msg_numchars) += ed_fmt_string_with_numchars (this, $my(msgline),
      (clear ? CLEAR : DONOT_CLEAR), msg, maybe_len,
      $my(msg_tabwidth), $my(dim)->num_cols - $my(msg_numchars));

  int close = (msg_flags & MSG_SET_CLOSE or
              (msg_flags & MSG_SET_OPEN) is UNSET);
  if (close)
    for (int i = $my(msg_numchars); i < $my(dim)->num_cols; i++)
      String.append_byte ($my(msgline), ' ');

  int set_color = (msg_flags & MSG_SET_COLOR);
  if (set_color) {
    String.prepend_with ($my(msgline), TERM_MAKE_COLOR(color));
    String.append_with ($my(msgline), TERM_COLOR_RESET);
  }

  Video.set.row_with ($my(video), $my(msg_row) - 1, $my(msgline)->bytes);

  int draw = (msg_flags & MSG_SET_DRAW);
  if (draw)
    Video.draw.row_at ($my(video), $my(msg_row));

  $my(msg_send) = 1;
}

static void ed_msg_set_fmt (ed_t *this, int color, int msg_flags, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  ed_msg_set (this, color, msg_flags, bytes, len);
}

static void ed_msg_line (ed_t *this, int color, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  int flags = MSG_SET_DRAW|MSG_SET_COLOR|MSG_SET_TO_MSG_LINE;
  ed_msg_set (this, color, flags, bytes, len);
}

static void ed_msg_error (ed_t *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  ed_msg_set (this, COLOR_ERROR, MSG_SET_DRAW|MSG_SET_COLOR, bytes, len);
}

static void ed_msg_send (ed_t *this, int color, const char *msg) {
  ed_msg_set (this, color, MSG_SET_DRAW|MSG_SET_COLOR, msg, -1);
}

static void ed_msg_send_fmt (ed_t *this, int color, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  ed_msg_set (this, color, MSG_SET_DRAW|MSG_SET_COLOR, bytes, len);
}

static char *ed_error_string (ed_t *this, int err) {
  int exists = Error.exists (err);
  if (exists) {
    String.replace_with ($my(ed_str), Error.errno_string (err));
    return $my(ed_str)->bytes;
  }

  char ebuf[MAXLEN_ERR_MSG];
  ebuf[0] = '\0';
  char epat[16];
  snprintf (epat, 16, "%d:", err);

  char *sp = strstr (ED_ERRORS, epat);

  if (sp is NULL) return NULL;
  int i;
  for (i = 0; *sp is ' ' or i <= (int) bytelen (epat); i++) sp++;
  for (i = 0; *sp and *sp isnot ' '; sp++) ebuf[i++] = *sp;
  for (; *sp and *sp is ' '; sp++);
  ebuf[i++] = ':'; ebuf[i++] = ' ';
  for (; *sp and *sp isnot ','; sp++) ebuf[i++] = *sp;
  ebuf[i] = '\0';

  String.replace_with_len ($my(ed_str), ebuf, i);
  return $my(ed_str)->bytes;
}

static void ed_set_topline (ed_t *ed UNUSED, buf_t *this) {
  time_t tim = time (NULL);
  struct tm *tm = localtime (&tim);

  String.replace_with_fmt ($myroots(topline), TERM_SET_COLOR_FMT,
      $OurRoots(uid) is 0  ? COLOR_SU : COLOR_TOPLINE);
  String.append_with_fmt ($myroots(topline), "[%s] [pid %d]",
    $my(mode), $OurRoots(pid));

  char tmnow[32];
  size_t len = strftime (tmnow, sizeof (tmnow), "[%a %d %H:%M:%S]", tm);
  int pad = $my(dim->num_cols) - ($myroots(topline)->num_bytes - 5) - len;
  if (pad > 0)
    loop (pad) String.append_byte ($myroots(topline), ' ');
  String.append_with ($myroots(topline), tmnow);
  String.clear_at ($myroots(topline), $my(dim)->num_cols + 5);
  String.append_with_fmt ($myroots(topline), TERM_COLOR_RESET);
  Video.set.row_with ($my(video), 0, $myroots(topline)->bytes);
}

static void buf_set_draw_topline (buf_t *this) {
  Ed.set.topline ($my(root), this);
  Video.draw.row_at ($my(video), 1);
}

static void buf_set_statusline (buf_t *this) {
  if ($my(dim->num_rows) is 1 or (
      $my(show_statusline) is 0 and 0 is IS_MODE (INSERT_MODE))) {
    String.replace_with ($my(statusline), " ");
    Video.set.row_with ($my(video), $my(statusline_row) - 1, $my(statusline)->bytes);
    return;
  }

  int cur_code = 0;
  if ($mycur(cur_col_idx) < (int) $mycur(data)->num_bytes) {
    cur_code = CUR_UTF8_CODE;
  }

  String.replace_with_fmt ($my(statusline),
    TERM_SET_COLOR_FMT "%s [%s] (line: %d/%d idx: %d len: %d chr: %d) %s",
    COLOR_STATUSLINE, $my(basename), $my(ftype)->name, this->cur_idx + 1,
    this->num_items, $mycur(cur_col_idx), $mycur(data)->num_bytes, cur_code,
    ($my(flags) & FILE_IS_WRITABLE) ? "" : "[RDONLY]");

  String.clear_at ($my(statusline), $my(dim)->num_cols + TERM_SET_COLOR_FMT_LEN);
  String.append_with_fmt ($my(statusline), "%s", TERM_COLOR_RESET);
  Video.set.row_with ($my(video), $my(statusline_row) - 1, $my(statusline)->bytes);
}

static void buf_set_draw_statusline (buf_t *this) {
  buf_set_statusline (this);
  Video.draw.row_at ($my(video), $my(statusline_row));
}

static string_t *get_current_number (buf_t *this, int *fidx) {
  if ($mycur(data)->num_bytes is 0) return NULL;

  string_t *nb = String.new (8);
  int type = 'd';
  int issign = 0;

  int orig_idx = $mycur(cur_col_idx);
  int idx = orig_idx;
  uchar c;

  while (idx >= 0) {
    c = $mycur(data)->bytes[idx--];

    if (type is 'x') {
      if ('0' isnot c) goto theerror;
      String.prepend_byte (nb, c);
      *fidx = idx + 1;
      break;
    }

    if (IS_HEX (c)) {  // this matchs IS_DIGIT
      String.prepend_byte (nb, c);
      continue;
    }

    if (c is 'x') {
      if (type is 'x') goto theerror;
      type = 'x';
      String.prepend_byte (nb, c);
      continue;
    }

    if (c is '-') {
      issign = 1; idx--;
    }

    *fidx = idx + 2;
    break;
  }

  int cur_idx = nb->num_bytes - 1;

  idx = orig_idx + 1;
  while (idx < (int) $mycur(data)->num_bytes) {
    c = $mycur(data)->bytes[idx++];

    if (IS_HEX (c)) { // this matchs IS_DIGIT
      String.append_byte (nb, c);
      continue;
    }

    if (c is 'x') {
      if (type is 'x') goto theerror;
      if (nb->num_bytes isnot 1 and nb->bytes[0] isnot '0') goto theerror;
      String.append_byte (nb, c);
      type = 'x';
      continue;
    }

    break;
  }

  if (nb->num_bytes is 0) goto theerror;

  if (type is 'x') {
    if (nb->num_bytes < 3) goto theerror;
    goto theend;
  }

  idx = cur_idx + 1;

  while (idx < (int) nb->num_bytes and IS_DIGIT (nb->bytes[idx])) idx++;
  if (idx < (int) nb->num_bytes) String.clear_at (nb, idx);

  int num = 0;
  idx = cur_idx;
  while (idx >= 0) {
    if (IS_HEX (nb->bytes[idx])) {
      num = idx + 1;
      *fidx = orig_idx - (cur_idx - idx) + 1;
      break;
    }
    idx--;
  }

  if (num) String.delete_numbytes_at (nb, num, 0);
  ifnot (nb->num_bytes) goto theerror;

  idx = 0;
  if (nb->bytes[idx++] is '0') {
    while (idx <= (int) nb->num_bytes - 1) {
      if (nb->bytes[idx] > '8') {
        String.clear_at (nb, idx);
        break;
      }
      idx++;
    }
    if (nb->num_bytes < 2) goto theerror;

    type = 'o';
    goto theend;
  }

  goto theend;

theerror:
  String.release (nb);
  return NULL;

theend:
  if (issign) String.prepend_byte (nb, '-');
  String.prepend_byte (nb, type);
  return nb;
}

static char *buf_get_current_word (buf_t *this,
  char *word, const char *NotWord, int NotWordlen, int *fidx, int *lidx) {
  return Cstring.extract_word_at ($mycur(data)->bytes, $mycur(data)->num_bytes,
    word, MAXLEN_WORD, NotWord, NotWordlen, $mycur (cur_col_idx), fidx, lidx);
}

#define SEARCH_UPDATE_ROW(idx)                    \
({                                                \
  int idx__ = (idx);                              \
  if (sch->dir is -1) {                           \
    if (idx__ is 0) {                             \
      idx__ = this->num_items - 1;                \
      sch->row = this->tail;                      \
    } else {                                      \
      idx__--;                                    \
      sch->row = sch->row->prev;                  \
    }                                             \
  } else {                                        \
    if (idx__ is this->num_items - 1) {           \
      idx__ = 0;                                  \
      sch->row = this->head;                      \
    } else {                                      \
      idx__++;                                    \
      sch->row = sch->row->next;                  \
    }                                             \
  }                                               \
                                                  \
  idx__;                                          \
})

#define SEARCH_FREE                               \
({                                                \
  String.release (sch->pat);                      \
  ListStackClear (sch, search_t);                 \
  ifnot (NULL is sch->prefix) free (sch->prefix); \
  ifnot (NULL is sch->match) free (sch->match);   \
  free (sch);                                     \
})

#define SEARCH_PUSH(idx_, row_)                   \
  search_t *s_ = Alloc (sizeof (search_t));       \
  s_->idx = (idx_);                               \
  s_->row  = (row_);                              \
  ListStackPush (sch, s_)

static int __buf_search__ (buf_t *this, Search_t *sch) {
  int retval = NOTOK;
  int idx = sch->cur_idx;
  int flags = 0;
  re_t *re = Re.new (sch->pat->bytes, flags, RE_MAX_NUM_CAPTURES, Re.compile);

  sch->found = 0;
  SEARCH_PUSH (sch->cur_idx, sch->row);

  do {
    int ret = Re.exec (re, sch->row->data->bytes, sch->row->data->num_bytes);
    if (ret is RE_UNBALANCED_BRACKETS_ERROR) {
      MSG_ERRNO (RE_UNBALANCED_BRACKETS_ERROR);
      break;
    }

    if (0 <= ret) {
      sch->idx = idx;
      sch->found = 1;

      sch->match = Alloc ((size_t) (re->match_len) + 1);
      Cstring.cp (sch->match, re->match_len + 1, re->match_ptr, re->match_len);

      sch->col = re->match_idx;
      sch->prefix = Alloc ((size_t) sch->col + 1);
      Cstring.cp (sch->prefix, sch->col + 1, sch->row->data->bytes, sch->col);

      sch->end = sch->row->data->bytes + re->retval;
      retval = OK;
      goto theend;
    }

    idx = SEARCH_UPDATE_ROW (idx);

  } while (idx isnot sch->cur_idx);

theend:
  Re.release (re);
  return retval;
}

static int readline_search_at_beg (readline_t **rl) {
  switch ((*rl)->c) {
    case ESCAPE_KEY:
    case '\r':
    case CTRL('n'):
    case CTRL('p'):
    case ARROW_UP_KEY:
    case ARROW_DOWN_KEY:

    case '\t':
    (*rl)->state |= READLINE_POST_PROCESS;
    return READLINE_POST_PROCESS;
  }

  (*rl)->state |= READLINE_OK;
  return READLINE_OK;
}

static void ed_search_history_push (ed_t *this, char *bytes, size_t len) {
  if ($my(max_num_hist_entries) < $my(history)->search->num_items) {
    histitem_t *tmp = DListPopTail ($my(history)->search, histitem_t);
    String.release (tmp->data);
    free (tmp);
  }

  histitem_t *h = Alloc (sizeof (histitem_t));
  h->data = String.new_with_len (bytes, len);
  DListPush ($my(history)->search, h);
}

static int buf_search (buf_t *this, char com, char *str, utf8 cc) {
  if (this->num_items is 0) return NOTHING_TODO;

  int toggle = 0;
  int hist_called = 0; /* if this variable declared a little bit before the for loop
  * gcc will complain for "maybe used uninitialized" because the code can jump to
  * the search label which is inside the for loop, clang do not warn for this, the
  * code works but valgrind reports uninitialized value */

  Search_t *sch = Alloc (sizeof (Search_t));
  sch->found = 0;
  sch->prefix = sch->match = NULL;
  sch->row = this->current;

  if (com is '/' or com is '*' or com is 'n') sch->dir = 1;
  else sch->dir = -1;

  sch->cur_idx = SEARCH_UPDATE_ROW (this->cur_idx);

  histitem_t *his = $my(history)->search->head;

  MSG(" ");

  readline_t *rl = Readline.new ($my(root), $my(term_ptr), Input.getkey,
      *$my(prompt_row_ptr), 1, $my(dim)->num_cols, $my(video));

  rl->error = readline_error;
  rl->at_beg = readline_search_at_beg;
  rl->at_end = Readline.set.break_state;
  rl->expr_reg = readline_expr_reg;
  rl->on_write = readline_on_write;

  rl->prompt_char = (com is '*' or com is '/') ? '/' : '?';

  if (com is '*' or com is '#') {
    com = '*' is com ? '/' : '?';

    char word[MAXLEN_WORD]; word[0] = '\0';
    int fidx, lidx;
    self(get.current_word, word, Notword, Notword_len, &fidx, &lidx);
    sch->pat = String.new_with (word);
    if (sch->pat->num_bytes) {
      Readline.set.line (rl, sch->pat->bytes, sch->pat->num_bytes);
      goto search;
    }
  } else {
    if (str isnot NULL) {
      sch->pat = String.new_with (his->data->bytes);
      Readline.insert_with_len (rl, sch->pat->bytes, sch->pat->num_bytes);

      com = 'n' is com ? '/' : '?';
      rl->prompt_char = com;
      Readline.write_and_break (rl);
      if (cc is -1)
        goto search;
      else
        goto theloop;

    } else if (com is 'n' or com is 'N') {
      if ($my(history)->search->num_items is 0) return NOTHING_TODO;
      sch->pat = String.new_with (his->data->bytes);
      Readline.insert_with_len (rl, sch->pat->bytes, sch->pat->num_bytes);

      com = 'n' is com ? '/' : '?';
      rl->prompt_char = com;
      Readline.write_and_break (rl);
      goto search;
    } else
      sch->pat = String.new (1);
  }

theloop:
  for (;;) {
    utf8 c;
    if (cc isnot -1)
      c = cc;
    else
      c = Readline.edit (rl)->c;

    string_t *p = Vstring.join (rl->line, "");

    if (rl->line->tail->data->bytes[0] is ' ')
      String.clear_at (p, p->num_bytes - 1);

    if (Cstring.eq (sch->pat->bytes, p->bytes)) {
      String.release (p);
    } else {
      String.replace_with (sch->pat, p->bytes);
      String.release (p);

search:
      ifnot (NULL is sch->prefix) { free (sch->prefix); sch->prefix = NULL; }
      ifnot (NULL is sch->match) { free (sch->match); sch->match = NULL; }
      __buf_search__ (this, sch);
      if (toggle) {
        sch->dir = (sch->dir is 1) ? -1 : 1;
        toggle = 0;
      }

      if (sch->found) {
        Msg.set_fmt ($my(root), COLOR_NORMAL, MSG_SET_OPEN, "|%d %d|%s",
            sch->idx + 1, sch->col, sch->prefix);

        size_t hl_idx = $myroots(msgline)->num_bytes;

        Msg.set_fmt ($my(root), COLOR_NORMAL, MSG_SET_APPEND|MSG_SET_CLOSE, "%s%s",
            sch->match, sch->end);

        Video.row_hl_at ($my(video), *$my(msg_row_ptr) - 1, COLOR_RED,
            hl_idx, hl_idx + bytelen(sch->match));

        sch->cur_idx = sch->idx;
      } else {
        search_t *s = ListStackPop (sch, search_t);
        if (NULL isnot s) {
          sch->cur_idx = s->idx;
          sch->row = s->row;
          free (s);
        }

      MSG(" ");
      }

      continue;
    }

    switch (c) {

      case ESCAPE_KEY:
        sch->found = 0;

      case '\r':
        goto theend;

      case CTRL('n'):
        if (sch->found) {
          sch->cur_idx = SEARCH_UPDATE_ROW (sch->idx);
          goto search;
        }

        continue;

      case CTRL('p'):
        if (sch->found) {
          sch->dir = (sch->dir is 1) ? -1 : 1;
          sch->cur_idx = SEARCH_UPDATE_ROW (sch->idx);
          toggle = 1;
          goto search;
        }

        continue;

      case ARROW_UP_KEY:
      case ARROW_DOWN_KEY:
        if ($my(history)->search->num_items is 0) continue;
        if (c is ARROW_DOWN_KEY) {
          if (his->prev isnot NULL)
            his = his->prev;
          else
            his = $my(history)->search->tail;
          hist_called++;
        } else {
          if (hist_called) {
            if (his->next isnot NULL)
              his = his->next;
            else
              his = $my(history)->search->head;
          } else hist_called++;
        }

        String.replace_with (sch->pat, his->data->bytes);

        rl->state |= READLINE_CLEAR_FREE_LINE;
        Readline.clear (rl);
        Readline.insert_with_len (rl, sch->pat->bytes, sch->pat->num_bytes);
        goto search;

      default:
        continue;
    }
  }

theend:
  if (sch->found) {
    ed_search_history_push ($my(root), sch->pat->bytes, sch->pat->num_bytes);
    self(normal.goto_linenr, sch->idx + 1, DRAW);
  }

  MSG(" ");

  Readline.clear (rl);
  Readline.release (rl);

  SEARCH_FREE;
  return DONE;
}

static int buf_grep_on_normal (buf_t **thisp, utf8 com, int count, int regidx) {
  (void) count; (void) regidx;
  buf_t *this = *thisp;

  if (com isnot '\r' and com isnot 'q') return 0;
  if (com is 'q') {
    if (NOTHING_TODO is Ed.win.change ($my(root), thisp, VED_COM_WIN_CHANGE_PREV_FOCUSED,
       NULL, NO_OPTION, NO_FORCE))
      return EXIT_THIS;
    return -1;
  }

  this = *thisp;
  self(normal.bol, DONOT_DRAW);
  int retval = buf_open_fname_under_cursor (thisp, 0, OPEN_FILE_IFNOT_EXISTS,
      DONOT_REOPEN_FILE_IF_LOADED, DRAW);
  if (NOTHING_TODO is retval)
    self(draw);
  return retval + 1;
}

static int buf_search_file (buf_t *this, char *fname, re_t *re) {
  FILE *fp = fopen (fname, "r");
  if (fp is NULL) return NOTOK;

  size_t fnlen = bytelen (fname);
  char *line = NULL;
  size_t len = 0;
  size_t nread = 0;
  int idx = 0;
  while (-1 isnot (int) (nread = ed_readline_from_fp (&line, &len, fp))) {
    idx++;
    int ret = Re.exec (re, line, nread);
    if (ret is RE_UNBALANCED_BRACKETS_ERROR) {
      MSG_ERRNO (RE_UNBALANCED_BRACKETS_ERROR);
      break;
    }
    if (ret is RE_NO_MATCH) continue;

    size_t blen = (fnlen) + 32 + nread;
    char bytes[blen + 1];
    snprintf (bytes, blen, "%s|%d col %d| %s", fname, idx, re->match_idx, line);
    buf_current_append_with (this, bytes);
    Re.reset_captures (re);
  }

  fclose (fp);
  if (line isnot NULL) free (line);
  return 0;
}

static int buf_grep (buf_t **thisp, char *pat, Vstring_t *fnames) {
  buf_t *this = *thisp;
  int idx = 0;
  win_t *w = Ed.get.win_by_name ($my(root), VED_SEARCH_WIN, &idx);
  this = Win.get.buf_by_name (w, VED_SEARCH_BUF, &idx);
  if (this is NULL) return NOTHING_TODO;
  self(clear);
  String.replace_with_fmt ($mycur(data), "searching for %s", pat);
  int flags = 0;
  re_t *re = Re.new (pat, flags, RE_MAX_NUM_CAPTURES, Re.compile);

  vstring_t *it = fnames->head;
  char *dname = it is NULL ? NULL : it->data->bytes;

  while (it) {
    char *fname = it->data->bytes;
    if (File.exists (fname))
      ifnot (Dir.is_directory (fname))
        if (File.is_readable (fname))
          ifnot (File.is_elf (fname))
            buf_search_file (this, fname, re);
    it = it->next;
  }

  ifnot (NULL is dname) {
    free ($my(cwd));
    if (*dname is '.' or *dname isnot DIR_SEP)
      $my(cwd) = Dir.current ();
    else
      $my(cwd) = Path.dirname (dname);
  }

  Re.release (re);

  if (this->num_items is 1) return NOTHING_TODO;

  self(set.video_first_row, 0);
  self(current.set, 0);
  $my(video)->row_pos = $my(cur_video_row);
  $my(video)->col_pos = $my(cur_video_col);
  self(normal.down, 1, DONOT_ADJUST_COL, DONOT_DRAW);
  ifnot (Cstring.eq ($from((*thisp), fname), VED_SEARCH_BUF))
    ed_buf_change ($my(root), thisp, VED_SEARCH_WIN, VED_SEARCH_BUF);
  else
    self(draw);

  return DONE;
}

static int buf_substitute (buf_t *this, const char *pat, const char *sub, int global,
                                     int interactive, int fidx, int lidx) {
  ed_record ($my(root), "buf_substitute (buf, \"%s\", \"%s\", %d, %d, %d, %d)",
    pat, sub, global, interactive, fidx, lidx);

  int retval = NOTHING_TODO;
  ifnot (this->num_items) return retval;

  string_t *substr = NULL;
  int flags = 0;
  re_t *re = Re.new (pat, flags, RE_MAX_NUM_CAPTURES, Re.compile);

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);

  row_t *it = this->head;
  int idx = 0;
  while (idx < fidx) {
    idx++;
    if (NULL is it->next) break;
    it = it->next;
  }

  if (lidx >= this->num_items) lidx = this->num_items - 1;

  while (idx++ <= lidx) {
    int bidx = 0;

searchandsub:;
    int done_substitution = 0;
    Re.reset_captures (re);

    if (0 > Re.exec (re, it->data->bytes + bidx,
        it->data->num_bytes - bidx)) goto thenext;

    String.release (substr);

    if (NULL is (substr = Re.parse_substitute (re, sub, re->match->bytes))) {
      MSG_ERROR("Error: %s", re->errmsg);
      goto theend;
    }

    if (interactive) {
      size_t matchlen = bidx + re->match_idx;
      char prefix[matchlen + 1];
      Cstring.cp (prefix, matchlen + 1, it->data->bytes, matchlen);

      utf8 chars[] = {'y', 'Y', 'n', 'N', 'q', 'Q', 'a', 'A', 'c', 'C'};
      char qu[MAXLEN_LINE]; /* using STR_FMT (a statement expression) causes  */
                        /* messages for uninitialized value[s] (on clang) */
      snprintf (qu, MAXLEN_LINE,
        "|match at line %d byte idx %d|\n"
        "%s%s%s%s%s\n"
        "|substitution string|\n"
        "%s%s%s\n"
        "replace? yY[es]|nN[o] replace all?aA[ll], continue next line? cC[ontinue], quit? qQ[uit]\n",
         idx, re->match_idx + bidx, prefix, TERM_MAKE_COLOR(COLOR_MENU_SEL), re->match->bytes,
         TERM_MAKE_COLOR(COLOR_MENU_BG), re->match_ptr + re->match_len,
         TERM_MAKE_COLOR(COLOR_MENU_SEL), (substr->num_bytes ? substr->bytes : " "), TERM_MAKE_COLOR(COLOR_MENU_BG));

      utf8 c = buf_quest (this, qu, chars, ARRLEN(chars));

      switch (c) {
        case 'n': case 'N': goto if_global;
        case 'q': case 'Q': goto theend;
        case 'c': case 'C': goto thenext;
        case 'a': case 'A': interactive = 0;
      }
    }

    action_t *baction = self(action.new_with, REPLACE_LINE, idx - 1,
        it->data->bytes, it->data->num_bytes);
    ListStackPush (Action, baction);

    String.replace_numbytes_at_with (it->data, re->match_len, re->match_idx + bidx,
      substr->bytes);
    done_substitution = 1;

    retval = DONE;

if_global:
    if (global) {
      int len = (done_substitution ? (int) substr->num_bytes : 1);
      bidx += (re->match_idx + len);
      if (bidx >= (int) it->data->num_bytes) goto thenext;
      goto searchandsub;
    }

thenext:
    it = it->next;
  }

theend:
  if (retval is DONE) {
    $my(flags) |= BUF_IS_MODIFIED;
    self(undo.push, Action);

    if ($mycur(cur_col_idx) >= (int) $mycur(data)->num_bytes)
      self(normal.eol, DONOT_DRAW);
    self(draw);
  } else
    self(Action.release, Action);

  Re.release (re);
  String.release (substr);

  return retval;
}

static FILE *ed_file_pointer_from_X (ed_t *this, int target) {
  (void) this;
  if (NULL is getenv ("DISPLAY")) return NULL;

  string_t *xclip_exec = Sys.which ("xclip", NULL);
  if (NULL is xclip_exec) return NULL;

  size_t len = xclip_exec->num_bytes + 32;
  char command[len];
  snprintf (command, len, "%s -o -selection %s", xclip_exec->bytes,
      (target is X_PRIMARY) ? "primary" : "clipboard");

  String.release (xclip_exec);

  return popen (command, "r");
}

static void ed_selection_to_X (ed_t *this, char *bytes, size_t len, int target) {
  (void) this;
  string_t *xclip_exec = Sys.which ("xclip", NULL);
  if (NULL is xclip_exec) return;

  if (NULL is getenv ("DISPLAY")) return;

  size_t ex_len = xclip_exec->num_bytes + 32;
  char command[ex_len + 32];
  snprintf (command, ex_len + 32, "%s -i -selection %s 2>/dev/null",
    xclip_exec->bytes, (target is X_PRIMARY) ? "primary" : "clipboard");

  String.release (xclip_exec);

  FILE *fp = popen (command, "w");
  if (NULL is fp) return;
  fwrite (bytes, 1, len, fp);
  pclose (fp);
}

static int buf_selection_to_X_word_actions_cb (buf_t **thisp, int fidx, int lidx,
                               bufiter_t *it, char *word, utf8 c, char *action) {
  (void) it; (void) action;
  buf_t *this = *thisp;
  ed_selection_to_X ($my(root), word, lidx - fidx + 1,
  	('*' is c ? X_PRIMARY : X_CLIPBOARD));
  return DONE;
}

static void reg_release (Reg_t *rg) {
  reg_t *reg = rg->head;
  while (reg) {
    reg_t *tmp = reg->next;
    String.release (reg->data);
    free (reg);
    reg = tmp;
  }

  rg->head = NULL;
}

static Reg_t *ed_reg_get (ed_t *this, int regidx) {
  if (regidx is REG_BLACKHOLE) return &$my(regs)[REG_BLACKHOLE];

  Reg_t *rg = NULL;
  if (regidx isnot REG_SHARED)
    rg = &$my(regs)[regidx];
  else
    rg = &$from($my(root), shared_reg)[0];

  return rg;
}

static Reg_t *ed_reg_new (ed_t *this, int regidx) {
  Reg_t *rg = ed_reg_get (this, regidx);
  reg_release (rg);
  return rg;
}

static void ed_reg_init_all (ed_t *this) {
  for (int i = 0; i < NUM_REGISTERS; i++)
    $my(regs)[i] = (Reg_t) {.reg = REGISTERS[i]};

  $my(regs)[REG_RDONLY].head = Alloc (sizeof (reg_t));
  $my(regs)[REG_RDONLY].head->data = String.new_with ("     ");
  $my(regs)[REG_RDONLY].head->next = NULL;
}

static int ed_reg_get_idx (ed_t *this, int c) {
  (void) this;
  if (c is 0x17) c = REG_CURWORD_CHR;
  char regs[] = REGISTERS; /* this is for tcc */
  char *r = Cstring.byte.in_str (regs, c);
  return (NULL isnot r) ? (r - regs) : NOTOK;
}

static Reg_t *ed_reg_append (ed_t *this, int regidx, int type, reg_t *reg) {
  if (regidx is REG_BLACKHOLE) return &$my(regs)[REG_BLACKHOLE];
  Reg_t *rg = NULL;
  if (regidx isnot REG_SHARED)
    rg = &$my(regs)[regidx];
  else
    rg = &$from($my(root), shared_reg)[0];

  rg->reg = regidx;
  rg->type = type;

  ListStackAppend (rg, reg_t, reg);
  return rg;
}

static Reg_t *ed_reg_push (ed_t *this, int regidx, int type, reg_t *reg) {
  if (regidx is REG_BLACKHOLE) return &$my(regs)[REG_BLACKHOLE];
  Reg_t *rg = NULL;
  if (regidx isnot REG_SHARED)
    rg = &$my(regs)[regidx];
  else
    rg = &$from($my(root), shared_reg)[0];

  rg->reg = regidx;
  rg->type = type;
  ListStackPush (rg, reg);
  return rg;
}

static Reg_t *ed_reg_push_r (ed_t *this, int regidx, int type, reg_t *reg) {
  if (regidx is REG_BLACKHOLE) return &$my(regs)[REG_BLACKHOLE];
  Reg_t *rg = NULL;
  if (regidx isnot REG_SHARED)
    rg = &$my(regs)[regidx];
  else
    rg = &$from($my(root), shared_reg)[0];

  reg_t *it = rg->head;
  if (it is NULL)
    return ed_reg_push (this, regidx, type, reg);

  rg->reg = regidx;
  rg->type = type;

  while (it) {
    if (it->next is NULL) break;
    it = it->next;
  }

  reg->next = NULL;
  it->next = reg;

  return rg;
}

static Reg_t *ed_reg_push_with (ed_t *this, int regidx, int type, char *bytes, int dir) {
  if (regidx is REG_BLACKHOLE) return &$my(regs)[REG_BLACKHOLE];
  reg_t *reg = Alloc (sizeof (reg_t));
  reg->data = String.new_with (bytes);
  if (dir is REVERSE_ORDER)
    return ed_reg_push_r (this, regidx, type, reg);
  return ed_reg_push (this, regidx, type, reg);
}

static int ed_reg_is_special (ed_t *this, int regidx) {
  (void) this; // maybe in future
  if (REG_SEARCH > regidx or (regidx > REG_EXPR and regidx < REG_CURWORD))
    return NOTOK;
  return OK;
}

static Reg_t *ed_reg_set (ed_t *this, int c, int type, char *bytes, int dir) {
  int regidx = ed_reg_get_idx (this, c);
  if (NOTOK is regidx) return NULL;
  ed_reg_new (this, regidx);
  return ed_reg_push_with (this, regidx, type, bytes, dir);
}

static Reg_t *ed_reg_setidx (ed_t *this, int regidx, int type, char *bytes, int dir) {
  ed_reg_new (this, regidx);
  return ed_reg_push_with (this, regidx, type, bytes, dir);
}

static int ed_reg_expression (ed_t *this, buf_t *buf, int regidx) {
  for (int i = 0; i < $my(num_expr_reg_cbs); i++) {
    int retval = $my(expr_reg_cbs)[i] (this, buf, regidx);
    if (retval isnot NO_CALLBACK_FUNCTION) return retval;
  }

  return NOTOK;
}

static int ed_reg_special_set (ed_t *this, buf_t *buf, int regidx) {
  if (NOTOK is ed_reg_is_special (this, regidx))
    return NOTHING_TODO;

  switch (regidx) {
    case REG_SEARCH:
      {
        histitem_t *his = $my(history)->search->head;
        if (NULL is his) return NOTOK;
        ed_reg_push_with (this, regidx, CHARWISE, his->data->bytes, DEFAULT_ORDER);
        return DONE;
      }

    case REG_PROMPT:
      if ($my(history)->readline->num_items is 0) return NOTOK;
      {
        readline_hist_item_t *it = $my(history)->readline->head;
        if (NULL is it) return ERROR;
        string_t *str = Vstring.join (it->data->line, "");
        ed_reg_push_with (this, regidx, CHARWISE, str->bytes, DEFAULT_ORDER);

        String.release (str);
        return DONE;
      }

    case REG_FNAME:
      ed_reg_new (this, regidx);
      ed_reg_push_with (this, regidx, CHARWISE, $from(buf, fname), DEFAULT_ORDER);
      return DONE;

    case REG_PLUS:
    case REG_STAR:
      {
        FILE *fp = ed_file_pointer_from_X (this, (REG_STAR is regidx) ? X_PRIMARY : X_CLIPBOARD);
        if (NULL is fp) return ERROR;
        ed_reg_new (this, regidx);
        size_t len = 0;
        char *line = NULL;

        // while (-1 isnot ed_readline_from_fp (&line, &len, fp)) {
        // do it by hand to look for new lines and proper set the type
        ssize_t nread;
        int type = CHARWISE;
        while (-1 isnot (nread = getline (&line, &len, fp))) {
          if (nread) {
            if (line[nread - 1] is '\n' or line[nread - 1] is '\r') {
              line[nread - 1] = '\0';
              type = LINEWISE;
            }

            ed_reg_push_with (this, regidx, type, line, REVERSE_ORDER);
            //ed_reg_push_with (this, regidx, type, line, DEFAULT_ORDER);
          }
        }

        if (line isnot NULL) free (line);
        pclose (fp);
        return DONE;
      }

    case REG_CURWORD:
      {
        char word[MAXLEN_WORD]; int fidx, lidx;
        ifnot (NULL is buf_get_current_word (buf, word, Notword, Notword_len,
            &fidx, &lidx)) {
          ed_reg_new (this, regidx);
          ed_reg_push_with (this, regidx, CHARWISE, word, DEFAULT_ORDER);
        } else
          return ERROR;
      }
      return DONE;

    case REG_EXPR:
      return ed_reg_expression (this, buf, regidx);

    case REG_SHARED:
      $my(regs)[REG_SHARED] = $from($my(root), shared_reg)[0];
      return DONE;

    case REG_BLACKHOLE:
      return NOTOK;
  }

  return NOTHING_TODO;
}

static Reg_t *ed_reg_set_with (ed_t *this, int regidx, int type, char *bytes, int dir) {
  if (regidx is REG_BLACKHOLE) return &$my(regs)[REG_BLACKHOLE];

  ed_reg_new (this, regidx);
  reg_t *reg = Alloc (sizeof (reg_t));
  reg->data = String.new_with (bytes);
  if (-1 is dir)
    return ed_reg_push_r (this, regidx, type, reg);
  return ed_reg_push (this, regidx, type, reg);
}

static utf8 buf_quest (buf_t *this, char *qu, utf8 *chs, int len) {
  Video.paint_rows_with ($my(video), -1, -1, -1, qu);
  SEND_ESC_SEQ ($my(video)->fd, TERM_CURSOR_HIDE);
  utf8 c;
  for (;;) {
    c = Input.getkey (STDIN_FILENO);
    for (int i = 0; i < len; i++)
      if (chs[i] is c) goto theend;
  }

theend:
  Video.resume_painted_rows ($my(video));
  SEND_ESC_SEQ ($my(video)->fd, TERM_CURSOR_SHOW);
  return c;
}

static utf8 ed_question (ed_t *this, char *qu, utf8 *chs, int len) {
  return buf_quest (this->current->current, qu, chs, len);
}

static char *buf_parse_line (buf_t *this, row_t *row, char *line, int idx) {
  Ustring.encode ($my(line), row->data->bytes, row->data->num_bytes,
      CLEAR, $my(ftype)->tabwidth, row->first_col_idx);

  int numchars = 0;
  int j = 0;

  ustring_t *it = $my(line)->current;
  ustring_t *tmp = it;
  while (it and numchars < $my(dim)->num_cols) {
    for (int i = 0; i < it->len; i++) line[j++] = it->buf[i];
    numchars += it->width;
    tmp = it;
    it = it->next;
  }

  it = tmp;
  while (numchars > $my(dim)->num_cols) {
    j -= it->len;
    numchars -= it->width;
    it = it->prev;
  }

  line[j] = '\0';
  return $my(syn)->parse (this, line, j, idx, row);
}

static void buf_draw_current_row (buf_t *this) {
  char line[MAXLEN_LINE];
  buf_parse_line (this, this->current, line, this->cur_idx);
  Video.set.row_with ($my(video), $my(video)->row_pos - 1, line);
  Video.draw.row_at ($my(video), $my(video)->row_pos);
  buf_set_draw_statusline (this);
  Cursor.set_pos ($my(term_ptr), $my(video)->row_pos, $my(video)->col_pos);
}

static void buf_to_video (buf_t *this) {
  char line[MAXLEN_LINE];
  int cur_idx = this->cur_idx;
  int idx = $my(video_first_row_idx);

  row_t *row = this->current;
  row_t *current = row;

  /* this is started actually as a workaround to fix the undo/redo
   * when the video_first_line property ended up with a free'd row
   * by a delete operation. Studing a bit the mechanism and based
   * on the algorithm which is weak, we either should try to (at least)
   * check all the rows on the undo/redo structs - but even then
   * there wasn't a guarrantee that after an opossite operation
   * the structures wouldn't end up as dangling pointers. Plus we 
   * had to check in every place (at least two) where is a delete
   * operation of this kind. So instead of complicating the code
   * considerable, maybe is better to be safe and do it only in
   * this block of code. Actually this is place where we can set
   * and adjust the right row pointer to this property. Though i
   * suspect that we might be doing without this just fine.
   */
  do {  // we know some details which the abstraction needs to check
    if (idx is cur_idx) break;  // so we can optimize a bit

    if (idx < cur_idx)
      for (int i = idx; i < cur_idx; i++) row = row->prev;
    else
      for (int i = idx; row->next and i < cur_idx; i++) row = row->next;
  } while (0);

  $my(video_first_row) = row;

  int i;
  for (i = $my(dim)->first_row - 1; i < $my(statusline_row) - 1; i++) {
    if (row is NULL) break;
    buf_parse_line (this, row, line, idx++);
    Video.set.row_with ($my(video), i, line);
    row = row->next;
  }

  this->current = current;

  while (i < $my(statusline_row) - 1)
    Video.set.row_with ($my(video), i++, $my(ftype)->on_emptyline->bytes);

  buf_set_statusline (this);
}

static void buf_flush (buf_t *this) {
  Video.render_set_from_to ($my(video), $my(dim)->first_row, $my(statusline_row));
  String.append_with_fmt ($my(video)->render, TERM_GOTO_PTR_POS_FMT,
      $my(video)->row_pos, $my(video)->col_pos);
  Video.flush ($my(video), $my(video)->render);
}

static void buf_draw (buf_t *this) {
  String.clear ($my(video)->render);
  Ed.set.topline ($my(root), this);
  Video.render_set_from_to ($my(video), $myroots(topline_row), $myroots(topline_row));
  self(to.video);
  self(flush);
}

static int buf_com_diff (buf_t **thisp, readline_t *rl, int to_stdout) {
  buf_t *this = *thisp;
  string_t *diff_exec = Sys.which ("diff", NULL);

  if (NULL is diff_exec) {
    Msg.error ($my(root), "diff executable can not be found in $PATH");
    return NOTOK;
  }

  int retval = NOTHING_TODO;

  char file[PATH_MAX]; file[0] = '\0';

  ifnot (NULL is rl) {
    int origin = Readline.arg.exists (rl, "origin");
    ifnot (origin) goto thenext_condition;

    if (NULL is $my(backupfile)) {
      Msg.send ($my(root), COLOR_WARNING, "backupfile hasn't been set");
      goto theend;
    }

    Cstring.cp (file, PATH_MAX, $my(backupfile), bytelen ($my(backupfile)));
    goto thediff;
  }

thenext_condition:
  ifnot (File.exists ($my(fname))) goto theend;

  Cstring.cp (file, PATH_MAX, $my(fname), bytelen ($my(fname)));

thediff:;

  tmpfname_t *tmpn = File.tmpfname.new (Sys.get.env_value ("E_TMPDIR"), $my(basename));
  if (NULL is tmpn or -1 is tmpn->fd) goto theend;

  char com[MAXLEN_COM];
  Cstring.cp_fmt (com, MAXLEN_COM, "%s -u %s %s", diff_exec->bytes,
      file, tmpn->fname->bytes);

  buf_write_to_fname (this, tmpn->fname->bytes, DONOT_APPEND, 0, this->num_items - 1, FORCE, VERBOSE_OFF);

  if (to_stdout)
    retval = Ed.sh.popen ($my(root), this, com, ED_PROC_WAIT_AT_END, NULL);
  else {
    this = Ed.buf.get ($my(root), VED_DIFF_WIN, VED_DIFF_BUF);
    if (this) {
      self(clear);
      retval = Ed.sh.popen ($my(root), this, com, ED_PROC_READ_STDOUT, NULL);
      retval = (retval == 1 ? OK : (retval == 0 ? 1 : retval));
      if (OK is retval) {     // diff returns 1 when files differ
        Ed.buf.change ($my(root), thisp, VED_DIFF_WIN, VED_DIFF_BUF);
        retval = OK;
      } else
        Msg.send_fmt ($my(root), COLOR_MSG, "No differences have been found");
    }
  }

  File.tmpfname.release (tmpn, FILE_TMPFNAME_UNLINK_FILE|FILE_TMPFNAME_CLOSE_FD);

theend:
  String.release (diff_exec);
  return retval;
}

static int ed_quit (ed_t *ed, int force, int global) {
  int retval = (global ? (force ? EXIT_ALL_FORCE : EXIT_ALL) : EXIT_THIS);
  if (force) return retval;

  win_t *w = ed->head;

 while (w) {
    if (win_isit_special_type (w)) goto winnext;

    buf_t *this = w->head;
    while (this isnot NULL) {
      if ($my(flags) & BUF_IS_SPECIAL
          or 0 is ($my(flags) & BUF_IS_MODIFIED)
          or ($my(flags) & BUF_IS_PAGER)
          or Cstring.eq ($my(fname), UNNAMED))
        goto bufnext;

      if ($my(flags) & BUF_SAVE_ON_EXIT) {
        self(write, FORCE);
        goto bufnext;
      }

      utf8 chars[] = {'y', 'Y', 'n', 'N', 'c', 'C','d'};
thequest:;
      utf8 c = buf_quest (this, STR_FMT (
         "%s has been modified since last change\n"
         "continue writing? [yY|nN], [cC]ansel, unified [d]iff?",
         $my(fname)), chars, ARRLEN (chars));
      switch (c) {
        case 'y':
        case 'Y':
          self(write, FORCE);
        case 'n':
        case 'N':
          break;
        case 'c':
        case 'C':
          retval = NOTHING_TODO;
          goto theend;
        case 'd':
          buf_com_diff (&this, NULL, 1);
          goto thequest;
      }

bufnext:
      this = this->next;
    }

winnext:
    w = w->next;
  }

theend:
  return retval;
}

static void buf_on_blankline (buf_t *this) {
  ifnot ($my(ftype)->clear_blanklines) return;

  int lineisblank = 1;
  for (int i = 0; i < (int) $mycur(data)->num_bytes; i++) {
    if ($mycur(data)->bytes[i] isnot ' ') {
      lineisblank = 0;
      break;
    }
  }

  if (lineisblank) {
    String.replace_with ($mycur(data), "");
    $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;
    $mycur(cur_col_idx) = $mycur(first_col_idx) = 0;
  }
}

static int buf_normal_right (buf_t *this, int count, int draw) {
  int is_ins_mode = IS_MODE (INSERT_MODE);

  if ($mycur(cur_col_idx) is ((int) $mycur(data)->num_bytes -
      Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)]) +
      is_ins_mode) or 0 is $mycur(data)->num_bytes or
      $mycur(data)->bytes[$mycur(cur_col_idx)] is 0 or
      $mycur(data)->bytes[$mycur(cur_col_idx)] is '\n')
    return NOTHING_TODO;

  Ustring.encode ($my(line), $mycur(data)->bytes, $mycur(data)->num_bytes,
      CLEAR, $my(ftype)->tabwidth, $mycur(cur_col_idx));

  ustring_t *it = buf_get_line_nth ($my(line), $mycur(cur_col_idx));

  int orig_count = count;
  while (count-- and it) {
    if (it->code is '\n' or it is $my(line)->tail) {
      if (count is orig_count - 1)
        return NOTHING_TODO;
      else
        break;
    }

    $mycur(cur_col_idx) += it->len;

    if ($my(video)->col_pos is $my(dim)->num_cols) {
      $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;
      $mycur(first_col_idx) = $mycur(cur_col_idx);
      ifnot (is_ins_mode)
        if (it->next)
           $my(video)->col_pos = $my(cur_video_col) =
               $my(video)->col_pos + (it->next->width - 1);
    } else {
      $my(video)->col_pos = $my(cur_video_col) = $my(video)->col_pos + 1;
      ifnot (is_ins_mode) {
        if (it->next) {
           $my(video)->col_pos = $my(cur_video_col) =
               $my(video)->col_pos + (it->next->width - 1);

          if ($my(video)->col_pos > $my(dim)->num_cols) {
            $my(video)->col_pos = $my(cur_video_col) = it->next->width;
            $mycur(first_col_idx) = $mycur(cur_col_idx);
          }
        }
      }

      if (is_ins_mode)
        $my(video)->col_pos = $my(cur_video_col) =
             $my(video)->col_pos + (it->width - 1);
    }
    it = it->next;
  }

  if (draw) self(draw_current_row);
  return DONE;
}

static int buf_normal_noblnk (buf_t *this) {
  ifnot ($mycur(data)->num_bytes) return NOTHING_TODO;

  if ($mycur(cur_col_idx) is 0 and 0 is IS_SPACE ($mycur(data)->bytes[$mycur(cur_col_idx)]))
    return NOTHING_TODO;

  $mycur(cur_col_idx) = 0;
  $my(video)->col_pos = $my(cur_video_col) =
      Ustring.width ($mycur(data)->bytes, $my(ftype)->tabwidth);

  int i = 0;
  for (; i < (int) $mycur(data)->num_bytes; i++) {
    if (IS_SPACE ($mycur(data)->bytes[i])) {
      self(normal.right, 1, DONOT_DRAW);
      continue;
    }

    break;
  }

  if (i) self(draw);

  return DONE;
}

static int buf_normal_eol (buf_t *this, int draw) {
  int retval_bol = DONE;
  if ($mycur(cur_col_idx) >= (int) $mycur(data)->num_bytes)
    retval_bol = self(normal.bol, DONOT_DRAW);
  int retval = self(normal.right, $mycur(data)->num_bytes * 4, draw);
  if (NOTHING_TODO is retval and retval_bol is DONE) {
    if (draw) self(draw);
    return DONE;
  }

  return retval;
}

static int buf_normal_left (buf_t *this, int count, int draw) {
  int is_ins_mode = IS_MODE (INSERT_MODE);
  ifnot ($mycur(cur_col_idx)) {
    if ($my(cur_video_col) isnot 1 and $mycur(data)->bytes[0] is 0)
      $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;
    return NOTHING_TODO;
  }

  Ustring.encode ($my(line), $mycur(data)->bytes, $mycur(data)->num_bytes,
      CLEAR, $my(ftype)->tabwidth, $mycur(cur_col_idx));

  ustring_t *it = buf_get_line_nth ($my(line), $mycur(cur_col_idx));

  if (it is NULL)
    it = $my(line)->tail;
  else
    if ($my(line)->num_items isnot 1)
      if ((0 is is_ins_mode) or (is_ins_mode and (it->next isnot NULL or
        it->code is '\n')))
      it = it->prev;

  int curcol = $mycur(first_col_idx);

  while (it and count-- and $mycur(cur_col_idx)) {
    int len = it->len;
    $mycur(cur_col_idx) -= len;
    ustring_t *fcol = buf_get_line_nth ($my(line), $mycur(first_col_idx));

    if ($my(cur_video_col) is 1 or $my(cur_video_col) - fcol->width is 0) {
      ifnot ($mycur(first_col_idx)) return NOTHING_TODO;

      int num = 0;
      ustring_t *tmp = NULL; // can not be NULL
      while ($mycur(first_col_idx) and fcol and num < $my(dim)->num_cols) {
        $mycur(first_col_idx) -= fcol->len;
        num += fcol->width;
        tmp = fcol;
        fcol = fcol->prev;
      }

      if (num > $my(dim)->num_cols) {
        $mycur(first_col_idx) += tmp->len;
        num -= tmp->width;
      }

      $my(video)->col_pos = $my(cur_video_col) = num;

      ifnot (is_ins_mode)
        $my(video)->col_pos = $my(cur_video_col) =
            $my(video)->col_pos + (it->width - 1);

      if (is_ins_mode and 0 is $mycur(first_col_idx))
        $my(video)->col_pos = $my(cur_video_col) =
            $my(video)->col_pos - (tmp->width) + 1;

    } else {
      int width = $my(cur_video_col) - 1;
      if (is_ins_mode)
        width -= it->width - 1;
      else
        if (it->next)
          width -= (it->next->width) - 1;

      ifnot (width) {
        width = 1;
        //$mycur(first_col_idx) -= it->len;
      }

      $my(video)->col_pos = $my(cur_video_col) = width;
    }

    it = it->prev;
  }

  ifnot (draw) return DONE;

  if ($mycur(first_col_idx) isnot curcol) {
    self(draw_current_row);
  } else {
    buf_set_draw_statusline (this);
    Cursor.set_pos ($my(term_ptr), $my(video)->row_pos, $my(video)->col_pos);
  }

  return DONE;
}

static int buf_normal_bol (buf_t *this, int draw) {
  ifnot ($mycur(cur_col_idx)) return NOTHING_TODO;
  $mycur(first_col_idx) = $mycur(cur_col_idx) = 0;
  $my(video)->col_pos = $my(cur_video_col) =
      Ustring.width ($mycur(data)->bytes, $my(ftype)->tabwidth);

  if (draw) self(draw_current_row);
  return DONE;
}

static int buf_normal_end_word (buf_t *this, int count, int run_insert_mode, int draw) {
  ifnot ($mycur(data)->num_bytes) return NOTHING_TODO;

  int cur_idx = $mycur(cur_col_idx);
  int retval = count <= 0 ? NOTHING_TODO : DONE;
  for (int i = 0; i < count; i++) {
    while (($mycur(cur_col_idx) isnot (int) $mycur(data)->num_bytes - 1) and
          (0 is (IS_SPACE ($mycur(data)->bytes[$mycur(cur_col_idx)])))) {
      retval = self(normal.right, 1, DONOT_DRAW);
      if (NOTHING_TODO is retval) break;
   }

    if (NOTHING_TODO is retval) break;
    if (NOTHING_TODO is (retval = self(normal.right, 1, DONOT_DRAW))) break;
  }

  if (cur_idx is $mycur(cur_col_idx)) {
    if (retval is DONE) {
      if (draw) self(draw);
      return NOTHING_TODO;
    }
  }

  if (retval is DONE) self(normal.left, 1, DONOT_DRAW);
  if (run_insert_mode) {
    if (draw) self(draw);
    buf_t **thisp = &this;
    return selfp(insert.mode, 'i', NULL);
  }

  if (IS_SPACE ($mycur(data)->bytes[$mycur(cur_col_idx)]))
    self(normal.left, 1, DONOT_DRAW);

  if (draw) self(draw);
  return DONE;
}

#define THIS_LINE_PTR_IS_AT_NTH_POS                                          \
({                                                                           \
  Ustring.encode ($my(line), $mycur(data)->bytes, $mycur(data)->num_bytes,   \
      CLEAR, $my(ftype)->tabwidth, $mycur(cur_col_idx) -                     \
      ($mycur(cur_col_idx) is (int) $mycur(data)->num_bytes and              \
       IS_MODE(INSERT_MODE) ? 1 : 0));                                       \
  int nth__ = $my(line)->cur_idx + 1;                                        \
  $my(prev_nth_ptr_pos) = nth__;                                             \
  $my(prev_num_items) = $my(line)->num_items;                                \
  do {                                                                       \
    if (0 is $mycur(data)->num_bytes or                                      \
        0 is $mycur(data)->bytes[0] or                                       \
        '\n' is $mycur(data)->bytes[0]) {                                    \
       nth__ = $my(nth_ptr_pos);                                             \
       break;                                                                \
     }                                                                       \
     if ($my(line)->num_items is 1)                                          \
       nth__ = $my(nth_ptr_pos);                                             \
     else if ($my(line)->current is $my(line)->tail)                         \
       $my(state) |= PTR_IS_AT_EOL;                                          \
     else                                                                    \
       $my(state) &= ~PTR_IS_AT_EOL;                                         \
     $my(nth_ptr_pos) = nth__;                                               \
  } while (0);                                                               \
  nth__;                                                                     \
})

static int buf_normal_up (buf_t *this, int count, int adjust_col, int draw) {
  int currow_idx = this->cur_idx;

  if (0 is currow_idx or 0 is count) return NOTHING_TODO;
  if (count > currow_idx) count = currow_idx;

  int nth = THIS_LINE_PTR_IS_AT_NTH_POS;
  int isatend = $my(state) & PTR_IS_AT_EOL;

  buf_on_blankline (this);

  currow_idx -= count;
  self(current.set, currow_idx);

  int col_pos = adjust_col ? self(adjust.col, nth, isatend) : $my(video)->first_col;
  int row = $my(video)->row_pos;
  int orig_count = count;

  if (row > count)
    while (count && row - count >= $my(dim)->first_row)
      count--;

  ifnot (adjust_col) $mycur(cur_col_idx) = 0;

  if (count) {
    if (count <= $my(video_first_row_idx)) {
      self(set.video_first_row, $my(video_first_row_idx) - count);
      $my(video)->col_pos = $my(cur_video_col) = col_pos;
    } else {
      self(set.video_first_row, currow_idx);
      $my(video)->row_pos = $my(cur_video_row) = $my(dim)->first_row;
      $my(video)->col_pos = $my(cur_video_col) = col_pos;
    }
  } else {
    $my(video)->row_pos = $my(cur_video_row) = row - orig_count;
    $my(video)->col_pos = $my(cur_video_col) = col_pos;
    if (draw) self(draw_current_row);
    return DONE;
  }

  if (draw) self(draw);
  return DONE;
}

static int buf_normal_down (buf_t *this, int count, int adjust_col, int draw) {
  int currow_idx = this->cur_idx;
  if (this->num_items - 1 is currow_idx)
    return NOTHING_TODO;

  if (count + currow_idx >= this->num_items)
    count = this->num_items - currow_idx - 1;

  int nth = THIS_LINE_PTR_IS_AT_NTH_POS;
  int isatend = $my(state) & PTR_IS_AT_EOL;

  buf_on_blankline (this);

  currow_idx += count;

  self(current.set, currow_idx);

  int col_pos = adjust_col ? self(adjust.col, nth, isatend) : $my(video)->first_col;

  int row = $my(video)->row_pos;
  int orig_count = count;

  while (count && count + row < $my(statusline_row)) count--;

  ifnot (adjust_col) $mycur(cur_col_idx) = 0;

  if (count) {
    self(set.video_first_row, $my(video_first_row_idx) + count);
    $my(video)->col_pos = $my(cur_video_col) = col_pos;
  } else {
    $my(video)->row_pos = $my(cur_video_row) = row + orig_count;
    $my(video)->col_pos = $my(cur_video_col) = col_pos;
    if (draw) self(draw_current_row);
    return DONE;
  }

  if (draw) self(draw);
  return DONE;
}

static int buf_normal_page_down (buf_t *this, int count, int draw) {
  if (this->num_items < ONE_PAGE
      or this->num_items - $my(video_first_row_idx) < ONE_PAGE + 1)
    return NOTHING_TODO;

  self(mark.set, MARK_UNNAMED);

  int nth = THIS_LINE_PTR_IS_AT_NTH_POS;
  int isatend = $my(state) & PTR_IS_AT_EOL;

  int row = $my(video)->row_pos;

  int frow = $my(video_first_row_idx);
  int currow_idx = this->cur_idx;

  while (count--) {
    frow += ONE_PAGE; currow_idx += ONE_PAGE;
    if (frow >= this->num_items or currow_idx >= this->num_items) {
      frow = this->num_items - ONE_PAGE;
      row = $my(dim->first_row) + 1;
      currow_idx = frow + 1;
      break;
      }
  }

  buf_on_blankline (this);

  self(current.set, currow_idx);
  self(set.video_first_row, frow);
  $my(video)->row_pos = $my(cur_video_row) = row;
  $my(video)->col_pos = $my(cur_video_col) = self(adjust.col, nth, isatend);
  if (draw) self(draw);
  return DONE;
}

static int buf_normal_page_up (buf_t *this, int count, int draw) {
  if ($my(video_first_row_idx) is 0 or this->num_items < ONE_PAGE)
    return NOTHING_TODO;

  self(mark.set, MARK_UNNAMED);

  int nth = THIS_LINE_PTR_IS_AT_NTH_POS;
  int isatend = $my(state) & PTR_IS_AT_EOL;

  int row = $my(video)->row_pos;
  int frow = $my(video_first_row_idx);
  int curlnr = this->cur_idx;

  while (count--) {
    frow -= ONE_PAGE;
    if (frow <= 0) {
      curlnr = 0 > frow ? 0 : curlnr - ONE_PAGE;
      row = frow < 0 ? $my(dim)->first_row : row;
      frow = 0;
      break;
      }

    curlnr -= ONE_PAGE;
  }

  buf_on_blankline (this);

  self(current.set, curlnr);
  self(set.video_first_row, frow);
  $my(video)->row_pos = $my(cur_video_row) = row;
  $my(video)->col_pos = $my(cur_video_col) = self(adjust.col, nth, isatend);

  if (draw) self(draw);
  return DONE;
}

static int buf_normal_bof (buf_t *this, int draw) {
  if (this->cur_idx is 0) return NOTHING_TODO;

  self(mark.set, MARK_UNNAMED);

  int nth = THIS_LINE_PTR_IS_AT_NTH_POS;
  int isatend = $my(state) & PTR_IS_AT_EOL;

  buf_on_blankline (this);

  self(set.video_first_row, 0);
  self(current.set, 0);

  $my(video)->row_pos = $my(cur_video_row) = $my(dim)->first_row;
  $my(video)->col_pos = $my(cur_video_col) = self(adjust.col, nth, isatend);

  if (draw) self(draw);
  return DONE;
}

static int buf_normal_eof (buf_t *this, int draw) {
  if ($my(video_first_row_idx) is this->num_items - 1) {
    ifnot (draw)
      return NOTHING_TODO;
    else
      goto do_draw;
  }

  self(mark.set, MARK_UNNAMED);

  int nth = THIS_LINE_PTR_IS_AT_NTH_POS;
  int isatend = $my(state) & PTR_IS_AT_EOL;

  buf_on_blankline (this);

  self(current.set, this->num_items - 1);

  $my(video)->col_pos = $my(cur_video_col) = self(adjust.col, nth, isatend);

  if (this->num_items < ONE_PAGE) {
    $my(video)->row_pos = $my(cur_video_row) =
        ($my(dim)->first_row + this->num_items) - 1;

    if (draw) goto do_draw;

    buf_set_draw_statusline (this);
    Cursor.set_pos ($my(term_ptr), $my(video)->row_pos, $my(video)->col_pos);
    return DONE;
  } else {
    self(set.video_first_row, this->num_items - (ONE_PAGE));
    $my(video)->row_pos = $my(cur_video_row) = $my(statusline_row) - 1;
  }

do_draw:
  if (draw) self(draw);
  return DONE;
}

static int buf_normal_goto_linenr (buf_t *this, int lnr, int draw) {
  int currow_idx = this->cur_idx;

  if (lnr <= 0 or lnr is currow_idx + 1)
    return NOTHING_TODO;

  if (lnr > this->num_items)
    lnr = this->num_items;

  self(mark.set, MARK_UNNAMED);

  if (lnr < currow_idx + 1)
    return self(normal.up, currow_idx - lnr + 1, ADJUST_COL, draw);

  return self(normal.down, lnr - currow_idx - 1, ADJUST_COL, draw);
}

static int buf_normal_replace_character_with (buf_t *this, utf8 c) {
  ed_record ($my(root), "buf_normal_replace_character_with (buf, %d)", c);

  char buf[5]; int len;
  Ustring.character (c, buf, &len);
  int clen = Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)]);
  String.replace_numbytes_at_with ($mycur(data), clen,
    $mycur(cur_col_idx), buf);

  $my(flags) |= BUF_IS_MODIFIED;
  self(draw_current_row);
  return DONE;
}

static int buf_normal_replace_character (buf_t *this) {
  if ($mycur(data)->num_bytes is 0) return NOTHING_TODO;

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);
  self(undo.push, Action);

  utf8 c = Input.getkey (STDIN_FILENO);

  return self(normal.replace_character_with, c);
}

static int buf_normal_delete_eol (buf_t *this, int regidx, int draw) {
  int clen = Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)]);
  if ($mycur(data)->num_bytes is 0)
    // or $mycur(cur_col_idx) is (int) $mycur(data)->num_bytes - clen)
    return NOTHING_TODO;

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);
  self(undo.push, Action);

  int len = $mycur(data)->num_bytes - $mycur(cur_col_idx);
  char buf[len + 1];
  Cstring.cp (buf, len + 1, $mycur(data)->bytes + $mycur(cur_col_idx), len);

  ed_reg_set_with ($my(root), regidx, CHARWISE, buf, 0);

  String.delete_numbytes_at ($mycur(data),
     $mycur(data)->num_bytes - $mycur(cur_col_idx), $mycur(cur_col_idx));

  if ($mycur(cur_col_idx) isnot 0)
    $mycur(cur_col_idx) -= clen;

  if ($mycur(cur_col_idx) is 0)
    $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;
  else
    if (0 is $mycur(first_col_idx))
      $my(video)->col_pos = $my(cur_video_col) = $my(video)->col_pos - 1;
    else {
      if (0 is $mycur(first_col_idx) - clen)
        $mycur(first_col_idx) -= clen;
      else
        $my(video)->col_pos = $my(cur_video_col) = $my(video)->col_pos - 1;
    }

  $my(flags) |= BUF_IS_MODIFIED;
  if (draw) self(draw_current_row);
  return DONE;
}

static int buf_insert_new_line (buf_t **thisp, utf8 com) {
  buf_t *this = *thisp;
  Action_t *Action = self(Action.new);
  action_t *action = self(action.new);
  undo_set (action, INSERT_LINE);

  int new_idx = this->cur_idx + ('o' is com ? 1 : -1);
  int currow_idx = this->cur_idx;

  if ('o' is com) {
    self(current.append_with, " ");
    String.clear ($mycur(data));
    action->idx = this->cur_idx;
    this->current = this->current->prev;
    this->cur_idx--;
    self(normal.down, 1, DONOT_ADJUST_COL, DONOT_DRAW);
  } else {
    self(current.prepend_with, " ");
    String.clear ($mycur(data));
    action->idx = this->cur_idx;

    this->current = this->current->next;
    this->cur_idx++;
    if ($my(video)->row_pos isnot $my(dim)->first_row)
      $my(video)->row_pos = $my(cur_video_row) = $my(video)->row_pos + 1;

    self(normal.up, 1, ADJUST_COL, DONOT_DRAW);
  }

  ListStackPush (Action, action);
  self(undo.push, Action);

  if (currow_idx > new_idx) {
    int t = new_idx;
    new_idx = currow_idx;
    currow_idx = t;
  }

  self(adjust.marks, INSERT_LINE, currow_idx, new_idx);

  $my(flags) |= BUF_IS_MODIFIED;
  self(draw);
  return  selfp(insert.mode, com, NULL);
}

static int buf_normal_join (buf_t *this, int draw) {
  if (this->num_items is 0 or this->num_items - 1 is this->cur_idx)
    return NOTHING_TODO;

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);

  row_t *row = buf_current_pop_next (this);

  self(Action.set_with, Action, DELETE_LINE, this->cur_idx + 1,
      row->data->bytes, row->data->num_bytes);
  self(undo.push, Action);

  if (row->data->num_bytes isnot 0) {
    RM_TRAILING_NEW_LINE;
    char *sp = row->data->bytes;
    if (*sp is ' ') { /* MAYBE string_trim_beg() */
      while (*++sp and *sp is ' '); /* at least a space */
      String.delete_numbytes_at (row->data, (sp - row->data->bytes) - 1, 1);
    }
    String.append_with ($mycur(data), row->data->bytes);
  }

  self(adjust.marks, DELETE_LINE, this->cur_idx, this->cur_idx + 1);
  $my(flags) |= BUF_IS_MODIFIED;
  self(release.row, row);
  if (draw) self(draw);
  return DONE;
}

static int buf_normal_delete (buf_t *this, int count, int regidx, int draw) {
  ifnot ($mycur(data)->num_bytes) return NOTHING_TODO;

  Action_t *Action = NULL;
  action_t *action = NULL;

  int is_norm_mode = IS_MODE (NORMAL_MODE);
  int perfom_undo = is_norm_mode or IS_MODE (VISUAL_MODE_CW) or IS_MODE (VISUAL_MODE_BW);

  if (perfom_undo) {
    action = self(action.new_with, REPLACE_LINE, this->cur_idx,
      $mycur(data)->bytes, $mycur(data)->num_bytes);
  }

  int clen = Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)]);
  int len = 0;

  int calc_width = perfom_undo;
  int width = 0;

  while (count--) {
    len += clen;
    if ($mycur(cur_col_idx) + clen is (int) $mycur(data)->num_bytes)
      break;

    if (calc_width) {
      int lwidth = Ustring.width ($mycur(data)->bytes + $mycur(cur_col_idx),
        $my(ftype)->tabwidth);
      if (lwidth > 1) width += lwidth - 1;
    }

    clen = Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx) + len]);
  }

  char buf[len + 1];
  Cstring.cp (buf, len + 1, $mycur(data)->bytes + $mycur(cur_col_idx), len);
  String.delete_numbytes_at ($mycur(data), len, $mycur(cur_col_idx));

  if (calc_width and 0 isnot width)
    $my(video)->col_pos = $my(cur_video_col) =
      $my(cur_video_col) - width;

  if ($mycur(cur_col_idx) is (int) $mycur(data)->num_bytes + (0 is is_norm_mode))
    self(normal.left, 1, DONOT_DRAW);
  else {
    int lwidth = Ustring.width ($mycur(data)->bytes + $mycur(cur_col_idx),
        $my(ftype)->tabwidth);
    if (lwidth > 1) {
      if (lwidth + $my(cur_video_col) > $my(dim)->num_cols) {
        $mycur(first_col_idx) = $mycur(cur_col_idx);
        $my(video)->col_pos = $my(cur_video_col) = lwidth;
      } else
        $my(video)->col_pos = $my(cur_video_col) =
          $my(cur_video_col) + lwidth - 1;
    }
  }

  if (perfom_undo) {
    action->cur_col_idx = $mycur(cur_col_idx);
    action->first_col_idx = $mycur(first_col_idx);

    Action = self(Action.new);
    ListStackPush (Action, action);
    self(undo.push, Action);
    ed_reg_set_with ($my(root), regidx, CHARWISE, buf, 0);
  }

  $my(flags) |= BUF_IS_MODIFIED;
  if (draw) self(draw_current_row);
  return DONE;
}

static int buf_inc_dec_char (buf_t *this, int count, utf8 com) {
  utf8 c = CUR_UTF8_CODE;
  if (CTRL('a') is com)
    c += count;
  else
    c -= count;

  /* this needs accuracy, so dont give that illusion and be strict to what you know */
  if (c < ' ' or (c > 126 and c < 161) or (c > 254 and c < 902) or c > 974)
    return NOTHING_TODO;

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);
  self(undo.push, Action);

  char ch[5]; int len; Ustring.character (c, ch, &len);
  int clen = Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)]);
  String.replace_numbytes_at_with ($mycur(data), clen, $mycur(cur_col_idx), ch);
  $my(flags) |= BUF_IS_MODIFIED;
  self(draw_current_row);
  return DONE;
}

static int buf_word_math (buf_t *this, int count, utf8 com) {
  int fidx = 0;

  string_t *word = get_current_number (this, &fidx);
  if (NULL is word) return buf_inc_dec_char (this, count, com);

  int nr = 0;
  char *p = word->bytes;
  int type = *p++;

  if (type is 'd') nr = atoi (p);
  else if (type is 'o')  nr = strtol (p, NULL, 8);
  else nr = strtol (p, NULL, 16);

  if (CTRL('a') is com) nr += count;
  else nr -= count;

  char new[32]; new[0] = '\0';
  if (type is 'd')
    Cstring.itoa (nr, new, 10);
  else {
    char s[16];
    if (type is 'o')
       Cstring.itoa (nr, s, 8);
    else
       Cstring.itoa (nr, s, 16);
    snprintf (new, 32, "0%s%s", ('x' is type ? "x" : ""), s);
  }

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);
  self(undo.push, Action);

  String.replace_numbytes_at_with ($mycur(data), word->num_bytes - 1, fidx, new);
  $my(flags) |= BUF_IS_MODIFIED;
  self(draw_current_row);

  String.release (word);
  return DONE;
}

static int ed_complete_word_callback (menu_t *menu) {
  buf_t *this = menu->user_data[MENU_BUF_IDX];

  int idx = 0;
  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;

    idx = $mycur(cur_col_idx) - ($mycur(cur_col_idx) isnot 0);

    while (idx >= 0 and (
         IS_SPACE ($mycur(data)->bytes[idx]) is 0 and
         IS_CNTRL ($mycur(data)->bytes[idx]) is 0 and
         NULL is memchr (Notword, $mycur(data)->bytes[idx], Notword_len)))
      idx--;

    $my(shared_int) = idx + 1;

    idx++;

    menu->pat[0] = '\0';
    menu->patlen = 0;

    while (idx < $mycur(cur_col_idx)) {
      menu->pat[menu->patlen++] = $mycur(data)->bytes[idx];
      idx++;
    }

    while ($mycur(data)->bytes[idx]) {
      if (IS_SPACE ($mycur(data)->bytes[idx]) or
          NULL isnot memchr (Notword, $mycur(data)->bytes[idx], Notword_len))
        break;
      menu->pat[menu->patlen++] = $mycur(data)->bytes[idx];
      idx++;
    }

    menu->pat[menu->patlen] = '\0';
  } else
    Menu.release_list (menu);

  Vstring_t *words = Vstring.new ();

  row_t *row = this->head;
  idx = -1;

  while (row isnot NULL) {
    if (row->data->bytes and ++idx isnot this->cur_idx) {
      char *p = strstr (row->data->bytes, menu->pat);
      if (NULL isnot p) {
        char word[MAXLEN_WORD];
        int lidx = 0;
        while (*p and (
            IS_SPACE (*p) is 0 and
            IS_CNTRL (*p) is 0 and
            NULL is memchr (Notword, *p, Notword_len))) {
          word[lidx++] = *p++;
        }

        word[lidx] = '\0';
        Vstring.add.sort_and_uniq (words, word);
      }
    }
    row = row->next;
  }

  ifnot (words->num_items) {
    free (words);
    menu->state |= MENU_QUIT;
    return NOTHING_TODO;
  }

  menu->list = words;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  return DONE;
}

static int buf_complete_word (buf_t **thisp) {
  buf_t *this = *thisp;
  int retval = DONE;

  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = $my(root),
    .user_data_second = this,
    .video = $my(video),
    .term = $my(term_ptr),
    .first_row = $my(video)->row_pos,
    .last_row = *$my(prompt_row_ptr) - 2,
    .first_col = $my(video)->col_pos,
    .prompt_row = *$my(prompt_row_ptr),
    .state = MENU_INIT,
    .process_list_cb = ed_complete_word_callback,
    .next_key = CTRL('n'),
    .pat = NULL,
    .patlen = 0));

  if ((retval = menu->retval) is NOTHING_TODO) goto theend;

  int orig_patlen = menu->patlen;

  char *word = Menu.create (menu);

  if (word isnot NULL) {
    Action_t *Action = self(Action.new);
    self(Action.set_with_current, Action, REPLACE_LINE);
    self(undo.push, Action);

    String.delete_numbytes_at ($mycur(data), orig_patlen, $my(shared_int));
    String.insert_at_with ($mycur(data), $my(shared_int), word);
    self(normal.end_word, 1, 0, DONOT_DRAW);

    this = *thisp;
    self(normal.right, 1, DONOT_DRAW);
    $my(flags) |= BUF_IS_MODIFIED;
    self(draw_current_row);
  }

theend:
  Menu.release (menu);
  return retval;
}

static int ed_complete_line_callback (menu_t *menu) {
  buf_t *this = menu->user_data[MENU_BUF_IDX];

  int idx = 0;

  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
    menu->pat[0] = '\0';
    menu->patlen = 0;

    char *s = $mycur(data)->bytes;

    while (IS_SPACE (*s)) { s++; idx++; }
    int clen = Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)]);

    while (idx++ < $mycur(cur_col_idx) + (clen - 1))
      menu->pat[menu->patlen++] = *s++;

    menu->pat[menu->patlen] = '\0';
  } else
    Menu.release_list (menu);

  Vstring_t *lines = Vstring.new ();

  row_t *row = this->head;
  idx = 0;

  while (row isnot NULL) {
    if (row->data->bytes and idx++ isnot this->cur_idx) {
      char *p = strstr (row->data->bytes, menu->pat);
      if (NULL isnot p) {
        if (p is row->data->bytes or ({
            int i = p - row->data->bytes;
            int found = 1;
            while (i--) {
              if (IS_SPACE (row->data->bytes[i]) is 0) {
                found = 0; break;
              }
            }
             found;}))
           Vstring.add.sort_and_uniq (lines, row->data->bytes);
       }
    }
    row = row->next;
  }

  ifnot (lines->num_items) {
    free (lines);
    menu->state |= MENU_QUIT;
    return NOTHING_TODO;
  }

  menu->list = lines;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  return DONE;
}

static int buf_complete_line (buf_t *this) {
  int retval = DONE;

  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = $my(root),
    .user_data_second = this,
    .video = $my(video),
    .term = $my(term_ptr),
    .first_row = $my(video)->row_pos,
    .last_row = *$my(prompt_row_ptr) - 2,
    .first_col = $my(video)->col_pos,
    .prompt_row = *$my(prompt_row_ptr),
    .state = MENU_INIT,
    .process_list_cb = ed_complete_line_callback,
    .next_key = CTRL('l'),
    .pat = NULL,
    .patlen = 0));

  if ((retval = menu->retval) is NOTHING_TODO) goto theend;

  char *line = Menu.create (menu);

  if (line isnot NULL) {
    Action_t *Action = self(Action.new);
    self(Action.set_with_current, Action, REPLACE_LINE);
    self(undo.push, Action);

    String.clear ($mycur(data));
    String.append_with ($mycur(data), line);
    $my(flags) |= BUF_IS_MODIFIED;
    self(draw_current_row);
  }

theend:
  Menu.release (menu);
  return retval;
}

static int buf_win_only (buf_t *this) {
  if (1 is $myparents(num_frames)) return NOTHING_TODO;

  for (int i = $myparents(num_frames) - 1; i > 0; i--)
    win_delete_frame ($my(parent), i);

  buf_t *it = $my(parent)->head;
  int idx = 0;
  while (it isnot NULL) {
    if (idx++ isnot $my(parent)->cur_idx)
      $from(it, flags) &= ~BUF_IS_VISIBLE;
    else {
      $my(video)->row_pos = $my(cur_video_row);
      $my(video)->col_pos = $my(cur_video_col);
    }

    it = it->next;
  }

  Win.draw ($my(parent));
  return DONE;
}

static int ed_complete_word_actions_cb (menu_t *menu) {
  buf_t *this = menu->user_data[MENU_BUF_IDX];

  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  ifnot (menu->patlen)
    menu->list = Vstring.dup ($myroots(word_actions));
  else {
    menu->list = Vstring.new ();
    vstring_t *it = $myroots(word_actions)->head;
    while (it) {
      if (Cstring.eq_n (it->data->bytes, menu->pat, menu->patlen))
        Vstring.add.sort_and_uniq (menu->list, it->data->bytes);
      it = it->next;
    }
  }

  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  return DONE;
}

static utf8 buf_complete_word_actions (buf_t *this, char *action) {
  int retval = DONE;
  utf8 c = ESCAPE_KEY;

  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = $my(root),
    .user_data_second = this,
    .video = $my(video),
    .term = $my(term_ptr),
    .first_row = $my(video)->row_pos,
    .last_row = *$my(prompt_row_ptr) - 2,
    .first_col = $my(video)->col_pos,
    .prompt_row = *$my(prompt_row_ptr),
    .state = MENU_INIT,
    .process_list_cb = ed_complete_word_actions_cb,
    .next_key = 'W',
    .pat = NULL,
    .patlen = 0));

  if ((retval = menu->retval) is NOTHING_TODO) goto theend;

  char *item = Menu.create (menu);

  if (item isnot NULL) {
    c = *item;
    char *tmp = item;
    int i = 0;
    for (; i < MAXLEN_WORD_ACTION - 1 and *tmp; i++)
      action[i] = *tmp++;
    action[i] = '\0';
  }

theend:                                     /* avoid list (de|re)allocation */
  //menu->state &= ~MENU_LIST_IS_ALLOCATED; /* list will be free'd at ed_release() */
  Menu.release (menu);
  return c;
}

static int buf_normal_handle_W (buf_t **thisp) {
  buf_t *this = *thisp;
  char action[MAXLEN_WORD_ACTION];
  utf8 c = buf_complete_word_actions (this, action);
  if (c is ESCAPE_KEY) return NOTHING_TODO;

  char word[MAXLEN_WORD]; int fidx, lidx;
  if (NULL is buf_get_current_word (this, word, Notword, Notword_len, &fidx, &lidx))
    return NOTHING_TODO;

  int retval = NOTHING_TODO;
  for (int i = 0; i < $myroots(word_actions_chars_len); i++)
    if (c is $myroots(word_actions_chars)[i]) {
      bufiter_t *it = self(iter.new, this->cur_idx);
      retval = $myroots(word_actions_cb)[i] (thisp, fidx, lidx, it, word, c, action);
      self(iter.release, it);
    }

  return retval;
}

static int ed_actions_token_cb (Vstring_t *str, char *tok, void *menu_o) {
  menu_t *menu = (menu_t *) menu_o;
  if (menu->patlen)
    ifnot (Cstring.eq_n (tok, menu->pat, menu->patlen)) return OK;

  Vstring.current.append_with (str, tok);
  return OK;
}

static int ed_complete_line_mode_actions_cb (menu_t *menu) {
  buf_t *this = menu->user_data[MENU_BUF_IDX];

  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  Vstring_t *items;
  items = cstring_chop ($myroots(line_mode_actions), '\n', NULL, ed_actions_token_cb, menu);
  menu->list = items;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  return DONE;
}

static utf8 buf_complete_line_mode_actions (buf_t *this, char *action) {
  int retval = DONE;
  utf8 c = ESCAPE_KEY;

  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = $my(root),
    .user_data_second = this,
    .video = $my(video),
    .term = $my(term_ptr),
    .first_row = $my(video)->row_pos,
    .last_row = *$my(prompt_row_ptr) - 2,
    .first_col = $my(video)->col_pos,
    .prompt_row = *$my(prompt_row_ptr),
    .state = MENU_INIT,
    .process_list_cb = ed_complete_line_mode_actions_cb,
    .return_if_one_item = ($myroots(line_mode_chars_len) isnot 1),
    .next_key = 'L',
    .pat = NULL,
    .patlen = 0));

  if ((retval = menu->retval) is NOTHING_TODO) goto theend;

  char *item = Menu.create (menu);

  if (item isnot NULL) {
    c = *item;
    char *tmp = item;
    int i = 0;
    for (; i < MAXLEN_WORD_ACTION - 1 and *tmp; i++)
      action[i] = *tmp++;
    action[i] = '\0';
  }

theend:
  Menu.release (menu);
  return c;
}

static int buf_normal_handle_L (buf_t **thisp) {
  buf_t *this = *thisp;

  ifnot ($myroots(line_mode_chars_len)) return NOTHING_TODO;
  char action[MAXLEN_WORD_ACTION];
  utf8 c = buf_complete_line_mode_actions (this, action);

  if (c is ESCAPE_KEY) return NOTHING_TODO;

  int retval = NOTHING_TODO;

  for (int i = 0; i < $myroots(num_line_mode_cbs); i++) {
    if (NULL is $myroots(line_mode_cbs)[i]) continue;
    retval = $myroots(line_mode_cbs)[i] (thisp, c, action,
      $mycur(data)->bytes, $mycur(data)->num_bytes);
    if (retval isnot NO_CALLBACK_FUNCTION) break;
  }

  retval = (retval is NO_CALLBACK_FUNCTION ? NOTHING_TODO : retval);
  return retval;
}

static int ed_complete_file_actions_cb (menu_t *menu) {
  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  buf_t *this = menu->user_data[MENU_BUF_IDX];

  Vstring_t *items;
  items = cstring_chop ($myroots(file_mode_actions), '\n', NULL, ed_actions_token_cb, menu);
  menu->list = items;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  return DONE;
}

static utf8 buf_complete_file_actions (buf_t *this, char *action) {
  int retval = DONE;
  utf8 c = ESCAPE_KEY;

  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = $my(root),
    .user_data_second = this,
    .video = $my(video),
    .term = $my(term_ptr),
    .first_row = $my(video)->row_pos,
    .last_row = *$my(prompt_row_ptr) - 2,
    .first_col = $my(video)->col_pos,
    .prompt_row = *$my(prompt_row_ptr),
    .state = MENU_INIT,
    .process_list_cb = ed_complete_file_actions_cb,
    .return_if_one_item = ($myroots(file_mode_chars_len) isnot 1),
    .next_key = 'F',
    .pat = NULL,
    .patlen = 0));

  if ((retval = menu->retval) is NOTHING_TODO) goto theend;

  char *item = Menu.create (menu);

  if (item isnot NULL) {
    c = *item;
    char *tmp = item;
    int i = 0;
    for (; i < MAXLEN_WORD_ACTION - 1 and *tmp; i++)
      action[i] = *tmp++;
    action[i] = '\0';
  }

theend:
  Menu.release (menu);
  return c;
}

static int buf_normal_handle_F (buf_t **thisp) {
  buf_t *this = *thisp;

  ifnot ($myroots(file_mode_chars_len)) return NOTHING_TODO;
  char action[MAXLEN_WORD_ACTION];
  utf8 c = buf_complete_file_actions (this, action);

  if (c is ESCAPE_KEY) return NOTHING_TODO;

  int retval = NOTHING_TODO;

  for (int i = 0; i < $myroots(num_file_mode_cbs); i++) {
    if (NULL is $myroots(file_mode_cbs)[i]) continue;
    retval = $myroots(file_mode_cbs)[i] (thisp, c, action);
    if (retval isnot NO_CALLBACK_FUNCTION) break;
  }

  retval = (retval is NO_CALLBACK_FUNCTION ? NOTHING_TODO : retval);
  return retval;
}

static int buf_normal_handle_ctrl_w (buf_t **thisp) {
  buf_t *this = *thisp;

  utf8 c = Input.getkey (STDIN_FILENO);

  switch (c) {
    case CTRL('w'):
    case ARROW_DOWN_KEY:
    case 'j':
    case 'w':
       {
         int frame = WIN_CUR_FRAME($my(parent)) + 1;
         if (frame > WIN_LAST_FRAME($my(parent))) frame = FIRST_FRAME;
         this = Win.frame.change ($my(parent), frame, DRAW);
         if (NULL isnot this) {
           *thisp = this;
           return DONE;
         }
       }
       break;

    case ARROW_UP_KEY:
    case 'k':
       {
         int frame = WIN_CUR_FRAME($my(parent)) - 1;
         if (frame < FIRST_FRAME) frame = WIN_LAST_FRAME($my(parent));
         this = Win.frame.change ($my(parent), frame, DRAW);
         if (NULL isnot this) {
           *thisp = this;
           return DONE;
         }
       }
       break;

    case 'o':
      return buf_win_only (this);

    case 'n':
      return buf_enew_fname (thisp, UNNAMED);

    case 's':
      return buf_split (thisp, UNNAMED);

    case 'l':
    case ARROW_RIGHT_KEY:
      return Ed.win.change ($my(root), thisp, VED_COM_WIN_CHANGE_NEXT,
          NULL, NO_OPTION, NO_FORCE);

    case 'h':
    case ARROW_LEFT_KEY:
      return Ed.win.change ($my(root), thisp, VED_COM_WIN_CHANGE_PREV,
          NULL, NO_OPTION, NO_FORCE);

    case '`':
      return Ed.win.change ($my(root), thisp, VED_COM_WIN_CHANGE_PREV_FOCUSED,
          NULL, NO_OPTION, NO_FORCE);

    default:
      break;
  }

  return NOTHING_TODO;
}


static int buf_normal_handle_g (buf_t **thisp, int count) {
  buf_t *this = *thisp;

  if (1 isnot count) return self(normal.goto_linenr, count, DRAW);

  utf8 c = Input.getkey (STDIN_FILENO);
  switch (c) {
    case 'g':
      return self(normal.bof, DRAW);

    case 'f':
      return buf_open_fname_under_cursor (thisp, Cstring.eq ($my(fname), VED_MSG_BUF)
         ? 0 : AT_CURRENT_FRAME, OPEN_FILE_IFNOT_EXISTS, DONOT_REOPEN_FILE_IF_LOADED,
         DRAW);

    case 'v':
      $my(state) |= BUF_LW_RESELECT;
      return selfp(normal.visual.lw);

    default:
      for (int i = 0; i < $myroots(num_on_normal_g_cbs); i++) {
        int retval = $myroots(on_normal_g_cbs)[i] (thisp, c);
        if (retval isnot NO_CALLBACK_FUNCTION) return retval;
      }

      return NOTHING_TODO;
  }

  return self(normal.goto_linenr, count, DRAW);
}

static int buf_normal_handle_G (buf_t *this, int count) {
  if (1 isnot count)
    return self(normal.goto_linenr, count, DRAW);

  return self(normal.eof, DRAW);
}

static int buf_normal_handle_comma (buf_t **thisp) {
  buf_t *this = *thisp;
  utf8 c = Input.getkey (STDIN_FILENO);
  switch (c) {
    case 'n':
      return buf_change (thisp, VED_COM_BUF_CHANGE_NEXT);

    case 'm':
      return buf_change (thisp, VED_COM_BUF_CHANGE_PREV);

    case ',':
      return buf_change (thisp, VED_COM_BUF_CHANGE_PREV_FOCUSED);

    case '.':
      return Ed.win.change ($my(root), thisp, VED_COM_WIN_CHANGE_PREV_FOCUSED,
          NULL, NO_OPTION, NO_FORCE);

    case '/':
      return Ed.win.change ($my(root), thisp, VED_COM_WIN_CHANGE_NEXT,
          NULL, NO_OPTION, NO_FORCE);

    case ';':
      $myroots(state) |= ED_NEXT;
      return EXIT_THIS;

    case '\'':
      $myroots(state) |= ED_PREV;
      return EXIT_THIS;

    case 'l':
      $myroots(state) |= ED_PREV_FOCUSED;
      return EXIT_THIS;
  }

  return NOTHING_TODO;
}

static int buf_handle_ctrl_x (buf_t **thisp) {
  buf_t *this = *thisp;
  utf8 c = Input.getkey (STDIN_FILENO);
  switch (c) {
    case 'l':
    case CTRL('l'):
      return buf_complete_line (this);

    case 'f':
    case CTRL('f'):
      return buf_insert_complete_filename (thisp);
  }

  return NOTHING_TODO;
}

static int buf_delete_word (buf_t *this, int regidx) {
  ifnot ($mycur(data)->num_bytes) return NOTHING_TODO;

  char word[MAXLEN_WORD]; int fidx, lidx;
  if (NULL is buf_get_current_word (this, word, Notword, Notword_len, &fidx, &lidx))
    return NOTHING_TODO;

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);
  self(undo.push, Action);

  int len = $mycur(cur_col_idx) - fidx;
  char buf[len + 1];
  Cstring.cp (buf, len + 1, $mycur(data)->bytes + fidx, len);

  self(normal.left, Ustring.char_num (buf, len), DONOT_DRAW);
  String.delete_numbytes_at ($mycur(data), bytelen (word), fidx);

  ed_reg_set_with ($my(root), regidx, CHARWISE, word, 0);

  $my(flags) |= BUF_IS_MODIFIED;
  self(draw_current_row);
  return DONE;
}

static int buf_delete_line (buf_t *this, int count, int regidx) {
  if (count > this->num_items - this->cur_idx)
    count = this->num_items - this->cur_idx;

  int currow_idx = this->cur_idx;

  int nth = THIS_LINE_PTR_IS_AT_NTH_POS;
  int isatend = $my(state) & PTR_IS_AT_EOL;

  Action_t *Action = self(Action.new);

  int ridx = regidx;
  Reg_t *rg = NULL;
  int reg_append = ('A' <= REGISTERS[regidx] and REGISTERS[regidx] <= 'Z');
                                       /* optimization for large buffers */
  int perfom_reg = (regidx isnot REG_UNNAMED or count < ($my(dim)->num_rows * 20)) and
                    regidx isnot REG_INTERNAL;

  if (perfom_reg) {
    if (regidx isnot REG_STAR and regidx isnot REG_PLUS) {
      if (reg_append) {
        ridx = REG_INTERNAL;
        rg = ed_reg_new ($my(root), ridx);
      } else {
        rg = ed_reg_new ($my(root), regidx);
      }
    }
  }

  int fidx = this->cur_idx;
  int lidx = fidx + count - 1;

  for (int idx = fidx; idx <= lidx; idx++) {
    action_t *action = self(action.new_with, DELETE_LINE, this->cur_idx,
        $mycur(data)->bytes, $mycur(data)->num_bytes);
    ListStackPush (Action, action);

    /* with large buffers, this really slowdown a lot the operation */
    if (perfom_reg) {
      rg = ed_reg_push_with (
        $my(root), ridx, LINEWISE, $mycur(data)->bytes, REVERSE_ORDER);
      rg->cur_col_idx = $mycur(cur_col_idx);
      rg->first_col_idx = $mycur(first_col_idx);
      rg->col_pos = $my(cur_video_col);
    }

    if (NULL is self(current.delete)) break;
  }

  if (this->num_items is 0) buf_on_no_length (this);

  $my(video)->col_pos = $my(cur_video_col) = self(adjust.col, nth, isatend);
  self(adjust.marks, DELETE_LINE, fidx, lidx);

  if (this->num_items is 1 and $my(cur_video_row) isnot $my(dim)->first_row)
    $my(video)->row_pos = $my(cur_video_row) = $my(dim)->first_row;

  if (this->cur_idx is currow_idx) {
    if ($my(video_first_row_idx) > fidx)
      $my(video_first_row_idx) = this->cur_idx;
  } else {
    if ($my(video_first_row_idx) isnot this->cur_idx)
      self(adjust.view);
  }

  $my(flags) |= BUF_IS_MODIFIED;
  if (perfom_reg) {
    if (reg_append) {
      ed_reg_append ($my(root), regidx, LINEWISE, rg->head);
      $myroots(regs)[REG_INTERNAL].head = NULL;
    }

    MSG("deleted into register [%c]%s", REGISTERS[regidx],
        reg_append ? " [appended]" : "");
  }

  self(draw);

  self(undo.push, Action);
  return DONE;
}

static int buf_normal_change_case (buf_t *this) {
  ed_record ($my(root), "buf_normal_change_case (buf)");

  utf8 c = CUR_UTF8_CODE;
  char buf[5]; int len;
  Action_t *Action;

  if ('a' <= c and c <= 'z')
    buf[0] = c - ('a' - 'A');
  else if ('A' <= c and c <= 'Z')
    buf[0] = c + ('a' - 'A');
  else {
    char *p = Cstring.byte.in_str (CASE_A, $mycur(data)->bytes[$mycur(cur_col_idx)]);
    if (NULL is p) {
      if (c > 0x80) {
        utf8 new;
        if (Ustring.is_lower (c))
          new = Ustring.to_upper (c);
        else
          new = Ustring.to_lower (c);

        if (new is c) goto theend;
        Ustring.character (new, buf, &len);
        goto setaction;
      } else
        goto theend;
    }

    buf[0] = CASE_B[p-CASE_A];
  }

  buf[1] = '\0';

setaction:
  Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);
  self(undo.push, Action);

  String.replace_numbytes_at_with ($mycur(data), bytelen (buf),
      $mycur(cur_col_idx), buf);
  self(draw_current_row);

theend:
  $my(flags) |= BUF_IS_MODIFIED;
  self(normal.right, 1, DRAW);
  return DONE;
}

static int buf_indent (buf_t *this, int count, utf8 com) {
  if (com is '<') {
    if ($mycur(data)->num_bytes is 0 or (IS_SPACE ($mycur(data)->bytes[0]) is 0))
      return NOTHING_TODO;
  } else
    if (Cstring.eq (VISUAL_MODE_LW, $my(mode)))
      ifnot ($mycur(data)->num_bytes) return NOTHING_TODO;

  Action_t *Action = self(Action.new);
  action_t *action = self(action.new_with, REPLACE_LINE, this->cur_idx,
      $mycur(data)->bytes, $mycur(data)->num_bytes);

  int shiftwidth = ($my(ftype)->shiftwidth ? $my(ftype)->shiftwidth : DEFAULT_SHIFTWIDTH);

  if ((shiftwidth * count) > $my(dim)->num_cols)
    count = $my(dim)->num_cols / shiftwidth;

  if (com is '>') {
    int len = shiftwidth * count;
    char s[len + 1];
    int i;
    for (i = 0; i < len; i++) { s[i] = ' '; } s[i] = '\0';
    String.prepend_with ($mycur(data), s);
  } else {
    char *s = $mycur (data)->bytes;
    int i = 0;
    while (IS_SPACE (*s) and i < shiftwidth * count) {
      s++; i++;
    }

    String.clear ($mycur(data));
    String.append_with ($mycur(data), s);
    if ($mycur(cur_col_idx) >= (int) $mycur(data)->num_bytes) {
      $mycur(cur_col_idx) = $mycur(first_col_idx) = 0;
      $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;
      self(normal.eol, DRAW);
    }
  }

  ListStackPush (Action, action);
  self(undo.push, Action);
  $my(flags) |= BUF_IS_MODIFIED;
  self(draw_current_row);
  return DONE;
}

static int buf_normal_Yank (buf_t *this, int count, int regidx) {
  if (count > this->num_items - this->cur_idx)
    count = this->num_items - this->cur_idx;

  int currow_idx = this->cur_idx;

  int ridx = regidx;
  Reg_t *rg = NULL;
  int reg_append = ('A' <= REGISTERS[regidx] and REGISTERS[regidx] <= 'Z');

  if (regidx isnot REG_STAR and regidx isnot REG_PLUS) {
    if (reg_append) {
      ridx = REG_INTERNAL;
      rg = ed_reg_new ($my(root), ridx);
    } else {
      rg = ed_reg_new ($my(root), regidx);
    }
  }

  String.clear ($my(shared_str));

  for (int i = 0; i < count; i++) {
    self(current.set, (currow_idx + count - 1) - i);
    if (regidx isnot REG_STAR and regidx isnot REG_PLUS) {
      rg = ed_reg_push_with (
          $my(root), ridx, LINEWISE, $mycur(data)->bytes, DEFAULT_ORDER);
      rg->cur_col_idx = $mycur(cur_col_idx);
      rg->first_col_idx = $mycur(first_col_idx);
      rg->col_pos = $my(cur_video_col);
    } else {
      String.prepend_with_fmt ($my(shared_str), "%s\n", $mycur(data)->bytes);
    }
  }

  if (regidx is REG_STAR or regidx is REG_PLUS)
    ed_selection_to_X ($my(root), $my(shared_str)->bytes, $my(shared_str)->num_bytes,
        (REG_STAR is regidx ? X_PRIMARY : X_CLIPBOARD));

  if (reg_append) {
    ed_reg_append ($my(root), regidx, LINEWISE, rg->head);
    $myroots(regs)[REG_INTERNAL].head = NULL;
  }

  MSG("yanked [linewise] into register [%c]%s", REGISTERS[regidx],
      reg_append ? " [appended]" : "");
  return DONE;
}

static int buf_normal_yank (buf_t *this, int count, int regidx) {
  if (count > (int) $mycur(data)->num_bytes - $mycur(cur_col_idx))
    count = $mycur(data)->num_bytes - $mycur(cur_col_idx);

  char buf[(count * 4) + 1];
  char *bytes = $mycur(data)->bytes + $mycur(cur_col_idx);

  int bufidx = 0;
  for (int i = 0; i < count and *bytes; i++) {
    int clen = Ustring.charlen ((uchar) *bytes);
    for (int j = 0; j < clen; j++) buf[bufidx + j] = bytes[j];
    bufidx += clen;
    bytes += clen;
    i += clen - 1;
  }

  buf[bufidx] = '\0';

  if (regidx isnot REG_STAR and regidx isnot REG_PLUS) {
    ed_reg_set_with ($my(root), regidx, CHARWISE, buf, 0);
  } else
    ed_selection_to_X ($my(root), buf, bufidx, (REG_STAR is regidx
        ? X_PRIMARY : X_CLIPBOARD));

  MSG("yanked [charwise] into register [%c]", REGISTERS[regidx]);
  return DONE;
}

static int buf_normal_put (buf_t *this, int regidx, utf8 com) {
  if (ERROR is ed_reg_special_set ($my(root), this, regidx))
    return NOTHING_TODO;

  Reg_t *rg = &$my(regs)[regidx];
  reg_t *reg = rg->head;

  if (NULL is reg) return NOTHING_TODO;

  Action_t *Action = self(Action.new);

  row_t *currow = this->current;
  int currow_idx = this->cur_idx;

  Reg_t tmprg;  // the compiler complains otherwise
  Reg_t *revrg = NULL;

  if (com is 'P') {
    /* reverse */
    revrg = &tmprg;
    revrg->head = NULL;
    while (reg) {
      reg_t *t = reg->next;
      ListStackPush (revrg, reg);
      reg = t;
    }

    reg = revrg->head;
  }

  int linewise_num = 0;

  while (reg isnot NULL) {
    action_t *action = self(action.new);
    if (rg->type is LINEWISE) {
      row_t *row = self(row.new_with, reg->data->bytes);
      undo_set (action, INSERT_LINE);
      linewise_num++;

      if ('p' is com)
        DListAppendCurrent (this, row);
      else
        DListPrependCurrent (this, row);

      action->idx = this->cur_idx;
    } else {
      undo_set (action, REPLACE_LINE);
      action->idx = this->cur_idx;
      action->bytes = Cstring.dup ($mycur(data)->bytes, $mycur(data)->num_bytes);
      String.insert_at_with ($mycur(data), $mycur(cur_col_idx) +
        (('P' is com or 0 is $mycur(data)->num_bytes) ? 0 :
           Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)])), reg->data->bytes);
    }

    ListStackPush (Action, action);

    reg = reg->next;
  }

  if (com is 'P') {
    /* reverse again */
    reg = revrg->head;
    rg->head = NULL;
    while (reg) {
      reg_t *t = reg->next;
      ListStackPush (rg, reg);
      reg = t;
    }
  }

  self(undo.push, Action);

  if (rg->type is LINEWISE) {
    self(adjust.marks, INSERT_LINE, this->cur_idx, this->cur_idx + linewise_num);

    if ('p' is com) {
      this->current = currow;
      this->cur_idx = currow_idx;
    } else {
      $mycur(cur_col_idx) = rg->cur_col_idx;
      $mycur(first_col_idx) = rg->first_col_idx;
      $my(video)->col_pos = $my(cur_video_col) = rg->col_pos;
    }
  }

  $my(flags) |= BUF_IS_MODIFIED;
  self(draw);
  return DONE;
}

static int buf_delete_inner (buf_t *this, utf8 c, int regidx) {
  ifnot ($mycur(data)->num_bytes) return NOTHING_TODO;

  Ustring.encode ($my(line), $mycur(data)->bytes, $mycur(data)->num_bytes,
        CLEAR, $my(ftype)->tabwidth, $mycur(cur_col_idx));

  ustring_t *it = $my(line)->current;

  int cur_idx = $mycur(cur_col_idx);
  int fidx = cur_idx;
  int lidx = cur_idx;
  int found = 0;
  int left = 0;

  int isc = it->code is c;
  int len = it->code isnot c;

  while (it isnot $my(line)->head) {
    it = it->prev;
    if (it->code is c) {
      found = 1;
      break;
    }
    fidx -= it->len;
    len += it->len;
    left++;
  }

  ifnot (found) {
    ifnot (isc)
      return NOTHING_TODO;

    len = 0;
    fidx = cur_idx + 1;
    left = 0;
  }

  if (found is 0 or isc is 0) {
    found = 0;
    it = $my(line)->current;

    while (it isnot $my(line)->tail) {
      it = it->next;
      if (it->code is c) {
        found = 1;
        break;
      }
      lidx += it->len;
      len += it->len;
    }

    ifnot (found) return NOTHING_TODO;
  }

  ifnot (len) return NOTHING_TODO;

  char word[len + 1];
  for (int i = 0; i < len; i++)
    word[i] = $mycur(data)->bytes[fidx + i];
  word[len] = '\0';

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);
  self(undo.push, Action);

  if (fidx > cur_idx)
    self(normal.right, 1, DONOT_DRAW);
  else
    self(normal.left, left, DONOT_DRAW);

  String.delete_numbytes_at ($mycur(data), bytelen (word), fidx);

  ed_reg_set_with ($my(root), regidx, CHARWISE, word, 0);

  $my(flags) |= BUF_IS_MODIFIED;
  self(draw_current_row);
  return DONE;
}

static int buf_normal_handle_c (buf_t **thisp, int count, int regidx) {
  (void) count;
  buf_t *this = *thisp;
  utf8 c = Input.getkey (STDIN_FILENO);
  switch (c) {
    case 'w':
      self(delete.word, regidx);
      return selfp(insert.mode, 'c', NULL);

    case 'i':
      c = Input.getkey (STDIN_FILENO);
      if (NOTHING_TODO is buf_delete_inner (this, c, regidx))
        return NOTHING_TODO;

      return selfp(insert.mode, 'c', NULL);
  }

  return NOTHING_TODO;
}

static int buf_normal_handle_d (buf_t *this, int count, int reg) {
  utf8 c = Input.getkey (STDIN_FILENO);
  switch (c) {
    case 'G':
    case END_KEY:
      count = (this->num_items) - this->cur_idx;
      break;
    case 'g':
    case HOME_KEY:
      count = this->cur_idx + 1;
      self(normal.bof, DRAW);
  }

  switch (c) {
    case 'G':
    case END_KEY:
    case 'g':
    case HOME_KEY:
    case 'd':
      return self(delete.line, count, reg);

    case 'w':
      return self(delete.word, reg);

    default:
      return NOTHING_TODO;
   }
}

static int buf_insert_change_line (buf_t *this, utf8 c, Action_t **action, int draw) {
  if ($mycur(data)->num_bytes) RM_TRAILING_NEW_LINE;

  if (c is ARROW_UP_KEY) self(normal.up, 1, ADJUST_COL, draw);
  else if (c is ARROW_DOWN_KEY) self(normal.down, 1, ADJUST_COL, draw);
  else if (c is PAGE_UP_KEY) self(normal.page_up, 1, draw);
  else if (c is PAGE_DOWN_KEY) self(normal.page_down, 1, draw);
  else {
    int isatend = $mycur(cur_col_idx) is (int) $mycur(data)->num_bytes;
    if (isatend) {
      self(current.append_with, $my(ftype)->autoindent (this, this->current)->bytes);
    } else {
      int len = ($mycur(data)->num_bytes - $mycur(cur_col_idx)) +
          $my(ftype)->autoindent (this, this->current)->num_bytes;
      char bytes[len + 1];
      Cstring.cp_fmt (bytes, len + 1, "%s%s",
        $my(ftype)->autoindent (this, this->current)->bytes,
           $mycur(data)->bytes + $mycur(cur_col_idx));

      String.clear_at ($mycur(data), $mycur(cur_col_idx));
      self(current.append_with, bytes);
    }

    this->current = this->current->prev;
    this->cur_idx--;
    self(adjust.marks, INSERT_LINE, this->cur_idx, this->cur_idx + 1);

    action_t *act = self(action.new);
    undo_set (act, INSERT_LINE);

    $my(cur_video_col) = $my(video)->col_pos = $my(video)->first_col;
    $mycur(first_col_idx) = $mycur(cur_col_idx) = 0;
    self(normal.down, 1, DONOT_ADJUST_COL, 0);
    if ($mycur(data)->num_bytes) ADD_TRAILING_NEW_LINE;
    self(normal.right, $my(shared_int) + isatend, DONOT_DRAW);

    act->idx = this->cur_idx;
    ListStackPush (*action, act);
    $my(flags) |= BUF_IS_MODIFIED;
    self(draw);
    return DONE;
  }

  action_t *act = self(action.new);
  undo_set (act, REPLACE_LINE);
  act->idx = this->cur_idx;
  act->bytes = Cstring.dup ($mycur(data)->bytes, $mycur(data)->num_bytes);
  ListStackPush (*action, act);
  $my(flags) |= BUF_IS_MODIFIED;
  return DONE;
}

static int buf_insert_character (buf_t *this, utf8 *c) {
  int has_pop_pup = 1;
  *c = BUF_GET_AS_NUMBER (has_pop_pup, $my(video)->row_pos - 1,
      $my(video)->col_pos + 1, $my(video)->num_cols, "utf8 code:");
  ifnot (*c) return NOTHING_TODO;
  if (0x07F <= *c and *c < 0x0A0) return NOTHING_TODO;
  if (*c < ' ' and '\t' isnot *c) return NOTHING_TODO;
  $my(state) |= ACCEPT_TAB_WHEN_INSERT;
  return NEWCHAR;
}

static int buf_insert_handle_ud_line_completion (buf_t *this, utf8 *c) {
  char *line;
  int len = 0;
  int up_or_down_line = *c is CTRL('y');

  if (up_or_down_line)
    ifnot (this->num_items)
      return NOTHING_TODO;
    else {
      line = $mycur(prev)->data->bytes;
      len = $mycur(prev)->data->num_bytes;
    }
  else
    if (this->cur_idx is this->num_items - 1)
      return NOTHING_TODO;
    else {
      line = $mycur(next)->data->bytes;
      len =  $mycur(next)->data->num_bytes;
    }

  if (len is 0) return NOTHING_TODO;

  int nolen = 0 is $mycur(data)->num_bytes or 0 is $mycur(data)->bytes[0]
    or '\n' is $mycur(data)->bytes[0];
  int nth = 0;
  if (nolen)
    nth = -1;
  else {
    Ustring.encode ($my(line), $mycur(data)->bytes, $mycur(data)->num_bytes,
        CLEAR, $my(ftype)->tabwidth, $mycur(cur_col_idx));
    if (0 is $mycur(data)->bytes[$mycur(cur_col_idx)])
      nth = $my(line)->num_items + 1;
    else
      nth = $my(line)->cur_idx + 1;
  }

  Ustring.encode ($my(line), line, len, CLEAR, $my(ftype)->tabwidth, 0);
  if ($my(line)->num_items < nth) return NOTHING_TODO;
  int n = 1;

  ustring_t *it = $my(line)->head;
  while (++n <= nth) it = it->next;

  ifnot (it->code) return NOTHING_TODO;

  *c = it->code;

  return NEWCHAR;
}

static int buf_insert_string (buf_t *this, char *str, size_t len, int draw) {
  String.insert_at_with ($mycur(data), $mycur(cur_col_idx), str);
  self(normal.right, Ustring.char_num (str, len), 1);
  if (draw) self(draw_current_row);
  return DONE;
}

static int buf_insert_last_insert (buf_t *this) {
  if ($my(last_insert)->num_bytes is 0) return NOTHING_TODO;
  return self(insert.string, $my(last_insert)->bytes, $my(last_insert)->num_bytes, DRAW);
}

typedef void (*draw_buf) (buf_t *);
typedef void (*draw_current_row) (buf_t *);

static void buf_draw_current_row_void (buf_t *this) {(void) this;}

#define VISUAL_ADJUST_IDXS(vis__)           \
do {                                        \
  if ((vis__).fidx > (vis__).lidx) {        \
    int t = (vis__).fidx;                   \
    (vis__).fidx = (vis__).lidx;            \
    (vis__).lidx = t;                       \
  }                                         \
} while (0)

#define VISUAL_RESTORE_STATE(vis__, mark__) \
   VISUAL_ADJUST_IDXS(vis__);               \
   state_restore (&(mark__));               \
   self(current.set, (mark__).cur_idx);     \
   this->cur_idx = (mark__).cur_idx

#define VISUAL_ADJUST_COL(idx)                                                      \
({                                                                                  \
  Ustring.encode ($my(line), $mycur(data)->bytes, $mycur(data)->num_bytes,          \
      CLEAR, $my(ftype)->tabwidth, idx);                                            \
  int nth_ = $my(line)->cur_idx + 1;                                                \
    self(normal.bol, DONOT_DRAW);                                                               \
    self(normal.right, nth_ - 1, DRAW);                                             \
})

#define VISUAL_INIT_FUN(fmode, parse_fun)                                           \
  char prev_mode[MAXLEN_MODE];                                                      \
  Cstring.cp (prev_mode, MAXLEN_MODE, $my(mode), MAXLEN_MODE - 1);                      \
  self(set.mode, (fmode));                                                          \
  buf_set_draw_topline (this);                                                      \
  mark_t mark; state_set (&mark); mark.cur_idx = this->cur_idx;                     \
  draw_buf dbuf = $self(draw);                                                      \
  draw_current_row drow = $self(draw_current_row);                                  \
  $self(draw) = buf_draw_current_row_void;                                          \
  $self(draw_current_row) = buf_draw_current_row_void;                              \
  int reg = -1;  int count = 1;  (void) count;                                      \
  $my(vis)[1] = (vis_t) {                                                           \
    .fidx = this->cur_idx, .lidx = this->cur_idx, .orig_syn_parser = $my(syn)->parse};                                  \
  $my(vis)[0] = (vis_t) {                                                           \
    .fidx = $mycur(cur_col_idx), .lidx = $mycur(cur_col_idx), .orig_syn_parser = $my(syn)->parse};\
  $my(syn)->parse = (parse_fun)

#define VIS_HNDL_CASE_REG(reg)                                              \
  case '"':                                                                 \
    if (-1 isnot (reg)) goto theend;                                        \
    (reg) = ed_reg_get_idx ($my(root), Input.getkey (STDIN_FILENO)); \
    continue

#define VIS_HNDL_CASE_INT(count)                                            \
  case '1'...'9':                                                           \
    {                                                                       \
      char intbuf[8];                                                       \
      intbuf[0] = c;                                                        \
      int idx = 1;                                                          \
      c = BUF_GET_NUMBER (intbuf, idx);                                     \
      if (idx is MAX_COUNT_DIGITS) goto handle_char;                        \
      intbuf[idx] = '\0';                                                   \
      count = atoi (intbuf);                                                \
                                                                            \
      goto handle_char;                                                     \
    }                                                                       \
                                                                            \
    continue

static char *buf_syn_parse_visual_lw (buf_t *this, char *line, int len, int idx, row_t *currow) {
  (void) len;

  if ((idx is $my(vis)[0].fidx) or
      (idx > $my(vis)[0].fidx and $my(vis)[0].fidx < $my(vis)[0].lidx and
       idx <= $my(vis)[0].lidx) or
      (idx < $my(vis)[0].fidx and $my(vis)[0].fidx > $my(vis)[0].lidx and
       idx >= $my(vis)[0].lidx) or
      (idx > $my(vis)[0].lidx and $my(vis)[0].lidx < $my(vis)[0].fidx and
       idx < $my(vis)[0].fidx)) {

    Ustring.encode ($my(line), line, len,
        CLEAR, $my(ftype)->tabwidth, currow->first_col_idx);

    ustring_t *it = $my(line)->current;

    String.replace_with_fmt ($my(shared_str), "%s%s",
         TERM_LINE_CLR_EOL, TERM_MAKE_COLOR(HL_VISUAL));

    int num = 0;

    while (num < $my(dim)->num_cols and it) {
      if (it->buf[0] is '\t') goto handle_tab;
      num += it->width;
      String.append_with ($my(shared_str), it->buf);
      goto next;

handle_tab:
      for (int i = 0; i < $my(ftype)->tabwidth and num < $my(dim)->num_cols; i++) {
        num++;
        String.append_byte ($my(shared_str), ' ');
      }

next:
      it = it->next;
    }

    String.append_with ($my(shared_str), TERM_COLOR_RESET);
    Cstring.cp (line, MAXLEN_LINE, $my(shared_str)->bytes, $my(shared_str)->num_bytes);
    return $my(shared_str)->bytes;
  }

  return $my(vis)[0].orig_syn_parser (this, line, len, idx, currow);
}

static int ed_visual_complete_actions_cb (menu_t *menu) {
  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  buf_t *this = menu->user_data[MENU_BUF_IDX];

  Vstring_t *items;
  if (IS_MODE(VISUAL_MODE_CW))
    items = cstring_chop ($myroots(cw_mode_actions), '\n', NULL, ed_actions_token_cb, menu);
  else if (IS_MODE(VISUAL_MODE_LW))
    items = cstring_chop ($myroots(lw_mode_actions), '\n', NULL, ed_actions_token_cb, menu);
  else
    items = cstring_chop (
      "insert text in front of the selected block\n"
      "change/replace selected block\n"
      "delete selected block\n", '\n', NULL,
      ed_actions_token_cb, menu);

  menu->list = items;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  return DONE;
}

static utf8 buf_visual_complete_actions (buf_t *this, char *action) {
  int retval = DONE;
  utf8 c = ESCAPE_KEY;

  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = $my(root),
    .user_data_second = this,
    .video = $my(video),
    .term = $my(term_ptr),
    .first_row = $my(video)->row_pos,
    .last_row = *$my(prompt_row_ptr) - 2,
    .first_col = $my(video)->col_pos,
    .prompt_row = *$my(prompt_row_ptr),
    .state = MENU_INIT,
    .process_list_cb = ed_visual_complete_actions_cb,
    .pat = NULL,
    .patlen = 0));

  if ((retval = menu->retval) is NOTHING_TODO) goto theend;

  char *item = Menu.create (menu);

  if (item isnot NULL) {
    c = *item;
    char *tmp = item;
    int i = 0;
    for (; i < MAXLEN_WORD_ACTION - 1 and *tmp; i++)
      action[i] = *tmp++;
    action[i] = '\0';
  }

theend:
  Menu.release (menu);
  return c;
}

static int buf_normal_visual_lw (buf_t **thisp) {
  buf_t *this = *thisp;
  VISUAL_INIT_FUN (VISUAL_MODE_LW, buf_syn_parse_visual_lw);

  utf8 c = ESCAPE_KEY;
  int goto_cb = 0;

  $my(vis)[0] = $my(vis)[1];

  if ($my(state) & BUF_LW_RESELECT) {
    $my(state) &= ~BUF_LW_RESELECT;
    if ($my(lw_vis_prev)[0].fidx isnot -1 and
        $my(lw_vis_prev)[0].lidx isnot -1 and
        $my(lw_vis_prev)[0].fidx < this->num_items and
        $my(lw_vis_prev)[0].lidx < this->num_items) {
      $my(vis)[0].fidx = $my(lw_vis_prev)[0].fidx;
      $my(vis)[0].lidx = $my(lw_vis_prev)[0].lidx;
      buf_normal_goto_linenr (this, $my(lw_vis_prev)[0].fidx + 1, DONOT_DRAW);
      state_set (&mark); mark.cur_idx = this->cur_idx;
      self(normal.goto_linenr, $my(lw_vis_prev)[0].lidx + 1, DONOT_DRAW);
    }
  }

  char vis_action[MAXLEN_WORD_ACTION];
  vis_action[0] = '\0';

  for (;;) {
    $my(vis)[0].lidx = this->cur_idx;
    dbuf (this);

    c = Input.getkey (STDIN_FILENO);

handle_char:
    switch (c) {
      case '\t':
        c = buf_visual_complete_actions (this, vis_action);
        goto handle_char;

      VIS_HNDL_CASE_REG(reg);
      VIS_HNDL_CASE_INT(count);

      case ARROW_DOWN_KEY:
        self(normal.down, 1, ADJUST_COL, DONOT_DRAW);
        continue;

      case ARROW_UP_KEY:
        self(normal.up, 1, ADJUST_COL, DONOT_DRAW);
        continue;

      case PAGE_DOWN_KEY:
        self(normal.page_down, 1, DRAW);
        continue;

      case PAGE_UP_KEY:
        self(normal.page_up, 1, DRAW);
        continue;

      case ESCAPE_KEY:
        VISUAL_RESTORE_STATE ($my(vis)[1], mark);
        goto theend;

      case HOME_KEY:
        self(normal.bof, DONOT_DRAW);
        continue;

      case 'G':
      case END_KEY:
        self(normal.eof, DONOT_DRAW);
        continue;

      case '>':
      case '<':
        if ($my(vis)[0].fidx <= $my(vis)[0].lidx) {
          VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        } else {
          VISUAL_ADJUST_IDXS($my(vis)[0]);
          self(current.set, $my(vis)[0].fidx);
          this->cur_idx = $my(vis)[0].fidx;
        }

        {
          Action_t *Action = self(Action.new);

          for (int i = $my(vis)[0].fidx; i <= $my(vis)[0].lidx; i++) {
            if (DONE is self(indent, count, c)) {
              Action_t *Laction = self(undo.pop);
              action_t *action = ListStackPop (Laction, action_t);
              ListStackPush (Action, action);
              free (Laction);
            }

            ifnot (this->current is this->tail) {
              this->current = this->current->next;
              this->cur_idx++;
            }
          }

          self(undo.push, Action);

          VISUAL_RESTORE_STATE ($my(vis)[1], mark);
          if (c is '<' and $mycur(cur_col_idx) >= (int) $mycur(data)->num_bytes - 1) {
            self(normal.noblnk);
            VISUAL_ADJUST_COL ($mycur(cur_col_idx));
          }
        }

        goto theend;

      case 'd':
      case 'y':
      case 'Y':
        if ($my(vis)[0].fidx <= $my(vis)[0].lidx) {
          VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        } else {
          VISUAL_ADJUST_IDXS($my(vis)[0]);
          self(current.set, $my(vis)[0].fidx);
          this->cur_idx = $my(vis)[0].fidx;
        }

        if (-1 is reg or c is 'Y') reg = (c is 'Y' ? REG_STAR : REG_UNNAMED);

        if (c is 'd')
          self(delete.line, $my(vis)[0].lidx - $my(vis)[0].fidx + 1, reg);
        else {
          self(normal.Yank, $my(vis)[0].lidx - $my(vis)[0].fidx + 1, reg);
          VISUAL_RESTORE_STATE ($my(vis)[1], mark);
        }

        goto theend;

      case 's':
        VISUAL_ADJUST_IDXS($my(vis)[0]);
        {
          readline_t *rl = Ed.readline.new ($my(root));
          string_t *str = String.new_with_fmt ("substitute --range=%d,%d --global -i --pat=",
              $my(vis)[0].fidx + 1, $my(vis)[0].lidx + 1);
          Readline.set.line (rl, str->bytes, str->num_bytes);
          buf_readline (&this, rl);
          String.release (str);
        }
        goto theend;

      case 'w':
        VISUAL_ADJUST_IDXS($my(vis)[0]);
        {
          readline_t *rl = Ed.readline.new ($my(root));
          string_t *str = String.new_with_fmt ("write --range=%d,%d ",
              $my(vis)[0].fidx + 1, $my(vis)[0].lidx + 1);
          Readline.set.line (rl, str->bytes, str->num_bytes);
          Readline.write_and_break (rl);
          buf_readline (&this, rl);
          String.release (str);
        }
        goto theend;

      case '+':
      case '*':
        if ($my(vis)[0].fidx <= $my(vis)[0].lidx) {
          VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        } else {
          VISUAL_ADJUST_IDXS($my(vis)[0]);
          self(current.set, $my(vis)[0].fidx);
          this->cur_idx = $my(vis)[0].fidx;
        }

        {
          row_t *row = this->current;
          Vstring_t *rows = Vstring.new ();
          for (int ii = $my(vis)[0].fidx; ii <= $my(vis)[0].lidx; ii++) {
            Vstring.current.append_with (rows, row->data->bytes);
            row = row->next;
          }
          string_t *str = Vstring.join (rows, "\n");
          ed_selection_to_X ($my(root), str->bytes, str->num_bytes,
              ('*' is c ? X_PRIMARY : X_CLIPBOARD));
          String.release (str);
          Vstring.release (rows);
          goto theend;
        }

      case '!':
        ifnot ($my(ftype)->read_from_shell) goto theend;

        if ($my(vis)[0].fidx <= $my(vis)[0].lidx) {
          VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        } else {
          VISUAL_ADJUST_IDXS($my(vis)[0]);
          self(current.set, $my(vis)[0].fidx);
          this->cur_idx = $my(vis)[0].fidx;
        }

        {
          row_t *row = this->current;
          string_t *str = String.new (128);

          int num_executed = 0;
          int flags = 0;
          int prev_line_continues = 0;

          for (int ii = $my(vis)[0].fidx; ii <= $my(vis)[0].lidx; ii++) {
            int line_continues = row->data->bytes[row->data->num_bytes - 1] is '\\';
            int is_last_line = (ii is $my(vis)[0].lidx);

            char *command = row->data->bytes;

            ifnot (prev_line_continues) {
              if (*command is '!') command++;
              while (*command is ' ') command++;
              if (*command is '!') command++;
            } else
              while (*command is ' ') command++;

            if (*command is '\0') {
              if (prev_line_continues) {
                if (line_continues)
                  goto next_shell_row;
                else
                  goto exec_command;
               }
              goto next_shell_row;
            }

            if (prev_line_continues)
              String.append_with_fmt (str, " %s", command);
            else
              String.replace_with (str, command);

            String.trim_end (str, '\\');
            String.trim_end (str, ' ');

            exec_command:
            ifnot (line_continues) {
              ifnot (str->num_bytes)
                goto next_shell_row;

              if (is_last_line)
                flags |= ED_PROC_WAIT_AT_END;

              Ed.sh.popen ($my(root), this, str->bytes, flags, NULL);
              num_executed++;
              String.clear (str);
            } else {
              if (is_last_line) {
                if (num_executed)
                  ed_tty_screen ($my(root));
                break;
              }
            }

            next_shell_row:
            prev_line_continues = line_continues;
            row = row->next;
          }

          String.release (str);
          goto theend;
        }

      case REG_SHARED_CHR:
        if ($my(vis)[0].fidx <= $my(vis)[0].lidx) {
          VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        } else {
          VISUAL_ADJUST_IDXS($my(vis)[0]);
          self(current.set, $my(vis)[0].fidx);
          this->cur_idx = $my(vis)[0].fidx;
        }

        {
          ed_reg_new ($my(root), REG_SHARED);
          row_t *row = this->current;

          for (int i = $my(vis)[0].fidx; i <= $my(vis)[0].lidx; i++) {
            ed_reg_push_with ($my(root), REG_SHARED, LINEWISE,
                row->data->bytes, REVERSE_ORDER);
            row = row->next;
          }
        }
        goto theend;

      default:
        for (int i = 0; i < $myroots(lw_mode_chars_len); i++)
          if (c is $myroots(lw_mode_chars)[i]) {
            if ($my(vis)[0].fidx <= $my(vis)[0].lidx) {
              VISUAL_RESTORE_STATE ($my(vis)[0], mark);
            } else {
              VISUAL_ADJUST_IDXS($my(vis)[0]);
              self(current.set, $my(vis)[0].fidx);
              this->cur_idx = $my(vis)[0].fidx;
            }

            goto_cb = 1;
            goto theend;

            callback:;

            row_t *row = this->current;
            Vstring_t *rows = Vstring.new ();
            for (int ii = $my(vis)[0].fidx; ii <= $my(vis)[0].lidx; ii++) {
              Vstring.current.append_with (rows, row->data->bytes);
              row = row->next;
            }

            for (int j = 0; j < $myroots(num_lw_mode_cbs); j++) {
              int retval = $myroots(lw_mode_cbs)[j] (thisp,
            	  $my(vis)[0].fidx, $my(vis)[0].lidx, rows, c, vis_action);
              if (retval isnot NO_CALLBACK_FUNCTION)
                break;
            }

            Vstring.release (rows);
            goto thereturn;
          }

        continue;
    }
  }

theend:
  $my(syn)->parse = $my(vis)[0].orig_syn_parser;
  $self(draw) = dbuf;
  $self(draw_current_row) = drow;
  self(set.mode, prev_mode);
  self(draw);

  if (goto_cb) goto callback;

thereturn:
  if (c isnot ESCAPE_KEY) {
    $my(lw_vis_prev)[0].fidx = $my(vis)[0].fidx;
    $my(lw_vis_prev)[0].lidx = $my(vis)[0].lidx;
  }

  return DONE;
}

static char *buf_syn_parse_visual_line (buf_t *this, char *line, int len, row_t *currow) {
  ifnot (len) return line;

  int fidx = $my(vis)[0].fidx;
  int lidx = $my(vis)[0].lidx;
  if (fidx > lidx)
    {int t = fidx; fidx = lidx; lidx = t;}

  Ustring.encode ($my(line), line, len,
      CLEAR, $my(ftype)->tabwidth, currow->first_col_idx);

  ustring_t *it = $my(line)->current;

  String.replace_with ($my(shared_str), TERM_LINE_CLR_EOL);

  int num = 0;
  int idx = currow->first_col_idx;

  if (idx > fidx)
    String.append_with_fmt ($my(shared_str), "%s", TERM_MAKE_COLOR(HL_VISUAL));

  while (num < $my(dim)->num_cols and it) {
    if (idx is fidx)
      String.append_with_fmt ($my(shared_str), "%s", TERM_MAKE_COLOR(HL_VISUAL));

    if (it->buf[0] is '\t') goto handle_tab;

    num += it->width; // (artifact) if a character occupies > 1 width and is the last
    // char, the code will do the wrong thing and probably will mess up the screen

    String.append_with ($my(shared_str), it->buf);

    goto next;

handle_tab:
    for (int i = 0; i < $my(ftype)->tabwidth and num < $my(dim)->num_cols; i++) {
      num++;
      String.append_byte ($my(shared_str), ' ');
    }

next:
    if (idx is lidx)
      String.append_with ($my(shared_str), TERM_COLOR_RESET);
    idx += it->len;
    it = it->next;
  }

  Cstring.cp (line, MAXLEN_LINE, $my(shared_str)->bytes, $my(shared_str)->num_bytes);
  return $my(shared_str)->bytes;
}

static char *buf_syn_parse_visual_cw (buf_t *this, char *line, int len, int idx, row_t *row) {
  (void) idx;
  return buf_syn_parse_visual_line (this, line, len, row);
}

static int buf_normal_visual_cw (buf_t **thisp) {
  buf_t *this = *thisp;

  VISUAL_INIT_FUN (VISUAL_MODE_CW, buf_syn_parse_visual_cw);

  $my(vis)[1] = $my(vis)[0];
  $my(vis)[0] = (vis_t) {.fidx = $mycur(cur_col_idx), .lidx = $mycur(cur_col_idx),
     .orig_syn_parser = $my(vis)[1].orig_syn_parser};

  int goto_cb = 0;
  char vis_action[MAXLEN_WORD_ACTION];
  vis_action[0] = '\0';

  for (;;) {
    $my(vis)[0].lidx = $mycur(cur_col_idx);
    drow (this);
    utf8 c = Input.getkey (STDIN_FILENO);

handle_char:
    switch (c) {
      case '\t':
        c = buf_visual_complete_actions (this, vis_action);
        goto handle_char;

      VIS_HNDL_CASE_REG(reg);
      VIS_HNDL_CASE_INT(count);

      case ARROW_LEFT_KEY:
        self(normal.left, 1, DONOT_DRAW);
        continue;

      case ARROW_RIGHT_KEY:
        self(normal.right, 1, DONOT_DRAW);
        continue;

      case ESCAPE_KEY:
        goto theend;

      case HOME_KEY:
        self(normal.bol, DRAW);
        continue;

      case END_KEY:
        self(normal.eol, DRAW);
        continue;

      case 'e':
        VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        {
          int len = $my(vis)[0].lidx - $my(vis)[0].fidx + 1;
          char fname[len + 1];
          Cstring.cp (fname, len + 1, $mycur(data)->bytes + $my(vis)[0].fidx, len);
          ifnot (File.exists (fname)) goto theend;
          win_edit_fname ($my(parent), thisp, fname, $myparents(cur_frame), 0, 0, 0);
        }

        goto theend;

      case 'd':
      case 'x':
      case 'y':
      case 'Y':
        if (-1 is reg or c is 'Y') reg = (c is 'Y' ? REG_STAR : REG_UNNAMED);

        if ($my(vis)[0].lidx < $my(vis)[0].fidx) {
          VISUAL_ADJUST_IDXS($my(vis)[0]);
        } else {   /* MACRO BLOCKS ARE EVIL */
          VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        }

        if (c is 'd' or c is 'x')
          self(normal.delete, $my(vis)[0].lidx - $my(vis)[0].fidx + 1, reg, DONOT_DRAW);
        else
          self(normal.yank, $my(vis)[0].lidx - $my(vis)[0].fidx + 1, reg);

        goto theend;

      case '+':
      case '*':
        if ($my(vis)[0].lidx < $my(vis)[0].fidx) {
          VISUAL_ADJUST_IDXS($my(vis)[0]);
        } else {   /* MACRO BLOCKS ARE EVIL */
          VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        }

        {
          string_t *str = String.new (($my(vis)[0].lidx - $my(vis)[0].fidx) + 2);
          for (int ii = $my(vis)[0].fidx; ii <= $my(vis)[0].lidx; ii++)
            String.append_byte (str, $mycur(data)->bytes[ii]);
          ed_selection_to_X ($my(root), str->bytes, str->num_bytes,
              ('*' is c ? X_PRIMARY : X_CLIPBOARD));
          String.release (str);
          goto theend;
        }

      case '!':
        ifnot ($my(ftype)->read_from_shell) goto theend;

        if ($my(vis)[0].lidx < $my(vis)[0].fidx) {
          VISUAL_ADJUST_IDXS($my(vis)[0]);
        } else {   /* MACRO BLOCKS ARE EVIL */
          VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        }

        {
          string_t *str = String.new (($my(vis)[0].lidx - $my(vis)[0].fidx) + 2);
          for (int ii = $my(vis)[0].fidx; ii <= $my(vis)[0].lidx; ii++)
            String.append_byte (str, $mycur(data)->bytes[ii]);
          if (str->num_bytes)
            Ed.sh.popen ($my(root), this, str->bytes, ED_PROC_WAIT_AT_END, NULL);

          String.release (str);
          goto theend;
        }

      default:
        for (int i = 0; i < $myroots(cw_mode_chars_len); i++)
          if (c is $myroots(cw_mode_chars)[i]) {
            if ($my(vis)[0].lidx < $my(vis)[0].fidx) {
              VISUAL_ADJUST_IDXS($my(vis)[0]);
            } else {   /* MACRO BLOCKS ARE EVIL */
              VISUAL_RESTORE_STATE ($my(vis)[0], mark);
            }

            goto_cb = 1;
            goto theend;
            callback:;

            string_t *str = String.new (($my(vis)[0].lidx - $my(vis)[0].fidx) + 2);
            for (int ii = $my(vis)[0].fidx; ii <= $my(vis)[0].lidx; ii++)
              String.append_byte (str, $mycur(data)->bytes[ii]);

            for (int j = 0; j < $myroots(num_cw_mode_cbs); j++) {
              int retval = $myroots(cw_mode_cbs)[j] (thisp,
                  $my(vis)[0].fidx, $my(vis)[0].lidx, str, c, vis_action);
              if (retval isnot NO_CALLBACK_FUNCTION)
                break;
            }

            String.release (str);
            goto thereturn;
          }

        continue;

    }
  }

theend:
  $my(syn)->parse = $my(vis)[0].orig_syn_parser;
  $self(draw) = dbuf;
  $self(draw_current_row) = drow;
  self(set.mode, prev_mode);
  this = *thisp;
  self(draw);

  if (goto_cb) goto callback;

thereturn:
  return DONE;
}

static char *buf_syn_parse_visual_bw (buf_t *this, char *line, int len, int idx, row_t *row) {
  (void) len;

  if ((idx is $my(vis)[1].fidx) or
      (idx > $my(vis)[1].fidx and $my(vis)[1].fidx < $my(vis)[1].lidx and
       idx <= $my(vis)[1].lidx) or
      (idx < $my(vis)[1].fidx and $my(vis)[1].fidx > $my(vis)[1].lidx and
       idx >= $my(vis)[1].lidx) or
      (idx > $my(vis)[1].lidx and $my(vis)[1].lidx < $my(vis)[1].fidx and
       idx < $my(vis)[1].fidx)) {
    return buf_syn_parse_visual_line (this, line, len, row);
  }

  return $my(vis)[0].orig_syn_parser (this, line, len, idx, row);
}


static int buf_normal_visual_bw (buf_t *this) {
  VISUAL_INIT_FUN (VISUAL_MODE_BW, buf_syn_parse_visual_bw);

  char vis_action[MAXLEN_WORD_ACTION];
  vis_action[0] = '\0';

  for (;;) {
    $my(vis)[1].lidx = this->cur_idx;
    $my(vis)[0].lidx = $mycur(cur_col_idx);
    dbuf (this);

    utf8 c = Input.getkey (STDIN_FILENO);

handle_char:
    switch (c) {
      case '\t':
        c = buf_visual_complete_actions (this, vis_action);
        goto handle_char;

      VIS_HNDL_CASE_REG(reg);
      VIS_HNDL_CASE_INT(count);

      case ARROW_DOWN_KEY:
        self(normal.down, 1, ADJUST_COL, DONOT_DRAW);
        continue;

      case ARROW_UP_KEY:
        self(normal.up, 1, ADJUST_COL, DONOT_DRAW);
        continue;

      case PAGE_DOWN_KEY:
        self(normal.page_down, 1, DRAW);
        continue;

      case PAGE_UP_KEY:
        self(normal.page_up, 1, DRAW);
        continue;

      case ESCAPE_KEY:
        VISUAL_RESTORE_STATE ($my(vis)[0], mark);
        goto theend;

      case HOME_KEY:
        self(normal.bof, DONOT_DRAW);
        continue;

      case 'G':
      case END_KEY:
        self(normal.eof, DONOT_DRAW);
        continue;

      case ARROW_LEFT_KEY:
        self(normal.left, 1, DONOT_DRAW);
        continue;

      case ARROW_RIGHT_KEY:
        self(normal.right, 1, DONOT_DRAW);
        continue;

      case 'i':
      case 'I':
      case 'c':
        {
          int row = $my(cur_video_row) - (2 < $my(cur_video_row));
          int index = $my(vis)[1].lidx;
          if (index > $my(vis)[1].fidx)
            while (row > 2 and index > $my(vis)[1].fidx) {
              row--; index--;
            }

          VISUAL_ADJUST_IDXS($my(vis)[1]);
          string_t *str = self(input_box, row, $my(vis)[0].fidx + 1,
              DONOT_ABORT_ON_ESCAPE, NULL);

          Action_t *Action = self(Action.new);
          Action_t *Baction =self(Action.new);

          for (int idx = $my(vis)[1].fidx; idx <= $my(vis)[1].lidx; idx++) {
            self(current.set, idx);
            self(adjust.view);
            VISUAL_ADJUST_COL ($my(vis)[0].fidx);

            if (c is 'c') {
              if ((int) $mycur(data)->num_bytes < $my(vis)[0].fidx)
                continue;
              else
                self(normal.delete, $my(vis)[0].lidx - $my(vis)[0].fidx + 1,
                    REG_BLACKHOLE, DONOT_DRAW);

              Action_t *Paction = self(undo.pop);
              action_t *action = ListStackPop (Paction, action_t);
              ListStackPush (Baction, action);
              free (Paction);
            } else {
              action_t *action = self(action.new_with, REPLACE_LINE, this->cur_idx,
                  $mycur(data)->bytes, $mycur(data)->num_bytes);
              ListStackPush (Baction, action);
            }

            String.insert_at_with ($mycur(data), $my(vis)[0].fidx, str->bytes);
            $my(flags) |= BUF_IS_MODIFIED;
          }

          String.release (str);

          action_t *baction = ListStackPop (Baction, action_t);
          while (baction isnot NULL) {
            ListStackPush (Action, baction);
            baction = ListStackPop (Baction, action_t);
          }

          free (Baction);
          self(undo.push, Action);
        }

        VISUAL_RESTORE_STATE ($my(vis)[1], mark);
        goto theend;

      case 'x':
      case 'd':
        if (-1 is reg) reg = REG_UNNAMED;
        {
          Action_t *Action = self(Action.new);
          Action_t *Baction =self(Action.new);

          VISUAL_ADJUST_IDXS($my(vis)[0]);
          VISUAL_ADJUST_IDXS($my(vis)[1]);

          for (int idx = $my(vis)[1].fidx; idx <= $my(vis)[1].lidx; idx++) {
            self(current.set, idx);
            self(adjust.view);
            VISUAL_ADJUST_COL ($my(vis)[0].fidx);
            if ((int) $mycur(data)->num_bytes < $my(vis)[0].fidx + 1) continue;
            int lidx__ = (int) $mycur(data)->num_bytes < $my(vis)[0].lidx ?
              (int) $mycur(data)->num_bytes : $my(vis)[0].lidx;
            self(normal.delete, lidx__ - $my(vis)[0].fidx + 1, reg, DONOT_DRAW);
            Action_t *Paction = self(undo.pop);
            action_t *action = ListStackPop (Paction, action_t);

            ListStackPush (Baction, action);
            free (Paction);
            $my(flags) |= BUF_IS_MODIFIED;
          }

          action_t *action = ListStackPop (Baction, action_t);
          while (action isnot NULL) {
            ListStackPush (Action, action);
            action = ListStackPop (Baction, action_t);
          }

          free (Baction);
          self(undo.push, Action);
        }

        VISUAL_RESTORE_STATE ($my(vis)[1], mark);
        goto theend;

      default:
        continue;
    }
  }

theend:
  VISUAL_ADJUST_COL($mycur(cur_col_idx));
  $my(syn)->parse = $my(vis)[0].orig_syn_parser;
  $self(draw) = dbuf;
  $self(draw_current_row) = drow;
  self(set.mode, prev_mode);
  self(draw);

  return DONE;
}

static int win_edit_fname (win_t *win, buf_t **thisp, const char *fname, int frame,
                                             int reload, int draw, int reopen) {
  buf_t *this = *thisp;

  if (fname is NULL and DONOT_RELOAD is reload)
    return NOTHING_TODO;

  if (fname isnot NULL) {
    ifnot (reopen) {
      int idx;
      buf_t *bn = Win.get.buf_by_name (win, fname, &idx);
      ifnot (NULL is bn) {
        *thisp = Win.set.current_buf (win, idx, DRAW);
        return DONE;
      }
    }

    if ($my(at_frame) is frame) $my(flags) &= ~BUF_IS_VISIBLE;

    buf_t *that = Win.buf.new (win, BufOpts (
        .fname = fname,
        .at_frame = frame));

    DListSetCurrent (that, 0);

    int cur_idx = win->cur_idx;
    int idx = Win.append_buf (win, that);
    DListSetCurrent (win, cur_idx);
    Win.set.current_buf (win, idx, (draw ? DRAW : DONOT_DRAW));

    *thisp = that;
    this = that;

    if (draw) self(draw);
    goto theend;
  }

  if (Cstring.eq ($my(fname), UNNAMED))
    return NOTHING_TODO;

  int cur_idx = this->cur_idx;
  self(normal.bof, DONOT_DRAW);
  self(delete.line, this->num_items, REG_BLACKHOLE);
  self(read.fname);

  self(normal.bof, DONOT_DRAW);
  Action_t *action = self(undo.pop);

  for (;;) {
    action_t *act = self(action.new);
    undo_set (act, INSERT_LINE);
    act->idx = this->cur_idx;
    act->bytes = Cstring.dup ($mycur(data)->bytes, $mycur(data)->num_bytes);
    ListStackPush (action, act);
    if (NOTHING_TODO is buf_normal_down (this, 1, DONOT_ADJUST_COL, 0)) break;
  }

  self(current.set, 0);
  self(delete.line, 1, REG_BLACKHOLE);
  Action_t *baction = self(undo.pop);
  action_t *act = self(action.new);
  state_cp (act, baction->head);
  self(Action.release, baction);

  ListStackPush (action, act);
  self(undo.push, action);

  if (this->num_items is 0) buf_on_no_length (this);

  $mycur(first_col_idx) = $mycur(cur_col_idx) = 0;
  $my(video)->row_pos = $my(cur_video_row) = $my(dim)->first_row;
  $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;
  $my(video_first_row) = this->current;
  $my(video_first_row_idx) = this->cur_idx;

  if (cur_idx >= this->num_items) cur_idx = this->num_items - 1;
  self(normal.goto_linenr, cur_idx + 1, draw);

  if (draw) self(draw);
  MSG("reloaded");
  return DONE;

theend:
  return DONE;
}

static char *buf_get_current_dir (buf_t *this, int new_allocation) {
  char *cwd = Dir.current ();
  if (NULL is cwd) {
    MSG_ERRNO (MSG_CAN_NOT_DETERMINATE_CURRENT_DIR);
    free (cwd);
    return NULL;
  }

  if (new_allocation) return cwd;

  String.replace_with ($my(shared_str), cwd);
  free (cwd);
  /* dangerous because it is used in so many cases but effective, */
  return $my(shared_str)->bytes;
  /* as cut a lot of alloc's, this is like a static string, with local scope, it
   * doesn't really mind, as long is under control and do not used at the same time */
}

static int buf_open_fname_under_cursor (buf_t **thisp, int frame,
                             int force_open, int reopen, int draw) {
  buf_t *this = *thisp;

  char fname[PATH_MAX];
  int fidx, lidx;

  if (NULL is buf_get_current_word (this, fname, Notfname, Notfname_len, &fidx, &lidx))
    return NOTHING_TODO;

  ifnot (NULL is $my(ftype)->on_open_fname_under_cursor)
    $my(ftype)->on_open_fname_under_cursor (fname, lidx - fidx + 1, PATH_MAX);

  char *line = $mycur(data)->bytes + lidx + 1;
  int lnr = 0;
  if (*line is '|' or *line is ':') {
    while (*++line) {
      if ('0' <= *line and *line <= '9') lnr = (10 * lnr) + (*line - '0');
      else break;
    }
  }

  ifnot (File.exists (fname))
    ifnot (force_open) return NOTHING_TODO;

  ifnot (reopen)
    do {
      int idx;
      buf_t *bn = NULL;
      if (*fname isnot DIR_SEP) {
        char *cwd = buf_get_current_dir (this, SHARED_ALLOCATION);
        if (NULL is cwd) return NOTHING_TODO;
        int len = bytelen (cwd) + bytelen (fname) + 1;
        char nfn[len + 1];
        snprintf (nfn, len + 1, "%s%c%s", cwd, DIR_SEP, fname);
        char tmp[PATH_MAX + 1];
        Path.real (nfn, tmp);
        bn = Win.get.buf_by_name ($my(parent), tmp,  &idx);
      } else
        bn = Win.get.buf_by_name ($my(parent), fname, &idx);

      if (NULL is bn) break;

      *thisp = Win.set.current_buf ($my(parent), idx, DONOT_DRAW);
      this = *thisp;

      if (NOTHING_TODO is self(normal.goto_linenr, lnr, draw))
        if (draw) self(draw);

      return DONE;
    } while (0);

  if (frame is AT_CURRENT_FRAME) frame = $myparents(cur_frame);

  if (NOTHING_TODO is Win.edit_fname ($my(parent), thisp, fname, frame, 0,
  	DONOT_DRAW, 1))
    return NOTHING_TODO;

  this = *thisp;

  if (NOTHING_TODO is self(normal.goto_linenr, lnr, draw))
    if (draw) self(draw);

  return DONE;
}

static int buf_split (buf_t **thisp, const char *fname) {
  buf_t *this = *thisp;
  buf_t *that = this;

  if (win_add_frame ($my(parent)) is NOTHING_TODO)
    return NOTHING_TODO;

  win_edit_fname ($my(parent), thisp, fname, $myparents(num_frames) - 1, 1, 0, 1);

  $myparents(cur_frame) = $myparents(num_frames) - 1;

  $from(that, flags) |= BUF_IS_VISIBLE;

  Win.draw ($my(parent));
  return DONE;
}

static int buf_enew_fname (buf_t **thisp, const char *fname) {
  buf_t *this = *thisp;
  win_t *w = Ed.win.new ($my(root), NULL, 1);
  $my(root)->prev_idx = $my(root)->cur_idx;
  Ed.append.win ($my(root), w);
  win_edit_fname (w, thisp, fname, 0, 1, 1, 1);
  return DONE;
}

static int ed_win_change (ed_t *this, buf_t **bufp, int com, const char *name,
                                            int accept_rdonly, int force) {
  if (this->num_items is 1)
    ifnot (force) return NOTHING_TODO;

  int idx = this->cur_idx;
  int cidx = idx;

  ifnot (NULL is name) {
    if (Cstring.eq ($from($from((*bufp), parent), name), name))
      return NOTHING_TODO;
    win_t *w = self(get.win_by_name, name, &idx);
    if (NULL is w) return NOTHING_TODO;
  } else {
    switch (com) {
      case VED_COM_WIN_CHANGE_PREV:
        if (--idx is -1) { idx = this->num_items - 1; } break;

      case VED_COM_WIN_CHANGE_NEXT:
        if (++idx is this->num_items) { idx = 0; } break;

      case VED_COM_WIN_CHANGE_PREV_FOCUSED:
        idx = this->prev_idx;
    }

    if (idx is cidx)
      ifnot (force) return NOTHING_TODO;

    ifnot (accept_rdonly) {
      int tmp_idx = cidx;
      for (;;) {
        win_t *w = self(get.win_by_idx, idx);
        if (idx is tmp_idx or w is NULL)
          return NOTHING_TODO;

        if (win_isit_special_type (w)) {
          if (com is VED_COM_WIN_CHANGE_PREV) {
            if (idx is 0) idx = this->num_items - 1;
            else idx--;
          } else {
            if (idx is this->num_items - 1) idx = 0;
            else idx++;
          }
        } else break;
      }
    }
  }

  self(set.current_win, idx);
  win_t *parent = this->current;
  *bufp = parent->current;

  $from((*bufp), parent) = parent;
  *bufp = Win.set.current_buf (parent, parent->cur_idx, DONOT_DRAW);
  Win.set.video_dividers (parent);
  Win.draw (parent);
  return DONE;
}

static int ed_win_delete (ed_t *this, buf_t **thisp, int count_special) {
  if (1 is self(get.num_win, count_special))
    return EXIT_THIS;

  win_t *parent = DListPopCurrent (this, win_t);
  win_release (parent);

  parent = this->current;

  *thisp = Win.set.current_buf (parent, parent->cur_idx, DONOT_DRAW);

  Win.set.video_dividers (parent);
  Win.draw (parent);
  return DONE;
}

static int buf_write_to_fname (buf_t *this, const char *fname, int append, int fidx,
                                            int lidx, int force, int verbose) {
  if (NULL is fname) return NOTHING_TODO;
  int retval = NOTHING_TODO;

  string_t *fnstr = String.new_with (fname);
  ifnot (fnstr->num_bytes) goto theend;

  if (fnstr->bytes[fnstr->num_bytes - 1] is '%' and (NULL isnot $my(basename))) {
    String.clear_at(fnstr, fnstr->num_bytes - 1);
    String.append_with (fnstr, $my(basename));
  }

  int fexists = File.exists (fnstr->bytes);
  if (fexists and 0 is append and 0 is force) {
    VED_MSG_ERROR(MSG_FILE_EXISTS_AND_NO_FORCE, fnstr->bytes);
    goto theend;
  }

  ifnot (fexists) append = 0;

  FILE *fp = fopen (fnstr->bytes, (append ? "a+" : "w"));
  if (NULL is fp) {
    MSG_ERRNO (errno);
    goto theend;
  }

  int idx = 0;
  row_t *it = this->head;
  while (idx++ < fidx) it = it->next;
  idx--;

  size_t bts = 0;
  while (idx++ <= lidx) {
    bts += fprintf (fp, "%s\n", it->data->bytes);
    it = it->next;
  }

  fclose (fp);
  if (verbose)
    MSG("%s: %zd bytes written%s", fnstr->bytes, bts, (append ? " [appended]" : " "));

  retval = DONE;

theend:
  String.release (fnstr);
  return retval;
}

static int buf_write (buf_t *this, int force) {
  if (Cstring.eq ($my(fname), UNNAMED)) {
    VED_MSG_ERROR(MSG_CAN_NOT_WRITE_AN_UNNAMED_BUFFER);
    return NOTHING_TODO;
  }

  if ($my(flags) & BUF_IS_RDONLY) {
    VED_MSG_ERROR(MSG_BUF_IS_READ_ONLY);
    return NOTHING_TODO;
  }

  ifnot ($my(flags) & BUF_IS_MODIFIED) {
    ifnot (force) {
      VED_MSG_ERROR(MSG_ON_WRITE_BUF_ISNOT_MODIFIED_AND_NO_FORCE);
      return NOTHING_TODO;
    }
  }

  if ($my(flags) & FILE_EXISTS) {
    struct stat st;
    if (NOTOK is stat ($my(fname), &st)) {
      $my(flags) &= ~FILE_EXISTS;
      utf8 chars[] = {'y', 'Y', 'n', 'N'};
      utf8 c = buf_quest (this, STR_FMT (
          "[Warning]\n%s: removed from filesystem since last operation\n"
          "continue writing? [yY|nN]", $my(fname)), chars, ARRLEN (chars));
      switch (c) {case 'n': case 'N': return NOTHING_TODO;};
    } else {
      if (2 isnot force or 0 is ($my(flags) & BUF_SAVE_ON_EXIT)) {
  #if defined(__MACH__)
        if ($my(st).st_mtimespec.tv_sec isnot st.st_mtimespec.tv_sec) {
  #else
        if ($my(st).st_mtim.tv_sec isnot st.st_mtim.tv_sec) {
  #endif
          utf8 chars[] = {'y', 'Y', 'n', 'N'};
          utf8 c = buf_quest (this, STR_FMT (
              "[Warning]%s: has been modified since last operation\n"
              "continue writing? [yY|nN]", $my(fname)), chars, ARRLEN (chars));
          switch (c) {case 'n': case 'N': return NOTHING_TODO;};
        }
      }
    }
  }

  FILE *fp = fopen ($my(fname), "w");
  if (NULL is fp) {
    MSG_ERRNO(errno);
    return NOTHING_TODO;
  }

  row_t *row = this->head;

  size_t bts = 0;
  while (row isnot NULL) {
    bts += fprintf (fp, "%s\n", row->data->bytes);
    row = row->next;
  }

  $my(flags) &= ~BUF_IS_MODIFIED;
  $my(flags) |= (FILE_IS_READABLE|FILE_IS_WRITABLE|FILE_EXISTS);

  fstat (fileno (fp), &$my(st));
  fclose (fp);
  MSG("%s: %zd bytes written", $my(fname), bts);
  return DONE;
}

static int buf_read_from_fp (buf_t *this, FILE *stream, fp_t *fp) {
  (void) stream;
  mark_t t;  state_set (&t);  t.cur_idx = this->cur_idx;
  row_t *row = this->current;
  Action_t *Action = self(Action.new);

  char *line = NULL;
  size_t len = 0;
  size_t t_len = 0;
  ssize_t nread;
  while (-1 isnot (nread = ed_readline_from_fp (&line, &len, fp->fp))) {
    t_len += nread;
    action_t *action = self(action.new);
    undo_set (action, INSERT_LINE);
    self(current.append_with, line);
    action->idx = this->cur_idx;
    ListStackPush (Action, action);
  }

  ifnot (NULL is line) free (line);

  ifnot (t_len)
    free (Action);
  else
    self(undo.push, Action);

  $my(flags) |= BUF_IS_MODIFIED;
  state_restore (&t);
  this->current = row; this->cur_idx = t.cur_idx;
  self(draw);
  return DONE;
}

static int buf_read_from_file (buf_t *this, char *fname) {
  if (0 isnot access (fname, R_OK|F_OK)) return NOTHING_TODO;
  ifnot (File.is_reg (fname)) return NOTHING_TODO;

  fp_t fp = (fp_t) {.fp = fopen (fname, "r")};
  if (NULL is fp.fp) {
    MSG_ERRNO(errno);
    return NOTHING_TODO;
  }

  int retval = self(read.from_fp, NULL, &fp);
  fclose (fp.fp);
  return retval;
}

static int buf_proc_read_cb (proc_t *proc, FILE *stream, FILE *fp) {
  buf_t *this = Proc.get.user_data (proc);
  fp_t fpt = {.fp = fp};
  return buf_read_from_fp (this, stream, &fpt);
}

static int ed_sh_popen (ed_t *ed, buf_t *this, char *com,
                         int flags, ProcRead_cb read_cb) {
  int retval = NOTOK;
  proc_t *proc = Proc.new ();

  int proc_flags = flags;
  proc_flags &= ~(ED_PROC_WAIT_AT_END);

  Proc.set.user_data (proc, this);

  ifnot (NULL is read_cb)
    Proc.set.read_stream_cb (proc, proc_flags, read_cb);
  else
    if ((flags & ED_PROC_READ_STDOUT) or (flags & ED_PROC_READ_STDERR))
      Proc.set.read_stream_cb (proc, proc_flags, buf_proc_read_cb);

  Proc.parse (proc, com);

  term_t *term = Ed.get.term (ed);

  ifnot (flags & ED_PROC_READ_STDOUT)
    Term.reset (term);

  if (NOTOK is Proc.open (proc)) goto theend;
  Proc.read (proc);
  retval = Proc.wait (proc);

theend:
  ifnot (flags & ED_PROC_READ_STDOUT) {
    if (flags & ED_PROC_WAIT_AT_END) {
      Term.set_mode (term, 'r');
      Input.getkey (STDIN_FILENO);
    }

    Term.set (term);
  }

  Proc.release (proc);
  win_t *w = Ed.get.current_win (ed);
  int idx = Win.get.current_buf_idx (w);
  Win.set.current_buf (w, idx, DONOT_DRAW);
  Win.draw (w);
  return retval;
}

static int buf_read_from_shell (buf_t *this, char *com, int rlcom, int wait_at_end) {
  ifnot ($my(ftype)->read_from_shell) return NOTHING_TODO;
  int flags = 0;

  if (wait_at_end) flags |= ED_PROC_WAIT_AT_END;

  if (rlcom is VED_COM_READ_SHELL)
    flags |= (ED_PROC_READ_STDOUT);

  return Ed.sh.popen ($my(root), this, com, flags, NULL);
}

static int buf_change_bufname (buf_t **thisp, const char *bufname) {
  buf_t *this = *thisp;
  if (Cstring.eq ($my(fname), bufname)) return NOTHING_TODO;
  int idx;
  buf_t *buf = Win.get.buf_by_name ($my(parent), bufname, &idx);
  if (NULL is buf) return NOTHING_TODO;

  int cur_frame = $myparents(cur_frame);

  /* a little bit before the last bit of the zero cycle  */

  /* This code block reflects perfectly the development philosophy,
   * as if you analyze it, it catches two conditions, with the first
   * to be a quite common scenario, while the second it never happens
   * at least in (my) usual workflow. Now. I know there are quite few
   * more to catch, but unless someone generiously offers the code i'm
   * not going to spend the energy on them and can be safely considered
   * as undefined or unpredictable behavior.
   */

  if (cur_frame isnot $from(buf, at_frame) and
      ($from(buf, flags) & BUF_IS_VISIBLE) is 0) {
    $from(buf, at_frame) = cur_frame;
    int row = $from(buf, cur_video_row) - $from(buf, dim)->first_row;
    int old_rows = $from(buf, dim)->last_row - $from(buf, dim)->first_row;
    $from(buf, dim) = $myparents(frames_dim)[$my(at_frame)];
    $from(buf, statusline_row) = $from(buf, dim)->last_row;
    $from(buf, cur_video_row) = $from(buf, dim)->first_row + row;
    int cur_rows = $from(buf, dim)->last_row - $from(buf, dim)->first_row;
    if (cur_rows < old_rows and
        $from(buf, cur_video_row) isnot $from(buf, dim)->first_row) {
      int diff = old_rows - cur_rows;
      buf_set_video_first_row (buf,  $from(buf, video_first_row_idx) + diff);
      $from(buf, cur_video_row) -= diff;
    }
  }

  *thisp = Win.set.current_buf ($my(parent), idx, DONOT_DRAW);
  this = *thisp;
  $my(video)->row_pos = $my(cur_video_row);
  Win.draw ($my(parent));
  return DONE;
}

static int buf_change (buf_t **thisp, int com) {
  buf_t *this = *thisp;

  if ($my(is_sticked)) return NOTHING_TODO;
  if (1 is $my(parent)->num_items) return NOTHING_TODO;

  int cur_frame = $myparents(cur_frame);
  int idx = $my(parent)->cur_idx;

  switch (com) {
    case VED_COM_BUF_CHANGE_PREV_FOCUSED:
      idx = $my(parent)->prev_idx;
      if (idx >= $my(parent)->num_items)
        idx = $my(parent)->num_items - 1 -
            ($my(parent)->cur_idx is $my(parent)->num_items - 1);
      goto change;

    case VED_COM_BUF_CHANGE_PREV:
      {
        buf_t *it = $my(parent)->current->prev;
        int index = idx;
        while (it) {
          index--;
          if ($from(it, at_frame) is cur_frame) {
            idx = index;
            goto change;
          }
          it = it->prev;
        }

        index = $my(parent)->num_items;
        it = $my(parent)->tail;
        while (it) {
          index--;
          if (index < idx) {
            idx = index;
            goto change;
          }

          if ($from(it, at_frame) is cur_frame and idx isnot index) {
            idx = index;
            goto change;
          }

          it = it->prev;
        }

        index = $my(parent)->num_items;
        it = $my(parent)->tail;
        while (it) {
          index--;
          if (index isnot idx) {
            idx = index;
            goto change;
          }
          it = it->prev;
        }
      }
      break;

    case VED_COM_BUF_CHANGE_NEXT:
      {
        buf_t *it = $my(parent)->current->next;
        int index = idx;
        while (it) {
          index++;
          if ($from(it, at_frame) is cur_frame) {
            idx = index;
            goto change;
          }
          it = it->next;
        }

        index = -1;
        it = $my(parent)->head;
        while (it) {
          index++;
          if (index > idx) {
            idx = index;
            goto change;
          }

          if ($from(it, at_frame) is cur_frame and idx isnot index) {
            idx = index;
            goto change;
          }

          it = it->next;
        }

        index = -1;
        it = $my(parent)->head;
        while (it) {
          index++;
          if (index isnot idx) {
            idx = index;
            goto change;
          }
          it = it->next;
        }
      }
      break;

    default: return NOTHING_TODO;
  }

change:
  *thisp = Win.set.current_buf ($my(parent), idx, DRAW);
  return DONE;
}

static void buf_clear (buf_t *this) {
  self(release.rows);
  self(undo.clear);
  this->head = this->tail = this->current = NULL;
  this->cur_idx = 0; this->num_items = 0;
  buf_on_no_length (this);

  $my(video_first_row) = this->head;
  $my(video_first_row_idx) = 0;

  self(current.set, 0);
  $mycur(cur_col_idx) = $mycur(first_col_idx) = 0;
  $my(cur_video_row) = $my(dim)->first_row;
  $my(cur_video_col) = 1;
}

static int buf_delete (buf_t **thisp, int idx, int force) {
  buf_t *this = *thisp;
  win_t *parent = $my(parent);

  int cur_idx = parent->cur_idx;
  int at_frame =$my(at_frame);

  if (cur_idx is idx) {
    if ($my(flags) & BUF_IS_SPECIAL) return NOTHING_TODO;
    if ($my(flags) & BUF_IS_MODIFIED) {
      ifnot (force) {
        VED_MSG_ERROR(MSG_ON_BD_IS_MODIFIED_AND_NO_FORCE);
        return NOTHING_TODO;
      }
    }

    int num = parent->num_items;
    Win.pop.current_buf (parent);
    ifnot (num - 1) return WIN_EXIT;
    this = parent->current;
  } else {
    DListSetCurrent (parent, idx);
    this = parent->current;
    if ($my(flags) & BUF_IS_SPECIAL) {
      DListSetCurrent (parent, cur_idx);
      return NOTHING_TODO;
    }

    if ($my(flags) & BUF_IS_MODIFIED) {
      ifnot (force) {
        VED_MSG_ERROR(MSG_ON_BD_IS_MODIFIED_AND_NO_FORCE);
        DListSetCurrent (parent, cur_idx);
        return NOTHING_TODO;
      }
    }

    Win.pop.current_buf (parent);
    DListSetCurrent (parent, (idx > cur_idx) ? cur_idx : cur_idx - 1);
  }

  int found = 0;
  int should_draw = 0;

  this = parent->head;
  while (this) {
    if ($my(at_frame) is at_frame) {
      found = 1;
      $my(flags) |= BUF_IS_VISIBLE;
      break;
    }

    this = this->next;
  }

  ifnot (found) win_delete_frame (parent, at_frame);

  this = parent->current;

  *thisp = Win.set.current_buf (parent, parent->cur_idx, DONOT_DRAW);

  if (cur_idx is idx) {
    if (found is 1 or $from(parent, num_frames) is 1) {
      if (NOTHING_TODO is buf_change (thisp, VED_COM_BUF_CHANGE_PREV_FOCUSED))
        should_draw = 1;
    } else {
      int frame = WIN_CUR_FRAME(parent) + 1;
      if (frame > WIN_LAST_FRAME(parent)) frame = FIRST_FRAME;
      *thisp = Win.frame.change ($my(parent), frame, DONOT_DRAW);
    }
  } else should_draw = 1;

  if (0 is found or should_draw) Win.draw (parent);

  return DONE;
}

static int ed_complete_digraph_callback (menu_t *menu) {
  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  Vstring_t *items = Vstring.new ();
  const char *digraphs[] = {
    "167 §",  "169 ©",  "171 «",  "174 ®",  "176 °",  "178 ²",  "179 ³", "183 ·",
    "185 ¹",  "187 »",  "188 ¼",  "189 ½",  "190 ¾",  "215 ×",  "247 ÷", "729 ˙",
    "8212 —", "8220 “", "8230 …", "8304 ⁰", "8308 ⁴", "8309 ⁵", "8310 ⁶",
    "8311 ⁷", "8312 ⁸", "8313 ⁹", "8314 ⁺", "8315 ⁻", "8316 ⁼", "8317 ⁽",
    "8318 ⁾", "8319 ⁿ", "8364 €", "8531 ⅓", "8532 ⅔", "8533 ⅕", "8534 ⅖",
    "8535 ⅗", "8536 ⅘", "8537 ⅙", "8538 ⅚", "8539 ⅛", "8540 ⅜", "8541 ⅝",
    "8542 ⅞", "8771 ≃",
    "8804 ≤", "8805 ≥ ", "8806 ≦", "8807 ≧",
    "9472 ─", "9474 │", "9484 ┌", "9488 ┐", "9492 └", "9496 ┘", "9500 ├",
    "9507 ┣", "9508 ┤", "9516 ┬", "9523 ┳", "9524 ┴", "9531 ┻", "9532 ┼",
    "9552 ═", "9553 ║", "9556 ╔", "9559 ╗", "9562 ╚", "9565 ╝", "9568 ╠",
    "9571 ╣", "9574 ╦", "9577 ╩", "9580 ╬", "9581 ╭", "9582 ╮", "9583 ╯",
    "9584 ╰",
    "9833 ♩", "9834 ♪", "9835 ♫", "9836 ♬", "9837 ♭",
    "9838 ♮", "9839 ♯", "10003 ✓"
  };

  for (int i = 0; i < (int) ARRLEN (digraphs); i++)
    if (menu->patlen) {
      if (Cstring.eq_n (digraphs[i], menu->pat, menu->patlen))
        Vstring.current.append_with (items, digraphs[i]);
    } else
      Vstring.current.append_with (items, digraphs[i]);

  menu->list = items;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  return DONE;
}

static int buf_complete_digraph (buf_t *this, utf8 *c) {
  int retval = DONE;

  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = $my(root),
    .user_data_second = this,
    .video = $my(video),
    .term = $my(term_ptr),
    .first_row = $my(video)->row_pos,
    .last_row = *$my(prompt_row_ptr) - 2,
    .first_col = $my(video)->col_pos,
    .prompt_row = *$my(prompt_row_ptr),
    .state = MENU_INIT,
    .process_list_cb = ed_complete_digraph_callback,
    .next_key = CTRL('k'),
    .pat = NULL,
    .patlen = 0));

  if ((retval = menu->retval) is NOTHING_TODO) goto theend;

  char *item = Menu.create (menu);

  *c = 0;
  if (item isnot NULL)
    while (*item isnot ' ') *c = (10 * *c) + (*item++ - '0');

theend:
  Menu.release (menu);
  return NEWCHAR;
}

static int ed_complete_arg (menu_t *menu) {
  buf_t *this = menu->user_data[MENU_BUF_IDX];

  int com = $my(shared_int);
  char *line = $my(shared_str)->bytes;

  if ($myroots(commands)[com]->args is NULL) {
    if (0 is $myroots(has_ed_readline_commands) or
        (com < VED_COM_BUF_DELETE_FORCE or
         com > VED_COM_BUF_CHANGE_ALIAS)) {
      menu->state |= MENU_QUIT;
      return NOTHING_TODO;
    }
  }

  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  Vstring_t *args = Vstring.new ();

  if ($myroots(has_ed_readline_commands)) {
    int patisopt = (menu->patlen ? Cstring.eq (menu->pat, "--bufname=") : 0);

    if ((com >= VED_COM_BUF_DELETE_FORCE and
         com <= VED_COM_BUF_CHANGE_ALIAS) or patisopt) {
      char *cur_fname = $my(fname);

      buf_t *it = $my(parent)->head;
      while (it) {
        ifnot (Cstring.eq (cur_fname, $from(it, fname))) {
          if ((0 is menu->patlen or 1 is patisopt) or
               Cstring.eq_n ($from(it, fname), menu->pat, menu->patlen)) {
            ifnot (patisopt) {
              size_t len = bytelen ($from(it, fname)) + 10 + 2;
              char bufn[len + 1];
              snprintf (bufn, len + 1, "--bufname=\"%s\"", $from(it, fname));
              Vstring.add.sort_and_uniq (args, bufn);
            } else
              Vstring.add.sort_and_uniq (args, $from(it, fname));
          }
        }
        it = it->next;
      }

      goto check_list;
    }
  }

  int i = 0;

  ifnot (menu->patlen) {
    while ($myroots(commands)[com]->args[i])
      Vstring.add.sort_and_uniq (args, $myroots(commands)[com]->args[i++]);
  } else {
    while ($myroots(commands)[com]->args[i]) {
      if (Cstring.eq_n ($myroots(commands)[com]->args[i], menu->pat, menu->patlen))
        if (NULL is strstr (line, $myroots(commands)[com]->args[i]) or
        	Cstring.eq ($myroots(commands)[com]->args[i], "--fname=") or
        	Cstring.eq_n ($myroots(commands)[com]->args[i], "--command=", 10))
          Vstring.add.sort_and_uniq (args, $myroots(commands)[com]->args[i]);
      i++;
    }
  }

check_list:
  ifnot (args->num_items) {
    menu->state |= MENU_QUIT;
    Vstring.release (args);
    return NOTHING_TODO;
  }

  menu->list = args;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  String.replace_with (menu->header, menu->pat);

  return DONE;
}

static int ed_complete_command (menu_t *menu) {
  buf_t *this = menu->user_data[MENU_BUF_IDX];

  if (menu->state & MENU_INIT) {
    menu->state &= ~MENU_INIT;
  } else
    Menu.release_list (menu);

  Vstring_t *coms = Vstring.new ();
  int i = 0;

  ifnot (menu->patlen) {
    while ($myroots(commands)[i])
      Vstring.add.sort_and_uniq (coms, $myroots(commands)[i++]->com);
  } else {
    while ($myroots(commands)[i]) {
      ifnot (Cstring.cmp_n ($myroots(commands)[i]->com, menu->pat, menu->patlen))
        Vstring.add.sort_and_uniq (coms, $myroots(commands)[i]->com);
      i++;
    }
  }

  ifnot (coms->num_items) {
    menu->state |= MENU_QUIT;
    Vstring.release (coms);
    return NOTHING_TODO;
  }

  menu->list = coms;
  menu->state |= (MENU_LIST_IS_ALLOCATED|MENU_REINIT_LIST);
  String.replace_with (menu->header, menu->pat);

  return DONE;
}

static int ed_complete_filename (menu_t *menu) {
  buf_t *this = menu->user_data[MENU_BUF_IDX];
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
    return NOTHING_TODO;
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

  return DONE;

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

  return DONE;
}

static int buf_insert_complete_filename (buf_t **thisp) {
  buf_t *this = *thisp;
  int retval = DONE;
  int fidx = 0; int lidx = 0;
  size_t WORD_LEN = PATH_MAX + 1;
  char word[WORD_LEN]; word[0] = '\0';
  if (IS_SPACE ($mycur(data)->bytes[$mycur(cur_col_idx)]) and (
      $mycur(data)->num_bytes > 1 and 0 is IS_SPACE ($mycur(data)->bytes[$mycur(cur_col_idx) - 1])))
    self(normal.left, 1, DRAW);

  self(get.current_word, word, Notfname, Notfname_len, &fidx, &lidx);
  size_t len = bytelen (word);
  size_t orig_len = len;

redo:;
  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = $my(root),
    .user_data_second = this,
    .video = $my(video),
    .term = $my(term_ptr),
    .first_row = $my(video)->row_pos,
    .last_row = *$my(prompt_row_ptr) - 2,
    .first_col = $my(video)->col_pos,
    .prompt_row = *$my(prompt_row_ptr),
    .state  = MENU_INIT,
    .process_list_cb = ed_complete_filename,
    .next_key = CTRL('f'),
    .pat = word,
    .patlen = len));

  if ((retval = menu->retval) is NOTHING_TODO) goto theend;

  for (;;) {
    char *item  = Menu.create (menu);

    if (NULL is item) { retval = NOTHING_TODO; goto theend; }

    menu->patlen = bytelen (item);
    Cstring.cp (menu->pat, MAXLEN_PATTERN, item, menu->patlen);
    menu->state |= MENU_FINALIZE;

    ed_complete_filename (menu);

    if (menu->state & MENU_DONE) break;

    len = $my(shared_str)->num_bytes;
    Cstring.cp (word, WORD_LEN, $my(shared_str)->bytes, $my(shared_str)->num_bytes);
    Menu.release (menu);
    goto redo;
  }

  String.replace_numbytes_at_with ($mycur(data), orig_len, fidx,
      $my(shared_str)->bytes);

  $my(flags) |= BUF_IS_MODIFIED;
  self(normal.end_word, 1, 0, DONOT_DRAW);
  self(draw_current_row);

theend:
  Menu.release (menu);
  return retval;
}

static int readline_tab_completion (readline_t *rl) {
  ed_t *this = (ed_t *) rl->user_data[READLINE_ED_USER_DATA_IDX];

  ifnot (rl->line->num_items) return READLINE_OK;

  int retval = READLINE_OK;
  buf_t *curbuf = this->current->current;

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

    $from(curbuf, shared_int) = rl->com;
    String.replace_with ($from(curbuf, shared_str), currline->bytes);

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
        if ($my(has_ed_readline_commands)) {
          if (rl->com >= VED_COM_BUF_DELETE_FORCE and
              rl->com <= VED_COM_BUF_CHANGE_ALIAS)
            type |= READLINE_TOK_ARG;
          else
            type |= READLINE_TOK_ARG_FILENAME;
        } else
          type |= READLINE_TOK_ARG_FILENAME;
      }
    }
  }

  String.release (currline);

  ifnot (type) return retval;

  int (*process_list) (menu_t *) = NULL;

  if (type & READLINE_TOK_ARG_FILENAME)
    process_list = ed_complete_filename;
  else if (type & READLINE_TOK_COMMAND)
    process_list = ed_complete_command;
  else if (type & READLINE_TOK_ARG)
    process_list = ed_complete_arg;

  menu_t *menu = Menu.new (MenuOpts (
    .user_data_first = this,
    .user_data_second = curbuf,
    .video = $my(video),
    .term = $my(term),
    .first_row = $my(prompt_row) - 2,
    .last_row  = $my(prompt_row) - 2,
    .first_col = 0,
    .prompt_row = $my(prompt_row),
    .state = MENU_INIT,
    .process_list_cb = process_list,
    .pat = tok,
    .patlen = toklen));

  if ((retval = menu->retval) is NOTHING_TODO) goto theend;

  if (type & READLINE_TOK_ARG_FILENAME)
    menu->clear_and_continue_on_backspace = 1;

  menu->return_if_one_item = 1;

  char *item;
  for (;;) {
    item = Menu.create (menu);

    if (NULL is item) goto theend;
    if (menu->state & MENU_QUIT) break;
    if (type & READLINE_TOK_ARG and Cstring.eq_n ("--command=", item, 10))
       Msg.send (this, COLOR_WARNING, "--command argument should be enclosed in a pair of '{}' braces");

    if (type & READLINE_TOK_COMMAND or type & READLINE_TOK_ARG) break;

    menu->patlen = bytelen (item);
    Cstring.cp (menu->pat, MAXLEN_PATTERN, item, menu->patlen);

    if (type & READLINE_TOK_ARG_FILENAME) menu->state |= MENU_FINALIZE;

    if (menu->process_list (menu) is NOTHING_TODO) goto theend;

    if (menu->state & (MENU_REDO|MENU_DONE)) break;
    if (menu->state & MENU_QUIT) goto theend;
  }

  if (type & READLINE_TOK_ARG_FILENAME) {
    ifnot (menu->state & MENU_REDO)
      if (rl->com isnot VED_COM_READ_SHELL and rl->com isnot VED_COM_SHELL) {

        String.prepend_with ($from(curbuf, shared_str), "--fname=\"");
        String.append_byte ($from(curbuf, shared_str), '"');
      }

    item = $from(curbuf, shared_str)->bytes;
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

static void ed_deinit_commands (ed_t *this) {
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
  $my(has_ed_readline_commands) = 0;
}

static void ed_realloc_command_arg (readline_com_t *rlcom, int num) {
  int orig_num = rlcom->num_args;
  rlcom->num_args = num;
  rlcom->args = Realloc (rlcom->args, sizeof (char *) * (rlcom->num_args + 1));
  for (int i = orig_num; i <= num; i++)
    rlcom->args[i] = NULL;
}

static void ed_add_command_arg (readline_com_t *rlcom, int flags) {
#define ADD_ARG(arg, len, idx) ({                             \
  if (idx is rlcom->num_args)                                 \
    ed_realloc_command_arg (rlcom, idx);                     \
  rlcom->args[idx] = Cstring.dup (arg, len);                      \
  idx++;                                                      \
})

  int i = 0;
  if (flags & READLINE_ARG_INTERACTIVE) ADD_ARG ("--interactive", 13, i);
  if (flags & READLINE_ARG_BUFNAME) ADD_ARG ("--bufname=", 10, i);
  if (flags & READLINE_ARG_RANGE) ADD_ARG ("--range=", 8, i);
  if (flags & READLINE_ARG_GLOBAL) ADD_ARG ("--global", 8, i);
  if (flags & READLINE_ARG_APPEND) ADD_ARG ("--append", 8, i);
  if (flags & READLINE_ARG_FILENAME) ADD_ARG ("--fname=", 8, i);
  if (flags & READLINE_ARG_SUB) ADD_ARG ("--sub=", 6, i);
  if (flags & READLINE_ARG_PATTERN) ADD_ARG ("--pat=", 6, i);
  if (flags & READLINE_ARG_VERBOSE) ADD_ARG ("--verbose", 9, i);
  if (flags & READLINE_ARG_RECURSIVE) ADD_ARG ("--recursive", 11, i);
}

static void ed_append_command_arg (ed_t *this, const char *com, const char *argname, size_t len) {
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
        ed_realloc_command_arg ($my(commands)[i], $my(commands)[i]->num_args + 1);
        $my(commands)[i]->args[$my(commands)[i]->num_args - 1] = Cstring.dup (argname, len);
      }

      return;
    }
    i++;
  }
}

static void ed_append_readline_commands (ed_t *this, const char **commands,
                     int num_commands, int num_args[], int flags[]) {
  int len = $my(num_commands) + num_commands;

  ifnot ($my(num_commands))
    $my(commands) = Alloc (sizeof (readline_com_t) * (num_commands + 1));
  else
    $my(commands) = Realloc ($my(commands), sizeof (readline_com_t) * (len + 1));

  int j = 0;
  int i = $my(num_commands);
  for (; i < len; i++, j++) {
    $my(commands)[i] = Alloc (sizeof (readline_com_t));
    size_t clen = bytelen (commands[j]);
    $my(commands)[i]->com = Alloc (clen + 1);
    Cstring.cp ($my(commands)[i]->com, clen + 1, commands[j], clen);

    ifnot (num_args[j]) {
      $my(commands)[i]->args = NULL;
      continue;
    }

    $my(commands)[i]->args = Alloc (sizeof (char *) * ((int) num_args[j] + 1));
    $my(commands)[i]->num_args = num_args[j];
    for (int k = 0; k <= num_args[j]; k++)
      $my(commands)[i]->args[k] = NULL;

    ed_add_command_arg ($my(commands)[i], flags[j]);
  }

  $my(commands)[len] = NULL;
  $my(num_commands) = len;
}

static void ed_append_readline_command (ed_t *this, const char *name, int args, int flags) {
  const char *commands[2] = {name, NULL};
  int largs[] = {args, 0};
  int lflags[] = {flags, 0};
  ed_append_readline_commands (this, commands, 1, largs, lflags);
}

static int ed_get_num_readline_commands (ed_t *this) {
  return $my(num_commands);
}

#if 0
static void readline_reg (readline_t *rl) {
  ed_t *this = (ed_t *) rl->user_data[READLINE_ED_USER_DATA_IDX];

  int regidx = ed_reg_get_idx (this, Input.getkey (STDIN_FILENO));
  if (NOTOK is regidx) return;

  buf_t *buf = self(get.current_buf);
  if (ERROR is ed_reg_special_set (this, buf, regidx))
    return;

  Reg_t *rg = &$my(regs)[regidx];
  if (rg->type is LINEWISE) return;

  reg_t *reg = rg->head;
  while (reg isnot NULL) {
    Readline.insert_with_len (rl, reg->data->bytes, reg->data->num_bytes);
    reg = reg->next;
  }
}
#endif

static int buf_test_key (buf_t *this) {
  utf8 c;
  MSG("press any key to test, press escape to end the test");
  char str[128]; char bin[32]; char chr[8]; int len;
  for (;;) {
    Cursor.hide ($my(term_ptr));
    c = Input.getkey (STDIN_FILENO);
    snprintf (str, 128, "dec: %d hex: 0x%x octal: 0%o bin: %s char: %s",
        c, c, c, Cstring.itoa (c, bin, 2), Ustring.character (c, chr, &len));
    MSG(str);
    if (c is ESCAPE_KEY) break;
  }

  Cursor.show ($my(term_ptr));
  return DONE;
}

static int __validate_utf8_cb__ (Vstring_t *messages, char *line, size_t len,
                                                        int lnr, void *obj) {
  int *retval = (int *) obj;
  char *message;
  int num_faultbytes;
  int cur_idx = 0;
  char *bytes = line;
  size_t orig_len = len;
  size_t index;

check_utf8:
  index = Ustring.validate ((unsigned char *) bytes, len, &message, &num_faultbytes);

  ifnot (index) return OK;

  Vstring.append_with_fmt (messages,
      "--== Invalid UTF8 sequence ==-\n"
      "message: %s\n"
      "%s\nat line number %d, at index %zd, num invalid bytes %d\n",
      message, line, lnr, index + cur_idx, num_faultbytes);

  *retval = NOTOK;
  cur_idx += index + num_faultbytes;
  len = orig_len - cur_idx;
  bytes = line + cur_idx;
  num_faultbytes = 0;
  message = NULL;
  goto check_utf8;

  return *retval;
}

static int buf_com_validate_utf8 (buf_t **thisp, readline_t *rl) {
  buf_t *this = *thisp;

  int retval = OK;

  Vstring_t *fnames = NULL;
  if (NULL is rl or (NULL is (fnames = Readline.get.arg_fnames (rl, -1)))) {
    fnames = Vstring.new ();
    Vstring.append_with (fnames, $my(fname));
  }

  Vstring_t *err_messages = Vstring.new ();

  vstring_t *it = fnames->head;

  while (it) {
    char *fname = it->data->bytes;
    ifnot (File.exists (fname)) {
      Vstring.append_with_fmt (err_messages, "%s doesn't exists", fname);
      retval = NOTOK;
      goto next;
    }

    ifnot (File.is_readable (fname)) {
      Vstring.append_with_fmt (err_messages, "%s is not readable", fname);
      retval = NOTOK;
      goto next;
    }

    File.readlines (fname, err_messages, __validate_utf8_cb__, &retval);

    if (retval is OK)
      Msg.send_fmt ($my(root), COLOR_SUCCESS, "Validating %s ... OK", fname);

next:
    it = it->next;
  }

  if (retval is NOTOK) {
    Ed.append.toscratch ($my(root), CLEAR, "ERROR MESSAGES:");
    it = err_messages->head;
    while (it) {
      Ed.append.toscratch ($my(root), DONOT_CLEAR, it->data->bytes);
      it = it->next;
    }

    Ed.scratch ($my(root), thisp, NOT_AT_EOF);
    retval = OK;
  }

  Vstring.release (fnames);
  Vstring.release (err_messages);
  return retval;
}

static int buf_validate_utf8_lw_mode_cb (buf_t **thisp, int fidx, int lidx,
                             Vstring_t *rows, utf8 c, char *action) {
  (void) action;
  if (c isnot 'v') return NO_CALLBACK_FUNCTION;

  buf_t *this = *thisp;

  int retval = OK;
  int count = lidx - fidx + 1;

  Vstring_t *err_messages = Vstring.new ();
  vstring_t *it = rows->head;

  int i = 0;
  while (it and i++ < count) {
    __validate_utf8_cb__ (err_messages, it->data->bytes,
        it->data->num_bytes, fidx + i, &retval);
    it = it->next;
  }

  if (retval is NOTOK) {
    Ed.append.toscratch ($my(root), CLEAR, "ERROR MESSAGES:");
    it = err_messages->head;
    while (it) {
      Ed.append.toscratch ($my(root), DONOT_CLEAR, it->data->bytes);
      it = it->next;
    }

    Ed.scratch ($my(root), thisp, NOT_AT_EOF);
  } else
    Msg.send ($my(root), COLOR_SUCCESS, "Validating text ... OK");

  return retval;
}

static utf8 buf_spell_question (spell_t *spell, buf_t **thisp,
        Action_t **Action, int fidx, int lidx, bufiter_t *iter) {
  buf_t *this = *thisp;
  ed_t *ed = $my(root);

  char prefix[fidx + 1];
  char lpart[iter->line->num_bytes - lidx];

  Cstring.substr (prefix, fidx, iter->line->bytes, iter->line->num_bytes, 0);
  Cstring.substr (lpart, iter->line->num_bytes - lidx - 1, iter->line->bytes,
     iter->line->num_bytes, lidx + 1);

  string_t *quest = String.new (512);

  String.append_with_fmt (quest,
    "Spelling [%s] at line %d and %d index\n%s%s%s\n",
     spell->word, iter->idx + 1, fidx, prefix, spell->word, lpart);

   ifnot (spell->guesses->num_items)
     String.append_with (quest, "Cannot find matching words and there are no suggestions\n");
   else
     String.append_with (quest, "Suggestions: (enter number to accept one as correct)\n");

  int charslen = 5 + spell->guesses->num_items;
  utf8 chars[charslen];
  chars[0] = 'A'; chars[1] = 'a'; chars[2] = 'c'; chars[3] = 'i'; chars[4] = 'q';
  vstring_t *it = spell->guesses->head;
  for (int j = 1; j <= spell->guesses->num_items; j++) {
    String.append_with_fmt (quest, "%d: %s\n", j, it->data->bytes);
    chars[4+j] = '0' + j;
    it = it->next;
  }

  String.append_with (quest,
      "Choises:\n"
      "a[ccept] word as correct and add it to the dictionary\n"
      "A[ccept] word as correct just for this session\n"
      "c[ansel] operation and continue with the next\n"
      "i[nput]  correct word by getting input\n"
      "q[uit]   quit operation\n");

  utf8 c = Ed.question (ed, quest->bytes, chars, charslen);
  String.release (quest);

  it = spell->guesses->head;
  switch (c) {
    case 'c': return SPELL_OK;
    case 'a':
      Spell.add_word_to_dictionary (spell, spell->word);
      Imap.set_with_keylen (spell->dic, spell->word);
      return SPELL_OK;

    case 'q': return SPELL_ERROR;

    case 'A':
      Imap.set_with_keylen (spell->ign_words, spell->word);
      return SPELL_OK;

    case 'i': {
      int row = self(get.current_video_row) - 1;
      int col = self(get.current_video_col);

      string_t *inp = self(input_box, row, col, 0, spell->word);
      ifnot (inp->num_bytes) {
        String.release (inp);
        return SPELL_OK;
      }

      self(Action.set_with, *Action, REPLACE_LINE, iter->idx,
          iter->line->bytes, iter->line->num_bytes);
      String.replace_numbytes_at_with (iter->line, spell->word_len, fidx, inp->bytes);
      String.release (inp);
      self(set.modified);
      return SPELL_CHANGED_WORD;
    }

    default: {
      self(Action.set_with, *Action, REPLACE_LINE, iter->idx,
          iter->line->bytes, iter->line->num_bytes);
      it = spell->guesses->head;
      for (int k = '1'; k < c; k++) it = it->next;
      String.replace_numbytes_at_with (iter->line, spell->word_len, fidx,
          it->data->bytes);
      self(set.modified);
      return SPELL_CHANGED_WORD;
    }
  }

  return SPELL_OK;
}

static int buf_spell (buf_t **thisp, readline_t *rl) {
  buf_t *this = *thisp;

  int range[2];
  int edit = Readline.arg.exists (rl, "edit");
  if (edit) {
    win_t *w = $my(parent);
    Win.edit_fname (w, thisp, Spell.get.dictionary ()->bytes, 0, 0, 1, 0);
    return OK;
  }

  ed_t *ed = $my(root);

  int retval = readline_get_buf_range (rl, *thisp, range);
  if (NOTOK is retval) {
    range[0] = self(get.current_row_idx);
    range[1] = range[0];
  }

  int count = range[1] - range[0] + 1;

  spell_t *spell = Spell.new ();
  if (SPELL_ERROR is Spell.init_dictionary (spell, Spell.get.dictionary (),
      Spell.get.num_entries (), NO_FORCE)) {
    Msg.send (ed, COLOR_RED, spell->messages->head->data->bytes);
    Spell.release (spell, SPELL_CLEAR_DICTIONARY);
    return NOTOK;
  }

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);

  int buf_changed = 0;

  char word[MAXLEN_WORD];

  bufiter_t *iter = self(iter.new, range[0]);

  int i = 0;
  while (iter and i++ < count) {
    int fidx = 0; int lidx = -1;
    string_t *line = iter->line;
    char *tmp = NULL;
    for (;;) {
      int cur_idx = lidx + 1 + (tmp isnot NULL);
      tmp = Cstring.extract_word_at (line->bytes, line->num_bytes,
          word, MAXLEN_WORD, SPELL_NOTWORD, SPELL_NOTWORD_LEN, cur_idx, &fidx, &lidx);

      if (NULL is tmp) {
        if (lidx >= (int) line->num_bytes - 1)
          goto itnext;
        continue;
      }

      int len = lidx - fidx + 1;
      if (len < (int) spell->min_word_len or len >= MAXLEN_WORD)
        continue;

      spell->word_len = len;
      Cstring.cp (spell->word, MAXLEN_WORD, word, len);

      retval = Spell.correct (spell);

      if (retval >= SPELL_WORD_IS_CORRECT) continue;

      retval = buf_spell_question (spell, thisp, &Action, fidx, lidx, iter);
      if (SPELL_ERROR is retval) goto theend;
      if (SPELL_CHANGED_WORD is retval) {
        retval = SPELL_OK;
        buf_changed = 1;
      }
    }
itnext:
    iter = self(iter.next, iter);
  }

theend:
  if (buf_changed) {
    self(undo.push, Action);
    self(draw);
  } else
    self(Action.release, Action);

  self(iter.release, iter);
  Spell.release (spell, SPELL_DONOT_CLEAR_DICTIONARY);
  return retval;
}

static int buf_spell_word (buf_t **thisp, int fidx, int lidx,
                                  bufiter_t *iter, char *word) {
  buf_t *this = *thisp;
  ed_t *ed = $my(root);

  int retval = NOTOK;

  spell_t *spell = Spell.new ();

  if (SPELL_ERROR is Spell.init_dictionary (spell, Spell.get.dictionary (),
      Spell.get.num_entries (), NO_FORCE)) {
    Msg.send (ed, COLOR_RED, spell->messages->head->data->bytes);
    Spell.release (spell, SPELL_CLEAR_DICTIONARY);
    return NOTOK;
  }

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);

  int len = lidx - fidx + 1;

  char lword[len + 1];
  int i = 0;
  while (i < len and NULL isnot Cstring.byte.in_str (SPELL_NOTWORD, word[i])) {
    fidx++;
    len--;
    i++;
  }

  int j = 0;
  int orig_len = len;
  len = 0;
  while (i < orig_len and NULL is Cstring.byte.in_str (SPELL_NOTWORD, word[i])) {
    lword[j++] = word[i++];
    len++;
  }

  lword[j] = '\0';

  if (i isnot len) {
    i = len - 1;
    while (i >= 0 and NULL isnot Cstring.byte.in_str (SPELL_NOTWORD, word[i--])) {
      lidx--;
      len--;
    }
  }

  if (len < (int) spell->min_word_len) goto theend;

  spell->word_len = len;
  Cstring.cp (spell->word, MAXLEN_WORD, lword, len);

  retval = Spell.correct (spell);

  if (retval >= SPELL_WORD_IS_CORRECT) {
    retval = OK;
    goto theend;
  }

  retval = buf_spell_question (spell, thisp, &Action, fidx, lidx, iter);

theend:
  if (retval is SPELL_CHANGED_WORD) {
    self(undo.push, Action);
    self(draw);
    retval = SPELL_OK;
  } else
    self(Action.release, Action);

  Spell.release (spell, SPELL_DONOT_CLEAR_DICTIONARY);
  return retval;
}

static int buf_spell_word_cb  (buf_t **thisp, int fidx, int lidx,
                  bufiter_t *it, char *word, utf8 c, char *action) {
  (void) fidx; (void) lidx; (void) action; (void) it; (void) word; (void) thisp;

  if (c isnot 'S') return NO_CALLBACK_FUNCTION;

  return buf_spell_word (thisp, fidx, lidx, it, word);
}

static int buf_spell_cw_mode_cb (buf_t **thisp, int fidx, int lidx, string_t *str, utf8 c, char *action) {
  if (c isnot 'S') return NO_CALLBACK_FUNCTION;

  buf_t *this = *thisp;

  bufiter_t *iter = self(iter.new, -1);
  int retval = buf_spell_word_cb (thisp, fidx, lidx, iter, str->bytes, c, action);
  self(iter.release, iter);
  return retval;
}

static int buf_spell_lw_mode_cb (buf_t **thisp, int fidx, int lidx, Vstring_t *vstr, utf8 c, char *action) {
  (void) fidx; (void) lidx; (void) action; (void) vstr;

  if (c isnot 'S') return NO_CALLBACK_FUNCTION;

  buf_t *this = *thisp;
  ed_t *ed = $my(root);

  readline_t *rl = Ed.readline.new (ed);
  string_t *str = String.new_with_fmt ("spell --range=%d,%d", fidx + 1, lidx + 1);
  Readline.set.visibility (rl, NO);
  Readline.set.line (rl, str->bytes, str->num_bytes);
  String.release (str);
  Readline.parse (rl);

  int retval = OK;

  if (SPELL_OK is (retval = buf_spell (thisp, rl)))
    Readline.history.push (rl);
  else
    Readline.release (rl);

  return retval;
}

static int buf_spell_readline_cb (buf_t **thisp, readline_t *rl, utf8 c) {
  (void) thisp; (void) c;

  int retval = RLINE_NO_COMMAND;
  string_t *com = Readline.get.command (rl);

  ifnot (Cstring.eq (com->bytes, "spell"))
    goto theend;

  retval = buf_spell (thisp, rl);

theend:
  String.release (com);
  return retval;
}

static void ed_set_readline_cb (ed_t *this, BufReadline_cb cb) {
  $my(num_readline_cbs)++;
  ifnot ($my(num_readline_cbs) - 1)
    $my(readline_cbs) = Alloc (sizeof (BufReadline_cb));
  else
    $my(readline_cbs) = Realloc ($my(readline_cbs), sizeof (BufReadline_cb) * $my(num_readline_cbs));

  $my(readline_cbs)[$my(num_readline_cbs) - 1] = cb;
}

static void ed_release_readline_cbs (ed_t *this) {
  ifnot ($my(num_readline_cbs)) return;
  free ($my(readline_cbs));
}

/* This is for normal mode. Better not to mix those two. This is for convenience
 * when the keyboard layout has been changed through the system */
static void ed_set_lang_map (ed_t *this, int lmap[][26]) {
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < ('z' - 'a') + 1; j++)
      $my(lmap)[i][j] = lmap[i][j];
}

/* As this intented for insert mode, though it might be used elsewhere. */
static void ed_set_lang_getkey (ed_t *this, LangGetKey_cb cb) {
  $my(lang_getkey) = cb;
}

static void ed_set_lang_mode (ed_t *this, const char *lang_mode) {
  size_t len = bytelen (lang_mode);
  if (len > 7) return;

  Cstring.cp ($my(lang_mode), 8, lang_mode, len);
}

static void ed_set_normal_on_g_cb (ed_t *this, BufNormalOng_cb cb) {
  $my(num_on_normal_g_cbs)++;
  ifnot ($my(num_on_normal_g_cbs) - 1)
    $my(on_normal_g_cbs) = Alloc (sizeof (BufNormalOng_cb));
  else
    $my(on_normal_g_cbs) = Realloc ($my(on_normal_g_cbs), sizeof (BufNormalOng_cb) * $my(num_on_normal_g_cbs));

  $my(on_normal_g_cbs)[$my(num_on_normal_g_cbs) - 1] = cb;
}

static void ed_release_on_normal_g_cbs (ed_t *this) {
  ifnot ($my(num_on_normal_g_cbs)) return;
  free ($my(on_normal_g_cbs));
}

static void ed_set_record_cb (ed_t *this, Record_cb cb) {
  $my(record_cb) = cb;
}

static void ed_set_i_record_cb (ed_t *this, IRecord_cb cb) {
  $my(i_record_cb) = cb;
}

static void ed_set_init_record_cb (ed_t *this, InitRecord_cb cb) {
  $my(init_record_cb) = cb;
}

static void ed_set_expr_reg_cb (ed_t *this, ExprRegister_cb cb) {
  $my(num_expr_reg_cbs)++;
  ifnot ($my(num_expr_reg_cbs) - 1)
    $my(expr_reg_cbs) = Alloc (sizeof (ExprRegister_cb));
  else
    $my(expr_reg_cbs) = Realloc ($my(expr_reg_cbs), sizeof (ExprRegister_cb) * $my(num_expr_reg_cbs));

  $my(expr_reg_cbs)[$my(num_expr_reg_cbs) - 1] = cb;
}

static void ed_release_expr_reg_cbs (ed_t *this) {
  ifnot ($my(num_expr_reg_cbs)) return;
  free ($my(expr_reg_cbs));
}

static int buf_readline (buf_t **thisp, readline_t *rl) {
  buf_t *this = *thisp;

  int retval = NOTHING_TODO;

  int is_special_win = win_isit_special_type ($my(parent));

  if (rl->state & READLINE_EXEC) goto exec;

  rl = Readline.edit (rl);

  if (rl->c isnot '\r') goto theend;

exec:
  rl->state &= ~READLINE_EXEC;

  if (rl->line->head is NULL or rl->line->head->data->bytes[0] is ' ')
    goto theend;

  Readline.parse (rl);

  for (int i = 0; i < $myroots(num_readline_cbs); i++) {
    retval = $myroots(readline_cbs)[i] (thisp, rl, rl->com);
    if (retval isnot RLINE_NO_COMMAND) goto theend;
  }

redo:
  switch (rl->com) {
    case VED_COM_WRITE_FORCE_ALIAS:
      rl->com = VED_COM_WRITE_FORCE;
      goto redo;

    case VED_COM_WRITE_FORCE_FORCE_ALIAS:
      rl->com = VED_COM_WRITE_FORCE_FORCE;
      goto redo;

    case VED_COM_WRITE_ALIAS:
      rl->com = VED_COM_WRITE;

    // fall through
    case VED_COM_WRITE_FORCE_FORCE:
    case VED_COM_WRITE_FORCE:
    case VED_COM_WRITE:
      if ($my(enable_writing)) {
        readline_arg_t *fname = Readline.get.arg (rl, READLINE_ARG_FILENAME);
        readline_arg_t *range = Readline.get.arg (rl, READLINE_ARG_RANGE);
        readline_arg_t *append = Readline.get.arg (rl, READLINE_ARG_APPEND);
        if (NULL is fname) {
          if (is_special_win) goto theend;
          if (NULL isnot range or NULL isnot append) goto theend;
          retval = self(write,
              (rl->com is VED_COM_WRITE ? 0 :
                (rl->com is VED_COM_WRITE_FORCE ? 1 : 2)));
        } else {
          if (NULL is range) {
            rl->range[0] = 0;
            rl->range[1] = this->num_items - 1;
          } else
            if (NOTOK is readline_parse_arg_buf_range (rl, range, this))
              goto theend;

          retval = buf_write_to_fname (this, fname->argval->bytes, NULL isnot append,
            rl->range[0], rl->range[1], VED_COM_WRITE_FORCE is rl->com, VERBOSE_ON);
        }
      } else
        Msg.error ($my(root), "writing is disabled, use :set --enable_writing to enable");

      goto theend;

    case VED_COM_EDIT_FORCE_ALIAS:
      rl->com = VED_COM_EDIT_FORCE;

    // fall through
    case VED_COM_EDIT_FORCE:
    case VED_COM_EDIT:
    case VED_COM_EDIT_ALIAS:
      if (is_special_win) goto theend;
      if ($my(is_sticked)) goto theend;
      {
        readline_arg_t *fname = Readline.get.arg (rl, READLINE_ARG_FILENAME);
        retval = Win.edit_fname ($my(parent), thisp, (NULL is fname ? NULL: fname->argval->bytes),
           $myparents(cur_frame), VED_COM_EDIT_FORCE is rl->com, 1, 1);
      }
      goto theend;

    case VED_COM_ETAIL:
      retval = win_edit_fname ($my(parent), thisp, NULL, $myparents(cur_frame), 1, 0, 1);
      self(normal.eof, DONOT_DRAW);
      goto theend;

    case VED_COM_QUIT_FORCE_ALIAS:
      rl->com = VED_COM_QUIT_FORCE;

    // fall through
    case VED_COM_QUIT_FORCE:
    case VED_COM_QUIT:
    case VED_COM_QUIT_ALIAS:
      retval = ed_quit ($my(root), VED_COM_QUIT_FORCE is rl->com,
          Readline.arg.exists (rl, "global"));
      goto theend;

    case VED_COM_WRITE_QUIT:
    case VED_COM_WRITE_QUIT_FORCE:
      self(write, NO_FORCE);
      retval = ed_quit ($my(root), VED_COM_WRITE_QUIT_FORCE is rl->com,
          Readline.arg.exists (rl, "global"));
      goto theend;

    case VED_COM_EDNEW:
       {
         readline_arg_t *fname = Readline.get.arg (rl, READLINE_ARG_FILENAME);
         if (NULL isnot fname)
           String.replace_with ($myroots(ed_str), fname->argval->bytes);
         else
           String.replace_with ($myroots(ed_str), UNNAMED);

          retval = EXIT_THIS;
          $myroots(state) |= ED_NEW;
        }

        goto theend;

    case VED_COM_EDNEXT:
      retval = EXIT_THIS;
      $myroots(state) |= ED_NEXT;
      goto theend;

    case VED_COM_EDPREV:
      retval = EXIT_THIS;
      $myroots(state) |= ED_PREV;
      goto theend;

    case VED_COM_EDPREV_FOCUSED:
      retval = EXIT_THIS;
      $myroots(state) |= ED_PREV_FOCUSED;
      goto theend;

    case VED_COM_ENEW:
       {
         readline_arg_t *fname = Readline.get.arg (rl, READLINE_ARG_FILENAME);
         if (NULL isnot fname)
           retval = buf_enew_fname (thisp, fname->argval->bytes);
         else
           retval = buf_enew_fname (thisp, UNNAMED);
        }
        goto theend;

    case VED_COM_TTY_SCREEN:
      ed_tty_screen ($my(root));
      retval = DONE;
      goto theend;

    case VED_COM_REDRAW:
       Win.draw ($my(root)->current);
       retval = DONE;
       goto theend;

    case VED_COM_BUF_CHECK_BALANCED:
      if ($my(ftype)->balanced isnot NULL) {
        int range[2];
        if (NOTOK is readline_get_buf_range (rl, this, range)) {
          range[0] = 0;
          range[1] = this->num_items - 1;
        }

        retval = $my(ftype)->balanced (thisp, range[0], range[1]);
      }
      goto theend;

    case VED_COM_MESSAGES:
      retval = Ed.messages ($my(root), thisp, AT_EOF);
      goto theend;

    case VED_COM_SCRATCH:
      retval = Ed.scratch ($my(root), thisp, AT_EOF);
      goto theend;

    case VED_COM_SEARCHES:
      retval = ed_buf_change ($my(root), thisp, VED_SEARCH_WIN, VED_SEARCH_BUF);
      goto theend;

    case VED_COM_DIFF_BUF:
      retval = ed_buf_change ($my(root), thisp, VED_DIFF_WIN, VED_DIFF_BUF);
      goto theend;

    case VED_COM_DIFF:
      retval = buf_com_diff (thisp, rl, 0);
      goto theend;

    case VED_COM_GREP:
      {
        readline_arg_t *pat = Readline.get.arg (rl, READLINE_ARG_PATTERN);
        if (NULL is pat) break;

        Vstring_t *fnames = Readline.get.arg_fnames (rl, -1);
        dirlist_t *dlist = NULL;

        if (NULL is fnames) {
          dlist = Dir.list (".", 0);
          if (NULL is dlist) break;
          fnames = dlist->list;
        }

        readline_arg_t *rec = Readline.get.arg (rl, READLINE_ARG_RECURSIVE);
        dirwalk_t *dw = NULL;

        ifnot (NULL is rec) {
          dw = Dir.walk.new (NULL, NULL);
          dw->depth = DIRWALK_MAX_DEPTH;
          vstring_t *it = fnames->head;
          while (it) {
            Dir.walk.run (dw, it->data->bytes);
            it = it->next;
          }

          ifnot (NULL is dlist) {
            dlist->release (dlist);
            dlist = NULL;
          } else
            Vstring.release (fnames);

          fnames = dw->files;
        }

        retval = buf_grep (thisp, pat->argval->bytes, fnames);

        ifnot (NULL is dlist)
          dlist->release (dlist);
        else
          if (NULL is dw)
            Vstring.release (fnames);

        ifnot (NULL is dw)
          Dir.walk.release (&dw);
      }
      goto theend;

    case VED_COM_SPLIT:
      {
        if (is_special_win) goto theend;
        readline_arg_t *fname = Readline.get.arg (rl, READLINE_ARG_FILENAME);
        if (NULL is fname)
          retval = buf_split (thisp, UNNAMED);
        else
          retval = buf_split (thisp, fname->argval->bytes);
      }
      goto theend;


    case VED_COM_READ:
    case VED_COM_READ_ALIAS:
      {
        readline_arg_t *fname = Readline.get.arg (rl, READLINE_ARG_FILENAME);
        if (NULL is fname) goto theend;
        retval = buf_read_from_file (this, fname->argval->bytes);
      }
      goto theend;

    case VED_COM_SHELL:
    case VED_COM_READ_SHELL:
      {
        string_t *com = Vstring.join (rl->line, "");
        String.delete_numbytes_at (com, (rl->com is VED_COM_SHELL ? 1 : 3), 0);
        retval = buf_read_from_shell (this, com->bytes, rl->com, ED_PROC_WAIT_AT_END);
        if (retval > OK) {
          Ed.append.message_fmt ($my(root), "%s exit_status %d\n", com->bytes, retval);
          retval = OK; // in case command exit_status is > 0
          // as on internal error NOTOK is returned
        }

        String.release (com);
      }
      goto theend;

    case VED_COM_SUBSTITUTE:
    case VED_COM_SUBSTITUTE_WHOLE_FILE_AS_RANGE:
    case VED_COM_SUBSTITUTE_ALIAS:
      retval = buf_com_substitute (*thisp, rl, &retval);
      goto theend;

    case VED_COM_BUF_CHANGE_PREV_ALIAS:
      rl->com = VED_COM_BUF_CHANGE_PREV;

    // fall through
    case VED_COM_BUF_CHANGE_PREV:
      if ($my(flags) & BUF_IS_SPECIAL) goto theend;
      if ($my(is_sticked)) goto theend;
      retval = buf_change (thisp, rl->com);
      goto theend;

    case VED_COM_BUF_CHANGE_NEXT_ALIAS:
      rl->com = VED_COM_BUF_CHANGE_NEXT;

    // fall through
    case VED_COM_BUF_CHANGE_NEXT:
      if ($my(flags) & BUF_IS_SPECIAL) goto theend;
      if ($my(is_sticked)) goto theend;
      retval = buf_change (thisp, rl->com);
      goto theend;

    case VED_COM_BUF_CHANGE_PREV_FOCUSED_ALIAS:
      rl->com = VED_COM_BUF_CHANGE_PREV_FOCUSED;

    // fall through
    case VED_COM_BUF_CHANGE_PREV_FOCUSED:
      if (is_special_win) goto theend;
      if ($my(is_sticked)) goto theend;
      retval = buf_change (thisp, rl->com);
      goto theend;

    case VED_COM_BUF_CHANGE_ALIAS:
      rl->com = VED_COM_BUF_CHANGE;

    // fall through
    case VED_COM_BUF_CHANGE:
      if ($my(flags) & BUF_IS_SPECIAL) goto theend;
      if ($my(is_sticked)) goto theend;
      {
        readline_arg_t *bufname = Readline.get.arg (rl, READLINE_ARG_BUFNAME);
        if (NULL is bufname) goto theend;
        retval = buf_change_bufname (thisp, bufname->argval->bytes);
      }
      goto theend;

    case VED_COM_BUF_SET:
      retval = buf_com_set (*thisp, rl);
      goto theend;

    case VED_COM_BUF_BACKUP:
      retval = buf_com_backupfile (*thisp);
      goto theend;

    case VED_COM_WIN_CHANGE_PREV_ALIAS:
      rl->com = VED_COM_WIN_CHANGE_PREV;

    // fall through
    case VED_COM_WIN_CHANGE_PREV:
      retval = Ed.win.change ($my(root), thisp, rl->com, NULL, 0, NO_FORCE);
      goto theend;

    case VED_COM_WIN_CHANGE_NEXT_ALIAS:
      rl->com = VED_COM_WIN_CHANGE_NEXT;

    // fall through
    case VED_COM_WIN_CHANGE_NEXT:
      retval = Ed.win.change ($my(root), thisp, rl->com, NULL, 0, NO_FORCE);
      goto theend;

    case VED_COM_WIN_CHANGE_PREV_FOCUSED_ALIAS:
      rl->com = VED_COM_WIN_CHANGE_PREV_FOCUSED;

    // fall through
    case VED_COM_WIN_CHANGE_PREV_FOCUSED:
      retval = Ed.win.change ($my(root), thisp, rl->com, NULL, 0, NO_FORCE);
      goto theend;

    case VED_COM_BUF_DELETE_FORCE_ALIAS:
      rl->com = VED_COM_BUF_DELETE_FORCE;

    // fall through
    case VED_COM_BUF_DELETE_FORCE:
    case VED_COM_BUF_DELETE_ALIAS:
    case VED_COM_BUF_DELETE:
      {
        int idx;
        readline_arg_t *bufname = Readline.get.arg (rl, READLINE_ARG_BUFNAME);
        if (NULL is bufname)
          idx = $my(parent)->cur_idx;
        else
          if (NULL is Win.get.buf_by_name ($my(parent), bufname->argval->bytes, &idx))
            idx = $my(parent)->cur_idx;

        retval = buf_delete (thisp, idx, rl->com is VED_COM_BUF_DELETE_FORCE);
      }
      goto theend;

    case VED_COM_TEST_KEY:
      buf_test_key (this);
      retval = DONE;
      goto theend;

    case VED_COM_SAVE_IMAGE:
      retval = buf_com_save_image (this, rl);
      goto theend;

    case VED_COM_EDIT_IMAGE:
      retval = buf_com_edit_image (thisp, rl);
      goto theend;

    case VED_COM_VALIDATE_UTF8:
      retval = buf_com_validate_utf8 (thisp, rl);
      goto theend;

    default:
     // for (int i = 0; i < $myroots(num_readline_cbs); i++) {
     //   retval = $myroots(readline_cbs)[i] (thisp, rl, rl->com);
     //   if (retval isnot RLINE_NO_COMMAND) break;
     // }
      goto theend;
  }

theend:
  this = *thisp; // as thisp can be modified, "this" might end up as NULL reference

  Readline.clear (rl);

  ifnot (DONE is retval)
    Readline.release (rl);
  else {
    rl->state &= ~READLINE_CLEAR_FREE_LINE;
    Readline.history.push (rl);
    Readline.last_component_push (rl);
  }

  return retval;
}

static buf_t *buf_insert_char_rout (buf_t *this, utf8 c, string_t *cur_insert) {
  int orig_col = $mycur(cur_col_idx)++;
  int width = 1;
  char buf[5];

  if ('~' >= c and c >= ' ') {
     buf[0] = c;
     buf[1] = '\0';
  } else if (c is '\t') {
    if ($my(ftype)->tab_indents is 0 or ($my(state) & ACCEPT_TAB_WHEN_INSERT)) {
      $my(state) &= ~ACCEPT_TAB_WHEN_INSERT;
      buf[0] = c;
      buf[1] = '\0';
      width = $my(ftype)->tabwidth;
    } else {
      width = $my(ftype)->shiftwidth;
      $mycur(cur_col_idx) += $my(ftype)->shiftwidth - 1;
      int i = 0;
      for (; i < $my(ftype)->shiftwidth; i++) buf[i] = ' ';
      buf[i] = '\0';
    }
  } else {
    $my(state) &= ~ACCEPT_TAB_WHEN_INSERT;
    $mycur(cur_col_idx)++;
    if (c < 0x800) {
      buf[0] = (c >> 6) | 0xC0;
      buf[1] = (c & 0x3F) | 0x80;
      buf[2] = '\0';
      width = Ustring.width (buf, $my(ftype)->tabwidth);
    } else {
      $mycur(cur_col_idx)++;
      if (c < 0x10000) {
        buf[0] = (c >> 12) | 0xE0;
        buf[1] = ((c >> 6) & 0x3F) | 0x80;
        buf[2] = (c & 0x3F) | 0x80;
        buf[3] = '\0';
        width = Ustring.width (buf, $my(ftype)->tabwidth);
      } else if (c < 0x110000) {
        $mycur(cur_col_idx)++;
        buf[0] = (c >> 18) | 0xF0;
        buf[1] = ((c >> 12) & 0x3F) | 0x80;
        buf[2] = ((c >> 6) & 0x3F) | 0x80;
        buf[3] = (c & 0x3F) | 0x80;
        buf[4] = '\0';
        width = Ustring.width (buf, $my(ftype)->tabwidth);
      }
    }
  }

  String.insert_at_with ($mycur(data), orig_col, buf);
  String.append_with (cur_insert, buf);
  if ($my(cur_video_col) is $my(dim)->num_cols or
      $my(cur_video_col) + width > $my(dim)->num_cols) {
    $mycur(first_col_idx) = $mycur(cur_col_idx);
    $my(video)->col_pos = $my(cur_video_col) = $my(video)->first_col;
  } else
    $my(video)->col_pos = $my(cur_video_col) = $my(cur_video_col) + width;

  return this;
}

static int buf_insert_reg (buf_t **thisp, string_t *cur_insert) {
  buf_t *this = *thisp;
  MSG ("insert register (charwise mode):");
  int regidx = ed_reg_get_idx ($my(root), Input.getkey (STDIN_FILENO));
  if (NOTOK is regidx) return NOTHING_TODO;

  if (ERROR is ed_reg_special_set ($my(root), this, regidx))
    return NOTHING_TODO;

  Reg_t *rg = &$my(regs)[regidx];
  if (rg->type is LINEWISE) return NOTHING_TODO;

  reg_t *reg = rg->head;
  if (NULL is reg) return NOTHING_TODO;

  utf8 c = 0;
  while (reg isnot NULL) {
    char *sp = reg->data->bytes;
    while (*sp) {
      int clen = Ustring.charlen ((uchar) *sp);
      c = Ustring.get.code (sp);
      sp += clen;
      this = buf_insert_char_rout (this, c, cur_insert);
    }
    reg = reg->next;
  }

  $my(flags) |= BUF_IS_MODIFIED;
  self(draw_current_row);
  return DONE;
}

static utf8 ed_lang_getkey (ed_t *this) {
  if (NULL is $my(lang_getkey) or Cstring.eq ($my(lang_mode), "en"))
    return Input.getkey (STDIN_FILENO);

  return $my(lang_getkey) (this, $my(lang_mode));
}

static int buf_insert_mode (buf_t **thisp, utf8 com, char *bytes) {
  buf_t *this = *thisp;
  utf8 c = 0;
  if (com is '\n') {
    com = 'i';
    c = '\n';
  }

  String.clear ($my(cur_insert));

  char prev_mode[MAXLEN_MODE];
  Cstring.cp (prev_mode, MAXLEN_MODE, $my(mode), MAXLEN_MODE - 1);

  Action_t *Action = self(Action.new);
  self(Action.set_with_current, Action, REPLACE_LINE);

  if (com is 'A' or ((com is 'a' or com is 'C') and $mycur(cur_col_idx) is (int)
      $mycur(data)->num_bytes - Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)]))) {
    ADD_TRAILING_NEW_LINE;

    self(normal.eol, DRAW);
  } else if (com is 'a')
    self(normal.right, 1, DRAW);

  if (com is 'i' or com is 'a') {
    int width = Ustring.width ($mycur(data)->bytes + $mycur(cur_col_idx),
        $my(ftype)->tabwidth);

    if (width > 1) {
      if ($my(cur_video_col) - width + 1 < 1) {
        Ustring.encode ($my(line), $mycur(data)->bytes, $mycur(data)->num_bytes,
            CLEAR, $my(ftype)->tabwidth, $mycur(first_col_idx));
        ustring_t *it = $my(line)->current;
        while ($mycur(first_col_idx) > 0 and $my(cur_video_col) < 1) {
          $mycur(first_col_idx) -= it->len;
          $my(video)->col_pos = $my(cur_video_col) =
            $my(cur_video_col) - it->width + 1;
          it = it->prev;
        }
      } else
        $my(video)->col_pos = $my(cur_video_col) =
          $my(cur_video_col) - width + 1;
      self(draw_current_row);
    }
  }

  if (bytes isnot NULL) {
    size_t blen = bytelen (bytes);
    for (size_t i = 0; i < blen; i++)
      this = buf_insert_char_rout (this, bytes[i], $my(cur_insert));

    $my(flags) |= BUF_IS_MODIFIED;
    self(draw_current_row);
    goto theend;
  }

  Cstring.cp ($my(mode), MAXLEN_MODE, INSERT_MODE, MAXLEN_MODE - 1);
  self(set.mode, INSERT_MODE);

  if ($my(show_statusline) is UNSET) buf_set_draw_statusline (this);
  buf_set_draw_topline (this);

  if (c isnot 0) goto handle_char;

theloop:
  for (;;) {

get_char:
    ed_check_msg_status ($my(root));

    c = ed_lang_getkey ($my(root));

handle_char:
    if (c > 0x7f)
      if (c < 0x0a0 or (c >= FN_KEY(1) and c <= FN_KEY(12)) or c is INSERT_KEY)
        continue;

    if ((c is '\t') or (c >= ' ' and c <= '~') or
       (c > 0x7f and ((c < ARROW_DOWN_KEY) or (c > HOME_KEY and
       (c isnot DELETE_KEY) and (c isnot PAGE_DOWN_KEY) and (c isnot PAGE_UP_KEY)
        and (c isnot END_KEY)))))
      goto new_char;

    switch (c) {
      case ESCAPE_KEY:
        goto theend;

      case CTRL('n'):
        buf_complete_word (thisp);
        goto get_char;

      case CTRL('x'):
        buf_handle_ctrl_x (thisp);
        goto get_char;

      case CTRL('a'):
        buf_insert_last_insert (this);
        goto get_char;

      case CTRL('v'):
        if (NEWCHAR is buf_insert_character (this, &c))
          goto new_char;
        goto get_char;

      case CTRL('r'):
        buf_insert_reg (thisp, $my(cur_insert));
        goto get_char;

      case CTRL('k'):
        if (NEWCHAR is buf_complete_digraph (this, &c))
          goto new_char;
        goto get_char;

      case CTRL('y'):
      case CTRL('e'):
        if (NEWCHAR is buf_insert_handle_ud_line_completion (this, &c)) {
          $my(state) |= ACCEPT_TAB_WHEN_INSERT;
          goto new_char;
        }

        goto get_char;

      case BACKSPACE_KEY:
        if ($mycur(cur_col_idx)) {
          int retv = self(normal.left, 1, DONOT_DRAW);
          if (NOTHING_TODO is self(normal.delete, 1, -1, DRAW))
            if (retv) self(draw_current_row);
          goto get_char;
        }

        if (0 is $my(ftype)->backspace_on_insert_goes_up_and_join or
            this->cur_idx is 0)
          goto get_char;


        buf_insert_change_line (this, ARROW_UP_KEY, &Action, DONOT_DRAW);

        self(normal.eol, DONOT_DRAW);
        ADD_TRAILING_NEW_LINE;

        if (DONE is self(normal.join, DONOT_DRAW)) {
          Action_t *Laction = self(undo.pop);
          action_t *laction = Laction->head->next;
          self(action.release, laction);
          ListStackPush (Action, Laction->head);
          free (Laction);
        }

        self(normal.right, 1, DONOT_DRAW);
        self(draw);

        goto get_char;

      case DELETE_KEY:
        if ($mycur(data)->num_bytes is 0 or
          // HAS_THIS_LINE_A_TRAILING_NEW_LINE or
            $mycur(data)->bytes[$mycur(cur_col_idx)] is 0 or
            $mycur(data)->bytes[$mycur(cur_col_idx)] is '\n') {
          if (HAS_THIS_LINE_A_TRAILING_NEW_LINE) {
            if ($mycur(cur_col_idx) + Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)])
                is (int) $mycur(data)->num_bytes)
              self(normal.left, 1, DONOT_DRAW);
            RM_TRAILING_NEW_LINE;
          }

          if (DONE is self(normal.join, DONOT_DRAW)) {
            action_t *laction = ListStackPop (Action, action_t);
            self(action.release, laction);
          }

          if ($mycur(cur_col_idx) is (int) $mycur(data)->num_bytes -
              Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)]))
            ADD_TRAILING_NEW_LINE;

          if ($mycur(cur_col_idx) isnot 0 or
              NOTHING_TODO is self(normal.right, 1, DRAW))
            self(draw);

          goto get_char;
        }

        self(normal.delete, 1, -1, DRAW);

        goto get_char;

      case HOME_KEY:
        self(normal.bol, DRAW);
        goto get_char;

      case END_KEY:
        ADD_TRAILING_NEW_LINE;
        self(normal.eol, DRAW);
        goto get_char;

      case ARROW_RIGHT_KEY:
        if($mycur(cur_col_idx) is (int) $mycur(data)->num_bytes -
            Ustring.charlen ((uchar) $mycur(data)->bytes[$mycur(cur_col_idx)]))
          ADD_TRAILING_NEW_LINE;
        self(normal.right, 1, DRAW);
        goto get_char;

      case ARROW_LEFT_KEY:
        self(normal.left, 1, DRAW);
        goto get_char;

      case ARROW_UP_KEY:
      case ARROW_DOWN_KEY:
      case PAGE_UP_KEY:
      case PAGE_DOWN_KEY:
      case  '\r':
      case  '\n':
        buf_insert_change_line (this, c, &Action, DRAW);

        if ('\r' is c and $my(cur_insert)->num_bytes) {
          String.replace_with ($my(last_insert), $my(cur_insert)->bytes);
          String.clear ($my(cur_insert));
        }

        if ('\n' is c) goto theend;

        goto theloop;

      default:
        goto get_char;
    }

new_char:
    this = buf_insert_char_rout (this, c, $my(cur_insert));
    $my(flags) |= BUF_IS_MODIFIED;
    self(draw_current_row);
    goto get_char;
  }

theend:
  self(normal.left, 1, DRAW);
  if ($mycur(data)->num_bytes)
    RM_TRAILING_NEW_LINE;
  self(set.mode, prev_mode);

  if ($mycur(data)->num_bytes) {
    $my(video)->col_pos = $my(cur_video_col) = $my(cur_video_col) +
       (Ustring.width ($mycur(data)->bytes + $mycur(cur_col_idx),
       $my(ftype)->tabwidth) - 1);
  }

  if ($my(cur_video_col) > $my(dim)->num_cols) {
    $mycur(first_col_idx) = $mycur(cur_col_idx);
      $my(video)->col_pos = $my(cur_video_col) =
        Ustring.width ($mycur(data)->bytes + $mycur(cur_col_idx),
           $my(ftype)->tabwidth - 1);
  }

  self(draw_current_row);

  buf_set_draw_topline (this);

  if ($my(cur_insert)->num_bytes)
    String.replace_with ($my(last_insert), $my(cur_insert)->bytes);

  if (NULL isnot Action->head)
    self(undo.push, Action);
  else
    free (Action);

  if ($my(flags) & BUF_IS_MODIFIED)
    if ($my(autosave) > 0) {
      long cur_sec = $my(saved_sec);
      $my(saved_sec) = Sys.get.clock_sec (DEFAULT_CLOCK);
      if (cur_sec > 0) {
        if ($my(saved_sec) - cur_sec > $my(autosave))
          self(write, FORCE);
        else
          $my(saved_sec) = cur_sec;
      }
    }

  return DONE;
}

static buf_t *ed_get_current_buf (ed_t *this) {
  return this->current->current;
}

static win_t *ed_get_current_win (ed_t *this) {
  return this->current;
}

static int ed_get_current_win_idx (ed_t *this) {
  return this->cur_idx;
}

static win_t *ed_get_win_by_name (ed_t *this, const char *name, int *idx) {
  if (NULL is name) return NULL;
  win_t *it = this->head;
  *idx = 0;
  while (it) {
    if (Cstring.eq ($from(it, name), name)) return it;
    (*idx)++;
    it = it->next;
  }

  return NULL;
}

static win_t *ed_get_win_by_idx (ed_t *this, int idx) {
  if (idx >= this->num_items or idx < 0) return NULL;
  int i = 0;
  win_t *it = this->head;
  while (it) {
    if (i++ is idx) return it;
    it = it->next;
  }

  return NULL;
}

static term_t *ed_get_term (ed_t *this) {
  return $my(term);
}

static void *ed_get_callback_fun (ed_t *this, const char *fun) {
  (void) this;
  if (Cstring.eq (fun, "autoindent_c")) return buf_autoindent_c;
  if (Cstring.eq (fun, "autoindent_default")) return buf_ftype_autoindent;
  return NULL;
}

static string_t *ed_get_topline (ed_t *this) {
  return $my(topline);
}

static video_t *ed_get_video (ed_t *this) {
  return $my(video);
}

static int ed_get_state (ed_t *this) {
  return $my(state);
}

static win_t *ed_get_win_head (ed_t *this) {
  return this->head;
}

static win_t *ed_get_win_next (ed_t *this, win_t *w) {
  (void) this;
  return w->next;
}

static int ed_get_num_win (ed_t *this, int count_special) {
  if (count_special) return this->num_items;
  int num = 0;
  win_t *it = this->head;
  while (it) {
    ifnot (win_isit_special_type (it)) num++;
    it = it->next;
  }
  return num;
}

static int ed_get_min_rows (ed_t *this) {
  (void) this;
  return MIN_ROWS;
}

static int ed_get_num_special_win (ed_t *this) {
  return $my(num_special_win);
}

static buf_t *ed_get_bufname (ed_t *this, char *fname) {
  buf_t *buf = NULL;
  win_t *w = this->head;
  int idx;
  while (w) {
    buf = Win.get.buf_by_name (w, fname, &idx);
    if (buf) return buf;
    w = w->next;
  }

  return buf;
}

static win_t *ed_set_current_win (ed_t *this, int idx) {
  int cur_idx = this->cur_idx;
  if (EINDEX isnot DListSetCurrent (this, idx))
    this->prev_idx = cur_idx;
  return this->current;
}

static int ed_check_sanity (ed_t *this) {
  if ($my(dim)->num_rows < MIN_ROWS) return NOTOK;
  if ($my(dim)->num_cols < MIN_COLS) return NOTOK;
  return OK;
}

static void ed_set_screen_size (ed_t *this, screen_dim_opts opts) {
  int num_rows = opts.num_rows;
  int num_cols = opts.num_cols;
  int frow = opts.first_row;
  int fcol = opts.first_col;
  int last_row = opts.last_row;

  if (opts.init_term or num_rows is 0 or num_cols is 0) {
    Term.reset ($my(term));
    Term.set ($my(term));
  } else {
    $my(term)->num_rows = num_rows;
    $my(term)->num_cols = num_cols;
  }

  if (last_row < 1)
    last_row = frow + $my(term)->num_rows;

  ifnot (NULL is $my(dim)) {
    free ($my(dim));
    $my(dim) = NULL;
  }

  $my(dim) = ed_dim_new (this, frow, last_row, fcol, $my(term)->num_cols);

  $my(msg_row)     = $my(dim)->num_rows;
  $my(topline_row) = $my(dim)->first_row;
  $my(prompt_row)  = $my(msg_row) - 1;

  Video.release ($my(video));
  $my(video) = Video.new (OUTPUT_FD, $my(dim)->num_rows, $my(dim)->num_cols, $my(dim)->first_row, $my(dim)->first_col);
}

static void ed_set_exit_quick (ed_t *this, int val) {
  $my(exit_quick) = val;
}

static dim_t *ed_set_dim (ed_t *this, dim_t *dim, int f_row, int l_row,
                                                   int f_col, int l_col) {
  (void) this;
  return dim_set (dim, f_row, l_row, f_col, l_col);
}

static int ed_append_win (ed_t *this, win_t *w) {
  DListAppendCurrent (this, w);
  return this->cur_idx;
}

static string_t *ed_history_get_search_file (ed_t *this) {
  return $my(hs_file);
}

static string_t *ed_history_get_readline_file (ed_t *this) {
  return $my(hrl_file);
}

static string_t *ed_history_set_search_file (ed_t *this, const char *file) {
  string_t *hs_file = $my(hs_file);
  ifnot (NULL is file) {
    if (NULL is hs_file)
      hs_file = String.new_with (file);
    else
      hs_file = String.replace_with (hs_file, file);
  } else {
    char *data_dir = Sys.get.env_value ("E_DATADIR");
    if (NULL is hs_file)
      hs_file = String.new_with (data_dir);
    else
      hs_file = String.replace_with (hs_file, data_dir);

    String.append_with_fmt (hs_file, "/.%s_libe_hist_search",
        Sys.get.env_value ("USERNAME"));
  }

  $my(hs_file) = hs_file;

  return $my(hs_file);
}

static string_t *ed_history_set_readline_file (ed_t *this, const char *file) {
  string_t *hrl_file = $my(hrl_file);
  ifnot (NULL is file) {
    if (NULL is hrl_file)
      hrl_file = String.new_with (file);
    else
      hrl_file = String.replace_with (hrl_file, file);
  } else {
    char *data_dir = Sys.get.env_value ("E_DATADIR");
    if (NULL is hrl_file)
      hrl_file = String.new_with (data_dir);
    else
      hrl_file = String.replace_with (hrl_file, data_dir);

    String.append_with_fmt (hrl_file, "/.%s_libe_hist_readline",
        Sys.get.env_value ("USERNAME"));
  }

  $my(hrl_file) = hrl_file;

  return $my(hrl_file);
}

static void ed_history_add (ed_t *this, char *bytes, size_t num_bytes) {
  readline_t *rl = self(readline.new);
  Readline.insert_with_len (rl, bytes, num_bytes);
  Readline.history.push (rl);
}

static void ed_history_add_lines (ed_t *this, Vstring_t *hist, int what) {
  if (NULL is hist) return;


  if (what is RLINE_HISTORY) {
    vstring_t *it = hist->head;
    while (it) {
      char *sp = Cstring.trim.end (it->data->bytes, '\n');
      sp = Cstring.trim.end (sp, ' ');
      self(history.add, it->data->bytes, bytelen (sp));
      it = it->next;
    }
    return;
  }

  if (what is SEARCH_HISTORY) {
    vstring_t *it = hist->head;
    while (it) {
      ed_search_history_push (this, it->data->bytes, it->data->num_bytes);
      it = it->next;
    }
    return;
  }
}

static void ed_history_read (ed_t *this) {
  ifnot ($OurRoots(uid)) return;

  Vstring_t *lines = NULL;

  if (File.exists ($my(hs_file)->bytes)) {
    lines = File.readlines ($my(hs_file)->bytes, NULL, NULL, NULL);
    self(history.add_lines, lines, SEARCH_HISTORY);
  }

  Vstring.clear (lines);

  if (File.exists ($my(hrl_file)->bytes)) {
    File.readlines ($my(hrl_file)->bytes, lines, NULL, NULL);
    self(history.add_lines, lines, RLINE_HISTORY);
  }

  Vstring.release (lines);
}

static void ed_history_write (ed_t *this) {
  ifnot ($OurRoots(uid)) return;

  FILE *fp = fopen ($my(hs_file)->bytes, "w");
  if (NULL is fp) goto hrl_file;

  histitem_t *it = $my(history)->search->tail;
  while (it) {
    fprintf (fp, "%s\n", it->data->bytes);
    it = it->prev;
  }

  fclose (fp);

hrl_file:
  fp = fopen ($my(hrl_file)->bytes, "w");
  if (NULL is fp) return;

  readline_hist_item_t *hrl = $my(history)->readline->tail;
  while (hrl) {
    string_t *line = Vstring.join (hrl->data->line, "");
    fprintf (fp, "%s\n", line->bytes);
    String.release (line);
    hrl = hrl->prev;
  }

  fclose (fp);
}

static int buf_word_actions_cb (buf_t **thisp, int fidx, int lidx,
                  bufiter_t *it, char *word, utf8 c, char *action) {
  buf_t *this = *thisp;
  int type = TO_UPPER; (void) type;

  switch (c) {
    case '+':
    case '*':
      return buf_selection_to_X_word_actions_cb (thisp, fidx, lidx, it, word, c, action);

    case '`':
      ed_reg_new ($my(root), REG_SHARED);
      ed_reg_push_with ($my(root), REG_SHARED, CHARWISE,
          word, NORMAL_ORDER);
      return DONE;

    case '~': {
      size_t len = lidx - fidx + 1;
      char buf[len+1]; // though it alwars returns ok, this might change in future
      if (OK isnot Ustring.swap_case (buf, word, len)) // to support malformed wstrings
        return NOTHING_TODO;

      buf[len] = '\0';
      self(delete.word, REG_UNNAMED);
      return selfp(insert.mode, 0, buf);
    }
      return DONE;

    case 'L':
      type = TO_LOWER;

    // fall through
    case 'U': {
      size_t len = lidx - fidx + 1;
      char buf[len+1];
      ifnot (Ustring.change_case (buf, word, len, type))
        return NOTHING_TODO;
      buf[len] = '\0';
      self(delete.word, REG_UNNAMED);
      return selfp(insert.mode, 0, buf);
    }
  }

  return NOTHING_TODO;
}

static void ed_set_word_actions (ed_t *this, utf8 *chars, int len,
                                  const char *actions, WordActions_cb cb) {
  if (len <= 0) return;
  int tlen = $my(word_actions_chars_len) + len;

  ifnot ($my(word_actions_chars_len)) {
    $my(word_actions_chars) = Alloc (sizeof (int *) * len);
    $my(word_actions_cb) = Alloc (sizeof (WordActions_cb) * len);
  }
  else {
    $my(word_actions_chars) = Realloc ($my(word_actions_chars), sizeof (int *) * tlen);
    $my(word_actions_cb) = Realloc ($my(word_actions_cb), sizeof (WordActions_cb) * tlen);
  }

  if (NULL is cb) cb = buf_word_actions_cb;

  for (int i = $my(word_actions_chars_len), j = 0; i < tlen; i++, j++) {
    $my(word_actions_chars)[i] = chars[j];
    $my(word_actions_cb)[i] = cb;
  }

  $my(word_actions_chars_len) = tlen;
  $my(word_actions) = cstring_chop (actions, '\n', $my(word_actions), NULL, NULL);
}

static void ed_set_word_actions_default (ed_t *this) {
  $my(word_actions) = Vstring.new ();
  $my(word_actions_chars) = NULL;
  $my(word_actions_chars_len) = 0;
  utf8 chars[] = {'+', '*', '`', '~', 'L', 'U', 'S', ESCAPE_KEY};
  char actions[] =
    "+send selected word to XA_CLIPBOARD\n"
    "*send selected word to XA_PRIMARY\n"
    "`send selected word to shared register\n"
    "~swap case\n"
    "Lower (convert word to lower case)\n"
    "Upper (convert word to upper case)";

  self(set.word_actions, chars, ARRLEN(chars), actions, buf_word_actions_cb);

  if ($OurRoots(uid)) {
    utf8 spc[] = {'S'}; char spact[] = "Spell selected word";
    self(set.word_actions, spc, ARRLEN(spc), spact, buf_spell_word_cb);
  }

}

static void ed_set_cw_mode_actions (ed_t *this, utf8 *chars, int len,
                 char *actions, VisualCwMode_cb cb) {
  ifnot (len) return;
  int tlen = $my(cw_mode_chars_len) + len;

  ifnot ($my(cw_mode_chars_len)) {
    $my(cw_mode_chars) = Alloc (sizeof (int *) * len);
    $my(cw_mode_actions) = Cstring.dup (actions, bytelen (actions));
  } else {
    $my(cw_mode_chars) = Realloc ($my(cw_mode_chars), sizeof (int *) * tlen);
    size_t alen = bytelen (actions);
    size_t plen = bytelen ($my(cw_mode_actions));
    $my(cw_mode_actions) = Realloc ($my(cw_mode_actions), alen + plen + 2);
    $my(cw_mode_actions)[plen] = '\n';
    for (size_t i = plen + 1, j = 0; i < alen + plen + 2; i++, j++) {
      $my(cw_mode_actions)[i] = actions[j];
    }

    $my(cw_mode_actions)[alen + plen + 1] = '\0';
  }

  for (int i = $my(cw_mode_chars_len), j = 0; i < tlen; i++, j++)
    $my(cw_mode_chars)[i] = chars[j];
  $my(cw_mode_chars_len) = tlen;

  if (NULL is cb) return;

  $my(num_cw_mode_cbs)++;
  ifnot ($my(num_cw_mode_cbs) - 1)
    $my(cw_mode_cbs) = Alloc (sizeof (VisualCwMode_cb));
  else
    $my(cw_mode_cbs) = Realloc ($my(cw_mode_cbs), sizeof (VisualCwMode_cb) * $my(num_cw_mode_cbs));

  $my(cw_mode_cbs)[$my(num_cw_mode_cbs) -1] = cb;
}

static void ed_release_cw_mode_cbs (ed_t *this) {
  ifnot ($my(num_cw_mode_cbs)) return;
  free ($my(cw_mode_cbs));
}

static void ed_set_cw_mode_actions_default (ed_t *this) {
  utf8 chars[] = {'e', 'd', 'y', 'Y', '+', '*', '!', 033};
  char actions[] =
    "edit selected area as filename\n"
    "delete selected area\n"
    "yank selected area\n"
    "Yank selected and also send selected area to XA_PRIMARY\n"
    "+send selected area to XA_CLIPBOARD\n"
    "*send selected area to XA_PRIMARY\n"
    "!execute selected area as a system command";

  self(set.cw_mode_actions, chars, ARRLEN(chars), actions, NULL);

  if (getuid ()) {
    utf8 sp[] = {'S'}; char spact[] = "Spell selected";
    self(set.cw_mode_actions, sp, 1, spact, buf_spell_cw_mode_cb);
  }

  utf8 ex[] = {'$'}; char exact[] = "$evaluate selected area as an expression";
  self(set.cw_mode_actions, ex, 1, exact, buf_eval_expression);
}

static void ed_set_lw_mode_actions (ed_t *this, utf8 *chars, int len,
                                   char *actions, VisualLwMode_cb cb) {
  ifnot (len) return;
  int tlen = $my(lw_mode_chars_len) + len;

  ifnot ($my(lw_mode_chars_len)) {
    $my(lw_mode_chars) = Alloc (sizeof (int *) * len);
    $my(lw_mode_actions) = Cstring.dup (actions, bytelen (actions));
  } else {
    $my(lw_mode_chars) = Realloc ($my(lw_mode_chars), sizeof (int *) * tlen);
    size_t alen = bytelen (actions);
    size_t plen = bytelen ($my(lw_mode_actions));
    $my(lw_mode_actions) = Realloc ($my(lw_mode_actions), alen + plen + 2);
    $my(lw_mode_actions)[plen] = '\n';
    for (size_t i = plen + 1, j = 0; i < alen + plen + 2; i++, j++) {
      $my(lw_mode_actions)[i] = actions[j];
    }

    $my(lw_mode_actions)[alen + plen + 1] = '\0';
  }

  for (int i = $my(lw_mode_chars_len), j = 0; i < tlen; i++, j++)
    $my(lw_mode_chars)[i] = chars[j];
  $my(lw_mode_chars_len) = tlen;

  if (NULL is cb) return;

  $my(num_lw_mode_cbs)++;
  ifnot ($my(num_lw_mode_cbs) - 1)
    $my(lw_mode_cbs) = Alloc (sizeof (VisualLwMode_cb));
  else
    $my(lw_mode_cbs) = Realloc ($my(lw_mode_cbs), sizeof (VisualLwMode_cb) * $my(num_lw_mode_cbs));

  $my(lw_mode_cbs)[$my(num_lw_mode_cbs) -1] = cb;
}

static void ed_release_lw_mode_cbs (ed_t *this) {
  ifnot ($my(num_lw_mode_cbs)) return;
  free ($my(lw_mode_cbs));
}

static void ed_set_lw_mode_actions_default (ed_t *this) {
  utf8 chars[] = {'s', 'w', 'd', 'y', '>', '<', '+', '*', '`', '!', 033};
  char actions[] =
    "substitute command for the selected lines\n"
    "write selected lines to file\n"
    "delete selected lines\n"
    "yank selected lines\n"
    "Yank selected and also send selected lines to XA_PRIMARY\n"
    ">indent in\n"
    "<indent out\n"
    "+send selected lines to XA_CLIPBOARD\n"
    "*send selected lines to XA_PRIMARY\n"
    "`send selected lines to the shared register\n"
    "!execute selected lines as a system command";

  self(set.lw_mode_actions, chars, ARRLEN(chars), actions, NULL);

  utf8 bc[] = {'b'}; char bact[] = "balanced objects";
  self(set.lw_mode_actions, bc, 1, bact, buf_balanced_lw_mode_cb);

  utf8 vc[] = {'v'}; char vact[] = "validate string for invalid utf8 sequences";
  self(set.lw_mode_actions, vc, 1, vact, buf_validate_utf8_lw_mode_cb);

  if ($OurRoots(uid)) {
    utf8 ev[] = {'@'}; char evact[] = "@evaluate selected lines";
    self(set.lw_mode_actions, ev, 1, evact, buf_evaluate_lw_mode_cb);

    utf8 sp[] = {'S'}; char spact[] = "Spell selected lines";
    self(set.lw_mode_actions, sp, 1, spact, buf_spell_lw_mode_cb);
  }

}

static void ed_release_line_mode_cbs (ed_t *this) {
  ifnot ($my(num_line_mode_cbs)) return;
  free ($my(line_mode_cbs));
  free ($my(line_mode_actions));
  free ($my(line_mode_chars));
}

static void ed_set_line_mode_actions
(ed_t *this, utf8 *chars, int len, char *actions, LineMode_cb cb) {
  ifnot (len) return;
  int tlen = $my(line_mode_chars_len) + len;

  ifnot ($my(line_mode_chars_len)) {
    $my(line_mode_chars) = Alloc (sizeof (int *) * len);
    $my(line_mode_actions) = Cstring.dup (actions, bytelen (actions));
  } else {
    $my(line_mode_chars) = Realloc ($my(line_mode_chars), sizeof (int *) * tlen);
    size_t alen = bytelen (actions);
    size_t plen = bytelen ($my(line_mode_actions));
    $my(line_mode_actions) = Realloc ($my(line_mode_actions), alen + plen + 2);
    $my(line_mode_actions)[plen] = '\n';
    for (size_t i = plen + 1, j = 0; i < alen + plen + 2; i++, j++) {
      $my(line_mode_actions)[i] = actions[j];
    }

    $my(line_mode_actions)[alen + plen + 1] = '\0';
  }

  for (int i = $my(line_mode_chars_len), j = 0; i < tlen; i++, j++)
    $my(line_mode_chars)[i] = chars[j];
  $my(line_mode_chars_len) = tlen;

  if (NULL is cb) return;

  $my(num_line_mode_cbs)++;
  ifnot ($my(num_line_mode_cbs) - 1)
    $my(line_mode_cbs) = Alloc (sizeof (LineMode_cb));
  else
    $my(line_mode_cbs) = Realloc ($my(line_mode_cbs), sizeof (LineMode_cb) * $my(num_line_mode_cbs));

  $my(line_mode_cbs)[$my(num_line_mode_cbs) -1] = cb;
}

static int buf_line_mode_actions_cb (buf_t **thisp, utf8 c, char *action, char *line, size_t size) {
  buf_t *this = *thisp;

  int retval = NO_CALLBACK_FUNCTION;
  switch (c) {
    case '+':
    case '*':
      goto x_selection;

    case '`':
      ed_reg_new ($my(root), REG_SHARED);
      ed_reg_push_with ($my(root), REG_SHARED, LINEWISE,
          line, NORMAL_ORDER);
     return DONE;

    default:
      goto theend;
  }

x_selection:
  switch (action[1]) {
    case '-':
      ed_selection_to_X ($my(root), line, size,
          ('*' is c ? X_PRIMARY : X_CLIPBOARD));
      break;

    default:
      ed_selection_to_X ($my(root), STR_FMT ("%s\n", line), size + 1,
          ('*' is c ? X_PRIMARY : X_CLIPBOARD));
  }
  return DONE;

theend:
  return retval;
}

static void ed_set_line_mode_actions_default (ed_t *this) {
  ifnot ($OurRoots(uid)) return;

  utf8 chars[] = {'`', '+', '*'};
  char actions[] =
    "`send current line to shared register\n"
    "+send current line to XA_CLIPBOARD (a LN appended)\n"
    "+-send current line to XA_CLIPBOARD (NO LN appended)\n"
    "*send current line to XA_PRIMARY (a LN appended)\n"
    "*-send current line to XA_PRIMARY (NO LN appended)"
    ;

  self(set.line_mode_actions, chars, ARRLEN(chars), actions, buf_line_mode_actions_cb);
}

static void ed_release_file_mode_cbs (ed_t *this) {
  ifnot ($my(num_file_mode_cbs)) return;
  free ($my(file_mode_cbs));
  free ($my(file_mode_actions));
  free ($my(file_mode_chars));
}

static void ed_set_file_mode_actions (ed_t *this, utf8 *chars, int len,
                                       char *actions, FileActions_cb cb) {
  ifnot (len) return;
  int tlen = $my(file_mode_chars_len) + len;

  ifnot ($my(file_mode_chars_len)) {
    $my(file_mode_chars) = Alloc (sizeof (int *) * len);
    $my(file_mode_actions) = Cstring.dup (actions, bytelen (actions));
  } else {
    $my(file_mode_chars) = Realloc ($my(file_mode_chars), sizeof (int *) * tlen);
    size_t alen = bytelen (actions);
    size_t plen = bytelen ($my(file_mode_actions));
    $my(file_mode_actions) = Realloc ($my(file_mode_actions), alen + plen + 2);
    $my(file_mode_actions)[plen] = '\n';
    for (size_t i = plen + 1, j = 0; i < alen + plen + 2; i++, j++) {
      $my(file_mode_actions)[i] = actions[j];
    }

    $my(file_mode_actions)[alen + plen + 1] = '\0';
  }

  for (int i = $my(file_mode_chars_len), j = 0; i < tlen; i++, j++)
    $my(file_mode_chars)[i] = chars[j];
  $my(file_mode_chars_len) = tlen;

  if (NULL is cb) return;

  $my(num_file_mode_cbs)++;
  ifnot ($my(num_file_mode_cbs) - 1)
    $my(file_mode_cbs) = Alloc (sizeof (FileActions_cb));
  else
    $my(file_mode_cbs) = Realloc ($my(file_mode_cbs), sizeof (FileActions_cb) * $my(num_file_mode_cbs));

  $my(file_mode_cbs)[$my(num_file_mode_cbs) -1] = cb;
}

static int buf_file_mode_actions_cb (buf_t **thisp, utf8 c, char *action) {
  (void) action;
  buf_t *this = *thisp;

  int retval = NO_CALLBACK_FUNCTION;
  switch (c) {
    case 'w':
      retval = self(write, VED_COM_WRITE_FORCE);
      break;

     case 'v':
       retval = buf_com_validate_utf8 (thisp, NULL);
       break;

     case '@':
      retval = NOTOK;
      ifnot ($OurRoots(uid)) break;

      if (0 is (($my(flags) & BUF_IS_SPECIAL)) and
          0 is Cstring.eq ($my(basename), UNNAMED)) {
        Vstring_t *lines = File.readlines ($my(fname), NULL, NULL, NULL);
        char *buf = Vstring.to.cstring (lines, ADD_NL);
        if (NULL is buf)
          buf = self(get.contents, ADD_NL);

        retval = buf_interpret (thisp, buf);
        Vstring.release (lines);
        break;
      }

      if (Cstring.eq ($my(basename), UNNAMED) or
          Cstring.eq ($my(basename), VED_SCRATCH_BUF)) {
        char *buf = self(get.contents, ADD_NL);
        retval = buf_interpret (thisp, buf);
      }

      break;

    case 'S': {
        int flags = self(get.flags);
        if (0 is (flags & BUF_IS_SPECIAL) and
            0 is Cstring.eq (self(get.basename), UNNAMED)) {
          readline_t *rl = Ed.readline.new ($my(root));
          string_t *str = String.new_with ("spell --range=%");
          Readline.set.visibility (rl, NO);
          Readline.set.line (rl, str->bytes, str->num_bytes);
          String.release (str);
          Readline.parse (rl);

          if (SPELL_OK is (retval = buf_spell (thisp, rl)))
            Readline.history.push (rl);
          else
            Readline.release (rl);
        }
    }
      break;

    default:
      break;
  }

  return retval;
}

static void ed_set_file_mode_actions_default (ed_t *this) {
  ifnot ($OurRoots(uid)) return;

  utf8 chars[] = {'w', 'v', '@'};
  char actions[] =
     "write buffer\n"
     "validate file for invalid utf8 sequences\n"
     "@evaluate buffer\n"
     "Spell file";
  self(set.file_mode_actions, chars, ARRLEN(chars), actions, buf_file_mode_actions_cb);

}

static void ed_release_at_exit_cbs (ed_t *this) {
  ifnot ($my(num_at_exit_cbs)) return;
  free ($my(at_exit_cbs));
}

static void ed_set_at_exit_cb (ed_t *this, EdAtExit_cb cb) {
  if (NULL is cb) return;
  $my(num_at_exit_cbs)++;
  ifnot ($my(num_at_exit_cbs) - 1)
    $my(at_exit_cbs) = Alloc (sizeof (EdAtExit_cb));
  else
    $my(at_exit_cbs) = Realloc ($my(at_exit_cbs), sizeof (EdAtExit_cb) * $my(num_at_exit_cbs));

  $my(at_exit_cbs)[$my(num_at_exit_cbs) -1] = cb;
}

static void ed_syn_append (ed_t *this, syn_t syn) {
  $my(syntaxes)[$my(num_syntaxes)] = syn;
  ifnot (NULL is syn.keywords) {
    char chars[] = COLOR_CHARS;
    int arlen = 16;
    int keyword_colors[16] = {
        HL_IDENTIFIER, HL_KEYWORD, HL_COMMENT, HL_OPERATOR, HL_NUMBER,
        HL_STRING, HL_STRING_DELIM, HL_FUNCTION, HL_VARIABLE, HL_TYPE,
        HL_DEFINITION, HL_ERROR, HL_QUOTE, HL_QUOTE_1, HL_QUOTE_2, HL_NORMAL};

#define whereis_c(c_) ({                \
	int idx_ = arlen - 1;               \
    char *sp = Cstring.byte.in_str (chars, c_); \
    if (sp isnot NULL) idx_ = sp-chars; \
    idx_;                               \
})

    int num = 0;
    while (syn.keywords[num] isnot NULL) num++;
    $my(syntaxes)[$my(num_syntaxes)].keywords_len = Alloc (sizeof (size_t) * num);
    $my(syntaxes)[$my(num_syntaxes)].keywords_colors = Alloc (sizeof (size_t) * num);
    for (int i = 0; i < num; i++) {
      size_t len = bytelen (syn.keywords[i]);
      $my(syntaxes)[$my(num_syntaxes)].keywords_len[i] = len - 2;
      char c = syn.keywords[i][len-1];
      $my(syntaxes)[$my(num_syntaxes)].keywords_colors[i] =
         keyword_colors[whereis_c(c)];
    }
  }

  ifnot (NULL is $my(syntaxes)[$my(num_syntaxes)].multiline_comment_continuation)
    $my(syntaxes)[$my(num_syntaxes)].multiline_comment_continuation_len =
      bytelen ($my(syntaxes)[$my(num_syntaxes)].multiline_comment_continuation);
  $my(num_syntaxes)++;
#undef whereis_c
}

static void ed_init_syntaxes (ed_t *this) {
  for (size_t i = 0; i < ARRLEN(HL_DB); i++)
    self(syn.append, HL_DB[i]);
}

static void ed_release (ed_t *this) {
  if (this is NULL) return;

  win_t *w = this->head;
  win_t *next;

  while (w isnot NULL) {
    next = w->next;
    win_release (w);
    w = next;
  }

  if ($myprop isnot NULL) {
    for (int i = 0; i < $my(num_at_exit_cbs); i++)
      $my(at_exit_cbs)[i] (this);

    ed_release_at_exit_cbs (this);

    free ($my(name));
    free ($my(dim));
    free ($my(saved_cwd));

    String.release ($my(topline));
    String.release ($my(msgline));
    String.release ($my(last_insert));
    String.release ($my(ed_str));
    String.release ($my(hs_file));
    String.release ($my(hrl_file));
    Ustring.release ($my(uline));
    Vstring.release ($my(rl_last_component));
    Video.release ($my(video));

    history_release (&$my(history));

    for (int i = 0; i < NUM_REGISTERS; i++) {
      if (i is REG_SHARED) continue;
      reg_release (&$my(regs)[i]);
    }

    for (int i = 0; i <= NUM_RECORDS; i++)
      Vstring.release ($my(records)[i]);

    for (int i = 0; i < $my(num_syntaxes); i++) {
      free ($my(syntaxes)[i].keywords_len);
      free ($my(syntaxes)[i].keywords_colors);
    }

    free ($my(cw_mode_chars)); free ($my(cw_mode_actions));
    free ($my(lw_mode_chars)); free ($my(lw_mode_actions));
    free ($my(word_actions_chars));
    free ($my(word_actions_cb));

    ed_release_readline_cbs (this);
    ed_release_on_normal_g_cbs (this);
    ed_release_expr_reg_cbs (this);
    ed_release_lw_mode_cbs (this);
    ed_release_cw_mode_cbs (this);
    /* if there is a wonder about the tendency to alling lines
     * into the same group of functionality, is because in cases
     * like this (that introduced the line mode); this and one another
     * tendency to group the function(s|ality) close to its other scope,
     * allow me to follow and copy all the functions from the "old"
     * file mode function(s|lity), to develop a bright new functionality in a matter
     * of minutes and without think a bit, by just copying the file mode
     * function and substitute in visual mode "file/line", and
     * using for the rest (proper function calling) C-E in normal mode
     * (all that alling the line mode additions, previous to file mode) 
     */
    ed_release_line_mode_cbs (this);
    ed_release_file_mode_cbs (this);

    Vstring.release ($my(word_actions));

    ed_deinit_commands (this);

    free ($myprop);
  }

  free (this);
}

static int  ed_test_state_bit (ed_t *this, int bit) {
  return $my(state) & (bit);
}

static void ed_unset_state_bit (ed_t *this, int bit) {
  $my(state) &= ~(bit);
}

static void ed_set_state_bit (ed_t *this, int bit) {
  $my(state) |= (bit);
}

static void ed_set_state (ed_t *this, int state) {
  $my(state) = state;
}

static void ed_init_special_win (ed_t *this) {
  ed_get_scratch_buf (this);
  ed_msg_buf (this);
  ed_diff_buf (this);
  ed_search_buf (this);
  $my(num_special_win) = 4;
}

static int ed_i_record_default (ed_t *this, Vstring_t *rec) {
  char *str = Vstring.to.cstring (rec, ADD_NL);
  if (bytelen (str) is (size_t) $my(record_header_len)) return NOTOK;

  la_t *in = La.get.current ($my(__LA__));

  ifnot (in)
    in = La.init_instance ($my(__LA__), LaOpts(
      .define_funs_cb = $OurRoots (la_define_funs_cb),
      .syntax_error = i_syntax_error_to_ed));

  La.set.user_data (in, $OurRoot);

  int retval = La.eval_string (in, str);

  free (str);

  if (retval is LA_ERR_SYNTAX) {
    buf_t *buf = this->current->current;
    ed_messages (this, &buf, AT_EOF);
  } else
    Msg.send_fmt (this, COLOR_MSG, "Executed record [%d]", $my(record_idx) + 1);

  return (retval isnot OK ? NOTOK : OK);
}

static void ed_record_default (ed_t *this, char *bytes) {
  if ($my(repeat_mode)) return;

  if ($my(record))
    Vstring.append_with ($my(records)[$my(record_idx)], bytes);
  else
    String.replace_with ($my(records)[NUM_RECORDS]->head->next->data, bytes);
}

static void ed_record (ed_t *this, const char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len+1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  $my(record_cb) (this, bytes);
}

static ssize_t __record_header_len__ (const char *header) {
  char *sp = (char *) header;

  size_t len = bytelen (header);

  char *buf = NULL;
  while (*sp and (buf = Cstring.byte.in_str (sp, '\n')) isnot NULL) {
    len++;
    sp = buf + 1;
  }

  return len;
}

static char *ed_init_record_default (ed_t *this) {
  if ($my(record_header_len) is -1) {
      $my(record_header_len) = 0;
    return (char *) "";
  }

  ifnot ($my(record_header_len)) {
  const char *buf =
      "var ed = e_get_ed_current ()\n"
      "var win = ed_get_current_win (ed)\n"
      "var buf = win_get_current_buf (win)\n"
      "var draw = 1";
   $my(record_header_len) = __record_header_len__ (buf);
   return (char *) buf;
  } else {
    const char *buf =
      "ed = e_get_ed_current ()\n"
      "win = ed_get_current_win (ed)\n"
      "buf = win_get_current_buf (win)\n"
      "draw = 1";
    $my(record_header_len) = __record_header_len__ (buf);
    return (char *) buf;
  }
}

static void ed_init_record (ed_t *this, int idx) {
  if (0 > idx or idx > NUM_RECORDS - 1) idx = 0;
  $my(record_idx) = idx;
  $my(record) = 1;

  Vstring_t *rec = $my(records)[$my(record_idx)];
  if (rec)
    Vstring.clear (rec);
  else
    rec = Vstring.new ();

  Vstring.append_with (rec, $my(init_record_cb) (this));

  $my(records)[$my(record_idx)] = rec;

  Msg.send_fmt (this, COLOR_MSG, "Recording into [%d]", $my(record_idx) + 1);
}

static int ed_interpr_record (ed_t *this, int idx) {
  if ($my(record)) return NOTHING_TODO;

  if (0 > idx or idx > NUM_RECORDS) idx = 0;

  if (NULL is $my(records)[idx])
    return NOTHING_TODO;

  $my(repeat_mode) = 1;
  int retval = $my(i_record_cb) (this, $my(records)[idx]);
  $my(repeat_mode) = 0;

  return retval;
}

static void ed_deinit_record (ed_t *this) {
  $my(record) = 0;
  Msg.send_fmt (this, COLOR_MSG, "End of Recording into [%d]", $my(record_idx) + 1);
}

static int ed_buf_normal_cmd (ed_t *ed, buf_t **thisp, utf8 com, int count, int regidx) {
  buf_t *this = *thisp;

  if (0 >= count) // or count > this->num_items)
     return NOTHING_TODO;

  if (regidx < 0 or regidx >= NUM_REGISTERS)
    regidx = REG_UNNAMED;

  int retval = NOTHING_TODO;
  switch (this->on_normal_beg (thisp, com, count, regidx)) {
    case -1: goto theend;
    case IGNORE_BLOCK: goto atend;
    case EXIT_THIS: return EXIT_THIS;
    default: break;
  }

  if (com > 'z') {
    if ($from(ed, lmap)[0][0] isnot 0) {
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < ('z' - 'a') + 1; j++) {
          if ($from(ed, lmap)[i][j] is com) {
            com = (i is 1 ? 'a' : 'A') + j;
            goto handle_com;
          }
        }
      }
    }
  }

handle_com:
  switch (com) {
    case 'q':
      if ($my(flags) & BUF_IS_PAGER) return BUF_QUIT;

    // fall through
    case 'Q':
      if ($from(ed, record))
        ed_deinit_record (ed);
      else
        ed_init_record (ed, Input.getkey (STDIN_FILENO) - '1');
      break;

    case '@':
      retval = ed_interpr_record (ed, Input.getkey (STDIN_FILENO) - '1');
      break;

    case '.':
      String.replace_with ($from(ed, records)[NUM_RECORDS]->head->data,
          $from(ed, init_record_cb) (ed));
      retval = ed_interpr_record (ed, NUM_RECORDS);
      break;

    case ':':
      {
      readline_t *rl = Ed.readline.new (ed);
      retval = buf_readline (thisp, rl);
      this = *thisp;
      }

      goto theend;

    case '/':
    case '?':
    case '*':
    case '#':
    case 'n':
    case 'N':
      retval = self(search, com, NULL, -1); break;

    case 'm':
      retval = self(mark.set, -1); break;

    case '`':
      retval = self(mark.jump); break;

    case CTRL('o'):
      retval = self(jump.to, RIGHT_DIRECTION); break;

    case CTRL('i'):
      retval = self(jump.to, LEFT_DIRECTION); break;

    case '~':
      retval = self(normal.change_case); break;

    case CTRL('a'):
    case CTRL('x'):
      retval = buf_word_math (this, count, com); break;

    case '^':
      retval = self(normal.noblnk); break;

    case '>':
    case '<':
      retval = self(indent, count, com); break;

    case 'y':
      retval = self(normal.yank, count, regidx); break;

    case 'Y':
      retval = self(normal.Yank, count, regidx); break;

    case ' ':
      ifnot ($my(ftype)->space_on_normal_is_like_insert_mode) break;

      (&$my(regs)[REG_RDONLY])->type = CHARWISE;
      (&$my(regs)[REG_RDONLY])->head->data->bytes[0] = ' ';
      (&$my(regs)[REG_RDONLY])->head->data->bytes[1] = '\0';
      (&$my(regs)[REG_RDONLY])->head->data->num_bytes = 1;
      (&$my(regs)[REG_RDONLY])->head->next = NULL;
     // (&$my(regs)[REG_RDONLY])->head->prev = NULL;
      retval = self(normal.put, REG_RDONLY, 'P'); break;

    case 'p':
    case 'P':
      retval = self(normal.put, regidx, com); break;

    case 'x':
    case DELETE_KEY:
      retval = self(normal.delete, count, regidx, DRAW); break;

    case BACKSPACE_KEY:
      ifnot ($mycur(cur_col_idx)) {
        if ($my(ftype)->backspace_on_first_idx_remove_trailing_spaces) {
          size_t len = $mycur(data)->num_bytes;
          for (int i = $mycur(data)->num_bytes - 1; i > 0; i--)
            if ($mycur(data)->bytes[i] is ' ')
              String.clear_at ($mycur(data), i);
            else
              break;

          if (len isnot $mycur(data)->num_bytes) {
            $my(flags) |= BUF_IS_MODIFIED;
            self(draw_current_row);
          }

          retval = DONE;
          break;
        }

        ifnot ($my(ftype)->backspace_on_normal_goes_up)
          break;

        if (NOTHING_TODO is self(normal.up, 1, DONOT_ADJUST_COL, DONOT_DRAW))
          break;

        if (NOTHING_TODO is self(normal.eol, DRAW))
          self(draw);

        break;
      }

      ifnot ($my(ftype)->backspace_on_normal_is_like_insert_mode) break;

    // fall through
    case 'X':
       if (DONE is self(normal.left, 1, DONOT_DRAW))
         if (NOTHING_TODO is (retval = self(normal.delete, count, regidx, DRAW)))
           self(draw_current_row);
       break;

    case 'J':
      retval = self(normal.join, DRAW); break;

    case '$':
      retval = self(normal.eol, DRAW); break;

    case CTRL('w'):
      retval = selfp(normal.handle.ctrl_w); break;

    case 'g':
      retval = selfp(normal.handle.g, count); break;

    case 'd':
      retval = self(normal.handle.d, count, regidx); break;

    case 'G':
      retval = self(normal.handle.G, count); break;

    case ',':
      retval = selfp(normal.handle.comma); break;

    case 'c':
      retval = selfp(normal.handle.c, count, regidx); break;

    case 'W':
      retval = selfp(normal.handle.W);
      break;

    case 'L':
      retval = selfp(normal.handle.L);
      break;

    case 'F':
      retval = selfp(normal.handle.F);
      break;

    case '0':
      retval = self(normal.bol, DRAW); break;

    case 'E':
    case 'e':
      retval = self(normal.end_word, count,
        ($my(ftype)->small_e_on_normal_goes_insert_mode is 1 and 'e' is com), DRAW);
      this = *thisp;
      break;

    case ARROW_RIGHT_KEY:
    case 'l':
      retval = self(normal.right, count, DRAW); break;

    case ARROW_LEFT_KEY:
    case 'h':
      retval = self(normal.left, count, DRAW); break;

    case ARROW_UP_KEY:
    case 'k':
      retval = self(normal.up, count, ADJUST_COL, DRAW); break;

    case ARROW_DOWN_KEY:
    case 'j':
      retval = self(normal.down, count, ADJUST_COL, DRAW); break;

    case PAGE_DOWN_KEY:
    case CTRL('f'):
      retval = self(normal.page_down, count, DRAW); break;

    case PAGE_UP_KEY:
    case CTRL('b'):
      retval = self(normal.page_up, count, DRAW); break;

    case HOME_KEY:
      retval = self(normal.bof, DRAW); break;

    case END_KEY:
      retval = self(normal.eof, DRAW); break;

    case CTRL('v'):
      retval = self(normal.visual.bw); break;

    case 'V':
      retval = selfp(normal.visual.lw); break;

    case 'v':
      retval = selfp(normal.visual.cw); break;

    case 'D':
      retval = self(normal.delete_eol, regidx, DRAW); break;

    case 'r':
      retval = self(normal.replace_character); break;

    case 'C':
       self(normal.delete_eol, regidx, DRAW);
       retval = selfp(insert.mode, com, NULL); break;

    case 'o':
    case 'O':
      retval = selfp(insert.new_line, com); break;

    case '\r':
      ifnot ($my(ftype)->cr_on_normal_is_like_insert_mode) break;

      com = '\n';

    // fall through
    case 'i':
    case 'a':
    case 'A':
      retval = selfp(insert.mode, com, NULL); break;

    case CTRL('r'):
    case 'u':
      retval = self(undo.exec, com); break;

    case CTRL('l'):
      Win.draw (ed->current);
      retval = DONE; break;

    case CTRL('j'):
      $myroots(state) |= ED_SUSPENDED;
      return EXIT_THIS;

    case '-':
    case '_':
      if ('A' <= REGISTERS[regidx] and REGISTERS[regidx] <= 'Z') {
        reg_release (&$myroots(regs)[regidx]);
        retval = DONE;
      } else
          retval = NOTHING_TODO;

      break;

    default:
      break;
  }

atend:
  this->on_normal_end (thisp, com, count, regidx);

theend:
  return retval;
}

#define NORMAL_GET_NUMBER                 \
  c = ({                                  \
    int i = 0;                            \
    char intbuf[8];                       \
    intbuf[i++] = c;                      \
    utf8 cc = BUF_GET_NUMBER (intbuf, i); \
                                          \
    if (i is MAX_COUNT_DIGITS)            \
      goto new_state;                     \
                                          \
    intbuf[i] = '\0';                     \
    count = atoi (intbuf);                \
    cc;                                   \
    })

static int ed_loop (ed_t *ed, buf_t *this) {
  int retval = NOTOK;
  int count = 1;
  utf8 c;
  int cmd_retv;
  int regidx = -1;

  for (;;) {
 new_state:
    regidx = -1;
    count = 1;

get_char:
    ed_check_msg_status (ed);
    c = Input.getkey (STDIN_FILENO);

handle_char:
    switch (c) {
      case NOTOK: goto theend;

      case '"':
        if (-1 isnot regidx) goto exec_block;
        regidx = ed_reg_get_idx (ed, Input.getkey (STDIN_FILENO));
        goto get_char;

      case '1'...'9':
        NORMAL_GET_NUMBER;
        goto handle_char;

      default:
exec_block:

        cmd_retv = ed_buf_normal_cmd (ed, &this, c, count, regidx);

        if (cmd_retv is DONE or cmd_retv is NOTHING_TODO)
          goto new_state;

        if (cmd_retv is BUF_QUIT) {
          if ($from(ed, exit_quick)) {
            cmd_retv = EXIT_ALL_FORCE;
            goto exit_this;
          }

          retval = buf_change (&this, VED_COM_BUF_CHANGE_PREV_FOCUSED);
          if (retval is NOTHING_TODO) {
            retval = Ed.win.change ($my(root), &this, VED_COM_WIN_CHANGE_PREV_FOCUSED,
                NULL, 0, NO_FORCE);
            if (retval is NOTHING_TODO)
              cmd_retv = EXIT_THIS;
            else
              goto new_state;
          } else
            goto new_state;
        }

exit_this:
        if (cmd_retv is EXIT_THIS or cmd_retv is EXIT_ALL or cmd_retv is EXIT_ALL_FORCE) {
          ifnot (($myroots(state) & ED_SUSPENDED)) {
            if (cmd_retv is EXIT_THIS)
              $myroots(state) |= ED_EXIT;
            else if (cmd_retv is EXIT_ALL)
              $myroots(state) |= ED_EXIT_ALL;
            else
              $myroots(state) |= ED_EXIT_ALL_FORCE;
          }

          retval = OK;
          goto theend;
        }

        if (cmd_retv is WIN_EXIT) {
          /* at this point this (probably) is a null reference */
          retval = ed_win_delete (ed, &this, NO_COUNT_SPECIAL);

          if (retval is DONE) goto new_state;

          ed->prop->state |= ED_EXIT;
          retval = OK;
          goto theend;
        }
    }
  }

theend:
  return retval;
}

static void ed_suspend (ed_t *this) {
  if ($my(state) & ED_SUSPENDED) return;
  $my(state) |= ED_SUSPENDED;
  Screen.clear ($my(term));
  Term.reset ($my(term));
}

static void ed_resume (ed_t *this) {
  ifnot ($my(state) & ED_SUSPENDED) return;
  $my(state) &= ~ED_SUSPENDED;
  Term.set ($my(term));
  Win.set.current_buf (this->current, this->current->cur_idx, DONOT_DRAW);
  Win.draw (this->current);
}

static void ed_init_commands (ed_t *this) {
  ifnot (NULL is $my(commands)) return;

  $my(has_ed_readline_commands) = 1;

  const char *ed_commands[VED_COM_END + 1] = {
    [VED_COM_BUF_BACKUP] = "@bufbackup",
    [VED_COM_BUF_CHANGE_NEXT] = "bufnext",
    [VED_COM_BUF_CHANGE_NEXT_ALIAS] = "bn",
    [VED_COM_BUF_CHANGE_PREV_FOCUSED] = "bufprevfocused",
    [VED_COM_BUF_CHANGE_PREV_FOCUSED_ALIAS] = "b`",
    [VED_COM_BUF_CHANGE_PREV] = "bufprev",
    [VED_COM_BUF_CHANGE_PREV_ALIAS] = "bp",
    [VED_COM_BUF_DELETE_FORCE] = "bufdelete!",
    [VED_COM_BUF_DELETE_FORCE_ALIAS] = "bd!",
    [VED_COM_BUF_DELETE] = "bufdelete",
    [VED_COM_BUF_DELETE_ALIAS] = "bd",
    [VED_COM_BUF_CHANGE] = "buffer",
    [VED_COM_BUF_CHANGE_ALIAS] = "b",
    [VED_COM_BUF_CHECK_BALANCED] = "@balanced_check",
    [VED_COM_BUF_SET] = "set",
    [VED_COM_DIFF_BUF] = "diffbuf",
    [VED_COM_DIFF] = "diff",
    [VED_COM_EDIT_FORCE] = "edit!",
    [VED_COM_EDIT_FORCE_ALIAS] = "e!",
    [VED_COM_EDIT] = "edit",
    [VED_COM_EDIT_ALIAS] = "e",
    [VED_COM_EDIT_IMAGE] = "@edit_image",
    [VED_COM_EDNEW] = "ednew",
    [VED_COM_ENEW] = "enew",
    [VED_COM_EDNEXT] = "ednext",
    [VED_COM_EDPREV] = "edprev",
    [VED_COM_EDPREV_FOCUSED] = "edprevfocused",
    [VED_COM_ETAIL] = "etail",
    [VED_COM_GREP] = "vgrep",
    [VED_COM_MESSAGES] = "messages",
    [VED_COM_QUIT_FORCE] = "quit!",
    [VED_COM_QUIT_FORCE_ALIAS] = "q!",
    [VED_COM_QUIT] = "quit",
    [VED_COM_QUIT_ALIAS] = "q",
    [VED_COM_READ] = "read",
    [VED_COM_READ_ALIAS] = "r",
    [VED_COM_READ_SHELL] = "r!",
    [VED_COM_REDRAW] = "redraw",
    [VED_COM_SCRATCH] = "scratch",
    [VED_COM_SEARCHES] = "searches",
    [VED_COM_SHELL] = "!",
    [VED_COM_SPLIT] = "split",
    [VED_COM_SUBSTITUTE] = "substitute",
    [VED_COM_SUBSTITUTE_WHOLE_FILE_AS_RANGE] = "s%",
    [VED_COM_SUBSTITUTE_ALIAS] = "s",
    [VED_COM_SAVE_IMAGE] = "@save_image",
    [VED_COM_TEST_KEY] = "testkey",
    [VED_COM_TTY_SCREEN] = "tty_screen",
    [VED_COM_VALIDATE_UTF8] = "@validate_utf8",
    [VED_COM_WIN_CHANGE_NEXT] = "winnext",
    [VED_COM_WIN_CHANGE_NEXT_ALIAS] = "wn",
    [VED_COM_WIN_CHANGE_PREV_FOCUSED] = "winprevfocused",
    [VED_COM_WIN_CHANGE_PREV_FOCUSED_ALIAS] = "w`",
    [VED_COM_WIN_CHANGE_PREV] = "winprev",
    [VED_COM_WIN_CHANGE_PREV_ALIAS] = "wp",
    [VED_COM_WRITE_FORCE] = "write!",
    [VED_COM_WRITE_FORCE_ALIAS] = "w!",
    [VED_COM_WRITE_FORCE_FORCE] = "write!!",
    [VED_COM_WRITE_FORCE_FORCE_ALIAS] = "w!!",
    [VED_COM_WRITE] = "write",
    [VED_COM_WRITE_ALIAS] = "w",
    [VED_COM_WRITE_QUIT_FORCE] = "wq!",
    [VED_COM_WRITE_QUIT] = "wq",
    [VED_COM_END] = NULL
  };

  int num_args[VED_COM_END + 1] = {
    [VED_COM_BUF_DELETE_FORCE ... VED_COM_BUF_DELETE_ALIAS] = 1,
    [VED_COM_BUF_CHANGE ... VED_COM_BUF_CHANGE_ALIAS] = 1,
    [VED_COM_BUF_CHECK_BALANCED] = 1,
    [VED_COM_EDIT ... VED_COM_ENEW] = 1,
    [VED_COM_GREP] = 3,
    [VED_COM_QUIT_FORCE ... VED_COM_QUIT_ALIAS] = 1,
    [VED_COM_READ ... VED_COM_READ_ALIAS] = 1,
    [VED_COM_SPLIT] = 1,
    [VED_COM_SUBSTITUTE ... VED_COM_SUBSTITUTE_ALIAS] = 5,
    [VED_COM_SAVE_IMAGE] = 1,
    [VED_COM_VALIDATE_UTF8] = 1,
    [VED_COM_WRITE_FORCE ... VED_COM_WRITE_ALIAS] = 4,
    [VED_COM_WRITE_QUIT_FORCE ... VED_COM_WRITE_QUIT] = 1
  };

  int flags[VED_COM_END + 1] = {
    [VED_COM_BUF_DELETE_FORCE ... VED_COM_BUF_DELETE_ALIAS] = READLINE_ARG_BUFNAME,
    [VED_COM_BUF_CHANGE ... VED_COM_BUF_CHANGE_ALIAS] = READLINE_ARG_BUFNAME,
    [VED_COM_BUF_CHECK_BALANCED] = READLINE_ARG_RANGE,
    [VED_COM_EDIT ... VED_COM_ENEW] = READLINE_ARG_FILENAME,
    [VED_COM_GREP] = READLINE_ARG_FILENAME|READLINE_ARG_PATTERN|READLINE_ARG_RECURSIVE,
    [VED_COM_QUIT_FORCE ... VED_COM_QUIT_ALIAS] = READLINE_ARG_GLOBAL,
    [VED_COM_READ ... VED_COM_READ_ALIAS] = READLINE_ARG_FILENAME,
    [VED_COM_SPLIT] = READLINE_ARG_FILENAME,
    [VED_COM_SUBSTITUTE ... VED_COM_SUBSTITUTE_ALIAS] =
      READLINE_ARG_RANGE|READLINE_ARG_GLOBAL|READLINE_ARG_PATTERN|READLINE_ARG_SUB|READLINE_ARG_INTERACTIVE,
    [VED_COM_VALIDATE_UTF8] = READLINE_ARG_FILENAME,
    [VED_COM_WRITE_FORCE ... VED_COM_WRITE_ALIAS] =
      READLINE_ARG_FILENAME|READLINE_ARG_RANGE|READLINE_ARG_BUFNAME|READLINE_ARG_APPEND,
    [VED_COM_WRITE_QUIT_FORCE ... VED_COM_WRITE_QUIT] = READLINE_ARG_GLOBAL
  };

  $my(commands) = Alloc (sizeof (readline_com_t) * (VED_COM_END + 1));

  int i = 0;
  for (i = 0; i < VED_COM_END; i++) {
    $my(commands)[i] = Alloc (sizeof (readline_com_t));
    size_t clen = bytelen (ed_commands[i]);
    $my(commands)[i]->com = Alloc (clen + 1);
    Cstring.cp ($my(commands)[i]->com, clen + 1, ed_commands[i], clen);

    ifnot (num_args[i]) {
      $my(commands)[i]->args = NULL;
      continue;
    }

    $my(commands)[i]->args = Alloc (sizeof (char *) * (num_args[i] + 1));
    $my(commands)[i]->num_args = num_args[i];
    for (int j = 0; j <= num_args[i]; j++)
      $my(commands)[i]->args[j] = NULL;

    ed_add_command_arg ($my(commands)[i], flags[i]);
  }

  $my(commands)[i] = NULL;
  $my(num_commands) = VED_COM_END;

  ed_append_command_arg (this, "set", "--persistent-layout=", 20);
  ed_append_command_arg (this, "set", "--enable-writing", 16);
  ed_append_command_arg (this, "set", "--backup-suffix=", 16);
  ed_append_command_arg (this, "set", "--no-backupfile", 15);
  ed_append_command_arg (this, "set", "--save-on-exit=", 15);
  ed_append_command_arg (this, "set", "--shiftwidth=", 13);
  ed_append_command_arg (this, "set", "--save-image=", 13);
  ed_append_command_arg (this, "set", "--image-file=", 13);
  ed_append_command_arg (this, "set", "--image-name=", 13);
  ed_append_command_arg (this, "set", "--backupfile", 12);
  ed_append_command_arg (this, "set", "--lang-mode=", 12);
  ed_append_command_arg (this, "set", "--tabwidth=", 11);
  ed_append_command_arg (this, "set", "--autosave=", 11);
  ed_append_command_arg (this, "set", "--ftype=", 8);
  ed_append_command_arg (this, "diff", "--origin", 8);
  ed_append_command_arg (this, "substitute", "--remove-doseol", 15);
  ed_append_command_arg (this, "s%",         "--remove-doseol", 15);
  ed_append_command_arg (this, "substitute", "--remove-tabs", 13);
  ed_append_command_arg (this, "s%",         "--remove-tabs", 13);
  ed_append_command_arg (this, "substitute", "--shiftwidth=", 13);
  ed_append_command_arg (this, "s%",         "--shiftwidth=", 13);
  ed_append_command_arg (this, "substitute", "--interpret-ctrl-backspace", 26);
  ed_append_command_arg (this, "s%",         "--interpret-ctrl-backspace", 26);
  ed_append_command_arg (this, "@save_image", "--as=", 5);

  if (getuid ()) {
    ed_append_readline_command (this, "spell", 1, READLINE_ARG_RANGE);
    ed_append_command_arg (this, "spell",  "--edit", 6);
    ed_set_readline_cb (this, buf_spell_readline_cb);
  }
}

static void readline_expr_reg (readline_t *rl) {
  ed_t *this = (ed_t *) rl->user_data[READLINE_ED_USER_DATA_IDX];

  int regidx = ed_reg_get_idx (this, Input.getkey (STDIN_FILENO));

  if (NOTOK is regidx) return;

  buf_t *buf = self(get.current_buf);
  if (ERROR is ed_reg_special_set (this, buf, regidx))
    return;

  Reg_t *rg = &$my(regs)[regidx];
  if (rg->type is LINEWISE) return;

  reg_t *reg = rg->head;
  while (reg isnot NULL) {
    Readline.insert_with_len (rl, reg->data->bytes, reg->data->num_bytes);
    reg = reg->next;
  }
}

static void readline_error (readline_t *rl, int error) {
  buf_t *this = (buf_t *) rl->user_data[READLINE_BUF_USER_DATA_IDX];
  if (this)
    MSG_ERRNO (error);
}

static void readline_on_write (readline_t *rl) {
  ed_t *this = (ed_t *) rl->user_data[READLINE_ED_USER_DATA_IDX];
  ed_check_msg_status (this);
}

static readline_t *ed_readline_new (ed_t *this) {
  readline_t *rl = Readline.new (this, $my(term), Input.getkey, $my(prompt_row),
      1, $my(dim)->num_cols, $my(video)) ;
  if ($my(commands) is NULL) ed_init_commands (this);
  rl->commands = $my(commands);
  rl->num_commands = $my(num_commands);
  rl->first_chars[0] = '~';
  rl->first_chars[1] = '`';
  rl->first_chars[2] = '@';
  rl->first_chars_len = 3;
  rl->trigger_first_char_completion = 1;
  rl->history = $my(history)->readline;
  rl->last_component = $my(rl_last_component);

  rl->error = readline_error;
  rl->on_write = readline_on_write;
  rl->expr_reg = readline_expr_reg;
  rl->tab_completion = readline_tab_completion;
  rl->last_component_push = readline_last_component_push_cb;

  return rl;
}

static readline_t *ed_readline_new_with (ed_t *this, char *bytes) {
  readline_t *rl = self(readline.new);
  Readline.set.line (rl, bytes, bytelen (bytes));
  return rl;
}

static ed_T *editor_new (void) {
  ed_T *this = Alloc (sizeof (ed_T));
  this->prop = Alloc (sizeof (ed_prop));

  *this = (ed_T) {
    .prop = this->prop,
    .self = (ed_self) {
      .quit = ed_quit,
      .record = ed_record,
      .resume = ed_resume,
      .scratch = ed_scratch,
      .suspend = ed_suspend,
      .question = ed_question,
      .messages = ed_messages,
      .dim_calc = ed_dim_calc,
      .dims_init = ed_dims_init,
      .release_info = ed_release_info,
      .check_sanity = ed_check_sanity,
      .deinit_commands = ed_deinit_commands,
      .set = (ed_set_self) {
        .dim = ed_set_dim,
        .state = ed_set_state,
        .topline = ed_set_topline,
        .lang_map = ed_set_lang_map,
        .lang_mode = ed_set_lang_mode,
        .state_bit = ed_set_state_bit,
        .record_cb = ed_set_record_cb,
        .at_exit_cb = ed_set_at_exit_cb,
        .exit_quick = ed_set_exit_quick,
        .lang_getkey = ed_set_lang_getkey,
        .screen_size = ed_set_screen_size,
        .current_win = ed_set_current_win,
        .expr_reg_cb = ed_set_expr_reg_cb,
        .i_record_cb = ed_set_i_record_cb,
        .readline_cb = ed_set_readline_cb,
        .word_actions = ed_set_word_actions,
        .on_normal_g_cb = ed_set_normal_on_g_cb,
        .init_record_cb = ed_set_init_record_cb,
        .lw_mode_actions = ed_set_lw_mode_actions,
        .cw_mode_actions = ed_set_cw_mode_actions,
        .line_mode_actions = ed_set_line_mode_actions,
        .file_mode_actions = ed_set_file_mode_actions
      },
      .unset = (ed_unset_self) {
        .state_bit = ed_unset_state_bit
      },
      .test = (ed_test_self) {
        .state_bit = ed_test_state_bit
      },
      .get = (ed_get_self) {
        .info = (edget_info_self) {
          .as_type = ed_get_info_as_type
        },
        .term = ed_get_term,
        .state = ed_get_state,
        .video = ed_get_video,
        .topline = ed_get_topline,
        .num_win = ed_get_num_win,
        .bufname = ed_get_bufname,
        .min_rows = ed_get_min_rows,
        .win_head = ed_get_win_head,
        .win_next = ed_get_win_next,
        .win_by_idx = ed_get_win_by_idx,
        .win_by_name = ed_get_win_by_name,
        .current_buf = ed_get_current_buf,
        .scratch_buf = ed_get_scratch_buf,
        .current_win = ed_get_current_win,
        .callback_fun = ed_get_callback_fun,
        .current_win_idx = ed_get_current_win_idx,
        .num_special_win = ed_get_num_special_win,
        .num_readline_commands = ed_get_num_readline_commands
      },
      .syn = (ed_syn_self) {
        .append = ed_syn_append,
        .get_ftype_idx = ed_syn_get_ftype_idx
      },
      .reg = (ed_reg_self) {
        .set = ed_reg_set,
        .setidx = ed_reg_setidx
      },
      .append = (ed_append_self) {
        .win = ed_append_win,
        .message = ed_append_message,
        .message_fmt = ed_append_message_fmt,
        .toscratch = ed_append_toscratch,
        .toscratch_fmt = ed_append_toscratch_fmt,
        .command_arg = ed_append_command_arg,
        .readline_commands = ed_append_readline_commands,
        .readline_command = ed_append_readline_command
      },
      .readjust = (ed_readjust_self) {
        .win_size =ed_win_readjust_size
      },
      .readline = (ed_readline_self) {
        .new = ed_readline_new,
        .new_with = ed_readline_new_with
      },
      .buf = (ed_buf_self) {
        .change = ed_buf_change,
        .get = ed_buf_get
      },
      .win = (ed_win_self) {
        .init = ed_win_init,
        .new = ed_win_new,
        .new_special = ed_win_new_special,
        .change = ed_win_change
      },
      .menu = (ed_menu_self) {
        .new = ed_menu_new,
        .release = ed_menu_release
      },
      .sh = (ed_sh_self) {
        .popen = ed_sh_popen
      },
      .history = (ed_history_self) {
        .add = ed_history_add,
        .read = ed_history_read,
        .write = ed_history_write,
        .add_lines = ed_history_add_lines,
        .set = (edhistory_set_self) {
          .readline_file = ed_history_set_readline_file,
          .search_file = ed_history_set_search_file
        },
        .get = (edhistory_get_self) {
          .readline_file = ed_history_get_readline_file,
          .search_file = ed_history_get_search_file
        }
      },
      .draw = (ed_draw_self) {
        .current_win = ed_draw_current_win
      },
    },
    .__Win__ = (win_T) {
      .self = (win_self) {
        .set = (win_set_self) {
          .current_buf = win_set_current_buf,
          .previous_idx = win_set_previous_idx,
          .has_dividers = win_set_has_dividers,
          .video_dividers = win_set_video_dividers,
          .min_rows = win_set_min_rows,
          .num_frames = win_set_num_frames
        },
        .buf = (win_buf_self) {
          .init = win_buf_init,
          .new = win_buf_new
        },
        .frame = (win_frame_self) {
          .change = win_frame_change
        },
        .get = (win_get_self) {
          .info = (winget_info_self) {
            .as_type = win_get_info_as_type
          },
          .current_buf = win_get_current_buf,
          .current_buf_idx = win_get_current_buf_idx,
          .buf_by_idx = win_get_buf_by_idx,
          .buf_by_name = win_get_buf_by_name,
          .buf_head = win_get_buf_head,
          .buf_next = win_get_buf_next,
          .num_buf = win_get_num_buf,
          .num_cols = win_get_num_cols
        },
        .isit = (win_isit_self) {
          .special_type = win_isit_special_type
        },
        .pop = (win_pop_self) {
          .current_buf = win_pop_current_buf
        },
        .adjust = (win_adjust_self) {
          .buf_dim = win_adjust_buf_dim,
        },
        .edit_fname = win_edit_fname,
        .draw = win_draw,
        .release_info = win_release_info,
        .append_buf = win_append_buf,
        .dim_calc = win_dim_calc
      },
    },
    .__Buf__ = (buf_T) {
      .self = (buf_self) {
        .release = (buf_release_self) {
          .row = buf_release_row,
          .rows = buf_release_rows,
          .info = buf_release_info,
          .line = buf_release_line
        },
        .get = (buf_get_self) {
          .info = (bufget_info_self) {
            .as_type = buf_get_info_as_type,
          },
          .prop = (bufget_prop_self) {
            .tabwidth = buf_get_prop_tabwidth
          },
          .row = (bufget_row_self) {
            .at = buf_get_row_at,
            .current = buf_get_row_current,
            .current_bytes = buf_get_row_current_bytes,
            .bytes_at = buf_get_row_bytes_at,
            .col_idx = buf_get_row_col_idx
          },
          .my_parent = buf_get_my_parent,
          .my_root = buf_get_my_root,
          .basename = buf_get_basename,
          .fname = buf_get_fname,
          .ftype_name = buf_get_ftype_name,
          .shared_str = buf_get_shared_str,
          .flags = buf_get_flags,
          .num_lines = buf_get_num_lines,
          .size = buf_get_size,
          .contents = buf_get_contents,
          .current_word = buf_get_current_word,
          .current_row_idx = buf_get_current_row_idx,
          .current_col_idx = buf_get_current_col_idx,
          .current_video_row = buf_get_current_video_row,
          .current_video_col = buf_get_current_video_col,
        },
        .set = (buf_set_self) {
          .mode = buf_set_mode,
          .fname = buf_set_fname,
          .ftype = buf_set_ftype,
          .backup = buf_set_backup,
          .modified = buf_set_modified,
          .autosave = buf_set_autosave,
          .autochdir = buf_set_autochdir,
          .save_on_exit = buf_set_save_on_exit,
          .on_emptyline = buf_set_on_emptyline,
          .video_first_row = buf_set_video_first_row,
          .show_statusline = buf_set_show_statusline,
          .as = (bufset_as_self) {
            .unnamed = buf_set_as_unnamed,
            .non_existant = buf_set_as_non_existant,
            .pager = buf_set_as_pager
          },
          .normal = (bufset_normal_self) {
            .at_beg_cb = buf_set_normal_beg_cb,
            .at_end_cb = buf_set_normal_end_cb
          },
          .row = (bufset_row_self) {
            .idx = buf_set_row_idx
          },
        },
        .syn = (buf_syn_self) {
          .init = buf_syn_init,
          .parser = buf_syn_parser
        },
        .ftype = (buf_ftype_self) {
          .set = buf_ftype_set,
          .init = buf_ftype_init,
          .release = buf_ftype_release
        },
        .to = (buf_to_self) {
          .video = buf_to_video
        },
        .isit = (buf_isit_self) {
          .special_type = buf_isit_special_type
        },
        .current = (buf_current_self) {
          .set = buf_current_set,
          .prepend = buf_current_prepend,
          .append = buf_current_append,
          .append_with = buf_current_append_with,
          .append_with_len = buf_current_append_with_len,
          .prepend_with = buf_current_prepend_with,
          .replace_with = buf_current_replace_with,
          .delete = buf_current_delete,
          .pop = buf_current_pop,
         },
        .row =  (buf_row_self) {
          .new_with = buf_row_new_with,
          .new_with_len = buf_row_new_with_len,
        },
        .read = (buf_read_self) {
          .fname = buf_read_fname,
          .from_fp = buf_read_from_fp
        },
        .iter = (buf_iter_self) {
          .new = buf_iter_new,
          .next = buf_iter_next,
          .release = buf_iter_release
        },
        .Action = (buf_Action_self) {
          .new = buf_Action_new,
          .release = buf_Action_release,
          .set_with = buf_Action_set_with,
          .set_with_current = buf_Action_set_with_current
        },
        .action = (buf_action_self) {
          .new = buf_action_new,
          .release = buf_action_release,
          .new_with = buf_action_new_with
        },
        .undo = (buf_undo_self) {
          .pop = buf_undo_pop,
          .push = buf_undo_push,
          .release = buf_undo_release,
          .init = buf_undo_init,
          .exec = buf_undo_exec,
          .clear = buf_undo_clear,
          .insert = buf_undo_insert,
          .delete_line = buf_undo_delete_line,
          .replace_line = buf_undo_replace_line
        },
        .redo = (buf_redo_self) {
          .pop = buf_redo_pop,
          .push = buf_redo_push
        },
        .normal = (buf_normal_self) {
          .handle = (bufnormal_handle_self) {
            .d = buf_normal_handle_d,
            .G = buf_normal_handle_G,
            .g = buf_normal_handle_g,
            .c = buf_normal_handle_c,
            .W = buf_normal_handle_W,
            .F = buf_normal_handle_F,
            .L = buf_normal_handle_L,
            .comma = buf_normal_handle_comma,
            .ctrl_w = buf_normal_handle_ctrl_w,
          },
          .visual = (bufnormal_visual_self) {
            .bw = buf_normal_visual_bw,
            .lw = buf_normal_visual_lw,
            .cw = buf_normal_visual_cw
          },
          .up = buf_normal_up,
          .bof = buf_normal_bof,
          .bol = buf_normal_bol,
          .eof = buf_normal_eof,
          .eol = buf_normal_eol,
          .put = buf_normal_put,
          .join = buf_normal_join,
          .yank = buf_normal_yank,
          .Yank = buf_normal_Yank,
          .down = buf_normal_down,
          .left = buf_normal_left,
          .right = buf_normal_right,
          .delete = buf_normal_delete,
          .noblnk =  buf_normal_noblnk,
          .page_up = buf_normal_page_up,
          .end_word = buf_normal_end_word,
          .page_down = buf_normal_page_down,
          .delete_eol = buf_normal_delete_eol,
          .goto_linenr = buf_normal_goto_linenr,
          .change_case = buf_normal_change_case,
          .replace_character = buf_normal_replace_character,
          .replace_character_with = buf_normal_replace_character_with
        },
        .delete = (buf_delete_self) {
          .word = buf_delete_word,
          .line = buf_delete_line
        },
        .insert = (buf_insert_self) {
          .string = buf_insert_string,
          .mode = buf_insert_mode,
          .new_line = buf_insert_new_line,
        },
        .adjust = (buf_adjust_self) {
          .col = buf_adjust_col,
          .view = buf_adjust_view,
          .marks = buf_adjust_marks
        },
        .jump = (buf_jump_self) {
          .to = buf_jump_to,
          .push = buf_jump_push
        },
        .jumps = (buf_jumps_self) {
          .release = buf_jumps_release,
          .init = buf_jumps_init
        },
        .mark = (buf_mark_self) {
          .set = buf_mark_set,
          .jump = buf_mark_jump,
          .restore = buf_mark_restore
        },
        .draw = buf_draw,
        .clear = buf_clear,
        .flush = buf_flush,
        .readline = buf_readline,
        .write = buf_write,
        .search = buf_search,
        .indent = buf_indent,
        .input_box = buf_input_box,
        .init_fname = buf_init_fname,
        .substitute = buf_substitute,
        .append_with = buf_append_with,
        .backupfile = buf_com_backupfile,
        .draw_current_row = buf_draw_current_row
      },
    },
    .__Msg__ = (msg_T) {
      .self = (msg_self) {
        .set = ed_msg_set,
        .set_fmt = ed_msg_set_fmt,
        .line = ed_msg_line,
        .send = ed_msg_send,
        .send_fmt = ed_msg_send_fmt,
        .error = ed_msg_error,
        //.error_fmt = ed_msg_error_fmt,
        .fmt = ed_msg_fmt,
        .write = ed_msg_write,
        .write_fmt = ed_msg_write_fmt
      },
    },
    .__EError__ = (eerror_T) {
      .self = (eerror_self) {
        .string = ed_error_string
      },
    },
  };

  __INIT__ (re);
  __INIT__ (io);
  __INIT__ (dir);
  __INIT__ (sys);
  __INIT__ (vui);
  __INIT__ (file);
  __INIT__ (path);
  __INIT__ (term);
  __INIT__ (imap);
  __INIT__ (smap);
  __INIT__ (proc);
  __INIT__ (spell);
  __INIT__ (video);
  __INIT__ (error);
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (vstring);
  __INIT__ (ustring);
  __INIT__ (readline);

  Sys.init_environment (SysEnvOpts());

  $my(video) = NULL;
  $my(term)  = NULL;

  return this;
}

static int fp_tok_cb (Vstring_t *unused, char *bytes, void *fp_type) {
  (void) unused;
  fp_t *fpt = (fp_t *) fp_type;
  fpt->error = 0;

  idx_t num = fprintf (fpt->fp, "%s\n", bytes);

  if (0 > num)
    fpt->error = errno;
  else
    fpt->num_bytes += num;

  return STRCHOP_OK;
}

public mutable size_t tostderr (char *bytes) {
  Vstring_t unused;
  fp_t fpt = {.fp = stderr};
  cstring_chop (bytes, '\n', &unused, fp_tok_cb, &fpt);
  return fpt.num_bytes;
}

static int ed_main (ed_t *this, buf_t *buf) {
  if ($my(state) & ED_SUSPENDED)
    self(resume);
  else {
    $from(buf, flags) |= BUF_IS_VISIBLE;
    Win.draw (this->current);
  }

  $my(state) = UNSET;

/*
  Msg.send (this, COLOR_CYAN,
      "Υγειά σου Κόσμε και καλό ταξίδι στο ραντεβού με την αιωνιότητα");
 */

  return ed_loop (this, buf);
}

static ed_t *ed_init (E_T *E, ed_opts opts) {
  ed_t *this = Alloc (sizeof (ed_t));
  $myprop = Alloc (sizeof (ed_prop));

  $my(Me) = E->__Ed__;

  $my(term) = $from($my(Me), term);

  $my(__Win__)     = &E->__Ed__->__Win__;
  $my(__Buf__)     = &E->__Ed__->__Buf__;
  $my(__LA__)      = &E->__Ed__->__LA__;
  $my(__E__)       = E;
  $my(__Msg__)     = &E->__Ed__->__Msg__;
  $my(__EError__)  = &E->__Ed__->__EError__;

  $my(has_topline) = $my(has_msgline) = $my(has_promptline) = 1;

  Term.set ($my(term));

  int frow = opts.first_row;
  int fcol = opts.first_col;
  int num_rows = opts.num_rows;
  int num_cols = opts.num_cols;

  ifnot (num_rows) num_rows = $my(term)->num_rows;
  ifnot (num_cols) num_cols = $my(term)->num_cols;

  $my(dim) = ed_dim_new (this, frow, num_rows, fcol, num_cols);

  $my(video) = Video.new (OUTPUT_FD, $my(dim)->num_rows, $my(dim)->num_cols, $my(dim)->first_row, $my(dim)->first_col);

  $my(msg_row)     = $my(dim)->num_rows;
  $my(prompt_row)  = $my(msg_row) - 1;
  $my(topline_row) = $my(dim)->first_row;

  $my(ed_str)  = String.new ($my(dim)->num_cols);
  $my(topline) = String.new ($my(dim)->num_cols);
  $my(msgline) = String.new ($my(dim)->num_cols);
  $my(last_insert) = String.new ($my(dim)->num_cols);
  $my(rl_last_component) = Vstring.new ();
  $my(uline) = Ustring.new ();

  $my(state) = UNSET;
  $my(msg_tabwidth) = 2;
  $my(saved_cwd) = Dir.current ();

  $my(history) = Alloc (sizeof (hist_t));
  $my(history)->search = Alloc (sizeof (h_search_t));
  $my(history)->readline = Alloc (sizeof (readline_hist_t));
  $my(history)->readline->history_idx = 0;
  $my(max_num_hist_entries) = RLINE_HISTORY_NUM_ENTRIES;
  $my(max_num_undo_entries) = UNDO_NUM_ENTRIES;
  $my(hs_file) = self(history.set.search_file, opts.hs_file);
  $my(hrl_file) = self(history.set.readline_file, opts.hrl_file);

  $my(has_ed_readline_commands) = 0;
  $my(exit_quick) = 0;
  $my(repeat_mode) = 0;
  $my(record) = 0;
  $my(record_idx) = -1;
  $my(record_header_len) = -1;
  $my(record_cb) = ed_record_default;
  $my(i_record_cb) = ed_i_record_default;
  $my(init_record_cb) = ed_init_record_default;
  $my(records)[NUM_RECORDS] = Vstring.new ();
  Vstring.append_with_len ($my(records)[NUM_RECORDS], " ", 1);
  Vstring.append_with_len ($my(records)[NUM_RECORDS], " ", 1);

  this->name_gen = ('z' - 'a') + 1;

  $my(lang_getkey) = NULL;
  Cstring.cp ($my(lang_mode), 8, DEFAULT_LANG_MODE, 2);

  $my(num_readline_cbs) = $my(num_on_normal_g_cbs) =
  $my(num_lw_mode_cbs) = $my(num_cw_mode_cbs) =
  $my(num_at_exit_cbs) = $my(num_file_mode_cbs) = 0;

  ed_reg_init_all (this);

  ed_init_syntaxes (this);

  ed_init_commands (this);

  ed_init_special_win (this);

  if (opts.flags & ED_INIT_OPT_LOAD_HISTORY) // after ed_init_commands
    self(history.read);

  ed_set_cw_mode_actions_default (this);
  ed_set_lw_mode_actions_default (this);
  ed_set_word_actions_default (this);
  ed_set_line_mode_actions_default (this);
  ed_set_file_mode_actions_default (this);

  $my(name) = ed_name_gen (&$OurRoots(name_gen), "ed:", 3);
  $OurRoots(orig_num_items)++;

  return this;
}

static ed_t *E_init (E_T *this, ed_opts opts) {
  ed_t *ed = ed_init (this, opts);

  int cur_idx = $my(cur_idx);

  DListAppendCurrent ($myprop, ed);

  if ($my(prev_idx) is -1)
    $my(prev_idx) = 0;
  else
    $my(prev_idx) = cur_idx;

  if (opts.init_cb isnot NULL) {
    opts.init_cb (ed, opts);

    if (NULL is $my(at_init_cb))
      $my(at_init_cb) = opts.init_cb;
  } else {
    ifnot (NULL is $my(at_init_cb))
      $my(at_init_cb) (ed, opts);
  }

  $from(ed, root) = this;
  $from(ed, E) = this->self;

  String.replace_with ($from(ed, records)[NUM_RECORDS]->head->data,
      $from(ed, init_record_cb) (ed));

  return ed;
}

static ed_t *E_new (E_T *this, ed_opts opts) {
  if (opts.term_flags)
    Term.set_state_bit (self(get.term), opts.term_flags);

  ed_t *ed = E_init (this, opts);

  int num_win = opts.num_win;
  if (num_win <= 0) num_win = 1;

  int num_frames = 1;

  win_t *w;
  loop (num_win) {
    w = ed_win_new (ed, NULL, num_frames);
    ed_append_win (ed, w);
  }

  ed_set_current_win (ed, $from(ed, num_special_win));

  return ed;
}

static int E_delete (E_T *this, int idx, int force_current) {
  $my(error_state) = 0;

  if (1 is $my(num_items) and 0 is force_current) {
    $my(error_state) |= LAST_INSTANCE_ERROR_STATE;
    return NOTOK;
  }

  if (idx < 0 or idx >= $my(num_items)) {
    $my(error_state) |= IDX_OUT_OF_BOUNDS_ERROR_STATE;
    return NOTOK;
  }

  if (idx is $my(cur_idx)) {
    ifnot (force_current) {
      $my(error_state) |= ARG_IDX_IS_CUR_IDX_ERROR_STATE;
      return NOTOK;
    } else
      $my(prev_idx) = $my(cur_idx) - 1;
  } else
    $my(prev_idx) = $my(cur_idx);

  ed_t *ed = DListPopAt ($myprop, ed_t, idx);

  ifnot (NULL is ed) ed_release (ed);

  if ($my(prev_idx) is -1)
    if ($my(num_items))
      $my(prev_idx) = 0;

  return OK;
}

static int E_set_i_dir (E_T *__e__, char *dir) {
  (void) __e__;
  if (NOTOK is __env_check_directory__ (dir, "interpreter directory", 0, 0, 0))
    return NOTOK;

  Sys.set.env_as (dir, "LA_DIR", 1);
  return OK;
}

static ed_t *E_set_current (E_T *this, int idx) {
  int cur_idx = $my(cur_idx);
  if (cur_idx is idx) return $my(current);

  if (EINDEX is DListSetCurrent ($myprop, idx))
    return NULL;

  $my(prev_idx) = cur_idx;

  return $my(current);
}

static ed_t *E_set_next (E_T *this) {
  int idx = $my(cur_idx);
  if (idx is $my(num_items) - 1)
    idx = 0;
  else
    idx++;

  return E_set_current (this, idx);
}

static ed_t *E_set_prev (E_T *this) {
  int idx = $my(cur_idx);
  if (idx is 0)
    idx = $my(num_items) - 1;
  else
    idx--;

  return E_set_current (this, idx);
}

static string_t *E_create_image (E_T *this) {
  if (NULL is $my(image_name) or NULL is $my(image_file))
    return NULL;

  string_t *img = String.new_with_fmt (
       "func e_image () {\n"
       "  var ed_instances = %d\n"
       "  var flags = 0\n"
       "  var frame_zero = 0\n"
       "  var draw = 1\n"
       "  var donot_draw = 0\n"
       "  var save_image = %d\n"
       "  var persistent_layout = %d\n"
       "  var ed_cur_idx = %d\n"
       "  var win_cur_idx = %d\n"
       "  var buf_cur_idx = %d\n"
       "  var ed = null\n"
       "  var cwin = null\n"
       "  var buf = null\n\n",
    $my(num_items),
    $my(save_image),
    $my(persistent_layout),
    $my(cur_idx),
    $my(current)->cur_idx,
    $my(current)->current->cur_idx);

  ed_t *ed = self(get.head);

  while (ed isnot NULL) {
    int num_win = ed_get_num_win (ed, NO_COUNT_SPECIAL);

    String.append_with_fmt (img,
       "  ed = ed_new (%d)\n", num_win);

    String.append_with (img, "  cwin = ed_get_current_win (ed)\n");

    int l_num_win = 0;
    win_t *cwin = ed_get_win_head (ed);

    while (cwin) {
      if (win_isit_special_type (cwin)) goto next_win;

      if (l_num_win++)
        String.append_with (img, "  cwin = ed_get_win_next (ed, cwin)\n\n");

      buf_t *buf = win_get_buf_head (cwin);
      while (buf) {
        if (($from(buf, flags) & BUF_IS_SPECIAL)) goto next_buf;
        char *bufname = $from(buf, fname);
        char *ftype_name = $from(buf, ftype)->name;
        long autosave = $from(buf, autosave);
        if (autosave > 0) autosave /= 60;
        int save_on_exit = (($from(buf, flags) & BUF_SAVE_ON_EXIT) ? 1 : 0);
        int cur_row_idx = buf->cur_idx;

        String.append_with (img, "\n");

        String.append_with_fmt (img,
              "  buf = win_buf_init (cwin, frame_zero, flags)\n"
              "  buf_init_fname (buf, \"%s\")\n"
              "  buf_set_ftype (buf, \"%s\")\n"
              "  buf_set_autosave (buf, %ld)\n"
              "  buf_set_save_on_exit (buf, %d)\n"
              "  buf_set_row_idx (buf, %d)\n"
              "  win_append_buf (cwin, buf)\n",
            bufname,
            ftype_name,
            autosave,
            save_on_exit,
            cur_row_idx);

next_buf:
        buf = win_get_buf_next (cwin, buf);
      }

next_win:
      cwin = ed_get_win_next (ed, cwin);
    }

    ed = ed->next;
    ifnot (NULL is ed)
      String.append_with (img,
          "  ed = e_set_ed_next ()\n");
  }

  String.append_with_fmt (img, "\n"
      "  e_set_image_file (\"%s\")\n"
      "  e_set_image_name (\"%s\")\n"
      "  e_set_save_image (save_image)\n"
      "  e_set_persistent_layout (persistent_layout)\n\n"
      "  ed = e_set_ed_by_idx (ed_cur_idx)\n"
      "  cwin = ed_set_current_win (ed, win_cur_idx)\n"
      "  win_set_current_buf (cwin, buf_cur_idx, donot_draw)\n\n"
      "  win_draw (cwin)\n"
      "}\n\n"
      "e_image ()\n",
    $my(image_file),
    $my(image_name));

  return img;
}

static int E_save_image (E_T *this, char *name) {
  ifnot ($my(num_items)) return NOTOK;

  if (NULL is name and NULL is $my(image_file))
    if ($my(current) isnot NULL)
      if ($my(current)->current isnot NULL)
        if ($my(current)->current->current isnot NULL)
          name = Path.basename ($from ($my(current)->current->current, fname));

  string_t *fname = NULL;
  char *iname = NULL;

  ifnot (NULL is name) {
    fname = String.new_with (name);
    iname = Cstring.dup (name, bytelen (name));
  } else {
    if (NULL is $my(image_file))
      return NOTOK;

    ifnot (NULL is $my(image_name))
      iname = Cstring.dup ($my(image_name), bytelen ($my(image_name)));
    else
      iname = Path.basename_sans_extname ($my(image_file));

    fname = String.new_with ($my(image_file));
  }

  int retval = NOTOK;

  char *extname = Path.extname (fname->bytes);

  size_t exlen = bytelen (extname);
  if (exlen) {
    if (exlen isnot fname->num_bytes) {
      char *p = fname->bytes + fname->num_bytes - 1;
      while (*p isnot '.') {
        p--;
        String.clear_at (fname, fname->num_bytes - 1);
      }
    } else  // .file
      String.append_byte (fname, '.');
  } else
    String.append_byte (fname, '.');

  ifnot (Path.is_absolute (fname->bytes)) {
    String.prepend_with (fname, "/scripts/");
    String.prepend_with (fname, Sys.get.env_value ("LA_DIR"));
  }

  String.append_with (fname, "lai");

  self(set.image_file, fname->bytes);
  self(set.image_name, iname);

  string_t *image = self(create_image);

  FILE *fp = fopen (fname->bytes, "w");
  if (NULL is fp) goto theend;

  fprintf (fp, "%s\n", image->bytes);
  fclose (fp);
  retval = OK;

theend:
  free (iname);
  String.release (fname);
  String.release (image);
  return retval;
}

static void E_set_image_file (E_T *this, char *name) {
  if (NULL is name) return;

  ifnot (NULL is $my(image_file))
    free ($my(image_file));

  char *cwd = NULL;

  size_t len = bytelen (name);

  char *extname = Path.extname (name);
  size_t exlen = bytelen (extname);

  int hasnot_ext = (0 is exlen or (exlen and 0 is Cstring.eq (extname, ".lai")));

  if (hasnot_ext) len += 4;

  ifnot (Path.is_absolute (name)) {
    cwd = Dir.current ();
    if (NULL is cwd) return;
    len += bytelen (cwd) + 1;
  }

  $my(image_file) = Alloc (len + 1);

  ifnot (Path.is_absolute (name))
    Cstring.cp_fmt ($my(image_file), len + 1, "%s/%s", cwd, name);
  else
    Cstring.cp ($my(image_file), len + 1, name, len - (hasnot_ext ? 4 : 0));

  if (hasnot_ext)
    Cstring.cat ($my(image_file), len + 1, ".lai");
}

static void E_set_image_name (E_T *this, char *name) {
  if (NULL is name) return;

  ifnot (NULL is $my(image_name))
    free ($my(image_name));

  $my(image_name) = Cstring.dup (name, bytelen (name));
}

static void E_set_save_image (E_T *this, int val) {
  $my(save_image) = val;
}

static void E_set_persistent_layout (E_T *this, int val) {
  $my(persistent_layout) = val;
}

static void E_set_state (E_T *this, int state) {
  $my(state) = state;
}

static void E_set_state_bit (E_T *this, int bit) {
  $my(state) |= bit;
}

static void E_unset_state_bit (E_T *this, int bit) {
  $my(state) &= ~(bit);
}

static int E_test_state_bit (E_T *this, int bit) {
  return $my(state) & (bit);
}

static la_T *E_get_la_class (E_T *this) {
  (void) this;
  return &__LA__;
}

static int E_get_state (E_T *this) {
  return $my(state);
}

static term_t *E_get_term (E_T *this) {
  return this->__Ed__->prop->term;
}

static ed_t *E_get_current (E_T *this) {
  return $my(current);
}

static ed_t *E_get_head (E_T *this) {
  return $my(head);
}

static ed_t *E_get_next (E_T *this, ed_t *ed) {
  (void) this;
  return ed->next;
}

static int E_get_num (E_T *this) {
  return $my(num_items);
}

static int E_get_idx (E_T *this, ed_t *ed) {
  int retval = NOTOK;
  ed_t *it = $my(head);
  int idx = -1;
  while (it) {
    idx++;
    if (ed is it) {
      retval = idx;
      break;
    }
    it = it->next;
  }

  return retval;
}

static int E_get_current_idx (E_T *this) {
  return $my(cur_idx);
}

static int E_get_prev_idx (E_T *this) {
  return $my(prev_idx);
}

static int E_get_error_state (E_T *this) {
  return $my(error_state);
}

static void E_set_at_init_cb (E_T *this, EdAtInit_cb cb) {
  $my(at_init_cb) = cb;
}

static void E_set_at_exit_cb (E_T *this, EAtExit_cb cb) {
  if (NULL is cb) return;
  $my(num_at_exit_cbs)++;
  ifnot ($my(num_at_exit_cbs) - 1)
    $my(at_exit_cbs) = Alloc (sizeof (EAtExit_cb));
  else
    $my(at_exit_cbs) = Realloc ($my(at_exit_cbs), sizeof (EAtExit_cb) * $my(num_at_exit_cbs));

  $my(at_exit_cbs)[$my(num_at_exit_cbs) -1] = cb;
}

static ed_t *E_next_editor (E_T *this, buf_t **thisp) {
  ifnot ($my(num_items)) return $my(current);
  ed_t *ed = E_set_next (this);
  win_t * w = ed_get_current_win (ed);
  *thisp = win_get_current_buf (w);
  win_set_current_buf (w, w->cur_idx, DRAW);
  return ed;
}

static ed_t *E_prev_editor (E_T *this, buf_t **thisp) {
  ifnot ($my(num_items)) return $my(current);
  ed_t *ed = E_set_prev (this);
  win_t * w = ed_get_current_win (ed);
  *thisp = win_get_current_buf (w);
  win_set_current_buf (w, w->cur_idx, DRAW);
  return ed;
}

static ed_t *E_prev_focused_editor (E_T *this, buf_t **thisp) {
  ifnot ($my(num_items)) return $my(current);
  ed_t *ed = E_set_current (this, $my(prev_idx));
  win_t * w = ed_get_current_win (ed);
  *thisp = win_get_current_buf (w);
  win_set_current_buf (w, w->cur_idx, DRAW);
  return ed;
}

static ed_t *E_new_editor (E_T *this, buf_t **thisp, char *fname) {
  ed_t *ed = self(new, EdOpts());
  win_t *w = ed_get_current_win (ed);

  *thisp = win_buf_new (w, BufOpts (.fname = fname));

  DListSetCurrent (*thisp, 0);

  win_append_buf (w, *thisp);
  win_set_current_buf (w, 0, DRAW);
  return ed;
}

static int E_exit_all (E_T *this) {
  int force = ($my(state) & ED_EXIT_ALL_FORCE);

  for (;;) {
    ed_t *ed = self(set.current, 0);
    if (NULL is ed) return OK;

    ifnot (force)
      if (NOTHING_TODO is ed_quit (ed, force, 1)) {
        self(set.current, $my(cur_idx));
        return NOTOK;
      }

    self(delete, $my(cur_idx), FORCE);
  }

  return OK;
}

static int E_main (E_T *this, buf_t *buf) {
  ed_t *ed = $from(buf, root);

  int retval = 0;
  int state = 0;

  if ($my(state) & (E_EXIT_ALL|E_EXIT_ALL_FORCE)) {
    if ($my(state) & (E_EXIT_ALL))
      state |= ED_EXIT_ALL;
    else
      state |= ED_EXIT_ALL_FORCE;

    goto exit_all;
  }

main:
  $my(state) &= ~(E_SUSPENDED|E_EXIT|E_PAUSE);
  if (ed_test_state_bit (ed, ED_PAUSE))
    $my(state) |= E_PAUSE;

  retval = ed_main (ed, buf);

  if ($my(state) & E_PAUSE)
    ed_set_state_bit (ed, ED_PAUSE);
  else
    if (ed_test_state_bit (ed, ED_PAUSE))
      $my(state) |= E_PAUSE;

  state = ed_get_state (ed);

  if (state & ED_EXIT)
    $my(state) |= E_EXIT;

  if (state & ED_SUSPENDED) {
    $my(state) |= E_SUSPENDED;
    ed_unset_state_bit (ed, ED_SUSPENDED);
    ed_suspend (ed);
    return retval;
  }

  if (state & ED_NEW) {
    if ($my(state) & E_DONOT_CHANGE_FOCUS)
      goto main;

    ed = E_new_editor (this, &buf, $from(ed, ed_str)->bytes);
    goto main;
  }

  if (state & ED_NEXT) {
    if ($my(state) & E_DONOT_CHANGE_FOCUS)
      goto main;

    ed = E_next_editor (this, &buf);
    goto main;
  }

  if (state & ED_PREV) {
    if ($my(state) & E_DONOT_CHANGE_FOCUS)
      goto main;

    ed = E_prev_editor (this, &buf);
    goto main;
  }

  if (state & ED_PREV_FOCUSED) {
    if ($my(state) & E_DONOT_CHANGE_FOCUS)
      goto main;

    ed = E_prev_focused_editor (this, &buf);
    goto main;
  }

exit_all:
  if ((state & ED_EXIT_ALL) or (state & ED_EXIT_ALL_FORCE)) {
    if ($my(save_image)) {
      if (NULL is $my(image_file))
        self(save_image, $my(image_name));
      else
        self(save_image, NULL);
    }

    if (NOTOK is self(exit_all)) {
      ed = self(get.current);
      buf = win_get_current_buf (ed_get_current_win (ed));
      goto main;
    }

    $my(state) |= E_EXIT;
    return retval;
  }

  if (($my(state) & E_EXIT)) {
    if ($my(state) & E_PAUSE)
      return retval;

    if ($my(save_image))
      if (
          ($my(num_items) is 1 and $my(orig_num_items) is 1) or
          ($my(num_items) is 1 and $my(persistent_layout) is 0) or
          ($my(num_items) is $my(orig_num_items) and $my(persistent_layout) is 1)) {
        if (NULL is $my(image_file))
          self(save_image, $my(image_name));
        else
          self(save_image, NULL);
      }

    self(delete, $my(cur_idx), FORCE);

    ifnot ($my(state) & E_DONOT_CHANGE_FOCUS)
      if ($my(num_items)) {
        ed = self(get.current);
        buf = win_get_current_buf (ed_get_current_win (ed));
        goto main;
      }
  }

  return retval;
}

static ed_T *ed_init_prop (E_T *__E__, ed_T *this) {
  $my(__Win__)     = &this->__Win__;
  $my(__Buf__)     = &this->__Buf__;
  $my(__LA__)      = &this->__LA__;
  $my(__E__)       = __E__;
  $my(__Msg__)     = &this->__Msg__;
  $my(__EError__)  = &this->__EError__;

  $my(Me) = this;
  $my(video) = NULL;
  $my(num_at_exit_cbs) = 0;
  $my(at_exit_cbs) = NULL;
  return this;
}

static int Ed_init (E_T *__E__, ed_T *this) {
  ed_init_prop (__E__, this);

  $my(term) = Term.new ();

  venv_new (this);

  Spell.set.dictionary (STR_FMT ("%s/spell/spell.txt", Sys.get.env_value ("E_DATADIR")));

  Term.set_state_bit ($my(term), (TERM_DONOT_CLEAR_SCREEN|TERM_DONOT_SAVE_SCREEN|TERM_DONOT_RESTORE_SCREEN));

  if (NOTOK is Term.set ($my(term)))
    return NOTOK;

  Term.reset ($my(term));

  Term.unset_state_bit ($my(term), (TERM_DONOT_CLEAR_SCREEN|TERM_DONOT_SAVE_SCREEN|TERM_DONOT_RESTORE_SCREEN));

  setvbuf (stdin, 0, _IONBF, 0);

  return OK;
}

/* interpreter */

#define LaRoot laroot->self

static VALUE la_e_get_ed_num (la_t *this) {
  E_T *laroot = La.get.user_data (this);
  return INT(LaRoot.get.num (laroot));
}

static VALUE la_e_set_ed_next (la_t *this) {
  E_T *laroot = La.get.user_data (this);
  return PTR(LaRoot.set.next (laroot));
}

static VALUE la_e_set_ed_by_idx (la_t *this, VALUE idxv) {
  int idx = AS_INT(idxv);
  E_T *laroot = La.get.user_data (this);
  return PTR(LaRoot.set.current (laroot, idx));
}

static VALUE la_e_set_save_image (la_t *this, VALUE val) {
  E_T *laroot = La.get.user_data (this);
  LaRoot.set.save_image (laroot, AS_INT(val));
  return INT(LA_OK);
}

static VALUE la_e_set_persistent_layout (la_t *this, VALUE valv) {
  int val = AS_INT(valv);
  E_T *laroot = La.get.user_data (this);
  LaRoot.set.persistent_layout (laroot, val);
  return INT(LA_OK);
}

static VALUE la_e_set_image_name (la_t *this, VALUE namev) {
  char *name = AS_STRING_BYTES(namev);
  E_T *laroot = La.get.user_data (this);
  LaRoot.set.image_name (laroot, name);
  return INT(LA_OK);
}

static VALUE la_e_set_image_file (la_t *this, VALUE filev) {
  char *file = AS_STRING_BYTES(filev);
  E_T *laroot = La.get.user_data (this);
  LaRoot.set.image_file (laroot, file);
  return INT(LA_OK);
}

static VALUE la_e_get_ed_current_idx (la_t *this) {
  E_T *laroot = La.get.user_data (this);
  return INT(LaRoot.get.current_idx (laroot));
}

static VALUE la_e_get_ed_current (la_t *this) {
  E_T *laroot = La.get.user_data (this);
  return PTR(LaRoot.get.current (laroot));
}

static VALUE la_ed_new (la_t *this, VALUE num_winv) {
  int num_win = AS_INT(num_winv);
  E_T *laroot = La.get.user_data (this);
  return PTR(LaRoot.new (laroot, EdOpts(.num_win = num_win)));
}

static VALUE la_ed_get_num_win (la_t *this, VALUE edv) {
  (void) this;
  ed_t *ed = (ed_t *) AS_PTR(edv);
  return INT(ed_get_num_win (ed, 0));
}

static VALUE la_ed_get_win_next (la_t *this, VALUE edv, VALUE winv) {
  (void) this;
  ed_t *ed = (ed_t *) AS_PTR(edv);
  win_t *win = (win_t *) AS_PTR(winv);
  return PTR(ed_get_win_next (ed, win));
}

static VALUE la_ed_get_current_win (la_t *this, VALUE edv) {
  (void) this;
  ed_t *ed = (ed_t *) AS_PTR(edv);
  return PTR(ed_get_current_win (ed));
}

static VALUE la_ed_set_current_win (la_t *this, VALUE edv, VALUE idxv) {
  (void) this;
  ed_t *ed = (ed_t *) AS_PTR(edv);
  int idx = AS_INT(idxv);
  return PTR(ed_set_current_win (ed, idx));
}

static VALUE la_buf_init_fname (la_t *this, VALUE bufv, VALUE fnamev) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  char *fn = AS_STRING_BYTES(fnamev);
  return INT(buf_init_fname (buf, fn));
}

static VALUE la_buf_set_ftype (la_t *this, VALUE bufv, VALUE ftypev) {
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  char *ftype = AS_STRING_BYTES(ftypev);
  E_T *laroot = La.get.user_data (this);
  buf_set_ftype (buf, ed_syn_get_ftype_idx ($from(laroot, current), ftype));
  return INT(LA_OK);
}

static VALUE la_buf_set_autosave (la_t *this, VALUE bufv, VALUE minutesv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  long minutes = (long) AS_INT(minutesv);
  buf_set_autosave (buf, minutes);
  return INT(LA_OK);
}

static VALUE la_buf_set_save_on_exit (la_t *this, VALUE bufv, VALUE v_val) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  int val = AS_INT(v_val);
  buf_set_save_on_exit (buf, val);
  return INT(LA_OK);
}

static VALUE la_buf_set_row_idx (la_t *this, VALUE bufv, VALUE rowv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  int row = AS_INT(rowv);
  return INT(buf_set_row_idx (buf, row, NO_OFFSET, 1));
}

static VALUE la_buf_draw (la_t *this, VALUE bufv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  buf_draw (buf);
  return INT(LA_OK);
}

static VALUE la_buf_substitute (la_t *this, VALUE bufv, VALUE patv, VALUE subv, VALUE globalv,
                                            VALUE interactivev, VALUE fidxv, VALUE lidxv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  char *pat = AS_STRING_BYTES(patv);
  char *sub = AS_STRING_BYTES(subv);
  int global = AS_INT(globalv);
  int interactive = AS_INT(interactivev);
  int fidx = AS_INT(fidxv);
  int lidx = AS_INT(lidxv);

  if (fidx is lidx) fidx = lidx = buf->cur_idx;

  return INT(buf_substitute (buf, pat, sub, global, interactive, fidx, lidx));
}

static VALUE la_win_buf_init (la_t *this, VALUE winv, VALUE framev, VALUE flagsv) {
  (void) this;
  int flags = AS_INT(flagsv);
  int frame = AS_INT(framev);
  win_t *win = (win_t *) AS_PTR(winv);
  return PTR(win_buf_init (win, frame, flags));
}

static VALUE la_win_set_current_buf (la_t *this, VALUE winv, VALUE idxv, VALUE drawv) {
  (void) this;
  win_t *win = (win_t *) AS_PTR(winv);
  int idx = AS_INT(idxv);
  int draw = AS_INT(drawv);
  return PTR(win_set_current_buf (win, idx, draw));
}

static VALUE la_win_get_current_buf (la_t *this, VALUE winv) {
  (void) this;
  win_t *win = (win_t *) AS_PTR(winv);
  return PTR(win_get_current_buf (win));
}

static VALUE la_win_draw (la_t *this, VALUE winv) {
  (void) this;
  win_t *win = (win_t *) AS_PTR(winv);
  win_draw (win);
  return INT(LA_OK);
}

static VALUE la_win_append_buf (la_t *this, VALUE winv, VALUE bufv) {
  (void) this;
  win_t *win = (win_t *) AS_PTR(winv);
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  return INT(win_append_buf (win, buf));
}

static VALUE la_buf_normal_page_down (la_t *this, VALUE bufv, VALUE countv, VALUE drawv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  int count = AS_INT(countv);
  int draw = AS_INT(drawv);
  return INT(buf_normal_page_down (buf, count, draw));
}

static VALUE la_buf_normal_page_up (la_t *this, VALUE bufv, VALUE countv, VALUE drawv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  int count = AS_INT(countv);
  int draw = AS_INT(drawv);
  return INT(buf_normal_page_up (buf, count, draw));
}

static VALUE la_buf_normal_goto_linenr (la_t *this, VALUE bufv, VALUE linenumv, VALUE drawv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  int linenum = AS_INT(linenumv);
  int draw = AS_INT(drawv);
  return INT(buf_normal_goto_linenr (buf, linenum, draw));
}

static VALUE la_buf_normal_replace_character_with (la_t *this, VALUE bufv, VALUE cv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  utf8 c = AS_INT(cv);
  return INT(buf_normal_replace_character_with (buf, c));
}

static VALUE la_buf_normal_change_case (la_t *this, VALUE bufv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  return INT(buf_normal_change_case (buf));
}

static VALUE la_buf_insert_string (la_t *this, VALUE bufv, VALUE strv, VALUE drawv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  char *str = AS_STRING_BYTES(strv);
  int draw = AS_INT(drawv);
  return INT(buf_insert_string (buf, str, bytelen (str), draw));
}

static VALUE la_buf_search (la_t *this, VALUE bufv, VALUE comv, VALUE strv, VALUE cv) {
  (void) this;
  buf_t *buf = (buf_t *) AS_PTR(bufv);
  char com = AS_INT(comv);
  char *str = AS_STRING_BYTES(strv);
  int c = AS_INT(cv);
  return INT(buf_search (buf, com, str, c));
}

struct lafun_t {
  const char *name;
  VALUE val;
  int nargs;
} lafuns[] = {
  { "e_set_ed_next",         PTR(la_e_set_ed_next), 0},
  { "e_set_ed_by_idx",       PTR(la_e_set_ed_by_idx), 1},
  { "e_set_save_image",      PTR(la_e_set_save_image), 1},
  { "e_set_persistent_layout", PTR(la_e_set_persistent_layout), 1},
  { "e_set_image_name",      PTR(la_e_set_image_name), 1},
  { "e_set_image_file",      PTR(la_e_set_image_file), 1},
  { "e_get_ed_num",          PTR(la_e_get_ed_num), 0},
  { "e_get_ed_current",      PTR(la_e_get_ed_current), 0},
  { "e_get_ed_current_idx",  PTR(la_e_get_ed_current_idx), 0},
  { "ed_new",                PTR(la_ed_new), 1},
  { "ed_get_num_win",        PTR(la_ed_get_num_win), 1},
  { "ed_get_win_next",       PTR(la_ed_get_win_next), 2},
  { "ed_get_current_win",    PTR(la_ed_get_current_win), 1},
  { "ed_set_current_win",    PTR(la_ed_set_current_win), 2},
  { "buf_set_ftype",         PTR(la_buf_set_ftype), 2},
  { "buf_set_autosave",      PTR(la_buf_set_autosave), 2},
  { "buf_set_save_on_exit",  PTR(la_buf_set_save_on_exit), 2},
  { "buf_set_row_idx",       PTR(la_buf_set_row_idx), 2},
  { "buf_normal_page_up",    PTR(la_buf_normal_page_up), 3},
  { "buf_normal_page_down",  PTR(la_buf_normal_page_down), 3},
  { "buf_normal_change_case",PTR(la_buf_normal_change_case), 1},
  { "buf_normal_goto_linenr",PTR(la_buf_normal_goto_linenr), 3},
  { "buf_normal_replace_character_with", PTR(la_buf_normal_replace_character_with), 2},
  { "buf_insert_string",     PTR(la_buf_insert_string), 3},
  { "buf_search",            PTR(la_buf_search), 4},
  { "buf_draw",              PTR(la_buf_draw), 1},
  { "buf_init_fname",        PTR(la_buf_init_fname), 2},
  { "buf_substitute",        PTR(la_buf_substitute), 7},
  { "win_buf_init",          PTR(la_win_buf_init), 3},
  { "win_draw",              PTR(la_win_draw), 1},
  { "win_append_buf",        PTR(la_win_append_buf), 2},
  { "win_set_current_buf",   PTR(la_win_set_current_buf), 3},
  { "win_get_current_buf",   PTR(la_win_get_current_buf), 1},
  { NULL, NULL_VALUE, 0}
};

static int la_define_funs_default_cb (la_t *this) {
  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  return LA_OK;
}

static LaDefineFuns_cb E_get_la_define_funs_cb (E_T *this) {
  return $my(la_define_funs_cb);
}

static la_T *__init_this_la__ (E_T *this) {
 __LAPTR__ = __init_la__ ();
  __LA__ = *__LAPTR__;
  $my(la_define_funs_cb) = la_define_funs_default_cb;
  this->__Ed__->__LA__ = __LA__;
  return __LAPTR__;
}

static int E_load_file (E_T *this, char *fn, int argc, const char **argv) {
  la_t *la = La.init_instance (&__LA__, LaOpts (
    .argc = argc,
    .argv = argv,
    .define_funs_cb = la_define_funs_default_cb,
    .la_dir = Sys.get.env_value ("LA_DIR")));

  La.set.user_data (la, this);

  int retval = La.load_file (&__LA__, la, fn);

  if (retval is NOTOK)
    tostderr (La.get.message (la));

  return retval;
}

public E_T *__init_ed__ (const char *name) {
  E_T *this = Alloc (sizeof (E_T));
  this->prop = Alloc (sizeof (E_prop));

  *this = (E_T) {
    .self = (E_self) {
      .new = E_new,
      .init = E_init,
      .main = E_main,
      .delete = E_delete,
      .exit_all = E_exit_all,
      .load_file = E_load_file,
      .save_image = E_save_image,
      .create_image = E_create_image,
      .release_info = E_release_info,
      .get = (E_get_self) {
        .num = E_get_num,
        .idx = E_get_idx,
        .head = E_get_head,
        .next = E_get_next,
        .term =  E_get_term,
        .state = E_get_state,
        .la_class = E_get_la_class,
        .current = E_get_current,
        .prev_idx = E_get_prev_idx,
        .current_idx = E_get_current_idx,
        .error_state = E_get_error_state,
        .la_define_funs_cb = E_get_la_define_funs_cb,
        .info = (E_get_info_self) {
          .as_type = E_get_info_as_type
        }
      },
      .set = (E_set_self) {
        .i_dir = E_set_i_dir,
        .state = E_set_state,
        .state_bit = E_set_state_bit,
        .image_name = E_set_image_name,
        .image_file = E_set_image_file,
        .save_image = E_set_save_image,
        .persistent_layout = E_set_persistent_layout,
        .at_exit_cb = E_set_at_exit_cb,
        .at_init_cb = E_set_at_init_cb,
        .next = E_set_next,
        .prev = E_set_prev,
        .current = E_set_current
      },
      .unset = (E_unset_self) {
        .state_bit = E_unset_state_bit
      },
      .test = (E_test_self) {
        .state_bit = E_test_state_bit
      }
    },
    .prop = $myprop,
    .__Ed__ =  editor_new ()
   };

  $my(Me) = this;

  $my(state) = 0;

  __init_this_la__ (this);

  if (NOTOK is Ed_init (this, this->__Ed__)) {
    __deinit_ed__ (&this);
    return NULL;
  }

  $my(__Ed__) = this->__Ed__->self;

  Cstring.cp ($my(name), MAXLEN_ED_NAME, name, MAXLEN_ED_NAME - 1);
  $my(name_gen) = ('z' - 'a') + 1;
  $my(cur_idx) = $my(prev_idx) = -1;
  $my(shared_reg)[0] = (Reg_t) {.reg = REG_SHARED_CHR};
  $my(image_name) = NULL;
  $my(save_image) = 0;
  $my(persistent_layout) = 0;
  $my(orig_num_items) = 0;

  return this;
}

static void ed_deallocate_prop (ed_T *this) {
  if ($myprop is NULL) return;
  Term.release (&$my(term));
  free ($myprop);
  $myprop = NULL;
}

static void deinit_ed (ed_T *this, int restore_state) {
  if (restore_state)
    Term.reset ($my(term));
  ed_deallocate_prop (this);
}

public void __deinit_ed__ (E_T **thisp) {
  if (NULL is *thisp) return;

  E_T *this = *thisp;

  E_exit_all (this);

  for (int i = 0; i < $my(num_at_exit_cbs); i++)
    $my(at_exit_cbs)[i] ();

  reg_release (&$my(shared_reg)[0]);

  if ($my(image_name) isnot NULL)
    free ($my(image_name));

  if ($my(image_file) isnot NULL)
    free ($my(image_file));

  deinit_ed (this->__Ed__, 0 is ($my(state) & E_DONOT_RESTORE_TERM_STATE));

  free (this->__Ed__);

  __deinit_la__ (&__LAPTR__);

  __deinit_sys__ ();
  __deinit_spell__ (&spellType);
  __deinit_vui__ (&vuiType);

  if ($my(num_at_exit_cbs)) free ($my(at_exit_cbs));

  free ($myprop);
  free (this);
  *thisp = NULL;
}
