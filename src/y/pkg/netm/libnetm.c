#define REQUIRE_IS_DIRECTORY
#define REQUIRE_STDIO
#define REQUIRE_DIRLIST
#define REQUIRE_STR_EQ
#define REQUIRE_STR_NEW_WITH_FMT
#define REQUIRE_STRING
#define REQUIRE_ALLOC

#include <libc.h>
#include <z/netm.h>

static int netm_get_wireless_iface (netm_t *this, const char *dir) {
  int retval = NOTOK;

  if (NULL == dir) {
    tostderr ("dir argument is a NULL pointer\n");
    return retval;
  }

  ifnot (is_directory (dir)) {
    tostderr ("%s: not a directory\n", dir);
    return retval;
  }

  dirlist_t *iface_dlist = NULL;
  dirlist_t *dlist = dirlist (dir);

  if (NULL == dlist) {
    tostderr ("%s: dirlist() failed: %s\n", dir, errno_string (sys_errno));
    return retval;
  }

  for (int i = 0; i < dlist->num_entries; i++) {
    char *entry = dlist->entries[i];
    if (str_eq (entry, "lo") ||
        str_eq (entry, "ens32"))
      continue;

    ifnot (NULL == iface_dlist)
      dirlist_release (&iface_dlist);

    char *newdir = str_new_with_fmt ("%s/%s", dir, entry);
    iface_dlist = dirlist (newdir);

    if (iface_dlist == NULL)
      continue;

    for (int j = 0; j < iface_dlist->num_entries; j++) {
      char *f = iface_dlist->entries[j];
      if (0 == str_eq (f, "wireless") &&
          0 == str_eq (f, "phy80211"))
      continue;

      if (NULL == this->iface)
        this->iface = string_new_with (entry);
      else
        string_replace_with (this->iface, entry);

      ifnot (NULL == this->iface_dir)
        Release (this->iface_dir);

      this->iface_dir = string_new_with_fmt ("%s/%s", dir, entry);
      retval = OK;
      goto theend;
    }
  }

theend:
  ifnot (NULL == dlist)
    dirlist_release (&dlist);

  ifnot (NULL == iface_dlist)
    dirlist_release (&iface_dlist);

  return retval;
}

netm_t *netm_init (const char *dir) {
  netm_t *this = Alloc (sizeof (netm_t));
  this->iface = this->iface_dir = NULL;
  netm_get_wireless_iface (this, dir);
  return this;
}

void netm_release (netm_t **thisp) {
  if (NULL == *thisp)
    return;

  netm_t *this = *thisp;
  string_release (this->iface);
  string_release (this->iface_dir);
  Release (this);
  *thisp = NULL;
}
