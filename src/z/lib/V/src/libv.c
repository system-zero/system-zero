/* this was pulled from: https://github.com/agathoklisx/libv
 * the [de]atach algorithm is from the dtach utility: https://github.com/crigler/dtach
 * dtach - A simple program that emulates the detach feature of screen.
 * Copyright (C) 2004-2016 Ned T. Crigler (see COPYING within this directory)
 */

#define LIBRARY "V"

#define  REQUIRE_STDIO
#define  REQUIRE_UNISTD
#define  REQUIRE_FCNTL
#define  REQUIRE_SIGNAL
#define  REQUIRE_TIME
#define  REQUIRE_PTY
#define  REQUIRE_TERMIOS
//#define  REQUIRE_IOCTL
#define  REQUIRE_SYS_TYPES
#define  REQUIRE_SYS_STAT
#define  REQUIRE_SYS_SELECT
#define  REQUIRE_SYS_SOCKET
#define  REQUIRE_SYS_UN

#define  REQUIRE_STRING_TYPE   DECLARE
#define  REQUIRE_CSTRING_TYPE  DECLARE
#define  REQUIRE_VSTRING_TYPE  DECLARE
#define  REQUIRE_TERM_TYPE     DECLARE
#define  REQUIRE_PATH_TYPE     DECLARE
#define  REQUIRE_FILE_TYPE     DECLARE
#define  REQUIRE_DIR_TYPE      DECLARE
#define  REQUIRE_SYS_TYPE      DECLARE
#define  REQUIRE_ARGPARSE_TYPE DECLARE
#define  REQUIRE_VWM_TYPE      DONOT_DECLARE
#define  REQUIRE_ERROR_TYPE    DECLARE
#define  REQUIRE_READLINE_TYPE DECLARE
#define  REQUIRE_VMAP_TYPE     DONOT_DECLARE
#define  REQUIRE_LA_TYPE       DECLARE
#define  REQUIRE_VIDEO_TYPE    DONOT_DECLARE
#define  REQUIRE_V_TYPE        DONOT_DECLARE

#include <z/cenv.h>

static la_T *__LAPTR__ = NULL;

#undef Vwm
#undef Vwin
#undef Vframe

#define Vwm vwm->self
#define Vwin vwm->win
#define Vframe vwm->frame

#define self(__f__, ...) this->self.__f__ (this, ##__VA_ARGS__)
#define $my(__p__) this->prop->__p__

#define V_NUM_OBJECTS (NUM_OBJECTS + 2)
#define LA_OBJECT V_NUM_OBJECTS - 1

#define SOCKET_MAX_DATA_SIZE (sizeof (struct winsize))

enum
{
  REDRAW_UNSPEC  = 0,
  REDRAW_NONE  = 1,
  REDRAW_CTRL_L  = 2,
  REDRAW_WINCH  = 3,
};

struct packet {
  unsigned char type;
  unsigned char len;
  union {
    unsigned char buf[SOCKET_MAX_DATA_SIZE];
    struct winsize ws;
  } u;
};

struct pty {
  int fd;
  pid_t pid;
  struct termios term;
  struct winsize ws;
};

struct client {
  struct client *next;
  struct client **pprev;
  int fd;
  int attached;
};

struct v_prop {
  char
    *current_dir,
    *image_name,
    *image_file;

  int
    input_fd,
    save_image,
    always_connect;

  term_t *term;

  char mode_key;
  char *sockname;

  int
    detach_char,
    no_suspend,
    redraw_method;

  int
    waitattach,
    dont_have_tty;

  struct client *clients;
  struct pty pty;

  PtyMain_cb pty_main_cb;
  PtyOnExecChild_cb exec_child_cb;

  int num_at_exit_cbs;
  PtyAtExit_cb *at_exit_cbs;

  string_t
    *tmp_dir,
    *data_dir,
    *as_sockname;

  v_opts *opts;

  struct termios orig_mode;

  readline_com_t **commands;
  int num_commands;

  la_t *la_instance;

  void *user_data[V_NUM_OBJECTS];
};

#define EOS "\033[999H"

static int win_changed;

static const char *const arg_parse_usage[] = {
  "v -s,--sockname= [options] [command] [command arguments]",
  NULL,
};

static const char usage[] =
  "v -s,--sockname= [options] [command] [command arguments]\n"
  "\n"
  "Options:\n"
  "    -s, --sockname=     set the socket name [required if --as= missing]\n"
  "        --as=           create the socket name in an inner environment [required if -s is missing]\n"
  "    -a, --attach        attach to the specified socket\n"
  "    -f, --force         connect to socket, even when socket exists\n"
  "        --send          send data to the specified socket from standard input\n"
  "        --exit          create the socket, fork and then exit\n"
  "        --remove-socket remove socket if exists and can not be connected\n"
  "        --loadfile=     load file for evaluation\n"
  "\n";

static int fd_set_nonblocking (int fd) {
  int flags = fcntl (fd, F_GETFL);
  if (flags < 0 or fcntl (fd, F_SETFL, flags | O_NONBLOCK) < 0)
    return NOTOK;

  return OK;
}

static int v_sock_create (v_t *this, char *sockname) {
  (void) this;
  struct sockaddr_un sockun;

  if (bytelen (sockname) > sizeof (sockun.sun_path) - 1) {
    errno = ENAMETOOLONG;
    return NOTOK;
  }

  int s = socket (PF_UNIX, SOCK_STREAM, 0);
  if (s is -1) {
    fprintf (stderr, "%s: socket(): %s\n", __func__, Error.errno_string (errno));
    return NOTOK;
  }

  sockun.sun_family = AF_UNIX;
  strcpy (sockun.sun_path, sockname);

  if (bind (s, (struct sockaddr*)&sockun, sizeof (sockun)) is -1) {
    fprintf (stderr, "%s: bind(): %s\n", __func__, Error.errno_string (errno));
    close (s);
    return NOTOK;
  }

  if (listen (s, 128) is -1) {
    fprintf (stderr, "%s: listen(): %s\n", __func__, Error.errno_string (errno));
    close (s);
    return NOTOK;
  }

  if (fd_set_nonblocking (s) < 0) {
    fprintf (stderr, "%s: fd_set_non_blocking(): %s\n", __func__, Error.errno_string (errno));
    close (s);
    return -1;
  }

  if (chmod (sockname, 0600) is -1) {
    fprintf (stderr, "%s: chmod(): %s\n", __func__, Error.errno_string (errno));
    close (s);
    return NOTOK;
  }

  return s;
}

static int v_sock_connect (v_t *this, char *sockname) {
  (void) this;

  struct sockaddr_un sockun;
  int s = socket (PF_UNIX, SOCK_STREAM, 0);
  if (s is -1) {
    fprintf (stderr, "socket(): %s\n", Error.errno_string (errno));
    return NOTOK;
  }

  sockun.sun_family = AF_UNIX;
  strcpy (sockun.sun_path, sockname);

  if (connect (s, (struct sockaddr*)&sockun, sizeof (sockun)) is -1) {
    close (s);
    /* ECONNREFUSED is also returned for regular files, so make
    ** sure we are trying to connect to a socket. */
    fprintf (stderr, "connect(): %s\n", Error.errno_string (errno));
    if (errno is ECONNREFUSED) {
      struct stat st;

      if (stat (sockname, &st) is -1) {
        fprintf (stderr, "stat(): %s\n", Error.errno_string (errno));
        return NOTOK;
      }

      if (0 is S_ISSOCK(st.st_mode) or S_ISREG(st.st_mode))
        errno = ENOTSOCK;
    }

    return NOTOK;
  }

  return s;
}

static int v_sock_send_data (v_t *this, int s, char *data, size_t len, int type) {
  (void) this;
  if (NULL is data) return NOTOK;

  if (len > SOCKET_MAX_DATA_SIZE) return NOTOK;

  struct packet pkt;
  pkt.type = type;
  pkt.len = len;
  for (int i = 0; i < pkt.len; i++) pkt.u.buf[i] = data[i];

  if (-1 is write (s, &pkt, sizeof(struct packet)))
    return NOTOK;

  return OK;
}

static void update_socket_modes (char *sockname, int exec) {
  struct stat st;
  mode_t newmode;

  if (stat (sockname, &st) < 0)
    return;

  if (exec)
    newmode = st.st_mode | S_IXUSR;
  else
    newmode = st.st_mode & ~S_IXUSR;

  if (st.st_mode != newmode)
    chmod (sockname, newmode);
}

static void pty_die (int sig) {
  if (sig is SIGCHLD)
    return;

  exit (1);
}

static void tty_die (int sig) {
  if (sig is SIGHUP or sig is SIGINT)
    fprintf (stdout, EOS "\r\n[detached]\r\n");
  else
    fprintf (stdout, EOS "\r\n[got signal %d - dying]\r\n", sig);

  exit (1);
}

static void tty_sigwinch_handler (int);
static void tty_sigwinch_handler (int sig) {
  (void) sig;
  signal (SIGWINCH, tty_sigwinch_handler);
  win_changed = 1;
}

static char *ustring_character (utf8 c, char *buf, int *len) {
  *len = 1;
  if (c < 0x80) {
    buf[0] = (char) c;
  } else if (c < 0x800) {
    buf[0] = (c >> 6) | 0xC0;
    buf[1] = (c & 0x3F) | 0x80;
    (*len)++;
  } else if (c < 0x10000) {
    buf[0] = (c >> 12) | 0xE0;
    buf[1] = ((c >> 6) & 0x3F) | 0x80;
    buf[2] = (c & 0x3F) | 0x80;
    (*len) += 2;
  } else if (c < 0x110000) {
    buf[0] = (c >> 18) | 0xF0;
    buf[1] = ((c >> 12) & 0x3F) | 0x80;
    buf[2] = ((c >> 6) & 0x3F) | 0x80;
    buf[3] = (c & 0x3F) | 0x80;
    (*len) += 3;
  } else
    return 0;

  buf[*len] = '\0';
  return buf;
}

static int tty_process_kbd (v_t *this, int s, struct packet *pkt) {
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  /* Suspend? */
  if (0 is $my(no_suspend) and (pkt->u.buf[0] is $my(term)->raw_mode.c_cc[VSUSP])) {
    pkt->type = MSG_DETACH;
    write (s, pkt, sizeof (struct packet));

    tcsetattr (0, TCSADRAIN, &$my(term)->orig_mode);
    fprintf (stdout, EOS "\r\n");
    kill (getpid(), SIGTSTP);
    tcsetattr (0, TCSADRAIN, &$my(term)->raw_mode);

    /* Tell the master that we are returning. */
    pkt->type = MSG_ATTACH;
    write (s, pkt, sizeof (struct packet));

    /* We would like a redraw, too. */
    pkt->type = MSG_REDRAW;
    pkt->len = $my(redraw_method);
    ioctl (0, TIOCGWINSZ, &pkt->u.ws);
    write (s, pkt, sizeof (struct packet));
    return 0;
  } else if (pkt->u.buf[0] is $my(mode_key)) {
    utf8 c = Vwm.getkey ($my(user_data)[VWM_OBJECT], 0);

    if (c is $my(detach_char)) {
      fprintf (stdout, EOS "\r\n[detached]\r\n");
      return 1;
    }

    pkt->len = 1;
    write (s, pkt, sizeof (struct packet));

    int len;
    char buf[8];
    ustring_character (c, buf, &len);
    pkt->len = len;
    for (int i = 0; i < len; i++) pkt->u.buf[i] = buf[i];
  }
  /* Just in case something pukes out. */
  else if (pkt->u.buf[0] is '\f')
    win_changed = 1;

  write (s, pkt, sizeof (struct packet));
  return 0;
}

static int v_tty_main (v_t *this) {
  int s = self(sock.connect, $my(sockname));

  if (s is NOTOK) {
    fprintf (stderr, "%s: %s\n", $my(sockname), Error.errno_string (errno));
    return 1;
  }

  signal (SIGPIPE, SIG_IGN);
  signal (SIGXFSZ, SIG_IGN);
  signal (SIGHUP,   tty_die);
  signal (SIGTERM,  tty_die);
  signal (SIGINT,   tty_die);
  signal (SIGQUIT,  tty_die);
  signal (SIGWINCH, tty_sigwinch_handler);

  Term.raw_mode ($my(term));
  Term.screen.save ($my(term));
  Term.screen.clear ($my(term));

  struct packet pkt;

  memset (&pkt, 0, sizeof (struct packet));
  pkt.type = MSG_ATTACH;
  write (s, &pkt, sizeof (struct packet));

  pkt.type = MSG_REDRAW;
  pkt.len = $my(redraw_method);
  ioctl (0, TIOCGWINSZ, &pkt.u.ws);
  write (s, &pkt, sizeof (struct packet));

  int retval = 0;

  unsigned char buf[BUFSIZE];
  fd_set readfds;

  for (;;) {
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(s, &readfds);

    int n = select (s + 1, &readfds, NULL, NULL, NULL);

    if (n < 0 and errno isnot EINTR and errno isnot EAGAIN) {
      fprintf (stderr, EOS "\r\n[select failed]\r\n");
      retval = -1;
      break;
    }

    if (n > 0 and FD_ISSET(s, &readfds)) {
      ssize_t len = read (s, buf, sizeof (buf));

      if (len is 0) {
        fprintf (stderr, EOS "\r\n[EOF - terminating]\r\n");
        break;
      } else if (len < 0) {
        fprintf (stderr, EOS "\r\n[read returned an error]\r\n");
        retval = -1;
        break;
      }

      write (STDOUT_FILENO, buf, len);
      n--;
    }

    if (n > 0 and FD_ISSET(STDIN_FILENO, &readfds)) {
      ssize_t len;

      pkt.type = MSG_PUSH;
      memset (pkt.u.buf, 0, sizeof (pkt.u.buf));
      len = read (STDIN_FILENO, pkt.u.buf, sizeof (pkt.u.buf));

      if (len <= 0) {
        retval = -1;
        break;
      }

      pkt.len = len;
      if (1 is (retval = tty_process_kbd (this, s, &pkt)))
        break;

      n--;
    }

    if (win_changed) {
      win_changed = 0;

      pkt.type = MSG_WINCH;
      ioctl (0, TIOCGWINSZ, &pkt.u.ws);
      write (s, &pkt, sizeof (pkt));
    }
  }

  Term.orig_mode ($my(term));
  Term.screen.restore ($my(term));

  if (1 isnot retval)
    unlink ($my(sockname));

  return (retval is -1 ? 1 : 0);
}

static int pty_child (v_t *this, int argc, char **argv) {
  $my(pty).term = $my(term)->orig_mode;
  memset (&$my(pty).ws, 0, sizeof (struct winsize));

  char name[1024];
  $my(pty).pid = forkpty (&$my(pty).fd, name, &$my(pty).term, NULL);

  if ($my(pty).pid < 0)
    return -1;

  if ($my(pty).pid is 0) {
    setsid ();

    int fd = open (name, O_RDWR|O_CLOEXEC|O_NOCTTY);
    close ($my(pty).fd);

    close (0);
    close (1);
    close (2);

    dup (fd);
    dup (fd);
    dup (fd);

    ioctl (0, TIOCSCTTY, 1);

    vwm_t *vwm = $my(user_data)[VWM_OBJECT];
    int rows = Vwm.get.lines (vwm);
    int cols = Vwm.get.columns (vwm);

    struct winsize wsiz;
    wsiz.ws_row = rows;
    wsiz.ws_col = cols;
    wsiz.ws_xpixel = 0;
    wsiz.ws_ypixel = 0;
    ioctl (fd, TIOCSWINSZ, &wsiz);

    close (fd);

    int retval = $my(exec_child_cb) (this, argc, argv);
    __deinit_v__ (&this);

    _exit (retval);
  }

  return 0;
}

static void killpty (struct pty *pty, int sig) {
  pid_t pgrp = -1;

#ifdef TIOCSIGNAL
  if (ioctl (pty->fd, TIOCSIGNAL, sig) >= 0)
    return;
#endif

#ifdef TIOCSIG
  if (ioctl (pty->fd, TIOCSIG, sig) >= 0)
    return;
#endif

#ifdef TIOCGPGRP
  if (ioctl (pty->fd, TIOCGPGRP, &pgrp) >= 0 and pgrp isnot -1 and
    kill (-pgrp, sig) >= 0)
    return;
#endif

  /* Fallback using the child's pid. */
  kill (-pty->pid, sig);
}

static void pty_activity (v_t *this, int s) {
  unsigned char buf[BUFSIZE];
  ssize_t len;
  struct client *p;
  fd_set readfds, writefds;
  int max_fd, nclients;

  len = read ($my(pty).fd, buf, sizeof (buf));
  if (len <= 0)
    exit (1);

  if (tcgetattr ($my(pty).fd, &$my(pty).term) < 0)
    exit (1);

top:
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_SET(s, &readfds);
  max_fd = s;

  for (p = $my(clients), nclients = 0; p; p = p->next) {
    if (!p->attached)
      continue;

    FD_SET(p->fd, &writefds);
    if (p->fd > max_fd)
      max_fd = p->fd;

    nclients++;
  }

  ifnot (nclients) return;

  if (select (max_fd + 1, &readfds, &writefds, NULL, NULL) < 0)
    return;

  for (p = $my(clients), nclients = 0; p; p = p->next) {
    ssize_t written;

    ifnot (FD_ISSET(p->fd, &writefds))
      continue;

    written = 0;
    while (written < len) {
      ssize_t n = write (p->fd, buf + written, len - written);

      if (n > 0) {
        written += n;
        continue;
      } else if (n < 0 and errno is EINTR)
        continue;
      else if (n < 0 and errno isnot EAGAIN)
        nclients = -1;
      break;
    }

    if (nclients isnot -1 and written is len)
      nclients++;
  }

  /* Try again if nothing happened. */
  if (!FD_ISSET(s, &readfds) and nclients is 0)
    goto top;
}

static void pty_socket_activity (v_t *this, int s) {
  int fd = accept (s, NULL, NULL);
  if (fd < 0)
    return;

  if (fd_set_nonblocking (fd) < 0) {
    close (fd);
    return;
  }

  struct client *p = Alloc (sizeof (struct client));

  p->fd = fd;
  p->attached = 0;
  p->pprev = &$my(clients);
  p->next = *(p->pprev);
  if (p->next)
    p->next->pprev = &p->next;
  *(p->pprev) = p;
}

static void pty_client_activity (v_t *this, struct client *p) {
  struct packet pkt;

  ssize_t len = read (p->fd, &pkt, sizeof (struct packet));
  if (len < 0 and (errno is EAGAIN or errno is EINTR))
    return;

  if (len <= 0) {
    close (p->fd);

    if (p->next)
      p->next->pprev = p->pprev;
    *(p->pprev) = p->next;
    free(p);
    return;
  }

  /* Push out data to the program. */
  if (pkt.type is MSG_PUSH) {
    if (pkt.len <= sizeof (pkt.u.buf))
      write ($my(pty).fd, pkt.u.buf, pkt.len);
  } else if (pkt.type is MSG_ATTACH)
    p->attached = 1;
  else if (pkt.type is MSG_DETACH)
    p->attached = 0;
  else if (pkt.type is MSG_WINCH) {
    $my(pty).ws = pkt.u.ws;
    ioctl ($my(pty).fd, TIOCSWINSZ, &$my(pty).ws);
  } else if (pkt.type == MSG_REDRAW) {
    int method = pkt.len;

    /* If the client didn't specify a particular method, use
    ** whatever we had on startup. */
    if (method is REDRAW_UNSPEC)
      method = $my(redraw_method);
    if (method is REDRAW_NONE)
      return;

    $my(pty).ws = pkt.u.ws;
    ioctl ($my(pty).fd, TIOCSWINSZ, &$my(pty).ws);

    /* Send a ^L character if the terminal is in no-echo and
    ** character-at-a-time mode. */
    if (method is REDRAW_CTRL_L) {
      char c = '\f';

      if ((($my(pty).term.c_lflag & (ECHO|ICANON)) is 0) and
           ($my(pty).term.c_cc[VMIN] is 0))
           //($my(pty).term.c_cc[VMIN] is 1)) {
        write ($my(pty).fd, &c, 1);
    } else if (method is REDRAW_WINCH)
      killpty (&$my(pty), SIGWINCH);
  }
}

static void pty_process (v_t *this, int s, int argc, char **argv, int statusfd) {
  setsid ();

  signal (SIGCHLD, pty_die);

  if (pty_child (this, argc, argv) < 0) {
    if (statusfd isnot -1)
      dup2 (statusfd, 1);

    if (errno is ENOENT)
      fprintf (stderr, "Could not find a pty.\n");
    else
      fprintf (stderr, "init_pty: %s\n", Error.errno_string (errno));

    unlink ($my(sockname));
    exit (1);
  }

  signal (SIGPIPE, SIG_IGN);
  signal (SIGXFSZ, SIG_IGN);
  signal (SIGHUP, SIG_IGN);
  signal (SIGTTIN, SIG_IGN);
  signal (SIGTTOU, SIG_IGN);
  signal (SIGINT, pty_die);
  signal (SIGTERM, pty_die);

  /* Close statusfd, since we don't need it anymore. */
  if (statusfd isnot -1) close (statusfd);

  /* Make sure stdin/stdout/stderr point to /dev/null. We are now a
  ** daemon. */
  int nullfd = open ("/dev/null", O_RDWR);
  dup2 (nullfd, 0);
  dup2 (nullfd, 1);
  dup2 (nullfd, 2);

  if (nullfd > 2)
    close (nullfd);

  struct client *p, *next;
  fd_set readfds;
  int
    max_fd,
    has_attached_client = 0;

  while (1) {
    int new_has_attached_client = 0;

    FD_ZERO(&readfds);
    FD_SET(s, &readfds);
    max_fd = s;

    /* When waitattach is set, wait until the client attaches
     * before trying to read from the pty. */
    if ($my(waitattach)) {
      if ($my(clients) and $my(clients)->attached)
        $my(waitattach) = 0;
    } else {
      FD_SET($my(pty).fd, &readfds);
      if ($my(pty).fd > max_fd)
        max_fd = $my(pty).fd;
    }

    for (p = $my(clients); p; p = p->next) {
      FD_SET(p->fd, &readfds);
      if (p->fd > max_fd)
        max_fd = p->fd;

      if (p->attached)
        new_has_attached_client = 1;
    }

    /* chmod the socket if necessary. */
    if (has_attached_client isnot new_has_attached_client) {
      update_socket_modes ($my(sockname), new_has_attached_client);
      has_attached_client = new_has_attached_client;
    }

    if (select (max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
      if (errno is EINTR or errno isnot EAGAIN)
        continue;

      unlink ($my(sockname));
      exit (1);
    }

    /* New client? */
    if (FD_ISSET(s, &readfds))
      pty_socket_activity (this, s);

    for (p = $my(clients); p; p = next) {
      next = p->next;
      if (FD_ISSET(p->fd, &readfds))
        pty_client_activity (this, p);
    }

    if (FD_ISSET($my(pty).fd, &readfds))
      pty_activity (this, s);
  }
}

static int v_pty_main (v_t *this, int argc, char **argv) {
  int s = self(sock.create, $my(sockname));

  if (s is NOTOK) {
    fprintf (stderr, "%s: %s\n", $my(sockname), Error.errno_string (errno));
    return NOTOK;
  }

  fcntl (s, F_SETFD, FD_CLOEXEC);

  int fd[2] = {-1, -1};

  /* If FD_CLOEXEC works, create a pipe and use it to report any errors
  ** that occur while trying to execute the program. */
  if (pipe (fd) >= 0) {
    if (fcntl (fd[0], F_SETFD, FD_CLOEXEC) < 0 or
        fcntl (fd[1], F_SETFD, FD_CLOEXEC) < 0) {
      close (fd[0]);
      close (fd[1]);
      fd[0] = fd[1] = -1;
    }
  }

  int rows; int cols;
  self(init.term, &rows, &cols);

  int retval;
  if (OK isnot (retval = $my(pty_main_cb) (this, argc, argv))) {
    unlink ($my(sockname));
    close (s);
    return retval;
  }

  pid_t pid = fork ();

  if (pid < 0) {
    fprintf (stderr, "fork: %s\n", Error.errno_string (errno));
    unlink ($my(sockname));
  } else if (pid is 0) {
    /* Child - this becomes the master */
    if (fd[0] != -1)
      close (fd[0]);

    pty_process (this, s, argc, argv, fd[1]);

    return 0;
  }

  close (s);

  return 0;
}

static term_t *v_init_term (v_t *this, int *rows, int *cols) {
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];

  term_t *term =  Vwm.get.term (vwm);

  Term.raw_mode (term);

  Term.init_size (term, rows, cols);

  Vwm.set.size (vwm, *rows, *cols, 1);

  return term;
}

static int v_pty_main_cb (v_t *this, int argc, char **argv) {
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];

  int rows = Vwm.get.lines (vwm);
  int cols = Vwm.get.columns (vwm);
  win_opts w_opts = WinOpts (
      .num_rows = rows,
      .num_cols = cols,
      .num_frames = 1,
      .max_frames = 2);

  vwm_win *win = Vwm.new.win (vwm, NULL, w_opts);
  vwm_frame *frame = Vwin.get.frame_at (win, 0);
  Vframe.set.argv (frame, argc, argv);
  Vframe.create_fd (frame);

  return OK;
}

static VwmReadlineArgs v_readline_cb (VwmReadlineArgs args) {
  v_t *this = args.user_data[V_OBJECT];

  vwm_t *vwm = args.vwm;
  vwm_frame *frame = args.frame;

  $my(commands) = Vwm.get.commands (vwm, &$my(num_commands));

  readline_t *rl = Vwm.readline.edit (vwm, args.win, args.frame, $my(commands), $my(num_commands));
  args.rl = rl;

  if (rl->c isnot '\r') goto theend;

  string_t *com = Readline.get.command (rl);
  args.com = com;

  if (Cstring.eq (com->bytes, "`chdir")) {
    Vstring_t *dir = Readline.get.arg_fnames (rl, 1);

    ifnot (NULL is dir) {
      args.retval = self(set.current_dir, dir->head->data->bytes, 0);
      Vstring.release (dir);
    }

    args.state |= VWM_READLINE_END;
    goto theend;
  } else if (Cstring.eq (com->bytes, "set")) {
    string_t *arg = Readline.get.anytype_arg (rl, "log-file");
    ifnot (NULL is arg) {
      int set_log = atoi (arg->bytes);
      if (set_log)
        Vframe.set.log (frame, NULL, 1);
      else
        Vframe.release_log (frame);
    }

    arg = Readline.get.anytype_arg (rl, "save-image");
    ifnot (NULL is arg)
      self(set.save_image, atoi (arg->bytes));

    arg = Readline.get.anytype_arg (rl, "image-file");
    ifnot (NULL is arg)
      self(set.image_file, arg->bytes);

    arg = Readline.get.anytype_arg (rl, "image-name");
    ifnot (NULL is arg)
      self(set.image_name, arg->bytes);

    arg = Readline.get.anytype_arg (rl, "always-connect");
    ifnot (NULL is arg)
      $my(always_connect) = atoi (arg->bytes);

    args.retval = OK;
    args.state |= VWM_READLINE_END;
    goto theend;
  } else if (Cstring.eq (com->bytes, "@save_image")) {
    char *fname = NULL;
    string_t *fn_arg = Readline.get.anytype_arg (rl, "as");

    ifnot (NULL is fn_arg) fname = fn_arg->bytes;

    args.retval = self(save_image, fname);
    args.state |= VWM_READLINE_END;

    goto theend;
  }

  args.state |= VWM_READLINE_PARSE;

theend:
  return args;
}

static int v_exec_child_cb (v_t *this, int argc, char **argv) {
  (void) argc; (void) argv;
  Term.orig_mode($my(term));
  Term.raw_mode ($my(term));

  vwm_t *vwm = $my(user_data)[VWM_OBJECT];

  int retval = Vwm.main (vwm);

  if ($my(save_image)) self(save_image, NULL);
  return retval;
}

static void v_init_commands (v_t *this) {
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];

  $my(commands) = Vwm.get.commands (vwm, &$my(num_commands));

  Vwm.readline.append_command (vwm, "`chdir", 6, 0);
  Vwm.readline.append_command_arg (vwm, "set", "--image-file=", 13);
  Vwm.readline.append_command_arg (vwm, "set", "--save-image=", 13);
  Vwm.readline.append_command_arg (vwm, "set", "--image-name=", 13);
  Vwm.readline.append_command_arg (vwm, "set", "--always-connect=", 17);

  Vwm.readline.append_command (vwm, "@save_image", 11, 0);
  Vwm.readline.append_command_arg (vwm, "@save_image", "--as=", 5);

  Vwm.set.readline_cb (vwm, v_readline_cb);
}

#define AS_V(__v__) (v_t *) AS_PTR(__v__)
#define AS_W(__w__) (vwm_win *) AS_PTR(__w__)
#define AS_F(__f__) (vwm_frame *) AS_PTR(__f__)

static VALUE la_v_get (la_t *la) {
  VALUE v = PTR(La.get.user_data (la));
  return v;
}

static VALUE la_v_get_vwm (la_t *la, VALUE v_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  VALUE v = PTR($my(user_data)[VWM_OBJECT]);
  return v;
}

static VALUE la_v_get_term (la_t *la, VALUE v_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  VALUE v = PTR(Vwm.get.term (vwm));
  return v;
}

static VALUE la_v_get_rows (la_t *la, VALUE v_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  VALUE v = INT(Vwm.get.lines (vwm));
  return v;
}

static VALUE la_v_get_cols (la_t *la, VALUE v_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  VALUE v = INT(Vwm.get.columns (vwm));
  return v;
}

static VALUE la_v_win_get_frame_at (la_t *la, VALUE v_value, VALUE w_value, VALUE idx_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  vwm_win *win = AS_W(w_value);
  int idx = AS_INT(idx_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  VALUE v = PTR(Vwin.get.frame_at (win, idx));
  return v;
}

static VALUE la_v_win_set_current_at (la_t *la, VALUE v_value, VALUE w_value, VALUE idx_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  vwm_win *win = AS_W(w_value);
  int idx = AS_INT(idx_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  VALUE v = PTR(Vwin.set.current_at (win, idx));
  return v;
}

static VALUE la_v_set_opt_force (la_t *la, VALUE v_value, VALUE i_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  int val = AS_INT(i_value);
  $my(opts)->force = (val isnot 0);
  $my(always_connect) = $my(opts)->force;
  VALUE v = INT(LA_OK);
  return v;
}

static VALUE la_v_set_raw_mode (la_t *la, VALUE v_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  Term.raw_mode (Vwm.get.term (vwm));
  VALUE v = INT(LA_OK);
  return v;
}

static VALUE la_v_set_size (la_t *la, VALUE v_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  int rows, cols;
  Term.init_size (Vwm.get.term (vwm), &rows, &cols);
  Vwm.set.size (vwm, rows, cols, 1);
  VALUE v = INT(LA_OK);
  return v;
}

static VALUE la_v_set_sockname (la_t *la, VALUE v_value, VALUE s_value) {
  (void) la;
  VALUE v = INT(LA_OK);
  v_t *this = AS_V(v_value);
  char *sockname = AS_STRING_BYTES(s_value);

  char tmp[PATH_MAX + 1];
  if (NULL is Path.real (sockname, tmp)) {
    v = INT(LA_NOTOK);
    return v;
  }

  $my(as_sockname) = String.new_with (tmp);
  return v;
}

static VALUE la_v_set_frame_command (la_t *la, VALUE v_value, VALUE fr_value, VALUE com_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  vwm_frame *frame = AS_F(fr_value);
  char *command = AS_STRING_BYTES(com_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  Vframe.set.command (frame, command);
  VALUE v = INT(LA_OK);
  return v;
}

static VALUE la_v_set_frame_visibility (la_t *la, VALUE v_value, VALUE fr_value, VALUE vis_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  vwm_frame *frame = AS_F(fr_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  int visibility = AS_INT(vis_value);
  Vframe.set.visibility (frame, visibility);
  VALUE v = INT(LA_OK);
  return v;
}

static VALUE la_sys_set_current_dir (la_t* la, VALUE dir_value) {
  (void) la;
  char *dir = AS_STRING_BYTES(dir_value);
  VALUE v = INT(chdir (dir));
  return v;
}

static VALUE la_v_set_frame_log (la_t *la, VALUE v_value, VALUE fr_value, VALUE fn_value, VALUE val_value) {
  (void) la;
  VALUE v = INT(LA_OK);
  v_t *this = AS_V(v_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  vwm_frame *frame = AS_F(fr_value);
  int val = AS_INT(val_value);

  if (fn_value.type is NULL_TYPE) {
    Vframe.set.log (frame, NULL, val);
    return v;
  }

  char *fname = AS_STRING_BYTES(fn_value);

  char tmp[PATH_MAX + 1];
  if (NULL is Path.real (fname, tmp)) {
    v = INT(LA_NOTOK);
    return v;
  }

  Vframe.set.log (frame, tmp, val);
  return v;
}

static VALUE la_v_set_image_file (la_t *la, VALUE v_value, VALUE fn_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  char *fn = AS_STRING_BYTES(fn_value);
  self(set.image_file, fn);
  VALUE v = INT(LA_OK);
  return v;
}

static VALUE la_v_set_image_name (la_t *la, VALUE v_value, VALUE name_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  char *name  = AS_STRING_BYTES(name_value);
  self(set.image_name, name);
  VALUE v = INT(LA_OK);
  return v;
}

static VALUE la_v_set_save_image (la_t *la, VALUE v_value, VALUE val_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  int val = AS_INT(val_value);
  self(set.save_image, val);
  VALUE v = INT(LA_OK);
  return v;
}

static VALUE la_v_set_current_at (la_t *la, VALUE v_value, VALUE idx_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  int idx = AS_INT(idx_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  vwm_win *win = Vwm.set.current_at (vwm, idx);
  VALUE v = PTR(win);
  return v;
}

static VALUE la_v_new_win (la_t *la, VALUE v_value, VALUE numfr_value, VALUE maxfr_value) {
  (void) la;
  v_t *this = AS_V(v_value);
  int num_frames = AS_INT(numfr_value);
  int max_frames = AS_INT(maxfr_value);
  vwm_t *vwm = $my(user_data)[VWM_OBJECT];
  int rows = Vwm.get.lines (vwm);
  int cols = Vwm.get.columns (vwm);

  vwm_win *win = Vwm.new.win (vwm, NULL, WinOpts (
    .num_rows = rows,
    .num_cols = cols,
    .num_frames = num_frames,
    .max_frames = max_frames));

  VALUE v = PTR(win);
  return v;
}

static int v_pty_main_void_cb (v_t *this, int argc, char **argv) {
  (void) this; (void) argc; (void) argv;
  return OK;
}

static VALUE la_v_main (la_t *la, VALUE v_value) {
  (void) la;
  VALUE v_notok = INT(LA_NOTOK);
  v_t *this = AS_V(v_value);
  if (NULL is $my(as_sockname))
    return v_notok;

  char *sockname = $my(as_sockname)->bytes;

  if (NULL is sockname) return v_notok;

  v_opts *opts = $my(opts);

  int attach = 0;
  if (File.exists (sockname)) {
    ifnot (File.is_sock (sockname)) {
      fprintf (stderr, "%s: is not a socket\n", sockname);
      return v_notok;
    }

    int fd = self(sock.connect, sockname);
    if (NOTOK is fd) {
      if (opts->force) {
        if (-1 is unlink (sockname)) {
          fprintf (stderr,
              "socket %s exists, and cannot be removed %s\n",
               sockname, strerror (errno));
          return v_notok;
        }
      } else {
        fprintf (stderr,
            "socket %s exists but can not connect/attach\n", sockname);
        return v_notok;
      }
    } else {
      close (fd);
      attach = 1;
    }
  }

  if (NOTOK is self(init.pty, sockname))
    return v_notok;

  //vwm_t *vwm = $my(user_data)[VWM_OBJECT];

  v_init_commands (this);

  ifnot (attach) {
    self(set.exec_child_cb, v_exec_child_cb);
    self(set.pty_main_cb, v_pty_main_void_cb);

    self(pty.main, 0, NULL);
  }

  int retval = self(tty.main);
  if (retval isnot OK)
    return v_notok;

  VALUE v = INT(LA_OK);
  return v;
}

struct v_lafun_t {
  const char *name;
  VALUE val;
  int nargs;
} vlafuns[] = {
  { "v_get",                  PTR(la_v_get), 0},
  { "v_get_vwm",              PTR(la_v_get_vwm), 1},
  { "v_get_term",             PTR(la_v_get_term), 1},
  { "v_get_rows",             PTR(la_v_get_rows), 1},
  { "v_get_cols",             PTR(la_v_get_cols), 1},
  { "v_set_size",             PTR(la_v_set_size), 1},
  { "v_set_sockname",         PTR(la_v_set_sockname), 2},
  { "v_set_raw_mode",         PTR(la_v_set_raw_mode), 1},
  { "v_set_frame_log",        PTR(la_v_set_frame_log), 4},
  { "v_set_save_image",       PTR(la_v_set_save_image), 2},
  { "v_set_image_name",       PTR(la_v_set_image_name), 2},
  { "v_set_image_file",       PTR(la_v_set_image_file), 2},
  { "v_set_current_at",       PTR(la_v_set_current_at), 2},
  { "v_set_opt_force",        PTR(la_v_set_opt_force), 2},
  { "v_set_frame_command",    PTR(la_v_set_frame_command), 3},
  { "v_set_frame_visibility", PTR(la_v_set_frame_visibility), 3},
  { "sys_set_current_dir",    PTR(la_sys_set_current_dir), 1},
  { "v_win_get_frame_at",     PTR(la_v_win_get_frame_at), 3},
  { "v_win_set_current_at",   PTR(la_v_win_set_current_at), 3},
  { "v_new_win",              PTR(la_v_new_win), 3},
  { "v_main",                 PTR(la_v_main), 1},
  { NULL, NULL_VALUE, 0}
};

static int v_la_define_funs_cb (la_t *this) {
  int err;
  for (int i = 0; vlafuns[i].name; i++) {
    if (LA_OK isnot (err = La.def (this, vlafuns[i].name, LA_CFUNC (vlafuns[i].nargs), vlafuns[i].val)))
      return err;
  }

  return LA_OK;
}

static int v_la_loadfile (v_t *this, char *fn, int argc, char **argv) {
  la_T *__la__ = (la_T *) $my(user_data)[LA_OBJECT];

  if (NULL is $my(la_instance))
    $my(la_instance) = La.init_instance (__la__,
        LaOpts(
          .argc = argc,
          .argv = argv,
          .la_dir = Sys.get.env_value ("LA_DIR"),
          .define_funs_cb = v_la_define_funs_cb,
          .user_data = this));

  return La.load_file (__la__, $my(la_instance), fn);
}

static int v_init_pty (v_t *this, char *sockname) {
  struct sockaddr_un sockun;
  if (bytelen (sockname) > sizeof (sockun.sun_path) - 1) {
    fprintf (stderr, "socket name succeeds %zd limit\n", sizeof (sockun.sun_path));
    return NOTOK;
  }

  if (tcgetattr (0, &$my(term)->orig_mode) < 0) {
    memset (&$my(term)->orig_mode, 0, sizeof (struct termios));
    $my(dont_have_tty) = 1;
  }

  $my(sockname) = sockname;
  $my(redraw_method) = REDRAW_WINCH;
  $my(no_suspend) = 0;
  $my(detach_char) = 04;
  $my(waitattach) = 1;

  $my(pty_main_cb) = v_pty_main_cb;
  $my(exec_child_cb) = v_exec_child_cb;

  return OK;
}

static void v_set_object (v_t *this, void *obj, int idx) {
  if (idx >= NUM_OBJECTS or idx < 0) return;
  $my(user_data)[idx] = obj;
}

static void v_set_exec_child_cb (v_t *this, PtyOnExecChild_cb cb) {
  $my(exec_child_cb) = cb;
}

static void v_set_pty_main_cb (v_t *this, PtyMain_cb cb) {
  $my(pty_main_cb) = cb;
}

static void v_set_at_exit_cb (v_t *this, PtyAtExit_cb cb) {
  if (NULL is cb) return;

  $my(num_at_exit_cbs)++;

  ifnot ($my(num_at_exit_cbs) - 1)
    $my(at_exit_cbs) = Alloc (sizeof (PtyAtExit_cb));
  else
    $my(at_exit_cbs) = Realloc ($my(at_exit_cbs), sizeof (PtyAtExit_cb) * $my(num_at_exit_cbs));

  $my(at_exit_cbs)[$my(num_at_exit_cbs) -1] = cb;
}

static int v_set_la_dir (v_t *this, char *dir) {
  if (NULL is dir)
    if (NULL is $my(data_dir) or $my(data_dir)->num_bytes is 0)
      return NOTOK;

  char scripts[] = "scripts";

  size_t len = 1 + bytelen (scripts);

  if (NULL is dir) {
    dir = $my(data_dir)->bytes;
    len += $my(data_dir)->num_bytes;
  } else
    len += bytelen (dir);

  ifnot (Dir.is_directory (dir)) return NOTOK;

  char tmp[PATH_MAX + 1];
  if (NULL is Path.real (dir, tmp)) return NOTOK;

  char script_dir[len + 1];
  Cstring.cp_fmt (script_dir, len + 1, "%s/scripts", dir);

  ifnot (File.exists (script_dir)) {
    if (-1 is Dir.make_parents (script_dir, S_IRWXU, DirOpts()))
      return NOTOK;
  } else {
    ifnot (Dir.is_directory (script_dir))
      return NOTOK;

    ifnot (File.is_rwx (script_dir))
      return NOTOK;
  }

  Sys.set.env_as (dir, "LA_DIR", 1);
  return OK;
}

static int v_set_data_dir (v_t *this, char *dir) {
  if (NULL is $my(data_dir))
    $my(data_dir) = String.new (32);

  String.clear ($my(data_dir));

  if (NULL is dir) {
    String.append_with ($my(data_dir), Sys.get.env_value ("DATADIR"));
    String.append_with ($my(data_dir), "/v");
  } else
    String.append_with ($my(data_dir), dir);

  ifnot (File.exists ($my(data_dir)->bytes)) {
    if (-1 is Dir.make_parents ($my(data_dir)->bytes, S_IRWXU, DirOpts()))
      goto theerror;
  } else {
    ifnot (Dir.is_directory ($my(data_dir)->bytes))
      goto theerror;

    ifnot (File.is_rwx ($my(data_dir)->bytes))
      goto theerror;
  }

  return OK;

theerror:
  self(unset.data_dir);
  return NOTOK;
}

static int v_set_tmp_dir (v_t *this, char *dir) {
  if (NULL is $my(tmp_dir))
    $my(tmp_dir) = String.new (32);

  String.clear ($my(tmp_dir));

  if (NULL is dir) {
    String.append_with ($my(tmp_dir), Sys.get.env_value ("TMPDIR"));
    String.append_with ($my(tmp_dir), "/v/");
    String.append_with ($my(tmp_dir), Sys.get.env_value ("USERNAME"));
  } else
    String.append_with ($my(tmp_dir), dir);

  ifnot (File.exists ($my(tmp_dir)->bytes)) {
    if (-1 is Dir.make_parents ($my(tmp_dir)->bytes, S_IRWXU, DirOpts()))
      goto theerror;
  } else {
    ifnot (Dir.is_directory ($my(tmp_dir)->bytes))
      goto theerror;

    ifnot (File.is_rwx ($my(tmp_dir)->bytes))
      goto theerror;
  }

  return OK;

theerror:
  self(unset.tmp_dir);
  return NOTOK;
}

static int v_set_current_dir (v_t *this, char *dir, int is_malloced) {
  char *cwd = dir;

  if (NULL is cwd) return NOTOK;

  if (-1 is chdir (cwd)) return NOTOK;

  ifnot (NULL is $my(current_dir))
    free ($my(current_dir));

  if (is_malloced)
    $my(current_dir) = cwd;
  else
    $my(current_dir) = Cstring.dup (dir, bytelen (dir));

  return OK;
}

static void v_set_image_file (v_t *this, char *name) {
  if (NULL is name) return;

  ifnot (NULL is $my(image_file))
    free ($my(image_file));

  char *cwd = NULL;

  size_t len = bytelen (name);

  char *extname = Path.extname (name);
  size_t exlen = bytelen (extname);

  int hasnot_ext = (0 is exlen or (exlen and 0 is Cstring.eq (extname, ".lai")));

  if (hasnot_ext) len += 4;

  ifnot (Path.is_absolute (name)) {
    cwd = Dir.current ();
    if (NULL is cwd) return;
    len += bytelen (cwd) + 1;
  }

  $my(image_file) = Alloc (len + 1);

  ifnot (Path.is_absolute (name))
    Cstring.cp_fmt ($my(image_file), len + 1, "%s/%s", cwd, name);
  else
    Cstring.cp ($my(image_file), len + 1, name, len - (hasnot_ext ? 4 : 0));

  if (hasnot_ext)
    Cstring.cat ($my(image_file), len + 1, ".lai");
}

static void v_set_image_name (v_t *this, char *name) {
  if (NULL is name) return;

  ifnot (NULL is $my(image_name))
    free ($my(image_name));

  $my(image_name) = Cstring.dup (name, bytelen (name));
}

static void v_set_save_image (v_t *this, int val) {
  $my(save_image) = val;
}

static void v_unset_data_dir (v_t *this) {
  String.release ($my(data_dir));
  $my(data_dir) = NULL;
}

static void v_unset_tmp_dir (v_t *this) {
  String.release ($my(tmp_dir));
  $my(tmp_dir) = NULL;
}

static string_t *v_init_sockname (v_t *this, char *sockdir, char *as) {
  if (NULL is as) return NULL;

  ifnot (NULL is Cstring.byte.in_str (as, '\\')) {
    fprintf (stderr, "`as' argument includes a slash\n");
    return NULL;
  }

  size_t aslen = bytelen (as);

  string_t *sockname = String.new (aslen + 16);

  if (NULL is sockdir) {
    if (NULL is $my(tmp_dir)) {
      if (NOTOK is self(set.tmp_dir, NULL)) {
        fprintf (stderr, "unable to set temp directory\n");
        goto theerror;
      }
    }

    String.append_with (sockname, $my(tmp_dir)->bytes);

    if (File.exists (sockname->bytes)) {
      ifnot (Dir.is_directory (sockname->bytes)) {
        fprintf (stderr, "%s: not a directory\n", sockname->bytes);
        goto theerror;
      }

      ifnot (File.is_rwx (sockname->bytes)) {
        fprintf (stderr, "%s: insufficient permissions\n", sockname->bytes);
        goto theerror;
      }

    } else {
      if (-1 is Dir.make_parents (sockname->bytes, S_IRWXU, DirOpts())) {
        fprintf (stderr, "%s: can not make directory\n", sockname->bytes);
        fprintf (stderr, "%s\n", Error.errno_string (errno));
        goto theerror;
      }
    }

    String.append_with_fmt (sockname, "/%s", as);
  } else {
    size_t dirlen = bytelen (sockdir);
    while (sockdir[dirlen - 1] is '/') {
      dirlen--;
      sockdir[dirlen] = '\0';
    }

    String.append_with_fmt (sockname, "%s/%s", sockdir, as);
  }

  struct sockaddr_un sockun;
  if (sockname->num_bytes > sizeof (sockun.sun_path) - 1) {
    fprintf (stderr, "socket name `%s' exceeds %zd limit\n", sockname->bytes, sizeof (sockun.sun_path));
    goto theerror;
  }

  return sockname;

theerror:
  String.release (sockname);
  return NULL;
}

static term_t *v_get_term (v_t *this) {
  return $my(term);
}

static char *v_get_sockname (v_t *this) {
  return $my(sockname);
}

static void *v_get_user_data_at (v_t *this, int idx) {
  if (idx >= NUM_OBJECTS or idx < 0) return NULL;
  return $my(user_data)[idx];
}

static size_t v_get_sock_max_data_size (v_t *this) {
  (void) this;
  return SOCKET_MAX_DATA_SIZE;
}

static int v_save_image (v_t *this, char *fname) {
  if (NULL is fname and NULL is $my(image_file) and NULL is $my(image_name))
    return NOTOK;

  string_t *file = NULL;
  if (NULL is fname) {
    if (NULL isnot $my(image_name)) {
      file = String.new_with (Sys.get.env_value ("LA_DIR"));
      String.append_with (file, "/scripts/");
      String.append_with (file, $my(image_name));
      if (file->bytes[file->num_bytes - 1] isnot 'i' and
          file->bytes[file->num_bytes - 2] isnot 'a' and
          file->bytes[file->num_bytes - 3] isnot 'l' and
          file->bytes[file->num_bytes - 4] isnot '.')
        String.append_with (file, ".lai");

      fname = file->bytes;
    } else
      fname = $my(image_file);
  }

  FILE *fp = fopen (fname, "w");
  if (NULL is fp) return NOTOK;

  vwm_t *vwm = $my(user_data)[VWM_OBJECT];

  int cur_win_idx = Vwm.get.current_win_idx (vwm);
  char *cwd = Dir.current ();
  int num_wins = Vwm.get.num_wins (vwm);

  fprintf (fp,
    "# v image script\n\n"
    "var v = v_get ()\n\n"
    "func v_image () {\n"
    "  # variable initialization\n"
    "  var pid = %d\n"
    "  var sockname = \"%s\"\n"
    "  var image_name = \"%s\"\n"
    "  var image_file = \"%s\"\n"
    "  var script_dir = \"%s\"\n"
    "  var cwd = \"%s\"\n"
    "  var save_image = %d\n"
    "  var num_frames = 0\n"
    "  var max_frames = 0\n"
    "  var log = 0\n"
    "  var remove_log = 1\n"
    "  var win = null\n"
    "  var frame = null\n"
    "  var cur_frame_idx = 0\n"
    "  var visibility = 0\n"
    "  var num_visible_frames = 0\n"
    "  var rows = v_get_rows (v)\n"
    "  var cols = v_get_cols (v)\n"
    "  var num_win = %d\n"
    "  var cur_win_idx = %d\n"
    "\n"
    "  var force = %d\n"
    "  if (force) {\n"
    "    v_set_opt_force (v, 1)\n"
    "  }\n"
    "\n"
    "  sys_set_current_dir (cwd)\n"
    "  v_set_sockname (v, sockname)\n"
    "  v_set_raw_mode (v)\n"
    "  v_set_size (v)\n",
    getpid (),
    self(get.sockname),
    (NULL is $my(image_name) ? "" : $my(image_name)),
    (NULL is $my(image_file) ? "" : $my(image_file)),
    Sys.get.env_value ("LA_DIR"),
    cwd,
    $my(save_image),
    num_wins,
    cur_win_idx,
    $my(always_connect)
  );

  free (cwd);

  for (int i = 0; i < num_wins; i++)  {
    vwm_win *win = Vwm.get.win_at (vwm, i);
    int num_frames = Vwin.get.num_frames (win);
    int max_frames = Vwin.get.max_frames (win);
    int num_visible_frames = Vwin.get.num_visible_frames (win);
    int cur_frame_idx = Vwin.get.current_frame_idx (win);
    char *name = Vwin.get.name (win);

    fprintf (fp,
      "\n"
      "  # %s\n"
      "  num_frames = %d\n"
      "  max_frames = %d\n"
      "  num_visible_frames = %d\n"
      "  cur_frame_idx = %d\n"
      "  win = v_new_win (v, num_frames, max_frames)\n",
      name,
      num_frames,
      max_frames,
      num_visible_frames,
      cur_frame_idx);

    for (int f = 0; f < num_frames; f++) {
      vwm_frame *frame = Vwin.get.frame_at (win, f);
      char *logfile =  Vframe.get.logfile (frame);
      char **argv = Vframe.get.argv (frame);
      int argc = Vframe.get.argc (frame);
      int visibility = Vframe.get.visibility (frame);

      fprintf (fp,
          "  frame = v_win_get_frame_at (v, win, %d)\n"
          "  v_set_frame_visibility (v, frame, %d)\n",
          f, visibility);

      fprintf (fp, "  v_set_frame_command (v, frame, \"");

      for (int j = 0; j < argc; j++)
        fprintf (fp, "%s%s", argv[j], (j is argc - 1 ? "" : " "));

      fprintf (fp, "\")\n");

      ifnot (NULL is logfile) {
        fprintf (fp, "  remove_log = %d\n", Vframe.get.remove_log (frame));
        fprintf (fp, "  v_set_frame_log (v, frame, null, remove_log)\n");
      }

      fprintf (fp, "\n");
    }

    fprintf (fp, "  frame = v_win_set_current_at (v, win, %d)\n", cur_frame_idx);
  }

  ifnot (NULL is $my(image_name))
    fprintf (fp, "  v_set_image_name (v, image_name)\n");

  ifnot (NULL is $my(image_file))
    fprintf (fp, "  v_set_image_file (v, image_file)\n");

  fprintf (fp, "  v_set_save_image (v, save_image)\n");

  fprintf (fp, "\n");

  fprintf (fp, "  win = v_set_current_at (v, %d)\n", cur_win_idx);
  fprintf (fp, "}\n\n");
  fprintf (fp, "v_image ()\n");
  fprintf (fp, "v_main (v)\n");
  fclose (fp);

  String.release (file);

  return OK;
}

static int v_send (v_t *this, char *sockname, char *data) {
  int s = self(sock.connect, sockname);
  if (s is NOTOK) return 1;

  size_t max_size = self(get.sock_max_data_size);
  char buf[max_size];
  int retval = 0;

  if (data isnot NULL) {
    size_t len = bytelen (data);
    int num = len;
    char *sp = data;

    while (num > 0) {
      size_t n = 0;
      for (;n < max_size and n < len; n++)
        buf[n] = *sp++;

      if (NOTOK is self(sock.send_data, s, buf, n, MSG_PUSH)) {
        retval = NOTOK;
        goto theend;
      }

      len -= n;
      num -= n;
    }

    goto theend;
  }

  if ($my(input_fd) is 0) { // read from the pipe
    retval = 1;
    goto theend;
  }

  for (;;) {
    ssize_t len = read ($my(input_fd), buf, max_size);
    if (0 is len) goto theend;
    if (len < 0) {
      retval = 1;
      fprintf (stderr, "error while reading from stdin\n");
      goto theend;
    }

    retval = self(sock.send_data, s, buf, len, MSG_PUSH);
  }

theend:
  close (s);
  return retval;
}

static int v_main (v_t *this) {
  v_opts *opts = $my(opts);

  int argc = opts->argc;
  char *sockname = opts->sockname;
  char *loadfile = opts->loadfile;
  char **argv = opts->argv;
  char *as = opts->as;
  char *data = opts->data;
  PtyOnExecChild_cb vexec_child = (opts->at_exec_child is NULL ? v_exec_child_cb : opts->at_exec_child);
  PtyMain_cb vpty_main = (opts->at_pty_main is NULL ? v_pty_main_cb : opts->at_pty_main);

  if ($my(opts)->parse_argv) {
    argparse_option_t options[] = {
      OPT_HELP (),
      OPT_GROUP("Options:"),
      OPT_STRING(0, "as", &as, "create the socket name in an inner environment [required if -s is missing]", NULL, 0, 0),
      OPT_STRING('s', "sockname", &sockname, "set the socket name [required if --as= missing]", NULL, 0, 0),
      OPT_STRING(0, "loadfile", &loadfile, "load file for evaluation", NULL, 0, 0), 
      OPT_BOOLEAN('a', "attach", &opts->attach, "attach to the specified socket", NULL, 0, 0),
      OPT_BOOLEAN(0, "force", &opts->force, "connect to socket, even when socket exists", NULL, 0, 0),
      OPT_BOOLEAN(0, "send", &opts->send_data, "send data to the specified socket", NULL, 0, 0),
      OPT_BOOLEAN(0, "exit", &opts->exit, "create the socket, fork and then exit", NULL, 0, 0),
      OPT_BOOLEAN(0, "remove-socket", &opts->remove_socket, "remove socket if exists and can not be connected", NULL, 0, 0),
      OPT_END()
    };

    argparse_t argparser;
    Argparse.init (&argparser, options, arg_parse_usage, ARGPARSE_DONOT_EXIT_ON_UNKNOWN);
    argc = Argparse.exec (&argparser, argc, (const char **) argv);
  }

  if (argc is -1) return 0;

  ifnot (NULL is loadfile)
    return v_la_loadfile (this, loadfile, argc, argv);

  if (NULL is sockname) {
    if (NULL is as) {
      fprintf (stderr, "required socket name hasn't been specified\n");
      return 1;
    }

    $my(as_sockname) = self(init.sockname, NULL, as);
    if (NULL is $my(as_sockname))
      return 1;

    sockname = $my(as_sockname)->bytes;
  } else
    $my(as_sockname) = String.new_with (sockname);

  if (opts->exit_on_no_command) {
    if (argc is 0 or argv is NULL) {
      if ((0 is opts->attach and 0 is opts->send_data)) {
        fprintf (stderr, "command hasn't been set\n");
        fprintf (stderr, "%s", usage);
        return 1;
      }
    }
  }

  if (File.exists (sockname)) {
    if (0 is opts->attach and 0 is opts->send_data) {
      ifnot (opts->force) {
        ifnot (opts->remove_socket) {
          fprintf (stderr, "%s: exists in the filesystem\n", sockname);
          return 1;
        }
      }
    }

    ifnot (File.is_sock (sockname)) {
      fprintf (stderr, "%s: is not a socket\n", sockname);
      return 1;
    }

    int fd = self(sock.connect, sockname);
    if (0 is opts->attach and 0 is opts->send_data)
      if (opts->remove_socket)
        unlink (sockname);

    if (NOTOK is fd) {
      if (opts->attach or opts->send_data) {
        if (opts->remove_socket)
          unlink (sockname);
        fprintf (stderr, "can not connect/attach to the socket\n");
        return 1;
      }
    } else
      close (fd);
  }

  if (0 is opts->send_data or (opts->send_data and data isnot NULL)) {
    if (0 is isatty (fileno (stdin))) {
      fprintf (stderr, "Not a controlled terminal\n");
      return 1;
    }
  }

  if (opts->send_data) {
    $my(input_fd) = dup (fileno (stdin));
    if ($my(input_fd) is -1) {
      fprintf (stderr, "dup(): %s\n", Error.errno_string (errno));
      return 1;
    }

    if (NULL is freopen ("/dev/tty", "r", stdin)) {
      fprintf (stderr, "freopen(): %s\n", Error.errno_string (errno));
      return 1;
    }

    return self(send, sockname, data);
  }

  if (NOTOK is self(init.pty, sockname))
    return 1;

  //vwm_t *vwm = $my(user_data)[VWM_OBJECT];

  v_init_commands (this);

  ifnot (opts->attach) {
    self(set.exec_child_cb, vexec_child);
    self(set.pty_main_cb, vpty_main);

    self(pty.main, argc, argv);
  }

  if (opts->exit)
    return 0;

  return self(tty.main);
}

public v_t *__init_v__ (v_opts *opts) {
  __INIT__(dir);
  __INIT__(sys);
  __INIT__(path);
  __INIT__(term);
  __INIT__(file);
  __INIT__(error);
  __INIT__(string);
  __INIT__(cstring);
  __INIT__(vstring);
  __INIT__(argparse);
  __INIT__(readline);

  v_t *this = Alloc (sizeof (v_t));
  this->prop = Alloc (sizeof (v_prop));

  this->self = (v_self) {
    .main = v_main,
    .send = v_send,
    .save_image = v_save_image,
    .get = (v_get_self) {
      .term = v_get_term,
      .sockname = v_get_sockname,
      .user_data_at = v_get_user_data_at,
      .sock_max_data_size = v_get_sock_max_data_size
    },
    .set = (v_set_self) {
      .la_dir = v_set_la_dir,
      .object = v_set_object,
      .tmp_dir = v_set_tmp_dir,
      .data_dir = v_set_data_dir,
      .save_image = v_set_save_image,
      .image_file = v_set_image_file,
      .image_name = v_set_image_name,
      .at_exit_cb = v_set_at_exit_cb,
      .current_dir = v_set_current_dir,
      .pty_main_cb = v_set_pty_main_cb,
      .exec_child_cb = v_set_exec_child_cb
    },
    .unset = (v_unset_self) {
      .tmp_dir = v_unset_tmp_dir,
      .data_dir = v_unset_data_dir
    },
    .init = (v_init_self) {
      .pty = v_init_pty,
      .term = v_init_term,
      .sockname = v_init_sockname
    },
    .sock = (v_sock_self) {
      .create = v_sock_create,
      .connect = v_sock_connect,
      .send_data = v_sock_send_data
    },
    .pty = (v_pty_self) {
      .main = v_pty_main
    },
    .tty = (v_tty_self) {
      .main = v_tty_main
    }
  };

  $my(opts) = opts;
  $my(image_file) = NULL;
  $my(image_name) = NULL;
  $my(as_sockname) = NULL;
  $my(current_dir) = NULL;
  $my(data_dir) = NULL;
  $my(save_image) = 0;
  $my(always_connect) = 0;
  $my(input_fd) = STDIN_FILENO;

  $my(num_at_exit_cbs) = 0;
  struct termios orig_mode;
  if (-1 isnot tcgetattr (0, &orig_mode)) { // dont exit yes, as we might be at the end of a pipe
    $my(orig_mode) = orig_mode;
    $my(input_fd) = 0;
  }

  if (-1 isnot tcgetattr ($my(input_fd), &orig_mode))
    $my(orig_mode) = orig_mode;

  Sys.init_environment (SysEnvOpts());

  vwm_t *vwm = __init_vwm__ ();

  __LAPTR__ = __init_la__ ();
  __LA__ = *__LAPTR__;

  $my(user_data)[VWM_OBJECT] = vwm;
  $my(user_data)[LA_OBJECT] = __LAPTR__;
  $my(user_data)[E_OBJECT] = NULL;
  $my(term) = Vwm.get.term (vwm);
  $my(mode_key) = Vwm.get.mode_key (vwm);
  Vwm.set.object (vwm, this, V_OBJECT);

  if (NOTOK is self(set.data_dir, NULL)) goto theerror;
  if (NOTOK is self(set.tmp_dir, NULL)) goto theerror;
  if (NOTOK is self(set.la_dir, NULL)) goto theerror;
  if (NOTOK is self(set.current_dir, Dir.current (), 1)) goto theerror;

  return this;

theerror:
  __deinit_v__ (&this);
  return NULL;
}

public void __deinit_v__ (v_t **thisp) {
  if (NULL is *thisp) return;

  v_t *this = *thisp;

  String.release ($my(as_sockname));
  self(unset.data_dir);
  self(unset.tmp_dir);

  ifnot (NULL is $my(image_file)) free ($my(image_file));
  ifnot (NULL is $my(image_name)) free ($my(image_name));
  ifnot (NULL is $my(current_dir)) free ($my(current_dir));

  vwm_t   *vwm   = $my(user_data)[VWM_OBJECT];
  __deinit_vwm__   (&vwm);

  la_T *__la__ = (la_T *) $my(user_data)[LA_OBJECT];
  __deinit_la__ (&__la__);

  tcsetattr ($my(input_fd), TCSAFLUSH, &$my(orig_mode));

  __deinit_sys__ ();

  free (this->prop);
  free (this);
  *thisp = NULL;
}
