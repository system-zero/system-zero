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
  :@info [--buf,--win,--ed] (with no arguments defaults to --buf) (this prints
                          details to the scratch buffer of the corresponded arguments)
  :`man [--section=int] manpage  (display man page on the scratch buffer)
  :q[!] [--global]        (quit (if force[!], do not check for modified buffers),
                              (if --global exit from all running editor instances))

  Unified Diff:
  This feature requires (for now) the `diff' utility.

  The :diff command open a dedicated "diff" buffer, with the results (if any) of
  the differences (in a unified format), between the buffer in memory with the one
  that is written on the disk.
  Note that it first clears the previous diff if any.

  The :diff command can take an "--origin" argument. In this case the command will
  display any differences, between the backup file and and the buffer in memory.
  For that to work the "backfile" option should be set (either on invocation or by
  using the :set --backupfile command first with the conjunction with the :@bufbackup
  command. Otherwise a warning message should be displayed.

  The :diffbuf command gives the focus to this "diff" buffer. Note, that this buffer
  can be quickly closed with 'q', line in a pager (likewise for the other special
  buffers, like the message buffer or the scratch buffer).

  Another usage of this feature is when quiting normally with :quit (without forcing) 
  and the buffer has been modified.
  In that case a dialog (below) presents some options:

    "[bufname] has been modified since last change
     continue writing? [yY|nN], [cC]ansel, unified [d]iff?"

    on 'y': write the buffer and continue
    on 'n': continue without writing
    on 'c': cancel operation at this point (some buffers might be closed already)
    on 'd': print to the stdout the unified diff and redo the question (note that
            when printing to the stdout, the previous terminal state is restored;
            any key can bring back the focus)

  UTF-8 Validation:
  There are two ways to check for invalid UTF-8 byte sequences.
  1. using the command :@validate_utf8 filename
  2. in visual linewise mode, by pressing v or through tab completion

  In both cases any error is redirected to the scratch buffer. It doesn't
  and (probably) never is going to do any magic, so the function is mostly
  only informational (at least for now).
  Usually any such invalid byte sequence is visually inspected as it messes
  up the screen.

  The code for this functionality is from the is_utf8 project at:
  https://github.com/JulienPalard/is_utf8
  specifically the is_utf8.c unit and the is_utf8() function
  Many Thanks.

  Copyright (c) 2013 Palard Julien. All rights reserved.
  but see src/lib/utf8/is_utf8.c for details.

  Regexp:
  This library uses a slightly modified version of the slre machine, which is an
  ISO C library that implements a subset of Perl regular expression syntax, see
  and clone at:

  https://github.com/cesanta/slre.git
  Many thanks.

  The substitution string in the ":substitute command", can use '&' to denote the
  full captured matched string.

  For captured substring a \1\2... can be used to mean, `nth' captured substring
  numbering from one.

  It is also possible to force caseless searching, by using (like pcre) (?i) in front
  of the pattern. This option won't work with multibyte characters. Searching for
  multibyte characters it should work properly though.

  To include a white space, the string should be (double) quoted. In that case a
  literal double quote '"', should be escaped. Alternatively a \s can be used to
  include a white space.

  Re Syntax.
    ^       Match beginning of a buffer
    $       Match end of a buffer
    ()      Grouping and substring capturing
    \s      Match whitespace
    \S      Match non-whitespace
    \d      Match decimal digit
    \n      Match new line character
    \r      Match line feed character
    \f      Match form feed character
    \v      Match vertical tab character
    \t      Match horizontal tab character
    \b      Match backspace character
    +       Match one or more times (greedy)
    +?      Match one or more times (non-greedy)
    *       Match zero or more times (greedy)
    *?      Match zero or more times (non-greedy)
    ?       Match zero or once (non-greedy)
    x|y     Match x or y (alternation operator)
    \meta   Match one of the meta character: ^$().[]*+?|\
    \xHH    Match byte with hex value 0xHH, e.g. \x4a
    [...]   Match any character from set. Ranges like [a-z] are supported
    [^...]  Match any character but ones from set

  A pattern can start with (?i) to denote `ignore case`

  Registers and Marks:
  Both are supported but with the minimal features (same with other myriad details
  that needs care).

    Mark set:
    [abcdghjklqwertyuiopzxcvbnm1234567890]
    Special Marks:
    - unnamed mark [`] jumps to the previous position

    Register set:
    [abcdghjklqwertyuiopzxsvbnm1234567890ABCDGHJKLQWERTYUIOPZXSVBNM]

    Special Registers:
    - unnamed ["] register (default)
    - current filename [%] register
    - last search [/] register
    - last command line [:] register
    - registers [+*] send|receive text to|from X clipboard (if xclip is available)
    - blackhole [_] register, which stores nothing
    - expression [=] register (experimental) (runtime code evaluation)
    - CTRL('w') current word
    - shared [`] register (accessed by all the editor instances)

  Note that for uppercase [A-Z], the content is appended to the current content,
  while for the [a-z] set, any previous content is replaced.
  An uppercase register can be cleared, by using in Normal mode the "-" command,
  prefixed with '"' and the register letter, e.g., "Z- for the "Z" register.

  History Completion Semantics (command line and search):
   - the ARROW_UP key starts from the last entry set in history, and scrolls down
     to the past entries

   - the ARROW_DOWN key starts from the first entry, and scrolls up to most recent

  Glob Support:
    (for now)
    - this is limited to just one directory depth
    - it uses only '*'
    - and understands (or should):
      `*'
      `/some/dir/*'
      `*string' or `string*string' or `string*'
        (likewise for directories)

    Note: many commands have a --recursive option

  Menus:
  Many completions (and there are many) are based on menus.
    Semantics and Keys:
    Navigation keys:
    - left and right (for left and right motions)
      the left key should move the focus to the previous item on line, unless the
      focus is on the first item in line, which in that case should focus to the
      previous item (the last one on the previous line, unless is on the first line
      which in that case should jump to the last item (the last item to the last
      line))

    - the right key should jump to the next item, unless the focus is on the last
      item, which in that case should focus to the next item (the first one on the
      next line, unless is on the last line, which in that case should jump to the
      first item (the first item to the first line))

    - page down/up keys for page down|up motions

    - tab key is like the right key

    Decision keys:
    - Enter accepts selection; the function should return the focused item to the
      caller

    - Spacebar can also accept selection if it is enabled by the caller. That is
      because a space can change the initial pattern|seed which calculates the
      displayed results. But using the spacebar speeds a lot of those operations,
      so in most cases is enabled, even in cases like CTRL('n') in insert mode.

    - Escape key aborts the operation

  In all the cases the window state should be restored afterwards.

  The sample Application that provides the main() function, can also read
  from standard input to an unnamed buffer. Thus it can be used as a pager:

     git diff "$@" | vedas --ftype=diff --pager "$@"

  Searching Files:
  This is a really quite basic emulation of quickfix vim's windows, written quite
  early at the development, so there has to be some discipline when is being used,
  as there a couple of things that need care.

  The command :vgrep it takes a pattern and at least a filename as argument[s]:

    :vgrep --pat=`pattern' [-r|--recursive] file[s]

  This should open a unique window intended only for searches and re-accessible
  with:

    :searches  (though it might be wise a `:copen' alias (to match vim's expectation))

  This window should open a frame at the bottom, with the results (if any) and it
  will set the pointer to the first item from the sorted and unique in items list.

  A carriage return should open the filename at the specific line number at the
  frame 0.

  A `q' on the results frame (the last one), will quit the window and focus again
  to the previous state (as it acts like a pager).

  This command can search recursively and skips (as a start) any object file.

  Note that because it is a really basic implementation, some unexpected results
  might occur, if there is no usage discipline of this feature (for instance :bd
  can bring some confusion to the layout and the functionality).

  Spelling:
  The application can provide spelling capabilities, using very simple code, based
  on an idea by Peter Norvig at:
  http://norvig.com/spell-correct.html

  The algorithms for transforming the `word', except the case handling are based
  on the checkmate_spell project at: https://github.com/syb0rg/checkmate

  Almost same code at: https://github.com/marcelotoledo/spelling_corrector

  Copyright  (C)  2007  Marcelo Toledo <marcelo@marcelotoledo.com>

  Version: 1.0
  Keywords: spell corrector
  Author: Marcelo Toledo <marcelo@marcelotoledo.com>
  Maintainer: Marcelo Toledo <marcelo@marcelotoledo.com>
  URL: http://marcelotoledo.com

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The word database is from:
  https://github.com/first20hours/google-10000-english
  Data files are derived from the Google Web Trillion Word Corpus, as described
  by Thorsten Brants and Alex Franz
  http://googleresearch.blogspot.com/2006/08/all-our-n-gram-are-belong-to-you.html
  and distributed by the Linguistic Data Consortium:
  http://www.ldc.upenn.edu/Catalog/CatalogEntry.jsp?catalogId=LDC2006T13.
  Subsets of this corpus distributed by Peter Novig:
  http://norvig.com/ngrams/
  Corpus editing and cleanup by Josh Kaufman.

  The above dictionary contains the 10000 most frequently used english words,
  and can be extended through the application by pressing 'a' on the dialog.

  This implementation offers ways to check for mispelling words.
  1. using the command line :spell --range=`range'
  2. on visual linewise mode, by pressing `S' or by using tab completion
  3. on visual characterize mode, by pressing `S' or by using tab completion
  4. on 'W' in normal mode
  5. on 'F' in normal mode (file operation mode)

  As it is a very simple approach with really few lines of code, it it is obvious
  that there is not guarantee, that will find and correct all the mispelled words
  in the document, though it can be taught with proper usage.

  This is the initial implementation and some more work is needed to make it more
  accurate, though for a start is serves its purpose quite satisfactory, plus there
  is no requirement and already helped me to this document.

  Saving and Restoring current layout.
  Setting at the runtime:

    :set --save-image=1 --image-name=name --persistent-layout=1

  This will save the current editor, window and buffer instances.
  If not a name was given, by default will be the basename (minus the extension) of
  the current buffer.

  To restore the saved layout, issue on invocation:

     E --load-file=name
</pre>
