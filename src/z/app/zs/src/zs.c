#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_STDARG
#define REQUIRE_TIME
#define REQUIRE_SYS_STAT
#define REQUIRE_SIGNAL
#define REQUIRE_TERMIOS

#define REQUIRE_SH_TYPE      DECLARE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_SYS_TYPE     DECLARE
#define REQUIRE_TERM_TYPE    DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_PATH_TYPE    DECLARE
#define REQUIRE_RLINE_TYPE   DECLARE
#define REQUIRE_ERROR_TYPE   DECLARE
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_VSTRING_TYPE DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_USTRING_TYPE DECLARE
#define REQUIRE_LIST_MACROS
#define REQUIRE_TERM_MACROS
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

/* to match rline's ones */
#undef  ARROW_UP_KEY
#define ARROW_UP_KEY   -20
#undef  ARROW_DOWN_KEY
#define ARROW_DOWN_KEY -21
#undef  HOME_KEY
#define HOME_KEY       -25
#undef  END_KEY
#define END_KEY        -26
#undef  PAGE_UP_KEY
#define PAGE_UP_KEY    -28
#undef  PAGE_DOWN_KEY
#define PAGE_DOWN_KEY  -29

#define DEFAULT_ROOT_PROMPT "\033[31m$\033[m "
#define DEFAULT_USER_PROMPT "\033[32m$\033[m "

#define MAXLEN_DIR          4095
#define MAXLEN_COMMAND      255
#define MAXLEN_COMMAND_LINE 8191
#define MAXLEN_HINT         63

#define ZS_COMMAND_HAS_NO_FILENAME_ARG (1 << 0)
#define ZS_COMMAND_HAS_LITERAL_ARG     (1 << 1)

#define ZS_NO_COMMAND 0
#define ZS_COMMAND    1

#define ZS_RLINE_ARG_IS_COMMAND        1
#define ZS_RLINE_ARG_IS_ARG            2
#define ZS_RLINE_ARG_IS_DIRECTORY      3
#define ZS_RLINE_ARG_IS_FILENAME       4
#define ZS_RLINE_ARG_IS_LAST_COMPONENT 5

#define MAXNUM_LAST_COMPONENTS 20

typedef struct zs_t zs_t;

/* (rather crude completion ui)
 * this code is from src/y/pkg/pager, slowly adapted for this environment
 */

#define DEFAULT_TABWIDTH 2

enum {
  PAGER_PROCESS_CHAR = 0,
  PAGER_CONTINUE,
  PAGER_EXIT
};

typedef struct pager_t pager_t;

typedef int (*PagerOnInput) (pager_t *, int *);

typedef struct video_t {
  int fd;
  int num_rows;
  int row_pos;
  int col_pos;
  int first_col;
  string **rows;
  string *render;
} video_t;

typedef struct line_t {
  int first_col_idx;
  int cur_col_idx;
  size_t num_bytes;
  Ustring_t *data;
} line_t;

typedef struct Buf {
  int cur_row_idx;
  int row_pos;
  int col_pos;
  int num_rows;
  int num_cols;
  int first_row;
  int first_col;
  int last_row;
  int last_col;
  int video_first_row_idx;
  int tabwidth;

  size_t  array_len;

  int     input_should_be_freed;
  string  **input;

  line_t  **lines;
  line_t  *cur_line;
  line_t  *prev_line;

  string  *tmp_buf;
  string  *empty_line;

  video_t *video;
  term_t  *term;
} Buf;

struct pager_t {
  int     first_row;
  int     first_col;
  int     last_row;
  int     last_col;
  int     num_cols;
  int     num_rows;
  int     c;

  video_t *video;
  string  **video_rows;

  term_t  *term;
  Buf     *buf;
};

typedef struct pager_opts {
  int first_row;
  int first_col;
  int last_col;
  int last_row;
  int input_should_be_freed;
} pager_opts;

#define PagerOpts(...) (pager_opts) {  \
  .first_row  = -1,                    \
  .first_col  = -1,                    \
  .last_row   = -1,                    \
  .last_col   = -1,                    \
  .input_should_be_freed = 0,          \
  __VA_ARGS__                          \
}

typedef struct completion_t {
  zs_t *zs;
  rline_t *rline;
  string **ar;
  size_t arlen;
  rlineCompletions *lc;
  int idx;
  string *arg;
  const char *prefix;
  size_t prefix_len;
  const char *suffix;
  size_t suffix_len;
  const char *curbuf;
  int quote_arg;
} completion_t;

typedef struct LastComp LastComp;
struct LastComp {
  char *lastcomp;
  LastComp *next;
};

typedef struct Command Command;
struct Command {
  char *name;
  int flags;
  Command *next;
};

struct zs_t {
  Command *command_head;
  int num_commands;

  LastComp *lastcomp_head;
  int numLastComp;

  char command[MAXLEN_COMMAND + 1];
  char hint[MAXLEN_HINT + 1];

  int
    arg_type,
    num_items,
    last_retval,
    exit_val;

  string
    *arg,
    *comdir,
    *origin_directory,
    *completion_command;

  rline_t *rline;
  term_t *term;
  pager_t *pager;
  completion_t *completion;
  sh_t *sh;
};

/* ui implementation (some more lines overhead that have no usage other
   than a completion ui, so they have no actual relationship with a shell
   (but useful anyways and almost a prerequisite for a shell these days)) */

#define DONE             0
#define DONE_NEEDS_DRAW  1
#define NOTHING_TODO     2

#define GOTO(r,c) Term.cursor.set_pos (My->term, r, c)
#define ONE_PAGE(__buf__)  ((__buf__->last_row - __buf__->first_row) + 1)

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

static void buf_clear_input (Buf *My) {
  for (size_t i = 0; i < My->array_len; i++)
    String.release (My->input[i]);
}

static void buf_release_input (Buf *My) {
  ifnot (My->input_should_be_freed) return;

  if (My->input is NULL) return;

  buf_clear_input (My);

  free (My->input);
  My->input = NULL;
}

static void buf_clear_lines (Buf *My) {
  if (My->lines is NULL) return;

  for (size_t i = 0; i < My->array_len; i++) {
    Ustring.release (My->lines[i]->data);
    free (My->lines[i]);
  }
}

static void buf_clear (Buf *My) {
  buf_release_input (My);
  buf_clear_lines (My);
  free (My->lines);
  My->lines = NULL;
}

static void buf_release (Buf *My) {
  buf_clear (My);
  String.release (My->empty_line);
  String.release (My->tmp_buf);
  free (My);
}

static void pager_release (pager_t *My) {
  video_release (My->video);
  buf_release (My->buf);
  free (My);
}

static void buf_set_lines (Buf *My) {
  for (size_t i = 0; i < My->array_len; i++) {
    My->lines[i] = Alloc (sizeof (line_t));
    Ustring_t *u = Ustring.new ();
    Ustring.encode (u, My->input[i]->bytes,
                       My->input[i]->num_bytes, 1, My->tabwidth, 0);
    My->lines[i]->data = u;
    My->lines[i]->first_col_idx = 0;
    My->lines[i]->cur_col_idx = 0;
    My->lines[i]->num_bytes = My->input[i]->num_bytes;
  }
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
  while (it) {
    num += it->width;
    if (num > My->num_cols) break; // we might loose accuracy but stay safe
                                   // as our demand is too simple
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

  IO.fd.write (My->video->fd, My->video->render->bytes, My->video->render->num_bytes);
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
    return DONE;
  }

  My->video_first_row_idx = (My->array_len - ONE_PAGE(My));
  My->row_pos = My->last_row;
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

  return DONE_NEEDS_DRAW;
}

static int buf_up (Buf *My) {
  if (My->row_pos > My->first_row) {
    My->row_pos--;
    My->cur_row_idx--;
    My->prev_line = My->cur_line;
    My->cur_line  = My->lines[My->cur_row_idx];
    return DONE;
  }

  if (My->row_pos is My->first_row and My->video_first_row_idx > 0) {
    My->cur_row_idx--;
    My->prev_line = My->cur_line;
    My->cur_line  = My->lines[My->cur_row_idx];
    My->video_first_row_idx--;
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
    return DONE;
  }

  if (My->row_pos is My->last_row and
      (size_t) My->cur_row_idx + 1 < My->array_len) {
    My->cur_row_idx++;
    My->prev_line = My->cur_line;
    My->cur_line  = My->lines[My->cur_row_idx];
    My->video_first_row_idx++;
    return DONE_NEEDS_DRAW;
  }

  return NOTHING_TODO;
}

static char *pager_main (pager_t *My, int idx, rline_t *rline, size_t *comlen) {
  Buf *buf = My->buf;

  char *match = NULL;

  string *s = String.new (8);

  buf->col_pos = idx + 1;

  buf_set_rows_and_draw (buf);

  int r;

  for (;;)  {
    My->c = Rline.fd_read (rline, 0);

    if (My->c is -1) break;

    if (My->c is 127) My->c = BACKSPACE_KEY;

    if (My->c is ESCAPE_KEY)
      My->c = Rline.check_special (rline, 0);

    switch (My->c) {
      case ESCAPE_KEY:
        goto theend;

      case ' ':
      case '\r':
        match = buf->input[buf->cur_row_idx]->bytes;
        *comlen = buf->input[buf->cur_row_idx]->num_bytes;
        goto theend;

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

      case '\t':
      case ARROW_DOWN_KEY:
        r = buf_down (buf);
        if (r isnot NOTHING_TODO) {
          if (r is DONE_NEEDS_DRAW)
            buf_set_rows_and_draw (buf);
          else
            GOTO(buf->row_pos, buf->col_pos);

          continue;
        }
        // else goto the first item in list
        // fallthrough

      case HOME_KEY:
        r = buf_home (buf);
        if (r is NOTHING_TODO or r is DONE) continue;

        buf_set_rows_and_draw (buf);

        continue;

      case ARROW_UP_KEY:
        r = buf_up (buf);
        if (r isnot NOTHING_TODO) {
          if (r is DONE_NEEDS_DRAW)
            buf_set_rows_and_draw (buf);
          else
            GOTO(buf->row_pos, buf->col_pos);

          continue;
        }
        // else goto the last item in list
        // fallthrough

      case END_KEY:
        r = buf_eof (buf);
        if (r is NOTHING_TODO)  continue;

        if (r is DONE_NEEDS_DRAW)
          buf_set_rows_and_draw (buf);
        else
          GOTO(buf->row_pos, buf->col_pos);

        continue;

      default:
        if (' ' < My->c) {
          char t[idx+2];
          int i = 0;
          for (; i < idx; i++) t[i] = buf->input[0]->bytes[i];
          t[i++] = My->c;
          idx = i;

          string **tmp = Alloc (sizeof (string) * buf->array_len);
          size_t tidx = 0;
          for (size_t j = 0; j < buf->array_len; j++) {
            if (Cstring.eq_n (buf->input[j]->bytes, t, idx)) {
              tmp[tidx++] = String.new_with_len (buf->input[j]->bytes, buf->input[j]->num_bytes);
            }
          }

          int len = tidx;

          ifnot (len) {
            free (tmp);
            goto theend;
          }

          buf_clear_lines (buf);
          free (buf->lines);

          buf_clear_input (buf);
          free (buf->input);
          buf->input = tmp;
          buf->array_len = len;

          buf->lines = Alloc (sizeof (line_t) * buf->array_len);
          buf_set_lines (buf);

          buf->cur_row_idx = 0;
          buf->video_first_row_idx = 0;
          buf->row_pos = My->first_row;
          buf->col_pos = idx + 1;
          buf_set_rows_and_draw (buf);
        }
    }
  }

theend:
  String.release (s);
  return match;
}

static pager_t *pager_new (zs_t *zs, string **lines, size_t array_len, pager_opts opts) {
  pager_t *My = zs->pager;

  int first_row = opts.first_row;
  int first_col = opts.first_col;
  int last_row  = opts.last_row;
  int last_col  = opts.last_col;

  My->first_row = (first_row is -1 ? 1 : first_row);
  My->last_row  = (last_row is -1 ? My->term->num_rows : last_row);
  My->first_col = (first_col is -1 ? 1 : first_col);
  My->last_col  = (last_col is -1 ? My->term->num_cols : last_col);
  My->num_rows  = My->last_row - My->first_row + 1;
  My->num_cols  = My->last_col - My->first_col + 1;

  Buf *buf = My->buf;

  buf->cur_row_idx = 0;
  buf->first_col = first_col;
  buf->video_first_row_idx = 0;
  buf->array_len = array_len;
  buf->input_should_be_freed = opts.input_should_be_freed;
  buf->input = lines;
  buf->cur_line = NULL;
  buf->prev_line = NULL;
  buf->lines = Alloc (sizeof (line_t) * array_len);
  buf->row_pos   = My->first_row;
  buf->col_pos   = My->first_col;
  buf->first_row = My->first_row;
  buf->last_row  = My->last_row;
  buf->last_col  = My->last_col;
  buf->num_rows  = My->num_rows;
  buf->num_cols  = My->num_cols;

  buf->tabwidth = DEFAULT_TABWIDTH;

  buf_set_lines (buf);

  buf->cur_line = buf->prev_line = buf->lines[0];

  buf_release_input (buf); // we don't need this anymore, free it if it is allowed

  return My;
}

static int completion_pager (completion_t *this) {
  ifnot (this->arlen) return -1;

  int r = 0;
  char *command = NULL;
  size_t comlen = 0;

  pager_t *p = this->zs->pager;
  term_t *term = this->zs->term;
  int termrestore = 1;

  if (this->arlen is 1) {
    Buf *buf = p->buf;
    buf->input = this->ar;
    buf->array_len = 1;
    command = this->ar[0]->bytes;
    comlen = this->ar[0]->num_bytes;
    this->zs->pager->c = 0;
    termrestore = 0;
    goto add_completion;
  }

  Term.cursor.get_pos (term, &term->orig_curs_row_pos, &term->orig_curs_col_pos);

  p = pager_new (this->zs, this->ar, this->arlen, PagerOpts (
    .first_row = 1,
    .first_col = 1,
    ));

  Term.screen.save (term);
  Term.screen.clear (term);

  term->is_initialized = 1; // fake it

  comlen = 0;
  command = pager_main (p, this->idx, this->rline, &comlen);

  r = NULL isnot command;

  if (r) {
add_completion:
    this->zs->num_items = 1;
    Rline.set.flags (this->rline, this->lc, RLINE_ACCEPT_ONE_ITEM);
    String.clear (this->arg);

    int cur_pos = this->prefix_len;

    if (this->prefix)
      String.append_with_len (this->arg, this->prefix, this->prefix_len);

    String.append_with_len (this->arg, command, comlen);
    cur_pos += comlen;

    if (this->suffix)
      String.append_with_len (this->arg, this->suffix, this->suffix_len);

    if (p->c is '\t')
      if (Cstring.eq (this->curbuf, this->arg->bytes)) {
         p->c = 0;
         return 0;
       }

    if (this->quote_arg) {
      if (Cstring.byte.in_str (command, ' ')) {
        int offset = 0;
        ifnot (NULL is this->prefix) {
          char *s = (char *) this->prefix + this->prefix_len;
          char *t = s;
          ifnot (*(s - 1) is '"') {
            ifnot (*(s - 1) is ' ') {
              s--;
              while (s > this->prefix) {
                if (*(s - 1) is ' ') break;
                s--;
              }
            }

            offset = 1;
            String.insert_byte_at (this->arg, '"', this->prefix_len - (t - s));
            cur_pos++;
          }
        }

        String.insert_byte_at (this->arg, '"', this->prefix_len + offset + comlen);
        cur_pos++;
      }
    }

    Rline.add_completion (this->rline, this->lc, this->arg->bytes, cur_pos);
    String.replace_with (this->zs->completion_command, command); // leave it at the end of the block
                                                                // as it might be used before
    this->zs->arg_type = ZS_RLINE_ARG_IS_COMMAND;
  }

  if (termrestore) {
    Term.cursor.set_pos (term, term->orig_curs_row_pos, term->orig_curs_col_pos);
    Term.screen.restore (term);
    term->is_initialized = 0;
  }

  p->buf->input_should_be_freed = 1;
  buf_clear (p->buf);

  return r;
}

static int zs_completion (const char *bufp, int curpos, rlineCompletions *lc, void *userdata) {
  zs_t *zs = (zs_t *) userdata;
  rline_t *rline = zs->rline;
  Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);

  zs->num_items = 0;
  string *arg = zs->arg;
  String.clear (arg);

  completion_t *completion = zs->completion;
  completion->lc = lc;
  completion->curbuf = bufp;
  completion->quote_arg = 0;
  zs->pager->c = 0;

  dirlist_t *dlist = NULL;
  char *dirname = NULL;
  char *basename = NULL;
  size_t dirlen;
  size_t bname_len;

  char *buf = (char *) bufp;
  int buf_isallocated = 0;
  size_t buflen;

  while (*buf is ' ' and curpos) {
    buf++;
    curpos--;
  }

  const char *lptr = buf + curpos;
  size_t lptrlen = bytelen (lptr);

  buflen = bytelen (buf) - lptrlen;

  char ptrbuf[buflen + 1];

  char *ptr = (char *) lptr;
  int ptrlen = 0;
  int is_arg = 0;
  int arglen = 0;
  int is_filename = 0;

  while (ptr isnot buf) {
    if (*(ptr - 1) is ' ')
      break;
    ptr--;
    arglen++;
  }

  if (buf[0] is '\0') {
    Command *it = zs->command_head;

    string **ar = Alloc (sizeof (string));
    int idx = 0;

    while (it) {
      ar = Realloc (ar, sizeof (string) * (idx + 1));
      ar[idx] = String.new (4);
      char *name = it->name;
      while (*name) {
        String.append_byte (ar[idx], *name);
        if (*name++ is '.') break;
      }

      while (it->next) {
        if (*(name - 1) is '.' and Cstring.eq_n (ar[idx]->bytes, it->next->name, ar[idx]->num_bytes)) {
          it = it->next;
          continue;
        }
        break;
      }

      it = it->next;
      idx++;
    }

    completion->ar = ar;
    completion->arlen = idx;
    completion->idx = 0;
    completion->prefix = NULL;
    completion->prefix_len = 0;
    completion->suffix = NULL;

    int r = completion_pager (completion);

    if (r is -1) free (ar);

    if (zs->pager->c is '\r' or zs->pager->c is '\t')
      goto theend;

    if (1 is r) {
      if (arg->bytes[arg->num_bytes-1] is '.') {
        it = zs->command_head;
        ar = Alloc (sizeof (string));
        idx = 0;

        while (it) {
          if (Cstring.eq_n (it->name, arg->bytes, arg->num_bytes)) {
            ar = Realloc (ar, sizeof (string) * (idx + 1));
            ar[idx++] = String.new_with (it->name + arg->num_bytes);
          }
          it = it->next;
        }

        if (1 is idx) {
          Rline.release_completions (rline, lc);
          zs->num_items = 1;
          Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);

          ifnot (Cstring.eq_n (ar[0]->bytes, arg->bytes, arg->num_bytes)) {
            String.append_with_len (arg, ar[0]->bytes, ar[0]->num_bytes);
            Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes);
          } else
            Rline.add_completion (rline, lc, ar[0]->bytes, ar[0]->num_bytes);

          zs->arg_type = ZS_RLINE_ARG_IS_COMMAND;
          String.release (ar[0]);
          free (ar);
          goto theend;
        }

        Rline.release_completions (rline, lc);
        char prefix[arg->num_bytes + 1];
        Cstring.cp (prefix, arg->num_bytes + 1, arg->bytes, arg->num_bytes);

        completion->ar = ar;
        completion->arlen = idx;
        completion->idx = 0;
        completion->prefix = prefix;
        completion->suffix = NULL;
        completion->prefix_len = arg->num_bytes;

        r = completion_pager (completion);
        if (-1 is r) free (ar);
      }
    }

    goto theend;
  }

  if (ptr is buf and
       ((*ptr is '.' and (*(ptr+1) is DIR_SEP or *(ptr+1) is '.')) or *ptr is DIR_SEP)) {
    char *sp = (char *) buf;
    char *tmp = sp;
    ptrlen = 1;
    sp++;
    while (*sp and *sp isnot ' ' and *(sp + 1) isnot ' ') {
      ptrlen++;
      sp++;
    }

    Cstring.cp (ptrbuf, ptrlen + 1, tmp, ptrlen);

    dirname = Path.dirname (ptrbuf);
    basename = Path.basename (ptrbuf);
    dirlen = bytelen (dirname);
    bname_len = bytelen (basename);

    int lidx = 0;
    int idx = 0;
    string **ar = Alloc (sizeof (string));

    if (bname_len and Cstring.eq (basename, "." DIR_SEP_STR) is 0) {
      string *d = NULL;
      int is_dir = Dir.is_directory (basename);

      ifnot (is_dir) {
        d = String.new_with (dirname);
        if (d->bytes[d->num_bytes-1] isnot DIR_SEP and *basename isnot DIR_SEP)
          String.append_byte (d, DIR_SEP);
        else
          if (Cstring.eq (dirname, DIR_SEP_STR) and *basename is DIR_SEP)
            String.clear (d);

        String.append_with_len (d, basename, bname_len);

        is_dir = Dir.is_directory (d->bytes);
      }

      if (is_dir) {
        if (d is NULL) {
          if (Cstring.eq (dirname, ".") and Cstring.eq_n (basename, "..", 2))
            d = String.new (bname_len);
          else
            d = String.new_with_len (dirname, dirlen);

          if (d->num_bytes and (d->bytes[d->num_bytes-1] isnot DIR_SEP and *basename isnot DIR_SEP))
            String.append_byte (d, DIR_SEP);
          else
            if (Cstring.eq (dirname, DIR_SEP_STR) and *basename is DIR_SEP)
              String.clear (d);

          String.append_with_len (d, basename, bname_len);
        }

        String.clear (zs->origin_directory);
        dlist  = Dir.list (d->bytes, DIRLIST_LNK_IS_DIRECTORY);
        lidx = d->num_bytes;
      } else {
        String.clear (zs->origin_directory);
        dlist  = Dir.list (dirname, DIRLIST_LNK_IS_DIRECTORY);
        lidx = dirlen;
      }

      if (NULL is dlist) {
        String.release (d);
        goto theend;
      }

      vstring_t *it = dlist->list->head;

      while (it) {
        ifnot (is_dir) {
          if (Cstring.eq_n (basename, it->data->bytes, bname_len)) {
            ar = Realloc (ar, sizeof (string) * (idx + 1));
            ar[idx] = String.new_with (dirname);
            if (ar[idx]->bytes[ar[idx]->num_bytes-1] isnot DIR_SEP)
              String.append_byte (ar[idx], DIR_SEP);
            String.append_with_len (ar[idx], it->data->bytes, it->data->num_bytes);

            idx++;
            lidx = dirlen + bname_len + (dirname[dirlen - 1] isnot DIR_SEP);
          }

        } else {
          ar = Realloc (ar, sizeof (string) * (idx + 1));
          ar[idx] = String.new_with_len (d->bytes, d->num_bytes);
          if (ar[idx]->bytes[ar[idx]->num_bytes-1] isnot DIR_SEP)
            String.append_byte (ar[idx], DIR_SEP);

          String.append_with_len (ar[idx], it->data->bytes, it->data->num_bytes);

          idx++;
          lidx = d->num_bytes - 1 + (d->bytes[d->num_bytes - 1] isnot DIR_SEP) + 1;
        }

        it = it->next;
      }

      String.release (d);
    } else {
      String.replace_with_len (zs->origin_directory, dirname, dirlen);
      dlist  = Dir.list (dirname, DIRLIST_LNK_IS_DIRECTORY);
      if (NULL is dlist) goto theend;

      vstring_t *it = dlist->list->head;

      while (it) {
        ar = Realloc (ar, sizeof (string) * (idx + 1));
        ar[idx] = String.new (8);

        if (Cstring.eq_n (ptrbuf, "." DIR_SEP_STR, 2))
          String.append_with_len (ar[idx], "." DIR_SEP_STR, 2);

        ifnot (lidx) lidx = ar[idx]->num_bytes;

        String.append_with_len (ar[idx], it->data->bytes, it->data->num_bytes);

        idx++;
        it = it->next;
      }
    }

    ifnot (idx) {
      free (ar);
      goto theend;
    }

    zs->arg_type = ZS_RLINE_ARG_IS_FILENAME;
    completion->ar = ar;
    completion->arlen = idx;
    completion->idx = lidx;
    completion->prefix = NULL;
    completion->prefix_len = 0;
    completion->suffix = lptr;
    completion->suffix_len = lptrlen;

    if (-1 is completion_pager (completion))
      free (ar);

    if (zs->pager->c is ' ') {
      if (zs->origin_directory->bytes[zs->origin_directory->num_bytes - 1] isnot DIR_SEP)
        String.append_byte (zs->origin_directory, DIR_SEP);

      String.append_with_len (zs->origin_directory, zs->completion_command->bytes,
          zs->completion_command->num_bytes);
      if (Dir.is_directory (zs->origin_directory->bytes))
        zs->pager->c = '\t';
    }
    goto theend;
  }

  if (buflen > 2 and buf[buflen-1] is '.') { // adjust code to count if it is really a command and not a hidden file
    Command *it = zs->command_head;

    string **ar = Alloc (sizeof (string));
    size_t idx = 0;
    while (it) {
      if (Cstring.eq_n (it->name, buf, buflen)) {
        ar = Realloc (ar, sizeof (string) * (idx + 1));
        ar[idx++] = String.new_with (it->name + buflen);
      }
      it = it->next;
    }

    completion->ar = ar;
    completion->arlen = idx;
    completion->idx = 0;
    completion->prefix = buf;
    completion->prefix_len = buflen;
    completion->suffix = lptr;
    completion->suffix_len = lptrlen;

    int r = completion_pager (completion);

    if (r is -1) free (ar);

    goto theend;
  }

  if (ptr isnot buf and *ptr is '-') {
    buflen = ptr - buf;
    is_arg = 1;
  }

  char *com = zs->command;
  int comlen = 0;
  char *sp = (char *) buf;
  while (*sp is ' ') sp++;
  if (sp is buf + buflen)
    goto theend;

  while (*sp) {
    if (*sp is ' ') break;
    if (comlen is MAXLEN_COMMAND)
      goto theend;

    com[comlen++] = *sp;
    sp++;
  }
  com[comlen] = '\0';

  string **argar = NULL;
  int argidx = 0;

  if (0 is is_arg and comlen) {
    Command *it = zs->command_head;

    while (it) {
      if (Cstring.eq (it->name, com)) {
        if (it->flags & ZS_COMMAND_HAS_NO_FILENAME_ARG) {
          is_arg = 1;
          buflen = ptr - buf;
          break;
        }

        if (it->flags & ZS_COMMAND_HAS_LITERAL_ARG) {
          size_t lbuflen = ptr - buf;

          size_t filelen = zs->comdir->num_bytes + 1 + comlen + sizeof ("/literal_args");
          char file[filelen + 1];
          Cstring.cp_fmt (file, filelen + 1, "%s/%s/literal_args", zs->comdir->bytes, com);

          if (-1 is access (file, F_OK|R_OK))
            break;

          FILE *fp = fopen (file, "r");
          if (NULL is fp) break;

          char *args = NULL;
          size_t argslen;
          ssize_t nread;
          String.replace_with_len (arg, buf, lbuflen);

          while (-1 isnot (nread = getline (&args, &argslen, fp))) {
            args[nread - 1] = '\0';
            sp = args;

            while (*sp is ' ') sp++;
            while (*sp) {
              if (*sp is ' ' or *sp is '\n') break;
              String.append_byte (arg, *sp++);
            }

            if (arg->num_bytes > lbuflen) {
              if (Cstring.eq_n (ptr, arg->bytes + lbuflen, arglen)) {
                if (NULL is argar)
                  argar = Alloc (sizeof (string));
                argar = Realloc (argar, sizeof (string) * (argidx + 1));
                argar[argidx] = String.new_with_len (arg->bytes + lbuflen, arg->num_bytes - lbuflen);
                argidx++;
                zs->arg_type = ZS_RLINE_ARG_IS_ARG;
              }

              String.clear_at (arg, lbuflen);
            }
          }

          fclose (fp);
          ifnot (NULL is args)
            free (args);
        }

        break;
      }

      it = it->next;
    }
  }

  if (is_arg) {
    int is_long = *(ptr + 1) is '-';

    size_t filelen = zs->comdir->num_bytes + 1 + comlen + sizeof ("/args");
    char file[filelen + 1];
    Cstring.cp_fmt (file, filelen + 1, "%s/%s/args", zs->comdir->bytes, com);

    if (-1 is access (file, F_OK|R_OK))
      goto theend;

    FILE *fp = fopen (file, "r");
    if (NULL is fp) goto theend;

    char *args = NULL;
    size_t argslen;
    ssize_t nread;
    String.append_with_len (arg, buf, buflen);

    while (-1 isnot (nread = getline (&args, &argslen, fp))) {
      args[nread - 1] = '\0';
      sp = args;

      while (*sp is ' ') sp++;
      while (*sp) {
        if (*sp is ' ' or *sp is '\n') break;
        if (*sp is '-' and (sp is args or *(sp - 1) is ' ')) {
          if (*(sp + 1) isnot '-') {
            if (is_long) {
              sp += 2;
              continue;
            }

            String.append_byte (arg, *sp++);
            continue;
          } else {
            String.append_with_len (arg, "--", 2);
            sp += 2;
            continue;
          }
        }

        if (*sp is ',') {
          if (arg->num_bytes > buflen) {
            if (Cstring.eq_n (ptr, arg->bytes + buflen, arglen)) {
              if (NULL is argar)
                argar = Alloc (sizeof (string));
              argar = Realloc (argar, sizeof (string) * (argidx + 1));
              argar[argidx] = String.new_with_len (arg->bytes + buflen, arg->num_bytes - buflen);
              argidx++;
              zs->arg_type = ZS_RLINE_ARG_IS_ARG;
            }

            String.clear_at (arg, buflen);
          }

          sp++;
          continue;
        }

        String.append_byte (arg, *sp++);
      }

      if (arg->num_bytes > buflen) {
        if (Cstring.eq_n (ptr, arg->bytes + buflen, arglen)) {
          if (NULL is argar)
            argar = Alloc (sizeof (string));
          argar = Realloc (argar, sizeof (string) * (argidx + 1));
          argar[argidx] = String.new_with_len (arg->bytes + buflen, arg->num_bytes - buflen);
          argidx++;
          zs->arg_type = ZS_RLINE_ARG_IS_ARG;
        }

        String.clear_at (arg, buflen);
      }
    }

    fclose (fp);
    ifnot (NULL is args)
      free (args);

    completion->ar = argar;
    completion->arlen = argidx;
    completion->idx = arglen;
    completion->prefix = buf;
    completion->prefix_len = buflen;
    completion->suffix = lptr;
    completion->suffix_len = lptrlen;

    int r = completion_pager (completion);

    if (r is -1) free (argar);
    if (r and zs->pager->c is ' ' and
        '=' is zs->completion_command->bytes[zs->completion_command->num_bytes - 1])
      zs->pager->c = ' ' - 1; // this is enough to ignore the extra space

    goto theend;
  }

  ptr = (char *) buf + buflen;
  ptrlen = buflen;

  is_filename = 0;
  while (ptrlen--) {
    char ch = *(ptr-1);
    if (('0' <= ch and ch <= '9') or
        ('a' <= ch and ch <= 'z') or
        ('A' <= ch and ch <= 'Z') or
        ch is '_' or ch is '.' or ch is '-') {
      ptr--;
      continue;
    }

    if (ch is '/') {
      is_filename = 1;
      ptr--;
      continue;
    }
    break;
  }

  if (-1 is ptrlen) {
    half_command: {}

    Command *it = zs->command_head;
    sp = (char *) buf;

    while (*sp) {
      if (*sp is '.') {
        sp++;

        string **ar = Alloc (sizeof (string));
        int idx = 0;

        while (it) {
          if (Cstring.eq_n (it->name, buf, buflen)) {
            ar[idx++] = String.new_with (it->name);
            if (sp - 1 is lptr) {
              lptr++; lptrlen--;
            }
          }

          it = it->next;
        }

        if (1 is idx) {
          zs->num_items = 1;
          Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);

          int cur_pos = ar[0]->num_bytes + 1;

          if (lptrlen)
            String.append_with_fmt (ar[0], " %s", lptr);
          else
            String.append_byte (ar[0], ' ');

          Rline.add_completion (rline, lc, ar[0]->bytes, cur_pos);
          zs->arg_type = ZS_RLINE_ARG_IS_COMMAND;

          String.release (ar[0]);
          free (ar);
          goto theend;
        }

        completion->ar = ar;
        completion->arlen = idx;
        completion->idx = 0;
        completion->prefix = NULL;
        completion->prefix_len = 0;
        completion->suffix = lptr;
        completion->suffix_len = lptrlen;

        if (-1 is completion_pager (completion))
          free (ar);

        goto theend;
      }

      sp++;
    }

    string **ar = Alloc (sizeof (string));
    size_t idx = 0;
    while (it) {
      char *name = it->name;

      if (Cstring.eq_n (name, buf, buflen)) {
        ar = Realloc (ar, sizeof (string) * (idx + 1));

        ar[idx] = String.new_with_len (name, buflen);
        name += buflen;
        while (*name) {
          String.append_byte (ar[idx], *name);
          if (*name++ is '.') break;
        }

        if (*(name - 1) is '.') {
          while (it->next) {
            if (Cstring.eq_n (ar[idx]->bytes, it->next->name, ar[idx]->num_bytes)) {
              it = it->next;
              continue;
            }
            break;
          }
        }

        idx++;
      }

      it = it->next;
    }

    if (1 is idx) {
      zs->num_items = 1;
      Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);

      int isnot_dot = ar[0]->bytes[ar[0]->num_bytes - 1] isnot '.';
      int cur_pos = ar[0]->num_bytes + isnot_dot;

      if (lptrlen)
        String.append_with_fmt (ar[0], " %s", lptr);
      else
        if (isnot_dot)
          String.append_byte (ar[0], ' ');

      ifnot (isnot_dot) {
        buflen = cur_pos;
        buf = Alloc (buflen + 1);
        Cstring.cp (buf, buflen + 1, ar[0]->bytes, buflen);
        buf_isallocated = 1;
        String.release (ar[0]);
        free (ar);
        goto half_command;
      }

      Rline.add_completion (rline, lc, ar[0]->bytes, cur_pos);

      zs->arg_type = ZS_RLINE_ARG_IS_COMMAND;

      String.release (ar[0]);
      free (ar);
      goto theend;
    }

    completion->ar = ar;
    completion->arlen = idx;
    completion->idx = buflen;
    completion->prefix = NULL;
    completion->prefix_len = 0;
    completion->suffix = lptr;
    completion->suffix_len = lptrlen;

    int r = completion_pager (completion);
    if (r is -1) free (ar);

    if (zs->pager->c is '\r' or zs->pager->c is '\t')
      goto theend;

    if (1 is r) {
      if (zs->completion_command->bytes[zs->completion_command->num_bytes-1] is '.') {
        it = zs->command_head;
        ar = Alloc (sizeof (string));
        idx = 0;

        while (it) {
          if (Cstring.eq_n (it->name, zs->completion_command->bytes, zs->completion_command->num_bytes)) {
            ar = Realloc (ar, sizeof (string) * (idx + 1));
            ar[idx++] = String.new_with (it->name + zs->completion_command->num_bytes);
          }
          it = it->next;
        }

        if (1 is idx) {
          Rline.release_completions (rline, lc);
          zs->num_items = 1;
          Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);

          ifnot (Cstring.eq_n (ar[0]->bytes, zs->completion_command->bytes, zs->completion_command->num_bytes)) { // duck
            String.append_with_len (zs->completion_command, ar[0]->bytes, ar[0]->num_bytes);
            int cur_pos = zs->completion_command->num_bytes;

            if (lptrlen)
              String.append_with_fmt (zs->completion_command, " %s", lptr);

            Rline.add_completion (rline, lc, zs->completion_command->bytes, cur_pos);
          } else {
            int cur_pos = ar[0]->num_bytes;
            if (lptrlen)
              String.append_with_fmt (ar[0], " %s", lptr);

            Rline.add_completion (rline, lc, ar[0]->bytes, cur_pos);
          }

          zs->arg_type = ZS_RLINE_ARG_IS_COMMAND;
          String.release (ar[0]);
          free (ar);
          goto theend;
        }

        Rline.release_completions (rline, lc);

        completion->ar = ar;
        completion->arlen = idx;
        completion->idx = 0;
        completion->prefix = zs->completion_command->bytes;
        completion->prefix_len = zs->completion_command->num_bytes;
        completion->suffix = lptr;
        completion->suffix_len = lptrlen;

        if (-1 is completion_pager (completion))
          free (ar);
      }
    }

    goto theend;
  }

  size_t diff = buflen - ptrlen - 1;
  buflen = (buflen - diff);
  ptrlen = diff;

  String.replace_with_len (arg, buf, buflen);

  Cstring.cp (ptrbuf, ptrlen + 1, ptr, ptrlen);

  dirname = Path.dirname (ptrbuf);
  basename = Path.basename (ptrbuf);

  is_filename = 0;

  if (Dir.is_directory (ptrbuf) or (File.is_lnk (ptrbuf) and Dir.lnk_is_directory (ptrbuf))) {
    if (ptrbuf[ptrlen-1] isnot DIR_SEP) {
      zs->num_items++;
      String.append_with_fmt (arg, "%s%c", ptrbuf, DIR_SEP);
      String.append_with_len (arg, lptr, lptrlen);
      Rline.add_completion (rline, lc, arg->bytes, arg->num_bytes - lptrlen);
      zs->arg_type = ZS_RLINE_ARG_IS_DIRECTORY;
      goto theend;
    }

    is_filename = 1;

    String.replace_with_len (zs->origin_directory, ptrbuf, ptrlen);

    dlist  = Dir.list (ptrbuf, DIRLIST_LNK_IS_DIRECTORY);
  } else ifnot (ptrlen) {
get_current: {}
    char *cwd = Dir.current ();
    if (NULL is cwd) goto theend;

    String.replace_with (zs->origin_directory, cwd);

    dlist = Dir.list (cwd, 0);
    free (cwd);
  } else {
    if (0 is Dir.is_directory (dirname) and (
        0 is File.is_lnk (dirname) and Dir.lnk_is_directory (dirname)))
      goto get_current;

    String.replace_with (zs->origin_directory, dirname);
    dlist = Dir.list (dirname, DIRLIST_LNK_IS_DIRECTORY);
  }

  if (NULL is dlist) goto theend;

  vstring_t *it = dlist->list->head;

  dirlen = bytelen (dirname);
  bname_len = bytelen (basename);

  if (1 is dirlen and *dirname is '.' and 0 is bname_len + is_filename) {
    string **ar = Alloc (sizeof (string));
    int idx = 0;
    if (argidx) {
      for (int i = 0; i < argidx; i++) {
        ar = Realloc (ar, sizeof (string) * (idx + 1));
        ar[idx] = argar[i];
        idx++;
      }

      free (argar);
    }

    while (it) {
      if (it->data->bytes[0] isnot '.') {
        ar = Realloc (ar, sizeof (string) * (idx + 1));
        ar[idx++] = String.new_with_len (it->data->bytes, it->data->num_bytes);
        zs->arg_type = ZS_RLINE_ARG_IS_FILENAME;
      }
      it = it->next;
    }

    completion->ar = ar;
    completion->arlen = idx;
    completion->idx = 0;
    completion->prefix = buf;
    completion->prefix_len = buflen;
    completion->suffix = lptr;
    completion->suffix_len = lptrlen;
    completion->quote_arg = 1;

    if (-1 is completion_pager (completion))
      free (ar);

    if (zs->pager->c is ' ') {
      if (zs->origin_directory->bytes[zs->origin_directory->num_bytes - 1] isnot DIR_SEP)
        String.append_byte (zs->origin_directory, DIR_SEP);

      String.append_with_len (zs->origin_directory, zs->completion_command->bytes,
          zs->completion_command->num_bytes);

      if (Dir.is_directory (zs->origin_directory->bytes))
        zs->pager->c = '\t';
    }

    goto theend;

  } else {
    string **ar = Alloc (sizeof (string));
    int lidx = bname_len;

    String.replace_with_len (arg, buf, buflen);

    int idx = 0;
    if (argidx) {
      for (int i = 0; i < argidx; i++) {
        ar = Realloc (ar, sizeof (string) * (idx + 1));
        ar[idx] = argar[i];
        idx++;
      }

      free (argar);
    }

    while (it) {
      ar = Realloc (ar, sizeof (string) * (idx + 1));
      if (is_filename) {
        lidx = 0;
        ifnot (idx) {
          ifnot (' ' is buf[buflen-1])
            String.append_byte (arg, ' ');
          String.append_with_len (arg, ptrbuf, ptrlen);
        }

        ar[idx++] = String.new_with_len (it->data->bytes, it->data->num_bytes);
      } else if (Cstring.eq_n (it->data->bytes, basename, bname_len)) {
        ifnot (idx)
          if ((dirlen is 1 and *dirname is DIR_SEP) or (dirlen isnot 1 and *dirname isnot '.') or
               Cstring.eq_n (dirname, "..", 2))
            String.append_with_fmt (arg, "%s%s", dirname,
              (dirname[dirlen-1] is DIR_SEP ? "" : DIR_SEP_STR));

        ar[idx++] = String.new_with_len (it->data->bytes, it->data->num_bytes);
      }

      it = it->next;
    }

    size_t prefixlen = arg->num_bytes;
    char prefix[prefixlen + 1];
    Cstring.cp (prefix, prefixlen + 1, arg->bytes, arg->num_bytes);

    completion->ar = ar;
    completion->arlen = idx;
    completion->idx = lidx;
    completion->prefix = prefix;
    completion->prefix_len = prefixlen;
    completion->suffix = lptr;
    completion->suffix_len = lptrlen;
    completion->quote_arg = 1;

    if (-1 is completion_pager (completion))
      free (ar);

    if (zs->pager->c is ' ') {
      if (zs->origin_directory->bytes[zs->origin_directory->num_bytes - 1] isnot DIR_SEP)
        String.append_byte (zs->origin_directory, DIR_SEP);

      String.append_with_len (zs->origin_directory, zs->completion_command->bytes,
          zs->completion_command->num_bytes);

      if (Dir.is_directory (zs->origin_directory->bytes))
        zs->pager->c = '\t';
    }
    goto theend;
  }

theend:
  ifnot (NULL is dlist) dlist->release (dlist);
  ifnot (NULL is dirname) free (dirname);
  if (buf_isallocated)
    free (buf);
  return zs->pager->c;
}

static int zs_on_input (const char *buf, string *prevLine, int *ch, int curpos, rlineCompletions *lc, void *userdata) {
  zs_t *zs = (zs_t *) userdata;
  rline_t *rline = zs->rline;

  int c = *ch;

  if (0 is curpos and buf[0] is '\0') {
    // complete command
    if (('A' <= c and c <= 'Z') or ('a' <= c and c <= 'z') or c is DIR_SEP) { // or ('a' <= c and c <= 'z') or c is '_') {
      char newbuf[2]; newbuf[0] = c; newbuf[1] = '\0';
      String.replace_with_len (prevLine, newbuf, 1);
      int r = zs_completion (newbuf, 1, lc, userdata);

      ifnot (zs->num_items) return -1;
      if (r is '\r' or r is '\t' or (zs->num_items is 1 and r is ' ')) {
        // also allow an extra space after the command
        *ch = r;
        return *ch;
      }

      *ch = 0;
      return 0;
    }

    if (c is '~') goto handle_tilda;

    return c;
  }

  // escape: (maybe vi mode?) (no probably not)
  if (c is ESCAPE_KEY)
    return c;

  // CTRL('/') or CTRL('-') instead of ALT('.')
  if (c is 037 and zs->numLastComp) {
    zs->num_items = 0;
    Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);

    const char *ptr = buf + curpos;
    size_t ptrlen = bytelen (ptr);

    string *arg = zs->arg;
    size_t len = bytelen (buf);
    len -= ptrlen;

    String.replace_with_len (arg, buf, len);

    LastComp *it = zs->lastcomp_head;
    while (it) {
      zs->num_items++;
      size_t llen = bytelen (it->lastcomp);
      String.append_with_len (arg, it->lastcomp, llen);
      String.append_with_len (arg, ptr, ptrlen);
      Rline.add_completion (rline, lc, arg->bytes, curpos + llen);
      zs->arg_type = ZS_RLINE_ARG_IS_LAST_COMPONENT;
      String.clear_at (arg, len);
      it = it->next;
    }

    ifnot (zs->num_items) return -1;
    return 0;
  }

  if (Cstring.eq_n (buf, "cd -", curpos + 1)) {
    const char *lptr = buf + curpos;
    size_t lptrlen = bytelen (lptr);
    size_t buflen = bytelen (buf) - 1 - lptrlen;

    string **ar = Alloc (sizeof (string));
    int idx = 1;
    while (1) {
      char *p = Sh.get.cdpath_at (zs->sh, idx);
      if (NULL is p) break;
      ar = Realloc (ar, sizeof (string) * (idx));
      ar[idx - 1] = String.new_with (p);
      idx++;
    }

    ifnot (idx - 1) {
      free (ar);
      return -1;
    }

    zs->num_items = 0;
    string *arg = zs->arg;
    String.clear (arg);

    completion_t *completion = zs->completion;
    completion->lc = lc;
    completion->curbuf = buf;
    completion->quote_arg = 0;
    zs->pager->c = 0;
    completion->ar = ar;
    completion->arlen = idx - 1;
    completion->idx = 0;
    completion->prefix = buf;
    completion->prefix_len = buflen;
    completion->suffix = lptr;
    completion->suffix_len = lptrlen;

    int r = completion_pager (completion);

    if (r is -1) {
      free (ar);
      return -1;
    }

    return zs->pager->c;
  }

  if (c is '-') {
    if (buf[curpos - 1] is ' ') {
      const char *ptr = buf + curpos;
      size_t buflen = bytelen (buf);
      size_t ptrlen = buflen - curpos;
      size_t len = curpos;

      char newbuf[buflen + 2];
      Cstring.cp (newbuf, buflen + 2, buf, len);
      newbuf[len] = c;
      Cstring.cp (newbuf + len + 1, ptrlen + 1, ptr, ptrlen);
      String.replace_with_len (prevLine, newbuf, 1);
      int r = zs_completion (newbuf, curpos + 1, lc, userdata);

      ifnot (zs->num_items) return -1;
      if (r is '\r' or r is '\t' or (zs->num_items is 1 and r is ' ')) {
        *ch = r;
        return *ch;
      }

      *ch = 0;
      return 0;
    }

  }

  if (c is '~') {
    if (buf[curpos - 1] is ' ' or 0 is curpos) {
handle_tilda:
      zs->num_items = 1;
      Rline.set.flags (rline, lc, RLINE_ACCEPT_ONE_ITEM);
      string *arg = zs->arg;

      const char *ptr = buf + curpos;
      size_t buflen = bytelen (buf);
      size_t ptrlen = buflen - curpos;
      size_t len = curpos;

      String.replace_with_len (arg, buf, len);
      char *home = Sys.get.env_value ("HOME");
      size_t homlen = bytelen (home);
      String.append_with_len (arg, home, homlen);
      String.append_byte (arg, DIR_SEP);
      int cur_pos = arg->num_bytes;
      String.append_with_len (arg, ptr, ptrlen);
      Rline.set.current (rline, lc, arg->bytes);
      Rline.set.curpos (rline, lc, cur_pos);
      Rline.refresh_line (rline, lc);
      String.clear_at (arg, len);
      return '\t';
    }
  }

  return -1;
}

static int zs_accept_one_item (const char *buf, rlineCompletions *lc, void *userdata) {
  zs_t *zs = (zs_t *) userdata;
  (void) zs; (void) buf; (void) lc;
//    if ('a' <= *buf and *buf <= 'z')
//      return -1;

  return 1;
}

static void zs_on_carriage_return (const char *buf, void *userdata) {
  zs_t *zs = (zs_t *) userdata;

  size_t len = bytelen (buf);
  ifnot (len) return;

  char *ptr = (char *) buf + len;
  while (*(ptr - 1) is ' ') ptr--;
  char *end = ptr;

  while (*(ptr - 1) isnot ' ') {
    ptr--;
    if (ptr is buf)
      break;
  }

  len = end - ptr;
  ifnot (len) return;

  LastComp *lastcomp = Alloc (sizeof (LastComp));
  lastcomp->lastcomp = Cstring.dup (ptr, len);

  zs->numLastComp++;

  if (zs->lastcomp_head is NULL) {
      zs->lastcomp_head = lastcomp;
      zs->lastcomp_head->next = NULL;
  } else {
    lastcomp->next = zs->lastcomp_head;
    zs->lastcomp_head = lastcomp;
    if (zs->numLastComp > MAXNUM_LAST_COMPONENTS) {
      zs->numLastComp--;
      LastComp *it = zs->lastcomp_head;
      int idx = 0;
      while (++idx < MAXNUM_LAST_COMPONENTS) it = it->next;
      free (it->next->lastcomp);
      free (it->next);
      it->next = NULL;
    }
  }
}

static char *zs_hints (const char *buf, int *color, int *bold, void *userdata) {
  (void) buf;
  zs_t *zs = (zs_t *) userdata;
  if (zs->num_items > 1) {
    Cstring.cp_fmt (zs->hint, MAXLEN_HINT + 1, " [%d items]", zs->num_items);
    *color = 33; *bold = 1;
    zs->num_items = 0;
    return zs->hint;
  }

  return NULL;
}

static void init_rline_commands (zs_t *this) {
  this->num_commands = 0;

  Command *comit;
  Command *head = Alloc (sizeof (Command));
  head->name = Cstring.dup ("exit", 4);
  comit = head;
  this->num_commands++;

  Command *next = Alloc (sizeof (Command));
  next->name = Cstring.dup ("cd", 2);
  comit->next = next;
  comit = next;
  this->num_commands++;

  next = Alloc (sizeof (Command));
  next->name = Cstring.dup ("pwd", 3);
  comit->next = next;
  comit = next;
  this->num_commands++;

  next = Alloc (sizeof (Command));
  next->name = Cstring.dup ("unsetenv", 8);
  comit->next = next;
  comit = next;
  this->num_commands++;

  next = Alloc (sizeof (Command));
  next->name = Cstring.dup ("rehash", 6);
  comit->next = next;
  comit = next;
  this->num_commands++;

  next = Alloc (sizeof (Command));
  next->name = Cstring.dup ("repeat", 6);
  comit->next = next;
  comit = next;
  this->num_commands++;

  char *flags = NULL;
  dirlist_t *dlist = NULL; // silence clang
  dlist = Dir.list (this->comdir->bytes, 0);
  if (NULL is dlist) goto theend;

  size_t flagslen = 0;

  vstring_t *it = dlist->list->head;
  while (it) {
    if (it->data->bytes[it->data->num_bytes - 1] isnot DIR_SEP) {
      it = it->next;
      continue;
    }

    String.clear_at (it->data, -1);

    next = Alloc (sizeof (Command));

    next->name = Cstring.dup (it->data->bytes, it->data->num_bytes);

    size_t filelen = this->comdir->num_bytes + 1 + it->data->num_bytes + sizeof ("/flags");
    char file[filelen + 1];
    Cstring.cp_fmt (file, filelen + 1, "%s/%s/flags", this->comdir->bytes, it->data->bytes);

    next->flags = 0;

    if (0 is access (file, F_OK|R_OK)) {
      FILE *fp = fopen (file, "r");
      if (NULL isnot fp) {
        ssize_t nread = getline (&flags, &flagslen, fp);
        if (-1 isnot nread) {
          flags[nread - 1] = '\0';
          next->flags = atoi (flags);
        }
        fclose (fp);
      }
    }

    comit->next = next;
    comit = next;
    it = it->next;
    this->num_commands++;
  }

theend:
  ifnot (NULL is flags) free (flags);
  ifnot (NULL is dlist) dlist->release (dlist);
  comit->next = NULL;
  this->command_head = head;
}

static void deinit_commands (Command *it) {
  while (it) {
    Command *tmp = it->next;
    free (it->name);
    free (it);
    it = tmp;
  }
}

static void deinit_lastcomp (LastComp *it) {
  while (it) {
    LastComp *tmp = it->next;
    free (it->lastcomp);
    free (it);
    it = tmp;
  }
}

static zs_t *zs_init_rline (void) {
  zs_t *zs = Alloc (sizeof (zs_t));
  rline_t *this = Rline.new ();

  char *datadir = Sys.get.env_value ("DATADIR");
  zs->comdir = String.new_with_fmt ("%s/zs/commands", datadir);

  size_t len = bytelen (datadir) + sizeof ("/zs/.zs_history");
  char histfile[len + 1];
  Cstring.cp_fmt (histfile, len + 1, "%s/zs/.zs_history", datadir);

  Rline.history.set.file (this, histfile);
  Rline.history.load (this);

  zs->arg = String.new (32);
  zs->origin_directory = String.new (128);
  zs->completion_command = String.new (32);

  ifnot (getuid ())
    Rline.set.prompt (this, DEFAULT_ROOT_PROMPT);
  else
    Rline.set.prompt (this, DEFAULT_USER_PROMPT);

  init_rline_commands (zs);

  Rline.set.hints_cb (this, zs_hints, zs); // UNUSED
  Rline.set.completion_cb (this, zs_completion, zs);
  Rline.set.on_input_cb (this, zs_on_input);
  Rline.set.on_carriage_return_cb (this, zs_on_carriage_return);
  Rline.set.accept_one_item_cb (this, zs_accept_one_item);

  zs->rline = this;

  zs->lastcomp_head = NULL;
  zs->numLastComp = 0;
  zs->completion = Alloc (sizeof (completion_t));
  zs->completion->zs = zs;
  zs->completion->rline = this;
  zs->completion->arg = zs->arg;
  return zs;
}

static int zs_commands (zs_t *this, sh_t *sh, const char *line, int *retval) {
  (void) sh;

  if (Cstring.eq (line, "rehash")) {
    deinit_commands (this->command_head);
    init_rline_commands (this);
    *retval = 0;
    return ZS_COMMAND;
  }

  return ZS_NO_COMMAND;
}

static zs_t *init (sh_t *sh) {
  zs_t *this = zs_init_rline ();

  this->sh = sh;

  this->term = Term.new ();
  this->term->mode = 's';
  Term.init_size (this->term, &this->term->num_rows, &this->term->num_cols);

  this->pager = Alloc (sizeof (pager_t));
  this->pager->term = this->term;
  this->pager->video = video_new (1, this->term->num_rows, this->term->num_cols);
  this->pager->buf = Alloc (sizeof (Buf));
  this->pager->buf->term = this->term;
  this->pager->buf->video = this->pager->video;
  this->pager->buf->tmp_buf = String.new (this->term->num_cols);
  this->pager->buf->empty_line = String.new (this->term->num_cols);
  for (size_t i = 0; i < (size_t) this->pager->buf->num_cols; i++)
    String.append_byte (this->pager->buf->empty_line, ' ');
  return this;
}

static int zs_interactive (sh_t *this) {
  int retval = OK;
  char *line;

  zs_t *zs = init (this);

  rline_t *rline = zs->rline;

  for (;;) {
    errno = 0;
    line = Rline.edit (rline);
    if (line is NULL) {
      if (errno is EAGAIN)
        continue;
       break;
    }

    char *save_line = line;

    if (ZS_COMMAND is zs_commands (zs, this, line, &retval))
      goto post_command;

    signal (SIGINT, SIG_IGN);
    retval = Sh.exec (this, line);
    if (retval < 0) {
      char *err = Sh.get.error (this);
      if (*err) fprintf (stderr, "%s\n", err);
    }

    signal (SIGINT, SIG_DFL);

    post_command:
    zs->last_retval = retval;
    line = save_line;

    Rline.history.add (rline, line);
    free (line);
    Sh.release_list (this);

    if (Sh.should_exit (this)) break;
  }

  Rline.history.save (rline);
  Rline.history.release (rline);
  Rline.release (rline);
  String.release (zs->comdir);
  String.release (zs->arg);
  String.release (zs->origin_directory);
  String.release (zs->completion_command);
  deinit_commands (zs->command_head);
  deinit_lastcomp (zs->lastcomp_head);
  Term.release (&zs->term);
  pager_release (zs->pager);
  free (zs->completion);
  free (zs);
  return retval;
}

int main (int argc, char **argv) {
  __INIT__(io);
  __INIT__(sh);
  __INIT__(sys);
  __INIT__(dir);
  __INIT__(path);
  __INIT__(file);
  __INIT__(error);
  __INIT__(rline);
  __INIT__(string);
  __INIT__(vstring);
  __INIT__(cstring);
  __INIT__(ustring);
  __INIT__(term);

  Sys.init_environment (SysEnvOpts());

  char dir[MAXLEN_DIR + 1]; dir[0] = '\0';
  char fname[MAXLEN_DIR + 1]; fname[0] = '\0';
  char command[MAXLEN_COMMAND_LINE + 1]; command[0] = '\0';

  int retval = 0;

  argc--; argv++;

  sh_t *this = NULL;
  char *curdir = NULL;
  int is_command = 0;
  size_t comlen = 0;
  size_t fnamelen = 0;

  int idx = 0;
  for (int i = idx; i < argc; i++) {
    ifnot (is_command) {
      if (Cstring.eq_n (argv[i], "--chdir=", 8)) {
        size_t dirlen = bytelen (argv[i] + 8);
        if (dirlen > MAXLEN_DIR) {
          Stderr.print_fmt ("--chdir=%s, path name is too long", argv[i] + 8);
          retval = 1;
          goto theend;
        }

        Cstring.cp (dir, MAXLEN_DIR + 1, argv[i] + 8, dirlen);
        continue;
      }

      if (Cstring.eq (argv[i], "-c")) {
        is_command = 1;
        continue;
      }

      fnamelen = bytelen (argv[i]);

      if (fnamelen > MAXLEN_DIR) {
        Stderr.print_fmt ("%s, path name is too long", argv[i]);
        retval = 1;
        goto theend;
      }

      Cstring.cp (fname, MAXLEN_DIR + 1, argv[i], fnamelen);
      idx = i;
      break;
    }

    if (comlen) {
      Cstring.cat (command, MAXLEN_COMMAND_LINE + 1, " ");
      comlen++;
    }

    comlen += bytelen (argv[i]);
    if (comlen > MAXLEN_COMMAND_LINE) {
      Stderr.print_fmt ("comands are too long too fit");
      retval = 1;
      goto theend;
    }

    Cstring.cat (command, MAXLEN_COMMAND_LINE + 1, argv[i]);
  }

  this = Sh.new ();

  curdir = Dir.current ();
  if (NULL is curdir) {
    Stderr.print ("cannot determinate current working directory\n");
    retval = 1;
    goto theend;
  }

  if (dir[0]) {
    if (chdir (dir) is -1) {
      Stderr.print_fmt ("%s, can not change to this directory, %s\n",
          dir, Error.errno_string (errno));
      retval = 1;
      goto theend;
    }

    setenv ("PWD", dir, 1);
  } else
    setenv ("PWD", curdir, 1);

  ifnot (comlen + fnamelen) {
    retval = zs_interactive (this);
    goto get_exit_val;
  }

  if (comlen) {
    retval = Sh.exec (this, command);
    goto get_exit_val;
  }

  retval = Sh.exec_file (this, fname);

  if (retval is NOTOK) {
    char *err = Sh.get.error (this);
    if (*err) fprintf (stderr, "%s\n", err);
  }

  get_exit_val:
  if (retval is NOTOK)
    retval = 1;
  else
    retval = Sh.get.exit_val (this);

theend:
  ifnot (NULL is curdir)
    free (curdir);

  __deinit_sys__ ();

  ifnot (NULL is this)
    Sh.release (this);

  return retval;
}
