```C
/* This is an oversimplified tiny libc for x86 and x86_64.
 *
 * It is based on syscalls and it is meant just for a primitive and
 * straight forward way to execute some code, without any intermediator.
 * It is not the safest way and it is not the optimized way to do this
 * kind of job.
 *
 * It is focused on clarity, that means that between an optimized code
 * that we can not understand, the simplest is preferable. We might want
 * to write more than one version of a function.
 *
 */
```

The syscalls implementation is based on [rt0 project](https://github.com/lpsantil/rt0), with some bits from
[minibase](https://github.com/arsv/minibase) and [neatlibc](https://github.com/aligrudi/neatlibc).

I also heavily looked (or got code from them) at the following libc implementations:

* [Diet Libc.](http://www.fefe.de/dietlibc)
* [Embedded Artistry libc.](https://github.com/embeddedartistry/libc)
* [Mlibc.](https://github.com/managarm/mlibc)

Their licenses should be lying withing the Licenses directory except from neatlibc
that has no license attached at it's repo. And as it is my primary source from
many years now, either by looking around those admirable projects by the same
author, or because I got a way too much from this tiny libc, it deserves a special
treatmeant here.

Many thanks to all.
