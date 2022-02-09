#ifndef MAN_HDR
#define MAN_HDR

#define MAN_DEFAULT_PATH_LEN    127
#define MAN_DEFAULT_SECTION_LEN 63
#define MAN_DEFAULT_SUFFIX_LEN  31
#define MAN_SECTION_LEN         31
#define MAN_PAGE_LEN            63

typedef struct man_t man_t;

typedef int  (*ManGetCharCallback) (man_t *);
typedef void (*ManUngetCharCallback) (man_t *, int);
typedef int  (*ManOutputCallback) (man_t *, const char *, ...);

struct man_t {
  char defaultPath[MAN_DEFAULT_PATH_LEN + 1];
  char defaultSection[MAN_DEFAULT_SECTION_LEN + 1];
  char defaultSuffix[MAN_DEFAULT_SUFFIX_LEN + 1];
  char section[MAN_SECTION_LEN + 1];
  char page[MAN_PAGE_LEN + 1];

  char whitespace[256];
  char doc_footer[512];  /* Document footer line */
  char man_file[256];

  char line_header[1520]; /* Page header line (snprintf complains rather for no reason) */
  char word[80];         /* Current word */

  FILE *input_fp;
  FILE *output_fp;

  int *line_ptr;
  int line[256];         /* Buffer for building output line */
  int pending_nl;        /* Is there a pending newline on output? */
  int current_line;      /* Line number = ? */
  int no_fill;           /* Disable 'filling' of lines */
  int left_indent;       /* Current step of left margin */
  int old_para_indent;   /* Indent to go back to after this paragraph */
  int right_margin;      /* Don't print past this column */
  int input_tab;         /* Tab width in input file */
  int right_adjust;      /* Adjust right margin */
  int standard_tab;      /* Amount left margin stepped by */
  int keep_nl;           /* How many nl to keep til eof */
  int optional_keep;     /* Is the next keep optional ? */
  int gaps_on_line;      /* Gaps on line for adjustments */
  int no_nl;             /* Next NL in input file is ignored */
  int catmode;           /* Have we seen a '.XX' command ? */
  int cur_font;          /* Current font, 1 == Roman */
  int next_line_indent;  /* Indent after next line_break */
  int is_tty;
  int verbose;

  void *userData;

  ManGetCharCallback     get_char;
  ManUngetCharCallback   unget_char;
  ManOutputCallback      output_string;
};

int manPage (man_t *, char *, const char *);
man_t *manInit (man_t *);

#endif /* MAN_HDR */
