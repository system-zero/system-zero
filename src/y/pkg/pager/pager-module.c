#define REQUIRE_STDIO
#define REQUIRE_TERMIOS

#define REQUIRE_STD_MODULE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_TERM_TYPE    DECLARE
#define REQUIRE_VIDEO_TYPE   DECLARE
#define REQUIRE_USTRING_TYPE DECLARE
#define REQUIRE_KEYS_MACROS
#define REQUIRE_TERM_MACROS

#include <z/cenv.h>

MODULE(pager);

#define GOTO(r,c) Term.cursor.set_pos (My->term, r, c)

#define ONE_PAGE My->last_row - My->first_row + 1

typedef struct line_t {
  Ustring_t *data;
  int first_col_idx;
  int cur_col_idx;
} line_t;

typedef struct Ftype {
  int tabwidth;
} Ftype;

typedef struct Buf {
  int     cur_row_idx;
  int     row_pos;
  int     col_pos;
  int     num_rows;
  int     num_cols;
  int     first_row;
  int     first_col;
  int     last_row;
  int     last_col;
  int     video_first_row_idx;
  size_t  array_len;

  Ftype   ftype;
  line_t  **lines;
} Buf;
	
typedef struct Me {
  int     cur_row_idx;
  int     row_pos;
  int     col_pos;
  int     num_rows;
  int     num_cols;
  int     first_row;
  int     first_col;
  int     last_row;
  int     last_col;
  int     video_first_row_idx;
  size_t  array_len;

  string  *empty_line;
  string  *buf;
  line_t  **lines;
  term_t  *term;
  video_t *video;

  Buf **buffers;
  int cur_buf;
} Me;

static void adjust_col (Me *My, line_t *prevline) {
  line_t *line = My->lines[My->cur_row_idx];
  ustring_t *it = line->data->head;

  if (NULL is it) {
    line->cur_col_idx = -1;
    My->col_pos = My->first_col;
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
  int cur_col_idx = 0;
  int num = 0;
  int nn = 0;

  it = line->data->head;

  while (it) {
    nn++;
    num += it->width;

    if (num is pos or nn is n or it is line->data->tail) {
      line->cur_col_idx = cur_col_idx;
      My->col_pos = (num - it->width) + (My->first_col);
      return;
    }

    cur_col_idx += it->len;
    it = it->next;
  }

  line->cur_col_idx = cur_col_idx;
  My->col_pos = num + My->first_col - 1;
}

static void set_video_row (Me *My, size_t vidx, size_t idx) {
  String.clear (My->buf);

  if (idx >= My->array_len) {
    Video.set.row_with (My->video, vidx, My->empty_line->bytes);
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
        String.append_byte (My->buf, ' ');
    } else
      String.append_with_len (My->buf, it->buf, it->len);

    it = it->next;
  }

  Video.set.row_with (My->video, vidx, My->buf->bytes);
}

static void set_video_rows (Me *My, size_t vidx, size_t fidx, size_t lidx) {
  for (; fidx <= lidx; fidx++) set_video_row (My, vidx++, fidx);
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
    if (i is idx or (i + (it->len - 1) is idx)) {
      return it;
    }

    i += it->len;
    it = it->next;
  }

  (*num) += line->data->tail->width;
  if (i is idx or (i + (line->data->tail->len - 1) is idx))
    return line->data->tail;

  return NULL;
}

static int pager_normal_mode (Me *My, string **lines) {
  int tabwidth = 8;

  for (size_t i = 0; i < My->array_len; i++)
    My->lines[i] = Alloc (sizeof (line_t));

  for (size_t i = 0; i < My->array_len; i++) {
    Ustring_t *u = Ustring.new ();
    Ustring.encode (u, lines[i]->bytes, lines[i]->num_bytes, 1, tabwidth, 0);
    My->lines[i]->data = u;
    My->lines[i]->first_col_idx = 0;
    My->lines[i]->cur_col_idx = 0;
  }

  set_video_rows (My, 0, 0, My->num_rows - 1);
  Video.draw.rows_from_to (My->video, My->first_row, My->last_row);

  GOTO(My->row_pos, My->col_pos);

  line_t    *line    = NULL;
  ustring_t *it      = NULL;
  ustring_t *fcol_it = NULL;
  int num;
  int n;
  int c;

  for (;;)  {
    c = IO.input.getkey (0);

    line = My->lines[My->cur_row_idx];

    switch (c) {
      case 'q':
      case 'Q':
        goto theend;

      case 'g':
        c = IO.input.getkey (0);
        ifnot (c is 'g') continue;
        // fallthrough

      case HOME_KEY:
        ifnot (My->video_first_row_idx + My->cur_row_idx)
          if (My->row_pos is My->first_row) continue;

        My->row_pos = My->first_row;
        My->video_first_row_idx = 0;
        My->cur_row_idx = 0;
        set_video_rows (My, 0, My->video_first_row_idx, My->video_first_row_idx + My->num_rows);
        Video.draw.rows_from_to (My->video, My->first_row, My->last_row);
        adjust_col (My, line);
        GOTO(My->row_pos, My->col_pos);
        continue;

      case 'G':
      case END_KEY:
        if ((size_t) My->video_first_row_idx is My->array_len - 1) continue;
        if ((size_t) My->cur_row_idx is My->array_len - 1) continue;

        My->cur_row_idx = My->array_len - 1;

        if ((size_t) ONE_PAGE > My->array_len) {
          My->row_pos = My->first_row + My->array_len - 1;
        } else {
          My->video_first_row_idx = My->array_len - ONE_PAGE;
          My->row_pos = My->last_row;
          set_video_rows (My, 0, My->video_first_row_idx, My->video_first_row_idx + My->num_rows);
          Video.draw.rows_from_to (My->video, My->first_row, My->last_row);
        }

        adjust_col (My, line);
        GOTO(My->row_pos, My->col_pos);
        continue;

      case CTRL('f'):
      case PAGE_DOWN_KEY:
        if ((size_t) (My->video_first_row_idx + My->num_rows) > My->array_len)
          continue;

        My->video_first_row_idx += My->num_rows;
        My->cur_row_idx         += My->num_rows;

        while ((size_t) My->cur_row_idx >= My->array_len)
          My->cur_row_idx--;

        n = My->array_len - (size_t) My->video_first_row_idx;
        while (My->row_pos > n) My->row_pos--;

        set_video_rows (My, 0, My->video_first_row_idx, My->video_first_row_idx + My->num_rows);
        Video.draw.rows_from_to (My->video, My->first_row, My->last_row);
        adjust_col (My, line);
        GOTO(My->row_pos, My->col_pos);
        continue;

      case CTRL('b'):
      case PAGE_UP_KEY:
        ifnot (My->video_first_row_idx)
          continue;

        n = My->num_rows;
        while (n) {
          My->video_first_row_idx--;
          n--;
          ifnot (My->video_first_row_idx) break;
        }

        My->cur_row_idx -= (My->num_rows - n);

        set_video_rows (My, 0, My->video_first_row_idx, My->video_first_row_idx + My->num_rows);
        Video.draw.rows_from_to (My->video, My->first_row, My->last_row);
        adjust_col (My, line);
        GOTO(My->row_pos, My->col_pos);
        continue;

      case 'j':
      case ARROW_DOWN_KEY:
        if (My->row_pos < My->last_row and (size_t) My->cur_row_idx < My->array_len - 1) {
          My->row_pos++;
          My->cur_row_idx++;

        } else if (My->row_pos is My->last_row) {
          if ((size_t) My->cur_row_idx + 1 < My->array_len) {
            My->cur_row_idx++;
            My->video_first_row_idx++;
            set_video_rows (My, 0, My->video_first_row_idx, My->video_first_row_idx + My->num_rows);
            Video.draw.rows_from_to (My->video, My->first_row, My->last_row);

          } else
            continue;

        } else
          continue;

        adjust_col (My, line);
        GOTO(My->row_pos, My->col_pos);
        continue;

      case 'k':
      case ARROW_UP_KEY:
        if (My->row_pos > My->first_row) {
          My->row_pos--;
          My->cur_row_idx--;

        } else if (My->row_pos is My->first_row) {
          if (My->video_first_row_idx > 0) {
            My->cur_row_idx--;
            My->video_first_row_idx--;
            set_video_rows (My, 0, My->video_first_row_idx, My->video_first_row_idx + My->num_rows);
            Video.draw.rows_from_to (My->video, My->first_row, My->last_row);

          } else
            continue;

        } else
          continue;

        adjust_col (My, line);
        GOTO(My->row_pos, My->col_pos);
        continue;

      case 'l':
      case ARROW_RIGHT_KEY:
        it = get_line_num (line, line->cur_col_idx, &num, &fcol_it);

        if (NULL is it or it is line->data->tail)
          continue;

        n = num - 1 + My->first_col;

        if (n < My->last_col) {
          line->cur_col_idx += it->len;
          My->col_pos = num + My->first_col;
          GOTO(My->row_pos, My->col_pos);
          continue;
        }

        if (n is My->last_col or n - it->width is My->last_col) {
          line->first_col_idx += fcol_it->len;
          line->cur_col_idx += it->len;
          set_video_row (My, My->row_pos - 1, My->cur_row_idx);
          Video.draw.rows_from_to (My->video, My->row_pos, My->row_pos);
          GOTO(My->row_pos, My->col_pos);
        }

        continue;

      case 'h':
      case ARROW_LEFT_KEY:
        it = get_line_num (line, line->cur_col_idx, &num, &fcol_it);

        if (it is NULL or it is line->data->head)
          continue;

        n = num - 1 + My->first_col;

        if (n > My->first_col) {
          My->col_pos = num - it->width;
          it = it->prev;
          line->cur_col_idx -= it->len;
          My->col_pos -= it->width;
          My->col_pos += My->first_col;
          GOTO(My->row_pos, My->col_pos);
          continue;
        }

        ifnot (line->first_col_idx)
          continue;

        it = it->prev;
        line->first_col_idx -= it->len;
        line->cur_col_idx   -= it->len;
        set_video_row (My, My->row_pos - 1, My->cur_row_idx);
        Video.draw.rows_from_to (My->video, My->row_pos, My->row_pos);
        GOTO(My->row_pos, My->col_pos);

        continue;

      default: continue;
    }
  }

theend:
  Video.release (My->video);
  Term.reset (My->term);
  Term.release (&My->term);

  for (size_t i = 0; i < My->array_len; i++) {
    Ustring.release (My->lines[i]->data);
    free (My->lines[i]);
  }

  free (My->lines);
  String.release (My->empty_line);
  String.release (My->buf);
  return OK;
}

static VALUE pager_main (la_t *this, VALUE v_lines) {
  ifnot (IS_ARRAY(v_lines)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an array");

  ArrayType *array = (ArrayType *) AS_ARRAY(v_lines);
  ifnot (array->type is STRING_TYPE)
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a StringType array");

  string **lines = (string **) AS_ARRAY(array->value);

  ifnot (array->len) return OK_VALUE;

  int first_row = 1;
  int first_col = 4;
  int last_col  = -1;
  int last_row  = -1;

  Me My = (Me) {
    .cur_row_idx = 0,
    .col_pos = first_col,
    .first_row = first_row,
    .last_row  = last_row,
    .first_col = first_col,
    .last_col = 0,
    .video_first_row_idx = 0,
    .array_len = array->len,
    .buf = String.new (128),
    .lines = Alloc (sizeof (line_t) * My.array_len),
    .term = Term.new (),
    .video = NULL
  };

  Term.set (My.term);

  My.row_pos    = My.first_row;
  My.col_pos    = My.first_col;
  My.last_row   = (last_row is -1 ? My.term->num_rows : last_row);
  My.last_col   = (last_col is -1 ? My.term->num_cols : last_col);
  My.num_rows   = My.last_row - My.first_row + 1;
  My.num_cols   = My.last_col - My.first_col + 1;
  My.empty_line = String.new (My.num_cols);
  My.video      = Video.new (1, My.num_rows, My.num_cols, My.first_row, My.first_col);

  for (size_t i = 0; i < (size_t) My.num_cols; i++)
    String.append_byte (My.empty_line, ' ');

  return INT(pager_normal_mode (&My, lines));
}

public int __init_pager_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(io);
  __INIT__(term);
  __INIT__(video);
  __INIT__(ustring);

  LaDefCFun lafuns[] = {
    { "pager", PTR(pager_main), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Pager = {
      main : pager,
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_pager_module__ (la_t *this) {
  (void) this;
}
