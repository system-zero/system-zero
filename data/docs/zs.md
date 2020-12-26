This is a quite minimal shell implementation and at very early stage of development.    
  
<pre>
Semantics:  
  Shell functionality:  
  
  - short-Circuit Operators “&&” and “||” are AND-OR operators.  
    “&&” executes the first command, and then executes the second  
    command if and only if the exit status of the first command is zero.   
    “||” is similar, but executes the  second command if and only if   
    the exit status of the first command is nonzero. Example:  
  
      test -d /tmp/dir || mkdir -v /tmp/dir && cp -v somefile /tmp/dir  
  
    Note that quite possible there are combinations that maybe violate  
    expectations. The semantics are not clear yet.  
  
  - pipes, e.g.,  
  
      ls | grep pat  
  
 - supported redirection (operators):  
  
   - redirect stdout to a filename  
  
     command >/filename  
  
   - redirect stdout to an existing filename (overwrite)  
  
     command >| /filename  
  
   - redirect stderr to a filename  
  
     command 2>/filename  
  
   - redirect stderr and stdout in a filename  
  
     command >& /filename  
  
   - likewise but overwrite existing filename  
  
     command >&| /filename  
  
   - redirect stderr to stdout in a pipeline  
  
     command 2>&1 | next_command  
  
  Readline functionality:   
  - History set/load/save  
  
  - Poor Command Completion  
  
  - Keys:  
     BACKSPACE: remove char at left of cursor  
        DELETE: remove char at right of cursor  
        CTRL_T: swaps current character with previous  
        CTRL_B:  
          LEFT: move left  
        CTRL_F:  
         RIGHT: move right  
            UP:  
        CTRL_P: previous item in history  
          DOWN:  
        CTRL_N: next item in history  
          HOME: move cursor to the start of line  
        CTRL_A: move cursor to the start of line  
           END:  
        CTRL_E:  
                move cursor at the end of line  
        CTRL_U: delete the whole line  
        CTRL_K: delete from current to end of line  
        CTRL_L: clear screen  
        CTRL_W: delete previous word  
        CTRL_D: remove char at right of cursor, or if the line is empty, act as end-of-file  
        CTRL_C: interrupts a command if it is running, otherwise it adds a line  
</pre>
