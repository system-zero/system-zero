
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>

#include <zc.h>
#include <libcstring.h>
#include <libproc.h>
#include <libsh.h>

static  cstring_T CstringT;
#define Cstring   CstringT.self

static pid_t CUR_PID = -1;

#define COMMAND_TYPE 0
#define PIPE_TYPE    1
#define CONJ_TYPE    2
#define DISJ_TYPE    3

typedef struct sh_prop {
  int saved_stdin;

  proc_t *head;
  proc_t *tail;
  proc_t *current;
  int num_items;
  int cur_idx;
} sh_prop;

struct sh_t {
  sh_prop *prop;
};

typedef struct shproc_t {
  int
    type,
    fds[2],
    should_exit,
    skip_next_proc;

  sigset_t mask;
} shproc_t;

static  proc_T ProcT;
#define Proc   ProcT.self

#define $myprop    this->prop
#define $my(__v__) $myprop->__v__

static int sh_append_proc (sh_t *this, proc_t *proc) {
  if ($my(head) is NULL) {
    $my(head) = proc;
    $my(tail) = proc;
    $my(current) = $my(head);
    $my(cur_idx) = 0;
  } else {
    Proc.set.next ($my(tail), proc);
    Proc.set.prev (proc, $my(tail));
    Proc.set.next (proc,  NULL);
    $my(tail) = proc;
  }

  return ++$my(num_items);
}

static void sh_release_list (sh_t *this) {
  proc_t *p = $my(head);
  while (p) {
    proc_t *t = Proc.get.next (p);
    shproc_t *sh = Proc.get.userdata (p);
    free (sh);
    Proc.release (p);
    p = t;
  }

  $my(head) = $my(tail) = $my(current) = NULL;
  $my(num_items) = 0;
  $my(cur_idx) = -1;
}

static void sh_release (sh_t *this) {
  if (this is NULL) return;

  sh_release_list (this);

  free ($myprop);
  free (this);
  this = NULL;
}

static int sh_pre_fork_default_cb (proc_t *proc) {
  shproc_t *sh = (shproc_t *) Proc.get.userdata (proc);

  sigemptyset (&sh->mask);
  sigaddset (&sh->mask, SIGCHLD);
  sigprocmask (SIG_BLOCK, &sh->mask, NULL);

  return OK;
}

static int sh_pre_fork_pipeline_cb (proc_t *this) {
  shproc_t *sh = (shproc_t *) Proc.get.userdata (this);
  if (-1 is pipe (&sh->fds[0]))
    return NOTOK;

  return sh_pre_fork_default_cb (this);
}

static void sh_sigint_handler (int sig) {
  (void) sig;

  if (CUR_PID is -1) return;

  kill (-CUR_PID, SIGINT);
}

static int sh_at_fork_default_cb (proc_t *proc) {
  shproc_t *sh = (shproc_t *) Proc.get.userdata (proc);

  sigprocmask (SIG_UNBLOCK, &sh->mask, NULL);

  CUR_PID = getpid ();
  return OK;
}

static int sh_at_fork_pipeline_cb (proc_t *proc) {
  shproc_t *sh = (shproc_t *) Proc.get.userdata (proc);

  if (-1 is close (sh->fds[0]))
    return NOTOK;

  if (-1 is close (STDOUT_FILENO))
    return NOTOK;

  if (-1 is dup (sh->fds[1]))
    return NOTOK;

  if (-1 is close (sh->fds[1]))
    return NOTOK;

  return sh_at_fork_default_cb (proc);
}

static int sh_interpret (proc_t *this) {
  if (NULL is this) return 1;

  shproc_t *sh = (shproc_t *) Proc.get.userdata (this);

  int retval = 1;
  sh->should_exit = 0;
  sh->skip_next_proc = 0;

  switch (sh->type) {
    case COMMAND_TYPE:
      retval = Proc.exec (this, NULL);
      break;

    case CONJ_TYPE:
      retval = Proc.exec (this, NULL);
      sh->skip_next_proc = retval isnot 0;
      break;

    case DISJ_TYPE:
      retval = Proc.exec (this, NULL);
      sh->skip_next_proc = retval is 0;
      break;

    case PIPE_TYPE:
      retval = Proc.open (this);
      if (NOTOK is retval) {
        sh->should_exit = 1;
        break;
      }

      if (-1 is close (sh->fds[1]) or
          -1 is close (STDIN_FILENO) or
          -1 is dup (sh->fds[0]) or
          -1 is close (sh->fds[0])) {
        retval = -1;
        sh->should_exit = 1;
        break;
      }

      retval = Proc.wait (this);

      if (retval isnot 0)
        sh->should_exit = 1;

      break;

    default:
      break;
   }

   return retval;
}

static sh_t *sh_new (void) {
  sh_t *this = Alloc (sizeof (sh_t));
  $myprop = Alloc (sizeof (sh_prop));
  $my(head) = $my(tail) = $my(current) = NULL;
  $my(cur_idx) = -1; $my(num_items) = 0;
  return this;
}

static int sh_parse (sh_t *this, char *buf) {
  if (NULL is buf) return NOTOK;

  size_t len = bytelen (buf);
  ifnot (len) return NOTOK;

  char cbuf[len + 1];
  Cstring.cp (cbuf, len + 1, buf, len);

  proc_t *p;
  shproc_t *sh;

  char *sp = cbuf;
  buf = sp;
  int type = COMMAND_TYPE;

  while (*sp) {
    if (*sp is '|') {
      ifnot (*(sp + 1)) goto theerror;

      if (*(sp + 1) is '|') {
        ifnot (*(sp + 2)) goto theerror;

        type = DISJ_TYPE;
        *sp = '\0';
        sp += 2;
        goto add_proc;
      }

      type = PIPE_TYPE;
      *sp = '\0';
      sp++;
      goto add_proc;
    }

    if (*sp is '&') {
      ifnot (*(sp + 1)) goto theerror;

      if (*(sp + 1) isnot '&') goto theerror;

      ifnot (*(sp + 2)) goto theerror;

      type = CONJ_TYPE;
      *sp = '\0';
      sp += 2;

      goto add_proc;
    }

    goto next;

    add_proc:
      p = Proc.new ();
      sh = Alloc (sizeof (shproc_t));
      sh->type = type;

      Proc.parse (p, buf);

      if (type is PIPE_TYPE) {
        Proc.set.at_fork_cb (p, sh_at_fork_pipeline_cb);
        Proc.set.pre_fork_cb (p, sh_pre_fork_pipeline_cb);
      } else {
        Proc.set.pre_fork_cb (p, sh_pre_fork_default_cb);
        Proc.set.at_fork_cb (p, sh_at_fork_default_cb);
      }

      Proc.set.userdata (p, sh);
      sh_append_proc (this, p);

      buf = sp;
      type = COMMAND_TYPE;
      ifnot (*sp) goto theend;

    next: sp++;
  }

  if (*buf) goto add_proc;

theend:
  return OK;

theerror:
  sh_release_list (this);
  return NOTOK;
}

static int sh_exec (sh_t *this, char *buf) {
  if (NULL is this) return NOTOK;

  if (NOTOK is sh_parse (this, buf))
    return NOTOK;

  int retval = 0;
  proc_t *p = $my(head);

  $my(saved_stdin) = dup (STDIN_FILENO);

  CUR_PID = -1;
  signal (SIGINT, sh_sigint_handler);

  while (p) {
    retval = sh_interpret (p);

    shproc_t *sh = (shproc_t *) Proc.get.userdata (p);

    if (sh->should_exit or retval is NOTOK)
      break;

    if (sh->skip_next_proc) {
      p = Proc.get.next (p);
    }

    if (p isnot NULL)
      p = Proc.get.next (p);
  }

  dup2 ($my(saved_stdin), STDIN_FILENO);

  return (retval < OK ? 1 : retval);
}

public sh_T __init_sh__ (void) {
  ProcT = __init_proc__ ();
  CstringT = __init_cstring__ ();

  return (sh_T) {
    .self = (sh_self) {
      .new = sh_new,
      .exec = sh_exec,
      .release = sh_release,
      .release_list = sh_release_list
    }
  };
}
