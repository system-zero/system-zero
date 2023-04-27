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
