#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef enum {
  ERROR_STRING_MUST_BE_IF,
  ERROR_STRING_MAY_NOT_FOLLOW_ELSE,
  ERROR_ERROR,
  ERROR_PREPROC_FAILURE,
  ERROR_MISSING_ARGUMENT,
  ERROR_INCLUDE_SYNTAX,
  ERROR_DEFINE_SYNTAX,
  ERROR_REDEFINE,
  ERROR_ILLEGAL_UNDEF,
  ERROR_RECURSIVE_MACRO,
  ERROR_EOF_IN_ARGUMENT,
  ERROR_MISPLACED_CONSTANT,
  ERROR_IF_OVERFLOW,
  ERROR_ILLEGAL_IF_LINE,
  ERROR_OPERATOR,
  ERROR_EXPR_OVERFLOW,
  ERROR_UNBALANCED_PARENS,
  ERROR_MISPLACED,
  ERROR_STRING_IN_IF,
  ERROR_DEFINED_SYNTAX,
  ERROR_ILLEGAL_ASSIGN,
  ERROR_ILLEGAL_BACKSLASH,
  ERROR_SIZEOF_SYNTAX,
  ERROR_SIZEOF_UNKNOWN,
  ERROR_SIZEOF_ILLEGAL_TYPE,
  ERROR_SIZEOF_NO_TYPE,
  ERROR_UNTERMINATED_STRING,
  ERROR_EOF_IN_COMMENT,
  ERROR_IFDEF_DEPTH,
  ERROR_ILLEGAL_CHARACTER,
  ERROR_ILLEGAL_CHARACTER2,
  ERROR_SIZEOF_BUG,
  ERROR_IF_OPERAND,
  ERROR_STRANG_CHARACTER,
  ERROR_STRANG_CHARACTER2,

  BORDER_ERROR_WARN, /* below this number: errors, above: warnings */

  WARN_CONTROL_LINE_IN_MACRO,
  WARN_ILLEGAL_COMMAND,
  WARN_UNEXPECTED_TEXT_IGNORED,
  WARN_TOO_FEW_VALUES_TO_SIZEOF,
  WARN_TOO_MANY_VALUES_TO_SIZEOF,
  WARN_NOT_DEFINED,
  WARN_INTERNAL_ERROR,
  WARN_MACRO_NEEDS_ARGUMENTS,
  WARN_WRONG_NUMBER_ARGUMENTS,
  WARN_DIVISION_BY_ZERO,
  WARN_ILLEGAL_OCTAL,
  WARN_MULTIBYTE_NOT_PORTABLE,
  WARN_CANNOT_OPEN_INCLUDE,
  WARN_BRACKET_DEPTH,
  WARN_PAREN_DEPTH,
  WARN_BRACE_DEPTH,
  WARN_NESTED_COMMENT,

  BORDER_WARN_FATAL, /* below this number: warnings, above: fatals */

  FATAL_TOO_MANY_NESTINGS,
  FATAL_FILENAME_BUFFER_OVERFLOW,
  FATAL_TOO_MANY_INCLUDE_DIRS,
  FATAL_TOO_MANY_INCLUDE_FILES,
  FATAL_TOO_MANY_ARGUMENTS_MACRO,
  FATAL_MACRO_AREA_OVERFLOW,
  FATAL_ILLEGAL_MACRO,
  FATAL_TOO_MANY_ARGUMENTS_EXPANSION,
  FATAL_OUT_OF_SPACE_IN_ARGUMENT,
  FATAL_WORK_AREA_OVERFLOW,
  FATAL_WORK_BUFFER_OVERFLOW,
  FATAL_OUT_OF_MEMORY,
  FATAL_TOO_MUCH_PUSHBACK


  } ErrorCode;

/**********************************************************************
 * RETURN CODES:
 *********************************************************************/

typedef enum {
  FCPP_OK,
  FCPP_OUT_OF_MEMORY,
  FCPP_TOO_MANY_NESTED_STATEMENTS,
  FCPP_FILENAME_BUFFER_OVERFLOW,
  FCPP_NO_INCLUDE,
  FCPP_OPEN_ERROR,
  FCPP_TOO_MANY_ARGUMENTS,
  FCPP_WORK_AREA_OVERFLOW,
  FCPP_ILLEGAL_MACRO,
  FCPP_EOF_IN_MACRO,
  FCPP_OUT_OF_SPACE_IN_MACRO_EXPANSION,
  FCPP_ILLEGAL_CHARACTER,
  FCPP_CANT_USE_STRING_IN_IF,
  FCPP_BAD_IF_DEFINED_SYNTAX,
  FCPP_IF_ERROR,
  FCPP_SIZEOF_ERROR,
  FCPP_UNTERMINATED_STRING,
  FCPP_TOO_MANY_INCLUDE_DIRS,
  FCPP_TOO_MANY_INCLUDE_FILES,
  FCPP_INTERNAL_ERROR,

  FCPP_LAST_ERROR
} ReturnCode;

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

#define FCPP_MAX_TAGS 64

typedef char *(*FcppInput) (char *, int, void *);           /* Input function */
typedef void  (*FcppOutput) (int, void *);                  /* output function */
typedef void  (*FcppError) (void *, const char *, va_list); /* error function */

struct fcppTag {
  int tag;
  void *data;
};

int fcppPreProcess (struct fcppTag *);
