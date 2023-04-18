typedef struct ScdocParser ScdocParser;

string *scdoc_parse (ScdocParser *, const char *, size_t);
void scdoc_release (ScdocParser *);

enum table_align {
  SCDOC_ALIGN_LEFT,
  SCDOC_ALIGN_CENTER,
  SCDOC_ALIGN_RIGHT,
  SCDOC_ALIGN_LEFT_EXPAND,
  SCDOC_ALIGN_CENTER_EXPAND,
  SCDOC_ALIGN_RIGHT_EXPAND,
};

struct table_row {
  struct table_cell *cell;
  struct table_row *next;
};

struct table_cell {
  enum table_align align;
  string *contents;
  struct table_cell *next;
};

typedef struct ScdocParser {
  string
    *input,
    *output;

  int
    col,
    line,
    qhead,
    fmt_col,
    fmt_line;

  uint32_t
    flags,
    queue[32];

  const char *str;
  char *input_ptr;
} ScdocParser;

