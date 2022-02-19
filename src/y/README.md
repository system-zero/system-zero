Why?

Because the 'y' is after 'z' all the way to 'a'. And to do some development for
the development itself! And to introduce the first bits of a libc.

But the actual why, is because we want to provide some kind of realization to our
code, like what it does, where it belongs, and what it can do to provide some
kind of a special treatment to this ecosystem.

So it is mainly for inner development without caring much to expose functionality,
other than low level programming tools that explore and assist development, so
in other words mostly boring stuff.

First we introduce an oversimplified libc for x86 and x86_64, based on syscalls.
The syscalls implementation is based on [rto project](https://github.com/lpsantil/rt0), with some bits from
[minibase](https://github.com/arsv/minibase) and [neatlibc](https://github.com/aligrudi/neatlibc).

This is actually a forever project, so we started with few functions and we will
add more as we need them. Functions belongs to a namespace which is based on the
standard header. The top line provides the function signature and which it should
correspond to the standard implementation. The following lines can declare the
dependencies, either other functions or types.

Our intention for this project is to create a single file libc, so we've used
our programming language to collect the functions and autogenerate this unit.
During the development we've faced with conditions, that can easily fail and
are not handled currently. For instance when we are using function sequences,
something may fail in the middle, and not all of the modules can handle some
unexpected input, neither the underlying mechanism knows what to do in those
cases.

Yet it was a been a bit of pleasure to program in this quite expressive way.
And it is an intention to humanize this language even more. And also to add
more specialized code, that will handle specific cases for this environment,
as we are not upstream for noone, so we can be as flexible we like, without
really caring to be political correct. And yes we know, that the machine is
quite weak that can easily break, but it is ok if we write in known ways.
