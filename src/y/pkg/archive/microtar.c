/* started from nicrotar: https://github.com/rxi/microtar
 * commit: 27076e1b9290e9c7842bb7890a54fcf172406c84
 *
 * Changes:
 *   - ustar compatible, that it has been implemented only on reading (for now)
 *   - recognize also as regular file types when type[0] is the nul byte ('\0')
 *   - use the prefix field if it has been set (under development)
 *   - steadily use own functions to adopt to the environment
 */

/*
 * Copyright (c) 2017 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#define  _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "microtar.h"

#ifndef DIR_SEP
//#define DIR_SEP         '/'
#endif

typedef struct {
  char name[MTAR_NAMELEN];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char type;
  char linkName[MTAR_NAMELEN];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char major[8];
  char minor[8];
  char prefix[MTAR_PREFIXLEN];
} mtar_raw_header_t;

static unsigned round_up(unsigned n, unsigned incr) {
  return n + (incr - n % incr) % incr;
}

static unsigned checksum(const mtar_raw_header_t* rh) {
  unsigned i;
  unsigned char *p = (unsigned char*) rh;
  unsigned res = 256;
  for (i = 0; i < offsetof(mtar_raw_header_t, checksum); i++) {
    res += p[i];
  }
  for (i = offsetof(mtar_raw_header_t, type); i < sizeof(*rh); i++) {
    res += p[i];
  }
  return res;
}

static int tread(mtar_t *tar, void *data, unsigned size) {
  int err = tar->read(tar, data, size);
  tar->pos += size;
  return err;
}

static int twrite(mtar_t *tar, const void *data, unsigned size) {
  int err = tar->write(tar, data, size);
  tar->pos += size;
  return err;
}

static int write_null_bytes(mtar_t *tar, int n) {
  int i, err;
  char nul = '\0';
  for (i = 0; i < n; i++) {
    err = twrite(tar, &nul, 1);
    if (err) {
      return err;
    }
  }
  return MTAR_ESUCCESS;
}

static int dir_is_directory (const char *dname) {
  struct stat st;
  if (-1 == lstat (dname, &st)) return 0;
  return S_ISDIR (st.st_mode);
}

static int raw_to_header(mtar_header_t *h, const mtar_raw_header_t *rh) {
  unsigned chksum1, chksum2;

  /* If the checksum starts with a null byte we assume the record is NULL */
  if (*rh->checksum == '\0') {
    return MTAR_ENULLRECORD;
  }

  /* Build and compare checksum */
  chksum1 = checksum(rh);
  sscanf(rh->checksum, "%o", &chksum2);

  if (chksum1 != chksum2) {
    return MTAR_EBADCHKSUM;
  }

  /* Load raw header into header */
  sscanf(rh->size,  "%o", &h->size);
  sscanf(rh->mode,  "%o", &h->mode);
  sscanf(rh->uid,   "%o", &h->uid);
  sscanf(rh->mtime, "%o", &h->mtime);
  h->type = rh->type;

  int i = 0;
  if (*rh->prefix != '\0') {
    for (; i < MTAR_PREFIXLEN; i++) {
      if (rh->prefix[i] == '\0') {
        char t[MTAR_PREFIXLEN];
        for (int idx = 0; idx < i; idx++) t[idx] = h->name[idx];
        t[i] = '\0';
        if (*rh->name != '/' && dir_is_directory (t))
          h->name[i++] = '/';

        break;
        // that block was to handle the following
      }
// prefix: libgit2-1.4.3/tests/resources/crlf_data/windows_to_odb/autocrlf_false,safecrlf_false,-crlf
// name  : all-lf-utf8bom
// produces:
// libgit2-1.4.3/tests/resources/crlf_data/windows_to_odb/autocrlf_false,safecrlf_false,-crlfall-lf-utf8bom
// though actually prefix and name should be concatenated with a directory separator
// so we have to add the separator by ourselves, so we've to check if [prefix] is a directory,
// but there isn't a gurrantee that the intent was really this (perhaps there is something i miss)
// note that here the prefix ends with 'f' and '\0' at 90 index, and name
// starts with 'a' so i do not see any directory separator, but in reality there
// is, as gnu-tar indicates and it can't be wrong

      h->name[i] = rh->prefix[i];
    }

    for (int idx = 0; idx < MTAR_NAMELEN; idx++) {
      if (rh->name[idx] == '\0') break;
      h->name[i++] = rh->name[idx];
    }

    h->name[i] = '\0';

  } else {
    for (i = 0; i < MTAR_NAMELEN; i++) {
      if (rh->name[i] == '\0') break;
      h->name[i] = rh->name[i];
    }

    h->name[i] = '\0';
  }

  i = 0;
  for (int idx = 0; idx < MTAR_NAMELEN; idx++) {
    if (rh->linkName[idx] == '\0') break;
    h->linkName[i++] = rh->linkName[idx];
  } // we might want to do the same

  h->linkName[i] = '\0';

  return MTAR_ESUCCESS;
}


static int header_to_raw(mtar_raw_header_t *rh, const mtar_header_t *h) {
  unsigned chksum;

  /* Load header into raw header */
  memset(rh, 0, sizeof(*rh));
  sprintf(rh->mode, "%o", h->mode);
  sprintf(rh->uid, "%o", h->uid);
  sprintf(rh->size, "%o", h->size);
  sprintf(rh->mtime, "%o", h->mtime);
  rh->type = h->type ? h->type : MTAR_TREG;
  strcpy(rh->name, h->name);
  strcpy(rh->linkName, h->linkName);

  /* Calculate and write checksum */
  chksum = checksum(rh);
  sprintf(rh->checksum, "%06o", chksum);
  rh->checksum[7] = ' ';

  return MTAR_ESUCCESS;
}


const char* mtar_strerror(int err) {
  switch (err) {
    case MTAR_ESUCCESS     : return "success";
    case MTAR_EFAILURE     : return "failure";
    case MTAR_EOPENFAIL    : return "could not open";
    case MTAR_EREADFAIL    : return "could not read";
    case MTAR_EWRITEFAIL   : return "could not write";
    case MTAR_ESEEKFAIL    : return "could not seek";
    case MTAR_EBADCHKSUM   : return "bad checksum";
    case MTAR_ENULLRECORD  : return "null record";
    case MTAR_ENOTFOUND    : return "file not found";
  }
  return "unknown error";
}


static int file_write(mtar_t *tar, const void *data, unsigned size) {
  unsigned res = fwrite(data, 1, size, tar->stream);
  return (res == size) ? MTAR_ESUCCESS : MTAR_EWRITEFAIL;
}


static int file_read(mtar_t *tar, void *data, unsigned size) {
  unsigned res = fread(data, 1, size, tar->stream);
  return (res == size) ? MTAR_ESUCCESS : MTAR_EREADFAIL;
}


static int file_seek(mtar_t *tar, unsigned offset) {
  int res = fseek(tar->stream, offset, SEEK_SET);
  return (res == 0) ? MTAR_ESUCCESS : MTAR_ESEEKFAIL;
}


static int file_close(mtar_t *tar) {
  fclose(tar->stream);
  return MTAR_ESUCCESS;
}


int mtar_open(mtar_t *tar, const char *filename, const char *mode) {
  int err;
  mtar_header_t h;

  /* Init tar struct and functions */
  memset(tar, 0, sizeof(*tar));
  tar->write = file_write;
  tar->read = file_read;
  tar->seek = file_seek;
  tar->close = file_close;

  /* Assure mode is always binary */
  if ( strchr(mode, 'r') ) mode = "rb";
  if ( strchr(mode, 'w') ) mode = "wb";
  if ( strchr(mode, 'a') ) mode = "ab";
  /* Open file */
  tar->stream = fopen(filename, mode);
  if (!tar->stream) {
    return MTAR_EOPENFAIL;
  }
  /* Read first header to check it is valid if mode is `r` */
  if (*mode == 'r') {
    err = mtar_read_header(tar, &h);
    if (err != MTAR_ESUCCESS) {
      mtar_close(tar);
      return err;
    }
  }

  /* Return ok */
  return MTAR_ESUCCESS;
}


int mtar_close(mtar_t *tar) {
  return tar->close(tar);
}


int mtar_seek(mtar_t *tar, unsigned pos) {
  int err = tar->seek(tar, pos);
  tar->pos = pos;
  return err;
}


int mtar_rewind(mtar_t *tar) {
  tar->remaining_data = 0;
  tar->last_header = 0;
  return mtar_seek(tar, 0);
}


int mtar_next(mtar_t *tar) {
  int err, n;
  mtar_header_t h;
  /* Load header */
  err = mtar_read_header(tar, &h);
  if (err) {
    return err;
  }
  /* Seek to next record */
  //n = round_up(h.size, 512) + sizeof(mtar_raw_header_t);
  n = round_up(h.size, 512) + MTAR_BLCKSIZ;
  return mtar_seek(tar, tar->pos + n);
}


int mtar_find(mtar_t *tar, const char *name, mtar_header_t *h) {
  int err;
  mtar_header_t header;
  /* Start at beginning */
  err = mtar_rewind(tar);
  if (err) {
    return err;
  }
  /* Iterate all files until we hit an error or find the file */
  while ( (err = mtar_read_header(tar, &header)) == MTAR_ESUCCESS ) {
    if ( !strcmp(header.name, name) ) {
      if (h) {
        *h = header;
      }
      return MTAR_ESUCCESS;
    }
    mtar_next(tar);
  }
  /* Return error */
  if (err == MTAR_ENULLRECORD) {
    err = MTAR_ENOTFOUND;
  }
  return err;
}


int mtar_read_header(mtar_t *tar, mtar_header_t *h) {
  int err;
  mtar_raw_header_t rh;
  /* Save header position */
  tar->last_header = tar->pos;
  /* Read raw header */
  err = tread(tar, &rh, sizeof(rh));
  tar->pos += 12;
  if (err) {
    return err;
  }
  /* Seek back to start of header */
  err = mtar_seek(tar, tar->last_header);
  if (err) {
    return err;
  }
  /* Load raw header into header struct and return */
  return raw_to_header(h, &rh);
}


int mtar_read_data(mtar_t *tar, void *ptr, unsigned size) {
  int err;
  /* If we have no remaining data then this is the first read, we get the size,
   * set the remaining data and seek to the beginning of the data */
  if (tar->remaining_data == 0) {
    mtar_header_t h;
    /* Read header */
    err = mtar_read_header(tar, &h);
    if (err) {
      return err;
    }
    /* Seek past header and init remaining data */
    //err = mtar_seek(tar, tar->pos + sizeof(mtar_raw_header_t));
    err = mtar_seek(tar, tar->pos + MTAR_BLCKSIZ);
    if (err) {
      return err;
    }
    tar->remaining_data = h.size;
  }
  /* Read data */
  err = tread(tar, ptr, size);
  if (err) {
    return err;
  }
  tar->remaining_data -= size;
  /* If there is no remaining data we've finished reading and seek back to the
   * header */
  if (tar->remaining_data == 0) {
    return mtar_seek(tar, tar->last_header);
  }
  return MTAR_ESUCCESS;
}


int mtar_write_header(mtar_t *tar, const mtar_header_t *h) {
  mtar_raw_header_t rh;
  /* Build raw header and write */
  header_to_raw(&rh, h);
  tar->remaining_data = h->size;
  return twrite(tar, &rh, sizeof(rh));
}


int mtar_write_file_header(mtar_t *tar, const char *name, unsigned size) {
  mtar_header_t h;
  /* Build header */
  memset(&h, 0, sizeof(h));
  strcpy(h.name, name);
  h.size = size;
  h.type = MTAR_TREG;
  //h.mode = 0664;
  /* Write header */
  return mtar_write_header(tar, &h);
}


int mtar_write_dir_header(mtar_t *tar, const char *name) {
  mtar_header_t h;
  /* Build header */
  memset(&h, 0, sizeof(h));
  strcpy(h.name, name);
  h.type = MTAR_TDIR;
  //h.mode = 0775;
  /* Write header */
  return mtar_write_header(tar, &h);
}


int mtar_write_data(mtar_t *tar, const void *data, unsigned size) {
  int err;
  /* Write data */
  err = twrite(tar, data, size);
  if (err) {
    return err;
  }
  tar->remaining_data -= size;
  /* Write padding if we've written all the data for this file */
  if (tar->remaining_data == 0) {
    return write_null_bytes(tar, round_up(tar->pos, 512) - tar->pos);
  }
  return MTAR_ESUCCESS;
}


int mtar_finalize(mtar_t *tar) {
  /* Write two NULL records */
  return write_null_bytes(tar, sizeof(mtar_raw_header_t) * 2);
}
