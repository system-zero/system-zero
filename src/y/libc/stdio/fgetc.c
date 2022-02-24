// requires: unistd/read.c

static int ic (FILE *fp) {
	if (fp->back != EOF) {
		int i = fp->back;
		fp->back = EOF;
		return i;
	}
	while (fp->fd >= 0 && fp->icur == fp->ilen) {
		int nr = read (fp->fd, fp->ibuf, fp->isize);
		if (nr <= 0)
			break;
		fp->ilen = nr;
		fp->icur = 0;
	}
	return fp->icur < fp->ilen ? (unsigned char) fp->ibuf[fp->icur++] : EOF;
}

int fgetc (FILE *fp) {
  return  ic (fp);
}
