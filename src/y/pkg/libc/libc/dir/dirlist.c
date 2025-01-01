// provides: dirlist_t *dirlist (const char *)
// provides: void dirlist_release (dirlist_t **)
// requires: stdlib/alloc.c
// requires: string/bytelen.c
// requires: string/str_new.c
// requires: dir/opendir.c
// requires: dir/dirent.h
// requires: dir/dirlist.h

dirlist_t *dirlist (const char *dir) {
  if (NULL == dir)
    return NULL;

  DIR *dh = sys_opendir (dir);
  if (NULL == dh)
    return NULL;

  dirlist_t *dlist = Alloc (sizeof (dirlist_t));

  dlist->num_entries = 0;
  dlist->mem_entries = 4;

  dlist->entries = Alloc (sizeof (char *) * dlist->mem_entries);
  dlist->types = Alloc (sizeof (int) * dlist->mem_entries);

  struct dirent *dp;

  while (1) {
    if (NULL == (dp = sys_readdir (dh)))
      break;

    size_t namelen = bytelen (dp->d_name);
    if (0 == namelen)
      continue;

    if (namelen <= 2 &&
        dp->d_name[0] == '.' && (dp->d_name[1] == '\0' || dp->d_name[1] == '.'))
      continue;

    dlist->num_entries++;
    if (dlist->num_entries > dlist->mem_entries) {
      dlist->mem_entries *= 2;
      dlist->entries = Realloc (dlist->entries, sizeof (char *) * dlist->mem_entries);
      dlist->types = Realloc (dlist->types, sizeof (int) * dlist->mem_entries);
    }

    dlist->entries[dlist->num_entries - 1] = str_new_with_len (dp->d_name, namelen);
    dlist->types[dlist->num_entries - 1] = dp->d_type;
  }

  sys_closedir (dh);
  return dlist;
}

void dirlist_release (dirlist_t **dirp) {
  if (*dirp == NULL)
    return;

  for (int i = 0; i < (*dirp)->num_entries; i++)
    Release ((*dirp)->entries[i]);

  Release ((*dirp)->types);
  Release (*dirp);
  *dirp = NULL;
}
