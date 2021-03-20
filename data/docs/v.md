<pre>
  The V environment consists of two applications. One that does window managment
  and one that  extends it with [de]attach capabilities.

  Window Managment Application (invoked as V.wm and requires a `command` argument:

  Key bindings:

  MODKEY-q           : quit the application
  MODKEY-K           : kill the current procedure in the current frame
  MODKEY-!           : open the default shell (by default zs-static)
  MODKEY-c           : open the default application (by default zs-static)
  MODKEY-[up|down|w] : switch to the upper|lower frame respectively
  MODKEY-[j|k]       : likewise
  MODKEY-[left|right]: switch to the prev|next window respectively
  MODKEY-[h|l]       : likewise
  MODKEY-`           : switch to the previously focused window
  MODKEY-[param]+    : increase the size of the current frame (default count 1)
  MODKEY-[param]-    : decrease the size of the current frame (default count 1)
  MODKEY-[param]=    : set the lines (param) of the current frame
  MODKEY-[param]n    : create and switch to a new window with `count' frames (default 1)
  MODKEY-E|PageUp    : edit the log file (if it is has been set)
  MODKEY-s           : split the window and add a new frame
  MODKEY-S[!ec]      : likewise, but also fork with a shell or an editor or the default application respectively (without a param is like MODE_KEY-s)
  MODKEY-d           : delete current frame
  MODKEY-CTRL(l)     : redraw current window
  MODKEY-MODE_KEY    : return the MODE_KEY to the application
  MODKEY-ESCAPE_KEY  : return with no action
  MODKEY-:           : command line mode (a Tab triggers auto completion, for commands,
                         arguments and filename[s])

  Notes:
    The MODKEY by default is CTRL-\.

    Param[eter] assumed digit[s] and should be typed before any command that
    gets a param argument.

  Command Line Mode:

    :quit               quit application

    :win_draw           redraw current window

    :frame_clear        clear frame
      --clear-log       also clear the log file, if it is enabled
      --clear-video-mem also clear the video associated memory

    :frame_delete       delete current frame (kills the application also)

    :split_and_fork     split current window and spawn an application (by default zs-static)
      --command={...}   spawn the specified command (`command` can take arguments
                          and should be closed with '}')

    :set                set setting[s]
      --log-file=[0|1]  if not zero, enable logging (if logging is enabled
                          MODKEY-[E|PageUp] can edit the output (default editor is E-static),
                          modifications will be honored)

    :win_new            create a new window (those can be unlimited)
      --num-frames=int  and split it into `num frames` (can not be > than MAX_FRAMES)
      --command={       likewise with above (can be specified as many as `num-frames`)
      --focus=[0|1]     if 0 donot switch to new window (default yes)
      --draw=[0|1]      if 0 donot draw the new window (default yes)


  The V application extends the vwm utility.

  It is invoked as V (V --help):
    Usage: v -s,--sockname= [options] [command] [command arguments]

      -h, --help            show this help message and exit

    Options:
      --as=<str>            create the socket name in an inner environment [required if -s is missing]
      -s, --sockname=<str>  set the socket name [required if --as= missing]
      --loadfile=<str>      load file for evaluation
      -a, --attach          attach to the specified socket
      --force               connect to socket, even when socket exists
      --send                send data to the specified socket
      --exit                create the socket, fork and then exit
      --remove-socket       remove socket if exists and can not be connected

  Key bindings:

  MODKEY-CTRL-D             detach application

  Command Line Mode:

    :set                     set setting[s]
      --log-file=[0|1]       if not zero, enable logging (if logging is enabled
                               MODKEY-[E|PageUp] can edit the output (default editor is E-static),
                               modifications will be honored)
      --save-imgage=[0|1]    save an image of the current layout automatically at exit
      --image-file=`file'    use `file' when saving
      --image-name=`as'      save it under a standard directory as `as' (preferable)
      --always-connect=[1|0] always try to connect to a socket (e.g., if exists but
                               there is noone to listen)

    :`chdir directory     change current directory
</pre>
