/* This is a very basic full screen window manager. It is based on a slang-module
 * written years ago, which the initial code was derived from dminiwm:
 * https://github.com/moetunes/dminiwm
 * Many thanks.
 */

/* It is mostly at least crash free, but the code could be optimized further
 * by studing a bit the X docs, though it works fast and with low resources.
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xlocale.h>
#include <X11/XKBlib.h>

#include <xwm.h>

struct xclient_t {
  Window win;

  xclient_t
    *next,
    *prev;

  int
    x,
    y,
    order,
    width,
    height;
};

struct xdesktop_t {
  int
    numwins,
    mode,
    growth;

  xclient_t
    *head,
    *current,
    *transient;
};

struct xOnMap_t {
  xOnMap_t *next;
  char *class;

  int
    desk,
    follow;
};

struct xpositional_t {
  xpositional_t *next;
  char *class;

  int
    x,
    y,
    width,
    height;
};

struct xwm_key_t {
  xwm_key_t *next;
  int modifier;
  KeySym keysym;
};

struct xfont_t {
  XFontStruct *font;          /* font structure */
  XFontSet fontset;           /* fontset structure */
  int height;                 /* height of the font */
  int width;
  unsigned int fh;            /* Y coordinate to draw characters */
  int ascent;
  int descent;
};

#define Alloc malloc
#define Release free

#define CLEANMASK(mask) (mask & ~(Xwm->numlockmask | LockMask))

static void xwm_maprequest (XEvent *);
static void xwm_keypress (XEvent *);
static void xwm_unmapnotify (XEvent *);
static void xwm_configurerequest (XEvent *);
static void xwm_destroynotify (XEvent *);
static void xwm_unmapnotify (XEvent *);

static void (*events[LASTEvent])(XEvent *e) = {
  [KeyPress] = xwm_keypress,
  [MapRequest] = xwm_maprequest,
  [UnmapNotify] = xwm_unmapnotify,
  [DestroyNotify] = xwm_destroynotify,
  [ConfigureRequest] = xwm_configurerequest
};

static struct xwm_t *Xwm;


static void xwm_add_window (Window, int, xclient_t *);
static void xwm_update_current (void);
static void xwm_save_desktop (int);
static void xwm_select_desktop (int);
static void xwm_tile (void);
static void xwm_remove_window (Window, int, int);
static unsigned long xwm_get_color (const char *);

// int (*xerrorxlib) (Display *, XErrorEvent *);
static int xerror (Display *dpy, XErrorEvent *ee) {
  (void) dpy;
  if (ee->error_code == BadWindow || (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
      || (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
      || (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
      || (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
      || (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
      || (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
      || (ee->request_code == X_CopyArea && ee->error_code == BadDrawable))
    return 0;

  if (ee->error_code == BadAccess)
    exit (1);

  // return xerrorxlib (dpy, ee);
  return 0;
}

static void xwm_keypress (XEvent *e) {
  KeySym keysym;
  XKeyEvent *ev = &e->xkey;

  keysym = XkbKeycodeToKeysym (Xwm->dpy, (KeyCode)ev->keycode, 0, 0);

  xwm_key_t *k;

  for (k = Xwm->keys; k; k = k->next)
    if (keysym == k->keysym && CLEANMASK(k->modifier) == (int) CLEANMASK(ev->state)) {
      char *kstr = XKeysymToString (k->keysym);
      if (NULL != Xwm->OnKeypress)
        Xwm->OnKeypress (Xwm, k->modifier, kstr);
      break;
   }
}

static void xwm_configurerequest (XEvent *e) {
  XConfigureRequestEvent *ev = &e->xconfigurerequest;
  XWindowChanges wc;

  wc.x = ev->x;
  wc.y = ev->y;
  wc.width = (ev->width < Xwm->sw - Xwm->bdw) ? ev->width : Xwm->sw + Xwm->bdw;
  wc.height = (ev->height < Xwm->sh - Xwm->bdw) ? ev->height : Xwm->sh + Xwm->bdw;
  wc.border_width = 0;
  wc.sibling = ev->above;
  wc.stack_mode = ev->detail;
  XConfigureWindow (Xwm->dpy, ev->window, ev->value_mask, &wc);
  XSync (Xwm->dpy, False);
}

static void xwm_maprequest (XEvent *e) {
  XMapRequestEvent *ev = &e->xmaprequest;

  XGetWindowAttributes (Xwm->dpy, ev->window, &Xwm->attr);

  if (Xwm->attr.override_redirect) return;

  xclient_t *c;

  for (c = Xwm->head; c; c = c->next)
    if (ev->window == c->win) {
      XMapWindow (Xwm->dpy, ev->window);
      return;
    }

  Window trans = None;

  if (XGetTransientForHint (Xwm->dpy, ev->window, &trans) && trans != None) {
    xwm_add_window (ev->window, 1, NULL);

    if ((Xwm->attr.y + Xwm->attr.height) > Xwm->sh)
      XMoveResizeWindow (Xwm->dpy, ev->window, Xwm->attr.x, 0, Xwm->attr.width , Xwm->attr.height - 10);

    XSetWindowBorderWidth (Xwm->dpy, ev->window, Xwm->bdw);
    XSetWindowBorder (Xwm->dpy, ev->window, Xwm->win_focus);
    XMapWindow (Xwm->dpy, ev->window);
    xwm_update_current ();
    return;
  }

  if (FULLSCREEN_MODE == Xwm->mode && Xwm->current != NULL)
    XUnmapWindow (Xwm->dpy, Xwm->current->win);

  int j = 0;
  int tmp = Xwm->current_desktop;

  xOnMap_t *o;
  XClassHint ch = {0};

  if (XGetClassHint (Xwm->dpy, ev->window, &ch))
    for (o = Xwm->onmap; o; o = o->next)
      if ((strcmp (ch.res_class, o->class) == 0) ||
          (strcmp (ch.res_name, o->class) == 0)) {
        xwm_save_desktop (tmp);
        xwm_select_desktop (o->desk - 1);

        for (c = Xwm->head; c; c = c->next)
          if (ev->window == c->win)
            ++j;

        if (j < 1)
          xwm_add_window (ev->window, 0, NULL);

        if (tmp == o->desk - 1) {
          xwm_tile ();
          XMapWindow (Xwm->dpy, ev->window);
          xwm_update_current ();
        } else
          xwm_select_desktop (tmp);

        if (o->follow && o->desk - 1 != Xwm->current_desktop) {
          int desk = o->desk - 1;
          xwm_change_desktop (desk);
        }

        if (ch.res_class)
          XFree (ch.res_class);

        if (ch.res_name)
          XFree (ch.res_name);

        return;
      }

  if (ch.res_class)
    XFree (ch.res_class);

  if (ch.res_name)
    XFree (ch.res_name);

  xwm_add_window (ev->window, 0, NULL);

  if (FULLSCREEN_MODE == Xwm->mode)
    xwm_tile ();
  else
    XMapWindow (Xwm->dpy, ev->window);

  xwm_update_current ();
}

static void xwm_destroynotify (XEvent *e) {
  int tmp = Xwm->current_desktop;
  xclient_t *c;
  XDestroyWindowEvent *ev = &e->xdestroywindow;

  xwm_save_desktop (tmp);

  for (int i = Xwm->current_desktop; i < Xwm->current_desktop + Xwm->desknum; ++i) {
    xwm_select_desktop (i % Xwm->desknum);

    for (c = Xwm->head; c; c = c->next)
      if (ev->window == c->win) {
        xwm_remove_window (ev->window, 0, 0);
        xwm_select_desktop (tmp);
        return;
      }

    if (Xwm->transient != NULL)
      for (c = Xwm->transient; c; c = c->next)
        if (ev->window == c->win) {
          xwm_remove_window (ev->window, 0, 1);
          xwm_select_desktop (tmp);
          return;
        }
  }

  xwm_select_desktop (tmp);
}

static void xwm_unmapnotify (XEvent *e) {
  XUnmapEvent *ev = &e->xunmap;
  xclient_t *c;

  if (ev->send_event == 1)
    for (c = Xwm->head; c; c = c->next)
      if (ev->window == c->win) {
        xwm_remove_window (ev->window, 1, 0);
        return;
      }
}

static unsigned long xwm_get_color (const char *color) {
  XColor c;
  Colormap map = DefaultColormap (Xwm->dpy, Xwm->screen);

  if (0 == XAllocNamedColor (Xwm->dpy, map, color, &c, &c))
    fprintf (stderr, "XWM: Error parsing color\n");

  return c.pixel;
}

static void xwm_sigchld_handler (int);
static void xwm_sigchld_handler (int sig) {
  if (signal (sig, xwm_sigchld_handler) == SIG_ERR) {
    fprintf (stderr, "Can't install SIGCHLD handler\n");
    exit (1);
  }

  while (0 < waitpid (-1, NULL, WNOHANG));
}

static void xwm_save_desktop (int i) {
  Xwm->desktops[i].numwins = Xwm->numwins;
  Xwm->desktops[i].mode = Xwm->mode;
  Xwm->desktops[i].growth = Xwm->growth;
  Xwm->desktops[i].head = Xwm->head;
  Xwm->desktops[i].current = Xwm->current;
  Xwm->desktops[i].transient = Xwm->transient;
}

static void xwm_add_window (Window w, int tw, xclient_t *cl) {
  xclient_t
    *c,
    *t,
    *dummy = Xwm->head;

  if (cl != NULL)
    c = cl;
  else
    c = Alloc (sizeof (xclient_t));

  if (tw == 0 && cl == NULL) {
    XClassHint ch = {0};
    int j = 0;
    xpositional_t *p;

    if (XGetClassHint (Xwm->dpy, w, &ch)) {
      for (p = Xwm->positional; p; p = p->next)
        if ((strcmp (ch.res_class, p->class) == 0) ||
            (strcmp (ch.res_name, p->class) == 0)) {
          XMoveResizeWindow (Xwm->dpy, w, p->x, p->y, p->width, p->height);
          ++j;
          break;
        }

      if (ch.res_class)
        XFree (ch.res_class);

      if (ch.res_name)
        XFree (ch.res_name);
    }

    if (j < 1) {
      XGetWindowAttributes (Xwm->dpy, w, &Xwm->attr);
      XMoveResizeWindow (Xwm->dpy, w, Xwm->attr.x, Xwm->attr.y,
          Xwm->attr.width + 80, Xwm->attr.height - 12);
      // XMoveWindow (Xwm->dpy, w, Xwm->sw / 2 - (Xwm->attr.width / 2), Xwm->sh / 2 - (Xwm->attr.height / 2));
    }

    XGetWindowAttributes (Xwm->dpy, w, &Xwm->attr);
    c->x = Xwm->attr.x;
    c->y = Xwm->attr.y;
    c->width = Xwm->attr.width;
    c->height = Xwm->attr.height;
  }

  c->win = w;
  c->order = 0;

  if (tw == 1)
    dummy = Xwm->transient;

  for (t = dummy; t; t = t->next)
    ++t->order;

  if (dummy == NULL) {
    c->next = NULL;
    c->prev = NULL;
    dummy = c;
  } else {
    c->prev = NULL;
    c->next = dummy;
    c->next->prev = c;
    dummy = c;
  }

  if (tw == 1) {
    Xwm->transient = dummy;
    xwm_save_desktop (Xwm->current_desktop);
    return;
  } else
    Xwm->head = dummy;

  Xwm->current = c;
  Xwm->numwins += 1;
  Xwm->growth = (Xwm->growth > 0) ? Xwm->growth * (Xwm->numwins - 1) / Xwm->numwins : 0;
  xwm_save_desktop (Xwm->current_desktop);
}

static void xwm_update_current (void) {
  if (Xwm->head == NULL) return;

  int border = ((Xwm->head->next == NULL && Xwm->mode == FULLSCREEN_MODE) ||
     (Xwm->mode == FULLSCREEN_MODE)) ? 0 : Xwm->bdw;

  xclient_t *c, *d;
  for (c = Xwm->head; c->next; c = c->next);

  for (d = c; d; d = d->prev) {
    XSetWindowBorderWidth (Xwm->dpy, d->win, border);

    if (d != Xwm->current) {
      if (d->order < Xwm->current->order)
        ++d->order;

      XSetWindowBorder (Xwm->dpy, d->win, Xwm->win_unfocus);
    } else {
      XSetWindowBorder (Xwm->dpy, d->win, Xwm->win_focus);
      XSetInputFocus (Xwm->dpy, d->win, RevertToParent, CurrentTime);
      XRaiseWindow (Xwm->dpy, d->win);
    }
  }

  Xwm->current->order = 0;

  if (Xwm->transient != NULL) {
    for (c = Xwm->transient; c->next; c = c->next);

    for (d = c; d; d = d->prev)
      XRaiseWindow (Xwm->dpy, d->win);

    XSetInputFocus (Xwm->dpy, Xwm->transient->win, RevertToParent, CurrentTime);
  }

  XSync (Xwm->dpy, False);
}

static void xwm_remove_window (Window w, int dr, int tw) {
  xclient_t
    *c,
    *t,
    *dummy;

  dummy = (tw == 1) ? Xwm->transient : Xwm->head;

  for (c = dummy; c; c = c->next) {
    if (c->win == w) {
      if (c->prev == NULL && c->next == NULL)
        dummy = NULL;
      else if (c->prev == NULL)  {
        dummy = c->next;
        c->next->prev = NULL;
      } else if (c->next == NULL)
        c->prev->next = NULL;
      else {
        c->prev->next = c->next;
        c->next->prev = c->prev;
      }

      break;
    }
  }

  if (tw == 1) {
    Xwm->transient = dummy;
    Release (c);
    xwm_save_desktop (Xwm->current_desktop);
    xwm_update_current ();
    return;
  } else {
    Xwm->head = dummy;
    XUngrabButton (Xwm->dpy, AnyButton, AnyModifier, c->win);
    XUnmapWindow (Xwm->dpy, c->win);

    Xwm->numwins--;

    if (Xwm->head != NULL)  {
      for (t = Xwm->head; t; t = t->next) {
        if (t->order > c->order)
          --t->order;

        if (t->order == 0)
          Xwm->current = t;
      }
    } else
      Xwm->current = NULL;

    if (dr == 0)
      Release (c);

    if (Xwm->numwins < 3)
      Xwm->growth = 0;

    xwm_save_desktop (Xwm->current_desktop);

    if (FULLSCREEN_MODE == Xwm->mode)
      xwm_tile ();

    xwm_update_current ();
    return;
  }
}

static void xwm_tile (void) {
  if (Xwm->head == NULL) return;

  xclient_t *c;

  if (FULLSCREEN_MODE == Xwm->mode  && Xwm->head != NULL &&  Xwm->head->next == NULL)  {
    XMapWindow (Xwm->dpy, Xwm->current->win);
    XMoveResizeWindow (Xwm->dpy, Xwm->head->win, 0, 0, Xwm->sw + Xwm->bdw, Xwm->sh + Xwm->bdw);
  } else  {
    switch (Xwm->mode) {
      case FULLSCREEN_MODE:
        XMoveResizeWindow (Xwm->dpy, Xwm->current->win, 0, 0, Xwm->sw + Xwm->bdw, Xwm->sh + Xwm->bdw);
        XMapWindow (Xwm->dpy, Xwm->current->win);
        break;

      case STACK_MODE:
        for (c = Xwm->head; c; c = c->next)
          XMoveResizeWindow (Xwm->dpy, c->win, c->x, c->y, c->width, c->height);
        break;
    }
  }
}

static void xwm_kill_client_now (Window w) {
  int n, i;
  XEvent ev;

  if (XGetWMProtocols (Xwm->dpy, w, &Xwm->protocols, &n) != 0) {
    for (i = n; i >= 0; --i)
      if (Xwm->protocols[i] == Xwm->xwm_delete_window) {
        ev.type = ClientMessage;
        ev.xclient.window = w;
        ev.xclient.message_type = Xwm->protos;
        ev.xclient.format = 32;
        ev.xclient.data.l[0] = Xwm->xwm_delete_window;
        ev.xclient.data.l[1] = CurrentTime;
        XSendEvent (Xwm->dpy, w, False, NoEventMask, &ev);
      }
  } else
    XKillClient (Xwm->dpy, w);

  XFree (Xwm->protocols);
}

static void xwm_select_desktop (int i) {
  Xwm->numwins = Xwm->desktops[i].numwins;
  Xwm->mode = Xwm->desktops[i].mode;
  Xwm->growth = Xwm->desktops[i].growth;
  Xwm->head = Xwm->desktops[i].head;
  Xwm->current = Xwm->desktops[i].current;
  Xwm->transient = Xwm->desktops[i].transient;
  Xwm->current_desktop = i;
}

static void xwm_grabkeys (void) {
  Xwm->numlockmask = 0;
  XModifierKeymap *modmap = XGetModifierMapping (Xwm->dpy);

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < modmap->max_keypermod; ++j) {
      if (modmap->modifiermap[i * modmap->max_keypermod + j] == XKeysymToKeycode (Xwm->dpy, XK_Num_Lock))
        Xwm->numlockmask = (1 << i);
    }
  }

  XFreeModifiermap (modmap);

  XUngrabKey (Xwm->dpy, AnyKey, AnyModifier, Xwm->root);

  KeyCode code;
  xwm_key_t *k;

  for (k = Xwm->keys; k; k = k->next) {
    code = XKeysymToKeycode (Xwm->dpy, k->keysym);
    XGrabKey (Xwm->dpy, code, k->modifier, Xwm->root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey (Xwm->dpy, code, k->modifier | LockMask, Xwm->root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey (Xwm->dpy, code, k->modifier | Xwm->numlockmask, Xwm->root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey (Xwm->dpy, code, k->modifier | Xwm->numlockmask | LockMask, Xwm->root, True, GrabModeAsync, GrabModeAsync);
  }
}

static void xwm_event_loop (void) {
  XEvent ev;

  while (0 == Xwm->quit && 0 == XNextEvent (Xwm->dpy, &ev))
    if (events[ev.type])
      events[ev.type](&ev);
}

static void xwm_release_font (void) {
  if (NULL == Xwm->font) return;
  Release (Xwm->font);
}

static void xwm_release_keys (void) {
  if (NULL == Xwm->keys) return;
  xwm_key_t *s = Xwm->keys;
  while (s) {
    xwm_key_t *next = s->next;
    Release (s);
    s = next;
  }
}

static void xwm_release_onmap (void) {
  if (NULL == Xwm->onmap) return;

  xOnMap_t *s = Xwm->onmap;
  while (s) {
    xOnMap_t *next = s->next;
    Release (s->class);
    Release (s);
    s = next;
  }
}

static void xwm_release_positional (void) {
  if (NULL == Xwm->positional) return;
  xpositional_t *s = Xwm->positional;
  while (s) {
    xpositional_t *next = s->next;
    Release (s->class);
    Release (s);
    s = next;
  }
}

static void xwm_release_desktops (void) {
  if (NULL == Xwm->desktops) return;
  for (int i = 0; i < Xwm->desknum; i++) {
    xclient_t *c = Xwm->desktops[i].head;
    while (c) {
      xclient_t *next = c->next;
      Release (c);
      c = next;
    }
  }

  Release (Xwm->desktops);
  Xwm->desktops = NULL;
}

static void xwm_set_desktops (int num) {
  xwm_release_desktops ();

  Xwm->desknum = num + 1;
  Xwm->desktops = Alloc (sizeof (xdesktop_t) * Xwm->desknum);

  for (int i = 0; i < Xwm->desknum; i++) {
    Xwm->desktops[i].growth = 0;
    Xwm->desktops[i].numwins = 0;
    Xwm->desktops[i].head = NULL;
    Xwm->desktops[i].current = NULL;
    Xwm->desktops[i].transient = NULL;
    Xwm->desktops[i].mode = DEFAULT_MODE;
  }
}

void xwm_next_win (void) {
  if (Xwm->numwins < 2) return;

  Xwm->current = (Xwm->current->next == NULL) ? Xwm->head : Xwm->current->next;

  if (FULLSCREEN_MODE == Xwm->mode)
    xwm_tile ();

  xwm_update_current ();
}

void xwm_prev_win (void) {
  if (Xwm->numwins < 2) return;

  xclient_t *c;

  if (Xwm->current->prev == NULL)
    for (c = Xwm->head; c->next; c = c->next);
  else
    c = Xwm->current->prev;

  Xwm->current = c;

  if (FULLSCREEN_MODE == Xwm->mode)
    xwm_tile ();

  xwm_update_current ();
}

void xwm_resize_stack (int inc) {
  if (Xwm->mode == STACK_MODE && Xwm->current != NULL) {
    Xwm->current->height += inc;
    XMoveResizeWindow (Xwm->dpy, Xwm->current->win, Xwm->current->x, Xwm->current->y,
        Xwm->current->width, Xwm->current->height + inc);
  }
}

void xwm_move_stack (int inc) {
  if (Xwm->mode == STACK_MODE && Xwm->current != NULL) {
    Xwm->current->y += inc;
    XMoveResizeWindow (Xwm->dpy, Xwm->current->win, Xwm->current->x, Xwm->current->y, Xwm->current->width, Xwm->current->height);
  }
}

void xwm_move_side_way (int inc) {
  if (Xwm->mode == STACK_MODE && Xwm->current != NULL) {
    Xwm->current->x += inc;
    XMoveResizeWindow (Xwm->dpy, Xwm->current->win, Xwm->current->x, Xwm->current->y, Xwm->current->width, Xwm->current->height);
  }
}

void xwm_spawn (const char **command) {
  if (fork () == 0) {
    if (fork () == 0) {
      if (Xwm->dpy)
        close (ConnectionNumber (Xwm->dpy));

      setsid ();

      execvp (command[0], (char *const *) command);
    }

    exit (0);
  }
}

void xwm_resize_side_way (int inc) {
  if (Xwm->mode == STACK_MODE && Xwm->current != NULL) {
    Xwm->current->width += inc;
    XMoveResizeWindow (Xwm->dpy, Xwm->current->win, Xwm->current->x, Xwm->current->y,
        Xwm->current->width + inc, Xwm->current->height);
  }
}

void xwm_kill_client (void) {
  if (Xwm->head == NULL) return;

  xwm_kill_client_now (Xwm->current->win);
  xwm_remove_window (Xwm->current->win, 0, 0);
}

void xwm_change_desktop (int desk) {
  if (desk == Xwm->current_desktop) return;
  int tmp = Xwm->current_desktop;

  xwm_save_desktop (Xwm->current_desktop);
  Xwm->previous_desktop = Xwm->current_desktop;

  xwm_select_desktop (desk);

  xclient_t *c;
  if (Xwm->head != NULL) {
    if (STACK_MODE == Xwm->mode)
      for (c = Xwm->head; c; c = c->next)
        XMapWindow (Xwm->dpy, c->win);

    xwm_tile ();
  }

  if (Xwm->transient != NULL)
    for (c = Xwm->transient; c; c = c->next)
      XMapWindow (Xwm->dpy, c->win);

  xwm_select_desktop (tmp);

  if (Xwm->transient != NULL)
    for (c = Xwm->transient; c; c = c->next)
       XUnmapWindow (Xwm->dpy, c->win);

  if (Xwm->head != NULL)
    for (c = Xwm->head; c; c = c->next)
      XUnmapWindow (Xwm->dpy, c->win);

  xwm_select_desktop (desk);
  xwm_update_current ();
}

void xwm_quit (void) {
  xclient_t *c;

  for (int i = 0; i < Xwm->desknum; ++i) {
    if (Xwm->desktops[i].head != NULL)
      xwm_select_desktop (i);
    else
      continue;

    for (c = Xwm->head; c; c = c->next)
      xwm_kill_client_now (c->win);
  }

  XClearWindow (Xwm->dpy, Xwm->root);
  XUngrabKey (Xwm->dpy, AnyKey, AnyModifier, Xwm->root);
  XSync (Xwm->dpy, False);
  XSetInputFocus (Xwm->dpy, Xwm->root, RevertToPointerRoot, CurrentTime);
  Xwm->quit = 1;
}

int xwm_get_desknum (void) {
  return Xwm->desknum - 1;
}

int xwm_get_previous_desktop (void) {
  return Xwm->previous_desktop;
}

int xwm_get_current_desktop (void) {
  return Xwm->current_desktop;
}

char **xwm_get_desk_class_names (int desk) {
  if (desk < 0 || desk >= Xwm->desknum - 1) return NULL;

  if (0 == Xwm->desktops[desk].numwins)
    return NULL;

  int num_wins = Xwm->desktops[desk].numwins;

  char **classes = Alloc ((num_wins * sizeof (char *)) + 1);

  xclient_t *c;
  XClassHint hint;

  int idx = 0;
  for (c = Xwm->desktops[desk].head; c; c = c->next) {
    if (XGetClassHint (Xwm->dpy, c->win, &hint)) {
      if (hint.res_name)
        //classes[idx] = strdup (hint.res_name);
        // will be free'd by the caller,
        // as this way, we avoid a reallocation
        classes[idx++] = hint.res_name;

      if (hint.res_class)
        XFree (hint.res_class);
    }
  }

  classes[idx] = NULL;
  return classes;
}

void xwm_set_key (const char *keysym, int modifier) {
  xwm_key_t *s = Alloc (sizeof (xwm_key_t));
  s->keysym = XStringToKeysym (keysym);
  s->modifier = modifier;
  s->next = Xwm->keys;
  Xwm->keys = s;
}

void xwm_set_on_startup_cb (OnStartup_cb cb) {
  Xwm->OnStartup = cb;
}

void xwm_set_on_keypress_cb (OnKeypress_cb cb) {
  Xwm->OnKeypress = cb;
}

void xwm_set_onmap (const char *class, int desk, int follow) {
  xOnMap_t *s = Alloc (sizeof (xOnMap_t));
  s->class = strdup (class);
  s->desk = desk;
  s->follow = follow;
  s->next = Xwm->onmap;
  Xwm->onmap = s;
}

void xwm_set_user_data (void *data) {
  Xwm->user_data = data;
}

void xwm_set_mode (int desk, int mode) {
  desk--;
  if (desk < 0 || desk >= Xwm->desknum - 1) return;
  Xwm->desktops[desk].mode = mode;
}

int xwm_startx (void) {
  xwm_sigchld_handler (SIGCHLD);

  if (NULL == (Xwm->dpy = XOpenDisplay (NULL))) {
    fprintf (stderr, "Cannot open display\n");
    return -1;
  }

  Xwm->screen = DefaultScreen (Xwm->dpy);
  Xwm->root = RootWindow (Xwm->dpy, Xwm->screen);
  Xwm->bdw = BORDER_WIDTH;
  Xwm->sw = XDisplayWidth (Xwm->dpy, Xwm->screen) - Xwm->bdw;
  Xwm->sh = XDisplayHeight (Xwm->dpy, Xwm->screen) - Xwm->bdw;
  Xwm->win_focus = xwm_get_color (FOCUS);
  Xwm->win_unfocus = xwm_get_color (UNFOCUS);

  if (NULL == setlocale (LC_ALL, ""))
    fprintf (stderr, "failed to set locale\n");

  xwm_grabkeys ();
  xwm_select_desktop (0);

  Xwm->xwm_delete_window = XInternAtom (Xwm->dpy, "WM_DELETE_WINDOW", False);
  Xwm->protos = XInternAtom (Xwm->dpy, "WM_PROTOCOLS", False);
  XSelectInput (Xwm->dpy, Xwm->root, SubstructureNotifyMask|SubstructureRedirectMask);

  Xwm->quit = 0;

  if (NULL != Xwm->OnStartup)
    Xwm->OnStartup (Xwm);

  xwm_event_loop ();

  XCloseDisplay (Xwm->dpy);

  return 0;
}

xwm_t *xwm_init (int desknum) {
  Xwm = Alloc (sizeof (xwm_t));
  if (NULL == Xwm) return NULL;

  Xwm->dpy = NULL;
  Xwm->onmap = NULL;
  Xwm->keys = NULL;
  Xwm->desktops = NULL;
  Xwm->positional = NULL;
  Xwm->OnKeypress = NULL;
  Xwm->user_data = NULL;

  XSetErrorHandler (xerror);

  xwm_set_desktops (desknum);

  return Xwm;
}

void xwm_deinit (void) {
  if (NULL == Xwm) return;

  xwm_release_font ();
  xwm_release_keys ();
  xwm_release_onmap ();
  xwm_release_desktops ();
  xwm_release_positional ();
  Release (Xwm);
  Xwm = NULL;
}
