
/* c_iflag bits */
#define IGNBRK   0000001  /* Ignore break condition.  */
#define BRKINT   0000002  /* Signal interrupt on break.  */
#define IGNPAR   0000004  /* Ignore characters with parity errors.  */
#define PARMRK   0000010  /* Mark parity and framing errors.  */
#define INPCK    0000020  /* Enable input parity check.  */
#define ISTRIP   0000040  /* Strip 8th bit off characters.  */
#define INLCR    0000100  /* Map NL to CR on input.  */
#define IGNCR    0000200  /* Ignore CR.  */
#define ICRNL    0000400  /* Map CR to NL on input.  */
#define IUCLC    0001000  /* Map uppercase characters to lowercase on input
                           * (not in POSIX).  */
#define IXON     0002000  /* Enable start/stop output control.  */
#define IXANY    0004000  /* Enable any character to restart output.  */
#define IXOFF    0010000  /* Enable start/stop input control.  */
#define IMAXBEL  0020000  /* Ring bell when input queue is full
                           * (not in POSIX).  */
#define IUTF8    0040000  /* Input is UTF8 (not in POSIX).  */

/* c_lflag bits */
#define ISIG     0000001   /* Enable signals.  */
#define ICANON   0000002   /* Canonical input (erase and kill processing).  */
#define XCASE    0000004
#define ECHO     0000010   /* Enable echo.  */
#define ECHOE    0000020   /* Echo erase character as error-correcting
                            * backspace.  */
#define ECHOK    0000040   /* Echo KILL.  */
#define ECHONL   0000100   /* Echo NL.  */
#define NOFLSH   0000200   /* Disable flush after interrupt or quit.  */
#define TOSTOP   0000400   /* Send SIGTTOU for background output.  */
#define ECHOCTL  0001000   /* If ECHO is also set, terminal special characters
                            * other than TAB, NL, START, and STOP are echoed as
                            * ^X, where X is the character with ASCII code 0x40
                            * greater than the special character
                            * (not in POSIX).  */
#define ECHOPRT  0002000   /* If ICANON and ECHO are also set, characters are
                            * printed as they are being erased
                            * (not in POSIX).  */
#define ECHOKE   0004000   /* If ICANON is also set, KILL is echoed by erasing
                            * each character on the line, as specified by ECHOE
                            * and ECHOPRT (not in POSIX).  */
#define FLUSHO   0010000   /* Output is being flushed.  This flag is toggled by
                            * typing the DISCARD character (not in POSIX).  */
#define PENDIN   0040000   /* All characters in the input queue are reprinted
                            * when the next character is read
                            * (not in POSIX).  */
#define IEXTEN   0100000   /* Enable implementation-defined input
                            * processing.  */

/* c_oflag bits */
#define OPOST    0000001  /* Post-process output.  */
#define OLCUC    0000002  /* Map lowercase characters to uppercase on output.
                           * (not in POSIX).  */
#define ONLCR    0000004  /* Map NL to CR-NL on output.  */
#define OCRNL    0000010  /* Map CR to NL on output.  */
#define ONOCR    0000020  /* No CR output at column 0.  */
#define ONLRET   0000040  /* NL performs CR function.  */
#define OFILL    0000100  /* Use fill characters for delay.  */
#define OFDEL    0000200  /* Fill is DEL.  */
#define NLDLY    0000400  /* Select newline delays:  */
#define   NL0    0000000  /* Newline type 0.  */
#define   NL1    0000400  /* Newline type 1.  */
#define CRDLY    0003000  /* Select carriage-return delays:  */
#define   CR0    0000000  /* Carriage-return delay type 0.  */
#define   CR1    0001000  /* Carriage-return delay type 1.  */
#define   CR2    0002000  /* Carriage-return delay type 2.  */
#define   CR3    0003000  /* Carriage-return delay type 3.  */
#define TABDLY   0014000  /* Select horizontal-tab delays:  */
#define   TAB0   0000000  /* Horizontal-tab delay type 0.  */
#define   TAB1   0004000  /* Horizontal-tab delay type 1.  */
#define   TAB2   0010000  /* Horizontal-tab delay type 2.  */
#define   TAB3   0014000  /* Expand tabs to spaces.  */
#define BSDLY    0020000  /* Select backspace delays:  */
#define   BS0    0000000  /* Backspace-delay type 0.  */
#define   BS1    0020000  /* Backspace-delay type 1.  */
#define FFDLY    0100000  /* Select form-feed delays:  */
#define   FF0    0000000  /* Form-feed delay type 0.  */
#define   FF1    0100000  /* Form-feed delay type 1.  */

#define VTDLY    0040000  /* Select vertical-tab delays:  */
#define   VT0    0000000  /* Vertical-tab delay type 0.  */
#define   VT1    0040000  /* Vertical-tab delay type 1.  */

#define XTABS    0014000

/* c_cc characters */
#define VINTR    0
#define VQUIT    1
#define VERASE   2
#define VKILL    3
#define VEOF     4
#define VTIME    5
#define VMIN     6
#define VSWTC    7
#define VSTART   8
#define VSTOP    9
#define VSUSP    10
#define VEOL     11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15
#define VEOL2    16

/* c_cflag bits.  */
#define CSIZE    0000060
#define   CS5    0000000
#define   CS6    0000020
#define   CS7    0000040
#define   CS8    0000060
#define CSTOPB   0000100
#define CREAD    0000200
#define PARENB   0000400
#define PARODD   0001000
#define HUPCL    0002000
#define CLOCAL   0004000

struct termios {
  unsigned c_iflag;
  unsigned c_oflag;
  unsigned c_cflag;
  unsigned c_lflag;
  unsigned char c_line;
  unsigned char c_cc[19];
};

struct winsize {
  unsigned short ws_row;
  unsigned short ws_col;
  unsigned short ws_xpixel;
  unsigned short ws_ypixel;
};
