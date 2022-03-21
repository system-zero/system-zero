static int string_eq_n (const char *sa, const char *sb, size_t n) {
  const uchar *spa = (const uchar *) sa;
  const uchar *spb = (const uchar *) sb;
  for (;n--; spa++, spb++) {
    if (*spa != *spb)
      return 0;

    if (*spa == 0) return 1;
  }

  return 1;
}

static int item_matchs (dl_t *dl, const char *name, size_t len) {
  for (int i = 0; i < dl->num; i++)
    if (string_eq_n (dl->dependencies[i], name, len))
      return 1;

  return 0;
}

static void dl_append_dependency (dl_t *dl, const char *name, int depth, struct found_t reason) {
  size_t len = bytelen (name);
  if (item_matchs (dl, name, len) or string_eq_n (dl->unit, name, len))
    return;

  char *item = Alloc (len + 32);

  for (size_t i = 0; i < len; i++)
    item[i] = name[i];

  const char *reas = "";

  switch (reason.how) {
    case RPATH:
      if ((int) reason.depth + 1 >= depth)
        reas = " [rpath]";
      break;

    case LD_LIBRARY_PATH:
      reas = " [LD_LIBRARY_PATH]";
      break;

    case RUNPATH:
      reas = " [runpath]";
      break;

    case LD_SO_CONF:
      reas = " [ld.so.conf]";
      break;

    case DIRECT:
      reas = " [direct]";
      break;

    case DEFAULT:
      reas = " [default path]";
      break;

    default:
      break;
  }

  size_t rlen = bytelen (reas);
  for (size_t i = 0; i < rlen; i++)
    item[len + i] = reas[i];
  item[len + rlen] = '\0';

  dl->num++;
  dl->dependencies = Realloc (dl->dependencies, dl->num * sizeof (char *));
  dl->dependencies[dl->num - 1] = item;
}

static void dl_not_found (size_t depth, size_t needed_not_found,
    struct small_vec_u64_t *needed_buf_offsets,
    char *runpath, struct libtree_state_t *s, int no_def_lib) {
  (void) runpath; (void) no_def_lib;
  dl_t *dl = s->dl;

  for (size_t i = 0; i < needed_not_found; ++i) {
    s->found_all_needed[depth] = i + 1 >= needed_not_found;
    size_t len = needed_buf_offsets->p[i];
    char *sp = s->string_table.arr + len;

    if (item_matchs (dl, sp, len)) continue;

    char *item = Alloc (len + 16);
    for (size_t j = 0; j < len; j++)
      item[j] = sp[j];

    const char *nfound = " [not found]";
    for (size_t j = 0; j < 12; j++)
      item[len + j] = nfound[j];

    item[len + 12] = '\0';

    dl->num++;
    dl->dependencies = Realloc (dl->dependencies, dl->num * sizeof (char *));
    dl->dependencies[dl->num - 1] = item;
  }
}

int DlDumpDependencies (dl_t *dl, const char *object) {
  struct libtree_state_t s;

  s.color = 0;
  // let the user decide
  s.verbosity = dl->verbose;
  s.path = dl->path;

  // now try to preserve the code logic
  // - no need to ask why, as either way we get what we really want
  struct utsname uname_val;
  if (uname (&uname_val) isnot 0)
      return NOTOK;

  s.PLATFORM = uname_val.machine;
  s.OSNAME = uname_val.sysname;
  s.OSREL = uname_val.release;

  s.LIB = (char *) "lib";

  /* here it is our tiny intervention */

  size_t len = bytelen (object);
  dl->unit = Alloc (len + 1);
  for (size_t i = 0; i < len; i++)
    dl->unit[i] = object[i];

  dl->unit[len] = '\0';
  dl->num = 0;
  dl->dependencies = Alloc (sizeof (char *));

  s.dl = dl;

  /* now everything else is from upstream */
  libtree_state_init(&s);

  parse_ld_so_conf(&s);
  parse_ld_library_path(&s);
  set_default_paths(&s);

  int result = recurse (object, 0, &s, EITHER,
    (struct found_t){.how = INPUT, .depth = 0});

  /* well... almost */
  dl = s.dl;

  libtree_state_free(&s);

  return (result != 0 ? NOTOK : OK); // todo get_error[_string]
}

void DlRelease (dl_t *dl) {
  for (int i = 0; i < dl->num; i++) free (dl->dependencies[i]);
  free (dl->dependencies);
  free (dl->unit);
}
