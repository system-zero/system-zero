#include <zc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>  /* for MAXSYMLINKS */

#include <dlist.h>
#include <libstring.h>
#include <libvstring.h>
#include <libcstring.h>
#include <libpath.h>

static  cstring_T CstringT;
#define Cstring   CstringT.self

static  vstring_T VstringT;
#define Vstring   VstringT.self

static char *path_basename (char *name) {
  ifnot (name) return name;
  char *p = Cstring.byte.null_in_str (name);
  if (p is NULL) p = name + bytelen (name) + 1;
  if (p - 1 is name and IS_DIR_SEP (*(p - 1)))
    return p - 1;

  while (p > name and IS_DIR_SEP (*(p - 1))) p--;
  while (p > name and IS_NOT_DIR_SEP (*(p - 1))) --p;
  if (p is name and IS_DIR_SEP (*p))
    return DIR_SEP_STR;
  return p;
}

/* ala SLang */
static char *path_basename_sans_extname (char *name) {
  char *bsnm = path_basename (name);
  ifnot (bsnm) return name;

  char *sp = bsnm;

  while (*(sp + 1) and *(sp + 1) isnot '.') sp++;

  size_t len = sp - bsnm + 1;
  char *buf = Alloc (len + 1);
  Cstring.cp (buf, len + 1, bsnm, len);
  return buf;
}

static char *path_extname (char *name) {
  ifnot (name) return name;
  char *p = Cstring.byte.null_in_str (name);
  if (p is NULL) p = name + bytelen (name) + 1;
  while (p > name and (*(p - 1) isnot '.')) --p;
  if (p is name) return "";
  p--;
  return p;
}

/* as a new c string (null terninated), any new created c strings
 * should be null byte terminated */
static char *path_dirname (char *name) {
  size_t len = bytelen (name);
  char *dname = NULL;
  if (name is NULL or 0 is len) {
    dname = Alloc (2); dname[0] = '.'; dname[1] = '\0';
    return dname;
  }

  char *sep = name + len - 1;

  /* trailing slashes */
  while (sep isnot name) {
    ifnot (IS_DIR_SEP (*sep)) break;
    sep--;
  }

  /* first found */
  while (sep isnot name) {
    if (IS_DIR_SEP (*sep)) break;
    sep--;
  }

  if (sep - 1 is name) goto theend;

  /* trim again */
  while (sep isnot name) {
    if (IS_NOT_DIR_SEP (*sep)) break;
    sep--;
  }

  if (sep is name) {
    dname = Alloc (2);
    dname[0] = (IS_DIR_SEP (*name)) ? DIR_SEP : '.'; dname[1] = '\0';
    return dname;
  }

theend:
  len = sep - name + 1;
  dname = Alloc (len + 1);
  Cstring.cp (dname, len + 1, name, len);

  return dname;
}

static int path_is_absolute (const char *path) {
  return IS_DIR_ABS (path);
}

/* Adapt realpath() from OpenBSD to this environment.
 *
 * While the algorithm is from the above implementation, we use our functions to do
 * the string processing work. */

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

static char *path_real (const char *path, char resolved[PATH_MAX]) {
  struct stat sb;
  char *p, *q, *s;
  size_t left_len, resolved_len;
  unsigned symlinks;
  int serrno, slen;
  char left[PATH_MAX], next_token[PATH_MAX], symlink[PATH_MAX];

  serrno = errno;
  symlinks = 0;

  if (path[0] is '/') {
    resolved[0] = '/';
    resolved[1] = '\0';

    if (path[1] == '\0')
      return (resolved);

    resolved_len = 1;
    left_len = Cstring.cp (left, PATH_MAX, path + 1, PATH_MAX - 1);
  } else {
    if (getcwd (resolved, PATH_MAX) is NULL) {
      Cstring.cp (resolved, PATH_MAX,  ".", 1);
      return NULL;
    }

    resolved_len = bytelen (resolved);
    left_len = Cstring.cp (left, PATH_MAX, path, PATH_MAX -1);
  }

  if (left_len >= PATH_MAX or resolved_len >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  /* Iterate over path components in `left' */
  while (left_len isnot 0) {
    /* Extract the next path component and adjust `left' and its length */
    p = Cstring.byte.in_str (left, '/');
    s = p ? p : left + left_len;

    if (s - left >= PATH_MAX) {
      errno = ENAMETOOLONG;
      return NULL;
    }

    /* in the case of ../../tmp/../home/../usr/lib/../../home/../tmp/a  and in the
     * last iteration (s - left) gives 16 bytes to copy, when it is just one [a].
     * memcpy() does what it told to do, so copies more than one byte, but since the
     * second byte in "a" is the null byte, the function works, but the statement:
     * next_token[s - left] = '\0'; does it wrong.
     * So i had to introduce byte_cp_all, that behaves as memcpy behaves.
     * The next line is the original implementation
     * memcpy (next_token, left, s - left); 
     */
    Cstring.byte.cp_all (next_token, left, s - left);
    next_token[s - left] = '\0';
    left_len -= s - left;

    if (p isnot NULL)
      Cstring.byte.mv (left, PATH_MAX, 0, s + 1 - left, left_len + 1);

    if (resolved[resolved_len - 1] isnot '/') {
      if (resolved_len + 1 >= PATH_MAX)  {
        errno = ENAMETOOLONG;
        return NULL;
      }

      resolved[resolved_len++] = '/';
      resolved[resolved_len] = '\0';
    }

    if (next_token[0] is '\0')
      continue;
    else if (Cstring.eq (next_token, "."))
      continue;
    else if (Cstring.eq (next_token, "..")) {
      /* Strip the last path component except when we have single "/" */
      if (resolved_len > 1) {
        resolved[resolved_len - 1] = '\0';
        q = Cstring.byte.in_str_r (resolved, '/') + 1;
        *q = '\0';
        resolved_len = q - resolved;
      }
      continue;
    }

    /* Append the next path component and lstat() it. If lstat() fails we still
     * can return successfully if there are no more path components left */
    resolved_len = Cstring.cat (resolved, PATH_MAX, next_token);
    if (resolved_len >= PATH_MAX) {
      errno = ENAMETOOLONG;
      return NULL;
    }

    if (lstat(resolved, &sb) isnot 0) {
      if (errno is ENOENT and p is NULL) {
        errno = serrno;
        return resolved;
      }
      return NULL ;
    }

    if (S_ISLNK(sb.st_mode)) {
      if (symlinks++ > MAXSYMLINKS) {
        errno = ELOOP;
        return NULL;
      }

      slen = readlink (resolved, symlink, PATH_MAX - 1);
      if (slen < 0)
        return NULL;

      symlink[slen] = '\0';
      if (symlink[0] is '/') {
        resolved[1] = 0;
        resolved_len =  1;
      } else if (resolved_len > 1) {
        /* Strip the last path component. */
        resolved[resolved_len - 1] = '\0';
        q = Cstring.byte.in_str_r (resolved, '/') + 1;
        *q = '\0';
        resolved_len = q - resolved;
      }

      /* If there are any path components left,then append them to symlink.
       * The result is placed in `left'. */
      if (p isnot NULL) {
        if (symlink[slen - 1] isnot '/') {
          if (slen + 1 >= PATH_MAX) {
            errno = ENAMETOOLONG;
            return NULL;
          }

          symlink[slen] = '/';
          symlink[slen + 1] = 0;
        }
        left_len = Cstring.cat (symlink, PATH_MAX, left);
        if (left_len >= PATH_MAX) {
          errno = ENAMETOOLONG;
          return NULL;
        }
      }
     left_len = Cstring.cp (left, PATH_MAX, symlink, PATH_MAX - 1);
    }
  }

  /* Remove trailing slash except when the resolved pathname is a single "/" */
  if (resolved_len > 1 and resolved[resolved_len - 1] is '/')
    resolved[resolved_len - 1] = '\0';

  return resolved;
}

static Vstring_t *path_split (const char *path) {
  if (NULL is path) return NULL;

  size_t len = bytelen (path);
  char lpath[len + 1];
  Cstring.cp (lpath, len + 1, path, len);

  char *sp = lpath;
  char *end = sp + len - 1;

  Vstring_t *p = Vstring.new ();

  for (;;) {
    char *basename = path_basename (sp);

    if (*end is DIR_SEP and end isnot lpath)
      *end = '\0'; /* remove the trailing slash ifnot the first one */

    Vstring.prepend_with (p, basename);

    if (sp is basename) break;

    *basename = '\0';
    end = basename - 1;
    sp = lpath;
  }

  return p;
}

public path_T __init_path__ (void) {
  CstringT = __init_cstring__ ();
  VstringT = __init_vstring__ ();

  return (path_T) {
    .self = (path_self) {
      .real = path_real,
      .split = path_split,
      .extname = path_extname,
      .dirname = path_dirname,
      .basename = path_basename,
      .is_absolute = path_is_absolute,
      .basename_sans_extname = path_basename_sans_extname,
    }
  };
}

