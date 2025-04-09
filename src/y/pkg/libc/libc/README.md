This is an oversimplified tiny libc for x86 and x86_64.

As it is mostly interested for the mechanics, it should prioritize code clarity/intention
instead of an optimization. Or else, it should be rather considered to ba a bug or simply
weakness to develop readable code, to assist us to understand the logic behind.

It is really meant for the most primitive and straight forward way to execute some code,
without any intermediator.

It probably is not the safest way and it is not the optimized way to do this kind of job.

Notable omissions:
  - dynamic linking
  - threads

The primary concentration is to provide a freestanding environment, so dynamic linking is
not a priority (for now), but a thread implementation is rather crucial.

This should compile with -std=c99 and onwards standards, until (included) c23, without a
warning. We do prefer c11 or c23 though.

The syscalls implementation was based on [rt0 project](https://github.com/lpsantil/rt0), with some bits from
[minibase](https://github.com/arsv/minibase) and [neatlibc](https://github.com/aligrudi/neatlibc).

Other C libraries that at least consulted or heavily inspired or got some bits from them:

* [Diet Libc.](http://www.fefe.de/dietlibc)
* [Embedded Artistry libc.](https://github.com/embeddedartistry/libc)
* [Mlibc.](https://github.com/managarm/mlibc)
* [Picolibc.](https://github.com/picolibc/picolibc)
* [klibc from Linux Kernel.](https://git.kernel.org/pub/scm/libs/klibc/klibc.git)

Their licenses and if it is appropriate, should be lying within the Licenses directory.

Specific (function scope) licenses, should be inlined into the unit or with a specific
License file at the same directory.

As neatlibc was my initial inspiration and which gave me the chance to implement the very
first bits, I feel that I owe the most. Also, as since minibase ended up to be my primary
source (and more close to my way of coding and thinking), I also feel that I owe the most.
Both are admirable projects and I feel that they deserve some special treatmeant here.

Many thanks to all.
