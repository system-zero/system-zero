#ifndef WITHOUT_ALLOC_INTERFACE
  #ifdef Alloc
  #undef Alloc
  #endif

  #ifdef Realloc
  #undef Realloc
  #endif

  #ifdef Release
  #undef Release
  #endif

  #ifdef MemInit
  #undef MemInit
  #endif

  #ifdef MemDeinit
  #undef MemDeinit
  #endif

  #define Alloc(__sz__) sys_malloc (__sz__)

  #define Realloc sys_realloc

  #define Release sys_free
  #define MemClear mem_clear
  #define MemInit mem_init
  #define MemDeinit mem_deinit

#endif /* WITHOUT_ALLOC_INTERFACE */
