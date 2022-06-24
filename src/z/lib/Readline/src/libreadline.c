#define LIBRARY "readline"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT
#define REQUIRE_TERMIOS

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE  DECLARE
#define REQUIRE_USTRING_TYPE  DECLARE
#define REQUIRE_VIDEO_TYPE    DECLARE
#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_DIR_TYPE      DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_READLINE_TYPE DONOT_DECLARE
#define REQUIRE_KEYS_MACROS
#define REQUIRE_LIST_MACROS

#define REQUIRE_TERM_MACROS

#include <z/cenv.h>

static readline_t *readline_edit (readline_t *);
static void readline_write_and_break (readline_t *);
static readline_t *readline_complete_history (readline_t *, int *, int);
static void readline_set_line (readline_t *, char *, size_t);

static void readline_insert_with_len (readline_t *this, char *bytes, size_t len) {
  char *sp = bytes;
  for (size_t i = 0; i < len; i++) {
    int clen = Ustring.charlen (bytes[i]);
    this->state |= (READLINE_INSERT_CHAR|READLINE_BREAK);
    this->c = Ustring.get.code (sp);
    readline_edit (this);
    i += clen - 1;
    sp += clen;
  }
}

static int readline_tab_completion (readline_t *this) {
  (void) this;
  return READLINE_OK;
}

static int readline_call_at_beg (readline_t **thisp) {
  (*thisp)->state |= READLINE_OK;
  return READLINE_OK;
}

static int readline_call_at_end (readline_t **thisp) {
  (*thisp)->state |= READLINE_OK;
  return READLINE_OK;
}

static int readline_set_break_state (readline_t **thisp) {
  (*thisp)->state |= READLINE_BREAK;
  return READLINE_BREAK;
}

static void readline_call_expr_reg (readline_t *this) {
  (void) this;
}

static void readline_call_on_write (readline_t *this) {
  (void) this;
}

static void readline_call_err (readline_t *this, int error) {
  (void) this; (void) error;
}

static int readline_last_arg_at_beg (readline_t **thisp) {
  switch ((*thisp)->c) {
    case READLINE_LAST_ARG_KEY:
    (*thisp)->state |= READLINE_POST_PROCESS;
    return READLINE_POST_PROCESS;
  }

  (*thisp)->state |= READLINE_OK;
  return READLINE_OK;
}

static int readline_history_at_beg (readline_t **thisp) {
  switch ((*thisp)->c) {
    case ESCAPE_KEY:
    case '\r':
    case ARROW_UP_KEY:
    case ARROW_DOWN_KEY:
    case '\t':
    (*thisp)->state |= READLINE_POST_PROCESS;
    return READLINE_POST_PROCESS;
  }

  (*thisp)->state |= READLINE_OK;
  return READLINE_OK;
}

static int readline_calc_columns (readline_t *this, int num_cols) {
  int cols = this->first_col + num_cols;
  ifnot (NULL is this->calc_columns)
    this->calc_columns (this, cols);
  return cols;
}

static readline_t *readline_new (void *user_data, term_t *term, InputGetch_cb getch,
      int prompt_row, int first_col, int num_cols, video_t *video) {
  readline_t *this = Alloc (sizeof (readline_t));
  this->term = term;
  this->video = video;
  this->getch = getch;
  this->at_beg = readline_call_at_beg;
  this->at_end = readline_call_at_end;
  this->error = readline_call_err;
  this->expr_reg = readline_call_expr_reg;
  this->on_write = readline_call_on_write;
  this->tab_completion = readline_tab_completion;
  this->max_num_hist_entries = READLINE_HISTORY_NUM_ENTRIES;
  this->user_data[0] = user_data;
  this->prompt_char = READLINE_PROMPT_CHAR;
  this->prompt_row = prompt_row;
  this->first_row = prompt_row;
  this->first_col = first_col;
  this->num_cols = readline_calc_columns (this, num_cols);
  this->row_pos = this->prompt_row;
  this->fd = term->out_fd;
  this->render = String.new (this->num_cols);
  this->line = Vstring.new ();
  vstring_t *s = Vstring.new_item ();
  s->data = String.new_with_len (" ", 1);
  DListAppendCurrent (this->line, s);

  this->state |= (READLINE_OK|READLINE_IS_VISIBLE);
  this->opts  |= (READLINE_OPT_HAS_HISTORY_COMPLETION|READLINE_OPT_HAS_TAB_COMPLETION);
  this->opts  |= (READLINE_OPT_CLEAR_PROMPTLINE_AT_END);
  return this;
}

static int readline_exec (readline_t *this) {
  if (this->exec isnot NULL)
    return this->exec (this);
  return READLINE_OK;
}

static void readline_release_members (readline_t *this) {
  Vstring.release (this->line);
  readline_arg_t *it = this->head;
  while (it isnot NULL) {
    readline_arg_t *tmp = it->next;
    String.release (it->argname);
    String.release (it->argval);
    free (it);
    it = tmp;
  }

  this->num_items = 0;
  this->cur_idx = 0;
}

static void readline_release (readline_t *this) {
  readline_release_members (this);
  String.release (this->render);
  free (this);
}

static void readline_clear (readline_t *this) {
  this->state &= ~READLINE_CLEAR;
  int row = this->first_row;
  while (row < this->prompt_row)
    Video.draw.row_at (this->video, row++);

  if (this->opts & READLINE_OPT_CLEAR_PROMPTLINE_AT_END)
    Video.set.row_with (this->video, this->prompt_row - 1, " ");

  Video.draw.row_at (this->video, this->prompt_row);

  if (this->state & READLINE_CLEAR_FREE_LINE) {
    Vstring.release (this->line);
    this->num_items = 0;
    this->cur_idx = 0;
    this->line = Vstring.new ();
    vstring_t *vstr = Vstring.new_item ();
    vstr->data = String.new_with_len (" ", 1);
    DListAppendCurrent (this->line, vstr);
  }

  this->state &= ~READLINE_CLEAR_FREE_LINE;
}

static vstring_t *readline_quote_subcommand (vstring_t *it) {
  if (it is NULL) return NULL;

  vstring_t *it_head = it->prev;

  char c = it->data->bytes[0];

  ifnot (c is '{') goto theend;

  // quote by default
  if (c isnot '"') it->data->bytes[0] = '"';

  it = it->next;

  char pc = 0, ppc = 0;
  int num_quotes = 0;

  while (it) {
    c = it->data->bytes[0];

    if (c is '}') {
      ifnot (num_quotes % 2) {
        it->data->bytes[0] = '"';
        break;
      }

      // allow to fail from my caller
      if (it->next is NULL)
        it->prev->next = NULL;
      else {
        it->next->prev = it->prev;
        it->prev->next = it->next;
      }

      break;
    }

    if (pc) ppc = pc;
    pc = c;

    if (c is '"' and
       (pc isnot '\\' or (pc is '\\' and ppc isnot '\\'))) {
       num_quotes++;

      vstring_t *vs = Vstring.new_item ();
      vs->data = String.new_with ("\\");

      vs->next = it;
      it->prev->next = vs;
      vs->prev = it->prev;
      it->prev = vs;
    }

    it = it->next;
  }

theend:
  return it_head;
}

static void readline_render (readline_t *this) {
  int has_prompt_char = (this->prompt_char isnot 0);
  String.clear (this->render);

  if (this->state & READLINE_SET_POS) goto set_pos;

  vstring_t *chars = this->line->head;
  this->row_pos = this->prompt_row - this->num_rows + 1;

  String.append_with_fmt (this->render, "%s" TERM_GOTO_PTR_POS_FMT
      TERM_SET_COLOR_FMT "%c", TERM_CURSOR_HIDE, this->first_row,
      this->first_col, COLOR_PROMPT, this->prompt_char);

  int cidx = 0;

  for (int i = 0; i < this->num_rows; i++) {
    if (i)
      String.append_with_fmt (this->render, TERM_GOTO_PTR_POS_FMT, this->first_row + i,
          this->first_col);

    for (cidx = 0; (cidx + (i * this->num_cols) + (i == 0 ? has_prompt_char : 0))
       < this->line->num_items and cidx < (this->num_cols -
          (i == 0 ? has_prompt_char : 0)); cidx++) {
      String.append_with (this->render, chars->data->bytes);
      chars = chars->next;
    }
  }

  //while (cidx++ < this->num_cols - 1) String.append_byte (this->render, ' ');
  while (cidx++ < this->num_cols - 2) String.append_byte (this->render, ' ');

  String.append_with_fmt (this->render, "%s%s", TERM_COLOR_RESET,
    (this->state & READLINE_CURSOR_HIDE) ? "" : TERM_CURSOR_SHOW);

set_pos:
  this->state &= ~READLINE_SET_POS;
  int row = this->first_row;
  int col = this->first_col;

  row += ((this->line->cur_idx + has_prompt_char) / this->num_cols);
  col += ((this->line->cur_idx + has_prompt_char) < this->num_cols
    ? has_prompt_char + this->line->cur_idx
    : ((this->line->cur_idx + has_prompt_char) % this->num_cols));

  String.append_with_fmt (this->render, TERM_GOTO_PTR_POS_FMT, row, col);
}

static void readline_write (readline_t *this) {
  int orig_first_row = this->first_row;

  if (this->line->num_items + 1 <= this->num_cols) {
    this->num_rows = 1;
    this->first_row = this->prompt_row;
  } else {
    int mod = this->line->num_items % this->num_cols;
    this->num_rows = (this->line->num_items / this->num_cols) + (mod isnot 0);
    if  (this->num_rows is 0) this->num_rows = 1;
    this->first_row = this->prompt_row - this->num_rows + 1;
  }

  while (this->first_row > orig_first_row)
    Video.draw.row_at (this->video, orig_first_row++);

  readline_render (this);

  FD.write (this->fd, this->render->bytes, this->render->num_bytes);
  this->state &= ~READLINE_WRITE;
}

static int array_any_int (int *ar, size_t len, int c) {
  for (size_t i = 0; i < len; i++)
    if (ar[i] is c)
      return 1;

  return 0;
}

static readline_t *readline_complete_last_arg (readline_t *this) {
  if (this->last_component is NULL or
      this->last_component->num_items is 0)
    return this;

  this->last_component->current = this->last_component->head;

  readline_t *lthis = readline_new (this->user_data, this->term, Input.getkey,
      this->prompt_row - 1,  1, this->num_cols, this->video);

  lthis->at_beg = readline_last_arg_at_beg;
  lthis->at_end = readline_set_break_state;

  lthis->opts &= ~READLINE_OPT_CLEAR_PROMPTLINE_AT_END;
  lthis->prompt_char = 0;

loop_again:
  if (lthis->line isnot NULL)
    Vstring.release (lthis->line);

  lthis->line = Vstring.dup (this->line);

  readline_set_line (lthis, this->last_component->current->data->bytes,
      this->last_component->current->data->num_bytes);

get_char:;
  utf8 c = readline_edit (lthis)->c;
  switch (c) {
    case ESCAPE_KEY:
      goto theend;

    case ' ':
    case '\r': goto thesuccess;
    case READLINE_LAST_ARG_KEY:
      if (this->last_component->current is this->last_component->tail)
        goto theend;

      this->last_component->current = this->last_component->current->next;
      goto loop_again;

    default: goto get_char;
  }

thesuccess:
  Vstring.release (this->line);
  this->line = Vstring.dup (lthis->line);

theend:
  readline_clear (lthis);
  readline_release (lthis);
  Video.draw.row_at (this->video, this->prompt_row);
  return this;
}

static readline_t *readline_edit (readline_t *this) {
  vstring_t *ch;
  int retval;

  if (this->state & READLINE_CLEAR) {
    readline_clear (this);
    if (this->state & READLINE_BREAK) goto theend;
  }

  if (this->state & READLINE_WRITE) {
    if (this->state & READLINE_IS_VISIBLE) {
      this->on_write (this);

      readline_write (this);
    } else
      this->state &= ~READLINE_WRITE;

    if (this->state & READLINE_BREAK)
      goto theend;
  }

  if (this->state & READLINE_PROCESS_CHAR)
    goto process_char;

  if (this->state & READLINE_INSERT_CHAR)
    goto insert_char;

  for (;;) {
thecontinue:
    this->state &= ~READLINE_CONTINUE;

    if (this->state & READLINE_IS_VISIBLE) {
      this->on_write (this);
      readline_write (this);
    }

    this->c = this->getch (STDIN_FILENO);

    retval = this->at_beg (&this);
    switch (retval) {
      case READLINE_OK: break;
      case READLINE_BREAK: goto theend; // CHANGE debug
      case READLINE_PROCESS_CHAR: goto process_char;
      case READLINE_CONTINUE: goto thecontinue;
      case READLINE_POST_PROCESS: goto post_process;
    }

process_char:
    this->state &= ~READLINE_PROCESS_CHAR;

    if (this->line->num_items is 1) {
      if (array_any_int (this->first_chars, this->first_chars_len, this->c)) {
        if (this->opts & READLINE_OPT_HAS_TAB_COMPLETION) {
          if (this->trigger_first_char_completion) {
            this->state |= (READLINE_INSERT_CHAR|READLINE_FIRST_CHAR_COMPLETION);
            goto insert_char;
          }
        }
      }
    }

theloop:
    switch (this->c) {
      case ESCAPE_KEY:
      case '\r':
        goto theend;

      case ARROW_UP_KEY:
      case ARROW_DOWN_KEY:
        ifnot (this->opts & READLINE_OPT_HAS_HISTORY_COMPLETION) goto post_process;
        this->history->history_idx = (this->c is ARROW_DOWN_KEY
            ? 0 : this->history->num_items - 1);
        this = readline_complete_history (this, &this->history->history_idx,
            (this->c is ARROW_DOWN_KEY ? -1 : 1));
        goto post_process;

      case READLINE_LAST_ARG_KEY:
        this = readline_complete_last_arg (this);
        goto post_process;

      case ARROW_LEFT_KEY:
         if (this->line->cur_idx > 0) {
           this->line->current = this->line->current->prev;
           this->line->cur_idx--;
           this->state |= READLINE_SET_POS;
         }
         goto post_process;

      case ARROW_RIGHT_KEY:
         if (this->line->cur_idx < (this->line->num_items - 1)) {
           this->line->current = this->line->current->next;
           this->line->cur_idx++;
           this->state |= READLINE_SET_POS;
         }
         goto post_process;

      case HOME_KEY:
      case CTRL('a'):
        this->line->cur_idx = 0;
        this->line->current = this->line->head;
        this->state |= READLINE_SET_POS;
        goto post_process;

      case END_KEY:
      case CTRL('e'):
        this->line->cur_idx = (this->line->num_items - 1);
        this->line->current =  this->line->tail;
        this->state |= READLINE_SET_POS;
        goto post_process;

      case DELETE_KEY:
        if (this->line->cur_idx is (this->line->num_items - 1) or
           (this->line->cur_idx is 0 and this->line->current->data->bytes[0] is ' ' and
            this->line->num_items is 0))
          goto post_process;
        {
          vstring_t *tmp = DListPopCurrent (this->line, vstring_t);
          if (NULL isnot tmp) {String.release (tmp->data); free (tmp);}
        }
        goto post_process;

      case BACKSPACE_KEY: {
          if (this->line->cur_idx is 0) continue;
          this->line->current = this->line->current->prev;
          this->line->cur_idx--;
          vstring_t *tmp = DListPopCurrent (this->line, vstring_t);
          if (NULL isnot tmp) {String.release (tmp->data); free (tmp);}
        }
        goto post_process;

      case CTRL('l'):
        this->state |= READLINE_CLEAR_FREE_LINE;
        readline_clear (this);
        goto post_process;

      case CTRL('r'):
        this->expr_reg (this);
        goto post_process;

      case '\t':
        ifnot (this->opts & READLINE_OPT_HAS_TAB_COMPLETION)
          goto post_process;

        retval = this->tab_completion (this);

        if (this->opts & READLINE_OPT_RETURN_AFTER_TAB_COMPLETION) {
          this->c = '\r';
          goto theend;
        }

        switch (retval) {
          case READLINE_PROCESS_CHAR:
            goto process_char;
        }


        goto post_process;

      default:
insert_char:
        this->state &= ~READLINE_INSERT_CHAR;

        if (this->c < ' ' or
            this->c is INSERT_KEY or
           (this->c > 0x7f and (this->c < 0x0a0 or (this->c >= FN_KEY(1) and this->c <= FN_KEY(12)) or
           (this->c >= ARROW_DOWN_KEY and this->c < HOME_KEY) or
           (this->c > HOME_KEY and (this->c is PAGE_DOWN_KEY or this->c is PAGE_UP_KEY or this->c is END_KEY))
           ))) {
          if (this->state & READLINE_BREAK) goto theend;
          goto post_process;
        }

        if (this->c < 0x80) {
          ch = Vstring.new_item ();
          ch->data = String.new_with_fmt ("%c", this->c);
        } else {
          ch = Vstring.new_item ();
          char buf[5]; int len;
          ch->data = String.new_with (Ustring.character (this->c, buf, &len));
        }

        if (this->line->cur_idx is this->line->num_items - 1 and ' ' is this->line->current->data->bytes[0]) {
          DListPrependCurrent (this->line, ch);
          this->line->current = this->line->current->next;
          this->line->cur_idx++;
        } else if (this->line->cur_idx isnot this->line->num_items - 1) {
          DListPrependCurrent (this->line, ch);
          this->line->current = this->line->current->next;
          this->line->cur_idx++;
        }
        else
          DListAppendCurrent (this->line, ch);

        if (this->state & READLINE_BREAK)
          goto theend;

        if (this->state & READLINE_FIRST_CHAR_COMPLETION) {
            this->state &= ~READLINE_FIRST_CHAR_COMPLETION;
            this->c = '\t';
            goto theloop;
        }

        goto post_process;

    }

post_process:
    this->state &= ~READLINE_POST_PROCESS;
    if (this->state & READLINE_BREAK) goto theend;
    retval = this->at_end (&this);
    switch (retval) {
      case READLINE_BREAK: goto theend;
      case READLINE_PROCESS_CHAR: goto process_char;
      case READLINE_CONTINUE: goto thecontinue;
    }
  }

theend:
  this->state &= ~READLINE_BREAK;
  return this;
}

static readline_t *readline_complete_history (readline_t *this, int *idx, int dir) {
  ifnot (this->history->num_items) return this;

  if (dir is -1) {
    if (*idx is 0)
      *idx = this->history->num_items - 1;
    else
      *idx -= 1;
  } else {
    if (*idx is this->history->num_items - 1)
      *idx = 0;
    else
      *idx += 1;
  }

  int lidx = 0;
  readline_hist_item_t *it = this->history->head;

  while (lidx < *idx) { it = it->next; lidx++; }

  readline_t *lthis = readline_new (this->user_data, this->term, Input.getkey,
      this->prompt_row - 1, 1, this->num_cols, this->video);

  lthis->opts &= ~READLINE_OPT_CLEAR_PROMPTLINE_AT_END;
  lthis->prompt_char = 0;

  ReadlineAtBeg_cb at_beg = this->at_beg;
  ReadlineAtEnd_cb at_end = this->at_end;
  this->at_beg = readline_history_at_beg;
  this->at_end = readline_set_break_state;

  int counter = this->history->num_items;

  goto thecheck;

theiter:
  ifnot (--counter)
    goto theend;

  if (dir is -1) {
    if (it->prev is NULL) {
      lidx = this->history->num_items - 1;
      it = this->history->tail;
    } else {
      it = it->prev;
      lidx--;
    }
  } else {
    if (it->next is NULL) {
      lidx = 0;
      it = this->history->head;
    } else {
      lidx++;
      it = it->next;
    }
  }

thecheck:;
#define __free_strings__ String.release (str); String.release (cur)

  string_t *str = Vstring.join (it->data->line, "");
  string_t *cur = Vstring.join (this->line, "");
  if (cur->bytes[cur->num_bytes - 1] is ' ')
    String.clear_at (cur, cur->num_bytes - 1);
  int match = (Cstring.eq_n (str->bytes, cur->bytes, cur->num_bytes));

  if (0 is cur->num_bytes or this->history->num_items is 1 or match) {
    __free_strings__;
    goto theinput;
  }

  __free_strings__;
  goto theiter;

theinput:
  readline_release_members (lthis);
  lthis->line = Vstring.dup (it->data->line);
  lthis->first_row = it->data->first_row;
  lthis->row_pos = it->data->row_pos;

  readline_write_and_break (lthis);

  utf8 c = readline_edit (this)->c;
  switch (c) {
    case ESCAPE_KEY:
      goto theend;

    case ' ':
    case '\r': goto thesuccess;
    case ARROW_DOWN_KEY: dir = -1; goto theiter;
    case ARROW_UP_KEY: dir = 1; goto theiter;
    default: goto theiter;
  }

thesuccess:
  readline_release_members (this);
  this->line = Vstring.dup (it->data->line);
  this->first_row = it->data->first_row;
  this->row_pos = it->data->row_pos;

theend:
  readline_clear (lthis);
  Video.draw.row_at (this->video, this->prompt_row);
  readline_release (lthis);
  this->at_beg = at_beg;
  this->at_end = at_end;
  return this;
}

static void readline_write_and_break (readline_t *this) {
  this->state |= (READLINE_WRITE|READLINE_BREAK);
  readline_edit (this);
}

static void readline_last_component_push (readline_t *this) {
  if (this->last_component_push is NULL) return;
  if (this->last_component is NULL) return;
  if (this->tail is NULL) return;
  if (this->tail->argval is NULL) return;
  this->last_component_push (this);
}

static vstring_t *readline_parse_command (readline_t *this) {
  vstring_t *it = this->line->head;
  char com[READLINE_MAXLEN_COM]; com[0] = '\0';
  int com_idx = 0;

  while (it isnot NULL and it->data->bytes[0] is ' ') it = it->next;

  if (it isnot NULL and it->data->bytes[0] is '!') {
    com[0] = '!'; com[1] = '\0';
    goto get_command;
  }

  while (it isnot NULL and it->data->bytes[0] isnot ' ') {
    for (size_t zi = 0; zi < it->data->num_bytes; zi++)
      com[com_idx++] = it->data->bytes[zi];
    it = it->next;
  }
  com[com_idx] = '\0';

get_command:
  this->com = READLINE_NO_COMMAND;

  int i = 0;
  for (i = 0; i < this->num_commands; i++) {
    if (Cstring.eq (this->commands[i]->com, com)) {
      this->com = i;
      break;
    }
  }

  return it;
}

static readline_t *readline_parse (readline_t *this) {
  vstring_t *it = readline_parse_command (this);

  while (it) {
    int type = 0;
    if (Cstring.eq (it->data->bytes, " ")) goto itnext;

    readline_arg_t *arg = Alloc (sizeof (readline_arg_t));
    string_t *opt = NULL;

    if (it->data->bytes[0] is '-') {
      if (it->next and it->next->data->bytes[0] is '-') {
        type |= READLINE_TOK_ARG_LONG;
        it = it->next;
      } else
        type |= READLINE_TOK_ARG_SHORT;

      it = it->next;
      if (it is NULL) {
        this->error (this, READLINE_ARGUMENT_MISSING_ERROR);
        this->com = READLINE_ARGUMENT_MISSING_ERROR;
        goto theerror;
      }

      opt = String.new (8);
      while (it) {
        if (it->data->bytes[0] is ' ')
          goto arg_type;

        if (it->data->bytes[0] is '=') {
          if (Cstring.eq (opt->bytes, "command"))
            it = readline_quote_subcommand (it->next);

          if (it->next is NULL) {
            this->error (this, READLINE_ARG_AWAITING_STRING_OPTION_ERROR);
            this->com = READLINE_ARG_AWAITING_STRING_OPTION_ERROR;
            goto theerror;
          }

          it = it->next;

          arg->argval = String.new (8);

          int is_quoted = '"' is it->data->bytes[0];
          if (is_quoted) it = it->next;

          while (it) {
            if (' ' is it->data->bytes[0]) {
              ifnot (is_quoted) {
                type |= READLINE_TOK_ARG_OPTION;
                goto arg_type;
              }
            }

            if ('"' is it->data->bytes[0]) {
              if (is_quoted) {
                is_quoted = 0;
                if (arg->argval->num_bytes is 1 and arg->argval->bytes[0] is '\\') {
                  arg->argval->bytes[arg->argval->num_bytes - 1] = '"';
                  is_quoted = 1;
                  it = it->next;
                   continue;
                } else if (arg->argval->num_bytes > 1 and arg->argval->bytes[arg->argval->num_bytes - 1] is '\\' and
                           arg->argval->bytes[arg->argval->num_bytes - 2] isnot '\\') {
                  arg->argval->bytes[arg->argval->num_bytes - 1] = '"';
                  is_quoted = 1;
                  it = it->next;
                  continue;
                } else { /* accept empty string --opt="" */
                  type |= READLINE_TOK_ARG_OPTION;
                  goto arg_type;
                }
              }
            }

            String.append_with (arg->argval, it->data->bytes);
            it = it->next;
          }

          if (is_quoted){
            this->error (this, READLINE_UNTERMINATED_QUOTED_STRING_ERROR);
            this->com = READLINE_UNTERMINATED_QUOTED_STRING_ERROR;
            goto theerror;
          }

          goto arg_type;
        }

        String.append_with (opt, it->data->bytes);
        it = it->next;
      }

arg_type:
      if (arg->argname isnot NULL) {
        String.replace_with (arg->argname, opt->bytes);
      } else
        arg->argname = String.new_with (opt->bytes);

      if (type & READLINE_TOK_ARG_OPTION) {
        if (Cstring.eq (opt->bytes, "pat"))
          arg->type |= READLINE_ARG_PATTERN;
        else if (Cstring.eq (opt->bytes, "sub"))
          arg->type |= READLINE_ARG_SUB;
        else if (Cstring.eq (opt->bytes, "range"))
          arg->type |= READLINE_ARG_RANGE;
        else if (Cstring.eq (opt->bytes, "bufname"))
          arg->type |= READLINE_ARG_BUFNAME;
        else if (Cstring.eq (opt->bytes, "fname"))
          arg->type |= READLINE_ARG_FILENAME;
        else {
          arg->type |= READLINE_ARG_ANYTYPE;
          int found_arg = 0;
          if (this->com < this->num_commands) {
            int idx = 0;
            while (idx < this->commands[this->com]->num_args) {
              ifnot (NULL is this->commands[this->com]->args[idx]) {
                if (Cstring.eq_n (opt->bytes, this->commands[this->com]->args[idx]+2, opt->num_bytes)) {
                  found_arg = 1;
                  break;
                }
              }
              idx++;
            }
          }

          ifnot (found_arg) {
            this->error (this, READLINE_UNRECOGNIZED_OPTION);
            this->com = READLINE_UNRECOGNIZED_OPTION;
          }
        }

        goto argtype_succeed;
      } else {
        if (Cstring.eq (opt->bytes, "i") or Cstring.eq (opt->bytes, "interactive"))
          arg->type |= READLINE_ARG_INTERACTIVE;
        else if (Cstring.eq (opt->bytes, "global"))
          arg->type |= READLINE_ARG_GLOBAL;
        else if (Cstring.eq (opt->bytes, "append"))
          arg->type |= READLINE_ARG_APPEND;
        else if (Cstring.eq (opt->bytes, "verbose"))
          arg->type |= READLINE_ARG_VERBOSE;
        else if (Cstring.eq (opt->bytes, "r") or Cstring.eq (opt->bytes, "recursive"))
          arg->type |= READLINE_ARG_RECURSIVE;
        else
          arg->type |= READLINE_ARG_ANYTYPE;

        goto argtype_succeed;
      }

theerror:
      String.release (opt);
      String.release (arg->argname);
      String.release (arg->argval);
      free (arg);
      goto theend;

argtype_succeed:
      String.release (opt);
      goto append_arg;
    } else {
      arg->argname = String.new_with (it->data->bytes);
      it = it->next;
      while (it isnot NULL and 0 is (Cstring.eq (it->data->bytes, " "))) {
        String.append_with (arg->argname, it->data->bytes);
        it = it->next;
      }

      //if (this->com is VED_COM_BUF_CHANGE or this->com is VED_COM_BUF_CHANGE_ALIAS) {
      //  opt = String.new_with ("bufname");
      //  arg->argval = String.new_with (arg->argname->bytes);
      //  type |= READLINE_TOK_ARG_OPTION;
      //  goto arg_type;
      //}

      char *glob = Cstring.byte.in_str (arg->argname->bytes, '*');
      ifnot (NULL is glob) {
        dirlist_t *dlist = NULL;
        string_t *dir = String.new (16);
        string_t *pre = NULL;
        string_t *post = NULL;

        if (arg->argname->num_bytes is 1) {
          String.append_byte (dir, '.');
          goto getlist;
        }

        char *sp = glob;
        ifnot (sp is arg->argname->bytes) {
          pre = String.new (sp - arg->argname->bytes + 1);
          while (--sp >= arg->argname->bytes and *sp isnot DIR_SEP)
            String.prepend_byte (pre, *sp);

          ifnot (*sp is DIR_SEP) sp++;
        }

        if (sp is arg->argname->bytes)
          String.append_byte (dir, '.');
        else
          while (--sp >= arg->argname->bytes)
            String.prepend_byte (dir, *sp);

        ifnot (bytelen (glob) is 1) {
          post = String.new ((arg->argname->bytes - glob) + 1);
          sp = glob + 1;
          while (*sp) String.append_byte (post, *sp++);
        }
getlist:
        dlist = Dir.list (dir->bytes, 0);
        if (NULL is dlist or dlist->list->num_items is 0) goto free_strings;
        vstring_t *fit = dlist->list->head;

        while (fit) {
          char *fname = fit->data->bytes;
           /* matter to change */
          if (fname[fit->data->num_bytes - 1] is DIR_SEP) goto next_fname;

          if (pre isnot NULL)
            ifnot (Cstring.eq_n (fname, pre->bytes, pre->num_bytes)) goto next_fname;

          if (post isnot NULL) {
            int pi; int fi = fit->data->num_bytes - 1;
            for (pi = post->num_bytes - 1; pi >= 0 and fi >= 0; pi--, fi--)
              if (fname[fi] isnot post->bytes[pi]) break;

            if (pi isnot -1) goto next_fname;
          }
          readline_arg_t *larg = Alloc (sizeof (readline_arg_t));
          ifnot (Cstring.eq (dir->bytes, "."))
            larg->argval = String.new_with_fmt ("%s/%s", dir->bytes, fname);
          else
            larg->argval = String.new_with (fname);

          larg->type |= READLINE_ARG_FILENAME;
          DListAppendCurrent (this, larg);
next_fname:
          fit = fit->next;
        }
free_strings:
        ifnot (NULL is pre) String.release (pre);
        ifnot (NULL is post) String.release (post);
        ifnot (NULL is dlist) dlist->release (dlist);
        String.release (arg->argname);
        String.release (dir);
        free (arg);
        goto itnext;
      } else {
        arg->type |= READLINE_ARG_FILENAME;
        arg->argval = String.new_with (arg->argname->bytes);
      }
    }

append_arg:
    DListAppendCurrent (this, arg);

itnext:
    if (it isnot NULL) it = it->next;
  }

theend:
  return this;
}

static void readline_set_line (readline_t *this, char *bytes, size_t len) {
  readline_insert_with_len (this, bytes, len);
  readline_write_and_break (this);
}

static void readline_set_opts (readline_t *this, int opts) {
  this->opts = opts;
}

static void readline_set_opts_bit (readline_t *this, int bit) {
  this->opts |= bit;
}

static void readline_set_state (readline_t *this, int state) {
  this->state = state;
}

static void readline_set_state_bit (readline_t *this, int bit) {
  this->state |= bit;
}

static void readline_set_prompt_char (readline_t *this, char c) {
  this->prompt_char = c;
}

static void readline_set_visibility (readline_t *this, int visible) {
  if (visible)
    this->state |= READLINE_IS_VISIBLE;
  else if (0 is visible)
    this->state &= ~READLINE_IS_VISIBLE;
}

static int readline_get_state (readline_t *this) {
  return this->state;
}

static string_t *readline_get_line (readline_t *this) {
  return Vstring.join (this->line, "");
}

static int readline_get_opts (readline_t *this) {
  return this->opts;
}

static Vstring_t *readline_get_arg_fnames (readline_t *this, int num) {
  Vstring_t *fnames = Vstring.new ();
  readline_arg_t *arg = this->head;
  if (num < 0) num = 256000;
  while (arg and num) {
    if (arg->type & READLINE_ARG_FILENAME) {
      Vstring.append_uniq (fnames, arg->argval->bytes);
      num--;
    }
    arg = arg->next;
  }

  ifnot (fnames->num_items) {
    free (fnames);
    return NULL;
  }

  fnames->current = fnames->head;
  fnames->cur_idx = 0;
  return fnames;
}

static readline_arg_t *readline_get_arg (readline_t *this, int type) {
  readline_arg_t *arg = this->tail;
  while (arg) {
    if (arg->type & type) return arg;
    arg = arg->prev;
  }

  return NULL;
}

static string_t *readline_get_anytype_arg (readline_t *this, const char *argname) {
  readline_arg_t *arg = this->tail;
  while (arg) {
    if (arg->type & READLINE_ARG_ANYTYPE) {
      if (Cstring.eq (arg->argname->bytes, argname))
        return arg->argval;
    }
    arg = arg->prev;
  }

  return NULL;
}

static string_t *readline_get_command (readline_t *this) {
  string_t *str = String.new (8);
  vstring_t *it = this->line->head;

  while (it isnot NULL and it->data->bytes[0] isnot ' ') {
    String.append_with (str, it->data->bytes);
    it = it->next;
  }

  return str;
}

static Vstring_t *readline_get_anytype_args (readline_t *this, const char *argname) {
  Vstring_t *args = NULL;
  readline_arg_t *arg = this->head;
  while (arg) {
    if (arg->type & READLINE_ARG_ANYTYPE) {
      if (Cstring.eq (arg->argname->bytes, argname)) {
        if (NULL is args) args = Vstring.new ();
        Vstring.current.append_with (args, arg->argval->bytes);
      }
    }
    arg = arg->next;
  }

  return args;
}

static void readline_history_push (readline_t *this) {
  if (this->max_num_hist_entries < this->history->num_items) {
    readline_hist_item_t *tmp = DListPopTail (this->history, readline_hist_item_t);
    readline_release (tmp->data);
    free (tmp);
  }

  readline_hist_item_t *hthis = Alloc (sizeof (readline_hist_item_t));
  hthis->data = this;
  DListPrependCurrent (this->history, hthis);
}

static int readline_arg_exists (readline_t *this, const char *argname) {
  readline_arg_t *arg = this->head;
  while (arg) {
    if (Cstring.eq (arg->argname->bytes, argname)) return 1;
    arg = arg->next;
  }

  return 0;
}

public readline_T __init_readline__ (void) {
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (vstring);
  __INIT__ (ustring);
  __INIT__ (video);
  __INIT__ (io);
  __INIT__ (dir);
  __INIT__ (term);

  return (readline_T) {
    .self = (readline_self) {
      .new = readline_new,
      .edit = readline_edit,
      .exec = readline_exec,
      .clear = readline_clear,
      .parse = readline_parse,
      .release= readline_release,
      .parse_command = readline_parse_command,
      .write_and_break = readline_write_and_break,
      .insert_with_len = readline_insert_with_len,
      .last_component_push = readline_last_component_push,
      .get = (readline_get_self) {
        .arg = readline_get_arg,
        .opts = readline_get_opts,
        .line = readline_get_line,
        .state = readline_get_state,
        .command = readline_get_command,
        .arg_fnames = readline_get_arg_fnames,
        .anytype_arg = readline_get_anytype_arg,
        .anytype_args = readline_get_anytype_args
      },
      .set = (readline_set_self) {
        .line = readline_set_line,
        .opts = readline_set_opts,
        .state = readline_set_state,
        .opts_bit = readline_set_opts_bit,
        .state_bit = readline_set_state_bit,
        .visibility = readline_set_visibility,
        .prompt_char = readline_set_prompt_char,
        .break_state = readline_set_break_state
      },
      .arg = (readline_arg_self) {
       .exists = readline_arg_exists
      },
      .history = (readline_history_self) {
        .push = readline_history_push
      }
    }
  };
}
