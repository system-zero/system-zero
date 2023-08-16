#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>

#include <xwm.h>

#define DESKNUM       19

#ifndef TERM
#define TERM          "xterm"
#endif

// ModMask + 
#define ROOT_IDX      0
#define SRC_IDX       1
#define Y_IDX         2
#define L_IDX         3
#define J_IDX         4
#define D_IDX         5
#define X_IDX         6
#define LIBC_IDX      7
#define MAKE_IDX      8
#define UNUSED_IDX    9
// ModMask + f1...f9
#define Z_MOD_IDX     10
#define Z_LIB_IDX     11
#define Z_APP_IDX     12
#define SYS_IDX       13
#define SYSTMP_IDX    14
#define MAIL_IDX      15
#define WEB_IDX       16
#define UNUSED_FN_IDX 17
#define STACKDESK_IDX 18

#define HELP                                       \
  "XWM KEYS\n"                                     \
  "-------------------------------------------\n"  \
  "WinKey-c           urxvt && shell\n"            \
  "WinKey-tab         next win\n"                  \
  "WinKey-q           prev win\n"                  \
  "WinKey-`           prev focused desktop\n"      \
  "WinKey-left        prev desktop\n"              \
  "WinKey-right       next desktop\n"              \
  "WinKey-[0-9]       change to desktop\n"         \
  "WinKey-F[1-9]      change to desktop\n"         \
  "\n"                                             \
  "WinKey-shift-q     quit\n"                      \
  "WinKey-shift-k     kill client\n"               \
  "\n"                                             \
  "WinKey-Alt-Right   resize to the right  12%%\n" \
  "WinKey-Alt-Left    resize to the left   12%%\n" \
  "WinKey-Alt-Up      resize to the top    12%%\n" \
  "WinKey-Alt-Down    resize to the bottom 12%%\n" \
  "\n"                                             \
  "Winkey-CTRL-Right  move to the right    15%%\n" \
  "Winkey-CTRL-Left   move to the left     15%%\n" \
  "Winkey-CTRL-Up     move to the top      15%%\n" \
  "Winkey-CTRL-Down   move to the bottom   15%%\n"

#define Release      free
#define STR_EQ       0 == strcmp
//#define FOLLOW       1

static void on_change_desktop (xwm_t *this, int desk_idx) {
  (void) this;
  char **classnames = NULL;
  classnames = xwm_get_desk_class_names (desk_idx);

  int idx = 0;

  switch (desk_idx) {
    case ROOT_IDX:
      if (NULL == classnames) {
        const char *argv[] = {TERM, "-name", "ROOT", "-cd", HOMEDIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }
      goto theend;

    case SRC_IDX: {
      int has_src = 0;
      int has_repo = 0;

      if (NULL != classnames) {
        while (classnames[idx]) {
          if (STR_EQ (classnames[idx], "SRCDIR"))
            has_src = 1;
          else if (STR_EQ (classnames[idx], "REPODIR"))
            has_repo = 1;
          idx++;
        }
      }

      if (0 == has_src) {
        const char *argv[] = {TERM, "-name", "SRCDIR", "-cd", SRCDIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }

      if (0 == has_repo) {
        const char *argv[] = {TERM, "-name", "REPODIR", "-cd", REPODIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }
    }
      goto theend;

    case MAKE_IDX:
    case Y_IDX: {
      int num = 0;
      if (NULL != classnames) {
        while (classnames[idx])
          if (STR_EQ (classnames[idx++], "YSRCPKGDIR")) {
          num++;
          if (num == 2) goto theend;
        }
      }

      const char *argv[] = {TERM, "-name", "YSRCPKGDIR", "-cd", YSRCPKGDIR, "-e", SHELL, NULL};
      xwm_spawn (argv);
      if (++num < 2)
        xwm_spawn (argv);
    }
      goto theend;

    case L_IDX: {
      int num = 0;
      if (NULL != classnames) {
        while (classnames[idx])
          if (STR_EQ (classnames[idx++], "LSRCPKGDIR")) {
          num++;
          if (num == 2) goto theend;
        }
      }

      const char *argv[] = {TERM, "-name", "LSRCPKGDIR", "-cd", LSRCPKGDIR, "-e", SHELL, NULL};
      xwm_spawn (argv);
      if (++num < 2)
        xwm_spawn (argv);
    }
      goto theend;

    case J_IDX: {
      int num = 0;
      if (NULL != classnames) {
        while (classnames[idx])
          if (STR_EQ (classnames[idx++], "JSRCPKGDIR")) {
          num++;
          if (num == 2) goto theend;
        }
      }

      const char *argv[] = {TERM, "-name", "JSRCPKGDIR", "-cd", JSRCPKGDIR, "-e", SHELL, NULL};
      xwm_spawn (argv);
      if (++num < 2)
        xwm_spawn (argv);
    }
      goto theend;

    case D_IDX : {
      int has_d = 0;
      int has_interp = 0;
      if (NULL != classnames) {
        while (classnames[idx]) {
          if (STR_EQ (classnames[idx], "DSRCPKGDIR"))
            has_d = 1;

          if (STR_EQ (classnames[idx], "INTERPRETER"))
            has_interp = 1;

          idx++;
        }
      }

      if (0 == has_interp) {
        const char *argv[] = {TERM, "-name", "INTERPRETER", "-e", INTERPRETER, NULL};
        xwm_spawn (argv);
      }

      if (0 == has_d) {
        const char *argv[] = {TERM, "-name", "DSRCPKGDIR", "-cd", DSRCPKGDIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }
    }

      goto theend;


    case X_IDX: {
      int num = 0;
      if (NULL != classnames) {
        while (classnames[idx])
          if (STR_EQ (classnames[idx++], "XSRCPKGDIR")) {
          num++;
          if (num == 2) goto theend;
        }
      }

      const char *argv[] = {TERM, "-name", "XSRCPKGDIR", "-cd", XSRCPKGDIR, "-e", SHELL, NULL};
      xwm_spawn (argv);
      if (++num < 2)
        xwm_spawn (argv);
    }
      goto theend;

    case LIBC_IDX: {
      int num = 0;
      if (NULL != classnames) {
        while (classnames[idx])
          if (STR_EQ (classnames[idx++], "LIBCSRCDIR")) {
          num++;
          if (num == 2) goto theend;
        }
      }

      const char *argv[] = {TERM, "-name", "LIBCSRCDIR", "-cd", LIBCSRCDIR, "-e", SHELL, NULL};
      xwm_spawn (argv);
      if (++num < 2)
        xwm_spawn (argv);
    }
      goto theend;

    case Z_MOD_IDX:
      if (NULL != classnames)
        while (classnames[idx])
          if (STR_EQ (classnames[idx++], "ZSRCMODDIR")) goto theend;

      {
        const char *argv[] = {TERM, "-name", "ZSRCMODDIR", "-cd", ZSRCMODDIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }

      goto theend;

    case Z_APP_IDX:
      if (NULL != classnames)
        while (classnames[idx])
          if (STR_EQ (classnames[idx++], "ZSRCAPPDIR")) goto theend;
      {
        const char *argv[] = {TERM, "-name", "ZSRCAPPDIR", "-cd", ZSRCAPPDIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }

      goto theend;

    case Z_LIB_IDX:
      if (NULL != classnames)
        while (classnames[idx])
          if (STR_EQ (classnames[idx++], "ZSRCLIBDIR")) goto theend;

      {
        const char *argv[] = {TERM, "-name", "ZSRCLIBDIR", "-cd", ZSRCLIBDIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }

      goto theend;

    case MAIL_IDX:
    case SYS_IDX:
    case WEB_IDX:
    case UNUSED_IDX:
    case UNUSED_FN_IDX:
      if (NULL == classnames) {
        const char *argv[] = {TERM, "-cd", HOMEDIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }

      goto theend;

    case SYSTMP_IDX:
      int num = 2;
      if (NULL != classnames) {
        while (classnames[idx]) {
          if (STR_EQ (classnames[idx], "SYSTMPDIR"))
            num--;
          idx++;
        }
      }

      while (num-- > 0) {
        const char *argv[] = {TERM, "-name", "SYSTMPDIR", "-cd", SYSTMPDIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }

      goto theend;

    case STACKDESK_IDX:
      if (NULL == classnames) {
        const char *argv[] = {TERM, "-cd", HOMEDIR, "-e", SHELL, NULL};
        xwm_spawn (argv);
      }

      goto theend;

  }

theend:
  if (NULL == classnames) return;

  idx = 0;
  while (classnames[idx])
    XFree (classnames[idx++]);

  Release (classnames);
}

static void on_keypress (xwm_t *this, int modifier, char *key) {
  int
    desk_idx,
    cur,
    num;

  switch (modifier) {
    case WIN_KEY:
      if (STR_EQ ("c", key)) {
        const char *argv[] = {TERM, "-e", SHELL, NULL};
        xwm_spawn (argv);
      } else if (STR_EQ ("Tab", key)) {
        xwm_next_win ();
      } else if (STR_EQ ("q", key)) {
        xwm_prev_win ();
      } else if (STR_EQ ("grave", key)) {
        int prev = xwm_get_previous_desktop ();
        xwm_change_desktop (prev);
      } else if (STR_EQ ("Left", key)) {
        cur = xwm_get_current_desktop ();
        num = xwm_get_desknum ();
        desk_idx = (cur + num - 1) % num;
        xwm_change_desktop (desk_idx);
      } else if (STR_EQ ("Right", key)) {
        cur = xwm_get_current_desktop ();
        num = xwm_get_desknum ();
        desk_idx = (cur + num + 1) % num;
        xwm_change_desktop (desk_idx);
      } else if ('0' <= key[0] && key[0] <= '9') {
        desk_idx = key[0] - '0';
        xwm_change_desktop (desk_idx);
        on_change_desktop (this, desk_idx);
      } else if ('F' == key[0] && ('1' <= key[1] && key[1] <= '9')) {
        desk_idx = key[1] - '0' + 9;
        xwm_change_desktop (desk_idx);
        on_change_desktop (this, desk_idx);
      }

      break;

    case WIN_SHIFT_KEY:
      if (STR_EQ ("q", key))
        xwm_quit ();
      else if (STR_EQ ("k", key))
        xwm_kill_client ();

      break;

    case ALT_KEY:
      if (STR_EQ ("F1", key)) {
        FILE *fp = fopen ("/tmp/xwm_help", "w");
        fprintf (fp, HELP);
        fclose (fp);
        const char *argv[] = {TERM, "-e", PAGER, "/tmp/xwm_help", NULL};
        xwm_spawn (argv);
      }

      break;

    case WIN_ALT_KEY:
      if (STR_EQ ("Right", key))
        xwm_resize_side_way (12);
      else if (STR_EQ ("Left", key))
        xwm_resize_side_way (-12);
      else if (STR_EQ ("Up", key))
        xwm_resize_stack (-12);
      else if (STR_EQ ("Down", key))
        xwm_resize_stack (12);

      break;

    case WIN_CONTROL_KEY:
      if (STR_EQ ("Right", key))
        xwm_move_side_way (15);
      else if (STR_EQ ("Left", key))
        xwm_move_side_way (-15);
      else if (STR_EQ ("Up", key))
        xwm_move_stack (-15);
      else if (STR_EQ ("Down", key))
        xwm_move_stack (15);
  }
}

static void on_startup (xwm_t *this) {
  (void) this;
  const char *argv[] = {TERM, "-e", SHELL, NULL};
  xwm_spawn (argv);
}

int main (int argc, char **argv) {
  (void) argc; (void) argv;

  xwm_init (DESKNUM);

  for (int i = 0; i < 10; i++) {
    char n[2]; n[0] = i + '0'; n[1] = '\0';
    xwm_set_key (n, Mod4Mask);
  }

  for (int i = 1; i < 9; i++) {
    char n[3]; n[0] = 'F'; n[1] = i + '0'; n[2] = '\0';
    xwm_set_key (n, Mod4Mask);
  }

  xwm_set_key ("c",      Mod4Mask);
  xwm_set_key ("z",      Mod4Mask);
  xwm_set_key ("q",      Mod4Mask);
  xwm_set_key ("Tab",    Mod4Mask);
  xwm_set_key ("Left",   Mod4Mask);
  xwm_set_key ("Right",  Mod4Mask);
  xwm_set_key ("grave",  Mod4Mask);
  xwm_set_key ("Return", Mod4Mask);
  xwm_set_key ("F9",     Mod4Mask);
  xwm_set_key ("x",      Mod4Mask);
  xwm_set_key ("v",      Mod4Mask);
  xwm_set_key ("l",      Mod4Mask);
  xwm_set_key ("e",      Mod4Mask);
  xwm_set_key ("a",      Mod4Mask);
  xwm_set_key ("s",      Mod4Mask);

  xwm_set_key ("q",      Mod4Mask|ShiftMask);
  xwm_set_key ("k",      Mod4Mask|ShiftMask);

  xwm_set_key ("v",      Mod1Mask);
  xwm_set_key ("F5",     Mod1Mask);
  xwm_set_key ("F3",     Mod1Mask);
  xwm_set_key ("F4",     Mod1Mask);
  xwm_set_key ("F2",     Mod1Mask);
  xwm_set_key ("F1",     Mod1Mask);

  xwm_set_key ("Right",  Mod4Mask|Mod1Mask);
  xwm_set_key ("Left",   Mod4Mask|Mod1Mask);
  xwm_set_key ("Up",     Mod4Mask|Mod1Mask);
  xwm_set_key ("Down",   Mod4Mask|Mod1Mask);
  xwm_set_key ("m",      Mod4Mask|Mod1Mask);
  xwm_set_key ("l",      Mod4Mask|Mod1Mask);

  xwm_set_key ("Right",  Mod4Mask|ControlMask);
  xwm_set_key ("Left",   Mod4Mask|ControlMask);
  xwm_set_key ("Up",     Mod4Mask|ControlMask);
  xwm_set_key ("Down",   Mod4Mask|ControlMask);

  xwm_set_mode (19, STACK_MODE);

  xwm_set_on_startup_cb (on_startup);
  xwm_set_on_keypress_cb (on_keypress);

  xwm_startx ();

  xwm_deinit ();

  return 0;
}
