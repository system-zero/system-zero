Why?

Because the 'y' is after 'z' all the way to 'a'. And to do some development for
the development itself! And to introduce the first bits of a libc.

But the actual why, is because we want to provide some kind of realization to our
code, like what it does, where it belongs, and what it can do to provide some
kind of a special treatment to this ecosystem.

So it is mainly for inner development without caring much to expose functionality,
other than low level programming tools that explore and assist development, so
in other words mostly boring stuff.

First we instantaniated a tiny [libc](libc/README.md) (and an endless project...).
We used a script (implemented in our programming Language) which collects functions,
structures, constants and macros, and then it generates a single file libc. Amd while, we've
faced with conditions which can easily fail and unhandled cases (like handling
a failure during a function sequence), it has a been a bit of pleasure to work
and express :)(yea it really should be, as we choosed the way!). Anyway, it is
an intention to humanize this language even more, with more expressions.

And because we want to offer some kind of special treatment to this environment,
we will probably do it with some specialized code, which it might be an outer loop
which it might also give us a chance to escape from some bad code (though it is
not a high priority to fix (as long the job is done it is ok)).

Will be smart also to do the same with our editor, by using even more modes than
quite many they already are.

Our second step was to introduce a C [preprocessor](lib/fcpp/README) (for internal development reasons)
and which is also exposed as a library for reuse by others. It is some very old
code from the early nineties, but that looks that for the moment it does best its
destined job. Many thanks for this, as it is always a pleasure to see such a good
old C code to compiles (often without modifications or few, just to silence some
warnings when we enable our debug C flags) cleanly by modern compilers, and this is
an admirable C property. In this distribution we already happily using code that
it was first written at 1988.

We continued with a rather oversimplified `make' like utility, rather specific
for this environment and its development flow. This script stressed the machine
a bit more than any other code so far (1000++ rather complicated lines of code),
and which revealed more than a couple memory issues with the hand written memory
managment mechanism. Most were fixed, and at least in one case to avoid a memory
leak (when we had no other choise than to use function recursively), we've used
a workaround. Instead of storing the object into the function, we used a nested
function to handle the encapsulated object, which it is released when exiting from
the function environment.  
So yes, it is more like a big hack, than any real proper mechanism, that quite
strangely still is going on though. This seeks for an explanation!

## Acknowledgements, References, Algorithms, Libraries:

* [Frexx CPP C Preprocessor.](http://daniel.haxx.se/projects/fcpp)

C libraries:
* [A minimal C Runtime.](https://github.com/lpsantil/rt0)
* [A base system and early userspace for Linux.](https://github.com/arsv/minibase)
* [Diet Libc.](http://www.fefe.de/dietlibc)
* [Neat Libc.](https://github.com/aligrudi/neatlibc)
* [Embedded Artistry libc.](https://github.com/embeddedartistry/libc)
* [Mlibc.](https://github.com/managarm/mlibc)
And the big two:
* [musl standard C library.](https://musl.libc.org)
* [glibc](https://www.gnu.org/software/libc/) and which is the one who owe most (the world wouldn't be the same without GNU and their essential for the developement and the runtime products). I do not think that will ever be again such a dominant and trustworthy platform, which also set the highest development and practical philosophical standards, and which I believe is the real treasure that belongs to all of us humans.
