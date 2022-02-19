struct termios {
  unsigned c_iflag;
  unsigned c_oflag;
  unsigned c_cflag;
  unsigned c_lflag;
  unsigned char c_line;
  unsigned char c_cc[19];
};
