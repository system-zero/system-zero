#define REQUIRE_ALLOC
#define REQUIRE_VSNPRINTF
#define REQUIRE_STRTOL
#define REQUIRE_STRING
#define REQUIRE_ISALNUM
#define REQUIRE_UTF8_CODE
#define REQUIRE_UTF8_CHARACTER
#define REQUIRE_STDIO
#define REQUIRE_STR_BYTES_IN_STR
#define REQUIRE_TIME
#define REQUIRE_GMTIME
#define REQUIRE_STRFTIME

// pulled from: https://git.sr.ht/~sircmpwn/scdoc
// at commit: afeda241f3f9b2c27e461f32d9c2a704ab82ef61

/* Intented as a simple man generator with a markdown like syntax.
 *
 * The code is compiled by using functionality from our own libraries and
 * so is not using nothing by libc.
 */

#include <libc.h>
#include <scdoc.h>

#define UTF8_INVALID 0x80
#define SCDOC_VERSION "1.11.2"

enum ScdocFormatting {
  FORMAT_BOLD = 1,
  FORMAT_UNDERLINE = 2,
  FORMAT_LAST = 4
};

static utf8 parser_getch (ScdocParser *p) {
  if (p->qhead)
    return (p->c = p->queue[--p->qhead]);

  if (p->str and *p->str) {
    int len = 0;
    p->c = utf8_code_and_len (p->str, &len);

    ifnot (p->c) {
      p->str = NULL;
      return EOF;
    }

    p->str += len;
    return p->c;
  }

  int len = 0;
  p->c = utf8_code_and_len (p->input_ptr, &len);

  ifnot (p->c)
    return EOF;

  p->input_ptr += len;

  if (p->c is '\n') {
    p->col = 0;
    p->line++;
  } else
    p->col++;

  return p->c;
}

static void parser_pushc (ScdocParser *p, utf8 c) {
  if (c and c isnot EOF and c isnot UTF8_INVALID)
    p->queue[p->qhead++] = c;
}

static void parser_pushstr (ScdocParser *p, const char *str) {
  p->str = str;
}

static int roff_macro (ScdocParser *p, const char *cmd, ...) {
  int n = p->output->num_bytes;
  string_append_with_fmt (p->output, ".%s", cmd);
  int l = p->output->num_bytes - n;

  va_list ap;
  va_start(ap, cmd);
  const char *arg;

  while ((arg = va_arg(ap, const char *))) {
    string_append_with_len (p->output, " \"", 2);

    while (*arg) {
      utf8 c = utf8_code (arg);
      if (c is '"') {
        string_append_byte (p->output, '\\');
        l++;
      }

      n = p->output->num_bytes;
      string_append_utf8 (p->output, c);
      l += (p->output->num_bytes - n);
    }

    string_append_byte (p->output, '"');
    l += 3;
  }

  va_end(ap);

  string_append_byte (p->output, '\n');
  return l + 1;
}

static string *parse_section (ScdocParser *p) {
  string *section = string_new (32);

  utf8 c;
  char *subsection;

  while ((c = parser_getch (p))) {
    if (c is EOF)
      break;

    if (c < 0x80 and isalnum ((uchar) c)) {
      string_append_byte (section, c);

    } else if (c is ')') {
      ifnot (section->num_bytes)
        break;

      int sec = str_to_long (section->bytes, &subsection, 10);

      if (section->bytes is subsection) {
        sys_fprintf (sys_stderr, "Expected section digit\n");
        string_release (section);
        return NULL;
      }

      if (sec < 0 or sec > 9) {
        sys_fprintf (sys_stderr, "Expected section between 0 and 9\n");
        string_release (section);
        return NULL;
      }

      return section;

    } else {
      sys_fprintf (sys_stderr, "Expected alphanumerical character or )\n");
      string_release (section);
      return NULL;
    }
  }

  sys_fprintf (sys_stderr, "Expected manual section\n");
  string_release (section);
  return NULL;
}

static string *parse_extra (ScdocParser *p) {
  string *extra = string_new (32);

  string_append_byte (extra, '"');

  utf8 c;

  while ((c = parser_getch (p))) {
    if (c is EOF)
      break;

    if (c is '"') {
      string_append_byte (extra, c);
      return extra;
    } else if (c is '\n') {
      sys_fprintf (sys_stderr, "Unclosed extra preamble field\n");
      string_release (extra);
      return NULL;
    } else {
      string_append_byte (extra, c);
    }
  }

  string_release (extra);
  return NULL;
}

static int parse_preamble (ScdocParser *p) {
  string *extras[2] = { NULL };
  string *section = NULL;

  int ex = 0;
  int retval = 0;

  char date[256];
  time_t date_time = sys_time (NULL);

  struct tm tm;
  struct tm *date_tm = sys_gmtime (&date_time, &tm);

  str_ftime (date, sizeof(date), "%F", date_tm);

  string *name = string_new (32);
  utf8 c;

  while ((c = parser_getch (p))) {
    if (c is EOF)
      break;

    if (c is UTF8_INVALID)
      return -1;

    if ((c < 0x80 and isalnum ((uchar) c))
        or c is '_' or c is '-' or c is '.') {
      string_append_byte (name, c);

    } else if (c is '(') {
      section = parse_section (p);

      if (NULL is section) {
        retval = -1;
        goto theend;
      }

    } else if (c is '"') {
      if (ex is 2) {
        sys_fprintf (sys_stderr, "Too many extra preamble fields\n");
        retval = -1;
        goto theend;
      }

      extras[ex] = parse_extra (p);
      if (extras[ex] is NULL) {
        retval = -1;
        goto theend;
      }
      ex++;

    } else if (c is '\n') {
      ifnot (name->num_bytes) {
        sys_fprintf (sys_stderr, "Expected preamble\n");
        retval = -1;
        goto theend;
      }

      if (section is NULL) {
        sys_fprintf (sys_stderr, "Expected manual section\n");
        retval = -1;
        goto theend;
      }

      char *ex2 = extras[0] isnot NULL ? extras[0]->bytes : NULL;
      char *ex3 = extras[1] isnot NULL ? extras[1]->bytes : NULL;
      string_append_with_fmt (p->output, ".TH \"%s\" \"%s\" \"%s\"",
         name->bytes, section->bytes, date);

      /* ex2 and ex3 are already double-quoted */
      if (ex2)
        string_append_with_fmt (p->output, " %s", ex2);

      if (ex3)
        string_append_with_fmt (p->output, " %s", ex3);

      string_append_byte (p->output, '\n');
      break;

    } else if (section is NULL) {
      sys_fprintf (sys_stderr, "Name characters must be A-Z, a-z, 0-9, `-`, `_`, or `.`\n");
      retval = -1;
      goto theend;
    }
  }

theend:
  string_release (name);
  for (int i = 0; i < 2; ++i)
    string_release (extras[i]);

  return retval;
}

static int parse_format (ScdocParser *p, enum ScdocFormatting fmt) {
  char formats[FORMAT_LAST] = {
    [FORMAT_BOLD] = 'B',
    [FORMAT_UNDERLINE] = 'I',
  };

  if (p->flags) {
    if ((p->flags & ~fmt)) {
      sys_fprintf (sys_stderr, "Cannot nest inline formatting (began with %c at %d:%d)\n",
          p->flags is FORMAT_BOLD ? '*' : '_',
          p->fmt_line, p->fmt_col);
      return -1;
    }

    string_append_with_len (p->output, "\\fR", 3);

  } else {
    string_append_with_fmt (p->output, "\\f%c", formats[fmt]);
    p->fmt_line = p->line;
    p->fmt_col = p->col;
  }

  p->flags ^= fmt;
  return 0;
}

static int parse_linebreak (ScdocParser *p) {
  utf8 plus = parser_getch (p);

  if (plus isnot '+') {
    string_append_byte (p->output, '+');
    parser_pushc (p, plus);
    return false;
  }

  utf8 lf = parser_getch (p);

  if (lf isnot '\n') {
    string_append_byte (p->output, '+');
    parser_pushc (p, lf);
    parser_pushc (p, plus);
    return false;
  }

  utf8 c = parser_getch (p);
  if (c is '\n') {
    sys_fprintf (sys_stderr, "Explicit line breaks cannot be followed by a blank line\n");
    return -1;
  }

  parser_pushc (p, c);
  string_append_with_len (p->output, "\n.br\n", 5);
  return true;
}

static int parse_text (ScdocParser *p) {
  utf8
    c,
    next,
    last = ' ';

  int i = 0;

  while ((c = parser_getch (p))) {
    if (c is EOF or c is UTF8_INVALID)
      break;

    switch (c) {
    case '\\':
      c = parser_getch (p);
      if (c is EOF or c is UTF8_INVALID) {
        sys_fprintf (sys_stderr, "Unexpected EOF\n");
        return -1;

      } else if (c is '\\')
        string_append_with_len (p->output, "\\\\", 2);
      else
        string_append_utf8 (p->output, c);

      break;

    case '*':
      if (-1 is parse_format (p, FORMAT_BOLD)) return -1;
      break;

    case '_':
      next = parser_getch (p);
      if (!isalnum ((uchar) last) or (
            (p->flags & FORMAT_UNDERLINE) and
            !isalnum ((uchar) next))) {
        if (-1 is parse_format (p, FORMAT_UNDERLINE)) return -1;

      } else
        string_append_utf8 (p->output, c);

      if (next is EOF)
        return 0;

      if (next is UTF8_INVALID)
        return -1;

      parser_pushc (p, next);
      break;

    case '+':
      switch (parse_linebreak (p)) {
        case true:
          last = '\n';
          break;

        case -1:
          return -1;
      }
      break;

    case '\n':
      string_append_utf8 (p->output, c);
      return 0;

    case '.':
      if (!i) {
        // Escape . if it's the first character
        string_append_with_len (p->output, "\\&.\\&", 5);
        break;
      }

      /* fallthrough */
    case '\'':
      if (!i) {
        // Escape ' if it's the first character
        string_append_with_len (p->output, "\\&'\\&", 5);
        break;
      }

      /* fallthrough */
    case '!':
    case '?':
      last = c;
      string_append_utf8 (p->output, c);

      // Suppress sentence spacing
      string_append_with_len (p->output, "\\&", 2);
      break;

    default:
      last = c;
      string_append_utf8 (p->output, c);
      break;
    }

    i++;
  }

  return 0;
}

static int parse_heading (ScdocParser *p) {
  utf8 c;
  int level = 1;

  while ((c = parser_getch (p))) {
    if (c is EOF)
      break;

    if (c is UTF8_INVALID)
      return -1;

    if (c is '#') {
      level++;
    } else if (c is ' ') {
      break;
    } else {
      sys_fprintf (sys_stderr, "Invalid start of heading (probably needs a space)\n");
      return -1;
    }
  }

  switch (level) {
    case 1:
      string_append_with_len (p->output, ".SH ", 4);
      break;

    case 2:
      string_append_with_len (p->output, ".SS ", 4);
      break;

    default:
      sys_fprintf (sys_stderr, "Only headings up to two levels deep are permitted\n");
      return -1;
  }

  while ((c = parser_getch (p))) {
    if (c is EOF)
      break;

    if (c is UTF8_INVALID)
      return -1;

    string_append_utf8 (p->output, c);

    if (c is '\n')
      break;
  }

  return 0;
}

static int parse_indent (ScdocParser *p, int *indent, bool write) {
  int i = 0;
  utf8 c;

  while ((c = parser_getch (p)) is '\t')
    i++;

  parser_pushc (p, c);

  if ((c is '\n' or c is UTF8_INVALID or c is EOF) and *indent isnot 0) {
    // Don't change indent when we encounter empty lines or EOF
    return *indent;
  }

  if (write) {
    if ((i - *indent) > 1) {
      sys_fprintf (sys_stderr, "Indented by an amount greater than 1\n");
      return -1;

    } else if (i < *indent) {
      for (int j = *indent; i < j; --j)
        roff_macro (p, "RE", NULL);

    } else if (i is *indent + 1) {
      string_append_with_len (p->output, ".RS 4\n", 6);
    }
  }

  *indent = i;
  return i;
}

static int list_header (ScdocParser *p, int *num) {
  if (*num is -1) {
    string_append_with_fmt (p->output, ".IP %s 4\n", "\\(bu");
  } else {
    string_append_with_fmt (p->output, ".IP %d. 4\n", *num);
    *num = *num + 1;
  }

  return 0;
}

static int parse_list (ScdocParser *p, int *indent, int num) {
  utf8 c;

  if ((c = parser_getch(p)) isnot ' ') {
    sys_fprintf (sys_stderr, "Expected space before start of list entry\n");
    return -1;
  }

  string_append_with_len (p->output, ".PD 0\n", 6);
  list_header (p, &num);

  if (-1 is parse_text (p))
    return -1;

  do {
    if (-1 is parse_indent (p, indent, true))
      return -1;

    c = parser_getch (p);

    if (c is EOF)
      break;

    if (c is UTF8_INVALID)
      return -1;

    switch (c) {
      case ' ':
        if ((c = parser_getch (p)) isnot ' ') {
          sys_fprintf (sys_stderr, "Expected two spaces for list entry continuation\n");
          return -1;
        }

        if (-1 is parse_text (p))
          return -1;

        break;

      case '-':
      case '.':
        if ((c = parser_getch (p)) isnot ' ') {
          sys_fprintf (sys_stderr, "Expected space before start of list entry\n");
          return -1;
        }

        list_header (p, &num);

        if (-1 is parse_text (p))
          return -1;

        break;

      default:
        roff_macro (p, "PD", NULL);
        parser_pushc (p, c);
        return 0;
    }

  } while (c isnot UTF8_INVALID and c isnot EOF);

  return 0;
}

static int parse_literal (ScdocParser *p, int *indent) {
  utf8 c;

  if ((c = parser_getch (p)) isnot '`' or
      (c = parser_getch (p)) isnot '`' or
      (c = parser_getch (p)) isnot '\n') {
    sys_fprintf (sys_stderr, "Expected ``` and a newline to begin literal block\n");
    return -1;
  }

  int stops = 0;

  roff_macro (p, "nf", NULL);
  string_append_with_len (p->output, ".RS 4\n", 6);

  bool check_indent = true;

  do {
    if (check_indent) {
      int _indent = *indent;
      if (-1 is parse_indent (p, &_indent, false))
        return -1;

      if (_indent < *indent) {
        sys_fprintf (sys_stderr, "Cannot deindent in literal block\n");
        return -1;
      }

      while (_indent > *indent) {
        _indent--;
        string_append_byte (p->output, '\t');
      }

      check_indent = false;
    }

    c = parser_getch (p);
    if (c is EOF)
      break;

    if (c is UTF8_INVALID)
      return -1;

    if (c is '`') {
      if (++stops is 3) {
        if ((c = parser_getch (p)) isnot '\n') {
          sys_fprintf (sys_stderr, "Expected literal block to end with newline\n");
          return -1;
        }

        roff_macro (p, "fi", NULL);
        roff_macro (p, "RE", NULL);
        return 0;
      }

    } else {
      while (stops isnot 0) {
        string_append_byte (p->output, '`');
        stops--;
      }

      switch (c) {
        case '.':
          string_append_with_len (p->output, "\\&.", 4);
          break;

        case '\'':
          string_append_with_len (p->output, "\\&'", 3);
          break;

        case '\\':
          c = parser_getch (p);
          if (c is UTF8_INVALID or c is EOF) {
            sys_fprintf (sys_stderr, "Unexpected EOF\n");
            return -1;

          } else if (c is '\\')
            string_append_with_len (p->output, "\\\\", 2);
          else
            string_append_utf8 (p->output, c);

          break;

        case '\n':
          check_indent = true;
          /* fallthrough */
        default:
          string_append_utf8 (p->output, c);
          break;
      }
    }
  } while (c isnot UTF8_INVALID and c isnot EOF);

  return 0;
}

static int parse_table (ScdocParser *p, utf8 style) {
  struct table_row *table = NULL;
  struct table_row *currow = NULL, *prevrow = NULL;
  struct table_cell *curcell = NULL;
  int column = 0;
  int numcolumns = -1;
  utf8 c;

  parser_pushc (p, '|');

  do {
    c = parser_getch (p);

    if (c is EOF)
      break;

    if (c is UTF8_INVALID)
      return -1;

    switch (c) {
      case '\n':
        goto commit_table;

      case '|':
        prevrow = currow;
        currow = Alloc(sizeof (struct table_row));

        if (prevrow) {
          if (column isnot numcolumns and numcolumns isnot -1) {
            sys_fprintf (sys_stderr, "Each row must have the same number of columns\n");
            return -1;
          }

          numcolumns = column;
          column = 0;
          prevrow->next = currow;
        }

        curcell = Alloc (sizeof (struct table_cell));
        currow->cell = curcell;
        if (!table)
          table = currow;

        break;

      case ':':
        if (!currow) {
          sys_fprintf (sys_stderr, "Cannot start a column without starting a row first\n");
          return -1;

        } else {
          struct table_cell *prev = curcell;
          curcell = Alloc (sizeof (struct table_cell));
          if (prev)
            prev->next = curcell;

          column++;
        }
        break;

      case ' ':
        goto continue_cell;

      default:
        sys_fprintf (sys_stderr,  "Expected either '|' or ':'\n");
        return -1;
        break;
    }

    c = parser_getch (p);

    if (c is EOF)
      break;

    if (c is UTF8_INVALID)
      return -1;

    switch (c) {
      case '[':
        curcell->align = SCDOC_ALIGN_LEFT;
        break;

      case '-':
        curcell->align = SCDOC_ALIGN_CENTER;
        break;

      case ']':
        curcell->align = SCDOC_ALIGN_RIGHT;
        break;

      case '<':
        curcell->align = SCDOC_ALIGN_LEFT_EXPAND;
        break;

      case '=':
        curcell->align = SCDOC_ALIGN_CENTER_EXPAND;
        break;

      case '>':
        curcell->align = SCDOC_ALIGN_RIGHT_EXPAND;
        break;

      case ' ':
        if (prevrow) {
          struct table_cell *pcell = prevrow->cell;
          for (int i = 0; i <= column and pcell; ++i, pcell = pcell->next) {
            if (i is column) {
              curcell->align = pcell->align;
              break;
            }
          }
        } else {
          sys_fprintf (sys_stderr, "No previous row to infer alignment from\n");
          return -1;
        }

        break;

      default:
        sys_fprintf (sys_stderr, "Expected one of '[', '-', ']', or ' '\n");
        return -1;
    }

    curcell->contents = string_new (32);

continue_cell:
    switch (c = parser_getch (p)) {
      case ' ':
        // Read out remainder of the text
        while ((c = parser_getch (p))) {
          if (c is EOF)
            break;

          if (c is UTF8_INVALID)
            return -1;

          switch (c) {
            case '\n':
              goto commit_cell;

            default:;
              string_append_byte (curcell->contents, c);
              break;
          }
        }

        break;

      case '\n':
        goto commit_cell;

      default:
        sys_fprintf (sys_stderr, "Expected ' ' or a newline\n");
        return -1;
    }

commit_cell:
    if (str_bytes_in_str (curcell->contents->bytes, "T{")
        or str_bytes_in_str (curcell->contents->bytes, "T}")) {
      sys_fprintf (sys_stderr, "Cells cannot contain T{ or T} due to roff limitations\n");
      return -1;
    }
  } while (c isnot UTF8_INVALID and c isnot EOF);

commit_table:

  if (c is EOF)
    return 0;

  if (c is UTF8_INVALID)
    return -1;

  roff_macro (p, "TS", NULL);

  switch (style) {
    case '[':
      string_append_with_len (p->output, "allbox;", 7);
      break;

    case ']':
      string_append_with_len (p->output, "box;", 4);
      break;
  }

  // Print alignments first
  currow = table;
  while (currow) {
    curcell = currow->cell;

    while (curcell) {
      const char *align = "";

      switch (curcell->align) {
        case SCDOC_ALIGN_LEFT:
          align = "l";
          break;

        case SCDOC_ALIGN_CENTER:
          align = "c";
          break;

        case SCDOC_ALIGN_RIGHT:
          align = "r";
          break;

        case SCDOC_ALIGN_LEFT_EXPAND:
          align = "lx";
          break;

        case SCDOC_ALIGN_CENTER_EXPAND:
          align = "cx";
          break;

        case SCDOC_ALIGN_RIGHT_EXPAND:
          align = "rx";
          break;

      }

      string_append_with_fmt (p->output, "%s%s", align, curcell->next ? " " : "");
      curcell = curcell->next;
    }

    string_append_with_fmt (p->output, "%s\n", currow->next ? "" : ".");
    currow = currow->next;
  }

  // Then contents
  currow = table;
  while (currow) {
    curcell = currow->cell;
    string_append_with_len (p->output, "T{\n", 3);

    while (curcell) {
      parser_pushstr (p, curcell->contents->bytes);

      if (-1 is parse_text (p)) return -1;

      if (curcell->next)
        string_append_with_len (p->output, "\nT}\tT{\n", 7);
      else
        string_append_with_len (p->output, "\nT}", 3);

      struct table_cell *prev = curcell;
      curcell = curcell->next;
      string_release (prev->contents);
      Release (prev);
    }

    string_append_byte (p->output, '\n');

    struct table_row *prev = currow;
    currow = currow->next;
    Release (prev);
  }

  roff_macro (p, "TE", NULL);
  string_append_with_len (p->output, ".sp 1\n", 6);

  return 0;
}

static int parse_document (ScdocParser *p) {
  utf8 c;
  int indent = 0;

  do {
    if (-1 is parse_indent (p, &indent, true)) return -1;

    c = parser_getch (p);

    if (c is EOF)
      break;

    if (c is UTF8_INVALID)
      return -1;

    switch (c) {
      case ';':
        if ((c = parser_getch (p)) isnot ' ') {
          sys_fprintf (sys_stderr, "Expected space after ; to begin comment\n");
          return -1;
        }

        do
          c = parser_getch (p);
        while (c isnot UTF8_INVALID and c isnot EOF and c isnot '\n');

        break;

      case '#':
        if (indent isnot 0) {
          parser_pushc (p, c);

          if (-1 is parse_text(p)) return -1;

          break;
        }

        if (-1 is parse_heading(p)) return -1;

        break;

      case '-':
        if (-1 is parse_list (p, &indent, -1)) return -1;
        break;

      case '.':
        if ((c = parser_getch (p)) is ' ') {
          parser_pushc (p, c);
          if (-1 is parse_list (p, &indent, 1)) return -1;

        } else {
          parser_pushc (p, c);
          if (-1 is parse_text (p)) return -1;
        }
        break;

      case '`':
        if (-1 is parse_literal (p, &indent)) return -1;
        break;

      case '[':
      case '|':
      case ']':
        if (indent isnot 0) {
          sys_fprintf (sys_stderr, "Tables cannot be indented\n");
          return -1;
        }

        if (-1 is parse_table (p, c)) return -1;
        break;

      case ' ':
        sys_fprintf (sys_stderr, "Tabs are required for indentation\n");
        break;

      case '\n':
        if (p->flags) {
          sys_fprintf (sys_stderr, "Expected %c before starting new paragraph (began with %c at %d:%d)",
              p->flags is FORMAT_BOLD ? '*' : '_',
              p->flags is FORMAT_BOLD ? '*' : '_',
              p->fmt_line, p->fmt_col);
          return -1;
        }

        roff_macro (p, "PP", NULL);
        break;

      default:
        parser_pushc (p, c);
        if (-1 is parse_text (p)) return -1;
        break;
    }
  } while (c isnot UTF8_INVALID and c isnot EOF);

  return 0;
}

static void output_scdoc_preamble (ScdocParser *p) {
  string_append_with (p->output, ".\\\" Generated by scdoc " SCDOC_VERSION "\n");
  string_append_with (p->output, ".\\\" Complete documentation for this program is not "
      "available as a GNU info page\n");
  string_append_with (p->output, ".ie \\n(.g .ds Aq \\(aq\n");
  string_append_with (p->output, ".el       .ds Aq '\n");
  // Disable hyphenation:
  roff_macro (p, "nh", NULL);
  // Disable justification:
  roff_macro (p, "ad l", NULL);
  string_append_with (p->output, ".\\\" Begin generated content:\n");
}

void scdoc_release (ScdocParser *p) {
  string_release (p->input);
  string_release (p->output);
}

string *scdoc_parse (ScdocParser *p, const char *input, size_t len) {
  p->input = string_new_with_len (input, len);
  p->output = string_new (256);
  p->input_ptr = p->input->bytes;
  p->line = 1;
  p->col = 1;
  p->qhead = 0;
  p->flags = 0;
  p->str = NULL;

  output_scdoc_preamble(p);

  if (-1 is parse_preamble (p)) return NULL;

  if (-1 is parse_document (p))
    return NULL;

  return p->output;
  // the output agrees with upstream's output (tested its own man page as input)
}
