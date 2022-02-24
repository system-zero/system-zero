// requires: stdio/FILE.h

#ifndef BUFSZ
#define BUFSZ 1024
#endif

static char _ibuf[BUFSZ], _obuf[BUFSZ], _ebuf[BUFSZ];
static FILE _stdin = {0, EOF, _ibuf, NULL, BUFSZ, 0, 0, 0, 0, 0, 0, 0};
static FILE _stdout = {1, EOF, NULL, _obuf, 0, BUFSZ};
// 0, 0, 0, 0, 0, 0};
static FILE _stderr = {2, EOF, NULL, _ebuf, 0, 1};
//, 0, 0, 0, 0, 0, 0};
FILE *stdin = &_stdin;
FILE *stdout = &_stdout;
FILE *stderr = &_stderr;

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
