#ifndef DListAppend
#define DListAppend(list, node)                        \
({                                                      \
  if ((list)->head is NULL) {                          \
    (list)->head = (node);                             \
    (list)->tail = (node);                             \
    (list)->current = (list)->head;                    \
    (list)->cur_idx = 0;                               \
  } else {                                             \
    (list)->tail->next = (node);                       \
    (node)->prev = (list)->tail;                       \
    (node)->next = NULL;                               \
    (list)->tail = (node);                             \
  }                                                    \
                                                       \
  (list)->num_items++;                                 \
})
#endif /* DListAppend */

#ifndef DListPush
#define DListPush(list, node)                          \
({                                                     \
  if ((list)->head == NULL) {                          \
    (list)->head = (node);                             \
    (list)->tail = (node);                             \
    (list)->current = (node);                          \
    (list)->head->next = NULL;                         \
    (list)->head->prev = NULL;                         \
  } else {                                             \
    (list)->head->prev = (node);                       \
    (node)->next = (list)->head;                       \
    (list)->head = (node);                             \
  }                                                    \
                                                       \
  (list)->num_items++;                                 \
  list;                                                \
})
#endif /* DListPush */

#ifndef DListInsertAt
#define DListInsertAt(list_, node_, idx_)              \
({                                                     \
  int __idx__ = idx_;                                  \
  do {                                                 \
    if (0 > __idx__) __idx__ += (list_)->num_items;    \
    if (__idx__ < 0 or __idx__ >= (list_)->num_items) {\
      __idx__ = INDEX_ERROR;                           \
      break;                                           \
    }                                                  \
    int cur_idx_ = (list_)->cur_idx;                   \
    DListSetCurrent(list_, __idx__);                   \
    DListAppendCurrent(list_, node_);                  \
    cur_idx_ += (cur_idx_ > __idx__ ? 1 : 0);          \
    DListSetCurrent(list_, cur_idx_);                  \
  } while (0);                                         \
  __idx__;                                             \
})
#endif /* DListInsertAt */

#ifndef DListPopTail
#define DListPopTail(list, type)                       \
({                                                     \
type *node = NULL;                                     \
do {                                                   \
  if ((list)->tail is NULL) break;                     \
  node = (list)->tail;                                 \
  (list)->tail->prev->next = NULL;                     \
  (list)->tail = (list)->tail->prev;                   \
  (list)->num_items--;                                 \
} while (0);                                           \
  node;                                                \
})
#endif /* DListPopTail */

#ifndef DListPopCurrent
#define DListPopCurrent(list, type)                    \
({                                                     \
type *node = NULL;                                     \
do {                                                   \
  if ((list)->current is NULL) break;                  \
  node = (list)->current;                              \
  if (1 is (list)->num_items) {                        \
    (list)->head = NULL;                               \
    (list)->tail = NULL;                               \
    (list)->current = NULL;                            \
    break;                                             \
  }                                                    \
  if (0 is (list)->cur_idx) {                          \
    (list)->current = (list)->current->next;           \
    (list)->current->prev = NULL;                      \
    (list)->head = (list)->current;                    \
    break;                                             \
  }                                                    \
  if ((list)->cur_idx is (list)->num_items - 1) {      \
    (list)->current = (list)->current->prev;           \
    (list)->current->next = NULL;                      \
    (list)->cur_idx--;                                 \
    (list)->tail = (list)->current;                    \
    break;                                             \
  }                                                    \
  (list)->current->next->prev = (list)->current->prev; \
  (list)->current->prev->next = (list)->current->next; \
  (list)->current = (list)->current->next;             \
} while (0);                                           \
  if (node isnot NULL) (list)->num_items--;            \
  node;                                                \
})
#endif /* DListPopCurrent */

#ifndef DListSetCurrent
#define DListSetCurrent(list, idx_)                    \
({                                                     \
  int idx__ = idx_;                                    \
  do {                                                 \
    if (0 > idx__) idx__ += (list)->num_items;         \
    if (idx__ < 0 or idx__ >= (list)->num_items) {     \
      idx__ = INDEX_ERROR;                             \
      break;                                           \
    }                                                  \
    if (idx__ is (list)->cur_idx) break;               \
    int idx___ = (list)->cur_idx;                      \
    (list)->cur_idx = idx__;                           \
    if (idx___ < idx__)                                \
      while (idx___++ < idx__)                         \
        (list)->current = (list)->current->next;       \
    else                                               \
      while (idx___-- > idx__)                         \
        (list)->current = (list)->current->prev;       \
  } while (0);                                         \
  idx__;                                               \
})
#endif /* DListSetCurrent */

#ifndef DListPopAt
#define DListPopAt(list, type, idx_)                   \
({                                                     \
  int cur_idx = (list)->cur_idx;                       \
  int __idx__ = DListSetCurrent (list, idx_);          \
  type *cnode = NULL;                                  \
  do {                                                 \
    if (__idx__ is INDEX_ERROR) break;                 \
    cnode = DListPopCurrent (list, type);              \
    if (cur_idx is __idx__) break;                     \
    if (cur_idx > __idx__) cur_idx--;                  \
    DListSetCurrent (list, cur_idx);                   \
  } while (0);                                         \
  cnode;                                               \
})
#endif /* DListPopAt */

#ifndef DListGetAt
#define DListGetAt(list_, type_, idx_)                 \
({                                                     \
  type_ *node = NULL;                                  \
  int idx__ = idx_;                                    \
  do {                                                 \
    if (0 > idx__) idx__ += (list_)->num_items;        \
    if (idx__ < 0 or idx__ >= (list_)->num_items) {    \
      idx__ = INDEX_ERROR;                             \
      break;                                           \
    }                                                  \
    if ((list_)->num_items / 2 < idx__) {              \
      node = (list_)->head;                            \
      while (idx__--)                                  \
        node = node->next;                             \
    } else {                                           \
      node = (list_)->tail;                            \
      while (idx__++ < (list_)->num_items - 1)         \
        node = node->prev;                             \
    }                                                  \
  } while (0);                                         \
  node;                                                \
})
#endif /* DListGetAt */

#ifndef DListGetIdx
#define DListGetIdx(list, type, node)                  \
({                                                     \
  int idx__ = INDEX_ERROR;                             \
  type *node__ = (list)->head;                         \
  if (list->num_items isnot 0 and NULL isnot node__) { \
    idx__ = -1;                                        \
    do {                                               \
      idx__++;                                         \
      if (node__ is node or node__ is (list)->tail)    \
        break;                                         \
      node__ = node__->next;                           \
    } while (1);                                       \
  }                                                    \
  idx__;                                               \
})
#endif /* DListGetIdx */

#ifndef DListPrependCurrent
#define DListPrependCurrent(list, node)                           \
({                                                                \
  if ((list)->current is NULL) {                                  \
    (list)->head = (node);                                        \
    (list)->tail = (node);                                        \
    (list)->cur_idx = 0;                                          \
    (list)->current = (list)->head;                               \
  } else {                                                        \
    if ((list)->cur_idx == 0) {                                   \
      (list)->head->prev = (node);                                \
      (node)->next = (list)->head;                                \
      (list)->head = (node);                                      \
      (list)->current = (list)->head;                             \
    } else {                                                      \
      (list)->current->prev->next = (node);                       \
      (list)->current->prev->next->next = (list)->current;        \
      (list)->current->prev->next->prev = (list)->current->prev;  \
      (list)->current->prev = (list)->current->prev->next;        \
      (list)->current = (list)->current->prev;                    \
    }                                                             \
  }                                                               \
                                                                  \
  (list)->num_items++;                                            \
  (list)->current;                                                \
})
#endif /* DListPrependCurrent */

#ifndef DListAppendCurrent
#define DListAppendCurrent(list, node)                 \
({                                                     \
  if ((list)->current is NULL) {                       \
    (list)->head = (node);                             \
    (list)->tail = (node);                             \
    (list)->cur_idx = 0;                               \
    (list)->current = (list)->head;                    \
  } else {                                             \
    if ((list)->cur_idx is (list)->num_items - 1) {    \
      (list)->current->next = (node);                  \
      (node)->prev = (list)->current;                  \
      (list)->current = (node);                        \
      (node)->next = NULL;                             \
      (list)->cur_idx++;                               \
      (list)->tail = (node);                           \
    } else {                                           \
      (node)->next = (list)->current->next;            \
      (list)->current->next = (node);                  \
      (node)->prev = (list)->current;                  \
      (node)->next->prev = (node);                     \
      (list)->current = (node);                        \
      (list)->cur_idx++;                               \
    }                                                  \
  }                                                    \
                                                       \
  (list)->num_items++;                                 \
  (list)->current;                                     \
})
#endif /* DListAppendCurrent */
