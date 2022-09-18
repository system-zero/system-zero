#define REQUIRE_STDIO
#define REQUIRE_CTYPE

#include <z/cenv.h>

#include <z/par.h>

typedef par_t Me;

/* 
  this is an almost untouchable copy all of the original par code actual algorithm,
  merged into a single unit and the functionality exposed as a library

  we use an instance and pass this to the functions as their first argument,
  and we exposed this without hiding anything from the user (for flexibity)

  however (for now) only the functions that expose functionality, changed their
  signature

  this is to keep as much of the original code with the exact way it was written,
  so nothing has been removed (except the duplicated headers)

  but the main function has been removed

  however it is possible to use the (perhaps known) exact same syntax par's command
  line interface, passed as an array of strings to the exposed function (so to mimic
  the original behavior)

  Usage:
    par_t *par_new (void);
     void  par_release (par_t *);
     void  par_parsearg (par_t *, const char *);
      int  par_process (par_t *, const char *);
*/

/* Development:

  handle failures with a less brutal way (some of them are like this)

  define more if not all of the options with default values

  and save the output to a string instead of sending it to stdout

  and many details ...
*/

/*
  see par.doc for documantation and many many details
  As an inner expression, i've been very lucky:
    - to read this very old code
    - to see it compiled by modern compilers in C11 without a single warning
      and turning on agrressive warnings
    - for the so many detailed comments
    - to read the documantation and laugh with the author about his apologies
      i'm totally sure that he should be very fine kind of a human being
    - and ... is very fast

  so I feel to owe a way too much appreciation for this gift, many thanks
*/


/*
Rights and Responsibilities

    The files listed in the Files List section above are Copyright
    (various years, see the copyright notice in each file)
    Adam M. Costello (henceforth "I", "me").

    I grant everyone ("you") permission to do whatever you like with
    these files, provided that if you modify them you take reasonable
    steps to avoid confusing or misleading people about who wrote the
    modified files (both you and I) or what version they are.  All my
    versions of Par will have version numbers consisting of only digits
    and periods, so you could distinguish your versions by including
    some other kind of character.

    I encourage you to send me your suggestions for improvements.  See
    the Bugs section for my address.

    Though I have tried to make sure that Par is free of bugs, I make no
    guarantees about its soundness.  Therefore, I am not responsible for
    any damage resulting from the use of these files.

    You may alternatively use these files under the MIT License:

        Permission is hereby granted, free of charge, to any person
        obtaining a copy of this software and associated documentation
        files (the "Software"), to deal in the Software without
        restriction, including without limitation the rights to use,
        copy, modify, merge, publish, distribute, sublicense, and/or
        sell copies of the Software, and to permit persons to whom
        the Software is furnished to do so, subject to the following
        conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
        KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
        WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
        AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
        HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
        WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
        FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
        OTHER DEALINGS IN THE SOFTWARE.
*/

/*
last touched in Par 1.53.0
last meaningful change in Par 1.31
Copyright 1993 Adam M. Costello

This is ANSI C code (C89).

*/

/* par.c */

#ifndef errmsg_size
#define errmsg_size 163
#endif

/* This is the maximum number of characters that will  */
/* fit in an errmsg_t, including the terminating '\0'. */
/* It will never decrease, but may increase in future  */
/* versions of this header file.                       */

typedef char errmsg_t[errmsg_size];

/* Any function which takes the argument errmsg_t errmsg must, before */
/* returning, either set errmsg[0] to '\0' (indicating success), or   */
/* write an error message string into errmsg, (indicating failure),   */
/* being careful not to overrun the space.                            */

const char * const outofmem =
  "Out of memory.\n";

const char * const impossibility =
  "Impossibility #%d has occurred.  Please report it.\n";

typedef struct buffer buffer;

static buffer *newbuffer(size_t itemsize, errmsg_t errmsg);
  /* newbuffer(itemsize,errmsg) returns a pointer to a    */
  /* new empty buffer which holds items of size itemsize. */
  /* itemsize must not be 0.  Returns NULL on failure.    */

static void freebuffer(buffer *buf);
  /* freebuffer(buf) frees the memory associated with */
  /* *buf.  buf may not be used after this call.      */

static void clearbuffer(buffer *buf);
  /* clearbuffer(buf) removes  */
  /* all items from *buf, but  */
  /* does not free any memory. */

static void additem(buffer *buf, const void *item, errmsg_t errmsg);
  /* additem(buf,item,errmsg) copies *item to the end of     */
  /* *buf.  item must point to an object of the proper size  */
  /* for *buf.  If additem() fails, *buf will be unaffected. */

static int numitems(buffer *buf);
  /* numitems(buf) returns the number of items in *buf. */

static void *copyitems(buffer *buf, errmsg_t errmsg);
  /* copyitems(buf,errmsg) returns an array of objects of */
  /* the proper size for *buf, one for each item in *buf, */
  /* or NULL if there are no items in buf.  The elements  */
  /* of the array are copied from the items in *buf, in   */
  /* order.  The array is allocated with malloc(), so it  */
  /* may be freed with free().  Returns NULL on failure.  */

static void *nextitem(buffer *buf);
  /* When buf was created by newbuffer, a pointer associated with buf  */
  /* was initialized to point at the first slot in *buf.  If there is  */
  /* an item in the slot currently pointed at, nextitem(buf) advances  */
  /* the pointer to the next slot and returns the old value.  If there */
  /* is no item in the slot, nextitem(buf) leaves the pointer where it */
  /* is and returns NULL.                                              */

/* UNUSED */
#if 0
static void rewindbuffer(buffer *buf);
#endif
  /* rewindbuffer(buf) resets the pointer used by   */
  /* nextitem() to point at the first slot in *buf. */

/* charset.h
Note: Those functions declared here which do not use errmsg
always succeed, provided that they are passed valid arguments.
*/

typedef struct charset charset;

static charset *parsecharset(const char *str, errmsg_t errmsg);
  /* parsecharset(str,errmsg) returns the set of characters defined by */
  /* str using charset syntax (see par.doc).  Returns NULL on failure. */

static void freecharset(charset *cset);
  /* freecharset(cset) frees any memory associated with */
  /* *cset.  cset may not be used after this call.      */

static int csmember(char c, const charset *cset);
  /* csmember(c,cset) returns 1 if c is a member of *cset, 0 otherwise. */

static charset *csunion(const charset *cset1, const charset *cset2, errmsg_t errmsg);
  /* csunion(cset1,cset2) returns a pointer to the   */
  /* union of *cset1 and *cset2, or NULL on failure. */

static charset *csdiff(const charset *cset1, const charset *cset2, errmsg_t errmsg);
  /* csdiff(cset1,cset2) returns a pointer to the set */
  /* difference *cset1 - *cset2 , or NULL on failure. */

static void csadd(charset *cset1, const charset *cset2, errmsg_t errmsg);
  /* csadd(cset1,cset2) adds the members of *cset2  */
  /* to *cset1.  On failure, *cset1 is not changed. */

static void csremove(charset *cset1, const charset *cset2, errmsg_t errmsg);
  /* csremove(cset1,cset2) removes the members of *cset2 */
  /* from *cset1.  On failure, *cset1 is not changed.    */

/* UNUSED */
#if 0
static charset *cscopy(const charset *cset, errmsg_t errmsg);
#endif
  /* cscopy(cset) returns a copy of cset, or NULL on failure. */

static void csswap(charset *cset1, charset *cset2);
  /* csswap(cset1,cset2) swaps the contents of *cset1 and *cset2. */

/* reformat.h */

static char **reformat(
  const char * const *inlines, const char * const *endline, int afp, int fs,
  int hang, int prefix, int suffix, int width, int cap, int fit, int guess,
  int just, int last, int Report, int touch, const charset *terminalchars,
  errmsg_t errmsg
);
  /* inlines is an array of pointers to input lines, up to but not  */
  /* including endline.  inlines and endline must not be equal.     */
  /* terminalchars is the set of terminal characters as described   */
  /* in "par.doc".  The other parameters are variables described in */
  /* "par.doc".  reformat(inlines, endline, afp, fs, hang, prefix,  */
  /* suffix, width, cap, fit, guess, just, last, Report, touch,     */
  /* terminalchars, errmsg) returns a NULL-terminated array of      */
  /* pointers to output lines containing the reformatted paragraph, */
  /* according to the specification in "par.doc".  None of the      */
  /* integer parameters may be negative.  Returns NULL on failure.  */

/*
buffer.c

additem(), copyitems(), and nextitem() rely on the fact that
sizeof (char) is 1.  See section A7.4.8 of The C Programming
Language, Second Edition, by Kerninghan and Ritchie.

*/

struct buffer {
  struct block *firstblk, /* The first block.                    */
               *current,  /* The last non-empty block, or        */
                          /* firstblk if all are empty.          */
               *nextblk;  /* The block containing the item to be */
                          /* returned by nextitem(), or NULL.    */
  int nextindex;          /* Index of item in nextblock->items.  */
  size_t itemsize;        /* The size of an item.                */
};

typedef struct block {
  struct block *next;  /* The next block, or NULL if none.              */
  void *items;         /* Storage for the items in this block.          */
  int maxhere,         /* Number of items that fit in *items.           */
      numprevious,     /* Total of numhere for all previous blocks.     */
      numhere;         /* The first numhere slots in *items are filled. */
} block;

static buffer *newbuffer(size_t itemsize, errmsg_t errmsg)
{
  buffer *buf;
  block *blk;
  void *items;
  int maxhere;

  maxhere = 124 / itemsize;
  if (maxhere < 4) maxhere = 4;

  buf = malloc(sizeof (buffer));
  blk = malloc(sizeof (block));
  items = malloc(maxhere * itemsize);
  if (!buf || !blk || !items) {
    strcpy(errmsg,outofmem);
    goto nberror;
  }

  buf->itemsize = itemsize;
  buf->firstblk = buf->current = buf->nextblk = blk;
  buf->nextindex = 0;
  blk->next = NULL;
  blk->numprevious = blk->numhere = 0;
  blk->maxhere = maxhere;
  blk->items = items;

  *errmsg = '\0';
  return buf;

nberror:

  if (buf) free(buf);
  if (blk) free(blk);
  if (items) free(items);
  return NULL;
}

static void freebuffer(buffer *buf)
{
  block *blk, *tmp;

  blk = buf->firstblk;
  while (blk) {
    tmp = blk;
    blk = blk->next;
    if (tmp->items) free(tmp->items);
    free(tmp);
  }

  free(buf);
}

static void clearbuffer(buffer *buf)
{
  block *blk;

  for (blk = buf->firstblk;  blk;  blk = blk->next)
    blk->numhere = 0;

  buf->current = buf->firstblk;
}

static void additem(buffer *buf, const void *item, errmsg_t errmsg)
{
  block *blk, *new;
  void *items;
  int maxhere;
  size_t itemsize = buf->itemsize;

  blk = buf->current;

  if (blk->numhere == blk->maxhere) {
    new = blk->next;
    if (!new) {
      maxhere = 2 * blk->maxhere;
      new = malloc(sizeof (block));
      items = malloc(maxhere * itemsize);
      if (!new || !items) {
        strcpy(errmsg,outofmem);
        goto aierror;
      }
      blk->next = new;
      new->next = NULL;
      new->maxhere = maxhere;
      new->numprevious = blk->numprevious + blk->numhere;
      new->numhere = 0;
      new->items = items;
    }
    blk = buf->current = new;
  }

  memcpy( ((char *) blk->items) + (blk->numhere * itemsize), item, itemsize );

  ++blk->numhere;

  *errmsg = '\0';
  return;

aierror:

  if (new) free(new);
  if (items) free(items);
}

static int numitems(buffer *buf)
{
  block *blk = buf->current;
  return blk->numprevious + blk->numhere;
}

static void *copyitems(buffer *buf, errmsg_t errmsg)
{
  int n;
  void *r;
  block *blk, *b;
  size_t itemsize = buf->itemsize;

  b = buf->current;
  n = b->numprevious + b->numhere;
  if (!n) return NULL;

  r = malloc(n * itemsize);
  if (!r) {
    strcpy(errmsg,outofmem);
    return NULL;
  }

  b = b->next;

  for (blk = buf->firstblk;  blk != b;  blk = blk->next)
    memcpy( ((char *) r) + (blk->numprevious * itemsize),
            blk->items, blk->numhere * itemsize);

  *errmsg = '\0';
  return r;
}

/* UNUSED */
#if 0
static void rewindbuffer(buffer *buf)
{
  buf->nextblk = buf->firstblk;
  buf->nextindex = 0;
}
#endif

static void *nextitem(buffer *buf)
{
  void *r;

  if (!buf->nextblk || buf->nextindex >= buf->nextblk->numhere)
    return NULL;

  r = ((char *) buf->nextblk->items) + (buf->nextindex * buf->itemsize);

  if (++buf->nextindex >= buf->nextblk->maxhere) {
    buf->nextblk = buf->nextblk->next;
    buf->nextindex = 0;
  }

  return r;
}

/*
reformat.c

The issues regarding char and unsigned char are relevant to the use of
the ctype.h functions.  See the comments near the beginning of par.c.

*/

typedef unsigned char wflag_t;

typedef struct word {
  const char *chrs;       /* Pointer to the characters in the word */
                          /* (NOT terminated by '\0').             */
  struct word *prev,      /* Pointer to previous word.             */
              *next,      /* Pointer to next word.                 */
                          /* Supposing this word were the first... */
              *nextline;  /*   Pointer to first word in next line. */
  int score,              /*   Value of the objective function.    */
      length;             /* Length of this word.                  */
  wflag_t flags;          /* Notable properties of this word.      */
} word;

/* The following may be bitwise-OR'd together */
/* to set the flags field of a word:          */

static const wflag_t
  W_SHIFTED = 1,  /* This word should have an extra space before */
                  /* it unless it's the first word in the line.  */
  W_CURIOUS = 2,  /* This is a curious word (see par.doc).       */
  W_CAPITAL = 4;  /* This is a capitalized word (see par.doc).   */

#define isshifted(w) ( (w)->flags & 1)
#define iscurious(w) (((w)->flags & 2) != 0)
#define iscapital(w) (((w)->flags & 4) != 0)

static int checkcapital(word *w)
/* Returns 1 if *w is capitalized according to the definition */
/* in par.doc (assuming <cap> is 0), or 0 if not.             */
{
  const char *p, *end;

  for (p = w->chrs, end = p + w->length;
       p < end && !isalnum(*(unsigned char *)p);
       ++p);
  return p < end && !islower(*(unsigned char *)p);
}

static int checkcurious(word *w, const charset *terminalchars)
/* Returns 1 if *w is curious according to */
/* the definition in par.doc, or 0 if not. */
{
  const char *start, *p;
  char ch;

  for (start = w->chrs, p = start + w->length;  p > start;  --p) {
    ch = p[-1];
    if (isalnum(*(unsigned char *)&ch)) return 0;
    if (csmember(ch,terminalchars)) break;
  }

  if (p <= start + 1) return 0;

  --p;
  do if (isalnum(*(unsigned char *)--p)) return 1;
  while (p > start);

  return 0;
}

static int simplebreaks(word *head, word *tail, int L, int last)

/* Chooses line breaks in a list of words which maximize the length of the   */
/* shortest line.  L is the maximum line length.  The last line counts as a  */
/* line only if last is non-zero. _head must point to a dummy word, and tail */
/* must point to the last word, whose next field must be NULL.  Returns the  */
/* length of the shortest line on success, -1 if there is a word of length   */
/* greater than L, or L if there are no lines.                               */
{
  word *w1, *w2;
  int linelen, score;

  if (!head->next) return L;

  for (w1 = tail, linelen = w1->length;
       w1 != head && linelen <= L;
       linelen += isshifted(w1), w1 = w1->prev, linelen += 1 + w1->length) {
    w1->score = last ? linelen : L;
    w1->nextline = NULL;
  }

  for ( ;  w1 != head;  w1 = w1->prev) {
    w1->score = -1;
    for (linelen = w1->length,  w2 = w1->next;
         linelen <= L;
         linelen += 1 + isshifted(w2) + w2->length,  w2 = w2->next) {
      score = w2->score;
      if (linelen < score) score = linelen;
      if (score >= w1->score) {
        w1->nextline = w2;
        w1->score = score;
      }
    }
  }

  return head->next->score;
}

static void normalbreaks(
  word *head, word *tail, int L, int fit, int last, errmsg_t errmsg
)
/* Chooses line breaks in a list of words according to the policy   */
/* in "par.doc" for <just> = 0 (L is <L>, fit is <fit>, and last is */
/* <last>).  head must point to a dummy word, and tail must point   */
/* to the last word, whose next field must be NULL.                 */
{
  word *w1, *w2;
  int tryL, shortest, score, target, linelen, extra, minlen;

  *errmsg = '\0';
  if (!head->next) return;

  target = L;

/* Determine minimum possible difference between  */
/* the lengths of the shortest and longest lines: */

  if (fit) {
    score = L + 1;
    for (tryL = L;  ;  --tryL) {
      shortest = simplebreaks(head,tail,tryL,last);
      if (shortest < 0) break;
      if (tryL - shortest < score) {
        target = tryL;
        score = target - shortest;
      }
    }
  }

/* Determine maximum possible length of the shortest line: */

  shortest = simplebreaks(head,tail,target,last);
  if (shortest < 0) {
    sprintf(errmsg,impossibility,1);
    return;
  }

/* Minimize the sum of the squares of the differences */
/* between target and the lengths of the lines:       */

  w1 = tail;
  do {
    w1->score = -1;
    for (linelen = w1->length,  w2 = w1->next;
         linelen <= target;
         linelen += 1 + isshifted(w2) + w2->length,  w2 = w2->next) {
      extra = target - linelen;
      minlen = shortest;
      if (w2)
        score = w2->score;
      else {
        score = 0;
        if (!last) extra = minlen = 0;
      }
      if (linelen >= minlen  &&  score >= 0) {
        score += extra * extra;
        if (w1->score < 0  ||  score <= w1->score) {
          w1->nextline = w2;
          w1->score = score;
        }
      }
      if (!w2) break;
    }
    w1 = w1->prev;
  } while (w1 != head);

  if (head->next->score < 0)
    sprintf(errmsg,impossibility,2);
}

static void justbreaks(
  word *head, word *tail, int L, int last, errmsg_t errmsg
)
/* Chooses line breaks in a list of words according to the  */
/* policy in "par.doc" for <just> = 1 (L is <L> and last is */
/* <last>).  head must point to a dummy word, and tail must */
/* point to the last word, whose next field must be NULL.   */
{
  word *w1, *w2;
  int numgaps, extra, score, gap, maxgap, numbiggaps;

  *errmsg = '\0';
  if (!head->next) return;

/* Determine the minimum possible largest inter-word gap: */

  w1 = tail;
  do {
    w1->score = L;
    for (numgaps = 0, extra = L - w1->length, w2 = w1->next;
         extra >= 0;
         ++numgaps, extra -= 1 + isshifted(w2) + w2->length, w2 = w2->next) {
      gap = numgaps ? (extra + numgaps - 1) / numgaps : L;
      if (w2)
        score = w2->score;
      else {
        score = 0;
        if (!last) gap = 0;
      }
      if (gap > score) score = gap;
      if (score < w1->score) {
        w1->nextline = w2;
        w1->score = score;
      }
      if (!w2) break;
    }
    w1 = w1->prev;
  } while (w1 != head);

  maxgap = head->next->score;
  if (maxgap >= L) {
    strcpy(errmsg, "Cannot justify.\n");
    return;
  }

/* Minimize the sum of the squares of the numbers   */
/* of extra spaces required in each inter-word gap: */

  w1 = tail;
  do {
    w1->score = -1;
    for (numgaps = 0, extra = L - w1->length, w2 = w1->next;
         extra >= 0;
         ++numgaps, extra -= 1 + isshifted(w2) + w2->length, w2 = w2->next) {
      gap = numgaps ? (extra + numgaps - 1) / numgaps : L;
      if (w2)
        score = w2->score;
      else {
        if (!last) {
          w1->nextline = NULL;
          w1->score = 0;
          break;
        }
        score = 0;
      }
      if (gap <= maxgap && score >= 0) {
        numbiggaps = extra % numgaps;
        score += (extra / numgaps) * (extra + numbiggaps) + numbiggaps;
        /* The above may not look like the sum of the squares of the numbers */
        /* of extra spaces required in each inter-word gap, but trust me, it */
        /* is.  It's easier to prove graphically than algebraicly.           */
        if (w1->score < 0  ||  score <= w1->score) {
          w1->nextline = w2;
          w1->score = score;
        }
      }
      if (!w2) break;
    }
    w1 = w1->prev;
  } while (w1 != head);

  if (head->next->score < 0)
    sprintf(errmsg,impossibility,3);
}

static char **reformat(
  const char * const *inlines, const char * const *endline, int afp, int fs,
  int hang, int prefix, int suffix, int width, int cap, int fit, int guess,
  int just, int last, int Report, int touch, const charset *terminalchars,
  errmsg_t errmsg
)
{
  int numin, affix, L, onfirstword = 1, linelen, numout, numgaps, extra, phase;
  const char * const *line, **suffixes = NULL, **suf, *end, *p1, *p2;
  char *q1, *q2, **outlines = NULL;
  word dummy, *head, *tail, *w1, *w2;
  buffer *pbuf = NULL;

/* Initialization: */

  *errmsg = '\0';
  dummy.next = dummy.prev = NULL;
  dummy.flags = 0;
  head = tail = &dummy;
  numin = endline - inlines;
  if (numin <= 0) {
    sprintf(errmsg,impossibility,4);
    goto rfcleanup;
  }
  numgaps = extra = 0;  /* unnecessary, but quiets compiler warnings */

/* Allocate space for pointers to the suffixes: */

  suffixes = malloc(numin * sizeof (const char *));
  if (!suffixes) {
    strcpy(errmsg,outofmem);
    goto rfcleanup;
  }

/* Set the pointers to the suffixes, and create the words: */

  affix = prefix + suffix;
  L = width - prefix - suffix;

  line = inlines, suf = suffixes;
  do {
    for (end = *line;  *end;  ++end);
    if (end - *line < affix) {
      sprintf(errmsg,
              "Line %ld shorter than <prefix> + <suffix> = %d + %d = %d\n",
              (long)(line - inlines + 1), prefix, suffix, affix);
      goto rfcleanup;
    }
    end -= suffix;
    *suf = end;
    p1 = *line + prefix;
    for (;;) {
      while (p1 < end && *p1 == ' ') ++p1;
      if (p1 == end) break;
      p2 = p1;
      if (onfirstword) {
        p1 = *line + prefix;
        onfirstword = 0;
      }
      while (p2 < end && *p2 != ' ') ++p2;
      w1 = malloc(sizeof (word));
      if (!w1) {
        strcpy(errmsg,outofmem);
        goto rfcleanup;
      }
      w1->next = NULL;
      w1->prev = tail;
      tail = tail->next = w1;
      w1->chrs = p1;
      w1->length = p2 - p1;
      w1->flags = 0;
      p1 = p2;
    }
    ++line, ++suf;
  } while (line < endline);

/* If guess is 1, set flag values and merge words: */

  if (guess) {
    for (w1 = head, w2 = head->next;  w2;  w1 = w2, w2 = w2->next) {
      if (checkcurious(w2,terminalchars)) w2->flags |= W_CURIOUS;
      if (cap || checkcapital(w2)) {
        w2->flags |= W_CAPITAL;
        if (iscurious(w1)) {
          if (w1->chrs[w1->length] && w1->chrs + w1->length + 1 == w2->chrs) {
            w2->length += w1->length + 1;
            w2->chrs = w1->chrs;
            w2->prev = w1->prev;
            w2->prev->next = w2;
            if (iscapital(w1)) w2->flags |= W_CAPITAL;
            else w2->flags &= ~W_CAPITAL;
            if (isshifted(w1)) w2->flags |= W_SHIFTED;
            else w2->flags &= ~W_SHIFTED;
            free(w1);
          }
          else w2->flags |= W_SHIFTED;
        }
      }
    }
    tail = w1;
  }

/* Check for too-long words: */

  if (Report)
    for (w2 = head->next;  w2;  w2 = w2->next) {
      if (w2->length > L) {
        linelen = w2->length;
        if (linelen > errmsg_size - 17)
          linelen = errmsg_size - 17;
        sprintf(errmsg, "Word too long: %.*s\n", linelen, w2->chrs);
        goto rfcleanup;
      }
    }
  else
    for (w2 = head->next;  w2;  w2 = w2->next)
      while (w2->length > L) {
        w1 = malloc(sizeof (word));
        if (!w1) {
          strcpy(errmsg,outofmem);
          goto rfcleanup;
        }
        w1->next = w2;
        w1->prev = w2->prev;
        w1->prev->next = w1;
        w2->prev = w1;
        w1->chrs = w2->chrs;
        w2->chrs += L;
        w1->length = L;
        w2->length -= L;
        w1->flags = 0;
        if (iscapital(w2)) {
          w1->flags |= W_CAPITAL;
          w2->flags &= ~W_CAPITAL;
        }
        if (isshifted(w2)) {
          w1->flags |= W_SHIFTED;
          w2->flags &= ~W_SHIFTED;
        }
      }

/* Choose line breaks according to policy in "par.doc": */

  if (just) justbreaks(head,tail,L,last,errmsg);
  else normalbreaks(head,tail,L,fit,last,errmsg);
  if (*errmsg) goto rfcleanup;

/* Change L to the length of the longest line if required: */

  if (!just && touch) {
    L = 0;
    w1 = head->next;
    while (w1) {
      for (linelen = w1->length, w2 = w1->next;
           w2 != w1->nextline;
           linelen += 1 + isshifted(w2) + w2->length, w2 = w2->next);
      if (linelen > L) L = linelen;
      w1 = w2;
    }
  }

/* Construct the lines: */

  pbuf = newbuffer(sizeof (char *), errmsg);
  if (*errmsg) goto rfcleanup;

  numout = 0;
  w1 = head->next;
  while (numout < hang || w1) {
    if (w1)
      for (w2 = w1->next, numgaps = 0, extra = L - w1->length;
           w2 != w1->nextline;
           ++numgaps, extra -= 1 + isshifted(w2) + w2->length, w2 = w2->next);
    linelen = suffix || (just && (w2 || last)) ?
                L + affix :
                w1 ? prefix + L - extra : prefix;
    q1 = malloc((linelen + 1) * sizeof (char));
    if (!q1) {
      strcpy(errmsg,outofmem);
      goto rfcleanup;
    }
    additem(pbuf, &q1, errmsg);
    if (*errmsg) goto rfcleanup;
    ++numout;
    q2 = q1 + prefix;
    if      (numout <= numin) memcpy(q1, inlines[numout - 1], prefix);
    else if (numin  >  hang ) memcpy(q1, endline[-1],         prefix);
    else {
      if (afp > prefix) afp = prefix;
      memcpy(q1, endline[-1], afp);
      q1 += afp;
      while (q1 < q2) *q1++ = ' ';
    }
    q1 = q2;
    if (w1) {
      phase = numgaps / 2;
      for (w2 = w1;  ;  ) {
        memcpy(q1, w2->chrs, w2->length);
        q1 += w2->length;
        w2 = w2->next;
        if (w2 == w1->nextline) break;
        *q1++ = ' ';
        if (just && (w1->nextline || last)) {
          phase += extra;
          while (phase >= numgaps) {
            *q1++ = ' ';
            phase -= numgaps;
          }
        }
        if (isshifted(w2)) *q1++ = ' ';
      }
    }
    q2 += linelen - affix;
    while (q1 < q2) *q1++ = ' ';
    q2 = q1 + suffix;
    if      (numout <= numin) memcpy(q1, suffixes[numout - 1], suffix);
    else if (numin  >  hang ) memcpy(q1, suffixes[numin  - 1], suffix);
    else {
      if (fs > suffix) fs = suffix;
      memcpy(q1, suffixes[numin - 1], fs);
      q1 += fs;
      while(q1 < q2) *q1++ = ' ';
    }
    *q2 = '\0';
    if (w1) w1 = w1->nextline;
  }

  q1 = NULL;
  additem(pbuf, &q1, errmsg);
  if (*errmsg) goto rfcleanup;

  outlines = copyitems(pbuf,errmsg);

rfcleanup:

  if (suffixes) free(suffixes);

  while (tail != head) {
    tail = tail->prev;
    free(tail->next);
  }

  if (pbuf) {
    if (!outlines)
      for (;;) {
        outlines = nextitem(pbuf);
        if (!outlines) break;
        free(*outlines);
      }
    freebuffer(pbuf);
  }

  return outlines;
}

/* charset.c */

typedef unsigned char csflag_t;

struct charset {
  char *inlist;    /* Characters in inlist are in the set.                */
  char *outlist;   /* Characters in outlist are not in the set.           */
                   /* inlist and outlist must have no common characters.  */
                   /* inlist and outlist may be NULL, which acts like "". */
  csflag_t flags;  /* Characters in neither list are in the set if they   */
                   /* belong to any of the classes indicated by flags.    */
};

/* The following may be bitwise-OR'd together */
/* to set the flags field of a charset:       */

static const csflag_t
  CS_UCASE = 1,   /* Includes all upper case letters.   */
  CS_LCASE = 2,   /* Includes all lower case letters.   */
  CS_NCASE = 4,   /* Includes all neither case letters. */
  CS_DIGIT = 8,   /* Includes all decimal digits.       */
  CS_SPACE = 16,  /* Includes all space characters.     */
  CS_NUL   = 32;  /* Includes the NUL character.        */


static int appearsin(char c, const char *str)
/* Returns 0 if c is '\0' or str is NULL or c     */
/* does not appear in *str.  Otherwise returns 1. */
{
  return c && str && strchr(str,c);
}

static int hexdigtoint(char c)
/* Returns the value represented by the hexadecimal */
/* digit c, or -1 if c is not a hexadecimal digit.  */
{
  const char *p, * const hexdigits = "0123456789ABCDEFabcdef";
  int n;

  if (!c) return -1;
  p = strchr(hexdigits, *(unsigned char *)&c);
  if (!p) return -1;
  n = p - hexdigits;
  if (n >= 16) n -= 6;
  return n;

  /* We can't do things like c - 'A' because we can't */
  /* depend on the order of the characters in ANSI C. */
  /* Nor can we do things like hexdigtoint[c] because */
  /* we don't know how large such an array might be.  */
}


static charset *parsecharset(const char *str, errmsg_t errmsg)
{
  charset *cset = NULL;
  buffer *cbuf = NULL;
  const char *p, * const singleescapes = "_sbqQx";
  int hex1, hex2;
  char ch;

  cset = malloc(sizeof (charset));
  if (!cset) {
    strcpy(errmsg,outofmem);
    goto pcserror;
  }
  cset->inlist = cset->outlist = NULL;
  cset->flags = 0;

  cbuf = newbuffer(sizeof (char), errmsg);
  if (*errmsg) goto pcserror;

  for (p = str;  *p;  ++p)
    if (*p == '_') {
      ++p;
      if (appearsin(*p, singleescapes)) {
        if      (*p == '_') ch = '_' ;
        else if (*p == 's') ch = ' ' ;
        else if (*p == 'b') ch = '\\';
        else if (*p == 'q') ch = '\'';
        else if (*p == 'Q') ch = '\"';
        else /*  *p == 'x'  */ {
          hex1 = hexdigtoint(p[1]);
          hex2 = hexdigtoint(p[2]);
          if (hex1 < 0  ||  hex2 < 0) goto pcsbadstr;
          *(unsigned char *)&ch = 16 * hex1 + hex2;
          p += 2;
        }
        if (!ch)
          cset->flags |= CS_NUL;
        else {
          additem(cbuf, &ch, errmsg);
          if (*errmsg) goto pcserror;
        }
      }
      else {
        if      (*p == 'A') cset->flags |= CS_UCASE;
        else if (*p == 'a') cset->flags |= CS_LCASE;
        else if (*p == '@') cset->flags |= CS_NCASE;
        else if (*p == '0') cset->flags |= CS_DIGIT;
        else if (*p == 'S') cset->flags |= CS_SPACE;
        else goto pcsbadstr;
      }
    }
    else {
      additem(cbuf,p,errmsg);
      if (*errmsg) goto pcserror;
    }
  ch = '\0';
  additem(cbuf, &ch, errmsg);
  if (*errmsg) goto pcserror;
  cset->inlist = copyitems(cbuf,errmsg);
  if (*errmsg) goto pcserror;

pcscleanup:

  if (cbuf) freebuffer(cbuf);
  return cset;

pcsbadstr:

  sprintf(errmsg, "Bad charset syntax: %.*s\n", errmsg_size - 22, str);

pcserror:

  if (cset) freecharset(cset);
  cset = NULL;
  goto pcscleanup;
}

static void freecharset(charset *cset)
{
  if (cset->inlist) free(cset->inlist);
  if (cset->outlist) free(cset->outlist);
  free(cset);
}

static int csmember(char c, const charset *cset)
{
  unsigned char uc;

  if (appearsin(c, cset->inlist )) return 1;
  if (appearsin(c, cset->outlist)) return 0;
  uc = *(unsigned char *)&c;

  /* The logic for the CS_?CASE flags is a little convoluted,  */
  /* but avoids calling islower() or isupper() more than once. */

  if (cset->flags & CS_NCASE) {
    if ( isalpha(uc) &&
         (cset->flags & CS_LCASE || !islower(uc)) &&
         (cset->flags & CS_UCASE || !isupper(uc))    ) return 1;
  }
  else {
    if ( (cset->flags & CS_LCASE && islower(uc)) ||
         (cset->flags & CS_UCASE && isupper(uc))    ) return 1;
  }

  return (cset->flags & CS_DIGIT && isdigit(uc)) ||
         (cset->flags & CS_SPACE && isspace(uc)) ||
         (cset->flags & CS_NUL   && !c         )    ;
}

static charset *csud(
  int u, const charset *cset1, const charset *cset2, errmsg_t errmsg
)
/* Returns the union of cset1 and cset2 if u is 1, or the set    */
/* difference cset1 - cset2 if u is 0.  Returns NULL on failure. */
{
  charset *csu;
  buffer *inbuf = NULL, *outbuf = NULL;
  char *lists[4], **list, *p, nullchar = '\0';

  csu = malloc(sizeof (charset));
  if (!csu) {
    strcpy(errmsg,outofmem);
    goto csuderror;
  }
  inbuf = newbuffer(sizeof (char), errmsg);
  if (*errmsg) goto csuderror;
  outbuf = newbuffer(sizeof (char), errmsg);
  if (*errmsg) goto csuderror;
  csu->inlist = csu->outlist = NULL;
  csu->flags =  u  ?  cset1->flags |  cset2->flags
                   :  cset1->flags & ~cset2->flags;

  lists[0] = cset1->inlist;
  lists[1] = cset1->outlist;
  lists[2] = cset2->inlist;
  lists[3] = cset2->outlist;

  for (list = lists;  list < lists + 4;  ++list)
    if (*list) {
      for (p = *list;  *p;  ++p)
        if (u  ?  csmember(*p, cset1) ||  csmember(*p, cset2)
               :  csmember(*p, cset1) && !csmember(*p, cset2)) {
          if (!csmember(*p, csu)) {
            additem(inbuf,p,errmsg);
            if (*errmsg) goto csuderror;
          }
        }
        else
          if (csmember(*p, csu)) {
            additem(outbuf,p,errmsg);
            if (*errmsg) goto csuderror;
          }
    }

  additem(inbuf, &nullchar, errmsg);
  if (*errmsg) goto csuderror;
  additem(outbuf, &nullchar, errmsg);
  if (*errmsg) goto csuderror;
  csu->inlist = copyitems(inbuf,errmsg);
  if (*errmsg) goto csuderror;
  csu->outlist = copyitems(outbuf,errmsg);
  if (*errmsg) goto csuderror;

csudcleanup:

  if (inbuf) freebuffer(inbuf);
  if (outbuf) freebuffer(outbuf);
  return csu;

csuderror:

  if (csu) freecharset(csu);
  csu = NULL;
  goto csudcleanup;
}

static charset *csunion(const charset *cset1, const charset *cset2, errmsg_t errmsg)
{
  return csud(1,cset1,cset2,errmsg);
}

static charset *csdiff(const charset *cset1, const charset *cset2, errmsg_t errmsg)
{
  return csud(0,cset1,cset2,errmsg);
}

static void csadd(charset *cset1, const charset *cset2, errmsg_t errmsg)
{
  charset *csu;

  csu = csunion(cset1,cset2,errmsg);
  if (*errmsg) return;
  csswap(csu,cset1);
  freecharset(csu);
}

static void csremove(charset *cset1, const charset *cset2, errmsg_t errmsg)
{
  charset *csu;

  csu = csdiff(cset1,cset2,errmsg);
  if (*errmsg) return;
  csswap(csu,cset1);
  freecharset(csu);
}

/* UNUSED */
#if 0
static charset *cscopy(const charset *cset, errmsg_t errmsg)
{
  charset emptycharset = { NULL, NULL, 0 };

  return csunion(cset, &emptycharset, errmsg);
}
#endif

static void csswap(charset *cset1, charset *cset2)
{
  charset tmp;

  tmp = *cset1;
  *cset1 = *cset2;
  *cset2 = tmp;
}
/*===

Regarding char and unsigned char:  ANSI C is a nightmare in this
respect.  Some functions, like puts(), strchr(), and getenv(), use char
or char*, so they work well with character constants like 'a', which
are char, and with argv, which is char**.  But several other functions,
like getchar(), putchar(), and isdigit(), use unsigned char (converted
to/from int).  Therefore innocent-looking code can be wrong, for
example:

    int c = getchar();
    if (c == 'a') ...

This is wrong because 'a' is char (converted to int) and could be
negative, but getchar() returns unsigned char (converted to int), so c
is always nonnegative or EOF.  For similar reasons, it is wrong to pass
a char to a function that expects an unsigned char:

    putchar('\n');
    if (isdigit(argv[1][0])) ...

Inevitably, we need to convert between char and unsigned char.  This can
be done by integral conversion (casting or assigning a char to unsigned
char or vice versa), or by aliasing (converting a pointer to char to
a pointer to unsigned char (or vice versa) and then dereferencing
it).  ANSI C requires that integral conversion alters the bits when the
unsigned value is not representable in the signed type and the signed
type does not use two's complement representation.  Aliasing, on the
other hand, preserves the bits.  Although the C standard is not at all
clear about which sort of conversion is appropriate for making the
standard library functions interoperate, I think preserving the bits
is what is needed.  Under that assumption, here are some examples of
correct code:

    int c = getchar();
    char ch;

    if (c != EOF) {
      *(unsigned char *)&ch = c;
      if (ch == 'a') ...
      if (isdigit(c)) ...
    }

    char *s = ...
    if (isdigit(*(unsigned char *)s)) ...

===*/

/* Structure for recording properties of lines within segments: */

typedef unsigned char lflag_t;

typedef struct lineprop {
  short p, s;     /* Length of the prefix and suffix of a bodiless */
                  /* line, or the fallback prelen and suflen       */
                  /* of the IP containing a non-bodiless line.     */
  lflag_t flags;  /* Boolean properties (see below).               */
  char rc;        /* The repeated character of a bodiless line.    */
} lineprop;

/* Flags for marking boolean properties: */

static const lflag_t L_BODILESS = 1,  /* Bodiless line.             */
                     L_INSERTED = 2,  /* Inserted by quote.         */
                     L_FIRST    = 4,  /* First line of a paragraph. */
                     L_SUPERF   = 8;  /* Superfluous line.          */

#define isbodiless(prop) ( (prop)->flags & 1)
#define isinserted(prop) (((prop)->flags & 2) != 0)
#define    isfirst(prop) (((prop)->flags & 4) != 0)
#define   issuperf(prop) (((prop)->flags & 8) != 0)
#define   isvacant(prop) (isbodiless(prop) && (prop)->rc == ' ')

static int digtoint(char c)
/* Returns the value represented by the digit c, or -1 if c is not a digit. */
{
  const char *p, * const digits = "0123456789";

  if (!c) return -1;
  p = strchr(digits,c);
  return  p  ?  p - digits  :  -1;

  /* We can't simply return c - '0' because this is ANSI C code,  */
  /* so it has to work for any character set, not just ones which */
  /* put the digits together in order.  Also, an array that could */
  /* be referenced as digtoint[c] might be bad because there's no */
  /* upper limit on CHAR_MAX.                                     */
}

static int strtoudec(const char *s, int *pn)
/* Converts the longest prefix of string s consisting of decimal   */
/* digits to an integer, which is stored in *pn.  Normally returns */
/* 1.  If *s is not a digit, then *pn is not changed, but 1 is     */
/* still returned.  If the integer represented is greater than     */
/* 9999, then *pn is not changed and 0 is returned.                */
{
  int n = 0, d;

  d = digtoint(*s);
  if (d < 0) return 1;

  do {
    if (n >= 1000) return 0;
    n = 10 * n + d;
    d = digtoint(*++s);
  } while (d >= 0);

  *pn = n;

  return 1;
}

static void compresuflen(
  const char * const *lines, const char * const *endline,
  const charset *bodychars, int body, int pre, int suf, int *ppre, int *psuf
)
/* lines is an array of strings, up to but not including endline.  */
/* Writes into *ppre and *psuf the comprelen and comsuflen of the  */
/* lines in lines.  Assumes that they have already been determined */
/* to be at least pre and suf.  endline must not equal lines.      */
{
  const char *start, *end, *knownstart, * const *line, *p1, *p2, *knownend,
             *knownstart2;

  start = *lines;
  end = knownstart = start + pre;
  if (body)
    while (*end) ++end;
  else
    while (*end && !csmember(*end, bodychars)) ++end;
  for (line = lines + 1;  line < endline;  ++line) {
    for (p1 = knownstart, p2 = *line + pre;
         p1 < end && *p1 == *p2;
         ++p1, ++p2);
    end = p1;
  }
  if (body)
    for (p1 = end;  p1 > knownstart;  )
      if (*--p1 != ' ') {
        if (csmember(*p1, bodychars))
          end = p1;
        else
          break;
      }
  *ppre = end - start;

  knownstart = *lines + *ppre;
  for (end = knownstart;  *end;  ++end);
  knownend = end - suf;
  if (body)
    start = knownstart;
  else
    for (start = knownend;
         start > knownstart && !csmember(start[-1], bodychars);
         --start);
  for (line = lines + 1;  line < endline;  ++line) {
    knownstart2 = *line + *ppre;
    for (p2 = knownstart2;  *p2;  ++p2);
    for (p1 = knownend, p2 -= suf;
         p1 > start && p2 > knownstart2 && p1[-1] == p2[-1];
         --p1, --p2);
    start = p1;
  }
  if (body) {
    for (p1 = start;
         start < knownend && (*start == ' ' || csmember(*start, bodychars));
         ++start);
    if (start > p1 && start[-1] == ' ') --start;
  }
  else
    while (end - start >= 2 && *start == ' ' && start[1] == ' ') ++start;
  *psuf = end - start;
}

static void delimit(
  const char * const *lines, const char * const *endline,
  const charset *bodychars, int repeat, int body, int div,
  int pre, int suf, lineprop *props
)
/* lines is an array of strings, up to but not including     */
/* endline.  Sets fields in each lineprop in the parallel    */
/* array props as appropriate, except for the L_SUPERF flag, */
/* which is never set.  It is assumed that the comprelen     */
/* and comsuflen of the lines in lines have already been     */
/* determined to be at least pre and suf, respectively.      */
{
  const char * const *line, *end, *p, * const *nextline;
  char rc;
  lineprop *prop, *nextprop;
  int anybodiless = 0, status;

  if (endline == lines) return;

  if (endline == lines + 1) {
    props->flags |= L_FIRST;
    props->p = pre, props->s = suf;
    return;
  }

  compresuflen(lines, endline, bodychars, body, pre, suf, &pre, &suf);

  line = lines, prop = props;
  do {
    prop->flags |= L_BODILESS;
    prop->p = pre, prop->s = suf;
    for (end = *line;  *end;  ++end);
    end -= suf;
    p = *line + pre;
    rc =  p < end  ?  *p  :  ' ';
    if (rc != ' ' && (isinserted(prop) || !repeat || end - p < repeat))
      prop->flags &= ~L_BODILESS;
    else
      while (p < end) {
        if (*p != rc) {
          prop->flags &= ~L_BODILESS;
          break;
        }
        ++p;
      }
    if (isbodiless(prop)) {
      anybodiless = 1;
      prop->rc = rc;
    }
    ++line, ++prop;
  } while (line < endline);

  if (anybodiless) {
    line = lines, prop = props;
    do {
      if (isbodiless(prop)) {
        ++line, ++prop;
        continue;
      }

      for (nextline = line + 1, nextprop = prop + 1;
           nextline < endline && !isbodiless(nextprop);
           ++nextline, ++nextprop);

      delimit(line,nextline,bodychars,repeat,body,div,pre,suf,prop);

      line = nextline, prop = nextprop;
    } while (line < endline);

    return;
  }

  if (!div) {
    props->flags |= L_FIRST;
    return;
  }

  line = lines, prop = props;
  status = ((*lines)[pre] == ' ');
  do {
    if (((*line)[pre] == ' ') == status)
      prop->flags |= L_FIRST;
    ++line, ++prop;
  } while (line < endline);
}

static void marksuperf(
  const char * const * lines, const char * const * endline, lineprop *props
)
/* lines points to the first line of a segment, and endline to one  */
/* line beyond the last line in the segment.  Sets L_SUPERF bits in */
/* the flags fields of the props array whenever the corresponding   */
/* line is superfluous.  L_BODILESS bits must already be set.       */
{
  const char * const *line, *p;
  lineprop *prop, *mprop, dummy;
  int inbody, num, mnum;

  for (line = lines, prop = props;  line < endline;  ++line, ++prop)
    if (isvacant(prop))
      prop->flags |= L_SUPERF;

  inbody = mnum = 0;
  mprop = &dummy;
  for (line = lines, prop = props;  line < endline;  ++line, ++prop)
    if (isvacant(prop)) {
      for (num = 0, p = *line;  *p;  ++p)
        if (*p != ' ') ++num;
      if (inbody || num < mnum)
        mnum = num, mprop = prop;
      inbody = 0;
    } else {
      if (!inbody) mprop->flags &= ~L_SUPERF;
      inbody = 1;
    }
}

static void setaffixes(
  const char * const *inlines, const char * const *endline,
  const lineprop *props, const charset *bodychars,
  const charset *quotechars, int hang, int body, int quote,
  int *pafp, int *pfs, int *pprefix, int *psuffix
)
/* inlines is an array of strings, up to but not including endline,    */
/* representing an IP.  inlines and endline must not be equal.  props  */
/* is the the parallel array of lineprop structures.  *pafp and *pfs   */
/* are set to the augmented fallback prelen and fallback suflen of the */
/* IP.  If either of *pprefix, *psuffix is less than 0, it is set to a */
/* default value as specified in "par.doc".                            */
{
  int numin, pre, suf;
  const char *p;

  numin = endline - inlines;

  if ((*pprefix < 0 || *psuffix < 0)  &&  numin > hang + 1)
    compresuflen(inlines + hang, endline, bodychars, body, 0, 0, &pre, &suf);

  p = *inlines + props->p;
  if (numin == 1 && quote)
    while (*p && csmember (*p, quotechars))
      ++p;
  *pafp = p - *inlines;
  *pfs = props->s;

  if (*pprefix < 0)
    *pprefix  =  numin > hang + 1  ?  pre  :  *pafp;

  if (*psuffix < 0)
    *psuffix  =  numin > hang + 1  ?  suf  :  *pfs;
}

static void freelines(char **lines)
/* Frees the elements of lines, and lines itself. */
/* lines is a NULL-terminated array of strings.   */
{
  char **line;

  for (line = lines;  *line;  ++line)
    free(*line);

  free(lines);
}

static char **par_readlines (Me *My, const char *input, int *num) {
/* Reads lines from stdin until EOF, or until a line beginning with a   */
/* protective character is encountered (in which case the protective    */
/* character is pushed back onto the input stream), or until a blank    */
/* line is encountered (in which case the newline is pushed back onto   */
/* the input stream).  Returns a NULL-terminated array of pointers to   */
/* individual lines, stripped of their newline characters.  Every NUL   */
/* character is stripped, and every white character is changed to a     */
/* space unless it is a newline.  If quote is 1, vacant lines will be   */
/* supplied as described for the q option in par.doc.  *pprops is set   */
/* to an array of lineprop structures, one for each line, each of whose */
/* flags field is either 0 or L_INSERTED (the other fields are 0).  If  */
/* there are no lines, *pprops is set to NULL.  The returned array may  */
/* be freed with freelines().  *pprops may be freed with free() if      */
/* it's not NULL.  On failure, returns NULL and sets *pprops to NULL.   */

  lineprop **pprops = &My->props;
  char *sp = (char *) input;

  buffer *cbuf = NULL, *lbuf = NULL, *lpbuf = NULL;
  int c, empty, blank, firstline, qsonly, oldqsonly = 0, vlnlen, i;
  char ch, *ln = NULL, nullchar = '\0', *nullline = NULL, *qpend,
       *oldln = NULL, *oldqpend = NULL, *p, *op, *vln = NULL, **lines = NULL;
  lineprop vprop = { 0, 0, 0, '\0' }, iprop = { 0, 0, 0, '\0' };

  /* oldqsonly, oldln, and oldquend don't really need to be initialized.   */
  /* They are initialized only to appease compilers that try to be helpful */
  /* by issuing warnings about unitialized automatic variables.            */

  iprop.flags = L_INSERTED;
  *My->errmsg = '\0';

  *pprops = NULL;

  cbuf = newbuffer (sizeof (char), My->errmsg);
  if (*My->errmsg) goto rlcleanup;

  lbuf = newbuffer (sizeof (char *), My->errmsg);
  if (*My->errmsg) goto rlcleanup;

  lpbuf = newbuffer (sizeof (lineprop), My->errmsg);
  if (*My->errmsg) goto rlcleanup;

  for (empty = blank = firstline = 1;  ;  ) {
    c = *sp++;
    if (!c) break;

    *(unsigned char *)&ch = c;

    if (ch == '\n') {
      if (blank) {
        sp--;
        break;
      }

      additem (cbuf, &nullchar, My->errmsg);
      if (*My->errmsg) goto rlcleanup;

      ln = copyitems (cbuf, My->errmsg);
      if (*My->errmsg) goto rlcleanup;

      if (My->quote) {
        for (qpend = ln;  *qpend && csmember (*qpend, My->quotechars);  ++qpend);
        for (p = qpend;  *p == ' ' || csmember (*p, My->quotechars);  ++p);
        qsonly =  *p == '\0';

        while (qpend > ln && qpend[-1] == ' ') --qpend;
        if (!firstline) {
          for (p = ln, op = oldln;
               p < qpend && op < oldqpend && *p == *op;
               ++p, ++op);

          if (!(p == qpend && op == oldqpend)) {
            if (!My->invis && (oldqsonly || qsonly)) {
              if (oldqsonly) {
                *op = '\0';
                oldqpend = op;
              }
              if (qsonly) {
                *p = '\0';
                qpend = p;
              }
            }
            else {
              vlnlen = p - ln;
              vln = malloc ((vlnlen + 1) * sizeof (char));
              if (!vln) {
                strncpy (My->errmsg, outofmem, errmsg_size);
                goto rlcleanup;
              }

              strncpy (vln, ln, vlnlen);
              vln[vlnlen] = '\0';

              additem (lbuf, &vln, My->errmsg);
              if (*My->errmsg) goto rlcleanup;

              additem (lpbuf, &iprop, My->errmsg);
              if (*My->errmsg) goto rlcleanup;
              vln = NULL;
            }
          }
        }

        oldln = ln;
        oldqpend = qpend;
        oldqsonly = qsonly;
      }

      additem (lbuf, &ln, My->errmsg);
      if (*My->errmsg) goto rlcleanup;
      ln = NULL;

      additem (lpbuf, &vprop, My->errmsg);
      if (*My->errmsg) goto rlcleanup;

      clearbuffer(cbuf);
      empty = blank = 1;
      firstline = 0;
    }
    else {
      if (empty) {
        if (csmember (ch, My->protectchars)) {
          sp--;
          break;
        }
        empty = 0;
      }

      if (!ch) continue;

      if (ch == '\t') {
        ch = ' ';
        for (i = My->Tab - numitems (cbuf) % My->Tab;  i > 0;  --i) {
          additem (cbuf, &ch, My->errmsg);
          if (*My->errmsg) goto rlcleanup;
        }
        continue;
      }

      if (csmember (ch, My->whitechars)) ch = ' ';
      else blank = 0;

      additem (cbuf, &ch, My->errmsg);
      if (*My->errmsg) goto rlcleanup;
    }
  }

  if (!blank) {
    additem (cbuf, &nullchar, My->errmsg);
    if (*My->errmsg) goto rlcleanup;

    ln = copyitems (cbuf, My->errmsg);
    if (*My->errmsg) goto rlcleanup;

    additem (lbuf, &ln, My->errmsg);
    if (*My->errmsg) goto rlcleanup;
    ln = NULL;

    additem (lpbuf, &vprop, My->errmsg);
    if (*My->errmsg) goto rlcleanup;
  }

  additem (lbuf, &nullline, My->errmsg);
  if (*My->errmsg) goto rlcleanup;

  *pprops = copyitems (lpbuf, My->errmsg);
  if (*My->errmsg) goto rlcleanup;

  lines = copyitems (lbuf, My->errmsg);

rlcleanup:

  if (cbuf) freebuffer(cbuf);
  if (lpbuf) freebuffer(lpbuf);
  if (lbuf) {
    if (!lines)
      for (;;) {
        lines = nextitem (lbuf);
        if (!lines) break;
        free (*lines);
      }

    freebuffer (lbuf);
  }

  if (ln) free(ln);
  if (vln) free(vln);

  *num = sp - input;
  return lines;
}

void par_parsearg (Me *My, const char *arg) {
/* Parses the command line argument in *arg, setting the objects pointed to */
/* by the other pointers as appropriate.  *phelp and *pversion are boolean  */
/* flags indicating whether the help and version options were supplied.     */
  const char *savearg = arg;
  charset *chars, *change;
  char oc;
  int n;

  *My->errmsg = '\0';

  if (*arg == '-') ++arg;

  chars =  *arg == 'B'  ?  My->bodychars     :
           *arg == 'P'  ?  My->protectchars  :
           *arg == 'Q'  ?  My->quotechars    :
           *arg == 'W'  ?  My->whitechars    :
           *arg == 'Z'  ?  My->terminalchars :
           NULL;

  if (chars) {
    ++arg;

    if (*arg != '='  &&  *arg != '+'  &&  *arg != '-') goto badarg;

    change = parsecharset (arg + 1, My->errmsg);

    if (change) {
      if      (*arg == '=')   csswap   (chars, change);
      else if (*arg == '+')   csadd    (chars, change, My->errmsg);
      else  /* *arg == '-' */ csremove (chars, change, My->errmsg);
      freecharset (change);
    }

    return;
  }

  if (isdigit(*(unsigned char *)arg)) {
    if (!strtoudec (arg, &n)) goto badarg;
    if (n <= 8) My->prefix = n;
    else My->width = n;
  }

  for (;;) {
    while (isdigit (*(unsigned char *)arg)) ++arg;

    oc = *arg;
    if (!oc) break;

    n = -1;
    if (!strtoudec (++arg, &n)) goto badarg;
    if (   oc == 'h' || oc == 'p' || oc == 'r'
        || oc == 's' || oc == 'T' || oc == 'w') {
      if      (oc == 'h')   My->hang   =  n >= 0 ? n :  1;
      else if (oc == 'p')   My->prefix =  n;
      else if (oc == 'r')   My->repeat =  n >= 0 ? n :  3;
      else if (oc == 's')   My->suffix =  n;
      else if (oc == 'T')   My->Tab    =  n >= 0 ? n : DEFAULT_TABWIDTH;
      else  /* oc == 'w' */ My->width  =  n >= 0 ? n : DEFAULT_WIDTH;
    }
    else {
      if (n < 0) n = 1;
      if (n > 1) goto badarg;
      if      (oc == 'b') My->body   = n;
      else if (oc == 'c') My->cap    = n;
      else if (oc == 'd') My->div    = n;
//      else if (oc == 'E') My->Err    = n;
      else if (oc == 'e') My->expel  = n;
      else if (oc == 'f') My->fit    = n;
      else if (oc == 'g') My->guess  = n;
      else if (oc == 'i') My->invis  = n;
      else if (oc == 'j') My->just   = n;
      else if (oc == 'l') My->last   = n;
      else if (oc == 'q') My->quote  = n;
      else if (oc == 'R') My->Report = n;
      else if (oc == 't') My->touch  = n;
      else goto badarg;
    }
  }

  return;

badarg:

  snprintf (My->errmsg, errmsg_size, "Bad argument: %.*s\n", errmsg_size - 16, savearg);
}

int par_process (Me *My, const char *input) {
  int sawnonblank, oweblank, n, i, afp, fs;
  char **endline, **firstline, *end, **nextline, **line;
  lineprop *firstprop, *nextprop;

  char *sp = (char *) input;

  char ch;
  int c;
  for (sawnonblank = oweblank = 0;  ;  ) {
    for (;;) {
      c = *sp++;
      if (!c) break;

      *(unsigned char *)&ch = c;
      if (My->expel && ch == '\n') {
        oweblank = sawnonblank;
        continue;
      }

      if (csmember (ch, My->protectchars)) {
        sawnonblank = 1;
        if (oweblank) {
          My->output_byte (My, '\n');
          oweblank = 0;
        }

        while (ch != '\n') {
          My->output_byte (My, c);

          c = *sp++;
          if (!c) break;

          *(unsigned char *)&ch = c;
        }
      }

      if (ch != '\n') break;  /* subsumes the case that c == EOF */

      My->output_byte (My, c);
    }

    if (!c) break;

    sp--;

    int num = 0;
    My->inlines = par_readlines (My, sp, &num);
    if (*My->errmsg) return -1;
    sp += num;

    for (endline = My->inlines;  *endline;  ++endline);
    if (endline == My->inlines) {
      free (My->inlines);
      My->inlines = NULL;
      continue;
    }

    sawnonblank = 1;
    if (oweblank) {
      My->output_byte (My, '\n');
      oweblank = 0;
    }

    delimit ((const char * const *) My->inlines,
            (const char * const *) endline,
            My->bodychars, My->repeat, My->body, My->div, 0, 0, My->props);

    if (My->expel)
      marksuperf((const char * const *) My->inlines,
                 (const char * const *) endline, My->props);

    firstline = My->inlines, firstprop = My->props;

    do {
      if (isbodiless (firstprop)) {
        if (   !(My->invis && isinserted (firstprop))
            && !(My->expel && issuperf (firstprop))) {
          for (end = *firstline;  *end;  ++end);
          if (!My->repeat || (firstprop->rc == ' ' && !firstprop->s)) {
            while (end > *firstline && end[-1] == ' ') --end;
            *end = '\0';
            My->output_line (My, *firstline);
          }
          else {
            n = My->width - firstprop->p - firstprop->s;
            if (n < 0) {
              snprintf (My->errmsg, errmsg_size, impossibility, 5);
              return -1;
            }

            for (int j = 0; j < firstprop->p; j++)
              My->output_byte (My, (*firstline)[j]);
            My->output_byte (My, '\n');

            //fprintf (stdout, "%.*s\n", firstprop->p, *firstline);

            for (i = n; i; --i)
              My->output_byte (My, *(unsigned char *)&firstprop->rc);

            My->output_line (My, end - firstprop->s);
          }
        }

        ++firstline, ++firstprop;
        continue;
      }

      for (nextline = firstline + 1, nextprop = firstprop + 1;
           nextline < endline && !isbodiless (nextprop) && !isfirst (nextprop);
           ++nextline, ++nextprop);

      My->prefix = My->prefixbak;
      My->suffix = My->suffixbak;

      setaffixes((const char * const *) firstline,
                 (const char * const *) nextline, firstprop, My->bodychars,
                 My->quotechars, My->hang, My->body, My->quote, &afp, &fs,
                 &My->prefix, &My->suffix);

      if (My->width <= My->prefix + My->suffix) {
        snprintf (My->errmsg, errmsg_size, "<width> (%d) <= <prefix> (%d) + <suffix> (%d)\n",
                My->width, My->prefix, My->suffix);
        return -1;
      }

      My->outlines =
        reformat ((const char * const *) firstline,
                 (const char * const *) nextline,
                 afp, fs, My->hang, My->prefix, My->suffix, My->width,
                 My->cap,
                 My->fit, My->guess, My->just, My->last, My->Report, My->touch,
                 (const charset *) My->terminalchars, My->errmsg);
      if (*My->errmsg) return -1;

      for (line = My->outlines;  *line;  ++line)
        My->output_line (My, *line);

      freelines (My->outlines);
      My->outlines = NULL;

      firstline = nextline, firstprop = nextprop;
    } while (firstline < endline);

    freelines (My->inlines);
    My->inlines = NULL;

    free (My->props);
    My->props = NULL;
  }

  return 0;
}

void par_release (Me *My) {
  if (My->bodychars)     freecharset (My->bodychars);
  if (My->protectchars)  freecharset (My->protectchars);
  if (My->quotechars)    freecharset (My->quotechars);
  if (My->whitechars)    freecharset (My->whitechars);
  if (My->terminalchars) freecharset (My->terminalchars);

  if (My->parinit)  free (My->parinit);
  if (My->props)    free (My->props);
  if (My->inlines)  freelines (My->inlines);
  if (My->outlines) freelines (My->outlines);
  free (My);
}

static int par_output_byte (Me *My, int c) {
  (void) My;
  return fprintf (stdout, "%c", (unsigned char) c);
}

static int par_output_line (Me *My, const char *s) {
  (void) My;
  return fprintf (stdout, "%s\n", s);
}

Me *par_new (void) {
  Me *My = malloc (sizeof (Me));
  (*My) = (Me) {
    .hang   = 0,
    .prefix = -1,
    .repeat = 0,
    .suffix = -1,
    .Tab    = DEFAULT_TABWIDTH,
    .width  = DEFAULT_WIDTH,
    .body   = 0,
    .cap    = 0,
    .div    = 0,
    .expel  = 0,
    .fit    = 0,
    .guess  = 0,
    .invis  = 0,
    .just   = DEFAULT_JUST,
    .last   = 0,
    .quote  = 0,
    .Report = 0,
    .touch = -1,
    .parinit = NULL,
    .inlines = NULL,
    .outlines = NULL,
    .bodychars = NULL,
    .protectchars = NULL,
    .quotechars = NULL,
    .whitechars = NULL,
    .terminalchars = NULL,
    .props = NULL,
    .errmsg = { '\0' },
    .output_byte = par_output_byte,
    .output_line = par_output_line,
    .userdata = NULL
  };

  const char *env;
  env = getenv ("PARBODY");
  if (!env) env = "";
  My->bodychars = parsecharset (env, My->errmsg);
  if (*My->errmsg) goto theerror;

  env = getenv ("PARPROTECT");
  if (!env) env = "";
  My->protectchars = parsecharset (env, My->errmsg);
  if (*My->errmsg) goto theerror;

  env = getenv ("PARQUOTE");
  if (!env) env = "> ";
  My->quotechars = parsecharset (env, My->errmsg);
  if (*My->errmsg) goto theerror;

  const char *const init_whitechars = " \f\n\r\t\v";

  My->whitechars = parsecharset (init_whitechars, My->errmsg);
  if (*My->errmsg) goto theerror;

  My->terminalchars = parsecharset (".?!:", My->errmsg);
  if (*My->errmsg) goto theerror;

  char *arg;

  env = getenv ("PARINIT");
  if (env) {
    size_t len = strlen (env);

    My->parinit = malloc ((len + 1) * sizeof (char));
    if (!My->parinit) {
      strncpy (My->errmsg, outofmem, errmsg_size);
      goto theerror;
    }

    strncpy (My->parinit, env, len + 1);

    arg = strtok (My->parinit, init_whitechars);
    while (arg) {
      par_parsearg (My, arg);
      if (*My->errmsg) goto theerror;

      arg = strtok (NULL, init_whitechars);
    }

    free (My->parinit);
    My->parinit = NULL;
  }

  if (My->Tab == 0) My->Tab = DEFAULT_TABWIDTH;

  if (My->touch < 0) My->touch = My->fit || My->last;

  My->prefixbak = My->prefix;
  My->suffixbak = My->suffix;

  return My;

theerror:
  fprintf (stderr, "%s\n", My->errmsg);
  par_release (My);
  return NULL;
}
