#ifdef OK
#undef OK
#endif

#ifdef NOTOK
#undef NOTOK
#endif

#ifdef ifnot
#undef ifnot
#endif

#ifdef is
#undef is
#endif

#ifdef isnot
#undef isnot
#endif

#ifdef or
#undef or
#endif

#ifdef and
#undef and
#endif

#define OK     0
#define NOTOK -1
#define is    ==
#define isnot !=
#define or    ||
#define and   &&
#define ifnot(__expr__) if (0 == (__expr__))
#ifdef private
#undef private
#endif

#ifdef public
#undef public
#endif

#ifdef UNUSED
#undef UNUSED
#endif

#define private __attribute__ ((visibility ("hidden")))
#define public  __attribute__ ((visibility ("default")))
#define UNUSED  __attribute__ ((unused))
