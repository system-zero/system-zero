// comment: __WORDSIZE is required unconditionally by stdarg

/* Some code from diet libc, some from common standard headers. 
 * Here we define `vva_copy' by default */

#if defined(__GNUC__ ) || defined(__CLANG__) || defined(__TINYC__)

typedef __builtin_va_list va_list;
#define va_start(v,l)     __builtin_va_start((v),(l))
#define va_end            __builtin_va_end
#define va_arg            __builtin_va_arg
#define va_copy(d,s)      __builtin_va_copy((d),(s))

#else

typedef char* va_list;

#if __WORDSIZE == 64
#define va_start(ap,argn) ap = ((char*) &argn) + 8
#else
#define va_start(ap,argn) ap = ((char*) &argn) + 4
#endif

#define va_arg(ap,type) (ap += sizeof (type), *(type*) ((void*) ap - sizeof (type)))
#define va_copy(x,y) x = y
#define va_end(ap) ((void) 0)

#endif
