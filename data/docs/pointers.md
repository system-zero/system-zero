Note, that this is about C pointers, and it is connected with the "Semantics" README
section, and it specifically refers to the "Dir.make" command, which we'll use here.

Lets say that we told to our shell:

   "make a directory with the name Cave"

so the "directory_name" variable will have "Cave" as its value.
Here is the code to get the first byte in C:

```C
   *directory_name
```

that's all. Do not think that there is a another way. That is how every programmer
in earth get the first byte of the variable "directory_name". The asterisk is the
C's way to get the underlying byte.

And quite logically and naturally is called a pointer, because that is what it does.
It *points* to the first byte of the "directory_name" variable.

If we wanted to get the second byte, then quite simply we'll do it like this:

```C
   *(directory_name + 1)
```

Do not still believe? Try it by yourself:
```C
#include <stdio.h>

int main (int argc, char **argv) {
  char *arg = argv[1]; // get the first given argument in the command line
  // Note that argv[0] always holds the name of the executable

  fprintf (stdout, "%c - %c - %c", *arg, *(arg + 1), *(arg + 2));

  /* send also a new line character to the standard output stream */
  fprintf (stdout, "\n");
  return 0;
}
```

Save the above to a file (say test.c), compile and then run:
```sh
  cc -o test test.c
  ./test "something with at least three characters"
```
If you don't provide an argument, the program will segfault!, which it is something
really really bad, and we really we have to do something for this.
We have to write code, to check if there is an argument, and only then we have to
continue with printing.

But this program suffers from another unhandled case, as it will do the wrong thing
if you pass an argument with less than 3 characters.

So, we've also to count the number of bytes of the argument, and here is a way:
```C
   int num = 0;  // start from zero
   int idx = 0;  // the first index

   /* we'll loop over the bytes untill the end of the string, which
    * this is simply the first byte that we'll found with a value of zero
    * (in C this is called the null byte).
    * BE CAREFUL. This is not the same as the zero number. The zero number it has
    * the value of 48. */

   while (*(arg + idx) != 0) {
     num++;  // increment the number of characters
     idx++;  // increment the index and continue with the next byte
   }
   /* since now we have the number of bytes, then we can easily print our argument. */

   /* but the above code could be also written like:

   char *sp = arg;
   while (*sp)
     sp++;
   num = sp - arg;

   /* and this is how (for years) programmers counted the bytes of a string, and
    * it is called pointer arithmetic.
    *
    * This small function is called strlen(), though the name can be misleading,
    * as this counts the number of bytes and not the number of characters, as one
    * may think.
    * The number of characters are not always the number of bytes.
    * For English letters [A-Z] and [a-z] this is certainly true, but not for all
    * the characters in the world!
    * How could be that possible? One byte can only have 256 values! Here is the
    * code to print the associated bytes and character represantation of all the
    * possible values of a byte: */

    for (int i = 0; i < 256; i++)
      fprintf (stdout, "%c has the value of %d\n", i, i);

    /* you'll notice a couple of weirdness. Why? Because some of the represantations
     * are not really printable. Like the backspace key, which has the value of 9, or
     * the space character that has the value of 32, as we can not really see the
     * emptyness! But it is there, it is just that we can not see it. */

   /* But how this code works: */
   char *sp = arg;  while (*sp) sp++; num = sp - arg;

   /* Because simply, we just incremented the address of the variable name, so it
    * points to next place/byte in the raw memory.
    *
    * A variable name is associated with the first byte located in memory, so it
    * isn't by just the address in the raw memory.
    *
    * There is no magic.
    *
    * A variable name is associated with the first byte located in memory.
    * It is called simply the "address". */

    /* Lastly we substracted the difference of the last pointer position with the
     * first, and we got our result.
     * Remember that the starting point of "sp" it was the address  of "arg", and
     * since we always advanced the "sp" pointer to the next byte, untill we reach
     * to the last ont (the null byte), the substraction will gives us ALWAYS the
     * correct result.
     * The C language *guarantees* the result.
     */
    num = sp - arg;
     /* Num now holds the number of bytes. Again. Not of characters. That is something
      * different. For instance a greek character represantation requires always two
      * bytes. Some other characters of our world's native languages require three, or
      * four (maximum). This is a special character encoding called UTF8, and which
      * is a de-facto standard. Well almost.
      */
```
