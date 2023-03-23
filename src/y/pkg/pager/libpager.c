#define REQUIRE_STDIO
#define REQUIRE_TERMIOS
#define REQUIRE_UNISTD
#define REQUIRE_SYS_STAT

#define REQUIRE_IO_TYPE       DECLARE
#define REQUIRE_DIR_TYPE      DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_USTRING_TYPE  DECLARE
#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE  DECLARE
#define REQUIRE_SYS_TYPE      DECLARE
#define REQUIRE_PATH_TYPE     DECLARE
#define REQUIRE_LIST_MACROS
#define REQUIRE_KEYS_MACROS
#define REQUIRE_TERM_MACROS

#include <z/cenv.h>
#include <z/pager.h>

#define DONE             0
#define DONE_NEEDS_DRAW  1
#define NOTHING_TODO     2

static void video_draw_row_at (video_t *, int, int);
static void video_resume_row_at (video_t *, int, int, int);

#include "readline.c"

#define GOTO(r,c) Term.cursor.set_pos (My->term, r, c)
#define ONE_PAGE(__buf__)  ((__buf__->last_row - __buf__->first_row) + 1)
#define CUR_COL My->cur_line->data->current

typedef pager_t Me;

static video_t *video_new (int fd, int nrows, int ncols) {
  video_t *video = Alloc (sizeof (video_t));
  video->fd = fd;
  video->num_rows = nrows;
  video->render = String.new (512);
  video->rows = Alloc (nrows * sizeof (string));
  for (int i = 0; i < nrows; i++)
    video->rows[i] = String.new (ncols);
  return video;
}

static void video_release (video_t *video) {
  String.release (video->render);
  for (int i = 0; i < video->num_rows; i++)
    String.release (video->rows[i]);
  free (video->rows);
  free (video);
}

static void video_draw_row_at (video_t *video, int row, int first_col) {
  String.clear (video->render);
  String.append_with_fmt (video->render,
    "%s" TERM_GOTO_PTR_POS_FMT "%s%s%s" TERM_GOTO_PTR_POS_FMT,
      TERM_CURSOR_HIDE, row, first_col, TERM_LINE_CLR_EOL,
      video->rows[row - 1]->bytes, TERM_CURSOR_SHOW, video->row_pos, video->col_pos);

  IO.fd.write (video->fd, video->render->bytes, video->render->num_bytes);
}

static void video_resume_row_at (video_t *video, int row, int first_col, int app_first_col) {
  if (first_col is app_first_col) {
    video_draw_row_at (video, row, first_col);
    return;
  }

  String.clear (video->render);
  String.append_with_fmt (video->render, "%s" TERM_GOTO_PTR_POS_FMT "%s",
   TERM_CURSOR_HIDE, row, 1, TERM_LINE_CLR_EOL);

  for (int i = 0; i < app_first_col; i++)
    String.append_byte (video->render, ' ');

  String.append_with_fmt (video->render,
    "%s%s" TERM_GOTO_PTR_POS_FMT, video->rows[row - 1]->bytes, TERM_CURSOR_SHOW,
     video->row_pos, video->col_pos);

  IO.fd.write (video->fd, video->render->bytes, video->render->num_bytes);
}

static void buf_release_input (Buf *My) {
  ifnot (My->input_should_be_freed) return;
  if (My->input is NULL) return;

  for (size_t i = 0; i < My->array_len; i++)
    String.release (My->input[i]);

  free (My->input);
  My->input = NULL;
}

static void buf_release (Buf *My) {
  buf_release_input (My);

  for (size_t i = 0; i < My->array_len; i++) {
    Ustring.release (My->lines[i]->data);
    free (My->lines[i]);
  }

  free (My->lines);
  My->lines = NULL;

  String.release (My->empty_line);
  String.release (My->tmp_buf);

  free (My->ftype);
  free (My);
}

void pager_release (Me *My) {
  video_release (My->video);

  ifnot (NULL is My->readline)
    readline_release (My->readline);

  if (My->term_should_be_freed) {
    Term.reset (My->term);
    Term.release (&My->term);
  }

  for (int i = 0; i < My->num_buf; i++)
    buf_release (My->buffers[i]);

  free (My->buffers);
  free (My);
}

static void set_video_row (Buf *My, size_t vidx, size_t idx) {
  String.clear (My->tmp_buf);

  if (idx >= My->array_len) {
    String.replace_with (My->video->rows[vidx], My->empty_line->bytes);
    return;
  }

  line_t *line = My->lines[idx];
  ustring_t *it = line->data->head;

  int cidx = 0;
  while (it) {
    if (cidx is line->first_col_idx) break;
    cidx += it->len;
    it = it->next;
  }

  int num = 0;
  while (it and num < My->num_cols) {
    num += it->width;
    if (it->code is '\t') {
      for (int j = 0; j < it->width; j++)
        String.append_byte (My->tmp_buf, ' ');
    } else
      String.append_with_len (My->tmp_buf, it->buf, it->len);

    it = it->next;
  }

  String.replace_with (My->video->rows[vidx], My->tmp_buf->bytes);
}

static void buf_set_render_row (Buf *My, int row_pos) {
  String.append_with_fmt (My->video->render,
    "%s" TERM_GOTO_PTR_POS_FMT "%s%s%s" TERM_GOTO_PTR_POS_FMT,
      TERM_CURSOR_HIDE, row_pos, My->first_col, TERM_LINE_CLR_EOL,
      My->video->rows[row_pos-1]->bytes, TERM_CURSOR_SHOW, My->row_pos, My->col_pos);
}

static int buf_set_statusline (Buf *My) {
  ifnot (My->has_statusline) return NOTHING_TODO;

  int cur_col_idx = (My->cur_line->cur_col_idx is -1 ? 0 : My->cur_line->cur_col_idx);
  int chr = (CUR_COL is NULL ? 0 : CUR_COL->code);

  string *s = My->video->rows[My->last_row];
  String.replace_with_fmt (s,
    "(line: %zd/%zd idx: %d len: %d chr: %d)",
    My->cur_row_idx + 1, My->array_len,
    cur_col_idx, My->cur_line->num_bytes, chr);

  size_t num = s->num_bytes;
  for (size_t i = num; i < (size_t) My->num_cols; i++)
    String.prepend_byte (s, ' ');

  buf_set_render_row (My, My->last_row + 1);
  return DONE;
}

static void buf_draw_statusline_and_set_pos (Buf *My) {
  String.clear (My->video->render);
  if (DONE is buf_set_statusline (My)) {
    IO.fd.write (My->video->fd, My->video->render->bytes, My->video->render->num_bytes);
    return;
  }

  GOTO(My->row_pos, My->col_pos);
}

static void buf_set_row_and_draw (Buf *My) {
  set_video_row (My, My->row_pos - 1, My->cur_row_idx);
  String.clear (My->video->render);
  buf_set_render_row (My, My->row_pos);
  buf_set_statusline (My);
  IO.fd.write (My->video->fd, My->video->render->bytes, My->video->render->num_bytes);
}

static void buf_set_rows (Buf *My) {
  size_t vidx = My->first_row - 1;
  size_t fidx = My->video_first_row_idx;
  size_t lidx = My->video_first_row_idx + My->num_rows - 1;
  for (; fidx <= lidx; fidx++)
    set_video_row (My, vidx++, fidx);
}

static void buf_set_rows_and_draw (Buf *My) {
  buf_set_rows (My);
  String.clear (My->video->render);
  for (int i = My->first_row; i <= My->last_row; i++)
    buf_set_render_row (My, i);

  buf_set_statusline (My);

  IO.fd.write (My->video->fd, My->video->render->bytes, My->video->render->num_bytes);
}

static ustring_t *get_line_num (line_t *line, int idx, int *num, ustring_t **fcol_it) {
  ustring_t *it = line->data->head;
  *num = 0;

  if (NULL is it) return NULL;

  int cidx = 0;
  while (it) {
   if (cidx is line->first_col_idx) {
     *fcol_it = it;
     break;
   }

   cidx += it->len;
   it = it->next;
  }

  idx -= cidx;
  int i = 0;
  while (it) {
    (*num) += it->width;
    if (i is idx or (i + (it->len - 1) is idx))
      return it;

    i += it->len;
    it = it->next;
  }

  (*num) += line->data->tail->width;
  if (i is idx or (i + (line->data->tail->len - 1) is idx))
    return line->data->tail;

  return NULL;
}

static void buf_adjust_col (Buf *My) {
  line_t *prevline = My->prev_line;
  line_t *curline  = My->cur_line;
  ustring_t *it = curline->data->head;

  if (NULL is it) {
    curline->cur_col_idx = -1;
    My->col_pos = My->first_col;
    CUR_COL = NULL;
    return;
  }

  it = prevline->data->head;
  int cidx = 0;
  while (it) {
    if (cidx is prevline->first_col_idx) break;
    cidx += it->len;
    it = it->next;
  }

  int n = 0;
  while (it) {
    n++;
    if (cidx is prevline->cur_col_idx) break;
    cidx += it->len;
    it = it->next;
  }

  int pos = My->col_pos - My->first_col + 1;
  int num = 0;
  int nn = 0;

  it = curline->data->head;
  cidx = 0;
  while (it) {
    if (cidx is curline->first_col_idx) break;
    cidx += it->len;
    it = it->next;
  }

  int cur_col_idx = cidx;

  while (it) {
    nn++;
    num += it->width;

    if (num is pos or nn is n or it is curline->data->tail) {
      curline->cur_col_idx = cur_col_idx;
      My->col_pos = (num - it->width) + (My->first_col);
      CUR_COL = it;
      return;
    }

    cur_col_idx += it->len;
    it = it->next;
  }

  curline->cur_col_idx = cur_col_idx;
  My->col_pos = num + My->first_col - 1;
  CUR_COL = it;
}

static int buf_home (Buf *My) {
  ifnot (My->video_first_row_idx + My->cur_row_idx)
    if (My->row_pos is My->first_row)
      return NOTHING_TODO;

  My->row_pos = My->first_row;
  My->video_first_row_idx = 0;
  My->cur_row_idx = 0;
  My->prev_line = My->cur_line;
  My->cur_line  = My->lines[My->cur_row_idx];
  buf_adjust_col (My);
  return DONE_NEEDS_DRAW;
}

static int buf_eof (Buf *My) {
  if ((size_t) My->video_first_row_idx is My->array_len - 1 or
      (size_t) My->cur_row_idx is My->array_len - 1)
    return NOTHING_TODO;

  My->cur_row_idx = My->array_len - 1;

  My->prev_line = My->cur_line;
  My->cur_line  = My->lines[My->cur_row_idx];

  if ((size_t) ONE_PAGE(My) > My->array_len) {
    My->row_pos = My->first_row + My->array_len - 1;
    buf_adjust_col (My);
    return DONE;
  }

  My->video_first_row_idx = (My->array_len - ONE_PAGE(My));
  My->row_pos = My->last_row;
  buf_adjust_col (My);
  return DONE_NEEDS_DRAW;
}

static int buf_page_down (Buf *My) {
  if ((size_t) (My->video_first_row_idx + My->num_rows) >= My->array_len)
    return NOTHING_TODO;

  My->video_first_row_idx += My->num_rows;
  My->cur_row_idx         += My->num_rows;

  while ((size_t) My->cur_row_idx >= My->array_len)
    My->cur_row_idx--;

  My->prev_line = My->cur_line;
  My->cur_line  = My->lines[My->cur_row_idx];

  int n = (My->array_len - (size_t) My->video_first_row_idx) + 1;
  while (My->row_pos > n) My->row_pos--;

  buf_adjust_col (My);

  return DONE_NEEDS_DRAW;
}

static int buf_page_up (Buf *My) {
  ifnot (My->video_first_row_idx) return NOTHING_TODO;

  int n = My->num_rows;
  while (n) {
    My->video_first_row_idx--;
    n--;
    ifnot (My->video_first_row_idx) break;
  }

  My->cur_row_idx -= (My->num_rows - n);
  My->prev_line    = My->cur_line;
  My->cur_line     = My->lines[My->cur_row_idx];

  buf_adjust_col (My);

  return DONE_NEEDS_DRAW;
}

static int buf_up (Buf *My) {
  if (My->row_pos > My->first_row) {
    My->row_pos--;
    My->cur_row_idx--;
    My->prev_line = My->cur_line;
    My->cur_line  = My->lines[My->cur_row_idx];
    buf_adjust_col (My);
    return DONE;
  }

  if (My->row_pos is My->first_row and My->video_first_row_idx > 0) {
    My->cur_row_idx--;
    My->prev_line = My->cur_line;
    My->cur_line  = My->lines[My->cur_row_idx];
    My->video_first_row_idx--;
    buf_adjust_col (My);
    return DONE_NEEDS_DRAW;
  }

  return NOTHING_TODO;
}

static int buf_down (Buf *My) {
  if (My->row_pos < My->last_row and (size_t) My->cur_row_idx < My->array_len - 1) {
    My->row_pos++;
    My->cur_row_idx++;
    My->prev_line = My->cur_line;
    My->cur_line  = My->lines[My->cur_row_idx];
    buf_adjust_col (My);
    return DONE;
  }

  if (My->row_pos is My->last_row and
      (size_t) My->cur_row_idx + 1 < My->array_len) {
    My->cur_row_idx++;
    My->prev_line = My->cur_line;
    My->cur_line  = My->lines[My->cur_row_idx];
    My->video_first_row_idx++;
    buf_adjust_col (My);
    return DONE_NEEDS_DRAW;
  }

  return NOTHING_TODO;
}

static int buf_left  (Buf *My) {
  int num = 0;
  line_t *line = My->cur_line;
  ustring_t *fcol_it;
  ustring_t *it = get_line_num (line, line->cur_col_idx, &num, &fcol_it);

  if (it is NULL or it is line->data->head)
    return NOTHING_TODO;

  int n = num - 1 + My->first_col;

  if (n > My->first_col) {
    My->col_pos = num - it->width;
    it = it->prev;
    line->cur_col_idx -= it->len;
    My->col_pos -= it->width;
    My->col_pos += My->first_col;
    CUR_COL = it;
    return DONE;
  }

  ifnot (line->first_col_idx)
    return NOTHING_TODO;

  it = it->prev;
  line->first_col_idx -= it->len;
  line->cur_col_idx   -= it->len;
  CUR_COL = it;
  return DONE_NEEDS_DRAW;
}

static int buf_eol (Buf *My) {
  line_t *line = My->cur_line;
  ustring_t *it = line->data->tail;
  if (it is NULL) return NOTHING_TODO;

  CUR_COL = it;
  int curidx = line->num_bytes - it->len;
  int lastidx = curidx;
  int num = it->width;
  it = it->prev;

  while (it) {
    if (num + it->width > My->num_cols) break;

    num += it->width;
    curidx -= it->len;
    if (num is My->num_cols) break;
    it = it->prev;
  }

  line->first_col_idx = curidx;
  line->cur_col_idx = lastidx;
  My->col_pos = (num - 1) + My->first_col;
  return DONE_NEEDS_DRAW;
}

static int buf_bol (Buf *My) {
  line_t *line = My->cur_line;
  ustring_t *it = line->data->head;
  if (it is NULL) return NOTHING_TODO;

  ifnot (line->cur_col_idx) return NOTHING_TODO;

  CUR_COL = it;
  line->first_col_idx = line->cur_col_idx = 0;
  My->col_pos = My->first_col;
  return DONE_NEEDS_DRAW;
}

static int buf_right (Buf *My) {
  int num = 0;
  line_t *line = My->cur_line;
  ustring_t *fcol_it;
  ustring_t *it = get_line_num (line, line->cur_col_idx, &num, &fcol_it);

  CUR_COL = it;

  if (NULL is it or it is line->data->tail)
    return NOTHING_TODO;

  int n = num - 1 + My->first_col;

  if (n < My->last_col) {
    line->cur_col_idx += it->len;
    My->col_pos = num + My->first_col;
    CUR_COL = CUR_COL->next;
    return  DONE;
  }

  if (n is My->last_col or n - it->width is My->last_col) {
    line->first_col_idx += fcol_it->len;
    line->cur_col_idx += it->len;
    CUR_COL = it->next;
    return DONE_NEEDS_DRAW;
  }

  return NOTHING_TODO;
}


/*
static void readline_realloc_command_arg (readline_com_t *rlcom, int num) {
  int orig_num = rlcom->num_args;
  rlcom->num_args = num;
  rlcom->args = Realloc (rlcom->args, sizeof (char *) * (rlcom->num_args + 1));
  for (int i = orig_num; i <= num; i++)
    rlcom->args[i] = NULL;
}

static void ed_add_command_arg (readline_com_t *rlcom, int flags) {
#define ADD_ARG(arg, len, idx) ({                             \
  if (idx is rlcom->num_args)                                 \
    readline_realloc_command_arg (rlcom, idx);                \
  rlcom->args[idx] = Cstring.dup (arg, len);                  \
})

  int i = 0;
  if (flags & READLINE_ARG_INTERACTIVE) ADD_ARG ("--interactive", 13, i++);
  if (flags & READLINE_ARG_BUFNAME) ADD_ARG ("--bufname=", 10, i++);
  if (flags & READLINE_ARG_RANGE) ADD_ARG ("--range=", 8, i++);
  if (flags & READLINE_ARG_GLOBAL) ADD_ARG ("--global", 8, i++);
  if (flags & READLINE_ARG_APPEND) ADD_ARG ("--append", 8, i++);
  if (flags & READLINE_ARG_FILENAME) ADD_ARG ("--fname=", 8, i++);
  if (flags & READLINE_ARG_SUB) ADD_ARG ("--sub=", 6, i++);
  if (flags & READLINE_ARG_PATTERN) ADD_ARG ("--pat=", 6, i++);
  if (flags & READLINE_ARG_VERBOSE) ADD_ARG ("--verbose", 9, i++);
  if (flags & READLINE_ARG_RECURSIVE) ADD_ARG ("--recursive", 11, i++);
}
*/

static void readline_append_command (readline_t  *rl, const char *com, const char **args, int num_args) {
  rl->num_commands++;

  ifnot (rl->num_commands - 1)
    rl->commands = Alloc (sizeof (readline_com_t));
  else
    rl->commands = Realloc (rl->commands, sizeof (readline_com_t) * rl->num_commands);

  rl->commands[rl->num_commands - 1] = Alloc (sizeof (readline_com_t));

  rl->commands[rl->num_commands - 1]->com = Cstring.dup (com, bytelen (com));

  rl->commands[rl->num_commands - 1]->num_args = num_args;

  ifnot (num_args) {
    rl->commands[rl->num_commands - 1]->args = NULL;
    return;
  }

  rl->commands[rl->num_commands - 1]->args = Alloc (sizeof (char *) * (num_args + 1));

  for (int i = 0; i < num_args; i++)
    rl->commands[rl->num_commands - 1]->args[i] = Cstring.dup (args[i], bytelen (args[i]));

  rl->commands[rl->num_commands - 1]->args[num_args] = NULL;

}

static readline_t *pager_new_readline (Me *My, int has_command_line) {
  ifnot (has_command_line) return NULL;
  readline_t *rl = readline_new (My, My->term, IO.input.getkey, My->term->num_rows - 1, 1, My->term->num_cols, My->video);
  const char *w_args[] = {"--no=", "--na"};
  readline_append_command (rl, "write", w_args, 2);
  const char *t_args[] = {"--do=", "--du"};
  readline_append_command (rl, "test", t_args, 2);
  return rl;
}

static int editline (Me *My, Buf *buf) {
  // just test that it does works and it is
  My->video->row_pos = buf->row_pos;
  My->video->col_pos = buf->col_pos;
  My->video->first_col = buf->first_col;

  readline_t *rl = My->readline;
  rl = readline_edit (rl);

  if (rl->line->head is NULL or rl->line->head->data->bytes[0] is ' ')
    return OK;

  readline_parse (rl);

  //string_t *com = readline_get_command (rl);
  //int is_du = readline_arg_exists (rl, "du");
  //Vstring_t *dirs = readline_get_arg_fnames (rl, -1);
  //string_t *do_s = readline_get_anytype_arg (rl, "do");
  //readline_clear (rl);
  //readline_reset (rl);
  //String.replace_with (buf->video->rows[buf->term->num_rows - 1], Sys.get.battery_info()->bytes);
  video_draw_row_at (buf->video, buf->term->num_rows, 1);
  GOTO(buf->row_pos, buf->col_pos);
  return OK;
}

static int pager_normal_mode (Me *My) {
  Buf *buf = My->cur_buf;

  buf_set_rows_and_draw (buf);

  int c;
  int r;

  for (;;)  {
    c = IO.input.getkey (0);

    r = My->on_input (My, &c);
    switch (r) {
      case PAGER_EXIT: goto theend;
      case PAGER_CONTINUE: continue;
    }

    switch (c) {
      case 'q':
      case 'Q':
        goto theend;

      case ':':
        editline (My, buf);
        continue;

      case 'g':
        c = IO.input.getkey (0);
        ifnot (c is 'g') continue;
        // fallthrough

      case HOME_KEY:
        r = buf_home (buf);
        if (r is NOTHING_TODO or r is DONE) continue;

        buf_set_rows_and_draw (buf);

        continue;

      case 'G':
      case END_KEY:
        r = buf_eof (buf);
        if (r is NOTHING_TODO)  continue;

        if (r is DONE_NEEDS_DRAW)
          buf_set_rows_and_draw (buf);
        else
          buf_draw_statusline_and_set_pos (buf);

        continue;

      case CTRL('f'):
      case PAGE_DOWN_KEY:
        r = buf_page_down (buf);
        if (r is NOTHING_TODO) continue;

        buf_set_rows_and_draw (buf);

        continue;

      case CTRL('b'):
      case PAGE_UP_KEY:
        r = buf_page_up (buf);
        if (r is NOTHING_TODO) continue;

        buf_set_rows_and_draw (buf);

        continue;

      case 'j':
      case ARROW_DOWN_KEY:
        r = buf_down (buf);
        if (r is NOTHING_TODO) continue;

        if (r is DONE_NEEDS_DRAW)
          buf_set_rows_and_draw (buf);
        else
          buf_draw_statusline_and_set_pos (buf);

        continue;

      case 'k':
      case ARROW_UP_KEY:
        r = buf_up (buf);
        if (r is NOTHING_TODO) continue;

        if (r is DONE_NEEDS_DRAW)
          buf_set_rows_and_draw (buf);
        else
          buf_draw_statusline_and_set_pos (buf);

        continue;

      case 'l':
      case ARROW_RIGHT_KEY:
        r = buf_right (buf);
        if (r is NOTHING_TODO) continue;

        if (r is DONE_NEEDS_DRAW)
          buf_set_row_and_draw (buf);
        else
          buf_draw_statusline_and_set_pos (buf);

        continue;

      case 'h':
      case ARROW_LEFT_KEY:
        r = buf_left (buf);

        if (r is NOTHING_TODO) continue;

        if (r is DONE_NEEDS_DRAW)
          buf_set_row_and_draw (buf);
        else
          buf_draw_statusline_and_set_pos (buf);

        continue;

      case '$':
        r = buf_eol (buf);

        if (r is NOTHING_TODO) continue;

        buf_set_row_and_draw (buf);

        continue;

      case '0':
        r = buf_bol (buf);

        if (r is NOTHING_TODO) continue;

        buf_set_row_and_draw (buf);

        continue;

      default: continue;
    }
  }

theend:
  return OK;
}

int pager_main (Me *My) {
  return pager_normal_mode (My);
}

static int on_input_cb (Me *My, int *c) {
  (void) My;
  (void) c;
  return 0;
}

Me *pager_new (string **lines, size_t array_len, pager_opts opts) {
  __INIT__(string);
  __INIT__(cstring);
  __INIT__(ustring);
  __INIT__(vstring);
  __INIT__(dir);
  __INIT__(io);
  __INIT__(sys);
  __INIT__(path);
  __INIT__(term);

  int first_row = opts.first_row;
  int first_col = opts.first_col;
  int last_row  = opts.last_row;
  int last_col  = opts.last_col;
  int tabwidth  = opts.tabwidth;

  Me *My = Alloc (sizeof (Me));

  (*My) = (Me) {
    .first_row = first_row,
    .last_row  = last_row,
    .first_col = first_col,
    .last_col = 0,
    .term = opts.term,
    .video = NULL,
    .readline = NULL,
    .on_input = on_input_cb
  };

  My->term_should_be_freed = My->term is NULL;

  if (My->term_should_be_freed) {
    My->term = Term.new ();
    Term.set (My->term);
  }

  My->first_row  = (first_row is -1 ? 1 : first_row);
  My->last_row   = (last_row is -1 ? My->term->num_rows : last_row);
  My->last_col   = (last_col is -1 ? My->term->num_cols : last_col);
  My->num_rows   = My->last_row - My->first_row + 1;
  My->num_cols   = My->last_col - My->first_col + 1;
  My->video      = video_new (1, My->term->num_rows, My->term->num_cols);
  My->readline   = pager_new_readline (My, opts.has_command_line);
  My->buffers = Alloc (sizeof (Buf));
  My->buffers[0] = Alloc (sizeof (Buf));
  Buf *buf = My->buffers[0];

  (*buf) = (Buf) {
    .cur_row_idx = 0,
    .col_pos = first_col,
    .first_row = first_row,
    .last_row  = last_row,
    .first_col = first_col,
    .last_col = 0,
    .video_first_row_idx = 0,
    .array_len = array_len,
    .has_statusline = opts.buf_has_statusline,
    .input_should_be_freed = opts.input_should_be_freed,
    .input = lines,
    .cur_line = NULL,
    .prev_line = NULL,
    .tmp_buf = String.new (My->term->num_cols),
    .empty_line = String.new (My->num_cols),
    .ftype = Alloc (sizeof (Ftype)),
    .lines = Alloc (sizeof (line_t) * array_len),
    .term = My->term,
    .video = My->video,
    .parent = My
  };

  buf->row_pos   = My->first_row;
  buf->col_pos   = My->first_col;
  buf->first_row = My->first_row;
  buf->last_row  = My->last_row - buf->has_statusline;
  buf->last_col  = My->last_col;
  buf->num_rows  = My->num_rows - buf->has_statusline;
  buf->num_cols  = My->num_cols;

  buf->ftype->tabwidth = tabwidth;

  for (size_t i = 0; i < (size_t) buf->num_cols; i++)
    String.append_byte (buf->empty_line, ' ');

 for (size_t j = 0; j < buf->array_len; j++)
    buf->lines[j] = Alloc (sizeof (line_t));

  for (size_t i = 0; i < buf->array_len; i++) {
    Ustring_t *u = Ustring.new ();
    Ustring.encode (u, buf->input[i]->bytes,
                       buf->input[i]->num_bytes, 1, buf->ftype->tabwidth, 0);
    buf->lines[i]->data = u;
    buf->lines[i]->first_col_idx = 0;
    buf->lines[i]->cur_col_idx = 0;
    buf->lines[i]->num_bytes = buf->input[i]->num_bytes;
  }

  buf->cur_line = buf->prev_line = buf->lines[0];

  My->buffers[0] = buf;
  My->cur_buf = buf;
  My->num_buf = 1;

  buf_release_input (buf); // we don't need this anymore, free it if it is allowed

  return My;
}
