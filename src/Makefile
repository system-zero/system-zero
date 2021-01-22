# This Makefile is to help us to initialize a minimun functional environment.
# It also serves as an introductory document, as it declares intentions and describe
# the implementation through some loud thoughts in comments, so it is more verbose
# than in usual Makefiles.  This is to be initialized also in the developed type of
# thinking. So though there are might be some pointers, for those that are not
# familiar with programming consepts, it was really written for developers, but it
# might be used by others too.

# This is our actual first file, and it is the one that will assist us to build the
# environment. It also serves as a vehicle to understand the needs, establish the
# hierarhy and finally to end up on things. So at the same time there are development
# type of comments. 

# We use a Make type of building system, which is and the most primitive in Unix land.
# Particularly the GNU make implementation, though we want to be POSIX compatible, so
# this Makefile code can be end up to be portable. We'll do this with time and iterations

#-----------------------------------------------#
# Application Layer.
API     := 0
REV     := 0
VERSION  = $(API).$(REV)
#-----------------------------------------------#
#           HIERARCHY

# we want this variable to be treated as immutable but we can't, as this it cannot
# be done actually, as the make utility allow variables to be overriden by anyone
# that call us through the command line of from an external Makefile.
# The last thing we can do is to use the plain assignment symbol '=' instead of using
# ':=' , more like a convention to declare an intention, rather it makes a difference.
THISDIR        = $(shell (cd . && pwd))
# actually we implement this variable this way, because we know that the following code:
# THISDIR := $(shell realpath .)
# is not quite portable in macOS, because this Operating System lacks a realpath
# implementation by default (it is available however through GNU coreutils macOS
# package made for this OS. On a later development phase, we may provide an own
# implementation or realpath, as it is quite useful to sanitizes paths).

# our sources namespace. We want this also in a constant state (again this is not possible)
SRCDIR         = $(THISDIR)

# machine architecture
SYSARCH       := $(shell uname -m)

# our target directory.
# If it is not already defined in the command line as:
#   make SYSDIR=/some/dir [target]
# or from another Makefile, then by default is this_directory/sys/$(uname -m)
SYSDIR        := $(THISDIR)/../sys/$(SYSARCH)

SYSLIBDIR      = $(SYSDIR)/lib
SYSBINDIR      = $(SYSDIR)/bin
SYSINCDIR      = $(SYSDIR)/include

# (not sure about them too yet: the question is: should be one or two? 
#  perhaps a USER_ prefix is justified, though this can be abstracted
#  at runtime by our identity)
SYSDATADIR     = $(SYSDIR)/../data
SYSTMPDIR      = $(SYSDIR)/../tmp

#-----------------------------------------------#
#             zero namespace

ZNAME       = z
Z_SRCDIR    = $(SRCDIR)/$(ZNAME)

  # z library namespace
  Z_SRCLIBDIR = $(Z_SRCDIR)/lib

    # our C environment
    ZCENV                = cenv
    Z_CENV_SRCDIR        := $(Z_SRCLIBDIR)/$(ZCENV)/src

    # C structures (types) 

    # a string type (string_t) with:
    # char *bytes; size_t num_bytes; size_t memsize;
    String_Type          = String
    Z_STRING_SRCDIR     := $(Z_SRCLIBDIR)/$(String_Type)/src

    # a container of vstring_t (string_t plus a next/prev ptrs) 
    Vstring_Type         = Vstring
    Z_VSTRING_SRCDIR    := $(Z_SRCLIBDIR)/$(Vstring_Type)/src

    # our own implementation of string functions with byte semantics
    Cstring_Type         = Cstring
    Z_CSTRING_SRCDIR    := $(Z_SRCLIBDIR)/$(Cstring_Type)/src

    # likewise a (UTF8) library to handle strings with character semantics
    Ustring_Type         = Ustring
    Z_USTRING_SRCDIR    := $(Z_SRCLIBDIR)/$(Ustring_Type)/src

    # a Directory Type
    Dir_Type             = Dir
    Z_DIR_SRCDIR        := $(Z_SRCLIBDIR)/$(Dir_Type)/src

    # Authentication Type
    Auth_Type            = Auth
    Z_AUTH_SRCDIR       := $(Z_SRCLIBDIR)/$(Auth_Type)/src

    # a Proc Type
    Proc_Type            = Proc
    Z_PROC_SRCDIR       := $(Z_SRCLIBDIR)/$(Proc_Type)/src

    # Readline Type
    Rline_Type           = Rline
    Z_RLINE_SRCDIR       = $(Z_SRCLIBDIR)/$(Rline_Type)/src

    # Shell Type
    Sh_Type              = Sh
    Z_SH_SRCDIR          = $(Z_SRCLIBDIR)/$(Sh_Type)/src

    # File Type
    File_Type            = File
    Z_FILE_SRCDIR        = $(Z_SRCLIBDIR)/$(File_Type)/src

    # Argparse Type
    Argparse_Type        = Argparse
    Z_ARGPARSE_SRCDIR    = $(Z_SRCLIBDIR)/$(Argparse_Type)/src

    # IO Type
    IO_Type              = IO
    Z_IO_SRCDIR          = $(Z_SRCLIBDIR)/$(IO_Type)/src

    # Path Type
    Path_Type            = Path
    Z_PATH_SRCDIR        = $(Z_SRCLIBDIR)/$(Path_Type)/src

    # Term Type
    Term_Type            = Term
    Z_TERM_SRCDIR        = $(Z_SRCLIBDIR)/$(Term_Type)/src

    # Imap Type
    Imap_Type            = Imap
    Z_IMAP_SRCDIR        = $(Z_SRCLIBDIR)/$(Imap_Type)/src

    # Imap Type
    Smap_Type            = Smap
    Z_SMAP_SRCDIR        = $(Z_SRCLIBDIR)/$(Smap_Type)/src

    # Re Type
    Re_Type              = Re
    Z_RE_SRCDIR          = $(Z_SRCLIBDIR)/$(Re_Type)/src

    # Video Type
    Video_Type           = Video
    Z_VIDEO_SRCDIR       = $(Z_SRCLIBDIR)/$(Video_Type)/src

    # E Type
    E_Type               = E
    Z_E_SRCDIR           = $(Z_SRCLIBDIR)/$(E_Type)/src

    # I Type
    I_Type               = I
    Z_I_SRCDIR           = $(Z_SRCLIBDIR)/$(I_Type)/src

    # Readline Type
    Readline_Type        = Readline
    Z_READLINE_SRCDIR    = $(Z_SRCLIBDIR)/$(Readline_Type)/src

    # though not a type, this is a rich double linked list
    # which is implemented with macros, so to gain abstraction.
    # a compatible container should include:
    #   head/current/tail pointers
    #   int cur_idx; int num_items; 
    # for any item in the list, it is assumed prev/next pointers
    Z_DLIST_SRCDIR      := $(Z_SRCLIBDIR)/dlist/src

  # C applicarion namespace
  Z_SRCAPPDIR  = $(Z_SRCDIR)/app

    # shell
    Z_SHELL             = zs
    Z_SHELL_SRCDIR     := $(Z_SRCAPPDIR)/$(Z_SHELL)/src

    # utility to execute commands with superuser rights
    # note that is installed as setuid root, with root/$(GROUP) permission
    Z_SU                = zsu
    Z_SU_SRCDIR        := $(Z_SRCAPPDIR)/$(Z_SU)/src

  # Notes about the mechanism intentions. This is to allow later
  # implementations to use an extended type. This is freely allowed
  # as long as the semantics (that this root version sets) do not
  # change. These are:
  #  - the names and the declared sequence of the properties are
  #    being respected
  #  - likewise for the returned types, the number and the type of
  #    arguments of the functions/methods
  #  - the new directory that overides one or all of the above,
  #    should respect this hierarchy, that means:
  #      - the name of the compilation units should match
  #      - provide a Makefile that could handle the compilation
#-----------------------------------------------#

Z_SYSLIBDIR  = $(SYSLIBDIR)/$(ZNAME)
Z_SYSINCDIR  = $(SYSINCDIR)/$(ZNAME)

Z_SHELL     := $(ZC_SHELL)
Z_SU        := $(ZC_SU)

# In this level we want a shell and an utility that will permit
# us to execute commands with SuperUser (su for sort) rights.
# This level should offer a way to do machine administration,
# that means in a Unix machine, and if we have the required rights, 
# that the time we gain access, it is the time we own the system.

#-----------------------------------------------#
#  C Implementation

  # we follow GNU cc, but we also care a bit for clang and tcc, but not much
  CC            := gcc

  # C11 is our minimum standard
  # (but we might be compiled by olders, but its not our buisness)
  CC_STD        := -std=c11

  BASE_FLAGS     = -I$(SYSINCDIR) -L$(Z_SYSLIBDIR)
  BASE_FLAGS    += -g -O2 -march=native -fdollars-in-identifiers
  BASE_FLAGS    += -DDECLARE=1 -DDONOT_DECLARE=0
  DEBUG_FLAGS   := -Wextra -Wshadow -Wall -Wunused-result -Wunused-function -Wunused-macros -Wno-override-init

  DEBUG         := 1
  RPATH         := 1

  # we use rpath in this early stage, so we do not need to run ldconfig
  ifeq ($(RPATH), 1)
    BASE_FLAGS    += -Wl,-rpath=$(Z_SYSLIBDIR):$(SYSLIBDIR)
  endif

  ZARGS  = NS=$(ZNAME)
  MARGS :=
  MARGS  = API=$(API) REV=$(REV) VERSION=$(VERSION) DEBUG=$(DEBUG)
  MARGS += SYSDIR=$(SYSDIR) SYSDATADIR=$(SYSDATADIR) SYSTMPDIR=$(SYSTMPDIR)
  MARGS += BASE_FLAGS='$(BASE_FLAGS)'

#----------------------------------------------------------#
cenv: Env
	@cd $(Z_CENV_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

clean-cenv:
	@cd $(Z_CENV_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean
#----------------------------------------------------------#
dlist: Env
	@cd $(Z_DLIST_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

clean-dlist:
	@cd $(Z_DLIST_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean
#----------------------------------------------------------#
libstring-shared: Env cenv
	@cd $(Z_STRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libstring-static: Env cenv
	@cd $(Z_STRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libstring-shared:
	@cd $(Z_STRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libstring-static:
	@cd $(Z_STRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libustring-shared: Env cenv
	@cd $(Z_USTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libustring-static: Env cenv
	@cd $(Z_USTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libustring-shared:
	@cd $(Z_USTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libustring-static:
	@cd $(Z_USTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libcstring-shared: Env cenv
	@cd $(Z_CSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libcstring-static: Env cenv
	@cd $(Z_CSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libcstring-shared:
	@cd $(Z_CSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libcstring-static:
	@cd $(Z_CSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libvstring-shared: Env cenv libstring-shared libcstring-shared
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libvstring-static: Env cenv libstring-static libcstring-static
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libvstring-shared:
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libvstring-static:
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libdir-shared: Env cenv libstring-shared libvstring-shared libcstring-shared libpath-shared
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libdir-static: Env cenv libstring-static libvstring-static libcstring-static libpath-static
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libdir-shared:
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libdir-static:
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libauth-shared: Env cenv libstring-shared libcstring-shared libustring-shared\
                libio-shared libterm-shared
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libauth-static: Env cenv libstring-static libcstring-static libustring-static\
                libio-static libterm-static
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libauth-shared:
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libauth-static:
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libproc-shared: Env cenv dlist libcstring-shared
	@cd $(Z_PROC_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libproc-static: Env cenv dlist libcstring-static
	@cd $(Z_PROC_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libproc-shared:
	@cd $(Z_PROC_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libproc-static:
	@cd $(Z_PROC_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
librline-shared: Env cenv libstring-shared
	@cd $(Z_RLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

librline-static: Env cenv libstring-static
	@cd $(Z_RLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-librline-shared:
	@cd $(Z_RLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-librline-static:
	@cd $(Z_RLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libsh-shared: Env cenv libcstring-shared libproc-shared
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libsh-static: Env cenv libcstring-static libproc-static
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libsh-shared:
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libsh-static:
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libfile-shared: Env cenv libstring-shared libcstring-shared libvstring-shared libdir-shared libpath-shared
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libfile-static: Env cenv libstring-static libcstring-static libvstring-static libdir-static libpath-static
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libfile-shared:
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libfile-static:
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libargparse-shared: Env cenv libstring-shared libcstring-shared libvstring-shared libdir-shared
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libargparse-static: Env cenv libstring-static libcstring-static libvstring-static libdir-static
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libargparse-shared:
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libargparse-static:
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libio-shared: Env cenv
	@cd $(Z_IO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libio-static: Env cenv
	@cd $(Z_IO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libio-shared:
	@cd $(Z_IO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libio-static:
	@cd $(Z_IO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libpath-shared: Env cenv libcstring-shared
	@cd $(Z_PATH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libpath-static: Env cenv libcstring-static
	@cd $(Z_PATH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libpath-shared:
	@cd $(Z_PATH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libpath-static:
	@cd $(Z_PATH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libterm-shared: Env cenv libstring-shared libio-shared
	@cd $(Z_TERM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libterm-static: Env cenv libstring-static libio-static
	@cd $(Z_TERM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libterm-shared:
	@cd $(Z_TERM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libterm-static:
	@cd $(Z_TERM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libimap-shared: Env cenv libcstring-shared
	@cd $(Z_IMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libimap-static: Env cenv libcstring-static
	@cd $(Z_IMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libimap-shared:
	@cd $(Z_IMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libimap-static:
	@cd $(Z_IMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libsmap-shared: Env cenv libcstring-shared
	@cd $(Z_SMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libsmap-static: Env cenv libcstring-static
	@cd $(Z_SMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libsmap-shared:
	@cd $(Z_SMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libsmap-static:
	@cd $(Z_SMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libre-shared: Env cenv libcstring-shared libstring-shared libustring-shared
	@cd $(Z_RE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libre-static: Env cenv libcstring-static libstring-static libustring-static
	@cd $(Z_RE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libre-shared:
	@cd $(Z_RE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libre-static:
	@cd $(Z_RE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libvideo-shared: Env cenv libcstring-shared libstring-shared libustring-shared libio-shared
	@cd $(Z_VIDEO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libvideo-static: Env cenv libcstring-static libstring-static libustring-static libio-static
	@cd $(Z_VIDEO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libvideo-shared:
	@cd $(Z_VIDEO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libvideo-static:
	@cd $(Z_VIDEO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libe-shared: Env cenv dlist libstring-shared libcstring-shared libvstring-shared libustring-shared\
             libpath-shared libfile-shared libdir-shared libterm-shared libio-shared\
             libre-shared libimap-shared libsmap-shared libi-shared
	@cd $(Z_E_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libe-static: Env cenv dlist libstring-static libcstring-static libvstring-static libustring-static\
             libpath-static libfile-static libdir-static libterm-static libio-static\
             libre-static libimap-static libsmap-static libi-static
	@cd $(Z_E_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libe-shared:
	@cd $(Z_E_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libe-static:
	@cd $(Z_E_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libi-shared: Env cenv libstring-shared libcstring-shared \
             libvstring-shared libustring-shared libpath-shared libfile-shared
	@cd $(Z_I_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libi-static: Env cenv libstring-static libcstring-static\
             libvstring-static libustring-static libpath-static libfile-static
	@cd $(Z_I_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libi-shared:
	@cd $(Z_I_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libi-static:
	@cd $(Z_I_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libreadline-shared: Env cenv libstring-shared libcstring-shared \
             libvstring-shared libustring-shared libpath-shared libfile-shared
	@cd $(Z_READLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libreadline-static: Env cenv libstring-static libcstring-static\
             libvstring-static libustring-static libpath-static libfile-static
	@cd $(Z_READLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libreadline-shared:
	@cd $(Z_READLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libreadline-static:
	@cd $(Z_READLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
zs-shared: Env cenv libcstring-shared libsh-shared librline-shared
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

zs-static: Env cenv libcstring-static libsh-static librline-static
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

clean-zs-shared:
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-zs-static:
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
zsu-shared: Env cenv libcstring-shared libstring-shared libproc-shared libauth-shared
	@cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) zsu-shared
	@cd $(Z_SU_SRCDIR) && sudo $(MAKE) $(ZARGS) $(MARGS) install-shared
	@cd $(Z_SU_SRCDIR) && $(RM) zsu-shared

# can not confirm (a libpam statically linked is required)
# other warnings might make this target unsuitable, e.g.,
#   warning: Using 'initgroups' in statically linked applications
#   requires at runtime the shared libraries from the glibc version
#   used for linking
zsu-static: Env cenv libcstring-static libstring-static libproc-static libauth-static
	@cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) zsu-static
	@cd $(Z_SU_SRCDIR) && sudo $(MAKE) $(ZARGS) $(MARGS) install-static
	@cd $(Z_SU_SRCDIR) && $(RM) zsu-static

clean-zsu-shared:
	@cd $(Z_SU_SRCDIR) && sudo $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-zsu-static:
	@cd $(Z_SU_SRCDIR) && sudo $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
# App+

APP_FLAGS  :=
APP_PREFIX := __
APP_FLAGS  += APP_PREFIX=$(APP_PREFIX)
APP_FLAGS  += $(ZARGS) $(MARGS)

  # file
  ZA_FILE             = file
  ZA_FILE_SRCDIR     := $(Z_SRCAPPDIR)/$(ZA_FILE)/src

file-shared: libfile-shared
	@cd $(ZA_FILE_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

file-static: libfile-static
	@cd $(ZA_FILE_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-file-shared:
	@cd $(ZA_FILE_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-file-static:
	@cd $(ZA_FILE_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static

  #----------------------------------------------------------#
  # dir
  ZA_DIR             = dir
  ZA_DIR_SRCDIR     := $(Z_SRCAPPDIR)/$(ZA_DIR)/src

dir-shared: libdir-shared libfile-shared libpath-shared libio-shared libargparse-shared
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

dir-static: libdir-static libfile-static libpath-static libio-static libargparse-static
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-dir-shared:
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-dir-static:
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
  #----------------------------------------------------------#
  # io
  ZA_IO             = io
  ZA_IO_SRCDIR     := $(Z_SRCAPPDIR)/$(ZA_IO)/src

io-shared: libio-shared
	@cd $(ZA_IO_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

io-static: libio-static
	@cd $(ZA_IO_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-io-shared:
	@cd $(ZA_IO_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-io-static:
	@cd $(ZA_IO_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
#----------------------------------------------------------#
  # term
  ZA_TERM          = term
  ZA_TERM_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_TERM)/src

term-shared: libterm-shared
	@cd $(ZA_TERM_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

term-static: libterm-static
	@cd $(ZA_TERM_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-term-shared:
	@cd $(ZA_TERM_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-term-static:
	@cd $(ZA_TERM_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
#----------------------------------------------------------#
  # i
  ZA_I          = i
  ZA_I_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_I)/src

i-shared: libi-shared
	@cd $(ZA_I_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

i-static: libi-static
	@cd $(ZA_I_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-i-shared:
	@cd $(ZA_I_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-i-static:
	@cd $(ZA_I_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
#----------------------------------------------------------#
  # e
  ZA_E          = e
  ZA_E_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_E)/src

e-shared: libe-shared libargparse-shared libproc-shared libstring-shared
	@cd $(ZA_E_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

e-static: libe-static libargparse-static libproc-static libstring-static
	@cd $(ZA_E_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-e-shared:
	@cd $(ZA_E_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-e-static:
	@cd $(ZA_E_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
#----------------------------------------------------------#
shared: zero-clean-shared zero-shared
zero-shared:  Env cenv dlist\
              libstring-shared libustring-shared libcstring-shared\
              libvstring-shared libdir-shared\
              libproc-shared libauth-shared librline-shared\
              libsh-shared libargparse-shared libio-shared\
              libfile-shared libpath-shared libterm-shared\
              libimap-shared libsmap-shared libvideo-shared\
              zs-shared zsu-shared\
              dir-shared file-shared io-shared term-shared

static: zero-clean-static zero-static
zero-static:  Env cenv dlist\
              libstring-static libustring-static libcstring-static\
              libvstring-static libdir-static\
              libproc-static libauth-static librline-static\
              libsh-static libargparse-static libio-static\
              libfile-static libpath-static libterm-static\
              libimap-static libsmap-static libvideo-static\
              zs-static\
              dir-static file-static io-static term-static

zero-clean-shared:\
              clean-cenv clean-dlist\
              clean-libstring-shared clean-libustring-shared\
              clean-libcstring-shared clean-libvstring-shared\
              clean-libdir-shared\
              clean-libproc-shared clean-libauth-shared\
              clean-librline-shared clean-libsh-shared\
              clean-libargparse-shared clean-libio-shared\
              clean-libfile-shared clean-libpath-shared\
              clean-libterm-shared clean-libimap-shared\
              clean-libsmap-shared clean-libvideo-shared\
              clean-libre-shared\
              clean-zs-shared clean-zsu-shared clean-dir-shared\
              clean-file-shared clean-io-shared clean-term-shared

zero-clean-static:\
              clean-cenv clean-dlist\
              clean-libstring-static clean-libustring-static\
              clean-libcstring-static clean-libvstring-static\
              clean-libdir-static\
              clean-libproc-static clean-libauth-static\
              clean-librline-static clean-libsh-static\
              clean-libargparse-static clean-libio-static\
              clean-libfile-static clean-libpath-static\
              clean-libterm-static clean-libimap-static\
              clean-libsmap-static clean-libvideo-static\
              clean-libre-static\
              clean-zs-static clean-zsu-static\
              clean-dir-static\
              clean-file-static clean-io-static clean-term-static\

all: shared static

#----------------------------------------------------------#
Env: makeenv checkenv
makeenv:
	@$(TEST) -d $(SYSDIR)      || $(MKDIR_P) $(SYSDIR)
	@$(TEST) -d $(SYSLIBDIR)   || $(MKDIR)   $(SYSLIBDIR)
	@$(TEST) -d $(SYSBINDIR)   || $(MKDIR)   $(SYSBINDIR)
	@$(TEST) -d $(SYSINCDIR)   || $(MKDIR)   $(SYSINCDIR)
	@$(TEST) -d $(SYSTMPDIR)   || $(MKDIR)   $(SYSTMPDIR)
	@$(TEST) -d $(SYSDATADIR)  || $(MKDIR)   $(SYSDATADIR)
	@$(TEST) -d $(Z_SYSDIR)    || $(MKDIR)   $(Z_SYSDIR)
	@$(TEST) -d $(Z_SYSBINDIR) || $(MKDIR)   $(Z_SYSBINDIR)
	@$(TEST) -d $(Z_SYSLIBDIR) || $(MKDIR)   $(Z_SYSLIBDIR)
	@$(TEST) -d $(Z_SYSINCDIR) || $(MKDIR)   $(Z_SYSINCDIR)

checkenv:
	@$(TEST) -w $(SYSDIR)      || exit 1
	@$(TEST) -w $(SYSBINDIR)   || exit 1
	@$(TEST) -w $(SYSLIBDIR)   || exit 1
	@$(TEST) -w $(SYSINCDIR)   || exit 1
	@$(TEST) -w $(SYSTMPDIR)   || exit 1
	@$(TEST) -w $(SYSDATADIR)  || exit 1
	@$(TEST) -w $(Z_SYSBINDIR) || exit 1
	@$(TEST) -w $(Z_SYSLIBDIR) || exit 1
	@$(TEST) -w $(Z_SYSINCDIR) || exit 1

INSTALL = install
RM = rm -f
CP = cp
AR = ar rs
CP_R = $(CP) -r
TEST = test
LN = ln
LN_S = $(LN) -s
MKDIR = mkdir
MKDIR_P = $(MKDIR) -p
