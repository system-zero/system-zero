  
Lets say that we told to our shell:  
  
    "make a directory with the name Cave"  
  
so the "directory_name" variable will have "Cave" as its value.  
Here is for instance the code to get the first byte:  
  
```C
   *directory_name
```
  
that's all. Do not think for another way. That is how every programmer in earth  
get the first byte of the variable "dname". The asterisk is the C way to get the  
underlying byte, and quite logically and naturally is called a pointer, because   
that is what it does. It points to the first byte of the "dname" variable.  If we  
wanted to get the second byte then quite simply we'll do it like this:  
  
```C
   *(dname + 1)
```
  
Do not still believe? Try it by yourself:  
```C
#include <stdio.h>

int main (int argc, char **argv) {
  char *arg = argv[1];
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
  
If you don't provide an argument, the program will segfault, which it is something  
really really bad, and we really we have to do something for this.  
  
We have to write code, to check if there is an argument, and only then we have to  
continue with printing. But this program suffers from another unhandled case, as  
it will do the wrong thing if you pass an argument with less than 3 characters. 
So, we've to count the number of bytes of the argument:  

```C
   int num = 0;  // start from zero
   int idx = 0;  // the first byte
   /* we'll loop over the bytes untill the end of the string, which
    * this is simply the first byte that we'll found with a value of zero
    * (in C this is called the null byte).
    * BE CAREFULL. This is not the same as the zero number. The zero number it has 
    * the value of 48. */
   while (*(arg + idx) != 0) {
     num++;  // increment the number of characters
     idx++;  // increment the index and continue with the next byte
   }
   /* since now we have the number of bytes, then we can easily print our argument. */
   
   /* but the above code could be also written like:

   char *sp = arg;
   while (*sp) sp++; num = sp - arg;

   /* and this is how (for years) programmers counted the bytes of a string.
    * This small function is called strlen(), though the name can be misleading,  
    * as this counts the number of bytes and not the number of characters, as one 
    * may think, as the number of characters do not ever assumed the number of bytes.
    * For English letters [A-Z] and [a-z] this is certainly true, but not for all the
    * characters in the world. How could be that possible? One byte can have only
    * 256 values! Here is the code to print the associated bytes: */

    for (int i = 0; i < 256; i++)
      fprintf (stdout, "%c has the value of %d\n", i, i);

    /* you'll see a couple of weirdness, because some of the represantations are
     * not really in printed form, like the backspace key, which has the
     * value of 9, or the space character that has the value of 32,
     * as we can not really see the emptyness! But it is there, it is just that
     * we can not see it. */

   /* But how this code works: */
   char *sp = arg;  while (*sp) sp++; num = sp - arg;

   /* Because simply, we just incremented the address of the variable name.
    *
    * A variable name is associated with the first byte located in memory,
    * so it isn't by just the address in memory.
    *
    * There is no magic.
    * A variable name is associated with the first byte located in memory.
    * It is called simply the "address". */
    
    /* Lastly we substract the difference of the pointers and we get
     * our result. Remember that the starting point of "sp" it was the address of "arg"
     * and since what we did was to increment the pointer to the next bit, the
     * C language guarantees the result.
     */
    num = sp - arg;
```
