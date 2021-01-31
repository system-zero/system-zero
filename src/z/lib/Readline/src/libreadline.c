#define LIBRARY "readline"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT
#define REQUIRE_TERMIOS

#define REQUIRE_DLIST_TYPE
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

#define REQUIRE_TERM_MACROS

#include <z/cenv.h>

#define BYTES_TO_READLINE(rl_, bytes, len)                                \
do {                                                                      \
  char *sp_ = (bytes);                                                    \
  for (int i__ = 0; i__ < (len); i__++) {                                 \
    int clen = Ustring.charlen ((bytes)[i__]);                            \
    (rl_)->state |= (READLINE_INSERT_CHAR|READLINE_BREAK);                \
    (rl_)->c = UTF8_CODE (sp_);                                           \
    readline_edit ((rl_));                                                \
    i__ += clen - 1;                                                      \
    sp_ += clen;                                                          \
    }                                                                     \
} while (0)

static readline_t *readline_edit (readline_t *);
static void readline_write_and_break (readline_t *);
static readline_t *readline_complete_history (readline_t *, int *, int);

static const utf8 offsetsFromUTF8[6] = {
  0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static int readline_tab_completion (readline_t *rl) {
  (void) rl;
  return READLINE_OK;
}

static int readline_call_at_beg (readline_t **rl) {
  (*rl)->state |= READLINE_OK;
  return READLINE_OK;
}

static int readline_call_at_end (readline_t **rl) {
  (*rl)->state |= READLINE_OK;
  return READLINE_OK;
}

static int readline_break (readline_t **rl) {
  (*rl)->state |= READLINE_BREAK;
  return READLINE_BREAK;
}

static int readline_last_arg_at_beg (readline_t **rl) {
  switch ((*rl)->c) {
    case READLINE_LAST_ARG_KEY:
    (*rl)->state |= READLINE_POST_PROCESS;
    return READLINE_POST_PROCESS;
  }

  (*rl)->state |= READLINE_OK;
  return READLINE_OK;
}

static int readline_history_at_beg (readline_t **rl) {
  switch ((*rl)->c) {
    case ESCAPE_KEY:
    case '\r':
    case ARROW_UP_KEY:
    case ARROW_DOWN_KEY:
    case '\t':
    (*rl)->state |= READLINE_POST_PROCESS;
    return READLINE_POST_PROCESS;
  }

  (*rl)->state |= READLINE_OK;
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
  readline_t *rl = Alloc (sizeof (readline_t));
  rl->term = term;
  rl->video = video;
  rl->getch = getch;
  rl->at_beg = readline_call_at_beg;
  rl->at_end = readline_call_at_end;
  rl->tab_completion = readline_tab_completion;
  rl->max_num_hist_entries = READLINE_HISTORY_NUM_ENTRIES;
  rl->user_data[0] = user_data;
  rl->prompt_char = READLINE_PROMPT_CHAR;
  rl->prompt_row = prompt_row;
  rl->first_row = prompt_row;
  rl->first_col = first_col;
  rl->num_cols = readline_calc_columns (rl, num_cols);
  rl->row_pos = rl->prompt_row;
  rl->fd = term->out_fd;
  rl->render = String.new (rl->num_cols);
  rl->line = Vstring.new ();
  vstring_t *s = Vstring.new_item ();
  s->data = String.new_with_len (" ", 1);
  DListAppendCurrent (rl->line, s);

  rl->state |= (READLINE_OK|READLINE_IS_VISIBLE);
  rl->opts  |= (READLINE_OPT_HAS_HISTORY_COMPLETION|READLINE_OPT_HAS_TAB_COMPLETION);
  return rl;
}

static int readline_exec (readline_t *this) {
  if (this->exec isnot NULL)
    return this->exec (this);
  return READLINE_OK;
}

static void readline_release_members (readline_t *rl) {
  Vstring.release (rl->line);
  readline_arg_t *it = rl->head;
  while (it isnot NULL) {
    readline_arg_t *tmp = it->next;
    String.release (it->argname);
    String.release (it->argval);
    free (it);
    it = tmp;
  }

  rl->num_items = 0;
  rl->cur_idx = 0;
}

public void readline_release (readline_t *rl) {
  readline_release_members (rl);
  String.release (rl->render);
  free (rl);
}

static void readline_clear (readline_t *rl) {
  rl->state &= ~READLINE_CLEAR;
  int row = rl->first_row;
  while (row < rl->prompt_row)
    Video.draw.row_at (rl->video, row++);

//  if (rl->prompt_row is $from(rl->ed, prompt_row))
    Video.set.row_with (rl->video, rl->prompt_row - 1, " ");

  Video.draw.row_at (rl->video, rl->prompt_row);

  if (rl->state & READLINE_CLEAR_FREE_LINE) {
    Vstring.release (rl->line);
    rl->num_items = 0;
    rl->cur_idx = 0;
    rl->line = Vstring.new ();
    vstring_t *vstr = Vstring.new_item ();
    vstr->data = String.new_with_len (" ", 1);
    DListAppendCurrent (rl->line, vstr);
  }

  rl->state &= ~READLINE_CLEAR_FREE_LINE;
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

static void readline_render (readline_t *rl) {
  int has_prompt_char = (rl->prompt_char isnot 0);
  String.clear (rl->render);

  if (rl->state & READLINE_SET_POS) goto set_pos;

  vstring_t *chars = rl->line->head;
  rl->row_pos = rl->prompt_row - rl->num_rows + 1;

  String.append_with_fmt (rl->render, "%s" TERM_GOTO_PTR_POS_FMT
      TERM_SET_COLOR_FMT "%c", TERM_CURSOR_HIDE, rl->first_row,
      rl->first_col, COLOR_PROMPT, rl->prompt_char);

  int cidx = 0;

  for (int i = 0; i < rl->num_rows; i++) {
    if (i)
      String.append_with_fmt (rl->render, TERM_GOTO_PTR_POS_FMT, rl->first_row + i,
          rl->first_col);

    for (cidx = 0; (cidx + (i * rl->num_cols) + (i == 0 ? has_prompt_char : 0))
       < rl->line->num_items and cidx < (rl->num_cols -
          (i == 0 ? has_prompt_char : 0)); cidx++) {
      String.append_with (rl->render, chars->data->bytes);
      chars = chars->next;
    }
  }

  while (cidx++ < rl->num_cols - 1) String.append_byte (rl->render, ' ');

  String.append_with_fmt (rl->render, "%s%s", TERM_COLOR_RESET,
    (rl->state & READLINE_CURSOR_HIDE) ? "" : TERM_CURSOR_SHOW);

set_pos:
  rl->state &= ~READLINE_SET_POS;
  int row = rl->first_row;
  int col = rl->first_col;

  row += ((rl->line->cur_idx + has_prompt_char) / rl->num_cols);
  col += ((rl->line->cur_idx + has_prompt_char) < rl->num_cols
    ? has_prompt_char + rl->line->cur_idx
    : ((rl->line->cur_idx + has_prompt_char) % rl->num_cols));

  String.append_with_fmt (rl->render, TERM_GOTO_PTR_POS_FMT, row, col);
}

static void readline_write (readline_t *rl) {
  int orig_first_row = rl->first_row;

  if (rl->line->num_items + 1 <= rl->num_cols) {
    rl->num_rows = 1;
    rl->first_row = rl->prompt_row;
  } else {
    int mod = rl->line->num_items % rl->num_cols;
    rl->num_rows = (rl->line->num_items / rl->num_cols) + (mod isnot 0);
    if  (rl->num_rows is 0) rl->num_rows = 1;
    rl->first_row = rl->prompt_row - rl->num_rows + 1;
  }

  while (rl->first_row > orig_first_row)
    Video.draw.row_at (rl->video, orig_first_row++);

  readline_render (rl);
  IO.fd.write (rl->fd, rl->render->bytes, rl->render->num_bytes);
  rl->state &= ~READLINE_WRITE;
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

  readline_t *lrl = readline_new (this->user_data, this->term, IO.getkey, this->prompt_row,
      1, this->num_cols, this->video);

  lrl->at_beg = readline_last_arg_at_beg;
  lrl->at_end = readline_break;

  lrl->prompt_row = this->first_row - 1;
  lrl->prompt_char = 0;

loop_again:
  if (lrl->line isnot NULL)
    Vstring.release (lrl->line);

  lrl->line = Vstring.dup (this->line);
  BYTES_TO_READLINE (lrl, this->last_component->current->data->bytes,
                 (int) this->last_component->current->data->num_bytes);
  readline_write_and_break (lrl);

get_char:;
  utf8 c = readline_edit (lrl)->c;
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
  this->line = Vstring.dup (lrl->line);

theend:
  readline_clear (lrl);
  readline_release (lrl);
  Video.draw.row_at (this->video, this->first_row); // is minus one
  return this;
}

static readline_t *readline_edit (readline_t *rl) {
  vstring_t *ch;
  int retval;

  if (rl->state & READLINE_CLEAR) {
    readline_clear (rl);
    if (rl->state & READLINE_BREAK) goto theend;
  }

  if (rl->state & READLINE_WRITE) {
    if (rl->state & READLINE_IS_VISIBLE)
      readline_write (rl);
    else
      rl->state &= ~READLINE_WRITE;

    if (rl->state & READLINE_BREAK)
      goto theend;
  }

  if (rl->state & READLINE_PROCESS_CHAR)
    goto process_char;

  if (rl->state & READLINE_INSERT_CHAR)
    goto insert_char;

  for (;;) {
thecontinue:
    rl->state &= ~READLINE_CONTINUE;

    if (rl->state & READLINE_IS_VISIBLE) {
//      ed_check_msg_status (rl->ed);
      readline_write (rl);
    }

    rl->c = rl->getch (STDIN_FILENO);

    retval = rl->at_beg (&rl);
    switch (retval) {
      case READLINE_OK: break;
      case READLINE_BREAK: goto theend; // CHANGE debug
      case READLINE_PROCESS_CHAR: goto process_char;
      case READLINE_CONTINUE: goto thecontinue;
      case READLINE_POST_PROCESS: goto post_process;
    }

process_char:
    rl->state &= ~READLINE_PROCESS_CHAR;

    if (rl->line->num_items is 1) {
      if (array_any_int (rl->first_chars, rl->first_chars_len, rl->c)) {
        if (rl->opts & READLINE_OPT_HAS_TAB_COMPLETION) {
          if (rl->trigger_first_char_completion) {
            rl->state |= (READLINE_INSERT_CHAR|READLINE_FIRST_CHAR_COMPLETION);
            goto insert_char;
          }
        }
      }
    }

theloop:
    switch (rl->c) {
      case ESCAPE_KEY:
      case '\r':
        goto theend;

      case ARROW_UP_KEY:
      case ARROW_DOWN_KEY:
        ifnot (rl->opts & READLINE_OPT_HAS_HISTORY_COMPLETION) goto post_process;
        rl->history->history_idx = (rl->c is ARROW_DOWN_KEY
            ? 0 : rl->history->num_items - 1);
        rl = readline_complete_history (rl, &rl->history->history_idx,
            (rl->c is ARROW_DOWN_KEY ? -1 : 1));
        goto post_process;

      case READLINE_LAST_ARG_KEY:
        rl = readline_complete_last_arg (rl);
        goto post_process;

      case ARROW_LEFT_KEY:
         if (rl->line->cur_idx > 0) {
           rl->line->current = rl->line->current->prev;
           rl->line->cur_idx--;
           rl->state |= READLINE_SET_POS;
         }
         goto post_process;

      case ARROW_RIGHT_KEY:
         if (rl->line->cur_idx < (rl->line->num_items - 1)) {
           rl->line->current = rl->line->current->next;
           rl->line->cur_idx++;
           rl->state |= READLINE_SET_POS;
         }
         goto post_process;

      case HOME_KEY:
      case CTRL('a'):
        rl->line->cur_idx = 0;
        rl->line->current = rl->line->head;
        rl->state |= READLINE_SET_POS;
        goto post_process;

      case END_KEY:
      case CTRL('e'):
        rl->line->cur_idx = (rl->line->num_items - 1);
        rl->line->current =  rl->line->tail;
        rl->state |= READLINE_SET_POS;
        goto post_process;

      case DELETE_KEY:
        if (rl->line->cur_idx is (rl->line->num_items - 1) or
           (rl->line->cur_idx is 0 and rl->line->current->data->bytes[0] is ' ' and
            rl->line->num_items is 0))
          goto post_process;
        {
          vstring_t *tmp = DListPopCurrent (rl->line, vstring_t);
          if (NULL isnot tmp) {String.release (tmp->data); free (tmp);}
        }
        goto post_process;

      case BACKSPACE_KEY: {
          if (rl->line->cur_idx is 0) continue;
          rl->line->current = rl->line->current->prev;
          rl->line->cur_idx--;
          vstring_t *tmp = DListPopCurrent (rl->line, vstring_t);
          if (NULL isnot tmp) {String.release (tmp->data); free (tmp);}
        }
        goto post_process;

      case CTRL('l'):
        rl->state |= READLINE_CLEAR_FREE_LINE;
        readline_clear (rl);
        goto post_process;

      //case CTRL('r'):
      //  readline_reg (rl);
      //  goto post_process;

      case '\t':
        ifnot (rl->opts & READLINE_OPT_HAS_TAB_COMPLETION)
          goto post_process;

        retval = rl->tab_completion (rl);

        if (rl->opts & READLINE_OPT_RETURN_AFTER_TAB_COMPLETION) {
          rl->c = '\r';
          goto theend;
        }

        switch (retval) {
          case READLINE_PROCESS_CHAR:
            goto process_char;
        }


        goto post_process;

      default:
insert_char:
        rl->state &= ~READLINE_INSERT_CHAR;

        if (rl->c < ' ' or
            rl->c is INSERT_KEY or
           (rl->c > 0x7f and (rl->c < 0x0a0 or (rl->c >= FN_KEY(1) and rl->c <= FN_KEY(12)) or
           (rl->c >= ARROW_DOWN_KEY and rl->c < HOME_KEY) or
           (rl->c > HOME_KEY and (rl->c is PAGE_DOWN_KEY or rl->c is PAGE_UP_KEY or rl->c is END_KEY))
           ))) {
          if (rl->state & READLINE_BREAK) goto theend;
          goto post_process;
        }

        if (rl->c < 0x80) {
          ch = Vstring.new_item ();
          ch->data = String.new_with_fmt ("%c", rl->c);
        } else {
          ch = Vstring.new_item ();
          char buf[5]; int len;
          ch->data = String.new_with (Ustring.character (rl->c, buf, &len));
        }

        if (rl->line->cur_idx is rl->line->num_items - 1 and ' ' is rl->line->current->data->bytes[0]) {
          DListPrependCurrent (rl->line, ch);
          rl->line->current = rl->line->current->next;
          rl->line->cur_idx++;
        } else if (rl->line->cur_idx isnot rl->line->num_items - 1) {
          DListPrependCurrent (rl->line, ch);
          rl->line->current = rl->line->current->next;
          rl->line->cur_idx++;
        }
        else
          DListAppendCurrent (rl->line, ch);

        if (rl->state & READLINE_BREAK)
          goto theend;

        if (rl->state & READLINE_FIRST_CHAR_COMPLETION) {
            rl->state &= ~READLINE_FIRST_CHAR_COMPLETION;
            rl->c = '\t';
            goto theloop;
        }

        goto post_process;

    }

post_process:
    rl->state &= ~READLINE_POST_PROCESS;
    if (rl->state & READLINE_BREAK) goto theend;
    retval = rl->at_end (&rl);
    switch (retval) {
      case READLINE_BREAK: goto theend;
      case READLINE_PROCESS_CHAR: goto process_char;
      case READLINE_CONTINUE: goto thecontinue;
    }
  }

theend:
  rl->state &= ~READLINE_BREAK;
  return rl;
}

static readline_t *readline_complete_history (readline_t *rl, int *idx, int dir) {
  ifnot (rl->history->num_items) return rl;

  if (dir is -1) {
    if (*idx is 0)
      *idx = rl->history->num_items - 1;
    else
      *idx -= 1;
  } else {
    if (*idx is rl->history->num_items - 1)
      *idx = 0;
    else
      *idx += 1;
  }

  int lidx = 0;
  readline_hist_item_t *it = rl->history->head;

  while (lidx < *idx) { it = it->next; lidx++; }

  readline_t *lrl = readline_new (rl->user_data, rl->term, IO.getkey, rl->prompt_row,
      1, rl->num_cols, rl->video);

  lrl->prompt_row = rl->first_row - 1;
  lrl->prompt_char = 0;

  ReadlineAtBeg_cb at_beg = rl->at_beg;
  ReadlineAtEnd_cb at_end = rl->at_end;
  rl->at_beg = readline_history_at_beg;
  rl->at_end = readline_break;

  int counter = rl->history->num_items;

  goto thecheck;

theiter:
  ifnot (--counter)
    goto theend;

  if (dir is -1) {
    if (it->prev is NULL) {
      lidx = rl->history->num_items - 1;
      it = rl->history->tail;
    } else {
      it = it->prev;
      lidx--;
    }
  } else {
    if (it->next is NULL) {
      lidx = 0;
      it = rl->history->head;
    } else {
      lidx++;
      it = it->next;
    }
  }

thecheck:;
#define __free_strings__ String.release (str); String.release (cur)

  string_t *str = Vstring.join (it->data->line, "");
  string_t *cur = Vstring.join (rl->line, "");
  if (cur->bytes[cur->num_bytes - 1] is ' ')
    String.clear_at (cur, cur->num_bytes - 1);
  int match = (Cstring.eq_n (str->bytes, cur->bytes, cur->num_bytes));

  if (0 is cur->num_bytes or rl->history->num_items is 1 or match) {
    __free_strings__;
    goto theinput;
  }

  __free_strings__;
  goto theiter;

theinput:
  readline_release_members (lrl);
  lrl->line = Vstring.dup (it->data->line);
  lrl->first_row = it->data->first_row;
  lrl->row_pos = it->data->row_pos;

  readline_write_and_break (lrl);

  utf8 c = readline_edit (rl)->c;
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
  readline_release_members (rl);
  rl->line = Vstring.dup (it->data->line);
  rl->first_row = it->data->first_row;
  rl->row_pos = it->data->row_pos;

theend:
  readline_clear (lrl);
  Video.draw.row_at (rl->video, rl->first_row); // is minus one
  readline_release (lrl);
  rl->at_beg = at_beg;
  rl->at_end = at_end;
  return rl;
}

static void readline_write_and_break (readline_t *rl){
  rl->state |= (READLINE_WRITE|READLINE_BREAK);
  readline_edit (rl);
}

#if 0
static void readline_insert_char_and_break (readline_t *rl) {
  rl->state |= (READLINE_INSERT_CHAR|READLINE_BREAK);
  readline_edit (rl);
}
#endif

/*
static void readline_reg (readline_t *rl) {
  ed_t *this = rl->ed;
  int regidx = ed_reg_get_idx (this, Input.get ($my(term)));
  if (NOTOK is regidx) return;

  buf_t *buf = self(get.current_buf);
  if (ERROR is ed_reg_special_set (this, buf, regidx))
    return;

  Reg_t *rg = &$my(regs)[regidx];
  if (rg->type is LINEWISE) return;

  reg_t *reg = rg->head;
  while (reg isnot NULL) {
    BYTES_TO_READLINE (rl, reg->data->bytes, (int) reg->data->num_bytes);
    reg = reg->next;
  }
}
*/

static void readline_last_component_push (readline_t *this) {
  if (this->last_component_push is NULL) return;
  if (this->last_component is NULL) return;
  if (this->tail is NULL) return;
  if (this->tail->argval is NULL) return;
  this->last_component_push (this);
}

static vstring_t *readline_parse_command (readline_t *rl) {
  vstring_t *it = rl->line->head;
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
  rl->com = READLINE_NO_COMMAND;

  int i = 0;
  for (i = 0; i < rl->commands_len; i++) {
    if (Cstring.eq (rl->commands[i]->com, com)) {
      rl->com = i;
      break;
    }
  }

  return it;
}

static readline_t *readline_parse (readline_t *rl) {
  vstring_t *it = readline_parse_command (rl);

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
        //if (this)
        //  MSG_ERRNO (READLINE_ARGUMENT_MISSING_ERROR);
        rl->com = READLINE_ARGUMENT_MISSING_ERROR;
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
            //if (this)
             // MSG_ERRNO (READLINE_ARG_AWAITING_STRING_OPTION_ERROR);
            rl->com = READLINE_ARG_AWAITING_STRING_OPTION_ERROR;
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
                if (arg->argval->bytes[arg->argval->num_bytes - 1] is '\\' and
                    arg->argval->bytes[arg->argval->num_bytes - 2] isnot '\\') {
                  arg->argval->bytes[arg->argval->num_bytes - 1] = '"';
                  is_quoted = 1;
                  it = it->next;
                  continue;
                }
                else { /* accept empty string --opt="" */
                  type |= READLINE_TOK_ARG_OPTION;
                  goto arg_type;
                }
              }
            }

            String.append_with (arg->argval, it->data->bytes);
            it = it->next;
          }

          if (is_quoted){
            //if (this)
            //  MSG_ERRNO (READLINE_UNTERMINATED_QUOTED_STRING_ERROR);
            rl->com = READLINE_UNTERMINATED_QUOTED_STRING_ERROR;
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
          if (rl->com < rl->commands_len) {
            int idx = 0;
            while (idx < rl->commands[rl->com]->num_args) {
              ifnot (NULL is rl->commands[rl->com]->args[idx]) {
                if (Cstring.eq_n (opt->bytes, rl->commands[rl->com]->args[idx]+2, opt->num_bytes)) {
                  found_arg = 1;
                  break;
                }
              }
              idx++;
            }
          }

          ifnot (found_arg) {
            //if (this)
           //   MSG_ERRNO (READLINE_UNRECOGNIZED_OPTION);
            rl->com = READLINE_UNRECOGNIZED_OPTION;
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

      //if (rl->com is VED_COM_BUF_CHANGE or rl->com is VED_COM_BUF_CHANGE_ALIAS) {
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
          DListAppendCurrent (rl, larg);
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
    DListAppendCurrent (rl, arg);

itnext:
    if (it isnot NULL) it = it->next;
  }

theend:
  return rl;
}

static void readline_set_line (readline_t *rl, char *bytes, size_t len) {
  BYTES_TO_READLINE (rl, bytes, (int) len);
  readline_write_and_break (rl);
}

static void readline_set_opts (readline_t *rl, int opts) {
  rl->opts = opts;
}

static void readline_set_opts_bit (readline_t *rl, int bit) {
  rl->opts |= bit;
}

static void readline_set_state (readline_t *rl, int state) {
  rl->state = state;
}

static void readline_set_state_bit (readline_t *rl, int bit) {
  rl->state |= bit;
}

static void readline_set_prompt_char (readline_t *rl, char c) {
  rl->prompt_char = c;
}

static void readline_set_visibility (readline_t *rl, int visible) {
  if (visible)
    rl->state |= READLINE_IS_VISIBLE;
  else if (0 is visible)
    rl->state &= ~READLINE_IS_VISIBLE;
}

static int readline_get_state (readline_t *rl) {
  return rl->state;
}

static string_t *readline_get_line (readline_t *rl) {
  return Vstring.join (rl->line, "");
}

static int readline_get_opts (readline_t *rl) {
  return rl->opts;
}

static Vstring_t *readline_get_arg_fnames (readline_t *rl, int num) {
  Vstring_t *fnames = Vstring.new ();
  readline_arg_t *arg = rl->head;
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

static readline_arg_t *readline_get_arg (readline_t *rl, int type) {
  readline_arg_t *arg = rl->tail;
  while (arg) {
    if (arg->type & type) return arg;
    arg = arg->prev;
  }

  return NULL;
}

static string_t *readline_get_anytype_arg (readline_t *rl, char *argname) {
  readline_arg_t *arg = rl->tail;
  while (arg) {
    if (arg->type & READLINE_ARG_ANYTYPE) {
      if (Cstring.eq (arg->argname->bytes, argname))
        return arg->argval;
    }
    arg = arg->prev;
  }

  return NULL;
}

static string_t *readline_get_command (readline_t *rl) {
  string_t *str = String.new (8);
  vstring_t *it = rl->line->head;

  while (it isnot NULL and it->data->bytes[0] isnot ' ') {
    String.append_with (str, it->data->bytes);
    it = it->next;
  }

  return str;
}

static Vstring_t *readline_get_anytype_args (readline_t *rl, char *argname) {
  Vstring_t *args = NULL;
  readline_arg_t *arg = rl->head;
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

  readline_hist_item_t *hrl = Alloc (sizeof (readline_hist_item_t));
  hrl->data = this;
  DListPrependCurrent (this->history, hrl);
}

static int readline_arg_exists (readline_t *rl, char *argname) {
  readline_arg_t *arg = rl->head;
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
        .prompt_char = readline_set_prompt_char
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
