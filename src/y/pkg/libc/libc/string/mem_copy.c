// provides: void *mem_copy (void *, const void *, size_t)

/*-
 * Copyright (c) 1990, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * License should be included within the source directory of this unit
 */

/*
 * sizeof(iword) MUST BE A POWER OF TWO
 * SO THAT wmask BELOW IS ALL ONES
 */
typedef  int iword;    /* "iword" used for optimal copy speed */

#define  wsize  sizeof (iword)
#define  wmask  (wsize - 1)

/*
 * Copy a block of memory, handling overlap.
 * This is the routine that actually implements
 * (the portable versions of) bcopy, memcpy, and memmove.
 */

void *mem_copy (void *dst0, const void *src0, size_t length) {
  char *dst = dst0;
  const char *src = src0;
  size_t t;

  if (length == 0 || dst == src)
    goto done;

  if ((unsigned long) dst < (unsigned long) src) {
    /* Copy forward */
    t = (uintptr_t) src;  /* only need low bits */
    if ((t | (uintptr_t) dst) & wmask) {
      /*
       * Try to align operands.  This cannot be done
       * unless the low bits match.
       */
      if ((t ^ (uintptr_t) dst) & wmask || length < wsize)
        t = length;
      else
        t = wsize - (t & wmask);

      length -= t;

      do {
       *dst++ = *src++;
      } while (--t);
    }
    /*
     * Copy whole words, then mop up any trailing bytes.
     */
    t = length / wsize;
    if (t) {
      do {
        *(iword *) dst = *(iword *) src;
        src += wsize;
        dst += wsize;
      } while (--t);
    }

    t = length & wmask;

    if (t) {
      do {
        *dst++ = *src++;
      } while (--t);
    }
  } else {
    /*
     * Copy backwards.  Otherwise essentially the same.
     * Alignment works as before, except that it takes
     * (t&wmask) bytes to align, not wsize-(t&wmask).
     */
    src += length;
    dst += length;
    t = (uintptr_t) src;
    if ((t | (uintptr_t) dst) & wmask) {
      if ((t ^ (uintptr_t) dst) & wmask || length <= wsize)
        t = length;
      else
        t &= wmask;

      length -= t;

      do {
        *--dst = *--src;
      } while (--t);
    }

    t = length / wsize;
    if (t) {
      do {
        src -= wsize;
        dst -= wsize;
        *(iword *) dst = *(iword *) src;
      } while (--t);
    }

    t = length & wmask;
    if (t) {
      do {
        *--dst = *--src;
      } while (--t);
    }
  }

done:
  return dst0;
}
