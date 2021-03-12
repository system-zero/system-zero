------------------------- [ zero point draft ] -------------------------------

[0.0 State](#ZERO_POINT_ZERO_STATE).

[Skip the description and goto the install instructions](#ZERO_POINT_ZERO_INSTRUCTIONS).

## Description

This is  to describe and implement  a functional computer system,  with as few
resources  is possible,  and provide  our own  solution for  as many  specific
computer tasks is possible.

Since this is a quite huge task, we'll  have to extend our system in a gradual
way, so  we have to divide  it into stages. At  the end of any  stage, the end
result should be in a usable state.

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

This procedure is called Init Process, and traditionally in a systemV, this is
done through a very simple mechanism. Such a procedure it looks in a directory
for files, which are usually shell scripts and loads them by usually using the
default shell  in UNIX,  which simply is  called `sh`, and  by default  found as
`/bin/sh`. This location  is standardized by the POSIX standard,  and this shell
is referred as POSIX `sh`.

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
"userspace kernel", but it is the implementation that makes it inadequate to a
UNIX like system,  which is based on modularization and  expectations. We also
want to be, and when we grown up, a "User Space Kernel".

Also usually  distributions are using  this init  process, to log  the booting
process and the  messages from the kernel,  in a way that will  be useful for
debugging.

At least in Linux systems, if anything go wrong with the init process, such an
init  system can  use a  console to  start an  emergency shell,  and which  is
actually the `dash` shell, a POSIX compatible `/bin/sh`.

Interacting  through  a shell  is  the  most  primitive  and powerful  way  to
administrate and  use a  system, and  in a  Linux ecosystem  is capable  to do
anything that it doesn't requires a graphical environment (an X display).

And that it is  what actually is being done by default, after  user login in a
system. For  instance a Linux system  offers a pseydoterminal that  could then
run a shell.

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
It is s system zero point anyway.

Our only requirement at this stage is a standard C library (`libc`), plus `libpam`
as  our authentication  mechanism, and  our building  tool, will  be the  make
utility, which is  also and the most primitive building  tool, since the birth
of `systemV`.

We're gonna to use some other core utilities, installed by default in all UNIX
systems, like `mkdir`, `ln`, `cp`, ... utilities.

Also the `sudo` utility (which only  required once to build its replacement) and
a `linker`.

The main development  environment here is a `Void` Linux  distribution, with GNU
libc, the  linker from GNU,  make from GNU, and  the core utilities  again the
ones that are provided by the GNU project.

Our `cc` (C  compiler) is GNU's C  Compiler Collection, and our  standard is the
last one published in 2011, known as `C11`.

But it may be build by others.

To build the zero basic system, issue:

```sh
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
pam and cretated a static library. But we'll leave this paragraph, for a reference.

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
For instance  C++,  is a superset of C, and any C++ compiler should compile C,
since any C code is valid code for C++. However C++, which at the begining was
just C with classes, during the natural evolution, it has been extended   with
more and more paradigms. So at some point has been accused that it is so good,
that a common mind can not handle it! It seems that is the only language  that
has all the qualifications to be considered as the the One True Language   and
the only one will ever need. It is like to have in your possesion a zillion of
some of the best cars in the world, but you haven't mastered a particularly one.
As a result, one with an old humble car, can go quicker and safer than you.
Perhaps, this could be avoided, by using a gradual extensibility. Perhaps. The
thing is that usually when there are so many good choises, you are just loosing
the point and your time, searching for the ultimate way to do your job. Pity.

We also believe  that the description shares the same  importance with the end
result. This  satisfy first  the inner human  curiosity, but  most importantly
allows a system to be further developed. But mainly because a human being gets
answers and has the chance to create a system based on own efforts, if there is
the desire. We want to know how to catch fishes, don't we? If not, and we like
just to be like kings, then please do not go any further, neither in this next
paragraph, nor in this life. It really doesn't makes sense such a life, as  it
can not bring happiness with no f way. Our meal that we've cooked by ourselves
and which were cultivated in our garden, it has the sweetest taste  possible.

We have also to appreciate the availability of the required tools to build the
system. These tools are free to use them (free as free beer) by anyone in this
world. But it is so, because we obey  and serve, to the one and only principle
that is:  the source code should be free (free as in freedom here).
We can be sure, that the people of this world, wouldn't be possible to produce
this tremendous in size and quality code, in that so sort time, if they had to
pay for those tools, or wouldn't be available code to study. We don't even want
to imagine this possibility! So we owe a lot, especially in the GNU project.

The open and free source model, it is for the benefit of all, and this  should
become quite crystal clear by now.

But this is also about justice. The tools should (should as an almost obligation
here) be available to the people of the world, no matter where they live and how
rich they are. Or their parents were, or even their grand/grand parents were.

It is believed that those are the ways to satisfy our uncontrollable desire for
the ultimate freedom and a pride path to our evolution, without  hiding  behind
our known by now illusions. But we are free to choose. There are ready solutions
and conveniences, special made to hold us back on our lifes, by pushing us in a
eternal painfull cycle. Somehow we should climb that hill. We are saying instead
of a torture this can became a pleasant journey. But even if its gettin painfull,
it is much better if we walk this path with others. So it isn't the open  source
model that should adjust, it is the other way around. The society and the system
should do this task, no matter how painfull that is for us. The end result  will
repay us with uncountable and precious gifts.

## Commands and Shell Interface.

So far, we've implemented an utility,  that can act as an intermediary between
the user  and the  kernel of  the Operating  System. This  implementation gets
input from  the keyboard, and then it parses and interprets this text, and  at
then makes the request for this kernel service. This kind of utility is called
a `shell`, and it is the traditional UNIX system powerfull interface, and which served the
people wi

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
about  the number  of  bytes, which  we  return  them to  our  callee.

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
man page for musca, which was an excellent fullscreen window manager for X, but
the name is reffered to a constellation. A really lovely name, if you are going
to ask my opinion.

In any  case the most important to keep here, is that in our system, we want to
unify usage and developing, as it is the reality.

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

Quite convenient mechanisms.

And some fragile too.
We've also introduced pipelines and also commands that use them. Its not easy to
make them work reliabe, as there many side affects:
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
And what it determinates that the output of printf will be the input of `Dir.make`.
We assume it is not an argument. What to do in a case  of an error and we are  in
the middle of the pipe, or our second command needs some info from the user (like
a password). In the latter case, what it determinates that the argunments are for
the `sudo` utility and not for the command itself?

As a remark, a pipeline it look like a `function composition`.

### The argument mechanism.
Here  we've used  two  arguments  known  in  everyone  that  used  those  tools.
The `--verbose` argument  says  to print  the result  to  `standard output`. The.
`--parents` argument says  to make any intermediate directory  Because they are.
standardized, we can also use their short version, which is, `-v` for `--verbose`.
and `-p` for `--parents`.

But though we learned to appreciate the power of the command line, somehow  it
looks, that soon or later, you will reach in a point, that will be not possible
to memorize, all these tools, and their arguments.

Fortunatelly modern shells offer all, a mechanism that is called autocompletion,
which is usually triggered by pressing Tab. Our shell doesn't really support this
great flexibility though for now.

Using an  autocompletion mechanism, simplifies a  lot of things and  mutates a
crucial and valid  objection for our system. Since we want to create for  every
function a matching frontend, so quite verbose, our `$PATH` will be filled rather
quickly by our functions, so the lookup will be probably slow.

This is the reason, why the a command starts with a `namespace` prefix and that
is why they begin with a Capital.

The other advantage is for the developers of this system. Since the name of the
command indicate an inter connection with a library, it is obvious where to look
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

Without joking,  yes it is a  number that the  kernel give to a  process, when
this process requests access to a file name.  This number for us, it is just a
esoteric kernel table, that in the case of a filename, it simply refers to the
underlying inode.

Couldn't have been  done simpler for us  mere mortals, as with  this number in
our hands, we now can access and manipulate the data.

Note that,  what is almost  unbelievable, is  that how ridiculously  simple to
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
because is  an intuitive,  this is  very popular  and already  established, as
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
There are could be practical in cases, but not economical.

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
  # shared targets
  make shared && make e-shared && make i-shared && make v-shared

  # static targets
  make static && make e-static && make i-static && make v-static
```
The development environment is gcc, but it should be compiled with  clang and
tinycc compilers, without a warning. In case of a warning it is considered an
error. Note that the interpreter, it crashes when it is compiled with tinycc.

## ZERO_POINT_ZERO_STATE
So far, we've implemented:
  - a rather stable ala [vim](http://www.vim.org) [editor](data/docs/editor.md)
  - a rather stable [terminal multiplexer](https://en.wikipedia.org/wiki/Terminal_multiplexer) [window manager](data/docs/v.md)
  - a very early draft (at the prototyping level) [shell](https://en.wikipedia.org/wiki/Shell_(computing)) [utility](data/docs/zs.md)
  - an early draft of a sudo like utility (audit review required)
  - a really really tiny [interpreter](data/docs/i.md)
  - a couple of commands that initialize the libraries (for development reasons)

## ZERO_POINT_ZERO_CONCLUSION
  ... todo

## ZERO_POINT_DEVELOPMENT
  ... todo

## License:
I  understand the UNLICENSE license  kind of thought. We  do not need laws and
licenses to make the right thing.

But I respect a lot the GNU philosophy and I feel that we owe a lot to the GNU
project. We  can be sure, that  we wouldn't be here  if it wasn't the  GPL and
GNU.

That is the reason for the GPL2. However, our actual law should obey in common
sense and an inner will, to do what we have to do by our own, and not  because
we have to do, because they told us to do. Our law should be streaming  by the
gained consience. Hopefully one day.

## Acknowledgements and References:

* [Mir: A jit compiler library.](https://github.com/vnmakarov/mir)
* [Tinycc: A tiny C compiler.](https://repo.or.cz/tinycc)
* [Tinyscript: A very simple scripting language.](https://github.com/totalspectrum/tinyscript)
* [Zig: A generar purpose programming language.](https://ziglang.org/)
* [Dictu: A simple programming language.](https://github.com/dictu-lang/Dictu)
* [VoidLinux: A Linux distribution without systemd.](https://voidlinux.org/)
* [Tagha: A small process virtual machine.](https://github.com/assyrianic/Tagha)
* [Crafting Interpreters.](https://github.com/munificent/craftinginterpreters)
* [Build Your Own Lisp.](https://github.com/orangeduck/BuildYourOwnLisp)
* [A Programming Language in Javascript.](http://lisperator.net/pltut/)
* [Linux From Scratch.](https://linuxfromscratch.org)
