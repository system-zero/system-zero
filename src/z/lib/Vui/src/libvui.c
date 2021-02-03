#define LIBRARY "vui"

#define REQUIRE_STDIO
#define REQUIRE_TERMIOS

#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_VSTRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_VIDEO_TYPE    DECLARE
#define REQUIRE_READLINE_TYPE DECLARE
#define REQUIRE_TERM_TYPE     DONOT_DECLARE
#define REQUIRE_VUI_TYPE      DONOT_DECLARE
#define REQUIRE_TERM_MACROS
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

#define $my(__p__) this.prop->__p__

struct vui_prop {
  int num_rows;
  int num_cols;
};


static void vui_menu_release_list (menu_t *this) {
  if (this->state & MENU_LIST_IS_ALLOCATED) {
    Vstring.release (this->list);
    this->state &= ~MENU_LIST_IS_ALLOCATED;
  }
}

static void vui_menu_release (menu_t *this) {
  vui_menu_release_list (this);
  String.release (this->header);
  free (this);
}

static menu_t *vui_menu_new (menu_opts opts) {
  menu_t *this = Alloc (sizeof (menu_t));
  this->fd = opts.fd;
  this->first_row = opts.first_row;
  this->last_row = opts.last_row;
  this->prompt_row = opts.prompt_row;
  this->first_col = opts.first_col + 1;
  this->num_cols = opts.num_cols;
  this->space_selects = opts.space_selects;
  this->return_if_one_item = opts.return_if_one_item;
  this->clear_and_continue_on_backspace = opts.clear_and_continue_on_backspace;
  this->video = opts.video;
  this->term = opts.term;
  this->process_list = opts.process_list_cb;
  this->getch = opts.getch_cb;
  this->patlen = opts.patlen;
  this->next_key = opts.next_key;
  this->user_data[0] = opts.user_data_first;
  this->user_data[1] = opts.user_data_second;

  this->orig_first_row = opts.first_row;
  this->num_rows = this->last_row - this->first_row + 1;
  this->orig_num_rows = this->num_rows;
  this->state |= (MENU_INIT|READLINE_IS_VISIBLE);
  this->header = String.new (8);

  ifnot (NULL is opts.pat) {
    if (this->patlen <= 0)  this->patlen = bytelen (opts.pat);
    if (this->patlen > MAXLEN_PATTERN) this->patlen = MAXLEN_PATTERN;

    Cstring.cp (this->pat, MAXLEN_PATTERN + 1, opts.pat, this->patlen);
  } else {
    this->patlen = 0;
    this->pat[0] = '\0';
  }

  this->retval = NOTOK;
  this->min_first_row = 3;

  if (this->process_list isnot NULL)
    this->retval = this->process_list (this);

  return this;
}

static void vui_menu_clear (menu_t *this) {
  if (this->header->num_bytes)
    Video.draw.row_at (this->video, this->first_row - 1);

  for (int i = 0;i < this->num_rows; i++)
    Video.draw.row_at (this->video, this->first_row + i);
}

static int readline_menu_at_beg (readline_t **rl) {
  switch ((*rl)->c) {
    case ESCAPE_KEY:
    case '\r':
    case ARROW_LEFT_KEY:
    case ARROW_RIGHT_KEY:
    case ARROW_UP_KEY:
    case ARROW_DOWN_KEY:
    case '\t':

      (*rl)->state |= READLINE_POST_PROCESS;
      return READLINE_POST_PROCESS;
  }

  (*rl)->state |= READLINE_OK;
  return READLINE_OK;
}

static int readline_menu_at_end (readline_t **rl) {
  menu_t *menu = (menu_t *) (*rl)->user_data[READLINE_MENU_USER_DATA_IDX];

  switch ((*rl)->c) {
    case BACKSPACE_KEY:
      if (menu->clear_and_continue_on_backspace) {
        vui_menu_clear (menu);
       (*rl)->state |= READLINE_CONTINUE;
       return READLINE_CONTINUE;
      }
      break;
  }

  (*rl)->state |= READLINE_BREAK;
  return READLINE_BREAK;
}

static char *vui_menu_create (menu_t *this) {
  readline_t *rl = Readline.new (this->user_data[0], this->term, this->getch,
     this->prompt_row,  1, this->num_cols, this->video);
  rl->at_beg = readline_menu_at_beg;
  rl->at_end = readline_menu_at_end;
  rl->user_data[READLINE_MENU_USER_DATA_IDX] = this;
  rl->state |= READLINE_IS_VISIBLE|READLINE_CURSOR_HIDE;
  rl->prompt_char = 0;

  if (this->state & READLINE_IS_VISIBLE) rl->state &= ~READLINE_IS_VISIBLE;

  Readline.insert_with_len (rl, this->pat, this->patlen);

init_list:;
  if (this->state & MENU_REINIT_LIST) {
    this->state &= ~MENU_REINIT_LIST;
    vui_menu_clear (this);
    this->first_row = this->orig_first_row;
  }

  char *match = NULL;
  int cur_idx = 0;
  int maxlen = 0;
  int vcol_pos = 1;
  int vrow_pos = 0;
  int frow_idx = 0;
  int num_rows = 0;
  int num_cols = 0;
  int mod = 0;
  int num = 1;
  int rend_rows = this->orig_num_rows;
  int first_row = this->first_row;
  int first_col = this->first_col;
  int orig_beh  = this->return_if_one_item;

  ifnot (this->list->num_items) goto theend;

  vstring_t *it = this->list->head;

  if (this->list->num_items is 1)
    if (this->return_if_one_item) {
      this->c = '\r';
      goto handle_char;
    }

  while (it) {
    if ((int) it->data->num_bytes > maxlen) maxlen = it->data->num_bytes;
    it = it->next;
  }

  ifnot (maxlen) goto theend;
  maxlen++;

  while ((first_col + 1) and first_col + maxlen > this->num_cols)
     first_col--;

  int avail_cols = this->num_cols - first_col;

  if (maxlen < avail_cols) {
    num = avail_cols / maxlen;
    if ((num - 1) + (maxlen * num) > avail_cols)
      num--;
  } else {
    num = 1;
    maxlen = avail_cols;
  }

  mod = this->list->num_items % num;
  num_cols = (num * maxlen);
  num_rows = (this->list->num_items / num) + (mod isnot 0);
  // +  (this->header->num_bytes isnot 0);

  if (num_rows > rend_rows) {
    while (first_row > this->min_first_row and num_rows > rend_rows) {
      first_row--;
      rend_rows++;
    }
  } else {
    rend_rows = num_rows;
  }

  this->num_rows = rend_rows;
  this->first_row = first_row;

  char *fmt = STR_FMT ("\033[%%dm%%-%ds%%s", maxlen);

  vrow_pos = first_row;

  for (;;) {
    it = this->list->head;
    for (int i = 0; i < frow_idx; i++)
      for (int j = 0; j < num; j++)
        it = it->next;

    string_t *render = String.new_with (TERM_CURSOR_HIDE);

    if (this->header->num_bytes) {
      String.append_with_fmt (render, TERM_GOTO_PTR_POS_FMT TERM_SET_COLOR_FMT,
         first_row - 1, first_col, COLOR_MENU_HEADER);

      if ((int) this->header->num_bytes > num_cols) {
        String.clear_at (this->header, num_cols - 1);
      } else
      while ((int) this->header->num_bytes < num_cols)
        String.append_byte (this->header, ' ');

      String.append_with_fmt (render, "%s%s", this->header->bytes, TERM_COLOR_RESET);
    }

    int ridx, iidx = 0; int start_row = 0;
    for (ridx = 0; ridx < rend_rows; ridx++) {
      start_row = first_row + ridx;
      String.append_with_fmt (render, TERM_GOTO_PTR_POS_FMT, start_row, first_col);

      for (iidx = 0; iidx < num and iidx + (ridx  * num) + (frow_idx * num) < this->list->num_items; iidx++) {
        int len = ((int) it->data->num_bytes > maxlen ? maxlen : (int) it->data->num_bytes);
        char item[len + 1];
        Cstring.cp (item, len + 1, it->data->bytes, len);

        int color = (iidx + (ridx * num) + (frow_idx * num) is cur_idx)
           ? COLOR_MENU_SEL : COLOR_MENU_BG;
        String.append_with_fmt (render, fmt, color, item, TERM_COLOR_RESET);
        it = it->next;
      }

      if (mod)
        for (int i = mod + 1; i < num; i++)
          for (int j = 0; j < maxlen; j++)
            String.append_byte (render, ' ');
   }

//    String.append_with (render, TERM_CURSOR_SHOW);

    IO.fd.write (this->fd, render->bytes, render->num_bytes);

    String.release (render);

    this->c = Readline.edit (rl)->c;
    if (this->state & MENU_QUIT) goto theend;

handle_char:

    if (this->c is this->next_key)
      this->c = ARROW_RIGHT_KEY;

    switch (this->c) {
      case ESCAPE_KEY: goto theend;

      case '\r':
      case ' ':
        if (' ' is this->c and this->space_selects is 0)
          goto insert_char;

        it = this->list->head;
        for (int i = 0; i < cur_idx; i++) it = it->next;
        match = it->data->bytes;
        goto theend;

      case ARROW_LEFT_KEY:
        ifnot (0 is cur_idx) {
          if (vcol_pos > 1) {
            cur_idx--;
            vcol_pos--;
            continue;
          }

          cur_idx += (num - vcol_pos);
          vcol_pos += (num - vcol_pos);
        }

        //__fallthrough__;

      case ARROW_UP_KEY:
        if (vrow_pos is first_row) {
          ifnot (frow_idx) {
            cur_idx = this->list->num_items - 1;
            vcol_pos = 1 + (mod ? mod - 1 : 0);
            frow_idx = num_rows - rend_rows;
            vrow_pos = first_row + rend_rows - 1;
            continue;
          }

          frow_idx--;
        } else
          vrow_pos--;

        cur_idx -= num;
        continue;

      case '\t':
      case ARROW_RIGHT_KEY:
         if (vcol_pos isnot num and cur_idx < this->list->num_items - 1) {
           cur_idx++;
           vcol_pos++;
           continue;
         }

        cur_idx -= (vcol_pos - 1);
        vcol_pos = 1;

        //__fallthrough__;

      case ARROW_DOWN_KEY:
        if (vrow_pos is this->last_row or (vrow_pos - first_row + 1 is num_rows)) {
          if (cur_idx + (num - vcol_pos) + 1 >= this->list->num_items) {
            frow_idx = 0;
            vrow_pos = first_row;
            cur_idx = 0;
            vcol_pos = 1;
            continue;
          }

          cur_idx += num;
          while (cur_idx >= this->list->num_items) {cur_idx--; vcol_pos--;}
          frow_idx++;
          continue;
        }

        cur_idx += num;
        while (cur_idx >= this->list->num_items) {cur_idx--; vcol_pos--;}
        vrow_pos++;
        continue;

      case PAGE_UP_KEY: {
          int i = rend_rows;
          while (i--) {
            if (frow_idx > 0) {
              frow_idx--;
              cur_idx -= num;
            } else break;
          }
        }

        continue;

      case PAGE_DOWN_KEY: {
          int i = rend_rows;
          while (i--) {
            if (frow_idx < num_rows - rend_rows) {
              frow_idx++;
              cur_idx += num;
            } else break;
          }
        }

        continue;

      default:
insert_char:
        {
          string_t *p = Vstring.join (rl->line, "");
          if (rl->line->tail->data->bytes[0] is ' ')
            String.clear_at (p, p->num_bytes - 1);

          ifnot (Cstring.eq (this->pat, p->bytes)) {
            this->patlen = p->num_bytes;
            Cstring.cp (this->pat, MAXLEN_PATTERN, p->bytes, p->num_bytes);
          }

          String.release (p);
        }

        this->process_list (this);

        if (this->state & MENU_QUIT) goto theend;

        cur_idx = 0;  // reset, as cur_idx can be out of bounds

        if (this->list->num_items is 1)
          if (this->return_if_one_item) {
            this->c = '\r';
            goto handle_char;
          }

        /* this it can be change in the callback, and is intented for backspace */
        this->return_if_one_item = orig_beh;

        if (this->state & MENU_REINIT_LIST) goto init_list;

        continue;
    }
  }

theend:
  vui_menu_clear (this);
  Readline.release (rl);
  return match;
}

public vui_T __init_vui__ (void) {
  __INIT__ (video);
  __INIT__ (string);
  __INIT__ (vstring);
  __INIT__ (cstring);
  __INIT__ (readline);
  __INIT__ (io);

  vui_prop *prop = Alloc (sizeof (vui_prop));

  vui_T this = (vui_T) {
    .self = (vui_self) {
      .menu = (vui_menu_self) {
        .new = vui_menu_new,
        .clear = vui_menu_clear,
        .create = vui_menu_create,
        .release = vui_menu_release,
        .release_list = vui_menu_release_list,
      }
    },
    .prop = prop
  };

  $my(num_rows) = 24;
  $my(num_cols) = 78;

  return this;
}

public void __deinit_vui__ (vui_T *this) {
  if (NULL is this) return;
  if (NULL is this->prop) return;
  free (this->prop);
}
