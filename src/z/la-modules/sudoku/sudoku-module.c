/*
  https://github.com/vaithak/Sudoku-Generator
  too many thanks
 */

#define REQUIRE_STDIO
#define REQUIRE_TIME
#define REQUIRE_TERMIOS

#define REQUIRE_STD_MODULE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_TERM_TYPE    DECLARE
#define REQUIRE_VIDEO_TYPE   DECLARE
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

#include <z/sudoku.h>

MODULE(sudoku);

#define GET_OPT_MAX_MISTAKES(defval) ({                                         \
  VALUE _v_max_mistakes = La.get.qualifier (this, "max_mistakes", INT(defval)); \
  ifnot (IS_INT(_v_max_mistakes))                                               \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting an integer qualifier");               \
  AS_INT(_v_max_mistakes);                                                      \
})

#define DEF_MAXNUM_MISTAKES 3

#define SEPARATOR    "│"
#define UPPER_LINE   " ┌─────┬─────┬─────┐"
#define CENTRAL_LINE " ├─────┼─────┼─────┤"
#define LAST_LINE    " └─────┴─────┴─────┘"

typedef struct Me {
  sudoku_t *sudoku;
  time_t    total;
  time_t    beg;
  time_t    end;
  int       visible;
  int       num_mistakes;
  int       max_mistakes;
  int       cells[9];
  int       grid[81];
  string   *buf;
  term_t   *term;
  video_t  *video;
} Me;


static VALUE sudoku_play (la_t *this) {
  (void) this;
  sudoku_t sudoku;
  Me My = (Me) {
    .sudoku = &sudoku,
    .total  = 0, .beg = 0, .end = 0, .visible = 0, .num_mistakes = 0,
    .buf   = NULL, .term  = NULL, .video  = NULL,
    .max_mistakes = GET_OPT_MAX_MISTAKES(DEF_MAXNUM_MISTAKES)
  };

play:
  sudoku_init (My.sudoku);
  sudoku_generate (My.sudoku);
  My.visible = My.beg = My.end = My.total = My.num_mistakes = 0;
  memset (My.grid, 0, sizeof (My.grid));
  memset (My.cells, 0, sizeof (My.cells));

  for (int i = 0, idx = 0; i < 9; i++)
    for (int j = 0; j < 9; j++)
      My.grid[idx++] = My.sudoku->grid[i][j];

  if (My.term is NULL) { My.term = Term.new (); Term.set (My.term); }

  int rows = My.term->num_rows;
  int cols = My.term->num_cols;

  if (My.video is NULL) My.video = Video.new (1, rows, cols, 1, 1);

  if (My.buf is NULL) My.buf = String.new (cols);  else String.clear (My.buf);

  int row = 0;

  String.append_with_fmt (My.buf, " Difficulty: %d", My.sudoku->difficultyLevel);
  Video.set.row_with (My.video, row++, My.buf->bytes);

  String.replace_with (My.buf, UPPER_LINE);
  Video.set.row_with (My.video, row++, My.buf->bytes);
  String.replace_with_fmt (My.buf, " %s", SEPARATOR);

  int ind = 0;

  int i, j, k, c;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      for (int l = 0; l < 3; l++) {
        for (k = 0; k < 3; k++) {
          c = My.grid[ind++];
          ifnot (c)
            String.append_byte (My.buf, ' ');
          else {
            String.append_byte (My.buf, c + '0');
            My.visible++;
          }

          ifnot (k is 2)
            String.append_byte (My.buf, ' ');
        }

        String.append_with (My.buf, SEPARATOR);
      }

      Video.set.row_with (My.video, row++, My.buf->bytes);
      ifnot (j is 2) {
        String.replace_with_fmt (My.buf, " %s", SEPARATOR);
      }
    }

    ifnot (i is 2) {
      String.replace_with (My.buf, CENTRAL_LINE);
      Video.set.row_with (My.video, row++, My.buf->bytes);
      String.replace_with_fmt (My.buf, " %s", SEPARATOR);
    }
  }

  String.replace_with (My.buf, LAST_LINE);
  Video.set.row_with (My.video, row++, My.buf->bytes);

  int first_row  = 3;
  int last_row   = first_row + 10;
  int msg_line   = rows - 1;
  int info_line  = last_row + 1;
  int actual_row = 0;
  int actual_col = 0;
  int last_col   = 19;
  int first_col  = 3;
  int num_rows   = 9;
  int line_rows[] = {
    first_row,     first_row + 1, first_row + 2,
    first_row + 4, first_row + 5, first_row + 6,
    first_row + 8, first_row + 9, first_row + 10};
  row = first_row;
  int col = first_col;
  int found = 0;
  int set_pos = 1;
  int msg_send = 0;
  int is_error = 0;
  int insert   = 0;

  String.replace_with_fmt (My.buf, " Cells left %d, Mistakes %d", 81 - My.visible, My.num_mistakes);
  Video.set.row_with (My.video, info_line, My.buf->bytes);
  Video.draw.rows_from_to (My.video, 1, rows);

  theloop:
    if (My.visible is 81 and msg_send is 0) {
      msg_send = 1; set_pos = 1;
      Video.set.row_with (My.video, msg_line, "You won!");
      Video.draw.row_at (My.video, msg_line + 1);
      Term.cursor.set_pos (My.term, row, col);
    }

    if (set_pos) {
      Term.cursor.set_pos (My.term, row, col);
      set_pos = 0;
    }

    c = IO.input.getkey (0);

    if (msg_send) {
      msg_send = 0;
      Video.set.row_with (My.video, msg_line, "");
      Video.draw.row_at (My.video, msg_line + 1);
      Term.cursor.set_pos (My.term, row, col);
    }

    if (My.num_mistakes > My.max_mistakes and c isnot 'q' and c isnot 'r')
     goto theloop;

    switch (c) {
      case 'q': goto theend;
      case 'r': goto play;

      case 'j':
      case ARROW_DOWN_KEY:
        if (row is last_row) goto theloop;
        set_pos = 1; row++; actual_row++; found = 0;
        for (i = 0; i < num_rows; i++) {
          if (row is line_rows[i]) { found = 1; break;}
        }

        ifnot (found) row++;
        goto hl;

      case 'k':
      case ARROW_UP_KEY:
        if (row is first_row) goto theloop;
        set_pos = 1; row--; actual_row--; found = 0;
        for (i = 0; i < num_rows; i++) {
          if (row is line_rows[i]) { found = 1; break;}
        }

        ifnot (found) row--;
        goto hl;

      case 'l':
      case ARROW_RIGHT_KEY:
        if (col + 2 <= last_col) {
          actual_col++; col += 2; set_pos = 1;
          goto hl;
        }
        goto theloop;

      case 'h':
      case ARROW_LEFT_KEY:
        if (col - 2 >= first_col) {
          actual_col--; col -= 2; set_pos = 1;
          goto hl;
        }
        goto theloop;

      case '1'...'9':
        if (My.sudoku->grid[actual_row][actual_col]) goto theloop;
        if (My.sudoku->solved[actual_row][actual_col] isnot c - '0') {
          String.replace_with_fmt (My.buf, "%c: wrong", c);
          My.num_mistakes++;
          if (My.num_mistakes > My.max_mistakes)
            String.replace_with_fmt (My.buf, "[%c] is wrong, you lost", c);
          else
            String.replace_with_fmt (My.buf, "[%c] is wrong, left %d tries", c, My.max_mistakes - My.num_mistakes);

          is_error = 1;
          goto send_msg;
        }

        My.sudoku->grid[actual_row][actual_col] = c - '0';
        My.grid[(actual_row * 9) + actual_col] = c - '0';
        goto set_line;

      default: goto theloop;
    }

set_line:
  ind = actual_row * 9;
  String.replace_with_fmt (My.buf, " %s", SEPARATOR);
  for (int l = 0; l < 3; l++) {
    for (k = 0; k < 3; k++) {
      c = My.grid[ind++];
      ifnot (c)
        String.append_byte (My.buf, ' ');
      else
        String.append_byte (My.buf, c + '0');

      ifnot (k is 2)
        String.append_byte (My.buf, ' ');
    }

    String.append_with (My.buf, SEPARATOR);
  }

  Video.set.row_with (My.video, row - 1, My.buf->bytes);
  Video.draw.row_at (My.video, row);
  My.visible++;
  insert = 1;

set_info:
  String.replace_with_fmt (My.buf, " Cells left %d, Mistakes %d", 81 - My.visible, My.num_mistakes);
  Video.set.row_with (My.video, info_line, My.buf->bytes);
  Video.draw.row_at (My.video, info_line + 1);

  set_pos = 1;
  ifnot (insert) goto theloop;
  insert = 0;

hl:
  c = My.sudoku->grid[actual_row][actual_col];
  ifnot (c) goto theloop;

  ind = 0;
  int cc;
  String.replace_with_fmt (My.buf, " %s", SEPARATOR);
  int lrow = first_row - 1;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      for (int l = 0; l < 3; l++) {
        for (k = 0; k < 3; k++) {
          cc = My.grid[ind++];
          ifnot (cc)
            String.append_byte (My.buf, ' ');
          else
            String.append_byte (My.buf, cc + '0');

          ifnot (k is 2)
            String.append_byte (My.buf, ' ');
        }

        String.append_with (My.buf, SEPARATOR);
      }

      Video.set.row_with (My.video, lrow++, My.buf->bytes);

      ifnot (j is 2)
        String.replace_with_fmt (My.buf, " %s", SEPARATOR);
    }

    ifnot (i is 2) {
      String.replace_with (My.buf, CENTRAL_LINE);
      Video.set.row_with (My.video, lrow++, My.buf->bytes);
      String.replace_with_fmt (My.buf, " %s", SEPARATOR);
    }
  }

  Video.draw.rows_from_to (My.video, first_row, last_row);

  int index = 1;
  vstring_t *v = My.video->head->next->next;
  while (v and index++ < last_row - 1) {
    char *beg = v->data->bytes;
    char *p = beg + 4;

    if (*p < 0) { v = v->next; continue; }

    while (*p) {
      if (*p < 0) { p += 3; continue; }

      if (*p is c + '0') {
        int fidx = p - beg;
        Video.row_hl_at (My.video, index, 31, fidx, fidx + 1);
        break;
      }
      p++;
    }

    v = v->next;
  }

  goto theloop;

send_msg:
  Video.set.row_with (My.video, msg_line, My.buf->bytes);
  Video.draw.row_at (My.video, msg_line + 1);
  set_pos = 1;
  msg_send = 1;

  if (is_error) { is_error = 0; goto set_info; }

  goto theloop;

theend:
  Video.release (My.video);
  Term.reset (My.term);
  Term.release (&My.term);
  String.release (My.buf);
  return OK_VALUE;
}

static VALUE Sudoku_generate (la_t *this) {
  int print = GET_OPT_PRINT();

  sudoku_t sudoku;
  sudoku_init (&sudoku);
  sudoku_generate (&sudoku);

  if (print) {
    sudoku_print (&sudoku);
    return OK_VALUE;
  }

  ArrayType *grid = ARRAY_NEW(INTEGER_TYPE, 81);
  integer *gridar = (integer *) AS_ARRAY(grid->value);
  ArrayType *solved = ARRAY_NEW(INTEGER_TYPE, 81);
  integer *solvedar = (integer *) AS_ARRAY(solved->value);

  for (int i = 0, idx = 0; i < 9; i++)
    for (int j = 0; j < 9; j++)
      gridar[idx++] = sudoku.grid[i][j];

  for (int i = 0, idx = 0; i < 9; i++)
    for (int j = 0; j < 9; j++)
      solvedar[idx++] = sudoku.solved[i][j];

  Vmap_t *m = Vmap.new (8);
  La.map.set_value (this, m, "grid",       ARRAY(grid), 1);
  La.map.set_value (this, m, "solved",     ARRAY(solved), 1);
  La.map.set_value (this, m, "difficulty", INT(sudoku.difficultyLevel), 1);
  return MAP(m);
}

public int __init_sudoku_module__ (la_t *this) {
  __INIT_MODULE__(this);

  __INIT__(io);
  __INIT__(term);
  __INIT__(video);

  LaDefCFun lafuns[] = {
    { "sudoku_generate",  PTR(Sudoku_generate), 0 },
    { "sudoku_play",      PTR(sudoku_play), 0 },
    { NULL, NULL_VALUE, 0}
  };

  int err;
  for (int i = 0; lafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, lafuns[i].name, LA_CFUNC (lafuns[i].nargs), lafuns[i].val)))
      return err;
  }

  const char evalString[] = EvalString (
    public var Sudoku = {
      generate : sudoku_generate,
      play     : sudoku_play,
    }
  );

  err = La.eval_string (this, evalString);
  if (err isnot LA_OK) return err;
  return LA_OK;
}

public void __deinit_sudoku_module__ (la_t *this) {
  (void) this;
  return;
}
