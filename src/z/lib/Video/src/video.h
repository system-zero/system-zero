#ifndef VIDEO_HDR
#define VIDEO_HDR

typedef struct video_t {
  vstring_t *head;
  vstring_t *tail;
  vstring_t *current;
        int  cur_idx;
        int  num_items;

  string_t
    *render,
    *tmp_render;

  Vstring_t *tmp_list;

  int
    fd,
    num_cols,
    num_rows,
    first_row,
    first_col,
    last_row,
    row_pos,
    col_pos;

  int *rows;
} video_t;

typedef struct video_set_self {
  void (*row_with) (video_t *, int, char *);
} video_set_self;

typedef struct video_draw_self {
  void
    (*row_at) (video_t *, int),
    (*all) (video_t *);

  int
    (*bytes) (video_t *, char *, size_t);
} video_draw_self;

typedef struct video_self {
  video_set_self set;
  video_draw_self draw;

  video_t
    *(*new) (int, int, int, int, int),
    *(*paint_rows_with) (video_t *, int, int, int, char *);

  void
    (*flush) (video_t *, string_t *),
    (*release) (video_t *),
    (*row_hl_at) (video_t *, int, int, int, int),
    (*render_set_from_to) (video_t *, int, int),
    (*resume_painted_rows) (video_t *);
} video_self;

typedef struct video_T {
  video_self self;
} video_T;

public video_T __init_video__ (void);

#endif /* VIDEO_HDR */
