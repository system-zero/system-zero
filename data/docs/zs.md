This is a quite minimal shell implementation, at very early stage of development,
that does just the basics.  
  
<pre>
Semantics:  
  Shell functionality:  
  
    - short-Circuit Operators “&&” and “||” AND-OR operators.  
      “&&” executes the first command, and then executes the second  
      command if and only if the exit status of the first command is zero.   
      “||” is similar, but executes the  second command if and only if   
      the exit status of the first command is nonzero. Example:  
  
      test -d /tmp/dir || mkdir -v /tmp/dir && cp -v somefile /tmp/dir  
  
      Note that quite possible there are combinations that maybe violate  
      those semantics.  
  
    - pipes:  
  
      ls | grep pat  
  
    - supported redirection (operators):  
  
      - redirect stdout to a filename  
  
        command >/filename  
  
      - redirect stdout to an existing filename (overwrite)  
  
        command >|/filename  
  
      - redirect stderr to a filename  
  
        command 2>/filename  
  
      - redirect stderr and stdout in a filename  
  
        command >&/filename  
  
      - likewise but overwrite existing filename  
  
        command >&|/filename  
  
      - redirect stderr to stdout in a pipeline  
  
        command 2>&1 | next_command  
  
  Readline functionality:  
    - Multiline mode  
    - UTF8-support  
    - History set/load/save  
    - Filename, and specific to this system, Command and Argument tab completion.  
  
      Command completion is triggered at the beggining of the line or when the cursor  
      is at the command token.  
      Argument completion is triggered when the last character is a dash (-) which it  
      is not at the beggining of the line or part of the command, and it is preceded  
      by a space.  
      Otherwise a filename completion is performed.  
  
    - Last component completion.  
      This is triggered with CTRL('/') or CTRL('-') and completes with the last space  
      and end of line delimited components of previous command lines.  
  
    - Command completion with the initial character.  
      This is done automatically if the line is empty and the first char is at  
      the [A-Z] range.  
  
    Note that in the case of multiply completion items, a hint that indicates the number  
    of items is printed right to the cursor. However, in this implementation it is not  
    visually possible to expand all the items in the screen at once.  

    The underlying machine is linenoise with UTF-8 support, but in this implementation  
    the API is incompatible, just to support tab completion at any cursor point in the line,  
    and not just at the end of the line. Also there are two more callback functions, one that  
    are called immediately after a received input and before any processing, and the other  
    on a carriage return.  
    Generally speaking, the tendency is for a bit aggressive interaction.  
  
  Word expansion:  
    Tilde expansion:  
      This expansion is performed when the first char of an argument is a '~', and
      which it is replaced by the user's home directory.  
    
    Environment variable substitution:
      This expansion is performed on ${env} token, which it is replaced with the
      value of the environment variable if found, otherwise with an empty string.
    
    Command substitution:  
      This expansion is performed on $(command) token, which it is replaced with the  
      output of the command.  
    
    Pathname expansion:
      Finally a pathname expansion it is performed using glob().
   
  Keys:  
    BACKSPACE: remove char at left of cursor  
       DELETE: remove char at right of cursor  
       CTRL_T: swaps the current character with the previous  
       CTRL_B:  
         LEFT: move left  
       CTRL_F:  
        RIGHT: move right  
         DOWN:  
       CTRL_N: next item in history  
           UP:  
       CTRL_P: previous item in history  
       CTRL_R: reverse incremental search  
         HOME: move cursor to the start of line  
       CTRL_A: move cursor to the start of line  
          END:  
       CTRL_E: move cursor at the end of line  
       CTRL_U: delete the whole line  
       CTRL_K: delete from current to end of line  
       CTRL_L: clear screen  
       CTRL_W: delete previous word  
       CTRL_D: remove char at right of cursor, or if the line is empty, act as end-of-file  
       CTRL_Y: insert previous deleted characters  
       CTRL_V: insert a control character  
          TAB: tab completion for commands, arguments and filenames  
       CTRL_/:  
       CTRL_-: last component completion   
       CTRL_C: interrupts a command if it is running, otherwise it adds a line  
       ESCAPE: on tab completion it restores original line state, otherwise it clears  
               the line  

  Builtin commands:  
    cd:
      Changes the current working directory. Without arguments the user's home directory  
      is assumed. With an "-" as argument, then it tries to change to the previous  
      working directory if any. When succesful the value of the current working directory  
      it is exported in the environment as "PWD".
   
   pwd:
     This prints the current working directory.  
   
   exit:  
     Exits back to the environment.  
     
Quirks:  
  A filename with embedded whitespace should be enclosed into double quotes.  
  Also in this same case, filename completion stops if the filename is a directory.  
  
  When tab completion returns more than one item, a hint that shows the number  
  of items, is displayed to the right of the cursor. There is no way to display  
  more than one item at once.  

  There is no job managment.  
</pre>
