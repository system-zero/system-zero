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
SYSDATADIR     = $(SYSDIR)/data
SYSTMPDIR      = $(SYSDIR)/tmp

#-----------------------------------------------#
#             zero namespace

ZNAME       = z
Z_SRCDIR    = $(SRCDIR)/$(ZNAME)

  # z library namespace
  Z_SRCLIBDIR = $(Z_SRCDIR)/lib

    # our way to write C code
    ZC                   = zc
    Z_C_SRCDIR          := $(Z_SRCLIBDIR)/$(ZC)/src

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

    # Input Type
    Input_Type          = Input
    Z_INPUT_SRCDIR     := $(Z_SRCLIBDIR)/$(Input_Type)/src

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

    # Io Type
    Io_Type              = Io
    Z_IO_SRCDIR          = $(Z_SRCLIBDIR)/$(Io_Type)/src

    # Path Type
    Path_Type            = Path
    Z_PATH_SRCDIR        = $(Z_SRCLIBDIR)/$(Path_Type)/src

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

  BASE_FLAGS     = -I$(Z_SYSINCDIR) -L$(Z_SYSLIBDIR)
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
zc: Env
	@cd $(Z_C_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

clean_zc:
	@cd $(Z_C_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean
#----------------------------------------------------------#
dlist: Env
	@cd $(Z_DLIST_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

clean_dlist:
	@cd $(Z_DLIST_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean
#----------------------------------------------------------#
libstring-shared: Env zc
	@cd $(Z_STRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libstring-static: Env zc
	@cd $(Z_STRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libstring_shared:
	@cd $(Z_STRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libstring_static:
	@cd $(Z_STRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libustring-shared: Env zc
	@cd $(Z_USTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libustring-static: Env zc
	@cd $(Z_USTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libustring_shared:
	@cd $(Z_USTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libustring_static:
	@cd $(Z_USTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libcstring-shared: Env zc
	@cd $(Z_CSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libcstring-static: Env zc
	@cd $(Z_CSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libcstring_shared:
	@cd $(Z_CSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libcstring_static:
	@cd $(Z_CSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libvstring-shared: Env zc libstring-shared libcstring-shared
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libvstring-static: Env zc libstring-static libcstring-static
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libvstring_shared:
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libvstring_static:
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libdir-shared: Env zc libstring-shared libvstring-shared libcstring-shared libpath-shared
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libdir-static: Env zc libstring-static libvstring-static libcstring-static libpath-static
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libdir_shared:
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libdir_static:
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libinput-shared: Env zc libustring-shared
	@cd $(Z_INPUT_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libinput-static: Env zc libustring-static
	@cd $(Z_INPUT_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libinput_shared:
	@cd $(Z_INPUT_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libinput_static:
	@cd $(Z_INPUT_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libauth-shared: Env zc libstring-shared libcstring-shared libustring-shared libinput-shared
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libauth-static: Env zc libstring-static libcstring-static libustring-static libinput-static
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libauth_shared:
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libauth_static:
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libproc-shared: Env zc dlist libcstring-shared
	@cd $(Z_PROC_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libproc-static: Env zc dlist libcstring-static
	@cd $(Z_PROC_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libproc_shared:
	@cd $(Z_PROC_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libproc_static:
	@cd $(Z_PROC_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
librline-shared: Env zc libstring-shared
	@cd $(Z_RLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

librline-static: Env zc libstring-static
	@cd $(Z_RLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_librline_shared:
	@cd $(Z_RLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_librline_static:
	@cd $(Z_RLINE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libsh-shared: Env zc libcstring-shared libproc-shared
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libsh-static: Env zc libcstring-static libproc-static
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libsh_shared:
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libsh_static:
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libfile-shared: Env zc libstring-shared libcstring-shared libvstring-shared libdir-shared libpath-shared
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libfile-static: Env zc libstring-static libcstring-static libvstring-static libdir-static libpath-static
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libfile_shared:
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libfile_static:
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libargparse-shared: Env zc libstring-shared libcstring-shared libvstring-shared libdir-shared
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libargparse-static: Env zc libstring-static libcstring-static libvstring-static libdir-static
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libargparse_shared:
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libargparse_static:
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libio-shared: Env zc
	@cd $(Z_IO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libio-static: Env zc
	@cd $(Z_IO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libio_shared:
	@cd $(Z_IO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libio_static:
	@cd $(Z_IO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
libpath-shared: Env zc libcstring-shared
	@cd $(Z_PATH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libpath-static: Env zc libcstring-static
	@cd $(Z_PATH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean_libpath_shared:
	@cd $(Z_PATH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_libpath_static:
	@cd $(Z_PATH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
zs-shared: Env zc libcstring-shared libsh-shared librline-shared
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

zs-static: Env zc libcstring-static libsh-static librline-static
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

clean_zs_shared:
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_zs_static:
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
zsu-shared: Env zc libcstring-shared libstring-shared libproc-shared libauth-shared
	cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) zsu-shared
	@cd $(Z_SU_SRCDIR) && sudo $(MAKE) $(ZARGS) $(MARGS) install-shared
	@cd $(Z_SU_SRCDIR) && $(RM) zsu-shared

# can not confirm (a libpam statically linked is required)
# other warnings might make this target unsuitable, e.g.,
#   warning: Using 'initgroups' in statically linked applications
#   requires at runtime the shared libraries from the glibc version
#   used for linking
zsu-static: Env zc libcstring-static libstring-static libproc-static libauth-static
	@cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) zsu-static
	@cd $(Z_SU_SRCDIR) && sudo $(MAKE) $(ZARGS) $(MARGS) install-static
	@cd $(Z_SU_SRCDIR) && $(RM) zsu-static

clean_zsu_shared:
	@cd $(Z_SU_SRCDIR) && sudo $(MAKE) $(ZARGS) $(MARGS) clean_shared

clean_zsu_static:
	@cd $(Z_SU_SRCDIR) && sudo $(MAKE) $(ZARGS) $(MARGS) clean_static
#----------------------------------------------------------#
# App+

APP_FLAGS  :=
APP_PREFIX := __
APP_FLAGS  += APP_PREFIX=$(APP_PREFIX)
APP_FLAGS  += $(ZARGS) $(MARGS)

  # file
  ZA_FILE             = zfile
  ZA_FILE_SRCDIR     := $(Z_SRCAPPDIR)/$(ZA_FILE)/src

zfile-shared: libfile-shared
	@cd $(ZA_FILE_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

zfile-static: libfile-static
	@cd $(ZA_FILE_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean_zfile_shared:
	@cd $(ZA_FILE_SRCDIR) && $(MAKE) $(APP_FLAGS) clean_shared

clean_zfile_static:
	@cd $(ZA_FILE_SRCDIR) && $(MAKE) $(APP_FLAGS) clean_static

  #----------------------------------------------------------#
  # dir
  ZA_DIR             = zdir
  ZA_DIR_SRCDIR     := $(Z_SRCAPPDIR)/$(ZA_DIR)/src

zdir-shared: libdir-shared libfile-shared libpath-shared libio-shared libargparse-shared
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

zdir-static: libdir-static libfile-static libpath-static libio-statuc libargparse-static
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean_zdir_shared:
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) clean_shared

clean_zdir_static:
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) clean_static
#----------------------------------------------------------#
zero-shared:  zero_clean_shared\
              Env zc dlist\
              libstring-shared libustring-shared libcstring-shared\
              libvstring-shared libdir-shared libinput-shared\
              libproc-shared libauth-shared librline-shared\
              libsh-shared libargparse-shared libio-shared\
              libfile-shared libpath-shared\
              zs-shared zsu-shared

zero-static:  zero_clean_static\
              Env zc dlist\
              libstring-static libustring-static libcstring-static\
              libvstring-static libdir-static libinput-static\
              libproc-static libauth-static librline-static\
              libsh-static libargparse-static libio-static\
              libfile-static libpath-static\
              zs-static

zero_clean_shared:\
              clean_zc clean_dlist\
              clean_libstring_shared clean_libustring_shared\
              clean_libcstring_shared clean_libvstring_shared\
              clean_libdir_shared clean_libinput_shared\
              clean_libproc_shared clean_libauth_shared\
              clean_librline_shared clean_libsh_shared\
              clean_libargparse_shared clean_libio_shared\
              clean_libfile_shared clean_libpath_shared\
              clean_zs_shared clean_zsu_shared

zero_clean_static:\
              clean_zc clean_dlist\
              clean_libstring_static clean_libustring_static\
              clean_libcstring_static clean_libvstring_static\
              clean_libdir_static clean_libinput_static\
              clean_libproc_static clean_libauth_static\
              clean_librline_static clean_libsh_static\
              clean_libargparse_static clean_libio_static\
              clean_libfile_static clean_libpath_static\
              clean_zs_static clean_zsu_static

all: zero-shared zero-static

#----------------------------------------------------------#
Env: makeenv checkenv
makeenv:
	@$(TEST) -d $(SYSDIR)      || $(MKDIR_P) $(SYSDIR)
	@$(TEST) -d $(SYSLIBDIR)   || $(MKDIR)   $(SYSLIBDIR)
	@$(TEST) -d $(SYSBINDIR)   || $(MKDIR)   $(SYSBINDIR)
	@$(TEST) -d $(SYSINCDIR)   || $(MKDIR)   $(SYSINCDIR)
	@$(TEST) -d $(SYSTMPDIR)   || $(MKDIR)   $(SYSTMPDIR)
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
