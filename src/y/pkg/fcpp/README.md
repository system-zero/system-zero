```C
/*
 * Frexx CPP (C Preprocessor)
 *
 * Copyright (c) by Daniel Stenberg 1993 - 2011
 *
 * This is a C preprocessor. It is a project based on public domain code, then
 * forked by Daniel in 1993 and future work has been done under a BSD license.
 *
 * The C preprocessor is now (mostly?) ANSI C compliant, and some tweaks have
 * been applied to also make it fairly usable to process other data files, such
 * as HTML for simple web sites.
 *
 * WWW:   http://daniel.haxx.se/projects/fcpp/
 * Code:  https://github.com/bagder/fcpp
 */

/******************************************************************************
 *                               FREXXWARE
 * ----------------------------------------------------------------------------
 *
 * Project: Frexx C Preprocessor
 * $Source: /home/user/start/cpp/RCS/usecpp.c,v $
 * $Revision: 1.6 $
 * $Date: 1994/06/02 09:11:01 $
 * $Author: start $
 * $State: Exp $
 * $Locker: start $
 *
 * ----------------------------------------------------------------------------
 * $Log: usecpp.c,v $
 * Revision 1.6  1994/06/02  09:11:01  start
 * Added the '-n' option!
 *
 * Revision 1.5  1994/06/02  08:51:49  start
 * Added three more command line parameters
 * Made -h invokes exit nice
 *
 * Revision 1.4  1994/01/24  09:37:17  start
 * Major difference.
 *
 * Revision 1.3  1993/12/06  13:51:20  start
 * A lot of new stuff (too much to mention)
 *
 * Revision 1.2  1993/11/11  07:16:39  start
 * New stuff
 *
 * Revision 1.1  1993/11/03  09:13:08  start
 * Initial revision
 *
 *
 *****************************************************************************/

 /* ag: 27 February 2022

  - merge the code as a single compilation unit

  - expose the functionality as a shared or static library

  - remove other platforms support other than UNIX like

  - silence various compiler warnings, produced when we enable DEBUG C flags

  * This is for now for internal usage and mainly for testing the generated
  * libc. We could use cpp but this is another C code, almost 30 years old,
  * that still compiles and works perfectly. This is an amazing and precious
  * C property, in my humble opinion, as I could not think of another language,
  * that such old code, could still be compiled or interpreted by the modern
  * compilers or interpreters.
  */

```
Build:

Using the provided Makefile:

```sh
  make shared && make main-shared
  make static && make main-static
```

Using our Make utility:
```sh
  Make shared -v fcpp
  Make static -v fcpp

# or both at once
  Make all -v fcpp
```
