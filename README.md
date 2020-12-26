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
that is assuming that has the the required tools to be a functional system.  
  
Another crusial task is to offer ways to the user of a system, to use the underlying  
machine's hardware, by binding device pointers in a special directory at the root  
mount point (/dev). The root mountpoint is denoted by a slash ("/"), and everything  
is related to this scratch system point.  
  
Another common task, is to start some given services explicitly set from the user,  
like to mount other filesystems in some given addresses of the root mountpoint.  
  
This procedure is called Init Process, and traditionally in a systemV, this is done  
through a very simple mechanism. Such a procedure it looks in a directory for files,  
which are usually shell scripts and loads them by usually using the default shell in  
UNIX, which simply is called "sh", and by default found as /bin/sh/.  
This location is standardized by the POSIX standard, and this shell is refered as  
POSIX sh.  
  
The sequence of executing those scripts is dictated by the first two bytes in the  
filenames, which should be digits, so the lower one in this list, is the one that take  
precedence. Naturally a script that has a 99 prefix, as the last one in the chain, it  
has the ability to overlap any previous ones.  
This is also satisfying the UNIX philosophy, where the user of a system should have  
the final control of a system, even if this is wrong.  
  
This mechanism, besides the flexibility, satisfy also user expectations, because the  
SystemV init system has been established through those years. It is modeled by again  
following the UNIX philosophy, which is modularization and simplicity.  
So a user that knows the mechanism, can modify a system to meet the specific needs,  
in an expected way and without the fear to bring down a monolithic system, like say  
"systemd", which some modern Linux distributions (at the 2020 era) favor instead of  
a traditional init, and which violates the above standards. There is nothing wrong  
with the underlying idea, to be a "userspace kernel", but it is the implementation  
that makes it inadequated to a UNIX like system, which is based on modularization  
and expectations. We also want to be when we grown up a "User Space Kernel".  
  
Also usually distributions are using this init process, to log the booting process  
plus messages from the kernel, in a way that will be usefull for debugging.  
  
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
authentication mechanism.  
  
Our building tool at this stage, will be the make utility, which is also and the most  
primitive building tool, since the birth of systemV.  
  
We're gonna to use some other core utilities, installed by default in all UNIX systems,  
like mkdir, ln, cp, ... utilities.  
  
Also the sudo utility (only required to build its replacement) and a linker.  
  
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
libc (though these days are at least a couple of years away). At some point we can try  
to build one standard C library in the same location with the rest of this system, so  
we can use this system as an Operating System (OS). In any case rolling our own libraries  
offers flexibility, and chances for optimizations.  
  
Note that the libraries, are packed as structures and exposed in an Object Oriented  
style for various reasons. Those Types, will follow us in every stage from now on,  
and we can refer them as the z standard c library. We'll introduce others in the way.  
  
This command, it will also built a quite minimal shell, which we'll refer to it as ["zs"](data/docs/zs.md),  
and the "zsu" utility which is the one that will allow us to execute ANY command with  
superuser rights. Note that the latter is installed as setuid root, that is owned by  
root, and so capable to destroy the system; note that this utility is at very early  
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
  
To be continued ...  
  
... though of course, we'll try to enhance this basic system with time.
  
License: I understand the UNLICENSE license kind of thought.  
We do not need laws and licenses to make the right thing.  

But I respect a lot the GNU philosophy and i feel that we owe a lot to  
the GNU project. We can be sure, that we wouldn't be here if it wasn't the GPL and GNU.    
That is the reason for the GPL2. However, and i do not know if it is possible  
i would like to release it also with the UNLICENSE or to the Public Domain.  
Or if it is allowed to even more liberal licenses. But i do not know and i can not  
understand these laws, so i plead guilty beforehand. However my actual law is called  
conscience.  
