/* (C) 1997 Robert de Bath
 * (C) 2013 rofl0r
 * under the terms of the GPL.
 *
 * This is a manual pager program, it will search for and format manual
 * pages which it then pipes to more.
 *
 * The program understands manual pages that have been compressed with
 * either 'compress' or 'gzip' and will decompress them on the fly.
 *
 * The environment is checked for these variables:
 *   MANSECT=1:2:3:4:5:6:7:8:9    # Manual section search order.
 *   MANPATH=/usr/local/man:/usr/man  # Directorys to search for man tree.
 *   PAGER=more        # pager progam to use.
 *   PATH=...        # Search for gzip/uncompress
 *
 * The program will display documents that are either in it's own "nroff -man"
 * like format or in "catman" format, it will not correctly display pages in
 * the BSD '-mdoc' format.
 *
 * Neither nroff nor any similar program is needed as this program has it's
 * own built in _man_ _page_ _formatter_. This is NOT an nroff clone and will
 * not (for instance) understand macros or tbl constructs.
 *
 * Unlike groff this is small, fast and picky!
 */

/*
 * The purpose here is to use this unit as a library. So we had to move first
 * all the static state into a structure and pass this state to the functions.
 *
 * The second thing is to use the end result with an own way instead of directly
 * printing either to standard output or pass it to a pager, which is the default.
 * So we introduced an output callback function, along with a void * pointer
 * as user data.
 *
 * The third thing is to pass a string as the roff document instead of looking
 * up to the MANPATH and opening the man page. Again we introduced two input
 * functions, but we still have to implement the functionality.
 *
 * But really the main aim here is to use this development as a vehicle to an
 * experiment, which is simply to make it indepented even from libc.
 * Again we still have to do the implementation.
 * 
 * Unfortunatelly i do not understand all the intentions of the code, so
 * there are some difficulties to adjust the code in a way, that will not
 * break the functionality and at the same time to be flexible. For now it
 * is not that big of success but it works!
 */

/* workaround for our build system */
#ifndef USE_SYSTEM_LIBC
#define USE_SYSTEM_LIBC
#define _POSIX_C_SOURCE 200809L
#endif

#include <sys/ioctl.h>

// prepare an abstraction for the switch we are talking about
#ifdef USE_SYSTEM_LIBC
#include <stdio.h>
#include <unistd.h>
#define STRING_FORMAT     snprintf
#define STRING_VFORMAT_FP vfprintf

#include <string.h>
#define MEM_COPY          memcpy
#define STRING_BYTELEN    strlen
#define STRING_CHR        strchr
#define STRING_R_CHR      strrchr
#define STRING_CP_FMT     snprintf
#define STRING_EQ(...)   (strcmp (__VA_ARGS__) == 0)
#define STRING_EQ_N(...) (strncmp (__VA_ARGS__) == 0)
#define STRING_CAT(_d_, _z_, _s_)       strcat (_d_, _s_)
#define STRING_CP(_d_, _dz_, _s_, _sz_) strcpy (_d_, _s_)

#include <stdlib.h>
#define ATOI     atoi
#define GET_ENV  getenv

#include <ctype.h>
#define IS_ALPHA isalpha
#define IS_SPACE isspace

#include <stdarg.h>

//#else
//#include "clib.c"
#endif

#include "man.h"

static int find_page (man_t *, char *, const char *);
static void step (char **pcurr, char **);
static int open_page (man_t *, char *);
static void close_page (man_t *);
static void do_file (man_t *);
static int fetch_word (man_t *);
static int do_command (man_t *);
static void do_skipeol (man_t *);
static int do_fontwords (man_t *, int, int, int);
static int do_noargs (man_t *, int);
static int do_argvcmd (man_t *, int);
static void build_headers (man_t *);
static void print_word (man_t *, const char *);
static void line_break (man_t *);
static void page_break (man_t *);
static void print_header (man_t *);
static void print_doc_footer (man_t *);

static int get_char_default (man_t *man) {
  return fgetc (man->input_fp);
}

static void unget_char_default (man_t *man, int c) {
  ungetc (c, man->input_fp);
}

static int output_string_default (man_t *man, const char *fmt, ...) {
  (void) man;
  va_list ap;
  va_start(ap, fmt);
  int num = STRING_VFORMAT_FP (man->output_fp, fmt, ap);
  va_end(ap);
  return num;
}

static void step (char **pcurr, char **pnext) {
  char *curr = *pcurr;
  char *next = *pnext;

  if (curr == 0)
    return;

  if (curr == next) {
    next = STRING_CHR(curr, ':');

    if (next)
      *next++ = 0;

  } else {
    curr = next;
    if (curr) {
      curr[-1] = ':';
      next = STRING_CHR(curr, ':');

      if (next)
        *next++ = 0;
    }
  }

  *pcurr = curr;
  *pnext = next;
}

static int open_page (man_t *man, char *name) {
  char *p;
  const char *command = '\0';
  char buf[256];

  if (access (name, 0) < 0)
    return -1;

  if ((p = STRING_R_CHR(name, '.'))) {
    if (STRING_EQ(p, ".gz"))
      command = "gzip -dc";
    else if (STRING_EQ(p, ".xz"))
      command = "xzcat -dc";
  }

  if (!command)
    command = "cat ";

  STRING_CP_FMT(buf, sizeof buf, "%s %s", command, name);

  if (!(man->input_fp = popen (buf, "r"))) return -1;
  return 0;
}

static int find_page (man_t *man, char *name, const char *sect) {
  static char defpath[] = "/usr/local/share/man:/usr/share/man";
  static char defsect[] = "1p:1:1perl:2:3p:3:3perl:4:5:6:7:8:9:0p";
  static char defsuff[] = ":.gz:.xz";
  static char manorcat[] = "man:cat";

  char fbuf[256];
  char *manpath;
  char *mansect = (char *) sect;
  char *mansuff;
  char *mc, *mp, *ms, *su, *nmc, *nmp, *nms, *nsu;
  int rv = -1;

  manpath = GET_ENV("MANPATH");
  if (!manpath)
    manpath = defpath;
  if (!mansect)
    mansect = GET_ENV("MANSECT");
  if (!mansect)
    mansect = defsect;
  mansuff = defsuff;

  if (STRING_CHR(name, '/')) {
    for (su = nsu = mansuff, step (&su, &nsu); su; step (&su, &nsu)) {
      STRING_FORMAT(fbuf, sizeof (fbuf), "%s%s", name, su);

      if ((rv = open_page (man, fbuf)) >= 0)
        break;
    }
    *man->man_file = 0;
    return rv;
  }

  /* SEARCH!! */
  for (mc = nmc = manorcat, step (&mc, &nmc); mc; step (&mc, &nmc))
    for (ms = nms = mansect, step (&ms, &nms); ms; step (&ms, &nms))
      for (mp = nmp = manpath, step (&mp, &nmp); mp; step (&mp, &nmp))
        for (su = nsu = mansuff, step (&su, &nsu); su; step (&su, &nsu)) {
          STRING_FORMAT(fbuf, sizeof fbuf, "%s/%s%s/%s.%s%s", mp, mc, ms, name, ms, su);

          /* Got it ? */
          if (access (fbuf, 0) < 0)
            continue;

          /* Try it ! */
          if ((rv = open_page (man, fbuf)) >= 0) {
            char *p;
            STRING_FORMAT(man->man_file, sizeof man->man_file, "%s", fbuf);
            p = STRING_R_CHR(man->man_file, '/');
            if (p)
              *p = 0;
            p = STRING_R_CHR(man->man_file, '/');
            if (p)
              p[1] = 0;
            return 0;
          }
        }

  return rv;
}

static void close_page (man_t *man) {
  if (man->input_fp) {
    pclose (man->input_fp);
    man->input_fp = NULL;
  }
}

/****************************************************************************
 * ifd is the manual page, ofd is the 'output' file or pipe, format it!
 */
static void do_file (man_t *man) {
  int nl;
  man->unget_char (man, '\r');

  while ((nl = fetch_word (man)) >= 0) {
    if (man->catmode) {
      if (STRING_EQ(man->word, "'\\\"") ||  STRING_EQ (man->word, "'''") == 0) {
        /* This is a marker sometimes used for opening subprocesses like
         * tbl and equ; this program ignores it.
         */
        do_skipeol (man);
      } else if (*man->whitespace == '\r')
        man->output_string (man, "%s%s", man->whitespace + 1, man->word);
      else
        man->output_string (man, "%s%s", man->whitespace, man->word);
    } else {
      if (man->keep_nl && nl && !man->no_nl) {
        if (man->optional_keep) {
          man->optional_keep = 0;
          if (man->line_ptr == 0 || man->next_line_indent < 0 ||
             man->left_indent + (man->line_ptr - man->line) + 1 > man->next_line_indent)
            line_break (man);
          else if (man->line_ptr != 0 && man->next_line_indent > 0) {
            while (man->left_indent + (man->line_ptr - man->line) + 1 <= man->next_line_indent)
              *man->line_ptr++ = man->cur_font + ' ';
          }
        } else
          line_break (man);
        if (man->keep_nl > 0)
          man->keep_nl--;
      }

      if (nl == 1 && man->no_fill)
        line_break (man);

      if (nl == 1 && (man->word[0] == '.' ||
               (man->word[0] == '\'' && STRING_EQ(man->word, "'\\\"")) ||
               (man->word[0] == '\'' && STRING_EQ(man->word, "'''"))
         )) {
        man->no_nl = 1;
        if (do_command (man) < 0)
          break;
      } else {
        if (*man->whitespace)
          print_word (man, man->whitespace);
        print_word (man, man->word);
        man->no_nl = 0;
      }
    }
  }

  print_doc_footer (man);
}

static int fetch_word (man_t *man) {
  static int col = 0;
  char *p;
  int ch, nl;

  nl = 0;
  *(p = man->whitespace) = 0;

  if (!man->catmode && !man->no_fill)
    p++;

  while ((ch = man->get_char (man)) != EOF && IS_SPACE(ch)) {
    if (nl && man->no_fill && ch != '.' && ch != '\n')
      break;
    if (nl && !man->catmode && ch == '\n') {
      *man->whitespace = 0;
      STRING_CP(man->word, 80, ".sp", 3);
      man->unget_char (man, ch);
      return 1;
    }
    nl = (ch == '\n' || ch == '\r');
    if (nl)
      col = 0;
    else
      col++;

    if (man->no_fill && nl && *man->whitespace) {
      *man->word = 0;
      man->unget_char (man, ch);
      return 0;
    }

    if (p < man->whitespace + sizeof (man->whitespace) - 1 && (!nl || man->catmode))
      *p++ = ch;

    if (ch == '\t' && !man->catmode) {
      p[-1] = ' ';
      while (col % man->input_tab) {
        if (p < man->whitespace + sizeof (man->whitespace) - 1)
          *p++ = ' ';
        col++;
      }
    }

    if (!man->catmode && !man->no_fill && nl)
      *(p = man->whitespace) = 0;
  }
  *p = 0;

  if (man->catmode && ch == '.' && nl)
    man->catmode = 0;

  *(p = man->word) = 0;
  if (ch == EOF || p > man->word + sizeof (man->word) / 2) {
    if (p != man->word) {
      man->unget_char (man, ch);
      *p = 0;
      return nl;
    }
    return -1;
  }
  man->unget_char (man, ch);

  while ((ch = man->get_char (man)) != EOF && !IS_SPACE(ch)) {
    if (p < man->word + sizeof (man->word) - 1)
      *p++ = ch;
    col++;
    if (ch == '\\') {
      if ((ch = man->get_char (man)) == EOF)
        break;
      // if ( ch == ' ' ) ch = ' ' + 0x80;    /* XXX Is this line needed? */
      if (p < man->word + sizeof (man->word) - 1)
        *p++ = ch;
      col++;
    }
  }
  *p = 0;
  man->unget_char (man, ch);

  return (nl != 0);
}

/****************************************************************************
 * Accepted nroff commands and executors.
 */

enum cmd_class {
  CCLASS_NONE = 0,
  CCLASS_PARAMETERED,
  CCLASS_FONTCHANGER,
  CCLASS_SO,
};

static const struct cmd_list_s {
  char cmd[3];
  char class; /* enum cmd_class */
  char id;
} cmd_list[] = {
  {"\\\"", CCLASS_NONE, 0},
  {"nh",   CCLASS_NONE, 0},  /* This program never inserts hyphens */
  {"hy",   CCLASS_NONE, 0},  /* This program never inserts hyphens */
  {"PD",   CCLASS_NONE, 0},  /* Inter-para distance is 1 line */
  {"DT",   CCLASS_NONE, 0},  /* Default tabs, they can't be non-default! */
  {"IX",   CCLASS_NONE, 0},  /* Indexing for some weird package */
  {"Id",   CCLASS_NONE, 0},  /* Line for RCS tokens */
  {"BY",   CCLASS_NONE, 0},  /* I wonder where this should go ? */
  {"nf",   CCLASS_NONE, 1},  /* Line break, Turn line fill off */
  {"fi",   CCLASS_NONE, 2},  /* Line break, Turn line fill on */
  {"sp",   CCLASS_NONE, 3},  /* Line break, line space (arg for Nr lines) */
  {"br",   CCLASS_NONE, 4},  /* Line break */
  {"bp",   CCLASS_NONE, 5},  /* Page break */
  {"PP",   CCLASS_NONE, 6},
  {"LP",   CCLASS_NONE, 6},
  {"P",    CCLASS_NONE, 6},  /* Paragraph */
  {"RS",   CCLASS_NONE, 7},  /* New Para + Indent start */
  {"RE",   CCLASS_NONE, 8},  /* New Para + Indent end */
  {"HP",   CCLASS_NONE, 9},  /* Begin hanging indent (TP without arg?) */
  {"ad",   CCLASS_NONE, 10},  /* Line up right margin */
  {"na",   CCLASS_NONE, 11},  /* Leave right margin unaligned */
  {"ta",   CCLASS_NONE, 12},  /* Changes _input_ tab spacing, right? */
  {"TH",   CCLASS_PARAMETERED, 1},/* Title and headers */
  {"SH",   CCLASS_PARAMETERED, 2},/* Section */
  {"SS",   CCLASS_PARAMETERED, 3},/* Subsection */
  {"IP",   CCLASS_PARAMETERED, 4},/* New para, indent except argument 1 */
  {"TP",   CCLASS_PARAMETERED, 5},/* New para, indent except line 1 */
  {"B",    CCLASS_FONTCHANGER, 22},/* Various font fiddles */
  {"BI",   CCLASS_FONTCHANGER, 23},
  {"BR",   CCLASS_FONTCHANGER, 21},
  {"I",    CCLASS_FONTCHANGER, 33},
  {"IB",   CCLASS_FONTCHANGER, 32},
  {"IR",   CCLASS_FONTCHANGER, 31},
  {"RB",   CCLASS_FONTCHANGER, 12},
  {"RI",   CCLASS_FONTCHANGER, 13},
  {"SB",   CCLASS_FONTCHANGER, 42},
  {"SM",   CCLASS_FONTCHANGER, 44},
  {"C",    CCLASS_FONTCHANGER, 22},/* PH-UX manual pages! */
  {"CI",   CCLASS_FONTCHANGER, 23},
  {"CR",   CCLASS_FONTCHANGER, 21},
  {"IC",   CCLASS_FONTCHANGER, 32},
  {"RC",   CCLASS_FONTCHANGER, 12},
  {"so",   CCLASS_SO, 0},
  {"\0\0", CCLASS_NONE, 0}
};

static int do_command (man_t *man) {
  const char *cmd;
  int i;
  char lbuf[16];

  cmd = man->word + 1;

  /* Comments don't need the space */
  if (STRING_EQ_N(cmd, "\\\"", 2))
    cmd = "\\\"";

  for (i = 0; cmd_list[i].cmd[0]; i++) {
    if (STRING_EQ(cmd_list[i].cmd, cmd))
      break;
  }

  if (cmd_list[i].cmd[0] == 0) {
    if (man->verbose) {
      STRING_CP(lbuf, 16, cmd, 3);
      lbuf[3] = 0;
      line_break (man);
      i = man->left_indent;
      man->left_indent = 0;
      print_word (man, man->word);
      line_break (man);
      STRING_FORMAT(man->word, sizeof man->word, "**** Unknown formatter command: .%s", lbuf);
      print_word (man, man->word);
      line_break (man);
      man->left_indent = i;
    }

    i = 0;    /* Treat as comment */
  }

FILE *saved_input_fp = NULL;

  switch (cmd_list[i].class) {
    case CCLASS_PARAMETERED:  /* Parametered commands */
      return do_argvcmd (man, cmd_list[i].id);

    case CCLASS_FONTCHANGER:  /* Font changers */
      return do_fontwords (man, cmd_list[i].id / 10, cmd_list[i].id % 10, 0);

    case CCLASS_SO:  /* .so */
      fetch_word (man);
      char man_back[256];
      STRING_CP(man_back, sizeof (man_back), man->man_file, sizeof (man->man_file) - 1);
      if (STRING_BYTELEN(man->man_file) + 4 < sizeof man->man_file)
        STRING_CAT(man->man_file, 256, man->word);

      saved_input_fp = man->input_fp;
  //    close_page (man);
      if (find_page (man, man->man_file, (char *) 0) < 0) {
        fprintf (stderr, "Cannot open .so file %s\n", man->word);
        return -1;
      }

      do_file (man);
      close_page (man);
      man->input_fp = saved_input_fp;
      STRING_CP(man->man_file, 256, man_back, 255);
      man->unget_char (man, '\r');
      break;

    case CCLASS_NONE:
    default:
      do_skipeol (man);
      if (cmd_list[i].id)
        return do_noargs (man, cmd_list[i].id);
  }
  return 0;
}

static void do_skipeol (man_t *man) {
  int ch;
  char *p = man->word;

  while ((ch = man->get_char (man)) != EOF && ch != '\n')
    if (p < man->word + sizeof (man->word) - 1)
      *p++ = ch;;

  *p = 0;
  man->unget_char (man, ch);
}

static void flush_word (man_t *man, char **p) {
  MEM_COPY(*p, "\\fR", 4);
  print_word (man, man->word);
  *p = man->word;
}

static void insert_font (char **p, int font) {
  const char ftab[] = " RBIS";
  MEM_COPY(*p, "\\f", 2);
  (*p)[2] = ftab[font];
  *p += 3;
}

static int do_fontwords (man_t *man, int this_font, int other_font, int early_exit) {
  char *p = man->word;
  int ch;
  int in_quote = 0;

  man->no_nl = 0; /* Line is effectivly been reprocessed so NL is visible */
  for (;;) {
    if (p == man->word) insert_font (&p, this_font);
    /* at each turn, at most 5 bytes are appended to word
     * in order to flush the buffer, 4 more bytes are required to stay free */
    if (p+5+4 >= man->word+sizeof man->word) {
      // assert(p+4<man->word+sizeof man->word);
      flush_word (man, &p);
      continue;
    }
    if ((ch = man->get_char (man)) == EOF || ch == '\n')
      break;
    if (ch == '"') {
      in_quote = !in_quote;
      continue;
    }
    if (in_quote || !IS_SPACE(ch)) {
      if (IS_SPACE(ch) && p > man->word + 3) {
        flush_word (man, &p);
        if (man->no_fill) print_word (man, " ");
        continue;
      }
      *p++ = ch;
      if (ch == '\\') {
        if ((ch = man->get_char (man)) == EOF || ch == '\n') break;
        *p++ = ch;
      }
      continue;
    }

    if (p != man->word + 3) {
      if (early_exit) break;

      if (this_font == other_font) flush_word (man, &p);
      int i = this_font;
      this_font = other_font;
      other_font = i;
      insert_font (&p, this_font);
    }
  }
  man->unget_char (man, ch);

  if (p > man->word + 3) flush_word (man, &p);

  return 0;
}

static int do_noargs (man_t *man, int cmd_id) {
  if (cmd_id < 10)
    line_break (man);
  switch (cmd_id) {
    case 1:
      man->no_fill = 1;
      break;
    case 2:
      man->no_fill = 0;
      break;
    case 3:
      man->pending_nl = 1;
      break;
    case 4:
      break;
    case 5:
      page_break (man);
      break;
    case 6:
      man->left_indent = man->old_para_indent;
      man->pending_nl = 1;
      break;
    case 7:
      man->pending_nl = 1;
      man->left_indent += man->standard_tab;
      man->old_para_indent += man->standard_tab;
      break;
    case 8:
      man->pending_nl = 1;
      man->left_indent -= man->standard_tab;
      man->old_para_indent -= man->standard_tab;
      break;

    case 10:
      man->right_adjust = 1;
      break;
    case 11:
      man->right_adjust = 0;
      break;
    case 12:
      man->input_tab = ATOI(man->word);
      if (man->input_tab <= 0)
        man->input_tab = 8;
      break;
  }
  return 0;
}

static int do_argvcmd (man_t *man, int cmd_id) {
  int ch;

  line_break (man);
  while ((ch = man->get_char (man)) != EOF && (ch == ' ' || ch == '\t')) ;
  man->unget_char (man, ch);

  switch (cmd_id + 10 * (ch == '\n')) {
    case 1:  /* Title and headers */
      page_break (man);
      man->left_indent = man->old_para_indent = man->standard_tab;
      build_headers (man);
      break;

    case 2:  /* Section */
      man->left_indent = 0;
      man->next_line_indent = man->old_para_indent = man->standard_tab;
      man->no_nl = 0;
      man->keep_nl = 1;
      man->pending_nl = 1;

      do_fontwords (man, 2, 2, 0);
      return 0;
    case 3:  /* Subsection */
      man->left_indent = man->standard_tab / 2;
      man->next_line_indent = man->old_para_indent = man->standard_tab;
      man->no_nl = 0;
      man->keep_nl = 1;
      man->pending_nl = 1;

      do_fontwords (man, 2, 2, 0);
      break;

    case 15:
    case 5:  /* New para, indent except line 1 */
      do_skipeol (man);
      man->next_line_indent = man->old_para_indent + man->standard_tab;
      man->left_indent = man->old_para_indent;
      man->pending_nl = 1;
      man->keep_nl = 1;
      man->optional_keep = 1;
      break;

    case 4:  /* New para, indent except argument 1 */
      man->next_line_indent = man->old_para_indent + man->standard_tab;
      man->left_indent = man->old_para_indent;
      man->pending_nl = 1;
      man->keep_nl = 1;
      man->optional_keep = 1;
      do_fontwords (man, 1, 1, 1);
      do_skipeol (man);
      break;

    case 14:
      man->pending_nl = 1;
      man->left_indent = man->old_para_indent + man->standard_tab;
      break;
  }

  return 0;
}

static void build_headers (man_t *man) {
  char buffer[5][80];
  int strno = 0;
  unsigned stroff = 0;
  int last_ch = 0, ch, in_quote = 0;

  for (ch = 0; ch < 5; ch++)
    buffer[ch][0] = 0;

  for (;;) {
    if ((ch = man->get_char (man)) == EOF || ch == '\n')
      break;
    if (ch == '"') {
      if (last_ch == '\\') {
        // assert(stroff > 0);
        stroff--;
        break;
      }
      in_quote = !in_quote;
      continue;
    }
    last_ch = ch;
    if (in_quote || !IS_SPACE(ch)) {
      /* Nb, this does nothing about backslashes, perhaps it should */
      if (stroff < sizeof (buffer[strno]) - 1)
        buffer[strno][stroff++] = ch;
      continue;
    }
    buffer[strno][stroff] = 0;

    if (buffer[strno][0]) {
      strno++;
      stroff = 0;
      if (strno == 5)
        break;
    }
  }
  if (strno < 5)
    buffer[strno][stroff] = 0;
  man->unget_char (man, ch);

  /* Ok we should have upto 5 arguments build the header and footer */

  size_t l0 = STRING_BYTELEN(buffer[0]),
         l1 = STRING_BYTELEN(buffer[1]),
         l2 = STRING_BYTELEN(buffer[2]),
         l3 = STRING_BYTELEN(buffer[3]),
         l4 = STRING_BYTELEN(buffer[4]),
         l01 = l0 + l1 + 2;

  STRING_FORMAT(man->line_header, sizeof man->line_header, "%s(%s)%*s%*s(%s)",
         buffer[0],
         buffer[1],
         (int) (man->right_margin / 2 - l01 + l4 / 2 + (l4 & 1)),
         buffer[4],
         (int) (man->right_margin / 2 - l4 / 2 - l01 + l0 - (l4 & 1)),
         buffer[0], buffer[1]);

  STRING_FORMAT(man->doc_footer, sizeof man->doc_footer, "%s%*s%*s(%s)",
         buffer[3],
         (int) (man->right_margin/2-l3+l2/2+(l2&1)),
         buffer[2],
         (int) (man->right_margin/2-l2/2-l01+l0-(l2&1)),
         buffer[0], buffer[1]);

  do_skipeol (man);
}

static void print_word (man_t *man, const char *pword) {
/* Eat   \&  \a .. \z and \A .. \Z
 * \fX   Switch to font X (R B I S etc)
 * \(XX  Special character XX
 * \X    Print as X
 */
// #define checkw(X) assert(d+X<wword+(sizeof wword/sizeof wword[0]))
// #define checkl(X) assert(man->line_ptr+X < man->line+(sizeof man->line/sizeof man->line[0]))
  const char *s;
  int *d, ch = 0;
  int length = 0;
  int wword[256];
  int sp_font = man->cur_font;

  /* Eat and translate characters. */
  for (s = pword, d = wword; *s; s++) {
    ch = 0;
    if (*s == '\n')
      continue;
    if (*s != '\\') {
      // checkw (1);
      *d++ = (ch = *s) + man->cur_font;
      length++;
    } else {
      if (s[1] == 0)
        break;
      s++;
      if (*s == 'f') {
        if (!man->is_tty) s++;
        else if (s[1]) {
          static char fnt[] = " RBI";
          char *p = STRING_CHR(fnt, *++s);
          if (p == 0)
            man->cur_font = 0x100;
          else
            man->cur_font = 0x100 * (p - fnt);
        }
        continue;
      } else if (*s == 's') {
        /* Font size adjusts - strip */
        while (s[1] && STRING_CHR("+-0123456789", s[1]))
          s++;
        continue;
      } else if (IS_ALPHA(*s) || STRING_CHR("!&^[]|~", *s))
        continue;
      else if (*s == '(' || *s == '*') {
      /* XXX Humm character xlate - http://mdocml.bsd.lv/man/mandoc_char.7.html */
        int out = '*';
        if (*s == '*') {
          if (s[1])
            ++s;
        } else if (s[1] == 'm' && s[2] == 'i') {
          out = '-';
          s+=2;
          goto K;
        }
        if (s[1])
          ++s;
        if (s[1])
          ++s;
        K:
        // checkw (1);
        *d++ = out + man->cur_font;
        length++;
        continue;
      }

      // checkw (1);
      *d++ = *s + man->cur_font;
      length++;
    }
  }

  // checkw (1);
  *d = 0;

  if (*wword == 0)
    return;

  if (man->line_ptr)
    if (man->line_ptr + ((man->line_ptr[-1] & 0xFF) == '.') - man->line + length >= man->right_margin - man->left_indent) {
      man->right_adjust = -man->right_adjust;
      line_break (man);
    }

  if (man->line_ptr == 0)
    man->line_ptr = man->line;
  else {
    // assert(man->line_ptr > man->line);
    if (!man->no_fill && (man->line_ptr[-1] & 0xFF) > ' ') {
      if ((man->line_ptr[-1] & 0xFF) == '.') {
        // checkl (1);
        *man->line_ptr++ = man->cur_font + ' ';
      }
      // checkl (1);
      *man->line_ptr++ = sp_font;
      man->gaps_on_line++;
    }
  }

  // checkl (length);
  MEM_COPY(man->line_ptr, wword, length * sizeof (int));
  man->line_ptr += length;
// #undef checkw
// #undef checkl
}

static void line_break (man_t *man) {
  int *d, ch;
  int spg = 1, rspg = 1, spgs = 0, gap = 0;

  if (man->line_ptr == 0)
    return;

  if (man->current_line == 0)
    print_header (man);

  if (man->current_line)
    man->current_line += 1 + man->pending_nl;

  for (; man->pending_nl > 0; man->pending_nl--)
    man->output_string (man, "\n");

  if (man->right_adjust < 0) {
    int over = man->right_margin - man->left_indent - (man->line_ptr - man->line);
    if (man->gaps_on_line && over) {
      spg = rspg = 1 + over / man->gaps_on_line;
      over = over % man->gaps_on_line;
      if (over) {
        if (man->current_line % 2) {
          spgs = over;
          spg++;
        } else {
          spgs = man->gaps_on_line - over;
          rspg++;
        }
      }
    }

    man->right_adjust = 1;
  }

  *(man->line_ptr) = 0;
  if (*man->line)
    for (ch = man->left_indent; ch > 0; ch--)
      man->output_string (man, " ");

  for (d = man->line; *d; d++) {
    ch = *d;
    if ((ch & 0xFF) == 0) {
      int i;
      if (gap++ < spgs)
        i = spg;
      else
        i = rspg;
      for (; i > 0; i--)
        man->output_string (man, " ");
    } else
      switch (ch >> 8) {
        case 2:
          man->output_string (man, "%c\b%c", ch & 0xFF, ch & 0xFF);
          break;

        case 3:
          man->output_string (man, "_\b%c", ch & 0xFF, ch & 0xFF);
          break;

        default:
          man->output_string (man, "%c", ch & 0xFF);
          break;
      }
  }

  man->output_string (man, "\n");

  man->line_ptr = 0;

  if (man->next_line_indent > 0)
    man->left_indent = man->next_line_indent;
  man->next_line_indent = -1;
  man->gaps_on_line = 0;
}

static void page_break (man_t *man) {
  line_break (man);
}

static void print_header (man_t *man) {
  man->pending_nl = 0;
  if (*man->line_header) {
    man->current_line = 1;
    man->output_string (man, "%s\n\n", man->line_header);
  }
}

static void print_doc_footer (man_t *man) {
  line_break (man);
  for (int i = 0; i < 3; i++)
    man->output_string (man, "\n");

  man->output_string (man, "%s", man->doc_footer);
}

man_t *manInit (man_t *man) {
  STRING_CP(man->defaultPath, MAN_DEFAULT_PATH_LEN + 1,
    "/usr/local/share/man:/usr/share/man", MAN_DEFAULT_PATH_LEN);
  STRING_CP(man->defaultSection, MAN_DEFAULT_SECTION_LEN + 1,
    "2:3p:3:1p:1:4:5:6:7:8:0p", MAN_DEFAULT_SECTION_LEN);
  STRING_CP(man->defaultSuffix, MAN_DEFAULT_SUFFIX_LEN + 1,
    ":.gz:.xz", MAN_DEFAULT_SUFFIX_LEN);

  man->line[0]       = '\0';
  man->section[0]    = '\0';
  man->whitespace[0] = '\0';
  man->whitespace[0] = '\0';
  man->doc_footer[0] = '\0';
  man->man_file[0]   = '\0';
  man->line_header[0]= '\0';

  man->optional_keep = 0;
  man->line_ptr      = 0;
  man->no_fill       = 0;
  man->current_line  = 0;
  man->left_indent   = 0;
  man->gaps_on_line  = 0;
  man->keep_nl       = 0;
  man->no_nl         = 1;
  man->catmode       = 1;
  man->right_adjust  = 1;
  man->input_tab     = 8;
  man->standard_tab  = 5;
  man->right_margin  = 65;
  man->cur_font      = 0x100;
  man->next_line_indent = -1;

  man->is_tty = 0;

  man->verbose = 0;
  man->input_fp = man->output_fp = NULL;
  man->get_char = get_char_default;
  man->unget_char = unget_char_default;
  man->output_string = output_string_default;
  return man;
}

int manPage (man_t *man, char *page, const char *section) {
  if (man->input_fp == NULL) {
    if (find_page (man, page, section) < 0) {
      if (section)
        fprintf (stderr, "No entry for %s in section %s of the manual.\n", page, section);
      else
        fprintf (stderr, "No manual entry for %s\n", page);

      return -1;
    }
  }

  int do_pclose_ofd = 0;

  if (man->output_fp == NULL) {

    if (isatty (1)) {    /* If writing to a tty do it to a pager */
      man->is_tty = 1;
      char *pager = GET_ENV("PAGER");
      if (pager) man->output_fp = popen (pager, "w");
      if (!man->output_fp) man->output_fp = popen ("less", "w");
      if (!man->output_fp) man->output_fp = popen ("more", "w");
      if (!man->output_fp) man->output_fp = stdout;
      else {
        do_pclose_ofd = 1;
        struct winsize ws;
        if (!ioctl (0, TIOCGWINSZ, &ws))
          man->right_margin = ws.ws_col > 251 ? 250 : ws.ws_col-2;
        else
          man->right_margin = 78;
      }
    } else
      man->output_fp = stdout;
  }

  do_file (man);

  if (do_pclose_ofd)
    pclose (man->output_fp);

  close_page (man);

  return 0;
}

