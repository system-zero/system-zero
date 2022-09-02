#define REQUIRE_STDIO
#define REQUIRE_TERMIOS

#define REQUIRE_STD_MODULE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_TERM_TYPE    DECLARE
#define REQUIRE_VIDEO_TYPE   DECLARE
#define REQUIRE_USTRING_TYPE DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_KEYS_MACROS
#define REQUIRE_TERM_MACROS

#include <z/cenv.h>

MODULE(pager);

#define GOTO(r,c) Term.cursor.set_pos (My->term, r, c)
#define ONE_PAGE(__buf__)  __buf__->last_row - __buf__->first_row + 1

#define IS_PAGER(__v__)({ int _r_ = 0; \
  if (IS_OBJECT(__v__)) { object *_o_ = AS_OBJECT(__v__); _r_ = Cstring.eq (_o_->name, "PagerType");}\
  _r_; \
})

#define GET_PAGER(__v__)\
({ \
  ifnot (IS_PAGER(__v__)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting a pager object");\
  object *_o_ = AS_OBJECT(__v__); \
   Me *_s_ = (Me *) AS_OBJECT (_o_->value); \
  _s_; \
})

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
  int     first_row;
  int     first_col;
  int     last_row;
  int     last_col;
  int     num_cols;
  int     num_rows;

  string  *empty_line;
  string  *buf;
  term_t  *term;
  video_t *video;

  Buf     **buffers;
  Buf     *cur_buf;
  int      num_buf;
} Me;


static VALUE pager_release (la_t *this, VALUE v_pager) {
  Me *My = GET_PAGER(v_pager);
  Video.release (My->video);
  Term.reset (My->term);
  Term.release (&My->term);

  String.release (My->empty_line);
  String.release (My->buf);

  for (int i = 0; i < My->num_buf; i++) {
    for (size_t j = 0; j < My->buffers[i]->array_len; j++) {
      Ustring.release (My->buffers[i]->lines[j]->data);
      free (My->buffers[i]->lines[j]);
    }
    free (My->buffers[i]->lines);
    free (My->buffers[i]);
  }

  free (My->buffers);
  free (My);

  return OK_VALUE;
}

static void adjust_col (Buf *My, line_t *prevline) {
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

static void set_video_row (Me *My, Buf *buf, size_t vidx, size_t idx) {
  String.clear (My->buf);

  if (idx >= buf->array_len) {
    Video.set.row_with (My->video, vidx, My->empty_line->bytes);
    return;
  }

  line_t *line = buf->lines[idx];
  ustring_t *it = line->data->head;

  int cidx = 0;
  while (it) {
    if (cidx is line->first_col_idx) break;
    cidx += it->len;
    it = it->next;
  }

  int num = 0;
  while (it and num < buf->num_cols) {
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

static void set_video_rows (Me *My, Buf *buf, size_t vidx, size_t fidx, size_t lidx) {
  for (; fidx <= lidx; fidx++) set_video_row (My, buf, vidx++, fidx);
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

static VALUE pager_normal_mode (la_t *this, VALUE v_pager) {
  Me *My = GET_PAGER(v_pager);

  Buf *buf = My->cur_buf;

  set_video_rows (My, buf, 0, 0, buf->num_rows - 1);
  Video.draw.rows_from_to (My->video, buf->first_row, buf->last_row);

  GOTO(buf->row_pos, buf->col_pos);

  line_t    *line    = NULL;
  ustring_t *it      = NULL;
  ustring_t *fcol_it = NULL;
  int num;
  int n;
  int c;

  for (;;)  {
    c = IO.input.getkey (0);

    line = buf->lines[buf->cur_row_idx];

    switch (c) {
      case 'q':
      case 'Q':
        goto theend;

      case 'g':
        c = IO.input.getkey (0);
        ifnot (c is 'g') continue;
        // fallthrough

      case HOME_KEY:
        ifnot (buf->video_first_row_idx + buf->cur_row_idx)
          if (buf->row_pos is buf->first_row) continue;

        buf->row_pos = buf->first_row;
        buf->video_first_row_idx = 0;
        buf->cur_row_idx = 0;
        set_video_rows (My, buf, 0, buf->video_first_row_idx, buf->video_first_row_idx + buf->num_rows);
        Video.draw.rows_from_to (My->video, buf->first_row, buf->last_row);
        adjust_col (buf, line);
        GOTO(buf->row_pos, buf->col_pos);
        continue;

      case 'G':
      case END_KEY:
        if ((size_t) buf->video_first_row_idx is buf->array_len - 1) continue;
        if ((size_t) buf->cur_row_idx is buf->array_len - 1) continue;

        buf->cur_row_idx = buf->array_len - 1;

        if ((size_t) ONE_PAGE(buf) > buf->array_len) {
          buf->row_pos = buf->first_row + buf->array_len - 1;
        } else {
          buf->video_first_row_idx = buf->array_len - ONE_PAGE(buf);
          buf->row_pos = buf->last_row;
          set_video_rows (My, buf, 0, buf->video_first_row_idx, buf->video_first_row_idx + buf->num_rows);
          Video.draw.rows_from_to (My->video, buf->first_row, buf->last_row);
        }

        adjust_col (buf, line);
        GOTO(buf->row_pos, buf->col_pos);
        continue;

      case CTRL('f'):
      case PAGE_DOWN_KEY:
        if ((size_t) (buf->video_first_row_idx + buf->num_rows) > buf->array_len)
          continue;

        buf->video_first_row_idx += buf->num_rows;
        buf->cur_row_idx         += buf->num_rows;

        while ((size_t) buf->cur_row_idx >= buf->array_len)
          buf->cur_row_idx--;

        n = buf->array_len - (size_t) buf->video_first_row_idx;
        while (buf->row_pos > n) buf->row_pos--;

        set_video_rows (My, buf, 0, buf->video_first_row_idx, buf->video_first_row_idx + buf->num_rows);
        Video.draw.rows_from_to (My->video, buf->first_row, buf->last_row);
        adjust_col (buf, line);
        GOTO(buf->row_pos, buf->col_pos);
        continue;

      case CTRL('b'):
      case PAGE_UP_KEY:
        ifnot (buf->video_first_row_idx)
          continue;

        n = buf->num_rows;
        while (n) {
          buf->video_first_row_idx--;
          n--;
          ifnot (buf->video_first_row_idx) break;
        }

        buf->cur_row_idx -= (buf->num_rows - n);

        set_video_rows (My, buf, 0, buf->video_first_row_idx, buf->video_first_row_idx + buf->num_rows);
        Video.draw.rows_from_to (My->video, buf->first_row, buf->last_row);
        adjust_col (buf, line);
        GOTO(buf->row_pos, buf->col_pos);
        continue;

      case 'j':
      case ARROW_DOWN_KEY:
        if (buf->row_pos < buf->last_row and (size_t) buf->cur_row_idx < buf->array_len - 1) {
          buf->row_pos++;
          buf->cur_row_idx++;

        } else if (buf->row_pos is buf->last_row) {
          if ((size_t) buf->cur_row_idx + 1 < buf->array_len) {
            buf->cur_row_idx++;
            buf->video_first_row_idx++;
            set_video_rows (My, buf, 0, buf->video_first_row_idx, buf->video_first_row_idx + buf->num_rows);
            Video.draw.rows_from_to (My->video, buf->first_row, buf->last_row);

          } else
            continue;

        } else
          continue;

        adjust_col (buf, line);
        GOTO(buf->row_pos, buf->col_pos);
        continue;

      case 'k':
      case ARROW_UP_KEY:
        if (buf->row_pos > buf->first_row) {
          buf->row_pos--;
          buf->cur_row_idx--;

        } else if (buf->row_pos is buf->first_row) {
          if (buf->video_first_row_idx > 0) {
            buf->cur_row_idx--;
            buf->video_first_row_idx--;
            set_video_rows (My, buf, 0, buf->video_first_row_idx, buf->video_first_row_idx + buf->num_rows);
            Video.draw.rows_from_to (My->video, buf->first_row, buf->last_row);

          } else
            continue;

        } else
          continue;

        adjust_col (buf, line);
        GOTO(buf->row_pos, buf->col_pos);
        continue;

      case 'l':
      case ARROW_RIGHT_KEY:
        it = get_line_num (line, line->cur_col_idx, &num, &fcol_it);

        if (NULL is it or it is line->data->tail)
          continue;

        n = num - 1 + buf->first_col;

        if (n < buf->last_col) {
          line->cur_col_idx += it->len;
          buf->col_pos = num + buf->first_col;
          GOTO(buf->row_pos, buf->col_pos);
          continue;
        }

        if (n is buf->last_col or n - it->width is buf->last_col) {
          line->first_col_idx += fcol_it->len;
          line->cur_col_idx += it->len;
          set_video_row (My, buf, buf->row_pos - 1, buf->cur_row_idx);
          Video.draw.rows_from_to (My->video, buf->row_pos, buf->row_pos);
          GOTO(buf->row_pos, buf->col_pos);
        }

        continue;

      case 'h':
      case ARROW_LEFT_KEY:
        it = get_line_num (line, line->cur_col_idx, &num, &fcol_it);

        if (it is NULL or it is line->data->head)
          continue;

        n = num - 1 + buf->first_col;

        if (n > buf->first_col) {
          buf->col_pos = num - it->width;
          it = it->prev;
          line->cur_col_idx -= it->len;
          buf->col_pos -= it->width;
          buf->col_pos += buf->first_col;
          GOTO(buf->row_pos, buf->col_pos);
          continue;
        }

        ifnot (line->first_col_idx)
          continue;

        it = it->prev;
        line->first_col_idx -= it->len;
        line->cur_col_idx   -= it->len;
        set_video_row (My, buf, buf->row_pos - 1, buf->cur_row_idx);
        Video.draw.rows_from_to (My->video, buf->row_pos, buf->row_pos);
        GOTO(buf->row_pos, buf->col_pos);

        continue;

      default: continue;
    }
  }

theend:
  return OK_VALUE;
}

static VALUE pager_main (la_t *this, VALUE v_pager) {
  return pager_normal_mode (this, v_pager);
}

static VALUE pager_new (la_t *this, VALUE v_lines) {
  ifnot (IS_ARRAY(v_lines)) THROW(LA_ERR_TYPE_MISMATCH, "awaiting an array");

  ArrayType *array = (ArrayType *) AS_ARRAY(v_lines);
  ifnot (array->type is STRING_TYPE)
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a StringType array");

  string **lines = (string **) AS_ARRAY(array->value);

  ifnot (array->len) return OK_VALUE;

  int first_row = 1;
  int first_col = 1;
  int last_col  = -1;
  int last_row  = -1;
  int tabwidth  = 8;

  Me *My = Alloc (sizeof (Me));

  (*My) = (Me) {
    .first_row = first_row,
    .last_row  = last_row,
    .first_col = first_col,
    .last_col = 0,
    .buf = String.new (128),
    .term = Term.new (),
    .video = NULL,
  };

  Term.set (My->term);

  My->last_row   = (last_row is -1 ? My->term->num_rows : last_row);
  My->last_col   = (last_col is -1 ? My->term->num_cols : last_col);
  My->num_rows   = My->last_row - My->first_row + 1;
  My->num_cols   = My->last_col - My->first_col + 1;
  My->num_rows   = My->last_row - My->first_row + 1;
  My->num_cols   = My->last_col - My->first_col + 1;

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
    .array_len = array->len,
    .lines = Alloc (sizeof (line_t) * array->len)
  };
  buf->row_pos   = My->first_row;
  buf->col_pos   = My->first_col;
  buf->last_row  = My->last_row;
  buf->last_col  = My->last_col;
  buf->num_rows  = My->num_rows;
  buf->num_cols  = My->num_cols;

  My->cur_buf = buf;
  My->num_buf = 1;

  My->empty_line = String.new (buf->num_cols);
  My->video      = Video.new (1, My->num_rows, My->num_cols, My->first_row, My->first_col);

  for (size_t i = 0; i < (size_t) My->num_cols; i++)
    String.append_byte (My->empty_line, ' ');

 for (size_t j = 0; j < My->buffers[0]->array_len; j++)
    My->buffers[0]->lines[j] = Alloc (sizeof (line_t));

  for (size_t i = 0; i < My->buffers[0]->array_len; i++) {
    Ustring_t *u = Ustring.new ();
    Ustring.encode (u, lines[i]->bytes,
                       lines[i]->num_bytes, 1, tabwidth, 0);
    My->buffers[0]->lines[i]->data = u;
    My->buffers[0]->lines[i]->first_col_idx = 0;
    My->buffers[0]->lines[i]->cur_col_idx = 0;
  }

  VALUE v = OBJECT(My);
  object *o = La.object.new (pager_release, NULL, "PagerType", v);
  return OBJECT(o);
}

public int __init_pager_module__ (la_t *this) {
  __INIT_MODULE__(this);
  __INIT__(io);
  __INIT__(term);
  __INIT__(video);
  __INIT__(ustring);
  __INIT__(cstring);

  LaDefCFun lafuns[] = {
    { "pager_new",  PTR(pager_new), 1 },
    { "pager_main", PTR(pager_main), 1 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Pager = {
      new  : pager_new,
      main : pager_main
     }
   );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_pager_module__ (la_t *this) {
  (void) this;
}
