#ifdef OK
#undef OK
#endif

#ifdef NOTOK
#undef NOTOK
#endif

#ifdef ifnot
#undef ifnot
#endif

#ifdef is
#undef is
#endif

#ifdef isnot
#undef isnot
#endif

#ifdef or
#undef or
#endif

#ifdef and
#undef and
#endif

#define OK     0
#define NOTOK -1
#define is    ==
#define isnot !=
#define or    ||
#define and   &&
#define ifnot(__expr__) if (0 == (__expr__))
#ifdef private
#undef private
#endif

#ifdef public
#undef public
#endif

#ifdef UNUSED
#undef UNUSED
#endif

#define private __attribute__ ((visibility ("hidden")))
#define public  __attribute__ ((visibility ("default")))
#define UNUSED  __attribute__ ((unused))

#ifdef REQUIRE_Z_ENV

#ifndef DECLARE
#define DECLARE 1
#endif

#ifndef DONOT_DECLARE
#define DONOT_DECLARE 1
#endif

/* modules */
#ifndef EvalString
#define EvalString(...) #__VA_ARGS__
#endif

#ifndef WITHOUT_ERRNO
//extern int *__errno_location (void);
//#define errno (*__errno_location ())
int errno;
#endif

/* we malfunction a bit on cases, but its ok for the purpose and for just a bit of time */
#ifndef WITHOUT_FILE
typedef struct _FILE FILE;
#endif

#ifdef REQUIRE_STD_MODULE
  #ifndef REQUIRE_VMAP_TYPE
    #define REQUIRE_VMAP_TYPE    DECLARE
  #endif

  #ifndef REQUIRE_STRING_TYPE
    #define REQUIRE_STRING_TYPE  DECLARE
  #endif

  #ifndef REQUIRE_VSTRING_TYPE
    #define REQUIRE_VSTRING_TYPE DECLARE
  #endif

  #ifndef REQUIRE_LA_TYPE
    #define REQUIRE_LA_TYPE      DECLARE
  #endif

#undef REQUIRE_STD_MODULE
#endif

   /* forward declarations for -Wmissing declarations */
#define MODULE(_name_)                                      \
  public int __init_ ## _name_ ## _module__ (la_t *);       \
  public void __deinit_ ## _name_ ## _module__ (la_t *);

  /* this should be called inside of the __init_module__() */
#define __INIT_MODULE__(__l__)   \
do {                             \
  __INIT__(vmap);                \
  __INIT__(string);              \
  __INIT__(vstring);             \
  __LA__ = *la_get_root (__l__); \
} while (0)

  /* types */
#ifdef APPLICATION

  #ifndef APPLICATION_HDR
  #define APPLICATION_HDR

    #ifndef WITHOUT_STDARG
      #ifndef STDARG_HDR
      #define STDARG_HDR
      #include <stdarg.h>
      #endif
    #endif

    #ifndef WITHOUT_ARGPARSE
      #ifndef UNISTD_HDR
      #define UNISTD_HDR
      #include <unistd.h>
      #endif

      #ifndef WITHOUT_ARGPARSE_DECLARATION
        #define REQUIRE_ARGPARSE_TYPE DECLARE
      #else
        #define REQUIRE_ARGPARSE_TYPE DONOT_DECLARE
      #endif
    #endif

    #ifndef WITHOUT_IO
      #ifndef STDIO_HDR
      #define STDIO_HDR
      #include <stdio.h>
      #endif

      #ifndef WITHOUT_IO_DECLARATION
        #define REQUIRE_IO_TYPE DECLARE
      #else
        #define REQUIRE_IO_TYPE DONOT_DECLARE
      #endif
    #endif

  #endif /* APPLICATION_HDR */

  #ifndef WITHOUT_USAGE
    #ifdef APP_OPTS
    static const char *const usage[] = { \
      APPLICATION " " APP_OPTS,          \
    NULL,                                \
    };
    #endif
  #endif

#undef APPLICATION
#endif /* APPLICATION */


#ifdef REQUIRE_LIST_MACROS
  #ifndef LIST_MACROS_HDR
  #define LIST_MACROS_HDR
  #include <z/list.h>
  #endif /* LIST_MACROS_HDR */

#undef REQUIRE_LIST_MACROS
#endif /* REQUIRE_LIST_MACROS */

#ifdef REQUIRE_ERROR_TYPE
  #ifndef ERROR_TYPE_HDR
  #define ERROR_TYPE_HDR
  #include <z/error.h>
  #endif /* ERROR_TYPE_HDR */

  #if (REQUIRE_ERROR_TYPE == DECLARE)
  static  error_T errorType;
  #define Error   errorType.self
  #endif

#undef REQUIRE_ERROR_TYPE
#endif /* REQUIRE_ERROR_TYPE */

#ifdef REQUIRE_STRING_TYPE
  #ifndef STRING_TYPE_HDR
  #define STRING_TYPE_HDR
  #include <z/stringt.h>
  #endif /* STRING_TYPE_HDR */

  #if (REQUIRE_STRING_TYPE == DECLARE)
  static  string_T stringType;
  #define String   stringType.self
  #endif

  typedef string_t string;

#undef REQUIRE_STRING_TYPE
#endif /* REQUIRE_STRING_TYPE */

#ifdef REQUIRE_CSTRING_TYPE
  #ifndef CSTRING_TYPE_HDR
  #define CSTRING_TYPE_HDR
  #include <z/cstring.h>
  #endif /* CSTRING_TYPE_HDR */

  #if (REQUIRE_CSTRING_TYPE == DECLARE)
  static  cstring_T cstringType;
  #define Cstring   cstringType.self
  #endif

#undef REQUIRE_CSTRING_TYPE
#endif /* REQUIRE_CSTRING_TYPE */

#ifdef REQUIRE_VSTRING_TYPE
  #ifndef VSTRING_TYPE_HDR
  #define VSTRING_TYPE_HDR
  #include <z/vstring.h>
  #endif /* VSTRING_TYPE_HDR */

  #if (REQUIRE_VSTRING_TYPE == DECLARE)
  static  vstring_T vstringType;
  #define Vstring   vstringType.self
  #endif

#undef REQUIRE_VSTRING_TYPE
#endif /* REQUIRE_VSTRING_TYPE */

#ifdef REQUIRE_USTRING_TYPE
  #ifndef USTRING_TYPE_HDR
  #define USTRING_TYPE_HDR
  #include <z/ustring.h>
  #endif /* USTRING_TYPE_HDR */

  #if (REQUIRE_USTRING_TYPE == DECLARE)
  static  ustring_T ustringType;
  #define Ustring   ustringType.self
  #endif

#undef REQUIRE_USTRING_TYPE
#endif /* REQUIRE_USTRING_TYPE */

#ifdef REQUIRE_IO_TYPE
  #ifndef IO_TYPE_HDR
  #define IO_TYPE_HDR
  #include <z/io.h>
  #endif /* IO_TYPE_HDR */

  #if (REQUIRE_IO_TYPE == DECLARE)
  static  io_T   ioType;
  #define IO     ioType.self
  #define Stderr ioType.self.err
  #define Stdout ioType.self.out
  #define Input  ioType.self.input
  #define FD     ioType.self.fd

  #endif

#undef REQUIRE_IO_TYPE
#endif /* REQUIRE_IO_TYPE */

#ifdef REQUIRE_RLINE_TYPE
  #ifndef RLINE_TYPE_HDR
  #define RLINE_TYPE_HDR
  #include <z/rline.h>
  #endif /* RLINE_TYPE_HDR */

  #if (REQUIRE_RLINE_TYPE == DECLARE)
  static  rline_T rlineType;
  #define Rline   rlineType.self
  #endif

#undef REQUIRE_RLINE_TYPE
#endif /* REQUIRE_RLINE_TYPE */


#ifdef REQUIRE_JSON_TYPE
  #ifndef JSON_TYPE_HDR
  #define JSON_TYPE_HDR
  #include <z/json.h>
  #endif /* JSON_TYPE_HDR */

  #if (REQUIRE_JSON_TYPE == DECLARE)
  static  json_T jsonType;
  #define Json   jsonType.self
  #endif

#undef REQUIRE_JSON_TYPE
#endif /* REQUIRE_JSON_TYPE */

#ifdef REQUIRE_SH_TYPE
  #ifndef SH_TYPE_HDR
  #define SH_TYPE_HDR
  #include <z/sh.h>
  #endif /* SH_TYPE_HDR */

  #if (REQUIRE_SH_TYPE == DECLARE)
  static  sh_T shType;
  #define Sh   shType.self
  #endif

#undef REQUIRE_SH_TYPE
#endif /* REQUIRE_SH_TYPE */

#ifdef REQUIRE_OS_TYPE
  #ifndef OS_TYPE_HDR
  #define OS_TYPE_HDR
  #include <z/os.h>
  #endif /* OS_TYPE_HDR */

  #if (REQUIRE_OS_TYPE == DECLARE)
  static  os_T osType;
  #define OS   osType.self
  #endif

#undef REQUIRE_OS_TYPE
#endif /* REQUIRE_OS_TYPE */

#ifdef REQUIRE_FILE_TYPE
  #ifndef FILE_TYPE_HDR
  #define FILE_TYPE_HDR
  #include <z/file.h>
  #endif /* FILE_TYPE_HDR */

  #if (REQUIRE_FILE_TYPE == DECLARE)
  static  file_T fileType;
  #define File   fileType.self
  #endif

#undef REQUIRE_FILE_TYPE
#endif /* REQUIRE_FILE_TYPE */

#ifdef REQUIRE_PATH_TYPE
  #ifndef PATH_TYPE_HDR
  #define PATH_TYPE_HDR
  #include <z/path.h>
  #endif /* PATH_TYPE_HDR */

  #if (REQUIRE_PATH_TYPE == DECLARE)
  static  path_T pathType;
  #define Path   pathType.self
  #endif

#undef REQUIRE_PATH_TYPE
#endif /* REQUIRE_PATH_TYPE */

#ifdef REQUIRE_PROC_TYPE
  #ifndef PROC_TYPE_HDR
  #define PROC_TYPE_HDR
  #include <z/proc.h>
  #endif /* PROC_TYPE_HDR */

  #if (REQUIRE_PROC_TYPE == DECLARE)
  static  proc_T procType;
  #define Proc   procType.self
  #endif

#undef REQUIRE_PROC_TYPE
#endif /* REQUIRE_PROC_TYPE */

#ifdef REQUIRE_DIR_TYPE
  #ifndef DIR_TYPE_HDR
  #define DIR_TYPE_HDR

  #ifndef SYS_STAT_HDR
  #define SYS_STAT_HDR
  #include <sys/stat.h>
  #endif /* SYS_STAT_HDR */

  #include <z/dir.h>
  #endif /* DIR_TYPE_HDR */

  #if (REQUIRE_DIR_TYPE == DECLARE)
  static  dir_T dirType;
  #define Dir   dirType.self
  #endif

#undef REQUIRE_DIR_TYPE
#endif /* REQUIRE_DIR_TYPE */

#ifdef REQUIRE_TERM_TYPE
  #ifndef TERM_TYPE_HDR
  #define TERM_TYPE_HDR
  #include <z/term.h>
  #endif /* TERM_TYPE_HDR */

  #if (REQUIRE_TERM_TYPE == DECLARE)
  static  term_T termType;
  #define Term   termType.self
  #define Screen termType.self.screen
  #define Cursor termType.self.cursor
  #endif

#undef REQUIRE_TERM_TYPE
#endif /* REQUIRE_TERM_TYPE */

#ifdef REQUIRE_AUTH_TYPE
  #ifndef AUTH_TYPE_HDR
  #define AUTH_TYPE_HDR
  #include <z/auth.h>
  #endif /* AUTH_TYPE_HDR */

  #if (REQUIRE_AUTH_TYPE == DECLARE)
  static  auth_T authType;
  #define Auth   authType.self
  #endif

#undef REQUIRE_AUTH_TYPE
#endif /* REQUIRE_AUTH_TYPE */

#ifdef REQUIRE_I_TYPE
  #ifndef I_TYPE_HDR
  #define I_TYPE_HDR
  #include <z/i.h>
  #endif /* I_TYPE_HDR */

  #if (REQUIRE_I_TYPE == DECLARE)
  static  i_T iType;
  #define I   iType.self
  #endif

#undef REQUIRE_I_TYPE
#endif /* REQUIRE_I_TYPE */

#ifdef REQUIRE_IMAP_TYPE
  #ifndef IMAP_TYPE_HDR
  #define IMAP_TYPE_HDR
  #include <z/imap.h>
  #endif /* IMAP_TYPE_HDR */

  #if (REQUIRE_IMAP_TYPE == DECLARE)
  static  imap_T imapType;
  #define Imap   imapType.self
  #endif

#undef REQUIRE_IMAP_TYPE
#endif /* REQUIRE_IMAP_TYPE */

#ifdef REQUIRE_SMAP_TYPE
  #ifndef SMAP_TYPE_HDR
  #define SMAP_TYPE_HDR
  #include <z/smap.h>
  #endif /* SMAP_TYPE_HDR */

  #if (REQUIRE_SMAP_TYPE == DECLARE)
  static  smap_T smapType;
  #define Smap   smapType.self
  #endif

#undef REQUIRE_SMAP_TYPE
#endif /* REQUIRE_SMAP_TYPE */

#ifdef REQUIRE_VMAP_TYPE
  #ifndef VMAP_TYPE_HDR
  #define VMAP_TYPE_HDR
  #include <z/vmap.h>
  #endif /* VMAP_TYPE_HDR */

  #if (REQUIRE_VMAP_TYPE == DECLARE)
  static  vmap_T vmapType;
  #define Vmap   vmapType.self
  #endif

#undef REQUIRE_VMAP_TYPE
#endif /* REQUIRE_VMAP_TYPE */

#ifdef REQUIRE_ARGPARSE_TYPE
  #ifndef ARGPARSE_TYPE_HDR
  #define ARGPARSE_TYPE_HDR
  #include <z/argparse.h>
  #endif /* ARGPARSE_TYPE_HDR */

  #if (REQUIRE_ARGPARSE_TYPE == DECLARE)
    #ifndef Argparse
    static  argparse_T argparseType;
    #define Argparse   argparseType.self
    #endif
  #endif

#undef REQUIRE_ARGPARSE_TYPE
#endif /* REQUIRE_ARGPARSE_TYPE */

#ifdef REQUIRE_SYS_TYPE
  #ifndef SYS_TYPE_HDR
  #define SYS_TYPE_HDR
  #include <z/sys.h>
  #endif /* SYS_TYPE_HDR */

  #if (REQUIRE_SYS_TYPE == DECLARE)
  static  sys_T sysType;
  #define Sys   sysType.self
  #endif

#undef REQUIRE_SYS_TYPE
#endif /* REQUIRE_SYS_TYPE */

#ifdef REQUIRE_RE_TYPE
  #ifndef RE_TYPE_HDR
  #define RE_TYPE_HDR
  #include <z/re.h>
  #endif /* RE_TYPE_HDR */

  #if (REQUIRE_RE_TYPE == DECLARE)
  static  re_T reType;
  #define Re   reType.self
  #endif

#undef REQUIRE_RE_TYPE
#endif /* REQUIRE_RE_TYPE */

#ifdef REQUIRE_SPELL_TYPE
  #ifndef SPELL_TYPE_HDR
  #define SPELL_TYPE_HDR
  #include <z/spell.h>
  #endif /* SPELL_TYPE_HDR */

  #if (REQUIRE_SPELL_TYPE == DECLARE)
  static  spell_T spellType;
  #define Spell   spellType.self
  #endif

#undef REQUIRE_SPELL_TYPE
#endif /* REQUIRE_SPELL_TYPE */

#ifdef REQUIRE_VIDEO_TYPE
  #ifndef VIDEO_TYPE_HDR
  #define VIDEO_TYPE_HDR
  #include <z/video.h>
  #endif /* VIDEO_TYPE_HDR */

  #if (REQUIRE_VIDEO_TYPE == DECLARE)
  static  video_T videoType;
  #define Video   videoType.self
  #endif

#undef REQUIRE_VIDEO_TYPE
#endif /* REQUIRE_VIDEO_TYPE */

#ifdef REQUIRE_VUI_TYPE
  #ifndef VUI_TYPE_HDR
  #define VUI_TYPE_HDR
  #include <z/vui.h>
  #endif /* VUI_TYPE_HDR */

  #if (REQUIRE_VUI_TYPE == DECLARE)
  static  vui_T vuiType;
  #define Vui   vuiType.self
  #define Menu  vuiType.self.menu
  #endif

#undef REQUIRE_VUI_TYPE
#endif /* REQUIRE_VUI_TYPE */

#ifdef REQUIRE_READLINE_TYPE
  #ifndef READLINE_TYPE_HDR
  #define READLINE_TYPE_HDR
  #include <z/readline.h>
  #endif /* READLINE_TYPE_HDR */

  #if (REQUIRE_READLINE_TYPE == DECLARE)
  static  readline_T readlineType;
  #define Readline   readlineType.self
  #endif

#undef REQUIRE_READLINE_TYPE
#endif /* REQUIRE_READLINE_TYPE */

#ifdef REQUIRE_LA_TYPE
  #ifndef LA_TYPE_HDR
  #define LA_TYPE_HDR
  #include <z/la.h>
  #endif /* LA_TYPE_HDR */

  #if (REQUIRE_LA_TYPE == DECLARE)
  static  la_T __LA__;	
  #define La   __LA__.self
  #endif

#undef REQUIRE_LA_TYPE
#endif /* REQUIRE_LA_TYPE */

#ifdef REQUIRE_E_TYPE
  #ifndef E_TYPE_HDR
  #define E_TYPE_HDR
  #include <z/e.h>
  #endif /* E_TYPE_HDR */

  #if (REQUIRE_E_TYPE == DECLARE)
  typedef E_T      e_T;
  static  e_T   *__E__ = NULL;
  #define E      __E__->self
  #define Ed     __E__->__Ed__->self
  #define Win    __E__->__Ed__->__Win__.self
  #define Buf    __E__->__Ed__->__Buf__.self
  #define Msg    __E__->__Ed__->__Msg__.self
  #define EError __E__->__Ed__->__EError__.self
  #endif

#undef REQUIRE_E_TYPE
#endif /* REQUIRE_E_TYPE */

#ifdef REQUIRE_VWM_TYPE
  #ifndef VWM_TYPE_HDR
  #define VWM_TYPE_HDR
  #include <z/vwm.h>
  #endif /* VWM_TYPE_HDR */

  #if (REQUIRE_VWM_TYPE == DECLARE)
  static  vwm_T  *__VWM__ = NULL;
  #define Vwm     __VWM__->self
  #define Vframe  __VWM__->frame
  #define Vwin    __VWM__->win
  #endif

  #ifndef SHELL
  #define SHELL "zs"
  #endif

  #ifndef EDITOR
  #define EDITOR "E"
  #endif

  #ifndef DEFAULT_APP
  #define DEFAULT_APP SHELL
  #endif

#undef REQUIRE_VWM_TYPE
#endif /* REQUIRE_VWM_TYPE */

#ifdef REQUIRE_V_TYPE
  #ifndef V_TYPE_HDR
  #define V_TYPE_HDR
  #include <z/v.h>
  #endif /* V_TYPE_HDR */

  #if (REQUIRE_V_TYPE == DECLARE)
  static  v_t *__V__;
  #define V    __V__->self
  #endif

#undef REQUIRE_V_TYPE
#endif /* REQUIRE_V_TYPE */

#ifdef REQUIRE_MD5_TYPE
  #ifndef MD5_TYPE_HDR
  #define MD5_TYPE_HDR
  #include <z/md5.h>
  #endif /* MD5_TYPE_HDR */

  #if (REQUIRE_MD5_TYPE == DECLARE)
  static  md5_T   md5Type;
  #define Md5     md5Type.self
  #endif

#undef REQUIRE_MD5_TYPE
#endif /* REQUIRE_MD5_TYPE */

#ifdef REQUIRE_SHA256_TYPE
  #ifndef SHA256_TYPE_HDR
  #define SHA256_TYPE_HDR
  #include <z/sha256.h>
  #endif /* SHA256_TYPE_HDR */

  #if (REQUIRE_SHA256_TYPE == DECLARE)
  static  sha256_T   sha256Type;
  #define Sha256     sha256Type.self
  #endif

#undef REQUIRE_SHA256_TYPE
#endif /* REQUIRE_SHA256_TYPE */

#ifdef REQUIRE_SHA512_TYPE
  #ifndef SHA512_TYPE_HDR
  #define SHA512_TYPE_HDR
  #include <z/sha512.h>
  #endif /* SHA512_TYPE_HDR */

  #if (REQUIRE_SHA512_TYPE == DECLARE)
  static  sha512_T   sha512Type;
  #define Sha512     sha512Type.self
  #endif

#undef REQUIRE_SHA512_TYPE
#endif /* REQUIRE_SHA512_TYPE */

#ifdef REQUIRE_RANDOM_TYPE
  #ifndef RANDOM_TYPE_HDR
  #define RANDOM_TYPE_HDR
  #include <z/random.h>
  #endif /* RANDOM_TYPE_HDR */

  #if (REQUIRE_RANDOM_TYPE == DECLARE)
  static  random_T   randomType;
  #define Random     randomType.self
  #endif

#undef REQUIRE_RANDOM_TYPE
#endif /* REQUIRE_RANDOM_TYPE */

#ifdef REQUIRE_BCRYPT_TYPE
  #ifndef BCRYPT_TYPE_HDR
  #define BCRYPT_TYPE_HDR
  #include <z/bcrypt.h>
  #endif /* BCRYPT_TYPE_HDR */

  #if (REQUIRE_BCRYPT_TYPE == DECLARE)
  static  bcrypt_T   bcryptType;
  #define Bcrypt     bcryptType.self
  #endif

#undef REQUIRE_BCRYPT_TYPE
#endif /* REQUIRE_BCRYPT_TYPE */

#ifdef REQUIRE_BASE64_TYPE
  #ifndef BASE64_TYPE_HDR
  #define BASE64_TYPE_HDR
  #include <z/base64.h>
  #endif /* BASE64_TYPE_HDR */

  #if (REQUIRE_BASE64_TYPE == DECLARE)
  static  base64_T   base64Type;
  #define Base64     base64Type.self
  #endif

#undef REQUIRE_BASE64_TYPE
#endif /* REQUIRE_BASE64_TYPE */

#ifdef REQUIRE_CONTAIN_TYPE
  #ifndef CONTAIN_TYPE_HDR
  #define CONTAIN_TYPE_HDR
  #include <z/contain.h>
  #endif /* CONTAIN_TYPE_HDR */

  #if (REQUIRE_CONTAIN_TYPE == DECLARE)
  static  contain_T   containType;
  #define Contain     containType.self
  #endif

#undef REQUIRE_CONTAIN_TYPE
#endif /* REQUIRE_CONTAIN_TYPE */

// define it before net
#ifdef REQUIRE_URL_TYPE
  #ifndef URL_TYPE_HDR
  #define URL_TYPE_HDR
  #include <z/url.h>
  #endif /* URL_TYPE_HDR */

  /* Do not define any type here. This unit is indepented. */

#undef REQUIRE_URL_TYPE
#endif /* REQUIRE_URL_TYPE */

#ifdef REQUIRE_DL_TYPE
  #ifndef DL_TYPE_HDR
  #define DL_TYPE_HDR
  #include <z/dl.h>
  #endif /* DL_TYPE_HDR */

  /* Do not define any type here. This unit is indepented. */

#undef REQUIRE_DL_TYPE
#endif /* REQUIRE_DL_TYPE */

#ifdef REQUIRE_SUN_TYPE
  #ifndef SUN_TYPE_HDR
  #define SUN_TYPE_HDR
  #include <z/sun.h>
  #endif /* SUN_TYPE_HDR */

  /* Do not define any type here. This unit is indepented. */

#undef REQUIRE_SUN_TYPE
#endif /* REQUIRE_SUN_TYPE */

#ifdef REQUIRE_MOON_TYPE
  #ifndef MOON_TYPE_HDR
  #define MOON_TYPE_HDR
  #include <z/moon.h>
  #endif /* MOON_TYPE_HDR */

  /* Do not define any type here. This unit is trying to be indepented. */

#undef REQUIRE_MOON_TYPE
#endif /* REQUIRE_MOON_TYPE */

#ifdef REQUIRE_NET_TYPE
  #ifndef NET_TYPE_HDR
  #define NET_TYPE_HDR
  #include <z/net.h>
  #endif /* NET_TYPE_HDR */

  #if (REQUIRE_NET_TYPE == DECLARE)
  static  net_T netType;
  #define Net   netType.self
  #endif

#undef REQUIRE_NET_TYPE
#endif /* REQUIRE_NET_TYPE */

/* Development (ignore this section) */

#ifdef REQUIRE_NETM_TYPE
  #ifndef NETM_TYPE_HDR
  #define NETM_TYPE_HDR
  #include <z/netm.h>
  #endif /* NETM_TYPE_HDR */

  #if (REQUIRE_NETM_TYPE == DECLARE)
  static  netm_T netmType;
  #define Netm   netmType.self
  #endif

#undef REQUIRE_NETM_TYPE
#endif /* REQUIRE_NETM_TYPE */

/* --------------------------------- */

#ifdef REQUIRE_KEYS_MACROS
  #ifndef KEYS_MACROS_HDR
  #define KEYS_MACROS_HDR

  #ifndef BACKSPACE_KEY
  #define BACKSPACE_KEY   010
  #endif

  #ifndef ESCAPE_KEY
  #define ESCAPE_KEY      033
  #endif

  #ifndef ARROW_DOWN_KEY
  #define ARROW_DOWN_KEY  0402
  #endif

  #ifndef ARROW_UP_KEY
  #define ARROW_UP_KEY    0403
  #endif

  #ifndef ARROW_LEFT_KEY
  #define ARROW_LEFT_KEY  0404
  #endif

  #ifndef ARROW_RIGHT_KEY
  #define ARROW_RIGHT_KEY 0405
  #endif

  #ifndef HOME_KEY
  #define HOME_KEY        0406
  #endif

  #ifndef FN_KEY
  #define FN_KEY(x)       (x + 0410)
  #endif

  #ifndef DELETE_KEY
  #define DELETE_KEY      0512
  #endif

  #ifndef INSERT_KEY
  #define INSERT_KEY      0513
  #endif

  #ifndef PAGE_DOWN_KEY
  #define PAGE_DOWN_KEY   0522
  #endif

  #ifndef PAGE_UP_KEY
  #define PAGE_UP_KEY     0523
  #endif

  #ifndef END_KEY
  #define END_KEY         0550
  #endif

  #ifndef CTRL
  #define CTRL(X) (X & 037)
  #endif

  #ifndef MODE_KEY
  #define MODE_KEY  CTRL('\\')
  #endif
  #endif /* KEYS_MACROS_HDR */

#undef REQUIRE_KEYS_MACROS
#endif /* REQUIRE_KEYS_MACROS */

#ifdef REQUIRE_TERM_MACROS
  #ifndef TERM_MACROS_HDR
  #define TERM_MACROS_HDR

  #define TERM_ITALIC                 "\033[3m"
  #define TERM_ITALIC_LEN             4
  #define TERM_INVERTED               "\033[7m"
  #define TERM_INVERTED_LEN           4
  #define TERM_LAST_RIGHT_CORNER      "\033[999C\033[999B"
  #define TERM_LAST_RIGHT_CORNER_LEN  12
  #define TERM_FIRST_LEFT_CORNER      "\033[H"
  #define TERM_FIRST_LEFT_CORNER_LEN  3
  #define TERM_GET_PTR_POS            "\033[6n"
  #define TERM_GET_PTR_POS_LEN        4
  #define TERM_SCREEN_SAVE            "\033[?47h"
  #define TERM_SCREEN_SAVE_LEN        6
  #define TERM_SCREEN_RESTORE        "\033[?47l"
  #define TERM_SCREEN_RESTORE_LEN     6
  #define TERM_SCREEN_CLEAR           "\033[2J"
  #define TERM_SCREEN_CLEAR_LEN       4
  #define TERM_SCROLL_RESET           "\033[r"
  #define TERM_SCROLL_RESET_LEN       3
  #define TERM_GOTO_PTR_POS_FMT       "\033[%d;%dH"
  #define TERM_CURSOR_HIDE            "\033[?25l"
  #define TERM_CURSOR_HIDE_LEN        6
  #define TERM_CURSOR_SHOW            "\033[?25h"
  #define TERM_CURSOR_SHOW_LEN        6
  #define TERM_CURSOR_RESTORE         "\0338"
  #define TERM_CURSOR_RESTORE_LEN     2
  #define TERM_CURSOR_SAVE            "\0337"
  #define TERM_CURSOR_SAVE_LEN        2
  #define TERM_AUTOWRAP_ON            "\033[?7h"
  #define TERM_AUTOWRAP_ON_LEN        5
  #define TERM_AUTOWRAP_OFF           "\033[?7l"
  #define TERM_AUTOWRAP_OFF_LEN       5
  #define TERM_BELL                   "\033[7"
  #define TERM_BELL_LEN               3
  #define TERM_NEXT_BOL              "\033E"
  #define TERM_NEXT_BOL_LEN           2
  #define TERM_SCROLL_REGION_FMT      "\033[%d;%dr"
  #define TERM_COLOR_RESET            "\033[m"
  #define TERM_COLOR_RESET_LEN        3
  #define TERM_SET_COLOR_FMT          "\033[%dm"
  #define TERM_SET_COLOR_FMT_LEN      5
  #define TERM_LINE_CLR_EOL           "\033[2K"
  #define TERM_LINE_CLR_EOL_LEN       4

  #define COLOR_RED         31
  #define COLOR_GREEN       32
  #define COLOR_YELLOW      33
  #define COLOR_BLUE        34
  #define COLOR_MAGENTA     35
  #define COLOR_CYAN        36
  #define COLOR_WHITE       37
  #define COLOR_FG_NORMAL   39
  #define COLOR_BG_NORMAL   49

  #define COLOR_BOX         COLOR_YELLOW

  #define TERM_SEND_ESC_SEQ(seq_) IO.fd.write (this->out_fd, seq_, seq_ ## _LEN)
  #define SEND_ESC_SEQ(fd_, seq_) IO.fd.write ((fd_), seq_, seq_ ## _LEN)

  #define COLOR_MENU_HEADER COLOR_CYAN
  #define COLOR_MENU_BG     COLOR_RED
  #define COLOR_MENU_SEL    COLOR_GREEN

  #define COLOR_SU          COLOR_RED
  #define COLOR_BOX         COLOR_YELLOW
  #define COLOR_MSG         COLOR_YELLOW
  #define COLOR_ERROR       COLOR_RED
  #define COLOR_PROMPT      COLOR_YELLOW
  #define COLOR_NORMAL      COLOR_FG_NORMAL
  #define COLOR_TOPLINE     COLOR_YELLOW
  #define COLOR_DIVIDER     COLOR_MAGENTA
  #define COLOR_WARNING     COLOR_MAGENTA
  #define COLOR_SUCCESS     COLOR_GREEN
  #define COLOR_STATUSLINE  COLOR_BLUE
  #endif /* TERM_MACROS_HDR */

  #define TERM_DONOT_SAVE_SCREEN    (1 << 0)
  #define TERM_DONOT_CLEAR_SCREEN   (1 << 1)
  #define TERM_DONOT_RESTORE_SCREEN (1 << 2)

#undef TERM_MACROS
#endif /*TERM_MACROS */

#ifdef REQUIRE_MAP_MACROS
  #ifndef MAP_MACROS_HDR
  #define MAP_MACROS_HDR

  #define MAP_DEFAULT_LENGTH 32
  // hs_ = ((hs_ << 5) + hs_) + __key__[i_++];              
  #define MAP_HASH_KEY(__map__, __key__) ({                   \
    ssize_t hs_ = 5381; int i_ = 0;                           \
    while (__key__[i_])                                       \
      hs_ = (hs_ * 33) ^ __key__[i_++];                       \
    hs_ % __map__->num_slots;                                 \
  })

  #define MAP_RELEASE_SLOT(_it, _tp, _fun)                    \
  ({                                                          \
    while (_it) {                                             \
      _tp *_tmp = _it->next;                                  \
      free (_it->key);                                        \
      _fun (_it->value);                                      \
      free (_it);                                             \
      _it = _tmp;                                             \
    }                                                         \
  })

  #define MAP_CLEAR(_map, _fun)                               \
  ({                                                          \
    for (size_t i_ = 0; i_ < _map->num_slots; i_++) {         \
      ifnot (_map->slots[i_]) continue;                       \
      _fun (_map->slots[i_]);                                 \
      _map->slots[i_] = NULL;                                 \
    }                                                         \
    _map->num_keys = 0;                                       \
  })

  #define MAP_RELEASE(_map, _fun)                             \
  do {                                                        \
    if (_map is NULL) return;                                 \
    _fun (_map);                                              \
    free (_map->slots);                                       \
    free (_map);                                              \
    _map = NULL;                                              \
  } while (0)

  #define MAP_NEW(_TP, _tp, _num)                             \
  ({                                                          \
     _TP *_map = Alloc (sizeof (_TP));                        \
     int _num_slots = (_num < 1 ? MAP_DEFAULT_LENGTH : _num); \
     _map->slots = Alloc (sizeof (_tp *) * num_slots);        \
     _map->num_slots = _num_slots;                            \
     _map->num_keys = 0;                                      \
     for (;--_num_slots >= 0;) _map->slots[_num_slots] = NULL;\
     _map;                                                    \
  })

  #define MAP_GET(_tp, _map, _key, _idx)        \
  ({                                            \
    _idx = MAP_HASH_KEY (_map, _key);           \
    _tp *_slot = _map->slots[_idx];             \
    while (_slot) {                             \
      if (Cstring.eq (_slot->key, _key)) break; \
      _slot = _slot->next;                      \
    }                                           \
    _slot;                                      \
  })

  #define MAP_POP(_tp, _map, _key, _idx)        \
  ({                                            \
    _idx = MAP_HASH_KEY (_map, _key);           \
    _tp *_slot = _map->slots[_idx];             \
    _tp *_prev = _slot;                         \
    while (_slot) {                             \
      if (Cstring.eq (_slot->key, _key)) {      \
        _prev->next = _slot->next;              \
        break;                                  \
      }                                         \
      _prev = _slot;                            \
      _slot = _slot->next;                      \
    }                                           \
    if (_slot != NULL) {                        \
      _map->num_keys--;                         \
      if (_prev == _slot)                       \
        _map->slots[_idx] = _slot->next;        \
    }                                           \
    _slot;                                      \
  })

  #define MAP_SET(_tp_, _map_, _key_, _val_)            \
  ({                                                    \
    uint _idx_ = 0;                                     \
    _tp_ *_it_ = MAP_GET(_tp_, _map_, _key_, _idx_);    \
    ifnot (NULL is _it_) {                              \
      _it_->value = _val_;                              \
    } else {                                            \
      _it_ = Alloc (sizeof (_tp_));                     \
      _it_->key = Cstring.dup (_key_, bytelen (_key_)); \
      _it_->value = _val_;                              \
      _it_->next = _map_->slots[_idx_];                 \
      _map_->slots[_idx_] = _it_;                       \
      _map_->num_keys++;                                \
    }                                                   \
    _it_;                                               \
  })

  #define MAP_NEW_ITEM(_tp_, _map_, _key_)              \
  ({                                                    \
    uint _idx_ = 0;                                     \
    _tp_ *_it_ = MAP_GET(_tp_, _map_, _key_, _idx_);    \
    if (NULL is _it_) {                                 \
      _it_ = Alloc (sizeof (_tp_));                     \
      _it_->key = Cstring.dup (_key_, bytelen (_key_)); \
      _it_->next = _map_->slots[_idx_];                 \
      _map_->slots[_idx_] = _it_;                       \
      _map_->num_keys++;                                \
    }                                                   \
    _it_;                                               \
  })

  #endif /* MAP_MACROS_HDR */

#undef REQUIRE_MAP_MACROS
#endif /* REQUIRE_MAP_MACROS */

#ifdef LIBRARY

  #ifndef LIBRARY_HDR
  #define LIBRARY_HDR
  #endif /* LIBRARY_HDR */

#undef LIBRARY
#endif /* LIBRARY */

#ifndef OPT_NO_VERBOSE
#define OPT_NO_VERBOSE 0
#endif

#ifndef OPT_VERBOSE_ON_ERROR
#define OPT_VERBOSE_ON_ERROR 1
#endif

#ifndef OPT_VERBOSE
#define OPT_VERBOSE 2
#endif

#ifndef OPT_VERBOSE_EXTRA
#define OPT_VERBOSE_EXTRA 3
#endif

#ifndef OPT_NO_FORCE
#define OPT_NO_FORCE       0
#endif

#ifndef OPT_FORCE
#define OPT_FORCE          1
#endif

#ifndef OPT_NO_BACKUP
#define OPT_NO_BACKUP      0
#endif

#ifndef OPT_BACKUP
#define OPT_BACKUP         1
#endif

#ifndef OPT_NO_DEREFERENCE
#define OPT_NO_DEREFERENCE 0
#endif

#ifndef OPT_DEREFERENCE
#define OPT_DEREFERENCE    1
#endif

#ifndef OPT_NO_PRESERVE
#define OPT_NO_PRESERVE    0
#endif

#ifndef OPT_PRESERVE
#define OPT_PRESERVE       1
#endif

#ifndef OPT_PRESERVE_OWNER
#define OPT_PRESERVE_OWNER 2
#endif

#ifndef OPT_NO_RECURSIVE
#define OPT_NO_RECURSIVE   0
#endif

#ifndef OPT_RECURSIVE
#define OPT_RECURSIVE      1
#endif

#ifndef OPT_NO_UPDATE
#define OPT_NO_UPDATE      0
#endif

#ifndef OPT_UPDATE
#define OPT_UPDATE         1
#endif

#ifndef OPT_NO_ALL
#define OPT_NO_ALL         0
#endif

#ifndef OPT_ALL
#define OPT_ALL            1
#endif

#ifndef OPT_NO_INTERACTIVE
#define OPT_NO_INTERACTIVE 0
#endif

#ifndef OPT_INTERACTIVE
#define OPT_INTERACTIVE    1
#endif

#ifndef OPT_MAXDEPTH
#define OPT_MAXDEPTH 1024
#endif

#ifndef OPT_NO_YEAR
#define OPT_NO_YEAR -1
#endif

#ifndef OPT_NO_MONTH
#define OPT_NO_MONTH -1
#endif

#ifndef OPT_NO_DAY
#define OPT_NO_DAY -1
#endif

#ifndef OPT_NO_HOUR
#define OPT_NO_HOUR -1
#endif

#ifndef OPT_NO_MINUTES
#define OPT_NO_MINUTES -1
#endif

#ifndef OPT_NO_SECONDS
#define OPT_NO_SECONDS -1
#endif

/* Those application routines are quite the same and is useless
 * to repeat ourselves.
 */
#define __INIT__(_T_) _T_ ## Type = __init_ ## _T_ ## __ ()

#define __INIT_APP__                 \
  __INIT__ (argparse);               \
  __INIT__ (io);                     \
  int version = 0;                   \
  int retval  = 0;                   \
  char *progname = (char *) argv[0]; \
  int argparse_flags = 0;            \
  (void) progname;                   \
  argparse_t argparser

#define PARSE_ARGS                                               \
  if (0 == isatty (STDIN_FILENO))                                \
    argparse_flags |= ARGPARSE_DONOT_EXIT_ON_UNKNOWN;            \
  Argparse.init (&argparser, options, usage, argparse_flags);    \
  argc = Argparse.exec (&argparser, argc, (const char **) argv); \
  CHECK_VERSION

#define CHECK_VERSION  \
  if (version) {       \
    fprintf (stderr, "%s\n", VERSION_STRING); \
    return 1;          \
  }

#define CHECK_ARGC     \
  ifnot (argc) {       \
    Argparse.print_usage (&argparser); \
    return 1;          \
  }

#endif /* REQUIRE_Z_ENV */
