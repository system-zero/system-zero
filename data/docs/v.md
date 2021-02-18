<pre>
  Vwm Key bindings:

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

  Notes:
    The MODKEY by default is CTRL-\.

    Param[eter] assumed digit[s] and should be typed before any command that
    gets an param argument.
</pre>
