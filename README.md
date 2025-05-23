## About our host.
I've been using github for years, and I've to admit that I've never encounterd any issue.
But also there were many times that I've been surprised by the quality of their services,
probably because the code was written from coders for coders, it matched the expectations
and beyond to be honest. Probably and from a user point of view, one of the best software
I've ever used.

However, there is a question in me:

Why the home of open source is a closed source platform?

of which I do not have an answer and I find myself unconformable.

I've to think a bit more, but the decision (unless something is going to be change in a quite
drastic way) seems relative easy.

## Research on most primitive functional systems from scratch.

This project has been started at the very first days of 2021, and the first cycle has
been ended at the last days of August of 2022.

Initially, we had set the ambitious goal to build a bootable system, but we were unable
to achieve this promise, however we were close as the main missing functionality is an
init.

## The initial idea and intention

The idea is about an independent (but a kernel) standalone usable system,
with the absolute required functionality for the complete administration,
within a primitive standardised environment, with an adjusted to the user
interface.

At the same time and equally important is demystification and realization
of the underground mechanics.

This has two main branches:

  - a [bootstrapped system](https://gitlab.com/giomasce/asmc) from nothing
    but a seed, that can provide the essential development tools

  - the actual exposed system and our main focus (at this initial state at
    least), so we assumed a C compiler, a standard C library, a linker and
    basic tools like make and core utilities

A bootstrapped system is a [complicated research](https://bootstrapping.miraheze.org/wiki/Live-bootstrap)
by its own, but it is out of scope at least for now, though it is a prerequisitie in
reality, as such it should be considered a missing ring in the chain.

The result of this zero branch, was a shell, an editor, a scripting language,
development tools, most of the system administration utilities and a rich set
of essential libraries. What we want to achieve is to migrate our code to use
our own libc.

Most of this code has been written from scratch, but we've also used many small
and mostly unknown published libraries, or specific algorithms, pulled from the
enormous pool of the free/open source ecosystem, and so we are grateful.

The development process produced such huge amount of data, which is spreading
into development sources or git logs and documents, that a significant amount
of time of intensive work is required to select and process this information,
with corrections, expansions and proper pointers on definite and unquestionable
documents to the web. The complicated nature of this project, this means some
myriad of autonomous subjects, that needs to be glued and be accessible by the
system itself first, but also in a form that can be extracted to common formats.

We are developing in a Linux system, with C as our programming language, and so we
are awaiting a C compiler, a libc, a linker, the make utility and probably a couple of coreutils
(we would like to record which ones), for building the first zero bits.

It is crucial to record this whole path to completion, for educational purposes,
and though this happens automatically by git, we would like in future to document it.

Our code usually prefers the obvious algorithm that can be read with clarity, instead
of an optimized one, even if we know that we sacrificy performance, though even slow C
can not be beaten. We give extensive focus to respect resources, though we are really
want our datatypes to be clear, without magic C trycks, so we want them rich and self
explained by their specific type and name, to ease our development process, because in
such enormous system, you really want to understand the code after it was well written.

We give extensive focus, to unite the execution namespace and the source code, as we
believe that both are interdependent and interchangeable, as the wise path to evolution
and quite probably to our freedom finally. For every bit of code, it is utterly important
to understand the "why" and the intentions. We let then our actual will (filtered by our
logic) and the fascination, to lead and give the fuel to this eternal path. So there has
to be an obvious fast path and where to lookup to locate those bits that has been used to
implemented this "why" in the source code, and then act accordingly by modifiying or just
accept it's decision by realizing the intention.

For those reasons we favor clarity instead of magic in our code. The code is, and it will
always be and our definite documentation, though it would be cool to record the semantics,
and we'll do it if we will be blessed with generous time.

The layout of this README, ended up mostly like a blog, and it is really quite
complicated unfortunately, and so its not your blame, if you don't understand.
Anyway [this](#Intention) section started all, but be aware that it is an early
first draft that needs a serious revision (probably 2 or 3 months of intensive
work), but at least for the next three/four years, it is not a priority.

Do not expect a hundred percent accuracy for all the technical statements that
are presented in the documents or within comments in the source code, but they
should be close. Likewise for the source code itself. It was written by a self
educated human being, who started C after his fifty plus, but it does not look
that bad for the kindergarden level, especially if you think that it is really
a unification of __many__ (small or big and complicated by their own) projects.

[ zero point draft]

Status:  
Implemented:
  - a rich set of libraries
  - a vim like editor
  - a terminal multiplexer (like gnu-screen)
  - a scripting language
  - a basic set of system/core utilities
  - a tiny shell
  - a package manager
  - a make kind utility
  - a container
  - a C library
  - a primitive http[s] client
  - a game (sudoku)
  - and some old C code from early ninities (there is a special interest)

The only requirenment is libc, except that the sudo replacement requires libpam
and the https functionality requires either openssl or libressl. We can do though  
without libpam, but we have to implement crypt() first.

Unimplemented:  
For a standalone system:
  - init
  - device managment
  - network managment
  - system configuration (timezone/password/groups)
  - libc/kernel/X installation?

Functionality:
  - mail managment
  - process viewer
  - a chat/file-sharing server/client
  - a gemini server/client (an almost sane protocol)
  
Development:
  - scdoc man page generator (done)
  - SSL library
  - libgit2
  - mir or|and tcc or|and probably tagha or|and gcc or zig, and a linker
  
Interesting to integrate in this system:
  - very old C codebases
  - mostly unknown tiny libraries and utilities
  - all of the kind of specific algorithms
 
Indepented units:
  - functions that operate on one thing
  
Written but not still exposed to this system:
  - an X window manager
  
See [Trunc Development.](#TRUNC)  
  
Briefly, many of them are almost fully functional, but some of them are in a
prototype state and with unhandled conditions and unimplemented functionality.
And while even now with some few lines of code it will probably can boot and
be used as a standalone system - so many little details that need some handling -
there isn't such a big will to take this path, as I've done it in the past.
So the focus seems to be shifted to things I've never dealt with them before,
as the main intention for this project is to understand the way and the why.

What it follows is the very first original README document, and it is surely a
mess at points.

## Intention

This is  to describe and implement  a functional computer system,  with as few
resources  is possible,  and provide  our own  solution for  as many  specific
computer tasks is possible.

It is meant to do that by exposing the development process that is sparked with
a declared intention. Somehow it is the intention that spawns a mechanism, that
through analyzing and will, leads and to the actual implementation. Perhaps it
is that journey that really matters at the end. The hope and the only  ambition
is that this material will offer some value, though there isn't an expectation.

It is written without [a well defined sense of time](#About), but it might be fitted best
probably at the middle of eighties, so we are probably a bit late. But a system
is about mechanics, and the procedure to demystification it's not about current
only relative terms.

Since this is a quite huge task, we'll  have to extend our system in a gradual
way, so  we have to divide  it into stages. At  the end of any  stage, the end
result should be in a usable state.

*Note* that this document needs a serious revision, as it is a quite first prototype, that
is expressed here sometimes in a list of ideas form. Some paragraphs are quite
verbose, and might loose their intented meaning. Some statements might overlap
with others. Expressions can be sometimes confusing, as the natural mind  flow
is like a rolling stone, that is hard at times to be reproduced with words and
it might be represented poorly.

*Warning*. This is rather a multi-dimensional project, with a clear and declared
emphasis, but this is also a vehicle to expose realizations and even desires!!!
with an unmercyfully way, as probably every little everything is connected. It
is attempted to be written as a prose, with a just in time compilation of some
thoughts to words, that are destined to describe, reveal or|and discover logic
and dimensions, that they can be used in a practical systematic way.

Again: the only ambition is to be honest to itself.

## Zero Point Description.

 (and the very beginning to implementation ... (estimated time to completion
  eighteen months (started at the very last days of 2020))

The  first  stage  is of  course  to  be  able  to get  control  after  kernel
initialization, so at some  point we'll have to be ready to  be pid 1. Usually
in the Unix land,  this is a program that is called init,  and it is the first
process that is spawned by the kernel  by default. Any process is inherited by
this pid 1 (Process Id).

Normally this is  a program, that makes  the necessary steps to  brings up the
computer in a usable state, and also to halt the system.

The first  task usually is  to mount the  required filesystems, like  the root
filesystem, that  is assuming that has  the required tools to  be a functional
system.

Another crucial  task is to  offer ways to  the user of  a system, to  use the
underlying  machine's  hardware,  by  binding device  pointers  in  a  special
directory at the root mount point (`/dev`).  The root mountpoint is denoted by a
slash (`/`), and everything is related to this scratch system point.

Another common task,  is to start some given services  explicitly set from the
user, like  to mount  other filesystems  in some given  addresses of  the root
mountpoint.

This procedure is called Init Process, and traditionally in [systemV](https://en.wikipedia.org/wiki/UNIX_System_V), this is
done through a very simple mechanism. Such a procedure it looks in a directory
for files, which are usually shell scripts and loads them by usually using the
default shell  in UNIX,  which simply is  called `sh`, and  by default  found as
`/bin/sh`. This location  is standardized by the POSIX standard,  and this shell
is referred commonly as POSIX `sh`.

The sequence of executing those scripts is  dictated by the first two bytes in
the filenames, which should  be digits, so the lower one in  this list, is the
one that take precedence. Naturally a script that has a 99 prefix, as the last
one in  the chain, it has  the ability to  overlap any previous ones.  This is
also satisfying the  UNIX philosophy, where the users of  a system should have
the final control of a system, even if they are wrong.

This  mechanism,  besides the  flexibility,  satisfy  also user  expectations,
because the SystemV  init system has been established through  those years. It
is modeled by again following the UNIX philosophy, which is modularization and
simplicity. So a  user that knows the  mechanism, can modify a  system to meet
the specific needs,  in an expected way  and without the fear to  bring down a
monolithic system, like  say "systemd", which some  modern Linux distributions
(at the 2020 era) favor instead of  a traditional init, and which violates the
above standards.  There is  nothing wrong  with the underlying  idea, to  be a
"userspace kernel", but it is the implementation that makes it inadequate for a
UNIX like system. We also want to be, and when we grown up, a "User Space Kernel".

Also usually distributions can use this init process to capture messages from the
kernel and log the booting process, in a way that will be useful for debugging.

At least in Linux systems, kernel messages are displayed in a console device, and
if anything go wrong with the booting process, such an init system can start up an
emergency shell, and which is actually the `dash` shell, a POSIX compatible `/bin/sh`.

Otherwise this same console device is being used for user authentication. After a
succesfull login the system spawns the default user's shell, which is a more rich
interactive shell than "/bin/sh", and which usually is the `bash` shell or the more
advanced `zsh` shell.

Interacting  through  a shell  is  the  most  primitive  and powerful  way  to
administrate and  use a  system, and  in a  Linux ecosystem  is capable  to do
anything that it doesn't requires a graphical environment (an X display).

And that  is what  we have to  implement at  our first stage.  We also  want a
utility that will offer a way to execute commands with superuser rights, so we
could administrate  the system.  Building those  two tools,  will allow  us to
slowly disassociate from our host.

## Implementation.

This is going to be in `C`, as the nature of the task, it requires direct access
to the underlying machine, and this is the main C property.

Another C  property, is that the  language do not  do anything by its  own, so
every little thing, like memory management,  should be done by the programmer.
So at  the same  time C  obeys truly the  spirit and  basic intention  of this
project, as we  have to implement ourselves at least  the basic functionality.
It is system zero point anyway.

Our only requirements at this stage is a standard C library (`libc`), plus `libpam`
as our authentication mechanism, and our building tool, which will be the `make`
utility, which is  also and the most primitive building  tool, since the birth
of `systemV`.

We're gonna to use some other core utilities, installed by default in all UNIX
systems, like `mkdir`, `ln`, `cp`, ... utilities.

Also the `sudo` utility (which is only required once to build its replacement) and
a `linker`.

The main development  environment here is a `Void` Linux  distribution, with GNU
libc, the  linker from GNU,  make from GNU, and  the core utilities  again the
ones that are provided by the GNU project.

Our `cc` (C  compiler) is GNU's C  Compiler Collection, and our  standard is the
last one published in 2011, known as `C11`.

But it may be build by others.

To build the zero basic system, issue:

```sh
  cd src
  make zero-shared
```

This will built first some libraries,  such as string type functions that deal
with  C strings.  Why? Because  it is  our  desire that  some day,  we can  be
independent by even a  libc (though these days are at least  a couple of years
away). At some  point we can try to  build one standard C library  in the same
location  with  the  rest of  this  system,  so  we  can use  this  system  as
an  Operating System  (OS).  In  any case  rolling  our  own libraries  offers
flexibility,  and chances  for  optimizations. Note  that  the libraries,  are
packed  as structures  and exposed  in an  Object Oriented  style for  various
reasons. Those Types,  will follow us in  every stage from now on,  and we can
refer them as the z standard c library. We'll introduce others in the way.

This command, it will  also built a quite minimal shell,  which we'll refer to
it as  [`zs`](data/docs/zs.md), and the  `zsu` utility  which is the  one that
will allow  us to  execute any  command with superuser  rights. Note  that the
latter is installed as  setuid root, that is owned by root,  and so capable to
destroy the system, and that this utility is at very early development. In any
case,  though we  really really  want to  be correct  and provide  an accurate
implementation,  we care  most about  to  describe the  procedure and  provide
a  solution  to  explain  some  why's. We  prefer  something  to  present  and
demonstrate, than nothing  at all. And as always,  development happens through
time and patches.

But  our main  intention  is  to describe  and  provide  the shorted  possible
solution  to  a  need.  And  at  this stage  we  care  only  about  the  basic
functionality. But as it has been said already a shell is capable to do almost
everything, this really depends of the  availability of a ecosystem. We'll try
to build some common tools in time.

It should  be noted,  that we  have linked against  shared libraries,  so this
system can be safely  used only in that same machine;  they will probably work
the  same,  on  a same  system  with  the  same  system libraries  in  a  same
architecture.

We also use  -Wl,-rpath=/path/to/libraries_directory. With this way  we do not
need to invoke  `sudo ldconfig -v`, and adding that  path to  the  linker  search
path. The -rpath option just says to  the linker, to  include this path direct
to the executable. So for now we'll use this option,  but probably  we'll  use
other method in later states.

Those targets can be built also statically, so they might be carried and  work
on different machines. Not always this is an option, as we'll see below.

To build the static versions, issue:
```sh
  cd src
  make zero-static
```
Note that the `zsu` utility, can not be built in my system, which lacks a static
installed `libpam`. Plus the compilation gives some warnings such:
```sh
zsu.c:190: warning: Using 'initgroups' in statically linked applications requires
at runtime the shared libraries from the glibc version used for linking
```
That probably means, that if we want to use that static tool in another system,
the linked libraries should match. There are reasons  for  this  probably,  but
that means that there is no much benefit for  linking  this  utility statically,
even if the compilation succeeded. And which it did, after  I  manually compiled
pam and cretated a static library. But there are many other functions that can
not be used safely, like many of the important nss functions or dlopen(), and
popular libraries like libcurl.

The  installed system hierarchy for now is:
```sh
  sys/$(uname -m)/{lib,bin,include}
  sys/data
  sys/tmp
```
We'll expand this later, with more directories and some explanations.

This is our  first state, and it is  the one that we care  most, regarding our
focus. This satisfy  our main fundamental principle, that  believes that every
product made by humans, should offer first (almost as prerequisitie), the most
primitive access level of the provided functionality possible. And is probably
wise to apply this principle rigorously when they are being extended.

We also believe  that the description shares the same  importance with the end
result. This  satisfy first  the inner human  curiosity, but  most importantly
allows a system to be further evolved and developed, by the human's realization
of how a system works underlying. Also by exposing the description, what it is
actually being exposed it is the intention. And by breaking down the pieces and
their intentions, the human's mind flow procedure (sooner or later, and if there
is a desire) will visualize the big picture as a sudden light.

This exact realization usually reveals the way to the implementation, either as
a new code or by modifying the existing.

Some words from the future (mid++ Octomber raining days and nights of 2021):
```C
 /*
  Later in the code path, we've been implemented a programming language, that
  suffers from two things:
    - very high code complexity to handle deallocations
    - there isn't a virtual machine, so optimizations are futile
    - there isn't a proper lexer/parser/compiler indepented code unit/namespace/phase

  Really it became really complicated to understand the intention of the code
  at places, and modifications are quite fragile with high probability to break
  the code logic, in another place of the code.

  However the language it is really nice one:
    - it is quite fast for what it does, that is to be the driver and leaving
      to C to do the underlying work
    - it handles the so desired deallocations (at least for the code that has
      been written so far (and it quite complex sometimes)), albeit with this
      hard an insufficient way it does
    - it is enough rich but and simple to use
    - and..., it is quite expressive.

  But why it ended up to be so bad code (though at the same time clever at places)?
    - I never wrote a programming language before. I'm not even a programmer with
      the common sense, I never was a proper student (again with the common sense).
      I just know enough to develop logic and design what it has to be designed.

      And as I can not learn by reading, as I have to be faced with the facts
      to realize a concept, the underlying implementation code, lacks the above
      fundamental for a programming language and well defined by now concepts.

  And so we are guilty, because we didn't followed the so called "break down
  the complexity in well defined pieces" UNIX wisdom mentality.

  A couple of days ago, I read the wonderful "Crafting Interpreters" book by
  Robert Nystrom. He says at some point, how hard is to incorporate at later
  phase a garbage collector, if you don't do it from the very beginning. I'm
  facing with this fact!

  Another true fact is though, that I could now recognize also the challenges
  of a programming language that are described by Bob, as I've been faced with
  them, and so by walking the path it is probably the way we have all the way
  up to realization at the end.

  I don't think that time will be enough generous to allow me to re-write it
  from the scratch. But it is quite fascinate to get a text file written with
  a special syntax, interpret it and transform it finnally to instructions.
  But I'm afraid that even if I do it at some point in time, it would probably
  be again the same language!!! So what it is the  point! But it is, it is my
  boy, as we may try to implement a Virtual Machine. So there is a temptation.

  In any case, all the programming concepts and algorithms, had their own scratch
  code written by someone who probably throw away it at some point for a rewrite.
  Or two. Or one and a half anyway.

  The wisdom here is to apply in your code the gained knowledge. It may looks
  that we haven't apllied much of it here. As for us, the whole point of this
  existance is realizations!!! Really.
  Realizations through the way to the implemention of intentions. In this case
  to glue the necessary components to build a system. And there
  is yet another reason. It is still eighties for us!

  For all the cases, mistakes are probably equally useful to be recorded, and
  it may be proved, that might help a lot to an understanding. My narrow view
  on this, that it might be proved, that all of it has been made because of a
  mistake, as the mistake that happens because of a will of an action, reveals
  a bit, which might sounds like an echo of the beat of the heart and which it
  might looks attractive to follow. Or something like this anyway. The thing is
  that we might look for a mistake, or an exception in the rule, or a backdoor.

 */
```

We have also to appreciate the availability of the required tools to build the
system. These tools are free to use them (free as free beer) by anyone in this
world. But it is so, because we obey  and serve, to the one and only principle
that really matters; that the source code should be free (free as in freedom here).

We can be sure, that the people of this world, wouldn't be possible to produce
this tremendous in size and quality code, in that so sort time, if they had to
pay for those tools, or if the code wouldn't be  available to study.  We don't
even want to imagine this possibility! So we owe a lot, especially in the GNU
project.
The open and free source model, it is for the benefit of all, and this  should
became quite crystal clear by now.

But this is also about justice. The tools should (should as an almost obligation
here) be available to the people of the world, no matter where they live and how
rich they are. Or their parents, or even their grand/grand parents.

It is believed that those are the ways to satisfy our uncontrollable desire for
the ultimate freedom and a pride path to our evolution, without hiding  behind
from our known by now convenient illusions (that probably they provide  to  us
excuses), to take a path that it might give us a chance to escape with a  style
from this eternal and painfull cycle, that always reproduces the same result.

Somehow and at some point all the atoms we should climb that hill to meet the
eternity. If we won a tiny wisdom, then we may realized that every single ring
of the chain it is equally and crucial important, as an indepented being that
holds an own wisdom, which it is required to tune and drive this single ship
in safe and fascinating waters.

In any case, and if we won a tiny and so hard to gain wisdom, then at least we
should try really really hard to make this a pleasant journey for every single
participant of the Now. By that desire alone, it is probably enough to attract
the lights of Justice that liberates, even if our convienences will blow up in
an infinite number of pieces to the space. Then we may feel that we are staring
our destination. Then it is up to every single existance to walk this last mile
of the path by their own will and with their own unique way.

And all of it is an Art by itself.

## Commands and Shell Interface.

So far, we've implemented an utility,  that can act as an intermediary between
the user  and the  kernel of  the Operating  System. This  implementation gets
input from  the keyboard, then it parses and interprets this text, and then
makes the request for this kernel service. This kind of utility is called a `shell`,
and it is the traditional UNIX system powerfull interface, and which served the
people through the very first days.

For instance in UNIX, if we wish to get a list of the contents of a directory,
we use the `ls` utility. If we use it without an argument, then it prints  the
contents of  the current  directory. A directory  is a just a file that  holds
the  names of other probably files. Those are references to a hierarchical tree
data structure, of a so called filesystem. One item of that structure is called
`inode`, and which  holds the file's data and metadata.
As we've been said already the root node of that list, in UNIX is denoted by the
slash (`/`).

The first utility that we've implemented, just to have something to work, so we
can develop slowly an application logic, is called `File.size`.
This utility is a frontend, of the underlying `File` library, which has a method
with the exact same name. This method uses a function from our standard C library
to get the number of bytes that are assiciated with the data of a filename/inode.
Here is the code:
```C
  static size_t file_size (const char *fname) {
    struct stat st;
    if (NOTOK is stat (fname, &st))  // NOTOK equals to -1 and is the usual code,
      return 0;                      // that libc functions return in cases of an
    return st.st_size;               // error. This error code it is stored in a
  }                                  // a special variable that is called errno.
```
The  `st`  structure contains  all  the   information about  the inode   that  is
associated with  the requested  filename. In  this function  we're interesting
about  the number  of  bytes, which  we  return  them to  our  caller.

It is callers duty to parse the user input, that usually comes from the keyboard,
before calling the function. Here is the call:
```C
  size_t size = File.size (filename);
  fprintf (stdout, "%zd\n", size);
```
Because the underlying function, doesn't really returns something meaningful, in
the case of a filename that doesn't exists (though it can be written better), it
is wise to check before the call for that case and so it could return a code back
to the environment, that will signal an error. And even more better is to  issue
a warning to the screen. The error code in those cases is always greater than 0,
as zero by convention equals with success in the UNIX command line interface.

The `fprintf()`  function, is asking from  the kernel, to print the output result,
onto the screen.

The output of a command can also be redirected into a filename, like:
```sh
  File.size Makefile > size_of_Makefile
```
In this case, the output was written into the "size_of_Makefile" file.

But also, the output of a command can be the input of another command, like:
```sh
  printf Makefile | File.size
```
Assuming that  the programmer of the  `File.size` command, wrote code  that will
handle the case when  the input is not coming from the  keyboard, but from the
standard input stream, the `Makefile` will be the input, as if the user has typed
this name in the keyboard.

This command chain is called pipeline, as the output of a command is the input
of the next command in the chain.

This  concept is  supported in  our shell.  In  fact, it  is going  to be  our
direction with a special way.

You maybe have noticed or confused, by the fact that the command and the underlying
function, are matching! This is totally unusual, in  fact nobody  else  has  done
something similar before, so we are going to deviate here, as we have to  offer
something bright new in this world! Now joking aside.

Our  libraries and their functions  are not  exposed with the traditional C way.
We choosed to expose their API (Application Programming Interface) as structures,
which their fields are pointers to functions.
Here is the signature of this specific one:
```C
  typedef struct file_self {
    ...
    size_t (*size) (const char *);
  } file_self;

  typedef struct file_T {
    file_self self;
  } file_T;

  /* and the simplified initialization: */

  file_t File_Type = (file_T) {
   .self = (file_self) {
     .size = file_size,
     ...
    }
  };

  /* the  following is a macro  (a bit of  syntactic sugar), that helps  the
   * programmer mind to focus on the implementation */

  #define File File_Type.self
```
Such a style is called usually  object oriented style, though in this specific
usage, there  is no object to  operate, since there are  no properties, though
they can be  added one day. It  is being used here for  code organization, and
because it offers flexibility, as the  user of this structure, it can override
this method with another function implementation, tailored in the needs.

This is a quite familiar pattern in programming, and because one of our primary
intentions is to develop a programming kind of thought, we really want to apply
such expressions in the command line, as it is almost a natural way of thinking.

This `namespace` kind of concept, it is also assists tremendously to categorize
applications, related to their functionality. We do that all the time. And also
that is what the programmers are trying to do when they baptize their functions
or their classes. Since the command line it is a public environment, this isn't
always a possibility for application developers. So quite many, if not the most,
they end up with a name, that isn't descriptive. I remember myself developing a
man page for "musca", which was an excellent fullscreen window manager for X, but
the name is reffered to a constellation. A really lovely name, if you are going
to ask my opinion.

In any  case the most important to keep here, is that in our system, we want to
unify usage and developing, as it is the actual reality.

So what to do if we want to extend the `File.size` command to get an attribute,
so it can print the size in Megabytes? We have two options. The obvious one, is
to use an extra argument. Easy and expected.  Some established  utilities, they
usually use for this kind type of arguments, `--blocksize=[SIZE]`, '-bytes`, or
`-h', a short option for `--human-readable` to append and [KMG..]. Recently the
`ls` utility, got a `--kibibytes` option to print the size  in 1024-byte block,
because Megabytes ended up to refer to 1000 bytes, because it was abused by the
hard disks manufactures, for marketing reasons, and  this now became a de-facto
standard.

Or we can use a symbolic link. This will work, because the fist argument in the
argument list (an array of strings) of the `main` function,  is the name of the
calling application. So we can develop logic in our code, based on that name and
dispatch our mechanism.  And this is polymorfism, and it is a common programming
concept, in which a function can choose, based on the type of its argument, which
function will call for its operation.
Many famous utilities uses this latter approach, like `busybox`.

Quite convenient mechanisms. And some fragile too.

We've also introduced pipelines and also commands that use them. Its not easy to
make them work reliabe, as there are many side effects:
```sh
  $ printf newdir/another/andanother | Dir.make --parents --verbose
  created directory: newdir, with mode 0755 (drwxr-xr-x)
  created directory: newdir/another, with mode 0755 (drwxr-xr-x)
  created directory: newdir/another/andanother, with mode 0755 (drwxr-xr-x)
  $ printf newdir/another/andanother | Dir.rm --parents --verbose
  removed directory: newdir/another/andanother
  removed directory: newdir/another
  removed directory: newdir
```
But what if we've used `echo` instead of printf? The echo utility it isn't  that
portable and the usual implementations they print a newline by default.
And what it determinates that the output of printf will be the input of `Dir.make`?

We assume it is not an argument. What to do in a case of an error and when we are in
the middle of the pipe, or our second command needs some info from the user (like
a password)? In the latter case, what it determinates that the arguments are for
the `sudo` utility and not for the command itself?

As a remark, a pipeline it looks like a `function composition`.

### The argument mechanism.

Here  we've used  two  arguments  known  to  everyone  that  used  those  tools.
The `--verbose` argument  says  to print  the result  to  `standard output`. The.
`--parents` argument says  to make any intermediate directory  Because they are.
standardized, we can also use their short version, which is, `-v` for `--verbose`.
and `-p` for `--parents`.

But though we learned to appreciate the power of the command line, somehow  it
looks, that soon or later, you will reach in a point, that will be not possible
to memorize, all these tools, and their arguments.

Fortunatelly modern shells offer all, a mechanism that is called autocompletion,
which is usually triggered by pressing Tab.

Using an  autocompletion mechanism, simplifies a  lot of things and  mutates a
crucial and valid  objection for our system. Since we want to create for  every
function a matching frontend, so quite verbose, our `$PATH` will be filled rather
quickly by our functions, so the lookup will be probably slow.

This is the reason, why our commands start with a `namespace` prefix and that
is why they begin with a Capital.

The other advantage is for the developers of this system. Since the name of the
command indicate an inner connection with a library, it is obvious where to look
for a fix or for further development. And on usual systems, this is  not  always
an easy fast case, but rather the exception.

## Semantics.

Slowly we'll have to set some semantics for our system.

We've already  implemented a "make directory"  command and in a  way, that can
receive the argument by reading the standard input. Here is the relevant code:

```C
  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    char dname[MAXLEN_PATH];
    if (NOTOK is IO.fd.read (STDIN_FILENO, directory_name, MAXLEN_PATH))
      ... logic to handle the error, as the function call was unsuccesfull
    else
      ...  continue logic below, as our request completed with success, and now
      we  have our input (like if the user has typed something in the keyboard),
      in the dname variable, by the read() method, of the File Descriptor
      Type/Class, which is included in the  Input Output class. In this case the
      File Descriptor class  is called subclass in programming consepts.
```
What is a file descriptor? Easy. It is a number!!!

Without joking,  yes it is a  number that the  kernel gives to a process, when
this process requests access to a file name.  This number for us, it is just a
esoteric kernel table, that in the case of a filename, it simply refers to the
underlying inode.

Couldn't have been  done simpler for us  mere mortals, as with  this number in
our hands, we now can access and manipulate the data.

Note that,  what is almost  unbelievable, is  that how ridiculously simple is to
read and  write back. The document  about [Pointers](data/docs/pointers.md) is
trying to explain some bits, by using the above example.

And one last thing  before we proceed. When a process  is a background process
or if it  is connected to a pipeline,  like in the above example,  it is being
said, that it has no controlling terminal; means that we can not input nothing
to that process through (lets say) the keyboard, as it isn't associated with a
terminal. In  such processes there isn't  a way to interact  directly with it,
but only through other mechanisms like through a socket.

In our code above,  `STDIN_FILENO` is a file descriptor that  refers to a kernel
table. By default this is 0,  while `STDOUT_FILENO` binds to 1 and `STDERR_FILENO`
to 2.

Now consider this shell session, which is using standard coreutils tools:
```sh
  $ mkdir test && cd test && mkdir -v ../test/-n
  mkdir: created  directory '../test/-n
  $ ls
  -n/
  $ echo something > file
  $ cat *
  1 something
```
Wrong and unexpected  result! It isn't a  bug. POSIX allows a  leading dash in
pathnames, so `mkdir` is conforming. The `cat` utility reads all the filenames and
outputs their  contents, but  in this case  what it happens,  is that  the cat
utility thinks  that `-n` is an  option to itself,  and so it includes  in its
output, also a number.

You can read a bit more in this [article](https://dwheeler.com/essays/fixing-unix-linux-filenames.html).

So we have to  make a decision. Should we allow a leading  dash, or a new line
or a tab or control characters, when we are creating a directory?

Our decision will set the semantics of our system.

Since in our  mind this doesn't make  sense, or at the  very least complicates
the code, we won't.  In the following code, we don't  even allow any character
outside of the `ASCII` range:
```C
  #define NOT_ALLOWED_IN_A_DIRECTORY_NAME "\t\n"

  if (*dir is '-' or *dir is ' ') {
    DIR_ERROR ("|%c| (%d) character is not allowed in front of a directory name\n",
        *dir, *dir);
    return NOTOK;
  }

  char *sp = dir;
  while (*sp) {
    if (' ' > *sp or *sp > 'z' or
        Cstring.byte.in_str (NOT_ALLOWED_IN_A_DIRECTORY_NAME, *sp)) {
      DIR_ERROR ("|%c| |%d| character is not allowed in a directory name\n", *sp, *sp);
      return NOTOK;
    }
```
Now, what would be prudent in our code, is that every time we need to create a
directory, we  should  call `Dir.make()`,  otherwise we might violate  the above
semantics.

In this case `Dir.make()` is our interface,  as it abstracts all the details for
us, and allows consistency, which is quite important to a system.

## Interface.

In this  sample system, we are  choosing to implement, a  user interface, that
will  resemble a  `vi(m)`  like  interface. Because  of  historical reasons  and
because is  intuitive,  this is  very popular  and already  established, as
quite many applications offer this capability.

A common  interface is quite  important for obvious  reasons, as it  allows to
apply the gained  knowledge everywhere with the same,  consistent and expected
way.

In our next  step, we'll implement an  editor, to ease us  in the development,
and to  administrate the machine. As  having a shell and  an editor available,
can really be more than enough many times.

Actually,  what  we  would like  to  have  to  feel  perfect, so  to  continue
development, is a way to execute some code.

The Emacs  editor, offers a  Lisp like interpreter,  allowing to its  users to
extend  a system,  plus comes  with  a quite  of wide  range of  applications.
Actually is more an environment, rather an editor. It is really more than just
an editor.

For  us, this  is the  moment, that  we have  to think  about our  development
environment.

But first lets build this really tiny vim clone:
```sh
  make e-shared
  make e-static
```

See the [documentation](data/docs/editor.md) about the provided functionality.

## Chrooting.

A chroot jail, is an isolated environment, useful as a build  environment,  or
to run unsafe applications without the danger to damage the host system.

This achieved by using the underlying `chroot()` system call, that changes the
root directory ([ch]ange [root]) for the current process, to a given directory
tree. Such a process can not access files outside of this directroy tree.

From the root directory of this distribution  (assumed a default installation),
issue:

```sh
# This is a here-document, where the string within the EOF delimiters, becomes
# the standard input for the cat command, which redirects its input to a file.
#
# We also surrounded the first EOF delimiter with quotes, so the text it won't
# be subject for parameter expansion, e.g., `uname -m` would be tranformed  in
# that system to "x86_64" otherwise.
#
# The first line is called a "shebang", and  it is the UNIX way to execute the
# specified program. In this case we call the system shell, which owe to parse
# and execute POSIX compatible scripts.
#
# After the redirection, we set the executable permission bits to this file, so
# we can execute directly this script. Otherwise we should call the interpreter
# explicitly, and pass the file as an argument, e.g. simply as: `sh file`.
#
# At the end, we execute this script passing our shell as an argument.
#
# Note, that the hash symbol ('#') is considered as a start of a comment and so
# the above lines and this one, are ignored by the shell.

cat <<- 'EOF' >chroot.sh
#!/bin/sh

CHROOT=sys/`uname -m`

umount_sys_and_exit () {
  umount $CHROOT/dev/pts
  umount $CHROOT/dev
  umount $CHROOT/tmp
  umount $CHROOT/proc
  umount $CHROOT/sys

  exit $1
}

mount_sys () {
  mount -o bind /dev    $CHROOT/dev      || umount_sys_and_exit 1
  mount -t devpts none  $CHROOT/dev/pts  || umount_sys_and_exit 1
  mount -t tmpfs tmpfs  $CHROOT/tmp      || umount_sys_and_exit 1
  mount -t proc proc    $CHROOT/proc     || umount_sys_and_exit 1
  mount -t sysfs sysfs  $CHROOT/sys      || umount_sys_and_exit 1
}

make_sys_hierarchy () {
  test -d "$CHROOT/dev"  || mkdir $CHROOT/dev         || exit 1
  test -d "$CHROOT/sys"  || mkdir $CHROOT/sys         || exit 1
  test -d "$CHROOT/proc" || mkdir $CHROOT/proc        || exit 1
  test -d "$CHROOT/tmp"  || mkdir -m 1777 $CHROOT/tmp || exit 1
}

if [ ! -d "$CHROOT" ]; then
  printf "$CHROOT not a directory\n"
  exit 1;
fi

make_sys_hierarchy
mount_sys
Sys.chroot $CHROOT "$@"
umount_sys_and_exit 0
EOF

chmod 755 chroot.sh

zsu chroot.sh /bin/zs-static
```
A few observations.

First we observe, that we need special priviliges to  execute this script,  as
the chroot system call requires such priviliges. In fact our UID and GID (user
and group identification respectively) are both 0, which are the user/group id
for the root user. This can be fixed, if we extend our chroot tool, to set the
uid and gid of the called process, to some given by the user arguments. And we
could probably do that, if the chroot mechanism was safe enough. But it isn't.

It has been long [demonstrated](data/docs/escaping_from_a_chroot_jail.md), and
proved by development and history, that you can actually escape from  a chroot
jail. Hardly ideal.

Also observe, that we can only run static compiled objects. Such objects,  are
collections of other possibly objects, that are linked into the program at the
compiled time, and which are archived together to form a standalone object.
Because of that, they don't require at the runtime their dependencies, but are
a lot larger objects than their shared counterparts, and should  be recompiled
everytime a dependency has been modified. They are initialized faster, but are
less gently with the resources as they do not share them with other executables.
They could be practical in cases, but not economical.

But lets see the dependencies of a shared object:
```sh
  ldd sys/`uname -m`/bin/zs-shared
```
(and its output that is shortened here for brevity)
```sh
  libdir-0.0.so => libdir-0.0.so (0x00007f61bb3ce000)
  libvstring-0.0.so => libvstring-0.0.so (0x00007f61bb3c8000)
  ... more internal libraries
  linux-vdso.so.1 (0x00007ffcb6197000)
  libc.so.6 => /lib/libc.so.6 (0x00007f61bb1d1000)
  /lib/ld-linux-x86-64.so.2 (0x00007f61bb3db000)
```
Of all we are more interested about the last three.

The vdso.so (virtual dynamic shared object) is a library,  that  executes  system
calls in the user space, instead of the kernel space.  These are  kernel  internal
implentation details, but rather out of scope for us mere humans, but see `man vdso`
for more information if desired.

Then there is the standard C library (`libc`). All the libraries are prefixed with
lib in their name. But we yet have to install a libc in our environment. Of course
we can use the one by the host if we want!  So the missing of a libc,  which  is a
requirenment, is one of the reasons that we can not run shared binaries.

The last one `ld.so` is the one we care here most.  This  is a program that locates
and loads shared libraries (like the libc in this case) and which it is stored in a
special section (.interp) in an ELF object, but it can also be  executed   directly
from the command line. The below command displays this specific information.
```sh
  readelf -l  sys/`uname -m`/bin/zs-shared | grep "interpreter"
  # outputs
  [Requesting program interpreter: /lib/ld-linux-x86-64.so.2]
```

So it is true, that we can't really continue development without installed first
a proper development environment.

We have also to create a proper hierarchy, character devices, a password database
and so on. For some of these reasons, we've also "binded" our /dev[ice] directory
and some special devices, to our system directory.  We also  created  and mounted
some special directories, like /proc and /sys and with a special filesystem type,
which a Linux system requires to be functional.

## Development Environment.

But what are our  choices? Should we continue with C?  A scripting language is
also  an option.  The  Rust language  is  also an  option, as  it  is the  zig
programming language.

What is interesting about zig?

The first thing is that it is a low level language like C, and it is almost as
simple as C is. Almost. As it is hard to beat C in clarity, as its grammar  is
very small. But it has a more secure memory model, as it allows to choose your
memory allocator, depending from the context. It has also a builtin mechanism
to build modules, plus  it can compile  the same code in almost  every  known
architecture.
You can also insert test blocks directly into the sources, which are called if
you invoke the compiler in testing mode. As a plus, it treats conditional/loop
statements as expressions, thus allowing a more natural flow when developing!

Quite attractive reasons and all of them without using external tools.

But it has not yet reached in a stable milestone, plus it still requires LLVM,
as it can not yet compile itself.

There is also Rust, in fact we are actually living in the kingdom of Rust.
The meaning of the existance of Rust is to become a safe system language, that
will offer, close to C[++] performance, with a model that is called  borrowing
and owning, which is a bright new system, that programmers should  adjust  and
dedicate some time to understand.

We know that C is not safe.  It doesn't do any automatic memory managment, and
the compiler it will not prevent you or even warn you usually, from blowing up
the system with out of bound operations, or integer overflows.  I respect  the
worries for a better world. But...

To be continued ...

## ZERO_POINT_ZERO_INSTRUCTIONS
```sh
  cd src

  # shared targets
  make shared && make e-shared && make la-shared && make v-shared

  # static targets
  make static && make e-static && make la-static && make v-static

  # Note, that the installation of the zsu utility requires the
  # sudo utility, as it has to be installed as setuid root.
```
The development environment is gcc, but it should be compiled with  clang and
tinycc compilers, without a warning, else it is considered an error. Note that
the interpreter, it crashes when it is compiled with tinycc.

## ZERO_POINT_ZERO_STATE
So far, we've implemented:
  - an ala [vim](http://www.vim.org) [editor](data/docs/editor.md)
  - a [terminal multiplexer](https://en.wikipedia.org/wiki/Terminal_multiplexer) [window manager](data/docs/v.md)
  - a very early draft of a [shell](https://en.wikipedia.org/wiki/Shell_(computing)) [utility](data/docs/zs.md)
  - an early draft of a sudo like utility (audit review required)
  - a tiny [interpreter](data/docs/i.md), that has been later [evolved](src/data/semantics/Language.md) significantly
  - a couple of commands that initialize the libraries (for development reasons)

## ZERO_POINT_ZERO_CONCLUSION
  ... todo

## ZERO POINT DEVELOPMENT

## Interpreter development.

We took a bit of break here, to dedicate some time and focus a bit  with the
interpreter.

Our development tools are capable to do basic administration, but we can not
extend our system, from within the isolated system. What we have only is our
humble tiny interpreter.

This one is derived from [tinyscript](https://github.com/totalspectrum/tinyscript).

Tinyscript was written for a memory constrained system, where its developer
wanted to apply some simple logic at runtime, with as few resources possible.
That's why it doesn't handle strings, signed integers and why it doesn't use
dynamic allocation. It just  reserves at the initialization some memory from
the stack in a memory arena, with a size that is defined  at the compilation
time.

In this memory arena it stores the symbols (starting from the begining of the
arena and going forward), and values (begining from the end of the  arena and
going backwards), both typedefed as `intptr_t`. So a lookup for a value it is
going back from the end, by decrementing the value pointer.  If this  pointer
meets the symbol pointer, then the system it gets out of memory.

All that all in a little more than a thousand of lines of compact code, to do
all the required and well defined tasks for an interpreter, such as scanning,
parsing and evaluation.

We have used this little machine at the begining, to save and restore  editor
sessions, and later we applied the same technique and to the virtual terminal
interface. And we could also use it as a scripting environment, in our shell!

It's not a surprise, that this tiny code it's enough, for a direct connection
with C. Since `intptr_t` is capable to store pointers, then the C abstraction
function signature interface, it uses direct the objects without even casting.
It seems that you can bind the whole C universe with it!

We took the freedom to modify tinyscript and to adjust it to our environment.
The first thing to do was to use dynamic allocation, so to avoid any out  of
memory operations, as long the system itself it doesn't run out of resources.

The next thing to do, was to remove any static declared symbols and put  them
in a struct (and with our own interface code), and which we are passing as the
first argument to the internal functions and also to the C function interface.

We also added basic string (and with multibyte) support, and some  established
operators like `+=, -=, /=, */, ...`, we modified the print function to handle
multibyte strings, and also we extended it with interpolation expressions, and
added some keywords, that are being used also in this codebase, like `is, isnot,
ifnot, ...). We also added code to handle operations on signed integers.
We've also used internally our own datatypes, and with the open possibility to
expose them to the interpreter, and extend it with more rich types, like: maps,
arrays, lists and our string type functions, and so on.

But the real value, is that we've used this, to realize a bit the machinery of
an interpreter, so we took the happy path to add support for nested functions,
and even lambda functions! Of course, this added code and a lot of complexity!
And because tinyscript is what is tinycc for the C language!  (the underlying
machine, it doesn't really separate the conserns and tries to do many  things,
with as few code is possible), and because it wasn't really destined to handle
the level of a programming language complexity, the result of those convenient
extensions, is at places of course some spaghetti code.

There isn't a type system, there isn't a Value Type, it is just an `intptr_t`,
which says nothing.

But it's not doable to hold a double!

There are some nice methods, that are called NaN tagging, or tagged  pointers.
I was close enough to finish the implementation, but when i tried the code  to
a 32bit machine, the compiler complained for the underlying type.

This method is using `uint64_t`, which is big enough to hold a double, but  it
also leaves a couple of bits free to store an information about the underlying
type, which is a member of a `union` with at most eight types free to use.

This magic is being used in quite of many languages, like lua[jit], javascript,
and became quite known with the [Crafting Interpreters](https://github.com/munificent/craftinginterpreters) book,
from Bob Nystrom. I first saw it, when i've been involved in [Dictu](https://github.com/dictu-lang/Dictu),
which is an excellent implementation of the Book, from my friend Jason.

But though it is economical and fast, it is too much "hardwarish" for my taste,
as I do not really understand all those bits, how they really work. Or I do, but
I do not have such enthusiasm to hold them in my mind, so i never really learned
them really. I just use them in safe cases, but without really knowing them.

What I like most is to design and implement primitive interfaces, and that is
why I like C, because it is just a thin abstraction! If it wasn't for all those
types and their slight details and the slightly silent UB.

So, and if we continue with the interpreter in a new territory, then we need a
more flexible type, and a little bit more costly though than an  `uint64_t`  i'm
afraid, and for sure not that fast. To be honest here and as a confess, I never
really cared __that__ much about performance. it is just that happens that C
offers that performance, but it feels more like a gift! The real priority  and
desire, was always the most straight access to the kernel, and  with the  most
simple best expressed method that is provided by the underlying environment.

For POSIX like systens, this is the `shell`. The joke here, is that despite how
it might looks, it is also and the most intuitive one for a human. It has been
proved also, that is and a clever investment, because if you learn it once, you
learn it for ever, and you use it everywhere the same way, as it is the  same
standardized interface everywhere, and so it isn't vulnerable to any specific
system choices of the aesthetics, or useless (usually) overcomplexities  with
a really waste of valuable resources for just an interface. So C offers  this
direct connection with the underlying system hardware and kernel, without any
interpreter in between. For a small price at the expressivity and without any
way to interfere with the system at runtime. So we need at least __One__.

We are actually walking at the One! That is where we're going, and we want to
be stable and end up on things with our API, when we reach at this point. If we
don't do that, we have to be prepared to introduce incompatibilities,  during
the logical evolution.  This is almost always a frustration for a developer to
adapt, especially when even point releases introduce such incompatibilities.

C success proves that simple fact, since C nener changes, or it changes in a
gradual way, leaving a decade between new revisions and standards. However it
is guarranteed that ANSI C code, will be compiled in every known platform.
And that is also the reason, why we have to appreciate POSIX, and why it is so
critical to base our lifes in Open Standards that they standardise established
and accepted practicing, which is proved useful, logical and stable in time.

So if we are inclined to continue to work in the interpreter, then we have to
introduce those API incompatibilities now, rather at a later state. So  it is
time to move on from our zero point, and start walking at the zero point zero
point one.

## ZERO POINT ZERO POINT POINT ONE (0.0.1)

We continue in this state, from the point we ended up at the bing bang state.

We did, what we told. We changed the type of the symbol's value, which made it
now incompatible with the C function interface signature.

This allow us for flexibility, but as we predicted we added a lot of overhead
too. A quick test, showed us a serious decrease in performance. Now it is the
time to test ourselves for conformance with our words. We said that we  favor
expressionism over performance! Well, we have to arm with courage and logic,
and face the reality.

As we said, the underlying machine can not pretend that has the mechanisms to
handle that level of complexity of a well defined programming language. But we
went ahead and added the posibility, to pass a function pointer as an argument
to another function, or to store it in a variable. This is really something!

Plus it is also capable to handle double types. Not that I know to handle them
properly. I don't. This is a science by itself, and you need a proper education
and serious study on the subject to gain the expertise. So quite quite probably
functions on doubles they need some adjustment. Right now, the code avoids any
bitwise operation on them. The thing for instance is, what it has to return in
that case? There are many subtleties like this, but the mechanism is there.

For some of those reasons, we introduced this development as different concept,
as its better not to mix for now these namespaces and wait a bit. For instance
even the previous code, could handle both the cases probably, with some bits of
hacks though, and quite possible in a such way that it might make this fragile
code even more complex to maintain and evolve. Not quite possible. For certain.

For this new code, that was placed in a Namespase called `La` (for Language),
there is a special [test unit](src/data/semantics/language/l-semantics.lai), which is a copy of
the old tests, plus specific tests, like tests on function pointers and doubles,
and as well its own [documentation](src/data/semantics/Language.md).

## 0.0.1 DEVELOPMENT

  - we've used the new machine in our editor as an extension language. Of course
    as we said there is a performance penalty, but this is noticeable only with
    an abnormal huge amount of iterations. And its not _that_ bad. Okey.  It's
    bad. But `double` support it is a prerequisitie, so we had to deal with this
    at some point.
    Now we have two types of Numbers. An Integer Type, and one that should be the
    largest quantity of one can gets, and can be used as argument to functions,
    that operate on floating point numbers. These are typedef'ed as:

```C
      typedef ptrdiff_t   integer;
      typedef double      number;
```

  - The `ptrdiff_t` type is in standard C, and it is the adequate type to handle
    pointer arithmetic. We also assume that an allocator can claim at most PTRDIFF_MAX.
    So our Pointer Type is an alias to `integer`.

    There is also a C string Type, though the support is really basic right now,
    and probably it is a good idea to handle some operations on them, with the
    same way that C does.

    Ideally, we have to have a more flexible String Type to work with strings
    in a more flexible and safe way. As we have the machine, the question is the
    interface. We can go a bit far and handle them with an object oriented way!
    If we take that path, the support should be generic though for consistency.

    So and as conclusion, we complicated a bit the C interface, as now all the
    arguments are `VALUE` types and the function should cast them properly. The
    first draft of the transition works flawlessly though, so it is just a matter
    of discipline to get them right. We've tried to leave them unchangeable, by
    assuming that the pointer to the struct `VALUE` is a pointer to its first
    member, that is the union actual value. But this didn't always worked, so
    perhaps this is not guarranteed. I do not know.

    As a small test to the new interface, we've introduced an implementation of
    an evaluation register for characterise visual mode, which can be extented
    and cooperate with the expression register which currently does nothing.

    And many small cases that previously wasn't possible or difficult to handle,
    now its just a matter of an extra logic. Like the case of arrays, which the
    current implementation didn't allowed to pass them as arguments to functions.

    This also reveals and the future route. We'll simply make it more slow, by
    adding a couple more of features and convienences!

## ZERO_POINT_ONE_STATE

And so we did, with many added features and convienences, during those last
three months, and now we have a way to extend the system in a dynamical way.

But this development fulfiled one other purpose too. The idea for a Common Programming
Language Specification. This is an implemetation of such a specification, which it is
believed that can be used as a base, to define the semantics and its syntax.

As a small synopsis of the development of this system, and as of now (20 July),
have been implemented:

  - an ala [vim](http://www.vim.org) [editor](data/docs/editor.md)
  - a [terminal multiplexer](https://en.wikipedia.org/wiki/Terminal_multiplexer) [window manager](data/docs/v.md)
  - a very early draft of a [shell](https://en.wikipedia.org/wiki/Shell_(computing)) [utility](data/docs/zs.md)
  - an early draft of a sudo like utility (audit review required)
  - a [programming language](src/data/semantics/Language.md)
  - a little bit more than a couple of commands that initialize the libraries and
    for first time the programming language

## ZERO_POINT_ONE_STATE_INSTALL_INSTRUCTIONS.

Requirements:

The make utility, a C compiler, a linker, a libc and libpam (for the sudo replacement).

```sh
  cd src

  # note that to clean up previous libraries and utilities, issue:
    make REV=0 clean-shared
    make REV=0 clean-static
  # or/and
    make REV=1 clean-shared
    make REV=1 clean-static

  # shared targets
  make shared && make e-shared && make la-shared && make v-shared

  # static targets
  make static && make e-static && make la-static && make v-static

  # Note, that the installation of the zsu utility requires the
  # sudo utility, as it has to be installed as setuid root.

```
Tested with `gcc` but the targets they should compiled with clang also.
The compilation should endup without a single warning, even with DEBUG enabled.
To do that use:
```
  make DEBUG=1 shared
  # or/and
  make DEBUG=1 static
```

## TRUNC:

 # Implemented:

  - a visual [editor](data/docs/editor.md) that is a tiny [vim](http://www.vim.org)
    clone, and which can be invoked as:

     E or E-static [options] [filename[s]]  
     E --help  # for a short help

   sources:  
     [E  library](src/z/lib/E/src/libe.c)  
     [executable](src/z/app/e/src/E.c)  

   state: rather stable

   maybe_crashed:  
     - under circumstances with a series of undo/redos  
       reproducible: a bit hard, as it does not happens on continuously
       undos/redos and without to change the cursor on different line between
       the two calls, and without to insert new lines

       cause:  For certain it is the jump code that calculates wrong the line numbers.  
       state: This is good to fix, but since this is not my style to work on an editor, it
         doesn't happen in the workflow, there is an inclination to devote time.
       [update at 12 of February: I was able to reliably reproduce it, and commited
         a fix (88cea19a0b75ca4598db3ff313a71567acc9e7ce), which fixed the case
         i could reproduce, and it seems that this is also the underlying bug;
         but time will tell though.]

     - under circumstances when writting with characters > ASCII_RANGE  
       reproducible: needs some time but possible. Write some greek and make
         various moves like backspace/delete.

       cause: easy (wrong calculation, when going [for|back]ward)  

       state: very good to fix as it is annoying, but need to have time and will

       [update: it happens also and with lines with tab characters (Makefiles),
         (it doesn't crash but it produces and renders unexpected results).
         It is annoying and should be fixed, though i still have to find the will.

    - not a crash but a visual thing. Sometimes in a combination of operations
      the cursor is not visible in insert mode. The fix should be a simple refresh
      but i have to reproduce when happens and never tried that hard.
      

  - a [terminal multiplexer](https://en.wikipedia.org/wiki/Terminal_multiplexer) [with window managment and [dea]ttach capabilities](data/docs/v.md) called as:

     V or V-static [options] socketname  
     V --help  # for s short help

   sources:  
   [V  library](src/z/lib/V/src/libv.c)  
   [WM library](src/z/lib/Vwm/src/libvwm.c)  
   [executable](src/z/app/v/src/v.c)  

   state: rather stable  
   crashes: not with current usage, but quite possible there are unhandled cases

  - a [programming language](src/data/semantics/Language.md) called as:

     La or La-static [options] [filename[s]]  
     La --help  # for options  

   sources:  
   [La library](src/z/lib/La/src/libla.c)  
   [executable](src/z/app/v/src/v.c)  

   state: on stabilization. Syntax and semantics are hard to change.  
   crashes: it is an ongoing work and it is quite fragile, but not
     with current code
     [update: crashes in recursive functions and when there two consecutive
       calls to itself. This bug still exists. Really i almost never use
       recursive functions, and when i do, i use return statements in tail call
       position.]

  - many libraries  
    state:  most of them are mature, used for a couple of years. But
      are unoptimized, and some of them written in the first days of
      C kindergarden (still in the last grade). But no crashes for a
      long time if the memory serves me well.

    sources: (some of them)  
    [String   library](src/z/lib/String/src/libstring.c)  
    [C String library](src/z/lib/Cstring/src/libcstring.c)  
    [File     library](src/z/lib/File/src/libfile.c)  
   ....

  - applications:  
    state: mostly basic unix utilities, and it is really an ongoing work,
      (used mostly for testing the development). Many of the usual options
      haven't been implemented.

      update (last days of Octomber): quite bit of the basic set of a system
      command line functionality (core utilities) has been implemented.

    sources: (some of them)  
    [Dir.make in C](src/z/app/dir/src/Dir.make.c)  
    [File.stat in La](src/z/app/file/src/File.stat.lai)  
    ...

  - a shell  
    state: don't never tried to work with it, as there are other areas to
      dedicate attention (it is not a priority, as the direction might change) 
       
      updates:  
        Sweet November Days and olive season: This is now at least a little bit
        more useful with command, argument, filename, last component completions
        and shell word expansion.
       
       End of the year: This is now quite functional and useful.
       
    sources:  
    [document](data/docs/zs.md)  
    [Sh   library](src/z/lib/Sh/src/libsh.c)  
    [Proc library](src/z/lib/Proc/src/libproc.c)  

  - an early draft of a sudo like utility  
    state: audit and careful review it is required. It works for my usage and
    I use it a lot. But there should be unhandled cases. It is just a proof of
    concept mostly and written in the very first days.

    sources:  
    [Auth Library](src/z/lib/Auth/src/libauth.c)  
    [executable](src/app/zsu/src/zsu.c)  

    notes:  
      - libpam is required (we might use another option one day, like crypt())
      - likewise the sudo utility is required at the build time, as the executable
        is installed as setuid root, it needs special priviliges.

  - a package manager  
    state: really early, but it seems to work with a couple of specs. It builds
      the static libpam library though.

  - a container  
    state: early but it works.

    This is an important point at the coding path, because there is now a way
    to see how the (disassociated from the host system) system works alone and
    without the flaws of a chroot jail.

    sources:  
    [Contain Library](src/z/lib/Contain/src/libcontain.c)  
    [executable](src/z/app/contain/src/Contain.new.c)  

    Usage:
      Contain.new sys/`uname -m`

Requirements:  
  The make utility, a C compiler, a linker, a libc and libpam  as zsu dependency
  for now (as we  don't really need it at this stage), so it has to be moved
   from the initialization system state).

```sh
  cd src

  # note that in order to clean up previous libraries and utilities, issue:
    make REV=0 clean-shared
    make REV=0 clean-static
  # likewise for other releases
  # Development note: the above should be one target. Possible a different Makefile.

  # shared targets
  make shared && make e-shared && make la-shared && make v-shared

  # static targets
  make static && make e-static && make la-static && make v-static

```
Tested with `gcc` and clang C compilers.
Also the tinycc compiler it builts succesfully the shared targets, but not the
static targets.

The compilation should endup without a single warning, even with the DEBUG flags
enabled.
To do that use:
```
  make DEBUG=1 shared
  # or/and
  make DEBUG=1 static
```
  DEBUG_FLAGS:
     -Wextra -Wshadow -Wall -Wunused-result -Wunused-function -Wunused-macros -Wno-override-init
     -Werror-implicit-function-declaration -Wsign-compare
     -Wpointer-arith -Wreturn-type -Wmissing-declarations -Wwrite-strings
     -Wuninitialized

By default libraries and applications are installed in sys/`uname -m` namespace.

For shared targets, normally you do not need to add the library path to ld.so.conf
and call `ldconfig`.
If a call to an executable fails with a message that linked libraries can not
be found, you may want to prepend LD_LIBRARY_PATH=sys/`uname -m`/lib/z in the
command line.

For convienence you may add the sys/`uname -m`/bin to PATH environment variable
to your shell environment file.

There is no support for installation to the `/` namespace.

[Current development](src/y/README.md)

## License:
I  understand the UNLICENSE license  kind of thought. We  do not need laws and
licenses to make the right thing.

But I respect a lot the GNU philosophy and I feel that we owe a lot to the GNU
project. We  can be sure, that  we wouldn't be here  if it wasn't the  GPL and
GNU. And Richard!

That is the reason for the GPL2. However, our actual law should obey in common
sense and an inner will, to do what we have to do by our own, and not  because
we have to do, because they told us to do. Our law should be streaming  by the
gained consience. Hopefully one day.

## Acknowledgements, References, Algorithms, Libraries:

* [Linux From Scratch.](https://linuxfromscratch.org)
* [VoidLinux: A Linux distribution without systemd.](https://voidlinux.org/)
* [Mir: A jit compiler library.](https://github.com/vnmakarov/mir)
* [Tinycc: A tiny C compiler.](https://repo.or.cz/tinycc)
* [Tinyscript: A very simple scripting language.](https://github.com/totalspectrum/tinyscript)
* [Tagha: A small process virtual machine.](https://github.com/assyrianic/Tagha)
* [Zig: A generar purpose programming language.](https://ziglang.org/)
* [Crafting Interpreters.](https://github.com/munificent/craftinginterpreters)
* [Dictu: A simple programming language.](https://github.com/dictu-lang/Dictu)
* [Build Your Own Lisp.](https://github.com/orangeduck/BuildYourOwnLisp)
* [A Programming Language in Javascript.](http://lisperator.net/pltut/)
* [SLang Programming Language.](https://jedsoft.org/slang)
* [Nan Boxing.](https://piotrduperas.com/posts/nan-boxing)
* [Nan Boxing.](https://leonardschuetz.ch/blog/nan-boxing/)
* [Bootstrapping an Operating System.](https://gitlab.com/giomasce/asmc)
* [Live Bootstrap](https://bootstrapping.miraheze.org/wiki/Live-bootstrap)
* [Simple Shell.](https://notabug.org/rain1/s/)
* [Linenoise (Steve's Bennet fork).](https://github.com/msteveb/linenoise)
* [Argument Parser.](https://github.com/cofyc/argparse)
* [Terminal [de|at]tach.](https://github.com/crigler/dtach.git)
* [Splitvt by Sam Lantiga.](https://slouken.libsdl.org/projects/splitvt/index.html)
* [Contain: A simple implementation of containers for Linux.](https://github.com/arachsys/containers)
* [OpenDoas.](https://github.com/Duncaen/OpenDoas)
* [Man utility.](https://github.com/rofl0r/hardcore-utils/blob/master/man.c)
* [Scdoc man page generator.](https://git.sr.ht/~sircmpwn/scdoc)
* [Suckless sbase.](https://core.suckless.org/sbase/)
* [Libdeflate.](https://github.com/ebiggers/libdeflate)
* [Microtar.](https://github.com/rxi/microtar)
* [Par.](https://bitbucket.org/amc-nicemice/par)
* [Super Light Regular Expression Library.](https://github.com/cesanta/slre)
* [Lightweight snprintf implementation.](https://github.com/mirtoto/snprintf)
* [Mini snprintf.](https://github.com/tatetian/mini-snprintf)
* [nanoprintf (ftoa()).](https://github.com/charlesnicholson/nanoprintf)
* [utf8.h.](https://github.com/sheredom/utf8.h)
* [is_utf8.](https://github.com/JulienPalard/is_utf8)
* [Libtree: ldd into a tree.](https://github.com/haampie/libtree)
* [Frexx CPP C Preprocessor.](http://daniel.haxx.se/projects/fcpp)
* [PCG Random Number Generation for C.](https://github.com/imneme/pcg-c-basic)
* [UUID generator.](https://github.com/rxi/uuid4)
* [A minimal C Runtime.](https://github.com/lpsantil/rt0)
* [Minibase: a base system and early userspace for Linux.](https://github.com/arsv/minibase)
* [Neat Libc.](https://github.com/aligrudi/neatlibc)
* [Diet Libc.](http://www.fefe.de/dietlibc)
* [Embedded Artistry libc.](https://github.com/embeddedartistry/libc)
* [Mlibc.](https://github.com/managarm/mlibc)
* [Picolibc.](https://github.com/picolibc/picolibc)
* [klibc from Linux Kernel.](https://git.kernel.org/pub/scm/libs/klibc/klibc.git)
* [Math library.](https://www.fixscript.org/blog/math-library)
* [Custom Malloc.](https://github.com/miguelperes/custom-malloc)
* [Dynamic Memory Allocation in C.](https://github.com/RAGUL1902/Dynamic-Memory-Allocation-in-C)
* [Simple TCP.](https://github.com/riyadhar8/TCP-Simple-Server-Client)
* [Http Status codes.](https://github.com/j-ulrich/http-status-codes-cpp)
* [Parse URL.](https://github.com/wc-duck/url_parse)
* [SunRiseSet.](https://github.com/troglobit/sun)
* [Xsel.](https://vergenet.net/~conrad/software/xsel)
* [Sudoku in C with classes!](https://github.com/vaithak/Sudoku-Generator)

## About
Inner will and Motivation.  
This isnot just a proof of concept, though it might feels that way. It took me
fifty something years, and as looking back in time, to realize that  there  is
a big possibility that every single breath had to prove something, though what
really was my intention is to catch the uncatchable now.

Hi, i'm you host, and i might lived (and without even realizing it for a long,
that this might be not common), to wait for a wave to climb and ride, to drive
this flow with the will against a wall, like a war without war.

I started programming around 2010, though actually it was around the winter of
2012, when some special circumstances and the situationism allowed me to study
programming, by implementing the first application (a personalized environment
which was a set of applications with a common interface), written in S-Lang. I
started with C at my 52 (in 2019), so and according with the gained wisdom, at
this time of writing (April of 2021), I just graduated from the kindergarden. So
this is actually and a research that will provide proves and some qualifications!!!
I hate to live for just proving, and especially when the proves, prove me wrong.
Now, on what to do with them! Anyway, almost all the followed code, after this
first scratch code, it was simply mostly iterations of that same system.

My interest is about primitive systems, that can use the most straight and the
most economical access to the underlying machine, with a standardized (through
time and experience) and intuitive interface, that can be adjusted to the user
personality, when interacting with this cold and unemotional machine. But  not
with an apathetic and ready served way, but through a will to gain at least  a
primitive knowledge, that through understanding the underlying implementation,
will be able to modify and evolve a system through that interactivity, if that
system respects standards and obeys to expectations. That simply means that to
achieve this intention, you can not really separate the actual  implementation
(sources) from the interface, though the abstraction draws an obvious  visible
line between those two consepts, but in reality the're so tied together,  like
two partners that dancing in the ice. The destiny is the same. Ideally an  ill
system should can be cured at runtime, possible by re-evaluating a part of its
algorithm.

And that was the main advantage of this first (made by a novice mind)  system.
Since then, and in all the iterations, never really achieved again  this level
of pure simplicity, to modify and mainly evolve a system at runtine, without a
need of restarting. How it did it? Simply any code was re-evaluated  again and
again, reading from the disk. Obviously not a real economical way, but yet  an
efficient and exciting way, as you could see the behavior that was changed, by
modifying a function. Such code that was proved to make the right thing, ended
up to be part of the core. It was that time that I adopted the `waterfall model`
as a testing practice, and has being used since then as an excuse to avoid of
writing testing suites. I miss those days.

And I miss the life in our poor village, in the wide valley of Thessally, as a
really young kid working in the cotton fields, under forty+ degrees at the hot
summer sun eating watermelons, under some grass roofs into the canals,  having
around some millions of frogs, producing their pretty loud amazing paradoxical
music at the silent nights of the full of energy valley.

αγαθοκλής (laki)
