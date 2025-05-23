#define __FLT_RADIX__ 2
#define FLT_RADIX __FLT_RADIX__

#define __FLT_MANT_DIG__ 24
#define FLT_MANT_DIG __FLT_MANT_DIG__
#define __DBL_MANT_DIG__ 53
#define DBL_MANT_DIG __DBL_MANT_DIG__
#define __LDBL_MANT_DIG__ 64
#define LDBL_MANT_DIG __LDBL_MANT_DIG__

#define __FLT_DIG__ 6
#define FLT_DIG __FLT_DIG__
#define __DBL_DIG__ 15
#define DBL_DIG __DBL_DIG__
#define __LDBL_DIG__ 18
#define LDBL_DIG __LDBL_DIG__

#define __FLT_MIN_EXP__ (-125)
#define FLT_MIN_EXP __FLT_MIN_EXP__
#define __DBL_MIN_EXP__ (-1021)
#define DBL_MIN_EXP __DBL_MIN_EXP__
#define __LDBL_MIN_EXP__ (-16381)
#define LDBL_MIN_EXP __LDBL_MIN_EXP__

#define __FLT_MIN_10_EXP__ (-37)
#define FLT_MIN_10_EXP __FLT_MIN_10_EXP__
#define __DBL_MIN_10_EXP__ (-307)
#define DBL_MIN_10_EXP __DBL_MIN_10_EXP__
#define __LDBL_MIN_10_EXP__ (-4931)
#define LDBL_MIN_10_EXP __LDBL_MIN_10_EXP__

#define __FLT_MAX_EXP__ 128
#define FLT_MAX_EXP __FLT_MAX_EXP__
#define __DBL_MAX_EXP__ 1024
#define DBL_MAX_EXP __DBL_MAX_EXP__
#define __LDBL_MAX_EXP__ 16384
#define LDBL_MAX_EXP __LDBL_MAX_EXP__

#define __FLT_MAX_10_EXP__ 38
#define FLT_MAX_10_EXP __FLT_MAX_10_EXP__
#define __DBL_MAX_10_EXP__ 308
#define DBL_MAX_10_EXP __DBL_MAX_10_EXP__
#define __LDBL_MAX_10_EXP__ 4932
#define LDBL_MAX_10_EXP __LDBL_MAX_10_EXP__

#define __FLT_MAX__ 3.40282346638528859811704183484516925e+38F
#define FLT_MAX __FLT_MAX__
#define __DBL_MAX__ ((double)1.79769313486231570814527423731704357e+308L)
#define DBL_MAX __DBL_MAX__
#define __LDBL_MAX__ 1.18973149535723176502126385303097021e+4932L
#define LDBL_MAX __LDBL_MAX__

#define __FLT_EPSILON__ 1.19209289550781250000000000000000000e-7F
#define FLT_EPSILON __FLT_EPSILON__
#define __DBL_EPSILON__ ((double)2.22044604925031308084726333618164062e-16L)
#define DBL_EPSILON __DBL_EPSILON__
#define __LDBL_EPSILON__ 1.08420217248550443400745280086994171e-19L
#define LDBL_EPSILON __LDBL_EPSILON__

#define __FLT_MIN__ 1.17549435082228750796873653722224568e-38F
#define FLT_MIN __FLT_MIN__
#define __DBL_MIN__ ((double)2.22507385850720138309023271733240406e-308L)
#define DBL_MIN __DBL_MIN__
#define __LDBL_MIN__ 3.36210314311209350626267781732175260e-4932L
#define LDBL_MIN __LDBL_MIN__

#if __WORDSIZE == 64
#define __FLT_EVAL_METHOD__ 0
#else
#define __FLT_EVAL_METHOD__ 2
#endif

#define FLT_EVAL_METHOD __FLT_EVAL_METHOD__

#define __DECIMAL_DIG__ 21
#define DECIMAL_DIG __DECIMAL_DIG__

#define __FLT_DECIMAL_DIG__ 9
#define FLT_DECIMAL_DIG __FLT_DECIMAL_DIG__
#define __DBL_DECIMAL_DIG__ 17
#define DBL_DECIMAL_DIG __DBL_DECIMAL_DIG__
#define __LDBL_DECIMAL_DIG__ 21
#define LDBL_DECIMAL_DIG __LDBL_DECIMAL_DIG__

#define __FLT_HAS_DENORM__ 1
#define FLT_HAS_SUBNORM __FLT_HAS_DENORM__
#define __DBL_HAS_DENORM__ 1
#define DBL_HAS_SUBNORM __DBL_HAS_DENORM__
#define __LDBL_HAS_DENORM__ 1
#define LDBL_HAS_SUBNORM __LDBL_HAS_DENORM__

#define __FLT_DENORM_MIN__ 1.40129846432481707092372958328991613e-45F
#define FLT_TRUE_MIN __FLT_DENORM_MIN__
#define __DBL_DENORM_MIN__ ((double)4.94065645841246544176568792868221372e-324L)
#define DBL_TRUE_MIN __DBL_DENORM_MIN__
#define __LDBL_DENORM_MIN__ 3.64519953188247460252840593361941982e-4951L
#define LDBL_TRUE_MIN __LDBL_DENORM_MIN__

#define FLT_ROUNDS 1
