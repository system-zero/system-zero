This is a quite minimal shell implementation, that does just the basics with a
decent interactivity.  
  
## Semantics: 
<pre>
  Shell functionality:  
  
    - short-Circuit Operators “&&” and “||” AND-OR operators.  
      “&&” executes the first command, and then executes the second  
      command if and only if the exit status of the first command is zero.   
      “||” is similar, but executes the  second command if and only if   
      the exit status of the first command is nonzero. Example:  
  
      test -d /tmp/dir || mkdir -v /tmp/dir && cp -v somefile /tmp/dir  
  
      Note that possible exist combinations that might violate those semantics.  
  
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
  
    The underlying machine is linenoise with UTF-8 support, but in this implementation  
    the API is incompatible, just to support tab completion at any cursor point in the line,  
    and not just at the end of the line.
   
    Also there are two more callback functions, one that is called immediately after
    a received input and before any processing, and the other on a carriage return.  

    Generally speaking, the tendency is for an aggressive interaction.  
  
  Word expansion:  
    Tilde expansion:  
      This expansion is performed when the first char of an argument is a '~', and
      which it is replaced by the user's home directory.  
    
    Environment variable substitution:
      This expansion is performed on ${env} token, which it is replaced with the
      value of the environment variable if found, otherwise with an empty string.
    
      Special environment variables:  
        ${?}           : exit value from the last command  
        ${?.to_string} : string represantation of the last exit value  

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
         HOME:
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
      is assumed.
      
      With a "-[digit*]" as argument, then the command tries to change to the given
      depth of previously working directories.

      When the command succeeds, the value of the current working directory it is
      exported in the environment as "PWD".
      
      Note that, if the length of the argument list is 1 and the first item on the
      list is a directory, it is also considered as a "cd" command and the item it is
      the intented directory.
      
      Note that even infinity ../../.. that goes way behind / it is considered as / (zsh does the same)
   
   pwd:  
     This prints the current working directory.  
   
   unsetenv: Deletes the argument `name' from the environment.  
   
   exit:  
     Exits back to the environment. Without argument it exits with a zero value. If
     the argument is less than zero, it returns 1, otherwise it returns the value
     of the argument, unless is greater than 127 and which in that case it is 127.

  Syntax:  
   
    A '#' anywhere it denotes a comment, and all the input until a new line character
    is consumed.
    
    $NAME=VALUE
      This adds `NAME' to the environment. If `NAME' already exists, the previous
      value it is overriden.
    
      The fist character in the name should be in [a-zA-Z] range. The rest could also
      be in the 0-9 range or '_'.
    
      `NAME' should be followed by '='.  

      The value of `VALUE' starts imediatelly after the '=' and ends up to the first
      encountered space. It should be composed with characters in the [a-zA-z0-9]
      range or with any of the '_', '/', ':', '~' characters.  
      It can also be surrounded by a pair of double quotes. In this case it can
      contain any character included embedded spaces.  
      The `VALUE' can be also subject for word expansion, like:  

        $name=$(which ls)  
        $name=$(ls / | grep usr)  
        $name=${var}/${another_var}  

  Selection Menu:
    This is a simple UI that displays one item per line and it is performed when  
    there are more than one item in the completion list.  
  
    keys:  
      ' '   : returns the selected item  
      '\r'  : likewise but it executes immediately the command line, included the selection  
      '\t'  : likewise but it continues with the autocompletion  
      DOWN  : next item (if it is the last item, this is the first item)  
        UP  : prev item (if it is the first item, this is the last item)  
      HOME  : focused to the first item  
      END   : focused to the last item  
      CTRL-f   :  
      PAGE_DOWN: scrolls one page down  
      CTRL-b   :  
      PAGE_UP  : scrolls one page up  
    
Quirks:  
  A filename with embedded whitespace should be enclosed into double quotes.  
  
  There is no job managment.  

  There is no language.  
  
  There are should be many unhandled cases.  
</pre>
