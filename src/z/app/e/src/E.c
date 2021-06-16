#define APPLICATION "E"
#define APP_OPTS   "file"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SIGNAL
#define REQUIRE_SYS_TYPES
#define REQUIRE_SYS_STAT
#define REQUIRE_FCNTL
#define REQUIRE_TERMIOS
#define REQUIRE_LOCALE

#define REQUIRE_STRING_TYPE   DECLARE
#define REQUIRE_VSTRING_TYPE  DECLARE
#define REQUIRE_CSTRING_TYPE  DECLARE
#define REQUIRE_READLINE_TYPE DECLARE
#define REQUIRE_SYS_TYPE      DECLARE
#define REQUIRE_TERM_TYPE     DECLARE
#define REQUIRE_FILE_TYPE     DECLARE
#define REQUIRE_PATH_TYPE     DECLARE
#define REQUIRE_DIR_TYPE      DECLARE
#define REQUIRE_ERROR_TYPE    DECLARE
#define REQUIRE_E_TYPE        DECLARE
#define REQUIRE_IMAP_TYPE     DONOT_DECLARE
#define REQUIRE_SMAP_TYPE     DONOT_DECLARE
#define REQUIRE_LA_TYPE       DONOT_DECLARE
#define REQUIRE_PROC_TYPE     DONOT_DECLARE
#define REQUIRE_VIDEO_TYPE    DONOT_DECLARE
#define REQUIRE_VUI_TYPE      DONOT_DECLARE
#define REQUIRE_TERM_MACROS

#include <z/cenv.h>

public void sigwinch_handler (int sig) {
  signal (sig, sigwinch_handler);
  int cur_idx = E.get.current_idx (__E__);

  ed_t *ed = E.get.head (__E__);

  while (ed) {
    Ed.set.screen_size (ed, ScreenDimOpts());
    ifnot (OK is Ed.check_sanity (ed)) {
      __deinit_ed__ (&__E__);
      Stderr.print ("available lines are less than the required\n");
      exit (1);
    }

    win_t *w = Ed.get.win_head (ed);
    while (w) {
      Ed.readjust.win_size (ed, w);
      w = Ed.get.win_next (ed, w);
    }

    ifnot (E.get.next (__E__, ed))
      break;

    ed = E.set.next (__E__);
  }

  ed = E.set.current (__E__, cur_idx);
  win_t *w = Ed.get.current_win (ed);
#ifdef HAS_TEMPORARY_WORKAROUND
  buf_t * buf = Win.get.current_buf (w);
  Buf.draw (buf);
#else
  Win.draw (w);
#endif
}

public void sighup_handler (int sig) {
  (void) sig;
  ed_t *ed = E.get.current (__E__);
  buf_t *buf = Ed.get.current_buf (ed);

  E.set.state_bit (__E__, E_EXIT_ALL_FORCE);
  E.main (__E__, buf);

  __deinit_ed__ (&__E__);
  exit (0);
}

mutable public void __alloc_error_handler__ (int err, size_t size,
                           char *file, const char *func, int line) {
  Stderr.print ("MEMORY_ALLOCATION_ERROR\n");
  Stderr.print_fmt ("File: %s\nFunction: %s\nLine: %d\n", file, func, line);
  Stderr.print_fmt ("Size: %zd\n", size);
  Stderr.print_fmt ("%s\n", Error.errno_string (err));

  ifnot (NULL is __E__) __deinit_ed__ (&__E__);

  exit (1);
}

static string_t *parse_command (char *bytes) {
  string_t *com = String.new (256);
  char *sp = bytes;
  while (*sp) {
    if (*sp isnot ':')
      String.append_byte (com, *sp);
    else {
      if (*(sp+1) isnot ':')
        String.append_byte (com, *sp);
      else {
        String.append_byte (com, ' ');
        sp++;
      }
    }
    sp++;
  }

  return com;
}

static int sys_man (buf_t **bufp, char *word, int section) {
  int retval = NOTOK;
  if (NULL is word) return NOTOK;

  string_t *man_exec = Sys.which ("man", NULL);
  if (NULL is man_exec) return NOTOK;

  ed_t *ed = E.get.current (__E__);

  string_t *com = NULL;

  buf_t *this = Ed.get.scratch_buf (ed);
  Buf.clear (this);

  int flags = (ED_PROC_READ_STDOUT|ED_PROC_READ_STDERR|ED_PROC_WAIT_AT_END);

  if (File.exists (word)) {
    if (Path.is_absolute (word))
      com = String.new_with_fmt ("%s %s", man_exec->bytes, word);
    else {
      char *cwdir = Dir.current ();
      com = String.new_with_fmt ("%s %s/%s", man_exec->bytes, cwdir, word);
      free (cwdir);
    }

    retval = Ed.sh.popen (ed, this, com->bytes, flags, NULL);
    goto theend;
  }

  int sections[9]; for (int i = 0; i < 9; i++) sections[i] = 0;
  int def_sect = 2;

  section = ((section <= 0 or section > 8) ? def_sect : section);
  com = String.new_with_fmt ("%s -s %d %s", man_exec->bytes,
     section, word);

  int total_sections = 0;
  for (int i = 1; i < 9; i++) {
    sections[section] = 1;
    total_sections++;
    retval = Ed.sh.popen (ed, this, com->bytes, flags, NULL);
    ifnot (retval) break;

    while (sections[section] and total_sections < 8) {
      if (section is 8) section = 1;
      else section++;
    }

    String.replace_with_fmt (com, "%s -s %d %s", man_exec->bytes,
        section, word);
  }

theend:
  String.release (com);
  String.release (man_exec);

  Ed.scratch (ed, bufp, 0);
  Buf.substitute (this, ".\b", "", GLOBAL, NO_INTERACTIVE, 0,
      Buf.get.num_lines (this) - 1);
  Buf.normal.bof (this, DRAW);

  return (retval > 0 ? NOTOK : OK);

}

static string_t *__ex_buf_serial_info__ (bufinfo_t *info) {
  string_t *sinfo = String.new_with ("BUF_INFO_STRUCTURE\n");
  String.append_with_fmt (sinfo,
    "fname       : \"%s\"\n"
    "cwd         : \"%s\"\n"
    "parent name : \"%s\"\n"
    "at frame    : %d\n"
    "num bytes   : %zd\n"
    "num lines   : %zd\n"
    "cur idx     : %d\n"
    "is writable : %d\n"
    "autosave    : %ld\n",
    info->fname, info->cwd, info->parents_name, info->at_frame,
    info->num_bytes, info->num_lines, info->cur_idx, info->is_writable,
    info->autosave);

  return sinfo;
}

static string_t *__ex_win_serial_info__ (wininfo_t *info) {
  string_t *sinfo = String.new_with ("WIN_INFO_STRUCTURE\n");
  String.append_with_fmt (sinfo,
    "name         : \"%s\"\n"
    "ed name      : \"%s\"\n"
    "num buf      : %zd\n"
    "num frames   : %d\n"
    "cur buf idx  : %d\n"
    "cur buf name : \"%s\"\n"
    "buf names    :\n",
    info->name, info->parents_name, info->num_items, info->num_frames,
    info->cur_idx, info->cur_buf);

  for (size_t i = 0; i < info->num_items; i++)
    String.append_with_fmt (sinfo, "%12d : \"%s\"\n", i + 1, info->buf_names[i]);

  return sinfo;
}

static string_t *__ex_ed_serial_info__ (edinfo_t *info) {
  string_t *sinfo = String.new_with ("ED_INFO_STRUCTURE\n");
  String.append_with_fmt (sinfo,
    "name         : \"%s\"\n"
    "normal win   : %zd\n"
    "special win  : %d\n"
    "cur win idx  : %d\n"
    "cur win name : \"%s\"\n"
    "win names    :\n",
    info->name, info->num_items, info->num_special_win, info->cur_idx,
    info->cur_win);

  for (size_t i = 0; i < info->num_items; i++)
    String.append_with_fmt (sinfo, "%12d : \"%s\"\n", i + 1, info->win_names[i]);
  return sinfo;
}

static string_t *__ex_e_serial_info__ (Einfo_t *info) {
  string_t *sinfo = String.new_with ("E_INFO_STRUCTURE\n");
  String.append_with_fmt (sinfo,
    "image name   : \"%s\"\n"
    "image file   : \"%s\"\n"
    "num editors  : %d\n"
    "cur ed idx   : %d\n",
    info->image_name, info->image_file, info->num_items, info->cur_idx);
  return sinfo;
}

static int __ex_com_info__ (buf_t **thisp, readline_t *rl) {
  (void) thisp; (void) rl;
  ed_t *ced = E.get.current (__E__);

  int
    buf = Readline.arg.exists (rl, "buf"),
    win = Readline.arg.exists (rl, "win"),
    ed  = Readline.arg.exists (rl, "ed"),
    e   = Readline.arg.exists (rl, "e");

  ifnot (buf + win + ed + e) buf = 1;

  Ed.append.toscratch (ced, CLEAR, "");

  if (buf) {
    bufinfo_t *binfo = Buf.get.info.as_type (*thisp);
    string_t *sbinfo = __ex_buf_serial_info__ (binfo);
    Ed.append.toscratch (ced, DONOT_CLEAR, sbinfo->bytes);
    String.release (sbinfo);
    Buf.release.info (*thisp, &binfo);
  }

  if (win) {
    win_t *cw = Ed.get.current_win (ced);
    wininfo_t *winfo = Win.get.info.as_type (cw);
    string_t *swinfo = __ex_win_serial_info__ (winfo);
    Ed.append.toscratch (ced, DONOT_CLEAR, swinfo->bytes);
    String.release (swinfo);
    Win.release_info (cw, &winfo);
  }

  if (ed) {
    edinfo_t *edinfo = Ed.get.info.as_type (ced);
    string_t *sedinfo = __ex_ed_serial_info__ (edinfo);
    Ed.append.toscratch (ced, DONOT_CLEAR, sedinfo->bytes);
    String.release (sedinfo);
    Ed.release_info (ced, &edinfo);
  }

  if (e) {
    Einfo_t *einfo = E.get.info.as_type (__E__);
    string_t *seinfo = __ex_e_serial_info__ (einfo);
    Ed.append.toscratch (ced, DONOT_CLEAR, seinfo->bytes);
    String.release (seinfo);
    E.release_info (__E__, &einfo);
  }

  Ed.scratch (ced, thisp, NOT_AT_EOF);

  return OK;
}

static int __ex_rline_cb__ (buf_t **thisp, readline_t *rl, utf8 c) {
  (void) thisp; (void) c;
  int retval = RLINE_NO_COMMAND;
  string_t *com = Readline.get.command (rl);

  if (Cstring.eq (com->bytes, "@info")) {
    retval = __ex_com_info__ (thisp, rl);
    goto theend;

  } else if (Cstring.eq (com->bytes, "`man")) {
    Vstring_t *names = Readline.get.arg_fnames (rl, 1);
    if (NULL is names) goto theend;

    string_t *section = Readline.get.anytype_arg (rl, "section");
    int sect_id = (NULL is section ? 0 : atoi (section->bytes));

    retval = sys_man (thisp, names->head->data->bytes, sect_id);
    Vstring.release (names);
  }

theend:
  String.release (com);
  return retval;
}

static void __ex_add_readline_commands__ (ed_t *this) {
  Ed.append.readline_command (this, "`man", 2, READLINE_ARG_FILENAME|READLINE_ARG_VERBOSE );
  Ed.append.command_arg (this, "`man", "--section=", 10);

  Ed.append.readline_command (this, "@info", 0, 0);
  Ed.append.command_arg (this, "@info", "--buf", 5);
  Ed.append.command_arg (this, "@info", "--win", 5);
  Ed.append.command_arg (this, "@info", "--ed",  4);
  Ed.append.command_arg (this, "@info", "--e",   3);

  Ed.set.readline_cb (this, __ex_rline_cb__);
}

static int __ex_word_actions_cb__ (buf_t **thisp, int fidx, int lidx,
                      bufiter_t *it, char *word, utf8 c, char *action) {
  (void) fidx; (void) lidx; (void) action; (void) it; (void) word; (void) thisp;

  int retval = NO_CALLBACK_FUNCTION;
  (void) retval; // gcc complains here for no reason
  switch (c) {
    case 'm':
      retval = sys_man (thisp, word, -1);
      break;

    default:
      break;
   }

  return retval;
}

static void __ex_add_word_actions__ (ed_t *this) {
  int num_actions = 1;
  utf8 chars[] = {'m'};
  char actions[] = "man page";

  Ed.set.word_actions (this, chars, num_actions, actions, __ex_word_actions_cb__);
}

char __ex_balanced_pairs[] = "()[]{}";
char *__ex_NULL_ARRAY[] = {NULL};

char *make_filenames[] = {"Makefile", NULL};
char *make_extensions[] = {".Makefile", NULL};
char *make_keywords[] = {
  "ifeq I", "ifneq I", "endif I", "else I", "ifdef I", "ifndef I", NULL};

char *sh_extensions[] = {".sh", ".bash", NULL};
char *sh_shebangs[] = {"#!/bin/sh", "#!/bin/bash", NULL};
char  sh_operators[] = "+:-%*><=|&()[]{}!$/`?";
char *sh_keywords[] = {
  "if I", "else I", "elif I", "then I", "fi I", "while I", "for I", "break I",
  "done I", "do I", "case I", "esac I", "in I", "EOF I", NULL};
char sh_singleline_comment[] = "#";

char *zig_extensions[] = {".zig", NULL};
char zig_operators[] = "+:-*^><=|&~.()[]{}/";
char zig_singleline_comment[] = "//";
char *zig_keywords[] = {
  "const V", "@import V", "pub I", "fn I", "void T", "try I",
  "else I", "if I", "while I", "true V", "false V", "Self V", "@This V",
  "return I", "u8 T", "struct T", "enum T", "var V", "comptime T",
  "switch I", "continue I", "for I", "type T", "void T", "defer I",
  "orelse I", "errdefer I", "undefined T", "threadlocal T", NULL};

char *lua_extensions[] = {".lua", NULL};
char *lua_shebangs[] = {"#!/bin/env lua", "#!/usr/bin/lua", NULL};
char  lua_operators[] = "+:-*^><=|&~.()[]{}!@/";
char *lua_keywords[] = {
  "do I", "if I", "while I", "else I", "elseif I", "nil I",
  "local I",  "self V", "require V", "return V", "and V",
  "then I", "end I", "function V", "or I", "in V",
  "repeat I", "for I",  "goto I", "not I", "break I",
  "setmetatable F", "getmetatable F", "until I",
  "true I", "false I", NULL
};

char lua_singleline_comment[] = "--";
char lua_multiline_comment_start[] = "--[[";
char lua_multiline_comment_end[] = "]]";

char *dictu_extensions[] = {".du", NULL};
char *dictu_shebangs[] = {"#!/usr/bin/dictu", NULL};
char  dictu_operators[] = "+:-*^><=|&~.()[]{}!@/";
char *dictu_keywords[] = {
  "beg I", "end I", "if I", "while I", "else I", "for I", "do I", "orelse I",
  "is I", "isnot I", "nil E", "not I", "var V", "const V", "return V", "and I",
  "or I", "self F", "this V", "then I", "def F",  "continue I", "break I", "init I", "class T",
  "trait T", "true V", "false E", "import T", "as T", "hasAttribute F", "getAttribute F",
  "setAttribute F", "super V", "type T", "set F", "assert E", "with F", "forever I",
  "use T", "elseif I", "static T",
   NULL};

char dictu_singleline_comment[] = "//";
char dictu_multiline_comment_start[] = "/*";
char dictu_multiline_comment_end[] = "*/";

char *md_extensions[] = {".md", NULL};

char *diff_extensions[] = {".diff", ".patch", NULL};

static char *__ex_diff_syn_parser (buf_t *this, char *line, int len, int idx, row_t *row) {
  (void) idx; (void) row;

  ifnot (len) return line;

  string_t *shared = Buf.get.shared_str (this);

  String.replace_with_len (shared, TERM_COLOR_RESET, TERM_COLOR_RESET_LEN);

  int color = HL_TXT;

  if (Cstring.eq_n (line, "--- ", 4)) {
    color = HL_IDENTIFIER;
    goto theend;
  }

  if (Cstring.eq_n (line, "+++ ", 4)) {
    color = HL_NUMBER;
    goto theend;
  }

  if (line[0] is  '-') {
    color = HL_VISUAL;
    goto theend;
  }

  if (line[0] is  '+') {
    color = HL_STRING_DELIM;
    goto theend;
  }

  if (Cstring.eq_n (line, "diff ", 5) or Cstring.eq_n (line, "index ", 6)
      or Cstring.eq_n (line, "@@ ", 3)) {
    color = HL_COMMENT;
    goto theend;
  }

theend:;
  String.append_with_fmt (shared, "%s%s%s", TERM_MAKE_COLOR(color), line, TERM_COLOR_RESET);
  Cstring.cp (line, MAXLEN_LINE, shared->bytes, shared->num_bytes);
  return line;
}

static ftype_t *__ex_diff_syn_init (buf_t *this) {
  ftype_t *ft= Buf.ftype.set (this, Ed.syn.get_ftype_idx (E.get.current (__E__), "diff"),
    FtypeOpts(.tabwidth = 0, .tab_indents = 0));
  return ft;
}

static char *__ex_syn_parser (buf_t *this, char *line, int len, int idx, row_t *row) {
  return Buf.syn.parser (this, line, len, idx, row);
}

static string_t *__ex_ftype_autoindent (buf_t *this, row_t *row) {
  FtypeAutoIndent_cb autoindent_fun = Ed.get.callback_fun (E.get.current (__E__), "autoindent_default");
  return autoindent_fun (this, row);
}

static string_t *__ex_c_ftype_autoindent (buf_t *this, row_t *row) {
  FtypeAutoIndent_cb autoindent_fun = Ed.get.callback_fun (E.get.current (__E__), "autoindent_c");
  return autoindent_fun (this, row);
}

static ftype_t *__ex_dictu_syn_init (buf_t *this) {
  ftype_t *ft= Buf.ftype.set (this, Ed.syn.get_ftype_idx (E.get.current (__E__), "dictu"),
    FtypeOpts(.autoindent = __ex_c_ftype_autoindent, .tabwidth = 2, .tab_indents = 1));
  return ft;
}

static ftype_t *__ex_lua_syn_init (buf_t *this) {
  ftype_t *ft= Buf.ftype.set (this, Ed.syn.get_ftype_idx (E.get.current (__E__), "lua"),
    FtypeOpts(.autoindent = __ex_c_ftype_autoindent, .tabwidth = 2, .tab_indents = 1));
  return ft;
}

static ftype_t *__ex_make_syn_init (buf_t *this) {
  ftype_t *ft = Buf.ftype.set (this,  Ed.syn.get_ftype_idx (E.get.current (__E__), "make"),
    FtypeOpts(.tabwidth = 4, .tab_indents = 0, .autoindent = __ex_ftype_autoindent));
  return ft;
}

static ftype_t *__ex_sh_syn_init (buf_t *this) {
  ftype_t *ft = Buf.ftype.set (this,  Ed.syn.get_ftype_idx (E.get.current (__E__), "sh"),
    FtypeOpts(.tabwidth = 4, .tab_indents = 0, .autoindent = __ex_ftype_autoindent));
  return ft;
}

static ftype_t *__ex_zig_syn_init (buf_t *this) {
  ftype_t *ft = Buf.ftype.set (this,  Ed.syn.get_ftype_idx (E.get.current (__E__), "zig"),
    FtypeOpts(.tabwidth = 4, .tab_indents = 0, .autoindent = __ex_c_ftype_autoindent));
  return ft;
}

static ftype_t *__ex_md_syn_init (buf_t *this) {
  ftype_t *ft = Buf.ftype.set (this,  Ed.syn.get_ftype_idx (E.get.current (__E__), "md"),
    FtypeOpts(.tabwidth = 4, .tab_indents = 0, .autoindent = __ex_c_ftype_autoindent, .clear_blanklines = 0));
  return ft;
}

/* really minimal and incomplete support */
syn_t ex_syn[] = {
  {
    "make", make_filenames, make_extensions, __ex_NULL_ARRAY,
    make_keywords, sh_operators,
    sh_singleline_comment, NULL, NULL, NULL,
    HL_STRINGS, HL_NUMBERS,
    __ex_syn_parser, __ex_make_syn_init, 0, 0, NULL, NULL, NULL,
  },
  {
    "sh", __ex_NULL_ARRAY, sh_extensions, sh_shebangs,
    sh_keywords, sh_operators,
    sh_singleline_comment, NULL, NULL, NULL,
    HL_STRINGS, HL_NUMBERS,
    __ex_syn_parser, __ex_sh_syn_init, 0, 0, NULL, NULL, __ex_balanced_pairs,
  },
  {
    "zig", __ex_NULL_ARRAY, zig_extensions, __ex_NULL_ARRAY, zig_keywords, zig_operators,
    zig_singleline_comment, NULL, NULL, NULL, HL_STRINGS, HL_NUMBERS,
    __ex_syn_parser, __ex_zig_syn_init, 0, 0, NULL, NULL, __ex_balanced_pairs
  },
  {
    "lua", __ex_NULL_ARRAY, lua_extensions, lua_shebangs, lua_keywords, lua_operators,
    lua_singleline_comment, lua_multiline_comment_start, lua_multiline_comment_end,
    NULL, HL_STRINGS, HL_NUMBERS,
    __ex_syn_parser, __ex_lua_syn_init, 0, 0, NULL, NULL, __ex_balanced_pairs,
  },
  {
    "dictu", __ex_NULL_ARRAY, dictu_extensions, dictu_shebangs, dictu_keywords, dictu_operators,
    dictu_singleline_comment, dictu_multiline_comment_start, dictu_multiline_comment_end,
    NULL, HL_STRINGS, HL_NUMBERS,
    __ex_syn_parser, __ex_dictu_syn_init, 0, 0, NULL, NULL, __ex_balanced_pairs,
  },
  {
    "diff", __ex_NULL_ARRAY, diff_extensions, __ex_NULL_ARRAY,
    __ex_NULL_ARRAY, NULL, NULL, NULL, NULL,
    NULL, HL_STRINGS_NO, HL_NUMBERS_NO,
    __ex_diff_syn_parser, __ex_diff_syn_init, 0, 0, NULL, NULL, NULL
  },
  {
    "md", __ex_NULL_ARRAY, md_extensions, __ex_NULL_ARRAY,
    __ex_NULL_ARRAY, NULL, NULL, NULL, NULL,
    NULL, HL_STRINGS_NO, HL_NUMBERS_NO,
    __ex_syn_parser, __ex_md_syn_init, 0, 0, NULL, NULL, NULL
  }
};

/* This is an example of using a language map, in insert mode, indepentendly
 * of the underlying system. This example is for a greek map and is activated
 * through :set --lang-mode=el. To change back use :set --lang-mode=en.
 */
static utf8 ex_lang_getkey (ed_t *ed, char *mode) {
  (void) ed;

  ifnot (Cstring.eq (mode, "el"))
    return Input.getkey (STDIN_FILENO);

#define ACCENT_CHAR     ';'
#define DIACRITIC_CHAR  ':'
#define ACCENT_MODIFIER            1
#define DIACRITICS_MODIFIER        2
#define ACCENT_DIACRITICS_MODIFIER 3

  int lmap[2][26] = {{
    913, 914, 936, 916, 917, 934, 915, 919, 921, 926, 922, 923, 924,
    925, 927, 928, ':', 929, 931, 932, 920, 937, 931, 935, 933, 918},{
    945, 946, 968, 948, 949, 966, 947, 951, 953, 958, 954, 955, 956,
    957, 959, 960, ';', 961, 963, 964, 952, 969, 962, 967, 965, 950
  }};

  utf8 c = -1;
  int modifier = 0;

  forever {
    c = Input.getkey (STDIN_FILENO);

    if (modifier is ACCENT_MODIFIER) {
      switch (c) {
        case ACCENT_CHAR: return 180;

        case 'A': return 902;
        case 'E': return 904;
        case 'I': return 906;
        case '0': return 908;
        case 'Y': return 910;
        case 'V': return 911;

        case 'a': return 940;
        case 'e': return 941;
        case 'h': return 942;
        case 'i': return 943;
        case 'o': return 972;
        case 'y': return 973;
        case 'v': return 974;
        default:  return -1;
       }
     }

     if (modifier is DIACRITICS_MODIFIER) {
       switch (c) {
         case 'i': return 970;
         case 'y': return 971;
         case DIACRITIC_CHAR : return 168;

         case ACCENT_CHAR:
           modifier = ACCENT_DIACRITICS_MODIFIER;
           continue;

         default: return -1;
       }
     }

     if (modifier is ACCENT_DIACRITICS_MODIFIER) {
       switch (c) {
         case 'y': return 944;
         default: return -1;
       }
     }

    if ('A' <= c and c <= 'Z') {
      return lmap[0][c - 'A'];
    }

    if ('a' <= c and c <= 'z') {
      return lmap[1][c - 'a'];
    }

    if (ACCENT_CHAR is c) {
      modifier = ACCENT_MODIFIER;
      continue;
    }

    if (DIACRITIC_CHAR is c) {
      modifier = DIACRITICS_MODIFIER;
      continue;
    }

    return c;
  }

  return c;
}

static void __init_ext__ (ed_t *this, ed_opts opts) {
  (void) opts;
  __ex_add_readline_commands__ (this);
  __ex_add_word_actions__ (this);

  for (size_t i = 0; i < ARRLEN(ex_syn); i++)
    Ed.syn.append (this, ex_syn[i]);

  Ed.set.lang_getkey (this, ex_lang_getkey);
}

int main (int argc, char **argv) {
  __INIT__ (dir);
  __INIT__ (sys);
  __INIT__ (path);
  __INIT__ (file);
  __INIT__ (term);
  __INIT__ (error);
  __INIT__ (string);
  __INIT__ (vstring);
  __INIT__ (cstring);
  __INIT__ (readline);

  Sys.init_environment (SysEnvOpts());

  __INIT_APP__;

  int ifd = -1;

  ifnot (isatty (fileno (stdin))) {
    /* this looks sufficient but time will tell */
    ifd = dup (fileno (stdin));
    if (NULL is freopen ("/dev/tty", "r", stdin))
      return 1;
  }

  setlocale (LC_ALL, "");
  AllocErrorHandler = __alloc_error_handler__;

  if (NULL is (__E__ = __init_ed__ ("vedas")))
    return 1;

  char
    *load_file = NULL,
    *ftype = NULL,
    *backup_suffix = NULL,
    *exec_com = NULL;

  int
    exit = 0,
    exit_quick = 0,
    filetype = FTYPE_DEFAULT,
    autosave = 0,
    backupfile = 0,
    ispager = 0,
    linenr = 0,
    column = 1,
    num_win = 1;

  argparse_option_t options[] = {
    OPT_HELP (),
    OPT_GROUP("Options:"),
    OPT_STRING(0, "ftype", &ftype, "set the file type", NULL, 0, 0),
    OPT_STRING(0, "backup-suffix", &backup_suffix, "backup suffix (default: ~)", NULL, 0, 0),
    OPT_STRING(0, "exec-com", &exec_com, "execute command", NULL, 0, 0),
    OPT_STRING(0, "load-file", &load_file, "eval file", NULL, 0, 0),
    OPT_INTEGER('+', "line-nr", &linenr, "start at line number", NULL, 0, SHORT_OPT_HAS_NO_DASH),
    OPT_INTEGER(0, "column", &column, "set pointer at column", NULL, 0, 0),
    OPT_INTEGER(0, "num-win", &num_win, "create new [num] windows", NULL, 0, 0),
    OPT_INTEGER(0, "autosave", &autosave, "interval time in minutes to autosave buffer", NULL, 0, 0),
    OPT_BOOLEAN(0, "backupfile", &backupfile, "backup file at the initial reading", NULL, 0, 0),
    OPT_BOOLEAN(0, "pager", &ispager, "behave as a pager", NULL, 0, 0),
    OPT_BOOLEAN(0, "exit", &exit, "exit", NULL, 0, 0),
    OPT_BOOLEAN(0, "exit-quick", &exit_quick, "exit when quiting current buffer", NULL, 0, 0),
    OPT_END()
  };

  PARSE_ARGS;

  if (argc is -1) goto theend;

  E.set.at_init_cb (__E__, __init_ext__);

  int term_flags = 0;

   /* minimal cooperation with libvwn */
  char *vwm_env_exists = getenv ("VWM");
  ifnot (NULL is vwm_env_exists) {
    term_t *term = E.get.term (__E__);
    term_flags = (TERM_DONOT_SAVE_SCREEN|TERM_DONOT_CLEAR_SCREEN|TERM_DONOT_RESTORE_SCREEN);
    Term.set_state_bit (term, term_flags);
  }

  ed_t *this = NULL;
  win_t *w = NULL;

  if (load_file isnot NULL and getuid ()) {
    retval = E.load_file (__E__, load_file, argc, argv);
    ifnot (OK is retval) {
      retval = 1;
      goto theend;
    }

    if (exit) goto theend;

    signal (SIGWINCH, sigwinch_handler);
    signal (SIGHUP, sighup_handler);

    this = E.get.current (__E__);
    ifnot (OK is Ed.check_sanity (this)) {
      retval = 1;
      goto theend;
    }

    if (exit_quick)
      Ed.set.exit_quick (this, 1);

    w = Ed.get.current_win (this);
    goto theloop;
  }

  num_win = (num_win < argc ? num_win : argc);

  this = E.new (__E__, EdOpts(
      .num_win = num_win,
      .term_flags = term_flags));
  Ed.set.at_exit_cb (this, Ed.history.write);

  if (NOTOK is Ed.check_sanity (this)) {
    retval = 1;
    goto theend;
  }

  filetype = Ed.syn.get_ftype_idx (this, ftype);

  w = Ed.get.current_win (this);

  if (0 is argc or ifd isnot -1) {
    /* just create a new empty buffer and append it to its
     * parent win_t to the frame zero */
    buf_t *buf = Win.buf.new (w, BufOpts (
        .ftype = filetype,
        .autosave = autosave,
        .flags = (ispager ? BUF_IS_PAGER : 0)));

    Win.append_buf (w, buf);

    /* check if input comes from stdin */
    if (ifd isnot -1) {
      FILE *fpin = fdopen (ifd, "r");
      fp_t fp = (fp_t) {.fp = fpin};
      Buf.read.from_fp (buf, NULL, &fp);
    }

  } else {
    int
      first_idx = Ed.get.current_win_idx (this),
      widx = first_idx,
      l_num_win = num_win;

    /* else create a new buffer for every file in the argvlist */
    for (int i = 0; i < argc; i++) {
      buf_t *buf = Win.buf.new (w, BufOpts(
          .fname = argv[i],
          .ftype = filetype,
          .at_frame = FIRST_FRAME,
          .at_linenr = linenr,
          .at_column = column,
          .backupfile = backupfile,
          .backup_suffix = backup_suffix,
          .autosave = autosave,
          .flags = (ispager ? BUF_IS_PAGER : 0)));

      Win.append_buf (w, buf);

      if (--l_num_win > 0)
        w = Ed.set.current_win (this, ++widx);
    }

    w = Ed.set.current_win (this, first_idx);
  }

  /* set the first indexed name in the argvlist, as current */
  Win.set.current_buf (w, 0, DRAW);

  signal (SIGWINCH, sigwinch_handler);
  signal (SIGHUP, sighup_handler);

  if (exec_com isnot NULL) {
    string_t *com = parse_command (exec_com);
    readline_t *rl = Ed.readline.new_with (this, com->bytes);
    rl->state |= READLINE_EXEC;
    buf_t *buf = Ed.get.current_buf (this);
    Buf.readline (&buf, rl);
    String.release (com);
  }

  if (exit) {
    if (retval <= NOTOK) retval = 1;
    goto theend;
  }

  if (exit_quick)
    Ed.set.exit_quick (this, 1);

theloop:;
  for (;;) {
    buf_t *buf = Ed.get.current_buf (this);

    retval = E.main (__E__, buf);

    if (E.test.state_bit (__E__, E_EXIT))
      break;

    if (E.test.state_bit (__E__, E_SUSPENDED)) {
      if (E.get.num (__E__) is 1) {
        /* as an example, we simply create another independed instance */
        this = E.new (__E__, EdOpts());
        Ed.set.at_exit_cb (this, Ed.history.write);

        w = Ed.get.current_win (this);
        buf = Win.buf.new (w, BufOpts());
        Win.append_buf (w, buf);
        Win.set.current_buf (w, 0, DRAW);
      } else {
        /* else jump to the next or prev */
        int prev_idx = E.get.prev_idx (__E__);
        this = E.set.current (__E__, prev_idx);
        w = Ed.get.current_win (this);
      }

      continue;
    }

    break;
  }

theend:
  __deinit_ed__ (&__E__);
  return retval;
}
