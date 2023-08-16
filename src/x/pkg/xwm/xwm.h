#ifndef XWM_H
#define XWM_H

#define WIN_KEY         (Mod4Mask)
#define ALT_KEY         (Mod1Mask)
#define WIN_SHIFT_KEY   (Mod4Mask|ShiftMask)
#define WIN_ALT_KEY     (Mod4Mask|Mod1Mask)
#define WIN_CONTROL_KEY (Mod4Mask|ControlMask)

#define FULLSCREEN_MODE 0
#define STACK_MODE      1
#define DEFAULT_MODE FULLSCREEN_MODE

#define BORDER_WIDTH    1

#define FOCUS   "#664422" /* dkorange */
#define UNFOCUS "#004050" /* blueish */

#define colorblack  "#000000"
#define colorwhite  "#ffffff"
#define colorcyan   "#001020"
#define color2      "#002030"
#define color3      "#665522"
#define color4      "#898900"
#define color5      "#776644"
#define color6      "#887733"
#define color7      "#998866"
#define color8      "#999999"
#define color9      "#000055"  // &9

typedef struct xwm_t xwm_t;
typedef struct xOnMap_t xOnMap_t;
typedef struct xpositional_t xpositional_t;
typedef struct xwm_key_t xwm_key_t;
typedef struct xclient_t xclient_t;
typedef struct xdesktop_t xdesktop_t;
typedef struct xfont_t xfont_t;

typedef void (*OnKeypress_cb) (xwm_t *, int, char *);
typedef void (*OnStartup_cb) (xwm_t *);
typedef int  (*Input_cb) (xwm_t *, char *, int, int, KeySym);

struct xwm_t {
  Window root;
  Display *dpy;

  xdesktop_t *desktops;
  xwm_key_t  *keys;
  xOnMap_t   *onmap;
  xpositional_t *positional;

  xclient_t
    *head,
    *current,
    *transient;

  Atom
    *protocols,
    xwm_delete_window,
    protos;

  int
    sh,
    sw,
    bdw,
    quit,
    mode,
    screen,
    growth,
    desknum,
    numwins,
    win_focus,
    win_unfocus,
    numlockmask,
    current_desktop,
    previous_desktop;

  XWindowAttributes attr;

  xfont_t *font;

  OnStartup_cb OnStartup;
  OnKeypress_cb OnKeypress;

  void *user_data;
};

xwm_t *xwm_init (int);
void  xwm_deinit (void);

int xwm_startx (void);
int xwm_get_desknum (void);
int xwm_get_previous_desktop (void);
int xwm_get_current_desktop (void);

char **xwm_get_desk_class_names (int);

void xwm_spawn (const char **);
void xwm_next_win (void);
void xwm_prev_win (void);
void xwm_change_desktop (int);
void xwm_quit (void);
void xwm_kill_client (void);
void xwm_resize_side_way (int);
void xwm_move_side_way (int);
void xwm_resize_stack (int);
void xwm_move_stack (int);
void xwm_set_key (const char *, int);
void xwm_set_on_startup_cb (OnStartup_cb);
void xwm_set_on_keypress_cb (OnKeypress_cb);
void xwm_set_onmap (const char *, int, int);
void xwm_set_user_data (void *);
void xwm_set_mode (int, int);

#endif /* XWM_H */
