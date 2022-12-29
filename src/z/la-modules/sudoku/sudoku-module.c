// add quality

/* La
  add:
     on object do this

  consider:
    on for the 'switch' implementation
    or
    when
*/

/* C
  add ON macro for a switch statement
*/

/* xwm
  extend:
    return action to previous window when quiting an application
      (e.g., alsamixer)
*/

#define REQUIRE_STDIO
#define REQUIRE_TIME
#define REQUIRE_TERMIOS

#define REQUIRE_STD_MODULE
#define REQUIRE_IO_TYPE      DECLARE
#define REQUIRE_TERM_TYPE    DECLARE
#define REQUIRE_FILE_TYPE    DECLARE
#define REQUIRE_DIR_TYPE     DECLARE
#define REQUIRE_VIDEO_TYPE   DECLARE
#define REQUIRE_KEYS_MACROS

#include <z/cenv.h>

#include <z/sudoku.h>

MODULE(sudoku);

#define SUDOKU_QUIT         0
#define SUDOKU_PLAY         1
#define SUDOKU_REPLAY       2

#define SUDOKU_DATADIR_ERR -2
#define SUDOKU_SAVED        0

#define SEPARATOR    "│"
#define UPPER_LINE   " ┌─────┬─────┬─────┐"
#define CENTRAL_LINE " ├─────┼─────┼─────┤"
#define LAST_LINE    " └─────┴─────┴─────┘"

#define SUDOKU_SEND_MSG() do {                            \
  Video.set.row_with (My->video, My->msg_line, My->buf->bytes); \
  Video.draw.row_at (My->video, My->msg_line + 1);        \
  set_pos = 1;                                            \
  My->msg_send = 1;                                       \
  if (is_error) { is_error = 0; goto set_info; }          \
} while (0)

#define SUDOKU_CLEAR_MSG() do {                           \
  Video.set.row_with (My->video, My->msg_line, "");       \
  Video.draw.row_at (My->video, My->msg_line + 1);        \
  Term.cursor.set_pos (My->term, row, col);               \
  My->msg_send = 0;                                       \
} while (0)

#define GET_ANSWER() ({                                   \
  Term.cursor.set_pos (My->term, My->msg_line + 1,        \
                                 My->buf->num_bytes + 1); \
  int _c = IO.input.getkey (0);                           \
  _c;                                                     \
})

#define GET_OPT_DATADIR() ({                                              \
  string *_datadir_ = NULL;                                               \
  VALUE _v_datadir = La.get.qualifier (this, "datadir", NULL_VALUE);      \
  if (IS_STRING(_v_datadir))                                              \
    _datadir_ = AS_STRING(_v_datadir);                                    \
  else if (IS_NULL(_v_datadir) == 0)                                      \
    THROW(LA_ERR_TYPE_MISMATCH, "datadir: awaiting a string or null qualifier"); \
  _datadir_;                                                              \
})

#define GET_OPT_SUDOKU() ({                                               \
  Vmap_t *_sudoku_ = NULL;                                                \
  VALUE _v_sudoku  = La.get.qualifier (this, "sudoku", NULL_VALUE);       \
  if (IS_MAP(_v_sudoku))                                                  \
    _sudoku_ = AS_MAP(_v_sudoku);                                         \
  else if (IS_NULL(_v_sudoku) == 0)                                       \
    THROW(LA_ERR_TYPE_MISMATCH, "sudoku: awaiting a map or null qualifier"); \
  _sudoku_;                                                               \
})

#define GET_OPT_NAME() ({                                              \
  string *_name_ = NULL;                                               \
  VALUE _v_name = La.get.qualifier (this, "name", NULL_VALUE);         \
  if (IS_STRING(_v_name))                                              \
    _name_ = AS_STRING(_v_name);                                       \
  else if (IS_NULL(_v_name) == 0)                                      \
    THROW(LA_ERR_TYPE_MISMATCH, "name: awaiting a string or null qualifier"); \
  _name_;                                                              \
})

/*
#define GET_OPT_SUDOKU(_len) ({                                     \
  *_len = 0;                                                              \
  integer *_sudoku_ = NULL;                                                \
  VALUE _v_sudoku = La.get.qualifier (this, "sudoku", NULL_VALUE);  \
  if (IS_INT_ARRAY(_v_sudoku))                                                 \
    _sudoku_ = AS_STRING_ARRAY(_v_sudoku, _len);                            \
  else if (IS_NULL(_v_sudoku) == 0)                                        \
    THROW(LA_ERR_TYPE_MISMATCH, "awaiting a string or null qualifier");   \
  _sudoku_;                                                                \
})
*/
typedef struct loaded_puzzle {
  int     id;
  time_t  when;
  string *name;
  int     secs;
  int     points;
  struct loaded_puzzle *next;
} loaded_puzzle;

typedef struct Me {
  sudoku_t *sudoku;
  time_t    total;
  time_t    beg;
  time_t    end;
  int       visible;
  int       quality;
  int       difficulty_level;
  int       num_mistakes;
  int       cells[9];
  int       grid[81];
  int       current_grid[9][9];
  int       num_rows;
  int       num_cols;
  int       msg_line;
  int       points;
  int       msg_send;
  int       id_idx;
  string   *buf;
  string   *filename;
  string   *name;
  string   *datadir;
  term_t   *term;
  video_t  *video;
  loaded_puzzle *loaded_puzzle;
} Me;

static int sudoku_save (Me *My, time_t postfix) {
  if (My->datadir is NULL) return SUDOKU_DATADIR_ERR;

  ifnot (File.exists (My->datadir->bytes))
    if (NOTOK is Dir.make (My->datadir->bytes, 0755, DirOpts()))
      return SUDOKU_DATADIR_ERR;

  ifnot (Dir.is_directory (My->datadir->bytes))
    return SUDOKU_DATADIR_ERR;

  if (My->filename is NULL)
    My->filename = String.new (8);
  else
    String.clear (My->filename);

  String.append_with (My->filename, My->datadir->bytes);
  String.append_byte (My->filename, '/');

  int is_solved = My->visible is 81;

  String.append_byte (My->filename, is_solved + '0');
  int difficulty = My->sudoku->difficultyLevel > 1600 ? 9 :
                   My->sudoku->difficultyLevel > 1400 ? 8 :
                   My->sudoku->difficultyLevel > 1200 ? 7 :
                   My->sudoku->difficultyLevel > 1000 ? 6 :
                   My->sudoku->difficultyLevel > 800  ? 5 :
                   My->sudoku->difficultyLevel > 600  ? 4 :
                   My->sudoku->difficultyLevel > 400  ? 3 :
                   My->sudoku->difficultyLevel > 200  ? 2 : 1;

  String.append_byte (My->filename, difficulty + '0');
  String.append_byte (My->filename, My->quality + '0');
  String.append_with_fmt (My->filename, ".%ld", postfix);

  String.replace_with_fmt (My->buf,  "return {\n      when : %ld\n    solved : [", postfix);
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      String.append_byte (My->buf, My->sudoku->solved[i][j] + '0');
      if (i + j isnot 16)
        String.append_with_len (My->buf,  ", ", 2);
    }

  String.append_with (My->buf,  "  ]\n      grid : [");
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      String.append_byte (My->buf, My->current_grid[i][j] + '0');
      if (i + j isnot 16)
        String.append_with_len (My->buf,  ", ", 2);
    }

  String.append_with (My->buf, "]\n    puzzle : [");

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      String.append_byte (My->buf, My->sudoku->grid[i][j] + '0');
      if (i + j isnot 16)
        String.append_with_len (My->buf,  ", ", 2);
    }

  String.append_with_fmt (My->buf, "]\n      name : \"%s\"\n", My->name->bytes);

  String.append_with (My->buf, "    points : ");
  if (is_solved)
    String.append_with_fmt (My->buf, "%d\n   seconds : %d\n  mistakes : %d\n",
      My->points, My->total, My->num_mistakes);
  else
    String.append_with (My->buf, "0\n   seconds : 0\n  mistakes : 0\n");

  String.append_with_fmt (My->buf, "   quality : %d\ndifficulty : %d\n",
    My->quality, My->sudoku->difficultyLevel);

  String.append_with_len (My->buf, "}\n", 2);

  File.write (My->filename->bytes, My->buf->bytes, My->buf->num_bytes);

  return SUDOKU_SAVED;
}

static int sudoku_loop (Me *My) {
  int retval = 0;
  int row, i, ind, j, k, c;

  int first_row  = 3;
  int last_row   = first_row + 10;
  int info_line  = last_row + 1;
  int actual_row = 0;
  int actual_col = 0;
  int last_col   = 19;
  int first_col  = 3;
  int num_rows   = 9;
  int line_rows[] = {
    first_row,     first_row + 1, first_row + 2,
    first_row + 4, first_row + 5, first_row + 6,
    first_row + 8, first_row + 9, first_row + 10
  };

  row = first_row;
  int col = first_col;
  int found = 0;
  int set_pos = 1;
  int is_error = 0;
  int insert   = 0;
  int min, sec, total;
  int pause = 0;
  int last_c = 0;

  String.replace_with_fmt (My->buf, " Cells left %d, Mistakes %d", 81 - My->visible, My->num_mistakes);
  Video.set.row_with (My->video, info_line, My->buf->bytes);
  Video.draw.rows_from_to (My->video, 1, My->num_rows);

  time_t now;
  My->beg = time (NULL);
  goto hl;

  theloop:
    if (My->visible is 81) {
      if (0 is My->msg_send and My->end) {
        My->total += My->end - My->beg;
        My->points = My->total;
        for (int m = 1; My->num_mistakes and m <= My->num_mistakes; m++)
          My->points += (100 + ((m * m) * 10));

        My->end = 0;
        min = My->total / 60;
        sec = My->total % 60;
        String.replace_with_fmt (My->buf,
         "Solved in %d minutes and %d seconds with %d mistakes: %d points",
          min, sec, My->num_mistakes, My->points);

        SUDOKU_SEND_MSG();
      }
    }

    if (set_pos) {
      Term.cursor.set_pos (My->term, row, col);
      set_pos = 0;
    }

    get_key:

    c = IO.input.getkey (0);

    if (My->msg_send) SUDOKU_CLEAR_MSG();

    if (My->visible is 81) {
      if (c isnot 'q' and c isnot 'Q' and c isnot 'p' and
          c isnot 'r' and c isnot 's' and c isnot 'S' and
          c isnot CTRL('L'))
      goto get_key;
    }

    if (pause and c isnot ' ') goto theloop;

    switch (c) {
      case 'q':
        if (My->visible is 81) {
          retval = SUDOKU_QUIT;
          goto theend;
        }

        String.replace_with (My->buf, "game hasn't been finished, use Q to force");
        SUDOKU_SEND_MSG();
        goto theloop;

      case 'Q': goto theend;

      case 'p': {
        String.replace_with (My->buf, "New puzzle ganerated");
        SUDOKU_SEND_MSG();
        retval = SUDOKU_PLAY;
        goto theend;
      }

      case 'r': {
        String.replace_with (My->buf, "Replaying current puzzle");
        if (My->visible is 81) {
          SUDOKU_SEND_MSG();
          retval = SUDOKU_REPLAY;
          goto theend;
        } else {
          String.append_with (My->buf, "? yY/nN");
          SUDOKU_SEND_MSG();

          do {
            c = GET_ANSWER();

            switch (c) {
              case 'y':
              case 'Y':
                retval = SUDOKU_REPLAY;
                goto theend;

              case 'n':
              case 'N':
              goto theloop;
            }
          } while (1);
        }
      }

      case 'S':
        String.replace_with (My->buf, "Settings: q[uality]");
        SUDOKU_SEND_MSG();
        c = GET_ANSWER();

        switch (c) {
          case 'q':
            String.replace_with (My->buf, "set quality: 1, 2, 3, 4 or 5");
            SUDOKU_SEND_MSG();
            c = GET_ANSWER();
            switch (c) {
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              My->quality = c - '0';
              String.replace_with_fmt (My->buf, "quality set as: %c", c);
              SUDOKU_SEND_MSG();
              break;
            }

            break;
          }

        Term.cursor.set_pos (My->term, row, col);
        goto get_key;

      case 's':
        if (SUDOKU_SAVED is sudoku_save (My, time (NULL))) {
          String.replace_with_fmt (My->buf, "puzzle has been saved as %s", My->filename->bytes);
          SUDOKU_SEND_MSG();
        }
        goto theloop;

      case CTRL('L'):
        Video.draw.rows_from_to (My->video, 1, My->num_rows);
        Term.cursor.set_pos (My->term, row, col);
        goto theloop;

      case ' ':
        ifnot (pause) {
          My->total += time (NULL) - My->beg;
          My->beg = 0;
          pause = 1;
          String.replace_with (My->buf, "paused");
          SUDOKU_SEND_MSG();
          My->msg_send = 0;
        } else {
          pause = 0;
          SUDOKU_CLEAR_MSG();
          My->beg = time (NULL);
        }

        goto theloop;

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
          if (row is line_rows[i]) { found = 1; break; }
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

      case '.': c = last_c;
      // fallthrough

      case '1'...'9':
        if (My->cells[c - '0' - 1] is 9) {
          String.replace_with_fmt (My->buf, "%d has been completed", c - '0');
          SUDOKU_SEND_MSG();
          goto theloop;
        }

        if (My->current_grid[actual_row][actual_col]) goto theloop;

        if (My->sudoku->solved[actual_row][actual_col] isnot c - '0') {
          String.replace_with_fmt (My->buf, "%c is a wrong number for that cell", c);
          My->num_mistakes++;
          is_error = 1;
          SUDOKU_SEND_MSG();
          goto theloop;
        }

        My->current_grid[actual_row][actual_col] = c - '0';
        My->grid[(actual_row * 9) + actual_col] = c - '0';
        My->cells[c - '0' - 1]++;
        My->visible++;
        last_c = c;

        if (My->visible is 81) My->end = time (NULL);

        if (My->cells[c - '0' - 1] is 9 and My->visible isnot 81) {
          String.replace_with_fmt (My->buf, "%d has been completed");
          SUDOKU_SEND_MSG();
        }

        goto set_line;

      default: goto theloop;
    }

set_line:
  ind = actual_row * 9;
  String.replace_with_fmt (My->buf, " %s", SEPARATOR);
  for (int l = 0; l < 3; l++) {
    for (k = 0; k < 3; k++) {
      c = My->grid[ind++];
      ifnot (c)
        String.append_byte (My->buf, ' ');
      else
        String.append_byte (My->buf, c + '0');

      ifnot (k is 2)
        String.append_byte (My->buf, ' ');
    }

    String.append_with (My->buf, SEPARATOR);
  }

  Video.set.row_with (My->video, row - 1, My->buf->bytes);
  Video.draw.row_at (My->video, row);
  insert = 1;

set_info:
  now = time (NULL);
  total = My->total + (now - My->beg);
  min = total / 60;
  sec = total % 60;
  String.replace_with_fmt (My->buf, " cells left %d, mistakes %d, in %d minutes and %d seconds",
    81 - My->visible, My->num_mistakes, min, sec);
  Video.set.row_with (My->video, info_line, My->buf->bytes);
  Video.draw.row_at (My->video, info_line + 1);

  set_pos = 1;
  ifnot (insert) goto theloop;
  insert = 0;

hl:
  c = My->current_grid[actual_row][actual_col];
  ifnot (c) goto theloop;

  ind = 0;
  int cc;
  String.replace_with_fmt (My->buf, " %s", SEPARATOR);
  int lrow = first_row - 1;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      for (int l = 0; l < 3; l++) {
        for (k = 0; k < 3; k++) {
          cc = My->grid[ind++];
          ifnot (cc)
            String.append_byte (My->buf, ' ');
          else
            String.append_byte (My->buf, cc + '0');

          ifnot (k is 2)
            String.append_byte (My->buf, ' ');
        }

        String.append_with (My->buf, SEPARATOR);
      }

      Video.set.row_with (My->video, lrow++, My->buf->bytes);

      ifnot (j is 2)
        String.replace_with_fmt (My->buf, " %s", SEPARATOR);
    }

    ifnot (i is 2) {
      String.replace_with (My->buf, CENTRAL_LINE);
      Video.set.row_with (My->video, lrow++, My->buf->bytes);
      String.replace_with_fmt (My->buf, " %s", SEPARATOR);
    }
  }

  Video.draw.rows_from_to (My->video, first_row, last_row);

  if (My->visible is 81) goto theloop;

  int index = 1;
  vstring_t *v = My->video->head->next->next;
  while (v and index++ < last_row - 1) {
    char *beg = v->data->bytes;
    char *p = beg + 4;

    if (*p < 0) { v = v->next; continue; }

    while (*p) {
      if (*p < 0) { p += 3; continue; }

      if (*p is c + '0') {
        int fidx = p - beg;
        Video.row_hl_at (My->video, index, 31, fidx, fidx + 1);
        break;
      }
      p++;
    }

    v = v->next;
  }

  if (My->cells[c - 1] is 9) {
     String.replace_with_fmt (My->buf, "%d has been completed", c);
     SUDOKU_SEND_MSG();
  }

  goto theloop;

theend:
  return retval;
}

static void sudoku_draw (Me *My) {
  int row = 0;

  String.append_with_fmt (My->buf, " Difficulty: %d", My->sudoku->difficultyLevel);
  Video.set.row_with (My->video, row++, My->buf->bytes);

  String.replace_with (My->buf, UPPER_LINE);
  Video.set.row_with (My->video, row++, My->buf->bytes);
  String.replace_with_fmt (My->buf, " %s", SEPARATOR);

  int ind = 0;

  int i, j, k, c;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      for (int l = 0; l < 3; l++) {
        for (k = 0; k < 3; k++) {
          c = My->grid[ind++];
          ifnot (c)
            String.append_byte (My->buf, ' ');
          else {
            String.append_byte (My->buf, c + '0');
            My->visible++;
          }

          ifnot (k is 2)
            String.append_byte (My->buf, ' ');
        }

        String.append_with (My->buf, SEPARATOR);
      }

      Video.set.row_with (My->video, row++, My->buf->bytes);
      ifnot (j is 2) {
        String.replace_with_fmt (My->buf, " %s", SEPARATOR);
      }
    }

    ifnot (i is 2) {
      String.replace_with (My->buf, CENTRAL_LINE);
      Video.set.row_with (My->video, row++, My->buf->bytes);
      String.replace_with_fmt (My->buf, " %s", SEPARATOR);
    }
  }

  String.replace_with (My->buf, LAST_LINE);
  Video.set.row_with (My->video, row++, My->buf->bytes);
}

static void sudoku_set_cells (Me *My) {
  memset (My->cells, 0, sizeof (My->cells));

  for (int i = 0; i < 81; i++) {
    int c = My->grid[i];
    ifnot (c) continue;
    My->cells[c - 1]++;
  }
}

static void sudoku_set_grid (Me *My, char grid[9][9]) {
  memset (My->grid, 0, sizeof (My->grid));

  for (int i = 0, idx = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      My->grid[idx++] = grid[i][j];
      My->current_grid[i][j] = grid[i][j];
    }
}

static VALUE init_me (la_t *this, Me *My) {
  if (My->term is NULL) {
    My->term = Term.new ();
    Term.set (My->term);
  }

  My->num_cols = My->term->num_cols;
  My->num_rows = My->term->num_rows;
  My->msg_line = My->num_rows - 1;

  if (My->video is NULL)
    My->video = Video.new (1, My->num_rows, My->num_cols, 1, 1);

  if (My->buf is NULL)
    My->buf = String.new (My->num_cols);

  string *name = GET_OPT_NAME();
  ifnot (NULL is name)
    My->name = String.dup (name);
  else
    My->name = String.new_with ("noname");

  My->datadir = NULL;
  string *datadir = GET_OPT_DATADIR();
  ifnot (NULL is datadir)
    My->datadir = String.dup (datadir);

  return OK_VALUE; // maybe it could be used in future
}

static VALUE sudoku_play (la_t *this) {
  sudoku_t sudoku;
  Me My = (Me) {
    .sudoku = &sudoku,
    .total  = 0, .beg = 0, .end = 0, .visible = 0, .num_mistakes = 0, .id_idx = 0,
    .buf = NULL, .datadir = NULL, .filename = NULL, .term  = NULL, .video  = NULL,
    .loaded_puzzle = NULL
  };

  init_me (this, &My);

  Vmap_t *m = GET_OPT_SUDOKU ();
  ifnot (NULL is m) {
    sudoku_init (My.sudoku);
    VALUE *v = Vmap.get (m, "difficulty");
    My.sudoku->difficultyLevel = AS_INT((*v));
    v = Vmap.get (m, "quality");
    My.quality = AS_INT((*v));

    v = Vmap.get (m, "grid");
    ArrayType *grid = (ArrayType *) AS_ARRAY((*v));
    integer *gridar = (integer *) AS_ARRAY(grid->value);
    for (int i = 0, idx = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        My.sudoku->grid[i][j] = gridar[idx++];

    for (int i = 0; i < 81; i++) My.grid[i] = gridar[i];

    v = Vmap.get (m, "solved");
    ArrayType *solved = (ArrayType *) AS_ARRAY((*v));
    integer *solvedar = (integer *) AS_ARRAY(solved->value);

    for (int i = 0, idx = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        My.sudoku->solved[i][j] = solvedar[idx++];

    goto init_grid;
  }

  for (;;) {
    sudoku_init (My.sudoku);
    sudoku_generate (My.sudoku);

    My.quality = 1;

    init_grid:
    sudoku_set_grid (&My, My.sudoku->grid);
    sudoku_set_cells (&My);

    My.visible = My.beg = My.end = My.total = My.num_mistakes = 0;

    String.clear (My.buf);

    sudoku_draw (&My);

    int r = sudoku_loop (&My);

    switch (r) {
      case SUDOKU_QUIT:   goto theend;
      case SUDOKU_REPLAY: goto init_grid;
      case SUDOKU_PLAY:
        //fallthrough
      default: break;
    }
  }

theend:
  Video.release (My.video);
  Term.reset (My.term);
  Term.release (&My.term);
  String.release (My.buf);
  String.release (My.filename);
  String.release (My.datadir);
  String.release (My.name);

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
  __INIT__(dir);
  __INIT__(term);
  __INIT__(file);
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
