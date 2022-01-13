#ifndef LDD_HDR
#define LDD_HDR

typedef struct dl_t {
  int endian;
  int bits;

  char *unit;
  char **dependencies;
  int num;
 int verbose;
  int path;
} dl_t;

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN   1234
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN      4321
#endif

int DlDumpDependencies (dl_t *, const char *);
void DlRelease (dl_t *);

#endif /* LDD_HDR */
