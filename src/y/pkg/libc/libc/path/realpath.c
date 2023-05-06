// provides: char *path_real (const char *, char[MAXLEN_PATH])
// requires: unistd/getcwd.c
// requires: unistd/readlink.c
// requires: string/str_eq.c
// requires: string/str_cat.c
// requires: string/str_copy.c
// requires: string/str_chr.c
// requires: string/str_chr_r.c
// requires: string/str_byte_move.c
// requires: string/mem_copy.c
// requires: string/bytelen.c
// requires: sys/lstat.c
// requires: sys/stat.h

/* Adapt realpath() from OpenBSD to this environment.
 *
 * While the algorithm is from the above implementation, we use our functions to do
 * the string processing work and getting working directory. */

/* $OpenBSD: realpath.c,v 1.13 2005/08/08 08:05:37 espie Exp $
 * Copyright (c) 2003 Constantin S. Svintsoff <kostik@iclub.nsu.ru>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must rei_produce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of the authors may not be used to endorse or promote
 * i_products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

char *path_real (const char *path, char resolved[MAXLEN_PATH]) {
  struct stat sb;
  char *p, *q, *s;
  size_t left_len, resolved_len;
  unsigned symlinks;
  int serrno, slen;
  char left[MAXLEN_PATH], next_token[MAXLEN_PATH], symlink[MAXLEN_PATH];

  serrno = sys_errno;
  symlinks = 0;

  if (path[0] == DIR_SEP) {
    resolved[0] = DIR_SEP;
    resolved[1] = '\0';

    if (path[1] == '\0')
      return (resolved);

    resolved_len = 1;
    left_len = str_copy (left, MAXLEN_PATH, path + 1, MAXLEN_PATH - 1);

  } else {
    if (sys_getcwd (resolved, MAXLEN_PATH) == -1) {
      sys_errno = ECANNOTGETCWD;
      str_copy (resolved, MAXLEN_PATH,  ".", 1);
      return NULL;
    }

    resolved_len = bytelen (resolved);
    left_len = str_copy (left, MAXLEN_PATH, path, MAXLEN_PATH -1);
  }

  if (left_len >= MAXLEN_PATH || resolved_len >= MAXLEN_PATH) {
    sys_errno = ENAMETOOLONG;
    return NULL;
  }

  /* Iterate over path components in `left' */
  while (left_len != 0) {
    /* Extract the next path component and adjust `left' and its length */
    p = str_chr (left, DIR_SEP);
    s = p ? p : left + left_len;

    if (s - left >= MAXLEN_PATH) {
      sys_errno = ENAMETOOLONG;
      return NULL;
    }

    mem_copy (next_token, left, s - left);

    next_token[s - left] = '\0';
    left_len -= s - left;

    if (p != NULL)
      str_byte_move (left, MAXLEN_PATH, 0, s + 1 - left, left_len + 1);

    if (resolved[resolved_len - 1] != DIR_SEP) {
      if (resolved_len + 1 >= MAXLEN_PATH)  {
        sys_errno = ENAMETOOLONG;
        return NULL;
      }

      resolved[resolved_len++] = DIR_SEP;
      resolved[resolved_len] = '\0';
    }

    if (next_token[0] == '\0')
      continue;
    else if (str_eq (next_token, "."))
      continue;
    else if (str_eq (next_token, "..")) {
      /* Strip the last path component except when we have single "/" */
      if (resolved_len > 1) {
        resolved[resolved_len - 1] = '\0';
        q = str_chr_r (resolved, DIR_SEP) + 1;
        *q = '\0';
        resolved_len = q - resolved;
      }
      continue;
    }

    /* Append the next path component and lstat() it. If lstat() fails we still
     * can return successfully if there are no more path components left */
    resolved_len = str_cat (resolved, MAXLEN_PATH, next_token);
    if (resolved_len >= MAXLEN_PATH) {
      sys_errno = ENAMETOOLONG;
      return NULL;
    }

    if (sys_lstat (resolved, &sb) != 0) {
      if (sys_errno == ENOENT && p == NULL) {
        sys_errno = serrno;
        return resolved;
      }

      return NULL ;
    }

    if (S_ISLNK(sb.st_mode)) {
      if (symlinks++ > MAXSYMLINKS) {
        sys_errno = ELOOP;
        return NULL;
      }

      slen = sys_readlink (resolved, symlink, MAXLEN_PATH - 1);
      if (slen < 0)
        return NULL;

      symlink[slen] = '\0';

      if (symlink[0] is DIR_SEP) {
        resolved[1] = 0;
        resolved_len =  1;
      } else if (resolved_len > 1) {
        /* Strip the last path component. */
        resolved[resolved_len - 1] = '\0';
        q = str_chr_r (resolved, DIR_SEP) + 1;
        *q = '\0';
        resolved_len = q - resolved;
      }

      /* If there are any path components left,then append them to symlink.
       * The result is placed in `left'. */
      if (p != NULL) {
        if (symlink[slen - 1] != DIR_SEP) {
          if (slen + 1 >= MAXLEN_PATH) {
            sys_errno = ENAMETOOLONG;
            return NULL;
          }

          symlink[slen] = DIR_SEP;
          symlink[slen + 1] = 0;
        }

        left_len = str_cat (symlink, MAXLEN_PATH, left);
        if (left_len >= MAXLEN_PATH) {
          sys_errno = ENAMETOOLONG;
          return NULL;
        }
      }

     left_len = str_copy (left, MAXLEN_PATH, symlink, MAXLEN_PATH - 1);
    }
  }

  /* Remove trailing slash except when the resolved pathname is a single "/" */
  if (resolved_len > 1 && resolved[resolved_len - 1] == DIR_SEP)
    resolved[resolved_len - 1] = '\0';

  return resolved;
}
