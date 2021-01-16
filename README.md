[zero point draft]  
## Description

This is to describe and implement a functional computer system, with  
as few resources is possible, and provide our own solution for as many  
specific computer tasks is possible.  
  
Since this is a quite huge task, we'll have to extend our system in a gradual way,  
so we have to divide it into stages. At the end of any stage, the end result should  
be in a usable state.  
  
The first stage is of course to be able to get control after kernel initialization,  
so at some point we'll have to be ready to be pid 1. Usually in the Unix land, this  
is a program that is called init, and it is the first process that is spawned by the  
kernel by default. Any process is inherited by this pid 1 (Process Id).  
  
Normally this is a program, that makes the necessary steps to brings up the computer  
in a usable state, and also to halt the system.  
  
The first task usually is to mount the required filesystems, like the root filesystem,  
that is assuming that has the required tools to be a functional system.  
  
Another crusial task is to offer ways to the user of a system, to use the underlying  
machine's hardware, by binding device pointers in a special directory at the root  
mount point (/dev). The root mountpoint is denoted by a slash ("/"), and everything  
is related to this scratch system point.  
  
Another common task, is to start some given services explicitly set from the user,  
like to mount other filesystems in some given addresses of the root mountpoint.  
  
This procedure is called Init Process, and traditionally in a systemV, this is done  
through a very simple mechanism. Such a procedure it looks in a directory for files,  
which are usually shell scripts and loads them by usually using the default shell in  
UNIX, which simply is called "sh", and by default found as /bin/sh.  
This location is standardized by the POSIX standard, and this shell is refered as  
POSIX sh.  
  
The sequence of executing those scripts is dictated by the first two bytes in the  
filenames, which should be digits, so the lower one in this list, is the one that take  
precedence. Naturally a script that has a 99 prefix, as the last one in the chain, it  
has the ability to overlap any previous ones.  
This is also satisfying the UNIX philosophy, where the users of a system should have  
the final control of a system, even if they are wrong.  
  
This mechanism, besides the flexibility, satisfy also user expectations, because the  
SystemV init system has been established through those years. It is modeled by again  
following the UNIX philosophy, which is modularization and simplicity.  
So a user that knows the mechanism, can modify a system to meet the specific needs,  
in an expected way and without the fear to bring down a monolithic system, like say  
"systemd", which some modern Linux distributions (at the 2020 era) favor instead of  
a traditional init, and which violates the above standards. There is nothing wrong  
with the underlying idea, to be a "userspace kernel", but it is the implementation  
that makes it inadequated to a UNIX like system, which is based on modularization  
and expectations.  
We also want to be, and when we grown up, a "User Space Kernel".  
  
Also usually distributions are using this init process, to log the booting process  
and the messages from the kernel, in a way that will be usefull for debugging.  
  
At least in Linux systems, if anything go wrong with the init process, such an init  
system can use a console to start an emergency shell, and which is actually the dash  
shell, a POSIX compatible /bin/sh.  
  
Interacting through a shell is the most primitive and powerfull way to administrate  
and use a system, and in a Linux ecosystem is capable to do anything that it doesn't  
requires a graphical environment (an X display).  
  
And that it is what actually is being done by default, after user login in a system.  
For instance a Linux system offers a pseydoterminal that could then run a shell.  
  
And that is what we have to implement at our first stage. We also want a utility that  
will offer a way to execute commands with superuser rights, so we could administrate  
the system. Building those two tools, will allow us to slowly disassociate from our  
host.  
  
## Implementation.  
  
This is going to be in C, as the nature of the task, it requires direct access to the  
underlying machine, and this is the main property of C.  
  
Another C property, is that the language do not do anything by its own, so every little  
thing, like memory managment, should be done by the programer.  
So at the same time C obeys trully the spirit and basic intention of this project, as  
we have to implement ourselves at least the basic functionality.  
It is s system zero point anyway.  
  
Our only requirenment at this stage is a standard C library (libc), plus libpam as our  
authentication mechanism, and our building tool, will be the make utility, which  
is also and the most primitive building tool, since the birth of systemV.  
  
We're gonna to use some other core utilities, installed by default in all UNIX systems,  
like mkdir, ln, cp, ... utilities.  
  
Also the sudo utility (which only required once to build its replacement) and a  
linker.  
  
The main development environment here is a Void Linux distribution, with GNU libc,  
the linker from GNU, make from GNU, and the core utilities again the ones that are  
provided by the GNU project.  
  
Our cc (C compiler) is GNU's C Compiler Collection, and our standard is the last one  
published in 2011, known as C11.  
  
But it may be build by others.  
  
To build the zero basic system, issue:  

```sh
  make zero-shared
```
  
This will built first some libraries, such as string type functions that deal with C   
strings. Why? Because it is our desire that some day, we can be independent by even a  
libc (though these days are at least a couple of years away). 
At some point we can try to build one standard C library in the same location with  
the rest of this system, so we can use this system as an Operating System (OS).  
In any case rolling our own libraries offers flexibility, and chances for optimizations.  
  
Note that the libraries, are packed as structures and exposed in an Object Oriented  
style for various reasons. Those Types, will follow us in every stage from now on,  
and we can refer them as the z standard c library. We'll introduce others in the way.  
  
This command, it will also built a quite minimal shell, which we'll refer to it as ["zs"](data/docs/zs.md),  
and the "zsu" utility which is the one that will allow us to execute ANY command with  
superuser rights. Note that the latter is installed as setuid root, that is owned by  
root, and so capable to destroy the system, and that this utility is at very early  
development. In any case, though we really really want to be correct and provide an  
accurate implementation, we care most about to describe the procedure and provide a  
solution to explain some why's. We prefer something to present and demonstrate, than  
nothing at all. And as always, development happens through time and patches.  
  
But our main intention is to describe and provide the shorted possible solution to a  
need. And at this stage we care only about the basic functionality. But as it has been  
said already a shell is capable to do almost everything, this really depends of the  
availability of a ecosystem. We'll try to build some common tools in time.  
  
It should be noted, that we have linked against shared libraries, so this system can be  
safely used only in that same machine; they will probably work the same, on a same  
system with the same system libraries in a same architecture.  
  
We also use -Wl,-rpath=/path/to/libraries_directory. With this way we  
do not need to invoke "sudo ldconfig -v" and adding that path to the  
linker search path, as the -rpath oprion says to the linker to include  
this path to the executable. So for now we'll use this option, but probably  
we'll use other method in later states.  
  
But those target, they can be built also statically, so they might be used in another machine with  
different libraries versions. But they must be at the same architecture.  
  
To build the static versions, issue:  

```sh
  make zero-static
```

Note that the zsu utility, can not be built in my system, which lacks a static installed  
libpam. Plus the compilation gives me some warnings such:  
zsu.c:190: warning: Using 'initgroups' in statically linked applications requires at  
runtime the shared libraries from the glibc version used for linking  
  
That probably means, that if we want to use that static tool in another system, the  
linked libraries should match. There are reasons for this probably, but that means that  
there is really no benefit for linking statically then.  
  
The installed system hierarchy is sys/$(uname -m)/{lib,bin,include}. So we might pack  
this repository and carry it in different systems, even if a 32bit architecture is thing  
of the past, though those words are written in such machine.  
  
This is our first state, and it is the one that we care most, regarding  
our focus. This satisfy our main fundamental principle, that believes that  
every tool, should offer first the freedom to access it in a basic primitive  
level, and apply this principle rigorously when they are being extended.  
  
We also believe that the description shares the same importance with the end result.  
This satisfy first the inner human curiosity, but most importantly allows a system to  
be further developed, but mainly because a human being gets answers and has the chance  
to create a system based on own efforts.  
  
We have also to appreciate the availability of the required tools to build the system.  
These tools are free to use them (free as free beer) by anyone in this world. 
But it is so, because we obey and serve, to the one and only principle that is:  
the source code is free (free as in freedom). Simply, we won't be here if it wasn't for  
this, as it had been proved, by this tremendous code evolution in such sort time.  
So actually at the end this is for the benefit of all. Let the open source programming   
model to be our guide and the bright paradigm in our life.  
  
It is the only way to satisfy our uncontrolable desire for the ultimate freedom  
without illusions.  
  
## Commands and Shell Interface.
  
So far, we've implemented an utility, that can act as an intermediary between the user  
and the kernel of the Operating System. This implementation gets input from the   
keyboard, interprets the text and then makes the request for the kernel service.    
This kind of utility is called a shell, and it is the traditional UNIX system interface.  
  
For instance in UNIX, if we wish to get a list of the contents of a directory, we 
use the ls utility. If we use it without an argument, then it prints in the screen,  
the contencts of the current directory. A directory is a file that holds the names   
of files which are references to a data structure that is called inode, that holds  
the file's data and metadata. Those inodes are part of the underlying filesystem, a 
tree directory structure, where the highest level in UNIX is denoted by the slash (/).  
  
The first such kind of utility that we've implemented is called File.size.  
This gets this information, for the specified filename and then prints the  
number of bytes (the data) that are associated with this filename.  
We got this information by calling a function from our standard C library, which is
responsible to communicate with the kernel. Here is the C code:
  
```C
static size_t file_size (const char *fname) {
  struct stat st;
  if (NOTOK is stat (fname, &st)) return 0;
  return st.st_size;
}
```
The st structure contains all the information about the inode that is associated  
with the requested filename. In this function we're interesting about the number of  
bytes, which we return them to our callee. So the programmer what it has to do only, is  
to parse the input from the user and then simply call this function, here it is:
  
```C 
   size_t size = File.size (filename);
   fprintf (stdout, "%zd\n", size);
```
  
The fprintf() function, is asking from the kernel, to print the output, the given text
onto the screen.

The output of a command can also be redirected into a filename, like:

```sh
  File.size Makefile >size_of_Makefile
```
The output of this command was written into the "size_of_Makefile" file.  
  
But also, the output of a command can be the input of another command, like:
```sh
  ls Makefile | File.size
```
Assuming that the programmer of the File.size command, wrote code that will handle  
the case when the input is not comming from the keyboard, but from the standard  
input stream. In our case this is handled, so the effect it is the same.  
  
This command chain is called pipeline, as the output of a command is the input of  
the next command in the chain.  
  
This concept is supported in our shell. In fact, it is going to be our direction.  
  
You maybe notice the weird command with a dot in between. This is totally unusual,  
in fact nobody else has done something similar before, so we are going to deviate  
here.  
  
Our libraries (functions) are not exposed with the traditional C way.  
We expose structures, which they have fields that are pointers to functions. Here  
is the signature of this specific one:
  
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
      .size = file_size
      ...
    }
  };

/* the following is a macro (a bit of syntactic sugar), that helps the
 * programmer mind to focus on the implementation */

#define File File_Type.self
```
  
Such a style is called usually object oriented style, though in this specific usage,  
there is no object to operate, since there are no properties, though they can be  
added one day. It is being used here for code organization, and because it offers  
flexibility, as the user of this structure, it can overide this method with another  
function implementation, tailored in the needs.  
  
In any case the most important here, is that the command File.size and the underlying  
library function matches. And in this case they also match the arguments.  
However, we can extend the command to take another argument from the command line  
and do something different, like to print the bytes also in MegaBytes.  
And many tools usually work that way.  
  
In our system however we will not. We will prefer to implement another command for  
this, that we'll call it Bytes.to_mbytes or something similar.  
That way we gain some bits of conveniences.  
  
Let's explain it. In Unix land there are two main tools to download data from the net.  
That is wget and curl. First the names are unusual and don't really have a relation  
with their job. Most utilities fell into this category. Secondly, the command line  
interface usually are different, though they are doing same things.  So there isn't  
a unification.  
  
But with the schema above we can do better. How? Let's say for simplification, that  
we wanted to wrap these tools, we could provide a Net command with submethods.  
The implementation it would be as easy as providing symbolic links to the same unit,  
and the programmer could lookup to the argv[0] (which is always the program name),
to develop logic.  
   
The other thing would be to unify their arguments, so the user finally has to  
learn one thing.  
  
Utilities such bysybox are working like this. The GNU project also is trying to do  
something similar. This is called the principle of less surprise and it works wonders.  
  
We'll focus to make our commands to read from standard input. In fact at the time  
of writing, we've implemented Dir.make and Dir.rm. Both are reading from stdin, like:
  
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
  
Here we used two arguments known in everyone that used those tools.  
The --verbose argument says to print the result to standard output.  
The --parents argument says to make any intermediate directory.  
Because they are standardized, we can also use their short version, which is,
-v for --verbose and -p for --parents.  
  
So we satisfy both camps; short argument lovers and the GNU style with two dashes  
and descriptive names. I'm in the second camp, though in my youth, i was an expert  
and i knew, and i still know many (though the memory fades a bit), both variants.  
The obvious advantage is for the novices. In any case modern shells offer all, a  
mechanism that is called autocompletion, which is usually triggered by pressing Tab.  
Out shell doesn't really support this great flexibility though for now.  
  
Using an autocompletion mechanism, simplifies a lot of things and mutates a crucial  
and valid objection. Our way is quite verbose, as our $PATH will be filled with  
myriad of commands and so the lookup maybe slow. Our counterargument is quite simple.  
  
Our commands begin with a Capital.

## Semantics.

Slowly we'll have to set some semantics for our system.  
  
We've already implemented a "make directory" command and in a way, that can receive  
the argument by reading the standard input. Here is the relevant code:  
  
```C

  ifnot (FdReferToATerminal (STDIN_FILENO)) {
    char dname[MAXLEN_PATH];
    if (NOTOK is Io.fd.read (STDIN_FILENO, directory_name, MAXLEN_PATH))
      ... logic to handle the error, as the function call was unsuccesfull
    else
      ... continue logic below, as our request completed with success, and now we  
      have our input (like if the user has typed something in the keyboard), in the dname
      variable, by the read() method, of the File Descriptor Type/Class, which is  
      included in the Input Output class. In this case the File Descriptor class  
      is called subclass in programming consepts.
```

What is a file descriptor? Easy. It is a number!!!  
  
Without joking, yes it is a number that the kernel give to a process, when this  
process requests access to a file name. This number for us, it is just a esoteric  
kernel table, that in the case of a filename, it simply refers to the underlying  
inode.  
  
Couldn't have been done simplier for us mere mortals, as with this number in our   
hands, we now can access and manipulate the data.  
  
Note that, what is almost unbelievable, is that how ridiculously simple to read and write back.  
The document about [Pointers](data/docs/pointers.md) is trying to explain some bits,  
by using the above example.  
  
And one last thing before we proceed. When a process is a background process or if  
it is connected to a pipeline, like in the above exampe, it is being said, that it  
has no controlling terminal; means that we can not input nothing to that process  
through (lets say) the keyboard, as it isn't associated with a terminal.  
In such processes there isn't a way to interact directly with it, but only through  
other mechanisms like through a socket.   

In our code above, STDIN_FILENO is a file descriptor that refers to a kernel table.  
By default this is 0, while STDOUT_FILENO binds to 1 and STDERR_FILENO to 2.  
  
Now consider this shell session, which is using standard coreutils tools:  
```sh
[aga][485](/tmp/a)mkdir -v ../a/-n
mkdir: created directory '../a/-n
[aga][486](/tmp/a)ls
-n/
[aga][487](/tmp/a)echo something > file
[aga][488](/tmp/a)cat *
     1	something
```
Wrong and unexpected result! It isn't a bug. POSIX allows a leading dash in pathnames,  
so mkdir is conforming. The cat utility reads all the filenames and outputs their  
contents, but in this case what it happens, is that the cat utility thinks that "-n"  
is an option to itself, and so it includes in its output, also a number.  
  
You can read a bit more in this [article](https://dwheeler.com/essays/fixing-unix-linux-filenames.html).  
  
So we have to make a decision. Should we allow a leading dash, or a new line or a  
tab or control characters, when we are creating a directory?  
  
Our decision will set the semantics of our system.  
  
Since in our mind this doesn't make sense, or at the very least complicates the code,
we won't. In the followin code, we don't even allow any character outside of the  
ASCII range:  
  
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
directory, we should call Dir.make (), otherwise we might violate the above semantics.  
  
In this case Dir.make() is our interface, as it abstracts all the details for us,  
and allows consistency, which is quite important to a system.  
  
To be continued ...  
  
License: I understand the UNLICENSE license kind of thought.  
We do not need laws and licenses to make the right thing.  

But I respect a lot the GNU philosophy and i feel that we owe a lot to the GNU project.  
We can be sure, that we wouldn't be here if it wasn't the GPL and GNU.  
That is the reason for the GPL2. However, and i do not know if it is possible i would  
like to release it and with the UNLICENSE or to the Public Domain, or to even more  
liberal licenses. But i do not know them nor i can understand these laws, so i plead  
guilty beforehand. In any case my actual law obeys in common sense and an inner will to  
do what i have to do by my own, and not because i have to do because they told me to  
do. Our law is and it should be streaming from the gained conscience.  
