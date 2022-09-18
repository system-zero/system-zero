#ifndef PAGER_HDR
#define PAGER_HDR

#define DEFAULT_TABWIDTH 8

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
  string **rows;
  string *render;
  int row_pos;
  int col_pos;
  int first_col;
} video_t;

typedef struct line_t {
  Ustring_t *data;
  int first_col_idx;
  int cur_col_idx;
  size_t num_bytes;
} line_t;

typedef struct Ftype {
  int tabwidth;
} Ftype;

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
  int has_statusline;
  size_t  array_len;

  int     input_should_be_freed;
  string  **input;

  Ftype   *ftype;
  line_t  **lines;
  line_t  *cur_line;
  line_t  *prev_line;

  string  *tmp_buf;
  string  *empty_line;

  video_t *video;
  term_t  *term;
  pager_t *parent;
} Buf;

typedef struct readline_t readline_t;

struct pager_t {
  int     first_row;
  int     first_col;
  int     last_row;
  int     last_col;
  int     num_cols;
  int     num_rows;

  video_t *video;
  string  **video_rows;

  int     term_should_be_freed;
  term_t  *term;

  Buf     **buffers;
  Buf     *cur_buf;
  int      num_buf;

  readline_t *readline;

  PagerOnInput on_input;
};

typedef struct pager_opts {
  int first_row;
  int first_col;
  int last_col;
  int last_row;
  int tabwidth;
  int term_should_be_freed;
  int input_should_be_freed;
  int buf_has_statusline;
  int has_command_line;

  term_t *term;
} pager_opts;

#define PagerOpts(...) (pager_opts) {  \
  .first_row  = -1,                    \
  .first_col  = -1,                    \
  .last_row   = -1,                    \
  .last_col   = -1,                    \
  .tabwidth   = DEFAULT_TABWIDTH,      \
  .term       = NULL,                  \
  .term_should_be_freed  = 1,          \
  .input_should_be_freed = 1,          \
  .buf_has_statusline = 1,             \
  .has_command_line = 1,               \
  __VA_ARGS__                          \
}

pager_t *pager_new (string **, size_t, pager_opts);
int pager_main (pager_t *);
void pager_release (pager_t *);

#endif /* PAGER_HDR */
