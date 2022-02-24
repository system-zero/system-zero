#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>

#include <stdarg.h>

#include "fcpp.h"

#define MAX_TAGS 40 /* maximum number of tags allowed! */

static char *own_input(char *, int, void *);
static void own_output(int, void *);
static void own_error(void *, char *, va_list);
static int SetOptions(int, char **, struct fppTag **);

static char ignore=FALSE;  /* if we should ignore strange flags! */
static char display=FALSE; /* display all options in use! */

static char dontreadprefs; /* set if only the command line is valid */

int main(int argc, char **argv)
{
  struct fppTag tags[MAX_TAGS];
  int i;
  struct fppTag *tagptr = tags;
  char *dealloc = NULL;

  /*
   * Append system-specific directories to the include directory list.
   * The include directories will be searched through in the same order
   * as you add them in the taglist!
   * The directory _must_ end with a proper directory speparator!
   */

  tagptr->tag=FPPTAG_INCLUDE_DIR;
  tagptr->data = (void *) "/usr/include/";

  tagptr++;

  if( !(i = SetOptions(argc, argv, &tagptr)))
    return(0);

  if (argc - i >2) {
    printf("Too many file arguments. Usage: cpp [options] [input [output]]\n");
    return(-1);
  }

  tagptr->tag=FPPTAG_INPUT;
  tagptr->data=(void *)own_input;
  tagptr++;

  if(i<argc) {
    /*
     * Open input file, "-" means use stdin.
     */
    if (strcmp(argv[i], "-")) {
      if (freopen(argv[i], "r", stdin) == NULL) {
	perror(argv[i]);
	fprintf(stderr, "Can't open input file \"%s\"", argv[i]);
	return(-2);
      }
      tagptr->tag=FPPTAG_INPUT_NAME;
      tagptr->data=argv[i];
      tagptr++;
      if(display)
	fprintf(stderr, "cpp: input: %s\n", argv[i]);
    } else				/* Else, just get stdin 	*/
      if(display)
	fprintf(stderr, "cpp: input: [stdin]\n");
    i++;
  } else
    if(display)
      fprintf(stderr, "cpp: input: [stdin]\n");

  if(i<argc) {
    /*
     * Get output file, "-" means use stdout.
     */
    if (strcmp(argv[i], "-")) {
      if (freopen(argv[i], "w", stdout) == NULL) {
	perror(argv[i]);
	fprintf(stderr, "Can't open output file \"%s\"", argv[i]);
	return(-1);
      }
      if(display)
	fprintf(stderr, "cpp: output: %s\n", argv[i]);
    } else
      if(display)
	fprintf(stderr, "cpp: output: [stdout]\n");
  } else
    if(display)
      fprintf(stderr, "cpp: output: [stdout]\n");

  tagptr->tag=FPPTAG_OUTPUT;
  tagptr->data=(void *)own_output;
  tagptr++;

  tagptr->tag=FPPTAG_ERROR;
  tagptr->data=(void *)own_error;
  tagptr++;

  /* The LAST tag: */

  tagptr->tag=FPPTAG_END;
  tagptr->data=0;
  tagptr++;

  fppPreProcess(tags);

  /*
   * Preprocess ready!
   */

  if( dealloc )
    free( dealloc );

  return(0);
}

static
char *own_input(char *buffer, int size, void *userdata)
{
  (void) userdata;
  return(fgets(buffer, size, stdin));
}

static void own_output(int c, void *userdata)
{
  (void) userdata;
  putchar(c);
}

static
void own_error(void *userdata, char *format, va_list arg)
{
  (void) userdata;
  vfprintf(stderr, format, arg);
}


static
int SetOptions(int argc, char **argv, struct fppTag **tagptr)
{
  int i;
  char *ap;
  for (i = 1; i < argc; i++) {
    ap = argv[i];
    if (*ap++ != '-' || *ap == '\0')
      break;
    else {
      char c = *ap++;

      if(display)
	fprintf(stderr, "cpp: option: %s\n", ap-2);

      switch (c) {                    /* Command character    */
      case 'Q':			      /* ignore unknown flags but */
	ignore=1;		      /* output them on stderr */
	break;

      case 'q':			      /* ignore unknown flags */
	ignore=2;
	break;

      case 'H':			      /* display all whitespaces */
	(*tagptr)->tag = FPPTAG_OUTPUTSPACE;
	(*tagptr)->data= (void *)TRUE;
	(*tagptr)++;
	break;

      case 'b': 		      /* display unbalance */
	(*tagptr)->tag = FPPTAG_OUTPUTBALANCE;
	(*tagptr)->data= (void *)TRUE;
	(*tagptr)++;
	break;

      case 'f':			      /* output all defined functions! */
	(*tagptr)->tag = FPPTAG_DISPLAYFUNCTIONS;
	(*tagptr)->data= (void *)TRUE;
	(*tagptr)++;
        break;

      case 'F':			      /* output all included files! */
	(*tagptr)->tag = FPPTAG_OUTPUTINCLUDES;
	(*tagptr)->data= (void *)TRUE;
	(*tagptr)++;
        break;

      case 'C':                       /* Keep comments */
	(*tagptr)->tag = FPPTAG_KEEPCOMMENTS;
	(*tagptr)->data= (void *)TRUE;
	(*tagptr)++;
	break;
	
      case 'D':                       /* Define symbol */
	(*tagptr)->tag=FPPTAG_DEFINE;
	(*tagptr)->data=argv[i]+2;
	(*tagptr)++;
	break;

      case 'd':                       /* Display all options */
	fprintf(stderr, "FOUND -d flag!\n");
	display = TRUE;
	break;
	
      case 'E':                       /* Ignore non-fatal errors */
	(*tagptr)->tag=FPPTAG_IGNORE_NONFATAL;
	(*tagptr)->data=(void *)TRUE;
	(*tagptr)++;
	break;
	
      case 'I':                       /* Include directory */
	(*tagptr)->tag=FPPTAG_INCLUDE_DIR;
	(*tagptr)->data=ap;
	(*tagptr)++;
	break;
	
      case 'J':                       /* Allow nested comments */
	(*tagptr)->tag=FPPTAG_NESTED_COMMENTS;
	(*tagptr)->data=ap;
	(*tagptr)++;
	break;
	
      case 'j':                       /* Warn at nested comments */
	(*tagptr)->tag=FPPTAG_WARN_NESTED_COMMENTS;
	(*tagptr)->data=ap;
	(*tagptr)++;
	break;
	
      case 'L':
	if(*ap == 'L') {	      /* Do not output #line */
	  (*tagptr)->tag=FPPTAG_LINE;
	} else {
	  /* Do not output the 'line' keyword */
	  (*tagptr)->tag=FPPTAG_OUTPUTLINE;
	}
	(*tagptr)->data=(void *)FALSE;
	(*tagptr)++;
	break;

      case 'M':                       /* Do not warn at missing includes */
	(*tagptr)->tag=FPPTAG_WARNMISSINCLUDE;
	(*tagptr)->data=(void *)FALSE;
	(*tagptr)++;
	break;

      case 'n':
        dontreadprefs^=1; /* toggle prefsreading, default is read prefs */

	/*
	 * This flag should reset all previously added tags!
	 */

        break;

      case 'N':                       /* No machine specific built-ins */
	(*tagptr)->tag=FPPTAG_BUILTINS;
	(*tagptr)->data=(void *)FALSE;
	(*tagptr)++;
	break;

      case 'B':			      /* No predefines like __LINE__, etc. */
	(*tagptr)->tag=FPPTAG_PREDEFINES;
	(*tagptr)->data=(void *)FALSE;
	(*tagptr)++;
	break;
	
      case 'P':			      /* No C++ comments */
	(*tagptr)->tag=FPPTAG_IGNORE_CPLUSPLUS;
	(*tagptr)->data=(void *)TRUE;
	(*tagptr)++;
	break;
	
      case 'p':			      /* warn about illegal # - instructions */
	(*tagptr)->tag = FPPTAG_WARNILLEGALCPP;
	(*tagptr)->data= (void *)TRUE;
	(*tagptr)++;
	break;

      case 'R':
	(*tagptr)->tag = FPPTAG_RIGHTCONCAT;
	(*tagptr)->data= (void *)TRUE;
	(*tagptr)++;
	break;

      case 's':			      /* sizeof table */
	(*tagptr)->tag=FPPTAG_INITFUNC;
	(*tagptr)->data=ap;
	(*tagptr)++;
	break;
	
      case 't':			      /* excluded functions */
	(*tagptr)->tag=FPPTAG_EXCLFUNC;
	(*tagptr)->data=ap;
	(*tagptr)++;
	break;
	
      case 'S':			      /* sizeof table */
	(*tagptr)->tag=FPPTAG_SIZEOF_TABLE;
	(*tagptr)->data=ap;
	(*tagptr)++;
	break;
	
      case 'U':                       /* Undefine symbol */
	(*tagptr)->tag=FPPTAG_UNDEFINE;
	(*tagptr)->data=ap;
	(*tagptr)++;
	break;

      case 'w':			      /* Output all #defines but not the
					       main file */
	(*tagptr)->tag=FPPTAG_OUTPUTMAIN;
	(*tagptr)->data=(void *)FALSE;
	(*tagptr)++;
        break;
          	
      case 'W':			      /* Output all #defines */
        (*tagptr)->tag=FPPTAG_OUTPUT_DEFINES;
        (*tagptr)->data=(void *)TRUE;
        (*tagptr)++;
	break;

      case 'X':
	(*tagptr)->tag=FPPTAG_INCLUDE_FILE;
	(*tagptr)->data=ap;
	(*tagptr)++;
	break;

/*
      case 'x':
	tags[tag]->tag=FPPTAG_INCLUDE_MACRO_FILE;
	tags[tag++]->data=ap;
	break;
*/
      case 'h':
      case '?': /* if a question mark is possible to specify! */
      default:			/* What is this one?	*/
	if( ignore < 2 && c != 'h') {
	  fprintf(stderr, "cpp: unknown option: -%s\n", ap-1);
	}
	if(!ignore || c == 'h') {
	  fprintf(stderr,
		  "Usage: cpp [options] [infile [outfile] ]\n\n"
		  "The following options are valid:\n"
		  "  -B\tNo mahcine specific built-in symbols\n"
		  "  -b\tOutput any parentheses, brace or bracket unbalance\n"
		  "  -C\tWrite source file comments to output\n"
		  "  -D\tDefine a symbol with the given (optional) value \"symbol[=value]\"\n"
		  "  -d\tDisplay all specified options\n"
		  "  -E\tIgnore non-fatal errors\n"
		  "  -F\tOutput all included file names on stderr\n"
		  "  -f\tOutput all defined functions' names on stderr\n"
		  "  -H\tOutput all whitespaces from the source file\n"
		  "  -h\tOutput this help text\n"
		  "  -I\tAdd directory to the #include search list\n"
		  "  -J\tAllow nested comments\n"
		  "  -j\tEnable warnings for nested comments\n"
		  "  -LL\tDon't output #line instructions\n"
		  "  -L\tDon't output the 'line' keyword in \"#line\" instructions\n"
		  "  -M\tDon't warn for missing include files\n"
		  "  -N\tDon't predefine target-specific names\n"
		  "  -n\tToggle prefs usage\n"
		  "  -P\tDon't recognize C++ comment style\n"
		  "  -p\tEnable warnings on non ANSI preprocessor instructions\n"
		  "  -Q\tIgnore but visualize undefined flags\n"
		  "  -q\tIgnore all undefined flags\n"
		  "  -R\tEvaluate the right part first in symbol concatenations\n"
		  "  -s\tInclude the following string at the top of each function\n"
		  "  -S\tSpecify sizes for #if sizeof\n"
		  "  -t\tThis function should not get an initial function\n"
		  "  -U\tUndefine symbol\n"
		  "  -V\tDon't output version information\n"
		  "  -W\tOutput all #defines\n"
                  "  -WWW\tWeb mode preprocessing\n"
		  "  -w\tOnly output #defines\n"
		  "  -X\tInclude file\n");
	  return(0);
	}	/* if (!ignore) */
      }		/* Switch on all options	*/
    }		/* If it's a -option            */
  }		/* For all arguments		*/

  return i;

} /* end of function */
