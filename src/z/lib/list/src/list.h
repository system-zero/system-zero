#ifndef LIST_HDR
#define LIST_HDR

#ifndef DListAppend
#define DListAppend(list_, node_)                        \
({                                                       \
  if ((list_)->head == NULL) {                           \
    (list_)->head = (node_);                             \
    (list_)->tail = (node_);                             \
    (list_)->current = (list_)->head;                    \
    (list_)->cur_idx = 0;                                \
  } else {                                               \
    (list_)->tail->next = (node_);                       \
    (node_)->prev = (list_)->tail;                       \
    (node_)->next = NULL;                                \
    (list_)->tail = (node_);                             \
  }                                                      \
                                                         \
  (list_)->num_items++;                                  \
})
#endif /* DListAppend */

#ifndef DListPush
#define DListPush(list_, node_)                          \
({                                                       \
  if ((list_)->head == NULL) {                           \
    (list_)->head = (node_);                             \
    (list_)->tail = (node_);                             \
    (list_)->current = (node_);                          \
    (list_)->head->next = NULL;                          \
    (list_)->head->prev = NULL;                          \
  } else {                                               \
    (list_)->head->prev = (node_);                       \
    (list_)->current->prev = (node_);                    \
    (list_)->current = (node_);                          \
    (node_)->next = (list_)->head;                       \
    (list_)->head = (node_);                             \
  }                                                      \
                                                         \
  (list_)->num_items++;                                  \
  list_;                                                 \
})
#endif /* DListPush */

#ifndef DListPrepend
#define DListPrepend(list_, node_) DListPush(list_, node_)
#endif

#ifndef DListInsertAt
#define DListInsertAt(list_, node_, idx_)               \
({                                                      \
  int __idx__ = idx_;                                   \
  do {                                                  \
    if (0 > __idx__) __idx__ += (list_)->num_items;     \
    if (__idx__ < 0 || __idx__ >= (list_)->num_items) { \
      __idx__ = EINDEX;                                 \
      break;                                            \
    }                                                   \
    int cur_idx_ = (list_)->cur_idx;                    \
    DListSetCurrent(list_, __idx__);                    \
    DListAppendCurrent(list_, node_);                   \
    cur_idx_ += (cur_idx_ > __idx__ ? 1 : 0);           \
    DListSetCurrent(list_, cur_idx_);                   \
  } while (0);                                          \
  __idx__;                                              \
})
#endif /* DListInsertAt */

#ifndef DListPopTail
#define DListPopTail(list_, type_)                      \
({                                                      \
type_ *node_ = NULL;                                    \
do {                                                    \
  if ((list_)->tail == NULL) break;                     \
  node_ = (list_)->tail;                                \
  (list_)->tail->prev->next = NULL;                     \
  (list_)->tail = (list_)->tail->prev;                  \
  (list_)->num_items--;                                 \
} while (0);                                            \
  node_;                                                \
})
#endif /* DListPopTail */

#ifndef DListPopCurrent
#define DListPopCurrent(list_, type_)                   \
({                                                      \
type_ *node_ = NULL;                                    \
do {                                                    \
  if ((list_)->current == NULL) break;                  \
  node_ = (list_)->current;                             \
  if (1 == (list_)->num_items) {                        \
    (list_)->head = NULL;                               \
    (list_)->tail = NULL;                               \
    (list_)->current = NULL;                            \
    break;                                              \
  }                                                     \
  if (0 == (list_)->cur_idx) {                          \
    (list_)->current = (list_)->current->next;          \
    (list_)->current->prev = NULL;                      \
    (list_)->head = (list_)->current;                   \
    break;                                              \
  }                                                     \
  if ((list_)->cur_idx == (list_)->num_items - 1) {     \
    (list_)->current = (list_)->current->prev;          \
    (list_)->current->next = NULL;                      \
    (list_)->cur_idx--;                                 \
    (list_)->tail = (list_)->current;                   \
    break;                                              \
  }                                                     \
  (list_)->current->next->prev = (list_)->current->prev;\
  (list_)->current->prev->next = (list_)->current->next;\
  (list_)->current = (list_)->current->next;            \
} while (0);                                            \
  if (node_ != NULL) (list_)->num_items--;              \
  node_;                                                \
})
#endif /* DListPopCurrent */

#ifndef DListSetCurrent
#define DListSetCurrent(list_, idx_)                    \
({                                                      \
  int idx__ = idx_;                                     \
  do {                                                  \
    if (0 > idx__) idx__ += (list_)->num_items;         \
    if (idx__ < 0 || idx__ >= (list_)->num_items) {     \
      idx__ = EINDEX;                                   \
      break;                                            \
    }                                                   \
    if (idx__ == (list_)->cur_idx) break;               \
    int idx___ = (list_)->cur_idx;                      \
    (list_)->cur_idx = idx__;                           \
    if (idx___ < idx__)                                 \
      while (idx___++ < idx__)                          \
        (list_)->current = (list_)->current->next;      \
    else                                                \
      while (idx___-- > idx__)                          \
        (list_)->current = (list_)->current->prev;      \
  } while (0);                                          \
  idx__;                                                \
})
#endif /* DListSetCurrent */

#ifndef DListPopAt
#define DListPopAt(list_, type_, idx_)                  \
({                                                      \
  int cur_idx = (list_)->cur_idx;                       \
  int __idx__ = DListSetCurrent (list_, idx_);          \
  type_ *cnode_ = NULL;                                 \
  do {                                                  \
    if (__idx__ == EINDEX) break;                       \
    cnode_ = DListPopCurrent (list_, type_);            \
    if (cur_idx == __idx__) break;                      \
    if (cur_idx > __idx__) cur_idx--;                   \
    DListSetCurrent (list_, cur_idx);                   \
  } while (0);                                          \
  cnode_;                                               \
})
#endif /* DListPopAt */

#ifndef DListGetAt
#define DListGetAt(list_, type__, idx_)                 \
({                                                      \
  type__ *node_ = NULL;                                 \
  int idx__ = idx_;                                     \
  do {                                                  \
    if (0 > idx__) idx__ += (list_)->num_items;         \
    if (idx__ < 0 || idx__ >= (list_)->num_items) {     \
      idx__ = EINDEX;                                   \
      break;                                            \
    }                                                   \
    if ((list_)->num_items / 2 < idx__) {               \
      node_ = (list_)->head;                            \
      while (idx__--)                                   \
        node_ = node_->next;                            \
    } else {                                            \
      node_ = (list_)->tail;                            \
      while (idx__++ < (list_)->num_items - 1)          \
        node_ = node_->prev;                            \
    }                                                   \
  } while (0);                                          \
  node_;                                                \
})
#endif /* DListGetAt */

#ifndef DListGetIdx
#define DListGetIdx(list_, type_, node_)                \
({                                                      \
  int idx__ = EINDEX;                                   \
  type_ *node__ = (list_)->head;                        \
  if (list_->num_items != 0 && NULL != node__) {        \
    idx__ = -1;                                         \
    do {                                                \
      idx__++;                                          \
      if (node__ == node_ || node__ == (list_)->tail)   \
        break;                                          \
      node__ = node__->next;                            \
    } while (1);                                        \
  }                                                     \
  idx__;                                                \
})
#endif /* DListGetIdx */

#ifndef DListPrependCurrent
#define DListPrependCurrent(list_, node_)                         \
({                                                                \
  if ((list_)->current == NULL) {                                 \
    (list_)->head = (node_);                                      \
    (list_)->tail = (node_);                                      \
    (list_)->cur_idx = 0;                                         \
    (list_)->current = (list_)->head;                             \
  } else {                                                        \
    if ((list_)->cur_idx == 0) {                                  \
      (list_)->head->prev = (node_);                              \
      (node_)->next = (list_)->head;                              \
      (list_)->head = (node_);                                    \
      (list_)->current = (list_)->head;                           \
    } else {                                                      \
      (list_)->current->prev->next = (node_);                     \
      (list_)->current->prev->next->next = (list_)->current;      \
      (list_)->current->prev->next->prev = (list_)->current->prev;\
      (list_)->current->prev = (list_)->current->prev->next;      \
      (list_)->current = (list_)->current->prev;                  \
    }                                                             \
  }                                                               \
                                                                  \
  (list_)->num_items++;                                           \
  (list_)->current;                                               \
})
#endif /* DListPrependCurrent */

#ifndef DListAppendCurrent
#define DListAppendCurrent(list_, node_)               \
({                                                     \
  if ((list_)->current == NULL) {                      \
    (list_)->head = (node_);                           \
    (list_)->tail = (node_);                           \
    (list_)->cur_idx = 0;                              \
    (list_)->current = (list_)->head;                  \
  } else {                                             \
    if ((list_)->cur_idx == (list_)->num_items - 1) {  \
      (list_)->current->next = (node_);                \
      (node_)->prev = (list_)->current;                \
      (list_)->current = (node_);                      \
      (node_)->next = NULL;                            \
      (list_)->cur_idx++;                              \
      (list_)->tail = (node_);                         \
    } else {                                           \
      (node_)->next = (list_)->current->next;          \
      (list_)->current->next = (node_);                \
      (node_)->prev = (list_)->current;                \
      (node_)->next->prev = (node_);                   \
      (list_)->current = (node_);                      \
      (list_)->cur_idx++;                              \
    }                                                  \
  }                                                    \
                                                       \
  (list_)->num_items++;                                \
  (list_)->current;                                    \
})
#endif /* DListAppendCurrent */

#ifndef ListStackFree
#define ListStackFree(list_, type_)                    \
do {                                                   \
  type_ *item_ = (list_)->head;                        \
  while (item_ != NULL) {                              \
    type_ *tmp_ = item_->next;                         \
    free (item_);                                      \
    item_ = tmp_;                                      \
  }                                                    \
} while (0)
#endif /* ListStackFree */

#ifndef ListStackAppend
#define ListStackAppend(list_, type_, node_)           \
({                                                     \
  type_ *item_ = (node_);                              \
  while (item_ && item_->next) item_ = item_->next;    \
  item_->next = NULL;                                  \
  item_ = (list_)->head;                               \
  if (item_ == NULL) {                                 \
    (list_)->head = (node_);                           \
  } else {                                             \
    while (item_->next != NULL) item_ = item_->next;   \
    item_->next = (node_);                             \
  }                                                    \
  (list_);                                             \
})
#endif /* ListStackAppend */

#ifndef ListStackPush
#define ListStackPush(list_, node_)                    \
({                                                     \
  if ((list_)->head == NULL) {                         \
    (list_)->head = (node_);                           \
    (list_)->head->next = NULL;                        \
  } else {                                             \
    (node_)->next = (list_)->head;                     \
    (list_)->head = (node_);                           \
  }                                                    \
                                                       \
 (list_);                                              \
})
#endif /* ListStackPush */

#ifndef ListStackPop
#define ListStackPop(list_, type_)                     \
({                                                     \
  type_ *node_ = (list_)->head;                        \
  if (node_ != NULL)                                   \
    (list_)->head = (list_)->head->next;               \
                                                       \
  node_;                                               \
})
#endif /* ListStackPop */

#ifndef ListStackPopTail
#define ListStackPopTail(list_, type_)                 \
({                                                     \
  type_ *node_ = (list_)->head;                        \
  type_ *tmp_ = NULL;                                  \
  while (node_->next) {                                \
    tmp_ = node_;                                      \
    node_ = node_->next;                               \
  }                                                    \
  if (tmp_) tmp_->next = NULL;                         \
  node_;                                               \
})
#endif /* ListStackPopTail */

#endif /* LIST_HDR */
