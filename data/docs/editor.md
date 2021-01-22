<pre>
  Normal mode:
 |
 |   key[s]          |  Semantics                     | count
 | __________________|________________________________|_______
 | CTRL-b, PG_UP     | scroll one page backward       | yes
 | CTRL-f, PG_DOWN   | scroll one page forward        | yes
 | HOME  gg          | home row                       |
 | END,  G           | end row                        |
 | h,l               | left|right cursor              | yes
 | ARROW[LEFT|RIGHT] | likewise                       | yes
 | k,j               | up|down line                   | yes
 | ARROW[UP|DOWN]    | likewise                       | yes
 | $                 | end of line                    |
 | 0                 | beginning of line              |
 | ^                 | first non blank character      |
 | count [gG]        | goes to line                   |
 | gf                | edit filename under the cursor |
 | e                 | end of word (goes insert mode) | yes
 | E                 | end of word                    | yes
 | ~                 | switch case                    |
 | m[mark]           | mark[a-z]                      |
 | `[mark]           | mark[a-z]                      |
 | CTRL-A            | increment (dec|octal|hex|char) | yes
 | CTRL-X            | decrement (likewise)           | yes
 | >, <              | indent [in|out]                | yes
 | [yY]              | yank [char|line]wise           | yes
 | [pP]              | put register                   |
 | d[d]              | delete line[s]                 | yes
 | d[g|HOME]         | delete lines to the beg of file|
 | d[G|END]          | delete lines to the end of file|
 | dw                | delete word                    |
 | cw                | change word                    |
 | ci[char]          | change inner text delimited by [char]|
 | x|DELETE          | delete character               | yes
 | D                 | delete to end of line          |
 | X|BACKSPACE       | delete character to the left   | yes
 | - BACKSPACE and if set and when current idx is 0, deletes trailing spaces|
 | - BACKSPACE and if set is like insert mode         |
 | r                 | replace character              |
 | C                 | delete to end of line (insert) |
 | J                 | join lines                     |
 | i|a|A|o|0         | insert mode                    |
 | u                 | undo                           |
 | CTRL-R            | redo                           |
 | CTRL-L            | redraw current window          |
 | V                 | visual linewise mode           |
 | v                 | visual characterize mode       |
 | CTRL-V            | visual blockwise mode          |
 | /                 | search forward                 |
 | ?                 | search backward                |
 | *                 | search current word forward    |
 | #                 | search current word backward   |
 | n                 | search next                    |
 | N                 | search Next (opposite)         |
 | CTRL-w            |                                |
 |   - CTRL-w        | frame forward                  |
 |   - w|j|ARROW_DOWN| likewise                       |
 |   - k|ARROW_UP    | frame backward                 |
 |   - o             | make current frame the only one|
 |   - s             | split                          |
 |   - n             | new window                     |
 |   - h|ARROW_LEFT  | window to the left             |
 |   - l|ARROW_RIGHT | window to the right            |
 |   - `             | previous focused window        |
 | g                 |                                |
 |   - g             | home row                       |
 |   - v             | visual linewise mode, with the |
 |                 previous selected rows if any      |
 |   - f             | open filename under the cursor |
 |        (on C filetype, can open header <header.h>) |
 | :                 | command line mode              |

 | Normal Mode Extensions or different behavior with vim.|
 | q                 | like :quit  if the buffer type is |
 |                     set to pager                      |
 | g                 |                                   |
 |   - b             | open link under the cursor to the |
 |             browser: requires the elinks text browser |
 |             to be installed, and it uses the -remote  |
 |             elinks option, so elinks should be running|
¹| CTRL-j            | detach editor and gives control to|
 |             the caller, it can be reatached with the  |
 |             exact status                              |
 | CTRL-O|CTRL-I     | jump to the previus|next location |
 |             to the jump list, (this differs from vim, |
 |             as this is like scrolling to the history) |
 |                                                       |
 | W                 | word operations mode (via a selection menu)|                            |
 |   - send `word' on XCLIPBOARD                         |
 |   - send `word' on XPRIMARY                           |
 |   - swap case                                         |
 |   - to lower                                          |
 |   - to upper                                          |
 |                                                       |
 | L                 | Line operations mode (via a selection menu)|
 | F                 | File operations mode (via a selection menu)|
 |                                                       |
 | ,                 |                                   |
 |   - n             | like :bn (next buffer)            |  see Command mode
 |   - m             | like :bp (previous buffer)        |      -||-
 |   - ,             | like :b` (prev focused buffer)    |      -||-
 |   - .             | like :w` (prev focused window)    |      -||-
 |   - /             | like :winext   (next window)      |      -||-
 |   - ;             | like :ednext   (next editor)      |      -||-
 |   - '             | like :edprev   (prev editor)      |      -||-
 |   - l             | like :edprevfocused (prev focused ed)|   -||-

Insert mode:
 |
 |   key[s]          |  Semantics                     | count
 |___________________|________________________________|_______
 | CTRL-y            | complete based on the prec line|
 | CTRL-e            | complete based on the next line|
 | CTRL-a            | last insert                    |
 | CTRL-x            | completion mode                |
 |   - CTRL-l or l   | complete line                  |
 |   - CTRL-f or f   | complete filename              |
 | CTRL-n            | complete word                  |
 | CTRL-v            | insert character (utf8 code)   |
 | CTRL-k            | insert digraph                 |
 | CTRL-r            | insert register contents (charwise only) |
 | motion normal mode commands with some differences explained bellow|
 | HOME              | goes to the beginning of line  |
 | END               | goes to the end of line        |
 | escape            | aborts                         |

Visual mode:
 |   key[s]          |  Semantics                     | count
 |___________________|________________________________|_______
 | >,<               | indent [in|out]                | yes
 | d                 | delete selected                |
 | y                 | yank selected                  |
 | s                 | search selected [linewise]     |
 | w                 | write selected [linewise]      |
 | i|I               | insert in front [blockwise]    |
 | c                 | change [blockwise]             |
 | both commands above use a readline instance (but without tab|history completion)|
 | x|d               | delete [(block|char)wise]      |
 | +                 | send selected to XA_CLIPBOARD [(char|line)wise|
 | *                 | send selected to XA_PRIMARY   [(char|line)wise|
 | e                 | edit as filename [charwise]    |
 | b                 | check for unbalanced pair of objects [linewise]|
 | v                 | check line[s] for invalid UTF-8 byte sequences [linewise]
 | TAB               | triggers a completion menu with the correspondent to the
 |                     specific mode above actions    |
 | escape            | aborts                         |
 | HOME|END|PAGE(UP|DOWN)|G|ARROW(RIGHT|LEFT|UP|DOWN) |
 | extend or narrow the selected area (same semantics with the normal mode)

Command line mode:
 |   key[s]          |  Semantics                     |
 |___________________|________________________________|
 | carriage return   | accepts                        |
 | escape            | aborts                         |
 | ARROW[UP|DOWN]    | search item on the history list|
 | ARROW[LEFT|RIGHT] | left|right cursor              |
 | CTRL-a|HOME       | cursor to the beginning        |
 | CTRL-e|END        | cursor to the end              |
 | DELETE|BACKSPACE  | delete next|previous char      |
 | CTRL-r            | insert register contents (charwise only)|
 | CTRL-l            | clear line                     |
 | CTRL-/ |CTRL-_    | insert last component of previous command|
 |   can be repeated for: RLINE_LAST_COMPONENT_NUM_ENTRIES (default: 10)|
 | TAB               | trigger completion[s]          |

Search:
 |   key[s]          |  Semantics                     | count
 |___________________|________________________________|_______
 | CTRL-n            | next                           |
 | CTRL-p            | previous                       |
 | carriage return   | accepts                        |
 | escape            | aborts                         |

  In this implementation while performing a search, the focus do not change
  until user accepts the match. The results and the dialog, are shown at the
  bottom lines (the message line as the last line on screen).
  It searches just once in a line, and it should highlight the captured string
  with a proper message composed as:

    |line_nr byte_index| matched line

  See at Regexp section for details.

  Line operation mode:
  This is triggered with 'L' in normal mode and for now can:
    - send current line to shared register
    - send current line to XA_CLIPBOARD (a LN appended)
    - send current line to XA_CLIPBOARD (NO LN appended)
    - send current line to XA_PRIMARY (a LN appended)
    - send current line to XA_PRIMARY (NO LN appended)

  File operation mode:
  This is triggered with 'F' in normal mode and for now can:
    - validate current buffer for invalid UTF8 byte sequences
    - write this file

  Command line mode:
  (note) Commands do not get a range as in vi[like], but from the command line
  switch --range=. Generally speaking the experience in the command line should
  feel more like a shell and specifically the zsh completion way.

  On Normal mode, it is possible to map native language to normal mode commands.
  Here is a sample:

  int lmap[2][26] = {{
    913, 914, 936, 916, 917, 934, 915, 919, 921, 926, 922, 923, 924,
    925, 927, 928, ':', 929, 931, 932, 920, 937, 931, 935, 933, 918},{
    945, 946, 968, 948, 949, 966, 947, 951, 953, 958, 954, 955, 956,
    957, 959, 960, ';', 961, 963, 964, 952, 969, 962, 967, 965, 950
  }};

  Ed.set.lang_map (this, lmap);

  These correspond to 'A'-'Z' and 'a'-'z' respectively.

  Auto completions (triggered with tab):
    - commands
    - arguments
    - filenames

  Command completion is triggered when the cursor is at the first word token.
  (note: and also by default, and when the first char is one of the "`~@" set).

  Arg completion is triggered when the first char word token is an '-' or
  when the current command, gets a bufname as an argument.

  In any other case a filename completion is performed.

  note: that if an argument (like a substitution string) needs a space, it should be
  quoted

  If a command takes a filename or a bufname as an argument, tab completion
  will quote by default the argument (for embedded spaces): this mechanism
  uses the --fname= argument.

  Options are usually long (that means prefixed with two dashes), unless some
  established/unambiguous like (for now):
    -i  for interactive
    -r  for recursive

  Default command line switches:
    --range=...
      valid ranges:
      --range=%              for the whole buffer
      --range=linenr,linenr  counted from 1
      --range=.              for current line
      --range=[linenr|.],$   from linenr to the end
      --range=linenr,. from  linenr to current line
     without --range         assumed current line number

    --global          is like the g flag on vim substitute
    --interactive,-i  is like the c flag on vim substitute
    --append          is like  >> redirection (used when writing to another file)

    --pat=`pat'       pat is a string that describes the pattern.
                     For more details see at Regexp section below to this document.

    --sub=`replacement string' but see at Regexp section for details.

  Commands:
  ! as the last character indicates force, unless is a shell command.

  :s[ubstitute] [--range=] --pat=`pat' --sub=`sub' [-i,--interactive] [--global]
  :w[rite][!] [filename  [--range] [--append]] (when the buffer is modified externally this aborts or raises a question if `force')
  :w[rite]!!  [filename  [--range] [--append]] (likewise, but write unconditionally)
  :wq[!]                 (write and quit (if force, do not check for modified buffers))
  :e[!] [filename]       (when e!, reread from current buffer filename)
  :enew [filename]       (new buffer on a new window)
  :etail                 (like :e! and 'G' (reload and go at the end of file))
  :split [filename]      (open filename at a new frame)
  :b[uf]p[rev]           (buffer previous)
  :b[uf]n[ext]           (buffer next)
  :b[uf][`|prevfocused]  (buffer previously focused)
  :b[uf]d[elete][!]      (buffer delete)
  :w[in]p[rev]           (window previous)
  :w[in]n[ext]           (window next)
  :w[in][`|prevfocused]  (window previously focused)
  :ednew|ednext|edprev|edprevfocused
                         (likewise but those are for manipulating editor instances,
                          ednew can use a filename as argument)
  :r[ead] filename       (read filename into current buffer)
  :r! cmd                (read into buffer cmd's standard output)
  :!cmd                  (execute command)
  :diff [--origin]       (shows a unified diff in a diff buffer, see Unified Diff)
  :diffbuf               (change focus to the `diff' window/buffer)
  :vgrep --pat=`pat' [--recursive] fname[s] (search for `pat' to fname[s])
  :redraw                (redraw current window)
  :searches              (change focus to the `search' window/buffer)
  :messages              (change focus to the message window/buffer)
  :testkey               (test keyboard keys)
  :set option            (set option for current buffer and control editor behavior)
                          --ftype=`ftype' set `ftype' as filetype
                          --tabwidth=[int] set tabwidth
                          --shiftwidth=[int] set shiftwidth
                          --backupfile set backup
                          --backup-suffix=[string] set backup suffix (default: ~)
                          --no-backupfile unset the backup option
                          --autosave=[int] set in minutes the interval, (used
                            at the end of insert mode to autosave buffer)
                          --enable-writing this will enable writing (buffer contents)
                          --save-image=[1|0] enable saving editor layout at exit
                          --image-file=`file' save image to `file'
                          --image-name=`name' save image as `name'
                          --persistent-layout=[1|0] [en|dis]able persistent editor layout
  :@balanced_check [--range=] (check for unbalanced pair of objects, without `range'
                          whole file is assumed)
  :@bufbackup             (backup file as (dirname(fname)/.basename(fname)`suffix',
                          but it has to be set first (with :set or with --backupfile),
                          if backupfile exists, it raises a question, same if this is
                          true at the initialization)
  :@validate_utf8 filename (check filename for invalid UTF-8 byte sequences)
  :@save_image [--as=file] (save current layout, that can be used at a next invocation
                            with --load-file=file.i, to restore it,
                            default filename: $SYSDATADIR/images/currentbufname.i)
  :@edit_image            (edit the current process image script (if exists))
  :q[!] [--global]        (quit (if force[!], do not check for modified buffers),
                              (if --global exit from all running editor instances))
</pre>
