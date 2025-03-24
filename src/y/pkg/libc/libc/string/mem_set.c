// provides: void *mem_set (void *, int, size_t)
// provides: void *mem_set_s (void *, size_t, int, size_t)
// provides: void *memset (void *, int, size_t) __attribute__((alias ("mem_set")))

void *mem_set (void *dest, int c, size_t count) {
  if (count == 0 || dest == NULL) // it seems that std memset set its dest arg as non-null
    return dest;          // at least that is what gcc tells me, when it is used as memset
  /*  warning: 'nonnull' argument 'dest' compared to NULL [-Wnonnull-compar ]
  if (count == 0 || dest == NULL)
  */

  unsigned char *ptr = dest;

  while (count--) {
    /* we've been faced a stack overflow as a result from a compiler optimization,
       when this function is aliased to std memset().

       First we made sure that nowhere in our code we use mem_set().
       However when our executable is running something from std libc side is
       calling mem_set() with a count of 1256.

       With an optimization level > -O1 we observe a repeatdable call to this
       function that results to stack exhausted (or whatever is called).

       However when we use (count - 1) for the loop everything is working, so
       probably the compiler at this step it does its optimization, which it looks
       wrong with a first thought.

       We can bypass this and use the given count, if we use a statement like:
     */

     tostdout ("");

     /* but this doesn't work with any function, and i'm sure there is better way */

    *ptr++ = (unsigned char) c;
  }


  return dest;

  /*
    Else we can use assmebly.
    For instance this code from diet libc is working for x86_64:
  */

#if 0
  __asm__ volatile (
    "movzbl %sil,%eax\t\n"
    "movabs $0x101010101010101,%rsi\t\n"
    "imul %rsi,%rax\t\n"
    "mov %rdi,%rsi\t\n"
    "mov %rdx,%rcx\t\n"
    "shr $3,%rcx\t\n"
    "rep stosq\t\n"
    "mov %rdx,%rcx\t\n"
    "and $7,%rcx\t\n"
    "rep stosb\t\n"
    "mov %rsi,%rax\t\n"
    "ret"
 );
#endif

  /* in any case we made sure that we never use mem_set() as it's really
     an oneline code.

     But the best is for the end:
     memset() is the only function that the compiler requires to be defined
     when a unit is compiled  with -nostdlib.
     So that's it; a two hours journey to explain the unexplained.
   */
}

void *mem_set_s (void *dest, size_t destsz, int c, size_t count) {
  if (count == 0 || dest == NULL || 0 == destsz)
    return dest;

  unsigned char *ptr = dest;

  for (size_t i = 0; i < destsz && count--; i++)
    *ptr++ = (unsigned char) c;

  return dest;
}

/* test {
// num-tests: 1
#define REQUIRE_MEM_SET
#define REQUIRE_ATOI
#define REQUIRE_FORMAT
#define REQUIRE_ALLOC
#define REQUIRE_STR_EQ

#include <libc.h>
#define FUNNAME "mem_set()"

static int first_test (int total) {
  tostdout ("[%d] testing " FUNNAME " %s - ", total, __func__);

  int retval = -1;

  char *a = Alloc (8);
  mem_set_s (a, 8, 0, 8);
  for (int i = 0; i < 8; i++) {
    if (a[i] != 0) {
      tostderr ("\e[31m[NOTOK]\e[m\n");
      goto theend;
    }
  }

  mem_set (a, 'x', 4);
  int eq = str_eq (a, "xxxx");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (NULL, 8, 'c', 6);

  eq = str_eq (a, "xxxx");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (a, 0, 'c', 6);

  eq = str_eq (a, "xxxx");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (a, 8, 'c', 0);

  eq = str_eq (a, "xxxx");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (a, 8, 'c', 6);

  eq = str_eq (a, "cccccc");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  mem_set_s (a, 1, 'x', 6);

  eq = str_eq (a, "xccccc");

  if (0 == eq) {
    tostderr ("\e[31m[NOTOK]\e[m\n");
    goto theend;
  }

  tostdout ("\e[32m[OK]\e[m\n");
  retval = 1;

theend:
  Release (a);
  return retval;
}

int main (int argc, char **argv) {
  if (1 == argc) return 1;

  int total = str_to_int (argv[1]);
  int failed = 0;

  total++;
  if (first_test (total) == -1) failed++;

  return failed;
}
} */
