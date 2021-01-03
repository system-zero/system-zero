/* based on:
 * https://docs.oracle.com/cd/E23824_01/html/819-2145/pam-1.html
 *
 * much logic came by the NetBSD doas sources at:
 * https://github.com/slicer69/doas
 *
 * and to OpenDoas sources at:
 * https://github.com/Duncaen/OpenDoas
 *
 */

#define LIBRARY "Auth"

#define REQUIRE_STDIO
#define REQUIRE_UNISTD
#define REQUIRE_SYS_TYPES
#define REQUIRE_TIME
#define REQUIRE_PWD
#define REQUIRE_GRP
#define REQUIRE_PAM
#define REQUIRE_STRING_TYPE  DECLARE
#define REQUIRE_USTRING_TYPE DECLARE
#define REQUIRE_CSTRING_TYPE DECLARE
#define REQUIRE_INPUT_TYPE   DECLARE
#define REQUIRE_AUTH_TYPE    DONOT_DECLARE

#include <z/cenv.h>

#define $my(__v__) this->prop->__v__

struct auth_prop {
  uid_t uid;
  gid_t gid;

  time_t timeout;

  string_t
    *user,
    *group,
    *test_prog,
    *hashed_data;

  int
    num_tries,
    cached_time;

  char shared_string[MAXLEN_USERNAME + 1];

  void *object;

  AuthGetPasswd_cb get_passwd_cb;
};

#define ROOT_UID  0
#define ROOT_GID  0

private int
auth_pam_conversation (int num_msg, const struct pam_message **msg,
                    struct pam_response* *resp, void *appdata_ptr) {
  (void) num_msg; (void) msg;
  struct pam_response *reply = Alloc (sizeof (struct pam_response));

  reply[0].resp = (char *) appdata_ptr;
  reply[0].resp_retcode = 0;

  *resp = reply;

  return PAM_SUCCESS;
}

private int auth_pam (auth_t *this, const char *pass) {
  if ($my(user) is NULL or
      $my(user)->num_bytes is 0 or
      pass is NULL or
      *pass is '\0')
    return NOTOK;

  size_t len = bytelen (pass);
  if (len > MAXLEN_PASSWD)
    return NOTOK;

  int retval = NOTOK;

  pam_handle_t *pamh = NULL;

  char *password = Alloc (len + 1);

  Cstring.cp (password, len + 1, pass, len);

  int saved_stdin = -1;
  int saved_stdout = -1;

  saved_stdin = dup (STDIN_FILENO);
  if (-1 is saved_stdin) goto theend;
  close (STDIN_FILENO);

  saved_stdout = dup (STDOUT_FILENO);
  if (-1 is saved_stdout) goto theend;
  close (STDOUT_FILENO);

  if (-1 is dup2 (STDERR_FILENO, STDOUT_FILENO))
    goto theend;

  struct pam_conv pamc = {auth_pam_conversation, password};

  retval = pam_start ($my(test_prog)->bytes, $my(user)->bytes,
      &pamc, &pamh);

  if (retval isnot PAM_SUCCESS) {
    fprintf (stderr, "pam_start(): %s\n", pam_strerror (pamh, retval));
    goto theend;
  }

  const char *ttydev;

  if (isatty (STDIN_FILENO)) {
    ttydev = ttyname (STDIN_FILENO);
    ifnot (NULL is ttydev) {
      if (Cstring.eq_n (ttydev, "/dev/", 5))
        ttydev += 5;

      retval = pam_set_item (pamh, PAM_TTY, ttydev);
      if (retval isnot PAM_SUCCESS) {
        fprintf (stderr, "pam_set_item(): %s\n", pam_strerror (pamh, retval));
        goto theend;
      }
    }
  }

  retval = pam_authenticate (pamh, PAM_SILENT|PAM_DISALLOW_NULL_AUTHTOK);

  if (retval is PAM_SUCCESS) {
    if (PAM_SUCCESS isnot (retval = pam_acct_mgmt (pamh, PAM_SILENT|PAM_DISALLOW_NULL_AUTHTOK))) {
      if (retval is PAM_NEW_AUTHTOK_REQD) {
        int num_tries = 3;
        while (num_tries--) {
		  retval = pam_chauthtok (pamh, PAM_SILENT|PAM_CHANGE_EXPIRED_AUTHTOK);
          if (retval is PAM_SUCCESS) break;
        }
      }
    }
  }

  if (retval is PAM_SUCCESS)
    retval = pam_setcred (pamh, PAM_REFRESH_CRED);

theend:
  ifnot (NULL is pamh)
    pam_end (pamh, retval);

  if (saved_stdin isnot -1) {
    if (-1 is dup2 (saved_stdin, STDIN_FILENO))
      retval = NOTOK;
    close (saved_stdin);
  }

  if (saved_stdout isnot -1) {
    close (STDOUT_FILENO);
    if (-1 is dup2 (saved_stdout, STDOUT_FILENO))
      retval = NOTOK;
  }

  return (retval is PAM_SUCCESS ? OK : NOTOK);
}

private int auth_reset_hashed (auth_t *this) {
  $my(timeout) = time (NULL);
  String.clear ($my(hashed_data));
  return OK;
}

private int auth_check (auth_t *this) {
  time_t cur = time (NULL);
  time_t diff = cur - $my(timeout);
  int is_expired = diff > $my(cached_time);

  if (is_expired) {
    int num_tries = $my(num_tries);
    int retval = NOTOK;

    for (;;) {
      retval = $my(get_passwd_cb) (this);
      if (retval is NOTOK)
        return NOTOK;

      retval = auth_pam (this, $my(hashed_data)->bytes);
      if (retval is OK) break;

      String.clear ($my(hashed_data));

      ifnot (num_tries--) {
        return NOTOK;
      }
    }

    String.append_byte ($my(hashed_data), '\n');
    $my(timeout) = cur;
  }

  return OK;
}

private void auth_release (auth_t *this) {
  if (this is NULL) return;
  String.release ($my(user));
  String.release ($my(group));
  String.release ($my(test_prog));
  String.release ($my(hashed_data));
  free (this->prop);
  free (this);
}

private int auth_get_passwd_default_cb (auth_t *this) {
  fprintf (stdout, "passwd:");
  fflush (stdout);

  Input.raw_mode (STDIN_FILENO);

  flockfile (stdin);

  utf8 c;
  char buf[8];
  int len;
  while ((c = Input.getkey (STDIN_FILENO)) isnot '\r') {
    len = 0;
    buf[0] = '\0';
    Ustring.character (c, buf, &len);
    String.append_with ($my(hashed_data), buf);
    memset (buf, 0, sizeof (buf));
  }

  fprintf (stdout, "\r\n");
  fflush (stdout);

  Input.orig_mode (STDIN_FILENO);

  funlockfile (stdin);

  return OK;
}

static gid_t auth_get_uid (auth_t *this) {
  if (NULL is this) return -1;

  return $my(uid);
}

static gid_t auth_get_gid (auth_t *this) {
  if (NULL is this) return -1;

  return $my(gid);
}

static char *auth_get_group (auth_t *this) {
  if (NULL is this or NULL is $my(group)) return NULL;
  size_t len = $my(group)->num_bytes;
  if (len > MAXLEN_USERNAME) return NULL;

  memset ($my(shared_string), 0, sizeof ($my(shared_string)));

  Cstring.cp ($my(shared_string), MAXLEN_USERNAME,
      $my(group)->bytes, len);
  return $my(shared_string);
}

static char *auth_get_user (auth_t *this) {
  if (NULL is this or NULL is $my(user)) return NULL;
  size_t len = $my(user)->num_bytes;
  if (len > MAXLEN_USERNAME) return NULL;

  memset ($my(shared_string), 0, sizeof ($my(shared_string)));

  Cstring.cp ($my(shared_string), MAXLEN_USERNAME,
      $my(user)->bytes, len);
  return $my(shared_string);
}

static int auth_set_timeout (auth_t *this, time_t timeout) {
  if (NULL is this) return NOTOK;
  time_t cur = time (NULL);
  if (cur < timeout) return NOTOK;
  $my(timeout) = timeout;
  return OK;
}

static int auth_set_cached_time (auth_t *this, int cached_time) {
  if (NULL is this) return NOTOK;

  if (cached_time < 0 or cached_time > MAX_CACHED_TIME)
    return NOTOK;

  $my(cached_time) = cached_time;
  return OK;
}

static int auth_set_num_tries (auth_t *this, int num_tries) {
  if (NULL is this) return NOTOK;

  if (num_tries < 0 or num_tries > MAX_NUM_TRIES_AFTER_FAILURE)
    return NOTOK;

  $my(num_tries) = num_tries;
  return OK;
}

static auth_t *auth_new (const char *user, const char *test_prog, int cached_time) {
  uid_t uid = getuid ();
  gid_t gid = getgid ();

  if (ROOT_UID is uid or ROOT_GID is gid) {
    fprintf (stderr, "can not be called by the root user\n");
    return NULL;
  }

  errno = 0;
  struct passwd *pswd = getpwuid (uid);
  if (NULL is pswd) {
    fprintf (stderr, "can not read password record %s\n", strerror (errno));
    return NULL;
  }

  size_t name_len = bytelen (pswd->pw_name);

  ifnot (name_len) {
    fprintf (stderr, "user name has no length\n");
    return NULL;
  }

  if (name_len > MAXLEN_USERNAME) {
    fprintf (stderr, "user name exceeds maximum length %d\n", MAXLEN_USERNAME);
    return NULL;
  }

  ifnot (NULL is user) {
    ifnot (Cstring.eq  (pswd->pw_name, user)) {
      fprintf (stderr, "%s: invalid user\n", user);
      return NULL;
    }
  }

  struct group *gr = getgrgid (gid);
  if (NULL is gr) {
    fprintf (stderr, "can not read group record %s\n", strerror (errno));
    return NULL;
  }

  size_t group_len = bytelen (gr->gr_name);

  ifnot (group_len) {
    fprintf (stderr, "group name has no length\n");
    return NULL;
  }

  if (group_len > MAXLEN_USERNAME) {
    fprintf (stderr, "group name exceeds maximum length %d\n", MAXLEN_USERNAME);
    return NULL;
  }

  auth_t *this = Alloc (sizeof (auth_t));
  this->prop = Alloc (sizeof (auth_prop));

  $my(uid) = uid;
  $my(gid) = gid;
  $my(num_tries) = MAX_NUM_TRIES_AFTER_FAILURE;
  $my(user) = NULL;
  $my(group) = NULL;
  $my(hashed_data) = NULL;
  $my(test_prog) = NULL;

  $my(user) = String.new_with (pswd->pw_name);
  $my(group) = String.new_with (gr->gr_name);

  $my(hashed_data) = String.new (32);

  $my(test_prog) = String.new_with (
      (NULL is test_prog ? AUTH_TEST_PROG : test_prog));

  $my(cached_time) =
      (cached_time <= 0 ? MIN_CACHED_TIME :
      (cached_time > MAX_CACHED_TIME ? MAX_CACHED_TIME : cached_time));

  $my(timeout) = 0;

  $my(get_passwd_cb) = auth_get_passwd_default_cb;
  $my(object) = NULL;

  return this;
}

public auth_T  __init_auth__ (void) {
  __INIT__ (input);
  __INIT__ (string);
  __INIT__ (cstring);
  __INIT__ (ustring);

  return (auth_T) {
    .self = (auth_self) {
      .new = auth_new,
      .check = auth_check,
      .release = auth_release,
      .set = (auth_set_self) {
        .timeout = auth_set_timeout,
        .num_tries = auth_set_num_tries,
        .cached_time = auth_set_cached_time
      },
      .get = (auth_get_self) {
        .uid = auth_get_uid,
        .gid = auth_get_gid,
        .user = auth_get_user,
        .group = auth_get_group
      }
    }
  };
}
