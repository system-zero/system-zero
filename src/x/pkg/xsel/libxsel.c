/*
 * xsel -- manipulate the X selection
 * Copyright (C) 2001 Conrad Parker <conrad@vergenet.net>
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 */

/* This is xsel from Conrad Parker:
   https://vergenet.net/~conrad/software/xsel/

   with a couple lines of latest development from:
   https://github.com/kfish/xsel

   The main difference is that is exposed as a library.
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <setjmp.h>
#include <signal.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <xsel.h>

static Display *Dpy = NULL;
static Window XWindow;

#define Alloc malloc
#define Release free

#define MAXLEN_ERR 4096
#define DO_OUTPUT 1
#define DO_INPUT  2

#define MIN(a,b) ((a)<(b)?(a):(b))

#define empty_string(s) (s==NULL||s[0]=='\0')

#define D_FATAL 0
#define D_WARN  1
#define D_INFO  2
#define D_TRACE 3

#define DEBUG_LEVEL D_FATAL

/* An instance of a MULTIPLE SelectionRequest being served */
typedef struct _MultTrack MultTrack;

struct _MultTrack {
  MultTrack *mparent;
  Display *dpy;
  Window requestor;
  Atom property;
  Atom selection;
  Time time;
  Atom *atoms;
  unsigned long length;
  unsigned long index;
  unsigned char *sel;
};

/* Selection serving states */
typedef enum {
  S_NULL=0,
  S_INCR_1,
  S_INCR_2
} IncrState;

/* An instance of a selection being served */
typedef struct _IncrTrack IncrTrack;

struct _IncrTrack {
  MultTrack *mparent;
  IncrTrack *prev, *next;
  IncrState state;
  Display *dpy;
  Window requestor;
  Atom property;
  Atom selection;
  Time time;
  Atom target;
  int format;
  unsigned char *data;
  int nelements; /* total */
  int offset, chunk, max_elements; /* all in terms of nelements */
};

/* Status of request handling */
typedef int HandleResult;
#define HANDLE_OK         0
#define HANDLE_ERR        (1<<0)
#define HANDLE_INCOMPLETE (1<<1)
#define DID_DELETE        (1<<2)

static sigset_t exit_sigs;

/* The jmp_buf to longjmp out of the signal handler */
static sigjmp_buf EnvAlrm;

/* Verbosity level for debugging */
static int DebugLevel = DEBUG_LEVEL;

/* Maxmimum request size supported by this X server */
static long MaxReq;

/* Our timestamp for all operations */
static Time TimeStamp;

static Atom TimestampAtom; /* The TIMESTAMP atom */
static Atom MultipleAtom; /* The MULTIPLE atom */
static Atom TargetsAtom; /* The TARGETS atom */
static Atom DeleteAtom; /* The DELETE atom */
static Atom IncrAtom; /* The INCR atom */
static Atom NullAtom; /* The NULL atom */
static Atom TextAtom; /* The TEXT atom */
static Atom UTF8Atom; /* The UTF8 atom */
static Atom CompoundTextAtom; /* The COMPOUND_TEXT atom */

/* Number of selection targets served by this.
 * (MULTIPLE, INCR, TARGETS, TIMESTAMP, DELETE, TEXT, UTF8_STRING and STRING)
 * NB. We do not currently serve COMPOUND_TEXT; we can retrieve it but do not
 * perform charset conversion.
 */
#define MAX_NUM_TARGETS 9
static int NUM_TARGETS;
static Atom SUPPORTED_TARGETS[MAX_NUM_TARGETS];

static int Daemon = False;

/* fstat() on stdin and stdout */
static struct stat InputStatBuf, OutputStatBuf;

static long TimeOut = 0;
static struct itimerval Timer;
static struct itimerval Zerot;

#define USEC_PER_SEC 1000000

static void exit_err (const char * fmt, ...) {
  va_list ap;
  int errno_save;
  char buf[MAXLEN_ERR];
  int n;

  errno_save = errno;

  va_start (ap, fmt);

  vsnprintf (buf, MAXLEN_ERR, fmt, ap);
  n = strlen (buf);

  snprintf (buf+n, MAXLEN_ERR-n, ": %s\n", strerror (errno_save));

  fflush (stdout); /* in case stdout and stderr are the same */
  fputs (buf, stderr);
  fflush (NULL);

  va_end (ap);
  exit (1);
}

static void print_err (const char * fmt, ...) {
  va_list ap;
  char buf[MAXLEN_ERR];

  va_start (ap, fmt);

  vsnprintf (buf, MAXLEN_ERR, fmt, ap);

  fflush (stdout); /* in case stdout and stderr are the same */
  fputs (buf, stderr);
  fputc ('\n', stderr);
  fflush (NULL);

  va_end (ap);
}

#define print_debug(x,y...) {if (x <= DebugLevel) print_err (y);}

static void *xs_malloc (size_t size) {
  void *ret;

  if (0 == size)
    size = 1;

  if (NULL == (ret = Alloc (size)))
    exit_err ("malloc error");

  return ret;
}

#define xs_strdup(s) ((unsigned char *) _xs_strdup ((const char *)s))

static char *_xs_strdup (const char *s) {
  char *ret;

  if (s == NULL)
    return NULL;

  if (NULL == (ret = strdup (s)))
    exit_err ("strdup error");

  return ret;
}

/*
 * If we exit in the middle of handling a SelectionRequest, we might leave the
 * requesting client hanging, so we try to be nice and finish handling
 * requests before terminating.  Hence we block SIG{ALRM,INT,TERM} while
 * handling requests and unblock them only while waiting in XNextEvent().
 */

static void block_exit_sigs (void) {
  sigprocmask (SIG_BLOCK, &exit_sigs, NULL);
}

static void unblock_exit_sigs (void) {
  sigprocmask (SIG_UNBLOCK, &exit_sigs, NULL);
}

static void alarm_handler (int sig) {
  (void) sig;
  siglongjmp (EnvAlrm, 1);
}

static void set_timer_time_out (void) {
  Timer.it_interval.tv_sec = TimeOut / USEC_PER_SEC;
  Timer.it_interval.tv_usec = TimeOut % USEC_PER_SEC;
  Timer.it_value.tv_sec = TimeOut / USEC_PER_SEC;
  Timer.it_value.tv_usec = TimeOut % USEC_PER_SEC;
}

static void set_daemon_time_out (void) {
  if (SIG_ERR == signal (SIGALRM, alarm_handler))
    exit_err ("error setting TimeOut handler");

  set_timer_time_out ();

  if (0 == sigsetjmp (EnvAlrm, 0))
    setitimer (ITIMER_REAL, &Timer, (struct itimerval *)0);
  else {
    print_debug (D_INFO, "daemon exiting after %d ms", TimeOut / 1000);
    exit (0);
  }
}

/*
 * become_daemon ()
 *
 * Perform the required procedure to become a daemon process, as
 * outlined in the Unix programming FAQ:
 * http://www.steve.org.uk/Reference/Unix/faq_2.html#SEC16
 */

static void become_daemon (void) {
  pid_t pid;
  int null_r_fd, null_w_fd;

  if (0 == Daemon) {
    /* If the user has specified a TimeOut, enforce it even if we don't
     * actually daemonize */
    set_daemon_time_out ();
    return;
  }

  if (-1 == (pid = fork ()))
    exit_err ("error forking");

  if (pid > 0)
    _exit (0);

  if (-1 == setsid ())
    exit_err ("setsid error");

  if (-1 == (pid = fork()))
    exit_err ("error forking");

  if (pid > 0)
    _exit (0);

  umask (0);

  /* dup2 /dev/null on stdin unless following input */
  null_r_fd = open ("/dev/null", O_RDONLY);
  if (-1 == null_r_fd)
    exit_err ("error opening /dev/null for reading");

  if (-1 == dup2 (null_r_fd, 0))
    exit_err ("error duplicating /dev/null on stdin");

  /* dup2 /dev/null on stdout */
  null_w_fd = open ("/dev/null", O_WRONLY|O_APPEND);
  if (-1 == null_w_fd)
    exit_err ("error opening /dev/null for writing");

  if (-1 == dup2 (null_w_fd, 1))
    exit_err ("error duplicating /dev/null on stdout");

  set_daemon_time_out ();
}

/*
 * get_timestamp ()
 *
 * Get the current X server time.
 *
 * This is done by doing a zero-length append to a random property of the
 * window, and checking the time on the subsequent PropertyNotify event.
 *
 * PRECONDITION: the window must have PropertyChangeMask set.
 */

static Time get_timestamp (void) {
  XEvent event;

  XChangeProperty (Dpy, XWindow, XA_WM_NAME, XA_STRING, 8,
                   PropModeAppend, NULL, 0);

  while (1) {
    XNextEvent (Dpy, &event);

    if (event.type == PropertyNotify)
      return event.xproperty.time;
  }
}

/*
 * SELECTION RETRIEVAL
 * ===================
 *
 * The following functions implement retrieval of an X selection,
 * optionally within a user-specified TimeOut.
 *
 *
 * Selection TimeOut handling.
 * ---------------------------
 *
 * The selection retrieval can time out if no response is received within
 * a user-specified time limit. In order to ensure we time the entire
 * selection retrieval, we use an interval timer and catch SIGALRM.
 * [Calling select() on the XConnectionNumber would only provide a TimeOut
 * to the first XEvent.]
 */

/*
 * get_append_property ()
 *
 * Get a window property and append its data to a buffer at a given offset
 * pointed to by *offset. 'offset' is modified by this routine to point to
 * the end of the data.
 *
 * Returns True if more data is available for receipt.
 *
 * If an error is encountered, the buffer is free'd.
 */
static Bool get_append_property (XSelectionEvent * xsl, unsigned char ** buffer,
                     unsigned long * offset, unsigned long * alloc) {
  unsigned char * ptr;
  Atom target;
  int format;
  unsigned long bytesafter, length;
  unsigned char * value;

  XGetWindowProperty (xsl->display, xsl->requestor, xsl->property,
                      0L, 1000000, True, (Atom)AnyPropertyType,
                      &target, &format, &length, &bytesafter, &value);

  if (target != XA_STRING && target != UTF8Atom &&
      target != CompoundTextAtom) {

    Release (*buffer);
    *buffer = NULL;
    return False;
  } else if (length == 0) {
    /* A length of 0 indicates the end of the transfer */
    print_debug (D_TRACE, "Got zero length property; end of INCR transfer");
    return False;
  } else if (format == 8) {
    if (*offset + length + 1 > *alloc) {
      *alloc = *offset + length + 1;
      if ((*buffer = realloc (*buffer, *alloc)) == NULL) {
        exit_err ("realloc error");
      }
    }
    ptr = *buffer + *offset;
    memcpy (ptr, value, length);
    ptr[length] = '\0';
    *offset += length;
    print_debug (D_TRACE, "Appended %d bytes to buffer\n", length);
  } else {
    print_debug (D_WARN, "Retrieved non-8-bit data\n");
  }

  return True;
}


/*
 * wait_incr_selection (selection)
 *
 * Retrieve a property of target type INCR. Perform incremental retrieval
 * and return the resulting data.
 */

static unsigned char *
  wait_incr_selection (Atom selection, XSelectionEvent *xsl, int init_alloc) {
  (void) selection;

  XEvent event;
  unsigned char *incr_base = NULL;
  unsigned long incr_alloc = 0, incr_xfer = 0;
  Bool wait_prop = True;

  print_debug (D_TRACE, "Initialising incremental retrieval of at least %d bytes\n", init_alloc);

  /* Take an interest in the requestor */
  XSelectInput (xsl->display, xsl->requestor, PropertyChangeMask);

  incr_alloc = init_alloc;
  incr_base = xs_malloc (incr_alloc);

  print_debug (D_TRACE, "Deleting property that informed of INCR transfer");
  XDeleteProperty (xsl->display, xsl->requestor, xsl->property);

  print_debug (D_TRACE, "Waiting on PropertyNotify events");
  while (wait_prop) {
    XNextEvent (xsl->display, &event);

    switch (event.type) {
      case PropertyNotify:
        if (event.xproperty.state != PropertyNewValue)
          break;

        wait_prop = get_append_property (xsl, &incr_base, &incr_xfer,
                                       &incr_alloc);
        break;
      default:
        break;
    }
  }

  /* when zero length found, finish up & delete last */
  XDeleteProperty (xsl->display, xsl->requestor, xsl->property);

  print_debug (D_TRACE, "Finished INCR retrieval");

  return incr_base;
}

/*
 * wait_selection (selection, request_target)
 *
 * Block until we receive a SelectionNotify event, and return its
 * contents; or NULL in the case of a deletion or error. This assumes we
 * have already called XConvertSelection, requesting a string (explicitly
 * XA_STRING) or deletion (DeleteAtom).
 */

static unsigned char *wait_selection (Atom selection, Atom request_target) {
  XEvent event;
  Atom target;
  int format;
  unsigned long bytesafter, length;
  unsigned char *value, *retval = NULL;
  Bool keep_waiting = True;

  while (keep_waiting) {
    XNextEvent (Dpy, &event);

    switch (event.type) {
      case SelectionNotify:
        if (event.xselection.selection != selection)
          break;

      if (event.xselection.property == None) {
        print_debug (D_WARN, "Conversion refused");
        value = NULL;
        keep_waiting = False;
      } else if (event.xselection.property == NullAtom &&
                 request_target == DeleteAtom) {
      } else {
        XGetWindowProperty (event.xselection.display,
            event.xselection.requestor,
            event.xselection.property, 0L, 1000000,
            False, (Atom)AnyPropertyType, &target,
            &format, &length, &bytesafter, &value);

        if (request_target == DeleteAtom && value == NULL) {
          keep_waiting = False;
        } else if (target == IncrAtom) {
          /* Handle INCR transfers */
          retval = wait_incr_selection (selection, &event.xselection,
                                        *(long *)value);
          keep_waiting = False;
        } else if (target != UTF8Atom && target != XA_STRING &&
                   target != CompoundTextAtom &&
                   request_target != DeleteAtom) {
          /* Report non-TEXT atoms */
          free (retval);
          retval = NULL;
          keep_waiting = False;
        } else {
          retval = xs_strdup (value);
          XFree (value);
          keep_waiting = False;
        }

        XDeleteProperty (event.xselection.display,
                         event.xselection.requestor,
                         event.xselection.property);

        }

        break;

      default:
        break;
    }
  }

  /* Now that we've received the SelectionNotify event, clear any
   * remaining TimeOut. */
  if (TimeOut > 0)
    setitimer (ITIMER_REAL, &Zerot, (struct itimerval *)0);
    //setitimer (ITIMER_REAL, (struct itimerval *)0, (struct itimerval *)0);

  return retval;
}

/*
 * get_selection (selection, request_target)
 *
 * Retrieves the specified selection and returns its value.
 *
 * If a non-zero TimeOut is specified then set a virtual interval
 * timer. Return NULL and print an error message if the TimeOut
 * expires before the selection has been retrieved.
 */

static unsigned char *get_selection (Atom selection, Atom request_target) {
  Atom prop;
  unsigned char *retval;

  prop = XInternAtom (Dpy, "XSEL_DATA", False);
  XConvertSelection (Dpy, selection, request_target, prop, XWindow,
                     TimeStamp);
  XSync (Dpy, False);

  if (TimeOut > 0) {
    if (SIG_ERR == signal (SIGALRM, alarm_handler))
      exit_err ("error setting TimeOut handler");

    set_timer_time_out ();

    if (0 == sigsetjmp (EnvAlrm, 0)) {
      setitimer (ITIMER_REAL, &Timer, (struct itimerval *)0);
      retval = wait_selection (selection, request_target);
    } else {
      print_debug (D_WARN, "selection timed out");
      retval = NULL;
    }
  } else
    retval = wait_selection (selection, request_target);

  return retval;
}

/*
 * get_selection_text (Atom selection)
 *
 * Retrieve a text selection. First attempt to retrieve it as UTF_STRING,
 * and if that fails attempt to retrieve it as a plain XA_STRING.
 *
 * NB. Before implementing this, an attempt was made to query TARGETS and
 * request UTF8_STRING only if listed there, as described in:
 * http://www.pps.jussieu.fr/~jch/software/UTF8_STRING/UTF8_STRING.text
 * However, that did not seem to work reliably when tested against various
 * applications (eg. Mozilla Firefox). This method is of course more
 * reliable.
 */

static unsigned char *get_selection_text (Atom selection) {
  unsigned char *retval;

  if (NULL == (retval = get_selection (selection, UTF8Atom)))
    retval = get_selection (selection, XA_STRING);

  return retval;
}


/* Forward declaration of refuse_all_incr () */
static void refuse_all_incr (void);

/*
 * handle_x_errors ()
 *
 * XError handler.
 */

static int handle_x_errors (Display * dpy, XErrorEvent * eev) {
  char err_buf[MAXLEN_ERR];

  /* Make sure to send a refusal to all waiting INCR requests
   * and delete the corresponding properties. */
  if (eev->error_code == BadAlloc)
    refuse_all_incr ();

  XGetErrorText (dpy, eev->error_code, err_buf, MAXLEN_ERR);
  exit_err (err_buf);

  return 0;
}

/*
 * clear_selection (selection)
 *
 * Clears the specified X selection 'selection'. This requests that no
 * process should own 'selection'; thus the X server will respond to
 * SelectionRequests with an empty property and we don't need to leave
 * a daemon hanging around to service this selection.
 */

static void clear_selection (Atom selection) {
  XSetSelectionOwner (Dpy, selection, None, TimeStamp);
  /* Call XSync to ensure this operation completes before program
   * termination, especially if this is all we are doing. */
  XSync (Dpy, False);
}

/*
 * own_selection (selection)
 *
 * Requests ownership of the X selection. Returns True if ownership was
 * granted, and False otherwise.
 */

static Bool own_selection (Atom selection) {
  Window owner;

  XSetSelectionOwner (Dpy, selection, XWindow, TimeStamp);
  /* XGetSelectionOwner does a round trip to the X server, so there is
   * no need to call XSync here. */
  owner = XGetSelectionOwner (Dpy, selection);
  if (owner != XWindow)
    return False;
  else {
    XSetErrorHandler (handle_x_errors);
    return True;
  }
}


static IncrTrack *incrtrack_list = NULL;

/*
 * add_incrtrack (it)
 *
 * Add 'it' to the head of incrtrack_list.
 */
static void add_incrtrack (IncrTrack *it) {
  if (incrtrack_list)
    incrtrack_list->prev = it;

  it->prev = NULL;
  it->next = incrtrack_list;
  incrtrack_list = it;
}

/*
 * remove_incrtrack (it)
 *
 * Remove 'it' from incrtrack_list.
 */

static void remove_incrtrack (IncrTrack *it) {
  if (it->prev)
    it->prev->next = it->next;

  if (it->next)
    it->next->prev = it->prev;

  if (incrtrack_list == it)
    incrtrack_list = it->next;
}

/*
 * fresh_incrtrack ()
 *
 * Create a new incrtrack, and add it to incrtrack_list.
 */

static IncrTrack *fresh_incrtrack (void) {
  IncrTrack *it;

  it = xs_malloc (sizeof (IncrTrack));
  add_incrtrack (it);

  return it;
}

/*
 * trash_incrtrack (it)
 *
 * Remove 'it' from incrtrack_list, and free it.
 */

static void trash_incrtrack (IncrTrack *it) {
  remove_incrtrack (it);
  free (it);
}

/*
 * find_incrtrack (atom)
 *
 * Find the IncrTrack structure within incrtrack_list pertaining to 'atom',
 * if it exists.
 */

static IncrTrack *find_incrtrack (Atom atom) {
  IncrTrack *iti;

  for (iti = incrtrack_list; iti; iti = iti->next)
    if (atom == iti->property)
      return iti;

  return NULL;
}

/* Forward declaration of handle_multiple() */
static HandleResult
handle_multiple (Display *Dpy, Window requestor, Atom property,
                 unsigned char * sel, Atom selection, Time time,
                 MultTrack * mparent);

/* Forward declaration of process_multiple() */
static HandleResult process_multiple (MultTrack * mt, Bool do_parent);

/*
 * confirm_incr (it)
 *
 * Confirm the selection request of ITER tracked by 'it'.
 */

static void notify_incr (IncrTrack *it, HandleResult hr) {
  XSelectionEvent ev;

  /* Call XSync here to make sure any BadAlloc errors are caught before
   * confirming the conversion. */
  XSync (it->dpy, False);

  print_debug (D_TRACE, "Confirming conversion");

  /* Prepare a SelectionNotify event to send, placing the selection in the
   * requested property. */
  ev.type = SelectionNotify;
  ev.display = it->dpy;
  ev.requestor = it->requestor;
  ev.selection = it->selection;
  ev.time = it->time;
  ev.target = it->target;

  if (hr & HANDLE_ERR)
    ev.property = None;
  else
    ev.property = it->property;

  XSendEvent (Dpy, ev.requestor, False,
      (unsigned long)NULL, (XEvent *)&ev);
}

/*
 * refuse_all_incr ()
 *
 * Refuse all INCR transfers in progress. ASSUMES that this is called in
 * response to an error, and that the program is about to bail out;
 * ie. incr_track is not cleaned out.
 */

static void refuse_all_incr (void) {
  IncrTrack *it;

  for (it = incrtrack_list; it; it = it->next) {
    XDeleteProperty (it->dpy, it->requestor, it->property);
    notify_incr (it, HANDLE_ERR);
    /* Don't bother trashing and list-removing these; we are about to
     * bail out anyway. */
  }
}

/*
 * complete_incr (it)
 *
 * Finish off an INCR retrieval. If it was part of a multiple, continue
 * that; otherwise, send confirmation that this completed.
 */

static void complete_incr (IncrTrack *it, HandleResult hr) {
  MultTrack *mparent = it->mparent;

  if (mparent) {
    trash_incrtrack (it);
    process_multiple (mparent, True);
  } else {
    notify_incr (it, hr);
    trash_incrtrack (it);
  }
}

/*
 * notify_multiple (mt, hr)
 *
 * Confirm the selection request initiated with MULTIPLE tracked by 'mt'.
 */

static void notify_multiple (MultTrack *mt, HandleResult hr) {
  XSelectionEvent ev;

  /* Call XSync here to make sure any BadAlloc errors are caught before
   * confirming the conversion. */
  XSync (mt->dpy, False);

  /* Prepare a SelectionNotify event to send, placing the selection in the
   * requested property. */
  ev.type = SelectionNotify;
  ev.display = mt->dpy;
  ev.requestor = mt->requestor;
  ev.selection = mt->selection;
  ev.time = mt->time;
  ev.target = MultipleAtom;

  if (hr & HANDLE_ERR)
    ev.property = None;
  else
    ev.property = mt->property;

  XSendEvent (Dpy, ev.requestor, False,
              (unsigned long)NULL, (XEvent *)&ev);
}

/*
 * complete_multiple (mt, do_parent, hr)
 *
 * Complete a MULTIPLE transfer. Iterate to its parent MULTIPLE if
 * 'do_parent' is true. If there is not parent MULTIPLE, send notification
 * of its completion with status 'hr'.
 */

static void complete_multiple (MultTrack *mt, Bool do_parent, HandleResult hr) {
  MultTrack *mparent = mt->mparent;

  if (mparent) {
    free (mt);
    if (do_parent)
      process_multiple (mparent, True);
  } else {
    notify_multiple (mt, hr);
    free (mt);
  }
}

/*
 * change_property (dpy, requestor, property, target, format, mode,
 *                  data, nelements)
 *
 * Wrapper to XChangeProperty that performs INCR transfer if required and
 * returns status of entire transfer.
 */

static HandleResult
change_property (Display *dpy, Window requestor, Atom property,
                 Atom target, int format, int mode,
                 unsigned char *data, int nelements,
                 Atom selection, Time time, MultTrack *mparent) {
  XSelectionEvent ev;
  long nr_bytes;
  IncrTrack *it;

  print_debug (D_TRACE, "change_property ()");

  nr_bytes = nelements * format / 8;

  if (nr_bytes <= MaxReq) {
    print_debug (D_TRACE, "data within maximum request size");
    XChangeProperty (dpy, requestor, property, target, format, mode,
                     data, nelements);

    return HANDLE_OK;
  }

  /* else */
  print_debug (D_TRACE, "large data transfer");

  /* Send a SelectionNotify event */
  ev.type = SelectionNotify;
  ev.display = dpy;
  ev.requestor = requestor;
  ev.selection = selection;
  ev.time = time;
  ev.target = target;
  ev.property = property;

  XSelectInput (ev.display, ev.requestor, PropertyChangeMask);

  XChangeProperty (ev.display, ev.requestor, ev.property, IncrAtom, 32,
                   PropModeReplace, (unsigned char *)&nr_bytes, 1);

  XSendEvent (dpy, requestor, False,
              (unsigned long)NULL, (XEvent *)&ev);

  /* Set up the IncrTrack to track this */
  it = fresh_incrtrack ();

  it->mparent = mparent;
  it->state = S_INCR_1;
  it->dpy = dpy;
  it->requestor = requestor;
  it->property = property;
  it->selection = selection;
  it->time = time;
  it->target = target;
  it->format = format;
  it->data = data;
  it->nelements = nelements;
  it->offset = 0;

  /* Maximum nr. of elements that can be transferred in one go */
  it->max_elements = MaxReq * 8 / format;

  /* Nr. of elements to transfer in this instance */
  it->chunk = MIN (it->max_elements, it->nelements - it->offset);

  return HANDLE_INCOMPLETE;
}

static HandleResult incr_stage_1 (IncrTrack *it) {
  XChangeProperty (it->dpy, it->requestor, it->property, it->target,
                   it->format, PropModeReplace, it->data, it->chunk);

  it->offset += it->chunk;

  it->state = S_INCR_2;

  return HANDLE_INCOMPLETE;
}

static HandleResult incr_stage_2 (IncrTrack *it) {
  it->chunk = MIN (it->max_elements, it->nelements - it->offset);

  if (it->chunk <= 0) {
    /* Now write zero-length data to the property */
    XChangeProperty (it->dpy, it->requestor, it->property, it->target,
                     it->format, PropModeAppend, NULL, 0);
    it->state = S_NULL;
    print_debug (D_TRACE, "Set si to state S_NULL");
    return HANDLE_OK;
  } else {
    print_debug (D_TRACE, "Writing chunk (%d bytes) to property",
                 it->chunk);
    XChangeProperty (it->dpy, it->requestor, it->property, it->target,
                     it->format, PropModeAppend, it->data+it->offset,
                     it->chunk);
    it->offset += it->chunk;
    print_debug (D_TRACE, "%d bytes remaining",
                 it->nelements - it->offset);
    return HANDLE_INCOMPLETE;
  }
}


/*
 * handle_timestamp (dpy, requestor, property)
 *
 * Handle a TIMESTAMP request.
 */

static HandleResult
handle_timestamp (Display *dpy, Window requestor, Atom property,
                  Atom selection, Time time, MultTrack *mparent) {
  return
    change_property (dpy, requestor, property, XA_INTEGER, 32,
                     PropModeReplace, (unsigned char *)&TimeStamp, 1,
                     selection, time, mparent);
}

/*
 * handle_targets (dpy, requestor, property)
 *
 * Handle a TARGETS request.
 */

static HandleResult
handle_targets (Display *dpy, Window requestor, Atom property,
                Atom selection, Time time, MultTrack *mparent) {
  Atom *targets_cpy;
  HandleResult r;

  targets_cpy = Alloc (sizeof (SUPPORTED_TARGETS));
  memcpy (targets_cpy, SUPPORTED_TARGETS, sizeof (SUPPORTED_TARGETS));

  r = change_property (dpy, requestor, property, XA_ATOM, 32,
                     PropModeReplace, (unsigned char *)targets_cpy,
                     NUM_TARGETS, selection, time, mparent);
  Release (targets_cpy);
  return r;
}

/*
 * handle_string (dpy, requestor, property, sel)
 *
 * Handle a STRING request; setting 'sel' as the data
 */
static HandleResult
handle_string (Display *dpy, Window requestor, Atom property,
               unsigned char *sel, Atom selection, Time time,
               MultTrack *mparent) {
  return
    change_property (dpy, requestor, property, XA_STRING, 8,
                     PropModeReplace, sel, strlen ((char *) sel),
                     selection, time, mparent);
}

/*
 * handle_utf8_string (dpy, requestor, property, sel)
 *
 * Handle a UTF8_STRING request; setting 'sel' as the data
 */
static HandleResult
handle_utf8_string (Display *dpy, Window requestor, Atom property,
                    unsigned char *sel, Atom selection, Time time,
                    MultTrack *mparent) {
  return change_property (dpy, requestor, property, UTF8Atom, 8,
                     PropModeReplace, sel, strlen ((char *) sel),
                     selection, time, mparent);
}

/*
 * handle_delete (dpy, requestor, property)
 *
 * Handle a DELETE request.
 */

static HandleResult handle_delete (Display *dpy, Window requestor, Atom property) {
  XChangeProperty (dpy, requestor, property, NullAtom, 0,
                   PropModeReplace, NULL, 0);

  return DID_DELETE;
}

/*
 * process_multiple (mt, do_parent)
 *
 * Iterate through a MultTrack until it completes, or until one of its
 * entries initiates an interated selection.
 *
 * If 'do_parent' is true, and the actions proscribed in 'mt' are
 * completed during the course of this call, then process_multiple
 * is iteratively called on mt->mparent.
 */

static HandleResult process_multiple (MultTrack *mt, Bool do_parent) {
  HandleResult retval = HANDLE_OK;
  unsigned long i;

  if (!mt)
    return retval;

  for (; mt->index < mt->length; mt->index += 2) {
    i = mt->index;
    if (mt->atoms[i] == TimestampAtom)
      retval |= handle_timestamp (mt->dpy, mt->requestor, mt->atoms[i+1],
                                  mt->selection, mt->time, mt);
    else if (mt->atoms[i] == TargetsAtom)
      retval |= handle_targets (mt->dpy, mt->requestor, mt->atoms[i+1],
                                mt->selection, mt->time, mt);
    else if (mt->atoms[i] == MultipleAtom)
      retval |= handle_multiple (mt->dpy, mt->requestor, mt->atoms[i+1],
                                 mt->sel, mt->selection, mt->time, mt);
    else if (mt->atoms[i] == XA_STRING || mt->atoms[i] == TextAtom)
      retval |= handle_string (mt->dpy, mt->requestor, mt->atoms[i+1],
                               mt->sel, mt->selection, mt->time, mt);
    else if (mt->atoms[i] == UTF8Atom)
      retval |= handle_utf8_string (mt->dpy, mt->requestor, mt->atoms[i+1],
                                    mt->sel, mt->selection, mt->time, mt);
    else if (mt->atoms[i] == DeleteAtom)
      retval |= handle_delete (mt->dpy, mt->requestor, mt->atoms[i+1]);
    else if (mt->atoms[i] == None) {
      /* the only other thing we know to handle is None, for which we
       * do nothing. This block is, like, __so__ redundant. Welcome to
       * Over-engineering 101 :) This comment is just here to keep the
       * logic documented and separate from the 'else' block. */
    } else
      /* for anything we don't know how to handle, we fail the conversion
       * by setting this: */
      mt->atoms[i] = None;

    /* If any of the conversions failed, signify this by setting that
     * atom to None ...*/
    if (retval & HANDLE_ERR)
      mt->atoms[i] = None;

    /* ... but don't propogate HANDLE_ERR */
    retval &= (~HANDLE_ERR);

    if (retval & HANDLE_INCOMPLETE)
      break;
  }

  if ((retval & HANDLE_INCOMPLETE) == 0)
    complete_multiple (mt, do_parent, retval);

  return retval;
}

/*
 * continue_incr (it)
 *
 * Continue an incremental transfer of IncrTrack * it.
 *
 * NB. If the incremental transfer was part of a multiple request, this
 * function calls process_multiple with do_parent=True because it is
 * assumed we are continuing an interrupted ITER, thus we must continue
 * the multiple as its original handler did not complete.
 */

static HandleResult continue_incr (IncrTrack *it) {
  HandleResult retval = HANDLE_OK;

  if (it->state == S_INCR_1)
    retval = incr_stage_1 (it);
  else if (it->state == S_INCR_2)
    retval = incr_stage_2 (it);

  /* If that completed the INCR, deal with completion */
  if ((retval & HANDLE_INCOMPLETE) == 0)
    complete_incr (it, retval);

  return retval;
}

/*
 * handle_multiple (dpy, requestor, property, sel, selection, time)
 *
 * Handle a MULTIPLE request; possibly setting 'sel' if any STRING
 * requests are processed within it. Return value has DID_DELETE bit set
 * if any delete requests are processed.
 *
 * NB. This calls process_multiple with do_parent=False because it is
 * assumed we are "handling" the multiple request on behalf of a
 * multiple already in progress, or (more likely) directly off a
 * SelectionRequest event.
 */

static HandleResult
handle_multiple (Display *dpy, Window requestor, Atom property,
                 unsigned char *sel, Atom selection, Time time,
                 MultTrack *mparent) {
  MultTrack *mt;
  int format;
  Atom type;
  unsigned long bytesafter;

  mt = xs_malloc (sizeof (MultTrack));

  XGetWindowProperty (dpy, requestor, property, 0L, 1000000,
                      False, (Atom)AnyPropertyType, &type,
                      &format, &mt->length, &bytesafter,
                      (unsigned char **)&mt->atoms);

  /* Make sure we got the Atom list we want */
  if (format != 32)
    return HANDLE_OK;

  mt->mparent = mparent;
  mt->dpy = dpy;
  mt->requestor = requestor;
  mt->sel = sel;
  mt->property = property;
  mt->selection = selection;
  mt->time = time;
  mt->index = 0;

  return process_multiple (mt, False);
}

/*
 * handle_selection_request (event, sel)
 *
 * Processes a SelectionRequest event 'event' and replies to its
 * sender appropriately, eg. with the contents of the string 'sel'.
 * Returns False if a DELETE request is processed, indicating to
 * the calling function to delete the corresponding selection.
 * Returns True otherwise.
 */

static Bool handle_selection_request (XEvent event, unsigned char *sel) {
  XSelectionRequestEvent *xsr = &event.xselectionrequest;
  XSelectionEvent ev;
  HandleResult hr = HANDLE_OK;
  Bool retval = True;

  /* Prepare a SelectionNotify event to send, either as confirmation of
   * placing the selection in the requested property, or as notification
   * that this could not be performed. */
  ev.type = SelectionNotify;
  ev.display = xsr->display;
  ev.requestor = xsr->requestor;
  ev.selection = xsr->selection;
  ev.time = xsr->time;
  ev.target = xsr->target;

  if (xsr->property == None && ev.target != MultipleAtom)
      /* Obsolete requestor */
      xsr->property = xsr->target;

  if (ev.time != CurrentTime && ev.time < TimeStamp)
    /* If the time is outside the period we have owned the selection,
     * which is any time later than timestamp, or if the requested target
     * is not a string, then refuse the SelectionRequest. NB. Some broken
     * clients don't set a valid timestamp, so we have to check against
     * CurrentTime here. */
    ev.property = None;
  else if (ev.target == TimestampAtom) {
    /* Return timestamp used to acquire ownership if target is TIMESTAMP */
    ev.property = xsr->property;
    hr = handle_timestamp (ev.display, ev.requestor, ev.property,
                           ev.selection, ev.time, NULL);
  } else if (ev.target == TargetsAtom) {
    /* Return a list of supported targets (TARGETS)*/
    ev.property = xsr->property;
    hr = handle_targets (ev.display, ev.requestor, ev.property,
                         ev.selection, ev.time, NULL);
  } else if (ev.target == MultipleAtom) {
    if (xsr->property == None) {
      /* Invalid MULTIPLE request */
      ev.property = None;
    } else {
      /* Handle MULTIPLE request */
      ev.property = xsr->property;
      hr = handle_multiple (ev.display, ev.requestor, ev.property, sel,
                            ev.selection, ev.time, NULL);
    }
  } else if (ev.target == XA_STRING || ev.target == TextAtom) {
    /* Received STRING or TEXT request */
    ev.property = xsr->property;
    hr = handle_string (ev.display, ev.requestor, ev.property, sel,
                        ev.selection, ev.time, NULL);
  } else if (ev.target == UTF8Atom) {
    /* Received UTF8_STRING request */
    ev.property = xsr->property;
    hr = handle_utf8_string (ev.display, ev.requestor, ev.property, sel,
                             ev.selection, ev.time, NULL);
  } else if (ev.target == DeleteAtom) {
    /* Received DELETE request */
    ev.property = xsr->property;
    hr = handle_delete (ev.display, ev.requestor, ev.property);
    retval = False;
  } else
    /* Cannot convert to requested target. This includes most non-string
     * datatypes, and INSERT_SELECTION, INSERT_PROPERTY */
    ev.property = None;

  /* Return False if a DELETE was processed */
  retval = (hr & DID_DELETE) ? False : True;

  /* If there was an error in the transfer, it should be refused */
  if (hr & HANDLE_ERR) {
    print_debug (D_TRACE, "Error in transfer");
    ev.property = None;
  }

  if (0 == (hr & HANDLE_INCOMPLETE)) {
    if (ev.property == None) {
      print_debug (D_TRACE, "Refusing conversion");
    } else
      print_debug (D_TRACE, "Confirming conversion");

    XSendEvent (Dpy, ev.requestor, False,
                (unsigned long)NULL, (XEvent *)&ev);

    /* If we return False here, we may quit immediately, so sync out the
     * X queue. */
    if (!retval)
      XSync (Dpy, False);
  }

  return retval;
}

/*
 * set_selection (selection, sel)
 *
 * Takes ownership of the selection 'selection', then loops waiting for
 * its SelectionClear or SelectionRequest events.
 *
 * Handles SelectionRequest events, first checking for additional
 * input if the user has specified 'follow' mode. Returns when a
 * SelectionClear event is received for the specified selection.
 */

static void set_selection (Atom selection, unsigned char *sel) {
  XEvent event;
  IncrTrack *it;

  if (False == own_selection (selection))
    return;

  for (;;) {
    /* Flush before unblocking signals so we send replies before exiting */
    XFlush (Dpy);
    unblock_exit_sigs ();
    XNextEvent (Dpy, &event);
    block_exit_sigs ();

    switch (event.type) {
      case SelectionClear:
        if (event.xselectionclear.selection == selection)
          return;
        break;

      case SelectionRequest:
        if (event.xselectionrequest.selection != selection)
          break;

        if (!handle_selection_request (event, sel))
          return;

        break;

      case PropertyNotify:
        if (event.xproperty.state != PropertyDelete)
          break;

        it = find_incrtrack (event.xproperty.atom);

        if (it != NULL)
          continue_incr (it);

        break;

      default:
        break;
      }
    }
}

/*
 * set_selection_daemon (selection, sel)
 *
 * Creates a daemon process to handle selection requests for the
 * specified selection 'selection', to respond with selection text 'sel'.
 * If 'sel' is an empty string (NULL or "") then no daemon process is
 * created and the specified selection is cleared instead.
 */

static void set_selection_daemon (Atom selection, unsigned char *sel) {
  if (empty_string (sel)) {
    clear_selection (selection);
    return;
  }

  become_daemon ();

  set_selection (selection, sel);
}

static void init_sel (Atom sel, int action) {
  (void) sel;
  int s = 0;
  long timeout_ms = 0L;

  Zerot.it_value.tv_sec = 0;
  Zerot.it_value.tv_usec = 0;
  Zerot.it_interval.tv_sec = 0;
  Zerot.it_interval.tv_usec = 0;

  if (action == DO_INPUT)
    if (-1 == fstat (0, &InputStatBuf))
      exit_err ("fstat error on stdin");

  if (action == DO_OUTPUT)
    if (-1 == fstat (1, &OutputStatBuf))
      exit_err ("fstat error on stdout");

  TimeOut = timeout_ms * 1000;

  TimeStamp = get_timestamp ();

  /* Get the maximum incremental selection size in bytes */
  /*MaxReq = MAX_SELECTION_INCR (dpy);*/
  MaxReq = 4000;

  NUM_TARGETS=0;

  /* Get the TIMESTAMP atom */
  TimestampAtom = XInternAtom (Dpy, "TIMESTAMP", False);
  SUPPORTED_TARGETS[s++] = TimestampAtom;
  NUM_TARGETS++;

  /* Get the MULTIPLE atom */
  MultipleAtom = XInternAtom (Dpy, "MULTIPLE", False);
  SUPPORTED_TARGETS[s++] = MultipleAtom;
  NUM_TARGETS++;

  /* Get the TARGETS atom */
  TargetsAtom = XInternAtom (Dpy, "TARGETS", False);
  SUPPORTED_TARGETS[s++] = TargetsAtom;
  NUM_TARGETS++;

  /* Get the DELETE atom */
  DeleteAtom = XInternAtom (Dpy, "DELETE", False);
  SUPPORTED_TARGETS[s++] = DeleteAtom;
  NUM_TARGETS++;

  /* Get the INCR atom */
  IncrAtom = XInternAtom (Dpy, "INCR", False);
  SUPPORTED_TARGETS[s++] = IncrAtom;
  NUM_TARGETS++;

  /* Get the TEXT atom */
  TextAtom = XInternAtom (Dpy, "TEXT", False);
  SUPPORTED_TARGETS[s++] = TextAtom;
  NUM_TARGETS++;

  /* Get the UTF8_STRING atom */
  UTF8Atom = XInternAtom (Dpy, "UTF8_STRING", True);
  if (UTF8Atom != None) {
    SUPPORTED_TARGETS[s++] = UTF8Atom;
    NUM_TARGETS++;
  } else
    UTF8Atom = XA_STRING;

  SUPPORTED_TARGETS[s++] = XA_STRING;
  NUM_TARGETS++;

  if (NUM_TARGETS > MAX_NUM_TARGETS)
    exit_err ("internal error num-targets (%d) > max-num-targets (%d)\n",
              NUM_TARGETS, MAX_NUM_TARGETS);

  /* Get the NULL atom */
  NullAtom = XInternAtom (Dpy, "NULL", False);

  /* Get the COMPOUND_TEXT atom.
   * NB. We do not currently serve COMPOUND_TEXT; we can retrieve it but
   * do not perform charset conversion.
   */
  CompoundTextAtom = XInternAtom (Dpy, "COMPOUND_TEXT", False);

  sigemptyset (&exit_sigs);
  sigaddset (&exit_sigs, SIGALRM);
  sigaddset (&exit_sigs, SIGINT);
  sigaddset (&exit_sigs, SIGTERM);
}

static int init_dpy () {
  if (Dpy) return 0;

  Dpy = XOpenDisplay (NULL);

  if (Dpy == NULL)
    return -1;

  Window root = XDefaultRootWindow (Dpy);

  int black;

  /* Create an unmapped window for receiving events */
  black = BlackPixel (Dpy, DefaultScreen (Dpy));
  XWindow = XCreateSimpleWindow (Dpy, root, 0, 0, 1, 1, 0, black, black);

  XStoreName (Dpy, XWindow, "xsel");

  XClassHint *class_hints;
  class_hints = XAllocClassHint();

  if (class_hints == NULL)
    exit_err ("Can't allocate class hints memory\n");

  class_hints->res_name   = (char *) "xsel";
  class_hints->res_class  = (char *) "XSel";
  XSetClassHint(Dpy, XWindow, class_hints);
  XFree(class_hints);

  XSelectInput (Dpy, XWindow, PropertyChangeMask);

  return 0;
}

void deinit_xsel (void) {
   XCloseDisplay (Dpy);
   Dpy = NULL;
}

char *xsel_get (xsel_opts_t opts) {
  if (-1 == init_dpy ())
    return NULL;

  TimeOut = opts.timeout;

  Atom selection;
  unsigned char *old_sel = NULL;

  if (opts.selection == XSEL_CLIPBOARD)
    selection = XInternAtom (Dpy, "CLIPBOARD", False);
  else
    selection = XA_PRIMARY;

  init_sel (selection, DO_OUTPUT);

  old_sel = get_selection_text (selection);

  if (old_sel) {
    size_t len = strlen ((char *) old_sel);
    char *res = Alloc (len + 1);
    strncpy (res, (char *) old_sel, len + 1);
    res[len] = '\0';

    if (opts.rm_last_nl) {
      if (res[len-1] == '\n')
        res[len-1] = '\0';
    }

    return res;
  }

  return NULL;
}

int xsel_put (char *buf, xsel_opts_t opts) {
  if (-1 == init_dpy ())
    return -1;

  TimeOut = opts.timeout;
  Daemon = opts.daemonize;

  Atom selection;

  if (opts.selection == XSEL_CLIPBOARD)
    selection = XInternAtom (Dpy, "CLIPBOARD", False);
  else
    selection = XA_PRIMARY;

  init_sel (selection, DO_INPUT);

  size_t buflen = strlen (buf);

  if (opts.rm_last_nl) {
    if (buf[buflen-1] == '\n') {
      buf[buflen-1] = '\0';
      buflen--;
    }
  }

  unsigned char
    *old_sel = NULL,
    *new_sel = NULL;

  if (opts.append_selection) {
    old_sel = get_selection_text (selection);
    new_sel = xs_malloc (strlen ((char *) old_sel) + buflen + 1);
    strcpy ((char *) new_sel, (char *) old_sel);
    strcat ((char *) new_sel, buf);
  } else {
    new_sel = xs_malloc (buflen + 1);
    strcpy ((char *) new_sel, buf);
  }

  set_selection_daemon (selection, new_sel);
  return 0;
}
