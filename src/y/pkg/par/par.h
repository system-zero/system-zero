#ifndef PAR_H_DEFINED
#define PAR_H_DEFINED

#define DEFAULT_TABWIDTH 8
#define DEFAULT_WIDTH    78
#define DEFAULT_JUST     1

#define errmsg_size      256

typedef struct charset charset;
typedef struct lineprop lineprop;
typedef char   errmsg_t[errmsg_size];
typedef struct par_t par_t;

typedef int (*ParOutputByte) (par_t *, int);
typedef int (*ParOutputLine) (par_t *, const char *);

struct par_t {
  int hang;
  int prefix;
  int suffix;
  int repeat;
  int Tab;
  int width;
  int body;
  int cap;
  int div;
  int expel;
  int fit;
  int guess;
  int invis;
  int just;
  int last;
  int quote;
  int Report;
  int touch;

  int prefixbak;
  int suffixbak;

  char *parinit;
  char **inlines;
  char **outlines;

  charset *bodychars;
  charset *protectchars;
  charset *quotechars;
  charset *whitechars;
  charset *terminalchars;
  lineprop *props;

  errmsg_t errmsg;

  ParOutputByte output_byte;
  ParOutputLine output_line;

  void *userdata;
};

par_t  *par_new (void);
void par_release (par_t *);
int  par_process (par_t *, const char *);
void par_parsearg (par_t *, const char *);

#endif /* PAR_H_DEFINED */
