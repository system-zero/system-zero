#define REQUIRE_STDIO

#define REQUIRE_STRING_TYPE    DECLARE
#define REQUIRE_VSTRING_TYPE   DECLARE
#define REQUIRE_CSTRING_TYPE   DECLARE
#define REQUIRE_USTRING_TYPE   DECLARE
#define REQUIRE_IO_TYPE        DECLARE
#define REQUIRE_VIDEO_TYPE     DONOT_DECLARE
#define REQUIRE_TERM_MACROS
#define REQUIRE_LIST_MACROS

#include <z/cenv.h>

static void video_alloc_list (video_t *this) {
  this->tmp_list = Vstring.new ();
  for (int i = 0; i < this->num_rows; i++) {
    vstring_t *vstr = Vstring.new_item ();
    vstr->data = String.new (this->num_cols);
    DListAppendCurrent (this->tmp_list, vstr);
  }
}

static video_t *video_new (int fd, int rows, int cols, int first_row, int first_col) {
  video_t *this = Alloc (sizeof (video_t));
  loop (rows) {
    vstring_t *row = Vstring.new_item ();
    row->data = String.new_with_len (" ", 1);
    DListAppendCurrent (this, row);
  }

  this->fd = fd;
  this->first_row = first_row;
  this->first_col = first_col;
  this->num_rows = rows;
  this->num_cols = cols;
  this->last_row = first_row + rows - 1;
  this->row_pos = this->first_row;
  this->col_pos = 1;
  this->render = String.new (cols);
  this->tmp_render = String.new (cols);
  this->rows = Alloc (sizeof (int) * this->num_rows);
  video_alloc_list (this);
  return this;
}

static void video_release (video_t *this) {
  if (this is NULL) return;

  Vstring.release (this->tmp_list);
  vstring_t *it = this->head;

  while (it isnot NULL) {
    vstring_t *next = it->next;
    String.release (it->data);
    free (it);
    it = next;
  }

  String.release (this->render);
  String.release (this->tmp_render);
  free (this->rows);
  free (this);
  this = NULL;
}

static void video_flush (video_t *this, string_t *render) {
  IO.fd.write (this->fd, render->bytes, render->num_bytes);
}

static void video_render_set_from_to (video_t *this, int frow, int lrow) {
  int fidx = frow - 1; int lidx = lrow - 1;

  String.append_with (this->render, TERM_CURSOR_HIDE);
  while (fidx <= lidx) {
    DListSetCurrent (this, fidx++);
    String.append_with_fmt (this->render, TERM_GOTO_PTR_POS_FMT "%s%s%s",
        this->cur_idx + 1, this->first_col, TERM_LINE_CLR_EOL,
        this->current->data->bytes, TERM_NEXT_BOL);
  }

  String.append_with (this->render, TERM_CURSOR_SHOW);
}

static void render_append_row_at (video_t *this, vstring_t *row, int at) {
  String.append_with_fmt (this->tmp_render,
      "%s" TERM_GOTO_PTR_POS_FMT "%s%s%s" TERM_GOTO_PTR_POS_FMT,
      TERM_CURSOR_HIDE, at, this->first_col, TERM_LINE_CLR_EOL,
      row->data->bytes, TERM_CURSOR_SHOW, this->row_pos, this->col_pos);
}

static void video_draw_rows_from_to (video_t *this, int first, int last) {
  if (first > last or first <= 0 or last > this->num_rows) return;

  int fidx = first - 1;
  int lidx = last - 1;

  DListSetCurrent(this, fidx);
  vstring_t *cur = this->current;

  String.clear (this->tmp_render);

  for (int i = fidx; i <= lidx; i++) {
    render_append_row_at (this, cur, i + 1);
    cur = cur->next;
  }

  video_flush (this, this->tmp_render);
}

static void video_draw_row_at (video_t *this, int at) {
  int idx = at - 1;

  if (DListSetCurrent(this, idx) is EINDEX) return;

  String.clear (this->tmp_render);
  vstring_t *row = this->current;
  render_append_row_at (this, row, at);
  video_flush (this, this->tmp_render);
}

static void video_draw_all (video_t *this) {
  String.clear (this->tmp_render);
  String.append_with_fmt (this->tmp_render,
      "%s%s" TERM_SCROLL_REGION_FMT "\033[0m\033[1m",
   TERM_CURSOR_HIDE, TERM_FIRST_LEFT_CORNER, 0, this->num_rows);

  int num_times = this->last_row;
  vstring_t *row = this->head;

  loop (num_times - 1) {
    String.append_with_fmt (this->tmp_render, "%s%s%s", TERM_LINE_CLR_EOL,
        row->data->bytes, TERM_NEXT_BOL);
    row = row->next;
  }

  String.append_with_fmt (this->tmp_render, "%s" TERM_GOTO_PTR_POS_FMT,
     TERM_CURSOR_SHOW, this->row_pos, this->col_pos);

  video_flush (this, this->tmp_render);
}

static void video_set_row_with (video_t *this, int idx, char *bytes) {
  if (DListSetCurrent (this, idx) is EINDEX) return;
  vstring_t *row = this->current;
  String.replace_with (row->data, bytes);
}

static void video_row_hl_at (video_t *this, int idx, int color,
                                               int fidx, int lidx) {
  if (DListSetCurrent (this, idx) is EINDEX) return;
  vstring_t *row = this->current;
  if (fidx >= (int) row->data->num_bytes) return;
  if (lidx >= (int) row->data->num_bytes) lidx = row->data->num_bytes - 1;

  String.insert_at_with_len (row->data, lidx, TERM_COLOR_RESET, TERM_COLOR_RESET_LEN);
  String.insert_at_with_len (row->data, fidx, TERM_MAKE_COLOR (color), TERM_SET_COLOR_FMT_LEN);
  String.replace_with_fmt (this->tmp_render, TERM_GOTO_PTR_POS_FMT, idx + 1, 1);
  String.append_with_len (this->tmp_render, row->data->bytes, row->data->num_bytes);

  video_flush (this, this->tmp_render);
}

static void video_resume_painted_rows (video_t *this) {
  int row = 0, i = 0;
  while (0 isnot (row = this->rows[i++])) video_draw_row_at (this, row);
  this->rows[0] = 0;
}

static video_t *video_paint_rows_with (video_t *this, int row, int f_col, int l_col, char *bytes) {
  int last_row = this->last_row - 3;
  int first_row = row < 1 or row > last_row ? last_row : row;
  int last_col = l_col < 0 or l_col > this->num_cols ? this->num_cols : l_col;
  int first_col = f_col < 0 or f_col > this->num_cols ? 1 : f_col;
  char *f_p = 0;
  f_p = bytes;
  char *l_p = bytes;

  Vstring_t *Vstr = this->tmp_list;
  vstring_t *vstr = Vstr->head;

  int num_items = 0;
  while (l_p) {
    String.clear (vstr->data);

    l_p = Cstring.byte.in_str (l_p, '\n');

    if (NULL is l_p) {
      String.append_with (vstr->data, f_p);
    } else {
      char line[l_p - f_p];
      int i = 0;
      while (f_p < l_p) line[i++] = *f_p++;
      f_p++; l_p++;
      line[i] = '\0';
      String.append_with (vstr->data, line);
    }

    if (++num_items is this->num_rows) break;
    vstr = vstr->next;
  }

  int num_rows = num_items;

  int i = 0; while (i < num_rows - 1 and first_row > 2) {i++; first_row--;};
  num_rows = i + 1;
  int num_chars = last_col - first_col + 1;

  String.clear (this->tmp_render);
  String.append_with_fmt (this->tmp_render, "%s" TERM_SET_COLOR_FMT, TERM_CURSOR_HIDE, COLOR_BOX);

  vstring_t *it = Vstr->head;
  i = 0;
  while (i < num_rows) {
    this->rows[i] = (i + first_row);
    String.append_with_fmt (this->tmp_render, TERM_GOTO_PTR_POS_FMT, first_row + i++, first_col);
    int num = 0; int idx = 0;
    while (num++ < num_chars and idx < (int) it->data->num_bytes) {
      int clen = Ustring.charlen ((uchar) it->data->bytes[idx]);
      for (int li = 0; li < clen; li++)
        String.append_byte (this->tmp_render, it->data->bytes[idx + li]);
      idx += clen;
    }

    while (num++ <= num_chars) String.append_byte (this->tmp_render, ' ');

    it = it->next;
  }

  this->rows[num_rows] = 0;

  String.append_with_fmt (this->tmp_render, "%s%s", TERM_COLOR_RESET, TERM_CURSOR_SHOW);
  video_flush (this, this->tmp_render);
  return this;
}
public video_T __init_video__ (void) {
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (vstring);
  __INIT__ (ustring);
  __INIT__ (io);

  return (video_T) {
    .self = (video_self) {
      .new =  video_new,
      .flush = video_flush,
      .release = video_release,
      .row_hl_at = video_row_hl_at,
      .paint_rows_with = video_paint_rows_with,
      .render_set_from_to = video_render_set_from_to,
      .resume_painted_rows = video_resume_painted_rows,
      .set = (video_set_self) {
        .row_with = video_set_row_with
      },
      .draw = (video_draw_self) {
        .all = video_draw_all,
        .row_at = video_draw_row_at,
        .rows_from_to = video_draw_rows_from_to
      }
    }
  };
}
