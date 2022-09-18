typedef struct menu_t menu_t;

typedef int (*MenuProcessList_cb) (menu_t *);

#define MENU_NUM_USER_DATA 4
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
  int state;
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
  void *user_data_third;
  void *user_data_fourth;
} menu_opts;

#define MenuOpts(...) (menu_opts) {         \
  .fd = STDOUT_FILENO,                      \
  .state = (MENU_INIT|READLINE_IS_VISIBLE), \
  .first_row = 1,                           \
  .last_row  = 24,                          \
  .prompt_row = 23,                         \
  .first_col = 1,                           \
  .num_cols = 78,                           \
  .space_selects = 1,                       \
  .return_if_one_item = 1,                  \
  .clear_and_continue_on_backspace = 0,     \
  .next_key = '\t',                         \
  .pat = NULL,                              \
  .patlen = 0,                              \
  .video = NULL,                            \
  .term = NULL,                             \
  .process_list_cb = NULL,                  \
  .getch_cb = Input.getkey,                 \
  .user_data_first = NULL,                  \
  .user_data_second = NULL,                 \
  .user_data_third = NULL,                  \
  .user_data_fourth = NULL,                 \
  __VA_ARGS__                               \
}

static void menu_release_list (menu_t *this) {
  if (this->state & MENU_LIST_IS_ALLOCATED) {
    Vstring.release (this->list);
    this->state &= ~MENU_LIST_IS_ALLOCATED;
  }
}

static void menu_release (menu_t *this) {
  menu_release_list (this);
  String.release (this->header);
  free (this);
}

static menu_t *menu_new (menu_opts opts) {
  menu_t *this = Alloc (sizeof (menu_t));
  this->fd = opts.fd;
  this->last_row = opts.last_row;
  this->first_row = (opts.first_row is 1 ? 2 : opts.first_row);
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
  this->state |= opts.state;
  this->user_data[0] = opts.user_data_first;
  this->user_data[1] = opts.user_data_second;
  this->user_data[2] = opts.user_data_third;
  this->user_data[3] = opts.user_data_fourth;

  this->orig_first_row = opts.first_row;
  this->num_rows = this->last_row - this->first_row + 1;
  this->orig_num_rows = this->num_rows;
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

static void menu_clear (menu_t *this) {
  if (this->header->num_bytes)
    video_resume_row_at (this->video, this->first_row - 1, 1, this->first_col);

  for (int i = 0; i < this->num_rows; i++)
    video_resume_row_at (this->video, this->first_row + i, 1, this->first_col);
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
        menu_clear (menu);
       (*rl)->state |= READLINE_CONTINUE;
       return READLINE_CONTINUE;
      }
      break;
  }

  (*rl)->state |= READLINE_BREAK;
  return READLINE_BREAK;
}

static char *menu_create (menu_t *this) {
  readline_t *rl = readline_new (this->user_data[0], this->term, this->getch,
     this->prompt_row, 1, this->num_cols, this->video);
  rl->at_beg = readline_menu_at_beg;
  rl->at_end = readline_menu_at_end;
  rl->user_data[READLINE_MENU_USER_DATA_IDX - 1] = this->user_data[1];
  rl->user_data[READLINE_MENU_USER_DATA_IDX] = this;
  rl->state |= READLINE_IS_VISIBLE|READLINE_CURSOR_HIDE;
  rl->prompt_char = 0;

  if (0 is (this->state & READLINE_IS_VISIBLE))
    rl->state &= ~READLINE_IS_VISIBLE;

  readline_insert_with_len (rl, this->pat, this->patlen);

init_list:;
  if (this->state & MENU_REINIT_LIST) {
    this->state &= ~MENU_REINIT_LIST;
    menu_clear (this);
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

  if (NULL is this->list or 0 is this->list->num_items) goto theend;

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

    int ridx, iidx = 0; int start_row = 0; int num_items = 0;

    for (ridx = 0; ridx < rend_rows; ridx++) {
      start_row = first_row + ridx;
      String.append_with_fmt (render, TERM_GOTO_PTR_POS_FMT, start_row, first_col);

      for (iidx = 0; iidx < num and iidx + (ridx  * num) + (frow_idx * num) < this->list->num_items; iidx++) {
        num_items++;
        int len = ((int) it->data->num_bytes > maxlen ? maxlen : (int) it->data->num_bytes);
        char item[len + 1];
        Cstring.cp (item, len + 1, it->data->bytes, len);

        int color = (iidx + (ridx * num) + (frow_idx * num) is cur_idx)
           ? COLOR_MENU_SEL : COLOR_MENU_BG;
        String.append_with_fmt (render, fmt, color, item, TERM_COLOR_RESET);
        it = it->next;
      }

      mod = num_items % num;

      if (mod)
        for (int i = mod; i < num; i++)
          for (int j = 0; j < maxlen; j++)
            String.append_byte (render, ' ');
   }

  //    String.append_with (render, TERM_CURSOR_SHOW);

    IO.fd.write (this->fd, render->bytes, render->num_bytes);

    String.release (render);

    this->c = readline_edit (rl)->c;
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

      // fall through
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

      // fall through
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
  menu_clear (this);
  readline_release (rl);
  return match;
}
