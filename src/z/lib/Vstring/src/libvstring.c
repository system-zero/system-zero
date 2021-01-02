#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include <zc.h>
#include <dlist.h>
#include <libstring.h>
#include <libcstring.h>
#include <libvstring.h>

static  string_T StringT;
#define String   StringT.self

static  cstring_T CstringT;
#define Cstring   CstringT.self

static void vstring_release_item (vstring_t *it) {
  if (it is NULL) return;
  String.release (it->data);
  free (it);
  it = NULL;
}

static void vstring_clear (Vstring_t *this) {
  if (NULL is this) return;

  vstring_t *it = this->head;
  while (it) {
    vstring_t *tmp = it->next;
    vstring_release_item (it);
    it = tmp;
  }

  this->head = this->tail = this->current = NULL;
  this->num_items = 0; this->cur_idx = -1;
}

static void vstring_release (Vstring_t *this) {
  if (NULL is this) return;
  vstring_clear (this);
  free (this);
}

static Vstring_t *vstring_new (void) {
  return Alloc (sizeof (Vstring_t));
}

static vstring_t *vstring_new_item (void) {
  return Alloc (sizeof (vstring_t));
}

static size_t vstring_get_size (Vstring_t *this) {
  size_t size = 0;
  vstring_t *it = this->head;

  while (it) {
    size += it->data->num_bytes;
    it = it->next;
  }

  return size;
}

static char *vstring_to_cstring (Vstring_t *this, int addnl) {
  size_t len = vstring_get_size (this) + (addnl ? this->num_items : 0);
  ifnot (len) return NULL;

  char *buf = Alloc (len + 1);

  vstring_t *it = this->head;

  size_t offset = 0;

  while (it) {
    Cstring.byte.cp (buf + offset, it->data->bytes, it->data->num_bytes);
    offset += it->data->num_bytes;
    if (addnl) buf[offset++] = '\n';
    it = it->next;
  }

  buf[len] = '\0';
  return buf;
}

/* maybe also a vstring_join_u() for characters as separators */
static string_t *vstring_join_allocated (Vstring_t *this, char *sep, string_t *bytes) {
  String.clear (bytes);

  vstring_t *it = this->head;

  while (it) {
    String.append_with_fmt (bytes, "%s%s", it->data->bytes, sep);
    it = it->next;
  }

  if (this->num_items)
    String.clear_at (bytes, bytes->num_bytes -
        (NULL is sep ? 0 : bytelen (sep)));

  return bytes;
}

static string_t *vstring_join (Vstring_t *this, char *sep) {
  string_t *bytes = String.new (32);
  return vstring_join_allocated (this, sep, bytes);
}

static void vstring_append (Vstring_t *this, vstring_t *new) {
  int cur_idx = this->cur_idx;
  DListSetCurrent (this, -1);
  DListAppendCurrent (this, new);
  DListSetCurrent (this, cur_idx);
}

static void vstring_current_append_with (Vstring_t *this, char *bytes) {
  vstring_t *vstr = Alloc (sizeof (vstring_t));
  vstr->data = String.new_with (bytes);
  DListAppendCurrent (this, vstr);
}

static void vstring_current_append_with_len (Vstring_t *this, char *bytes, size_t len) {
  vstring_t *vstr = Alloc (sizeof (vstring_t));
  vstr->data = String.new_with_len (bytes, len);
  DListAppendCurrent (this, vstr);
}

static void vstring_append_with_len (Vstring_t *this, char *bytes, size_t len) {
  int cur_idx = this->cur_idx;
  if (cur_idx isnot this->num_items - 1)
    DListSetCurrent (this, -1);

  vstring_current_append_with_len (this, bytes, len);
  DListSetCurrent (this, cur_idx);
}

static void vstring_append_with (Vstring_t *this, char *bytes) {
  int cur_idx = this->cur_idx;
  if (cur_idx isnot this->num_items - 1)
    DListSetCurrent (this, -1);

  vstring_current_append_with (this, bytes);
  DListSetCurrent (this, cur_idx);
}

static void vstring_append_with_fmt (Vstring_t *this, char *fmt, ...) {
  size_t len = VA_ARGS_FMT_SIZE(fmt);
  char bytes[len + 1];
  VA_ARGS_GET_FMT_STR(bytes, len, fmt);
  vstring_append_with_len (this, bytes, len);
}

static void vstring_current_prepend_with (Vstring_t *this, char *bytes) {
  vstring_t *vstr = Alloc (sizeof (vstring_t));
  vstr->data = String.new_with (bytes);
  DListPrependCurrent (this, vstr);
}

static void vstring_prepend_with (Vstring_t *this, char *bytes) {
  vstring_t *vstr = Alloc (sizeof (vstring_t));
  vstr->data = String.new_with (bytes);
  DListPrepend (this, vstr);
}

/* like cstring_dup(), as a new copy */
static Vstring_t *vstring_dup (Vstring_t *this) {
  Vstring_t *vs = vstring_new ();
  vstring_t *it = this->head;
  while (it) {
    vstring_current_append_with (vs, it->data->bytes);
    it = it->next;
  }

  DListSetCurrent (vs, this->cur_idx);
  return vs;
}

/* so far, we always want sorted and unique members */
static Vstring_t *vstring_add_sort_and_uniq (Vstring_t *this, char *bytes) {
  vstring_t *vs = Alloc (sizeof (vstring_t));
  vs->data = String.new_with (bytes);

  int res = 1;

  if (this->head is NULL) {
    this->head = this->tail = vs;
    this->head->prev = this->head->next = NULL;
    goto theend;
  }

  res = strcmp (bytes, this->head->data->bytes);

  ifnot (res) goto theend;

  if (0 > res) {
    this->head->prev = vs;
    vs->next = this->head;
    this->head = vs;
    this->head->prev = NULL;
    goto theend;
  }

  if (this->num_items is 1) {
    res = 1;
    vs->prev = this->head;
    this->tail = vs;
    this->tail->next = NULL;
    this->head->next = vs;
    goto theend;
  }

  res = strcmp (bytes, this->tail->data->bytes);

  if (0 < res) {
    this->tail->next = vs;
    vs->prev = this->tail;
    this->tail = vs;
    this->tail->next = NULL;
    goto theend;
  } else ifnot (res) {
    goto theend;
  }

  vstring_t *it = this->head->next;

  while (it) {
    res = strcmp (bytes, it->data->bytes);
    ifnot (res) goto theend;

    if (0 > res) {
      it->prev->next = vs;
      it->prev->next->next = it;
      it->prev->next->prev = it->prev;
      it->prev = it->prev->next;
      it = it->prev;
      goto theend;
    }
    it = it->next;
  }

theend:
  if (res)
    this->num_items++;
  else {
    String.release (vs->data);
    free (vs);
  }

  return this;
}

static void vstring_append_uniq (Vstring_t *this, char *bytes) {
  vstring_t *it = this->head;
  while (it) {
    if (Cstring.eq (it->data->bytes, bytes)) return;
    it = it->next;
  }

  vstring_t *vs = Alloc (sizeof (vstring_t));
  vs->data = String.new_with (bytes);

  DListAppend (this, vs);
}

static char **vstring_shallow_copy (Vstring_t *vstr, char **array) {
  vstring_t *it = vstr->head;
  int idx = 0;
  while (it) {
    array[idx++] = it->data->bytes;
    it = it->next;
  }

  return array;
}

static vstring_t *vstring_pop_at (Vstring_t *vstr, int idx) {
  vstring_t *t = DListPopAt (vstr, vstring_t, idx);
  return t;
}

static void vstring_remove_at (Vstring_t *vstr, int idx) {
  vstring_t *t = vstring_pop_at (vstr, idx);
  vstring_release_item (t);
}

public vstring_T __init_vstring__ (void) {
  StringT = __init_string__ ();
  CstringT = __init_cstring__ ();

  return (vstring_T) {
    .self = (vstring_self) {
      .new = vstring_new,
      .dup = vstring_dup,
      .join = vstring_join,
      .clear = vstring_clear,
      .pop_at = vstring_pop_at,
      .append = vstring_append,
      .release = vstring_release,
      .new_item = vstring_new_item,
      .remove_at = vstring_remove_at,
      .append_with = vstring_append_with,
      .append_uniq = vstring_append_uniq,
      .prepend_with = vstring_prepend_with,
      .release_item = vstring_release_item,
      .shallow_copy = vstring_shallow_copy,
      .join_allocated = vstring_join_allocated,
      .append_with_fmt = vstring_append_with_fmt,
      .append_with_len = vstring_append_with_len,
      .current = (vstring_current_self) {
        .append_with = vstring_current_append_with,
        .prepend_with = vstring_current_prepend_with,
        .append_with_len = vstring_current_append_with_len
      },
      .add = (vstring_add_self) {
        .sort_and_uniq = vstring_add_sort_and_uniq
      },
      .to = (vstring_to_self) {
        .cstring = vstring_to_cstring
      },
      .get = (vstring_get_self) {
        .size = vstring_get_size
      }
    }
  };
}
