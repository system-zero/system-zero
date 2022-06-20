#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* end of taglist: */
#define FCPPTAG_END 0

/* To make the preprocessed output keep the comments: */
#define FCPPTAG_KEEPCOMMENTS 1 /* data is TRUE or FALSE */

/* To define symbols to the preprocessor: */
#define FCPPTAG_DEFINE 2 /* data is the string "symbol" or "symbol=<value>" */

/* To make the preprocessor ignore all non-fatal errors: */
#define FCPPTAG_IGNORE_NONFATAL 3 /* data is TRUE or FALSE */

/* To add an include directory to the include directory list: */
#define FCPPTAG_INCLUDE_DIR 4 /* data is directory name ending with a '/' (on
				amiga a ':' is also valid) */

/* To define all machine specific built-in #defines, default is TRUE: */
#define FCPPTAG_BUILTINS 5 /* data is TRUE or FALSE */

/* To define predefines like __LINE__, __DATE__, etc. default is TRUE: */
#define FCPPTAG_PREDEFINES 6 /* data is TRUE or FALSE */

/* To make fpp leave C++ comments in the output: */
#define FCPPTAG_IGNORE_CPLUSPLUS 7 /* data is TRUE or FALSE */

/* To define new sizes to #if sizeof: */
#define FCPPTAG_SIZEOF_TABLE 8 /* data is sizeof table string */

/* To undefine symbols: */
#define FCPPTAG_UNDEFINE 9 /* data is symbol name */

/* Output all #defines: */
#define FCPPTAG_OUTPUT_DEFINES 10 /* data is TRUE or FALSE */

/* Initial input file name: */
#define FCPPTAG_INPUT_NAME 11 /* data is string */

/* Input function: */
#define FCPPTAG_INPUT 12 /* data is an input funtion */

/* Output function: */
#define FCPPTAG_OUTPUT 13 /* data is an output function */

/* User data, sent in the last argument to the input function: */
#define FCPPTAG_USERDATA 14 /* data is user data */

/* Whether to exclude #line instructions in the output, default is FALSE */
#define FCPPTAG_LINE 15 /* data is TRUE or FALSE */

/* Error function. This is called when FCPP finds any warning/error/fatal: */
#define FCPPTAG_ERROR 16 /* data is function pointer to a
			   "void (*)(void *, char *, va_list)" */

/* Whether to warn for illegal cpp instructions */
#define FCPPTAG_WARNILLEGALCPP 17 /* data is boolean, default is FALSE */

/* Output the 'line' keyword on #line-lines? */
#define FCPPTAG_OUTPUTLINE 18 /* data is boolean, default is TRUE */

/* Output all included file names to stderr */
#define FCPPTAG_OUTPUTINCLUDES 20 /* data is boolean, default is FALSE */

/* Display warning if there is any brace, bracket or parentheses unbalance */
#define FCPPTAG_OUTPUTBALANCE 21 /* data is boolean, default is FALSE */

/* Display all whitespaces in the source */
#define FCPPTAG_OUTPUTSPACE 22 /* data is boolean, default is FALSE */

/* Allow nested comments */
#define FCPPTAG_NESTED_COMMENTS 23 /* data is boolean, default is FALSE */

/* Enable warnings at nested comments */
#define FCPPTAG_WARN_NESTED_COMMENTS 24 /* data is boolean, default is FALSE */

/* Enable warnings at missing includes */
#define FCPPTAG_WARNMISSINCLUDE 25 /* data is boolean, default is TRUE */

/* Output the main file */
#define FCPPTAG_OUTPUTMAIN 26 /* data is boolean, default is TRUE */

/* Include file */
#define FCPPTAG_INCLUDE_FILE 27 /* data is char pointer */

/* Include macro file */
#define FCPPTAG_INCLUDE_MACRO_FILE 28 /* data is char pointer */

/* Evaluate the right part of a concatenate before the concat */
#define FCPPTAG_RIGHTCONCAT 29 /* data is boolean, default is FALSE */

/* Include the specified file at the beginning of each function */
#define FCPPTAG_INITFUNC 30 /* data is char pointer or NULL */

/* Define function to be excluded from the "beginning-function-addings" */
#define FCPPTAG_EXCLFUNC 31 /* data is char pointer */

/* Enable output of all function names defined in the source */
#define FCPPTAG_DISPLAYFUNCTIONS 32

#define FCPP_MAX_TAGS 40

typedef char *(*FcppInput) (char *, int, void *);           /* Input function */
typedef void  (*FcppOutput) (int, void *);                  /* output function */
typedef void  (*FcppError) (void *, const char *, va_list); /* error function */

struct fcppTag {
  int tag;
  void *data;
};

int fcppPreProcess (struct fcppTag *);
