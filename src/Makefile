# This Makefile is to help us to initialize a minimun functional environment.
# It also serves as an introductory document, as it declares intentions and describe
# the implementation through some loud thoughts in comments, so it is more verbose
# than in usual Makefiles and serve also to initialize also a developer way of
# thinking. So though there are might be some pointers (for those that are not
# familiar with programming consept), it was really written for developers, but it
# might be used by others too.

# This is our actual first file, and it is the one that will assist us to build the
# environment. It also serves as a vehicle to understand the needs, establish the
# hierarhy and finally to end up on things. So at the same time there are development
# type of comments.

# We use a Make type of building system, which is and the most primitive in Unix land.
# Particularly the GNU make implementation, though we want to be POSIX compatible, so
# this Makefile code can be end up to be portable. We'll do this with time and iterations.

#-----------------------------------------------#
# Application Interface and versioning.
API     := 0
REV     := 3
VERSION  = $(API).$(REV)
#-----------------------------------------------#
#           HIERARCHY

# We want this variable to be treated as immutable but we can't, as this it cannot
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
# implementation or realpath, as it is quite useful to sanitize paths).

# our sources namespace. We want this also in a constant state (again this is not possible)
SRCDIR         = $(THISDIR)

# machine architecture
SYSARCH       := $(shell uname -m)

# our target directory.
# If it is not already defined in the command line as:
#   make SYSDIR=/some/dir [target]
# or from another Makefile, then by default is this_directory/../sys/$(uname -m)
SYSDIR        := $(THISDIR)/../sys/$(SYSARCH)

SYSLIBDIR      = $(SYSDIR)/lib
SYSBINDIR      = $(SYSDIR)/bin
SYSINCDIR      = $(SYSDIR)/include

# (not sure about those two yet: the question is: should be one or two? 
#  perhaps a USER_ prefix is justified, though this can be abstracted
#  at runtime by our identity)
SYSDATADIR     = $(SYSDIR)/../data
SYSTMPDIR      = $(SYSDIR)/../tmp

SYSLADIR       = $(SYSDATADIR)/la
SYSSCRIPTDIR   = $(SYSLADIR)/scripts
SYSMODULEDIR   = $(SYSLIBDIR)/la-modules
SYSSCRIPTDIR   = $(SYSLIBDIR)/la-lib
SYSZMODULEDIR  = $(SYSLIBDIR)/z/la-modules
SYSZSCRIPTDIR  = $(SYSLIBDIR)/z/la-lib

UID       := $(shell id -u)
GID       := $(shell id -g)
USERNAME  := $(shell id -un)
GROUPNAME := $(shell id -gn)
HOME      := $(HOME)
TERM      := $(TERM)

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

    # our own implementation of C string functions with byte semantics
    Cstring_Type         = Cstring
    Z_CSTRING_SRCDIR    := $(Z_SRCLIBDIR)/$(Cstring_Type)/src

    # likewise a (UTF8) library to handle strings with character semantics
    Ustring_Type         = Ustring
    Z_USTRING_SRCDIR    := $(Z_SRCLIBDIR)/$(Ustring_Type)/src

    # a Directory Type
    Dir_Type             = Dir
    Z_DIR_SRCDIR        := $(Z_SRCLIBDIR)/$(Dir_Type)/src

    # Authentication Type (this requires libpam)
    Auth_Type            = Auth
    Z_AUTH_SRCDIR       := $(Z_SRCLIBDIR)/$(Auth_Type)/src

    # a Proc Type
    Proc_Type            = Proc
    Z_PROC_SRCDIR       := $(Z_SRCLIBDIR)/$(Proc_Type)/src

    # Readline Type (based on linenoise)
    Rline_Type           = Rline
    Z_RLINE_SRCDIR       = $(Z_SRCLIBDIR)/$(Rline_Type)/src

    # Shell Type
    Sh_Type              = Sh
    Z_SH_SRCDIR          = $(Z_SRCLIBDIR)/$(Sh_Type)/src

    # OS Type
    OS_Type              = OS
    Z_OS_SRCDIR          = $(Z_SRCLIBDIR)/$(OS_Type)/src

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

    # Smap Type
    Smap_Type            = Smap
    Z_SMAP_SRCDIR        = $(Z_SRCLIBDIR)/$(Smap_Type)/src

    # Vmap Type
    Vmap_Type            = Vmap
    Z_VMAP_SRCDIR        = $(Z_SRCLIBDIR)/$(Vmap_Type)/src

    # Re Type
    Re_Type              = Re
    Z_RE_SRCDIR          = $(Z_SRCLIBDIR)/$(Re_Type)/src

    # Sys Type
    Sys_Type             = Sys
    Z_SYS_SRCDIR         = $(Z_SRCLIBDIR)/$(Sys_Type)/src

    # Error Type
    Error_Type           = Error
    Z_ERROR_SRCDIR       = $(Z_SRCLIBDIR)/$(Error_Type)/src

    # E Type
    E_Type               = E
    Z_E_SRCDIR           = $(Z_SRCLIBDIR)/$(E_Type)/src

    # I Type (not used anymore, but stays as a reference)
    I_Type               = I
    Z_I_SRCDIR           = $(Z_SRCLIBDIR)/$(I_Type)/src

    La_Type              = La
    Z_LA_SRCDIR          = $(Z_SRCLIBDIR)/$(La_Type)/src

    # Video Type
    Video_Type           = Video
    Z_VIDEO_SRCDIR       = $(Z_SRCLIBDIR)/$(Video_Type)/src

    # Readline Type (this is the main autocompletion mechanism)
    Readline_Type        = Readline
    Z_READLINE_SRCDIR    = $(Z_SRCLIBDIR)/$(Readline_Type)/src

    # Spell Type
    Spell_Type           = Spell
    Z_SPELL_SRCDIR       = $(Z_SRCLIBDIR)/$(Spell_Type)/src

    # Json Type
    Json_Type           = Json
    Z_JSON_SRCDIR       = $(Z_SRCLIBDIR)/$(Json_Type)/src

    # Vui Type
    Vui_Type            = Vui
    Z_VUI_SRCDIR        = $(Z_SRCLIBDIR)/$(Vui_Type)/src

    # Vwm Type
    Vwm_Type           = Vwm
    Z_VWM_SRCDIR       = $(Z_SRCLIBDIR)/$(Vwm_Type)/src

    # V Type
    V_Type             = V
    Z_V_SRCDIR         = $(Z_SRCLIBDIR)/$(V_Type)/src

    # Md5 Type
    Md5_Type           = Md5
    Z_MD5_SRCDIR       = $(Z_SRCLIBDIR)/$(Md5_Type)/src

    # Sha256 Type
    Sha256_Type        = Sha256
    Z_SHA256_SRCDIR    = $(Z_SRCLIBDIR)/$(Sha256_Type)/src

    # Sha512 Type
    Sha512_Type        = Sha512
    Z_SHA512_SRCDIR    = $(Z_SRCLIBDIR)/$(Sha512_Type)/src

    # Random Type
    Random_Type        = Random
    Z_RANDOM_SRCDIR    = $(Z_SRCLIBDIR)/$(Random_Type)/src

    # Base64 Type
    Base64_Type        = Base64
    Z_BASE64_SRCDIR    = $(Z_SRCLIBDIR)/$(Base64_Type)/src

    # Bcrypt Type
    Bcrypt_Type        = Bcrypt
    Z_BCRYPT_SRCDIR    = $(Z_SRCLIBDIR)/$(Bcrypt_Type)/src

    # Contain Type
    Contain_Type       = Contain
    Z_CONTAIN_SRCDIR   = $(Z_SRCLIBDIR)/$(Contain_Type)/src

    # Net Type
    Net_Type       = Net
    Z_NET_SRCDIR   = $(Z_SRCLIBDIR)/$(Net_Type)/src

    # Url Type
    Url_Type       = Url
    Z_URL_SRCDIR   = $(Z_SRCLIBDIR)/$(Url_Type)/src

    # Dl Type
    Dl_Type       = Dl
    Z_DL_SRCDIR   = $(Z_SRCLIBDIR)/$(Dl_Type)/src
    # Moon_Type
    Moon_Type       = Moon
    Z_MOON_SRCDIR   = $(Z_SRCLIBDIR)/$(Moon_Type)/src

    # Sun_Type
    Sun_Type       = Sun
    Z_SUN_SRCDIR   = $(Z_SRCLIBDIR)/$(Sun_Type)/src

    # Man_Type
    Man_Type       = Man
    Z_MAN_SRCDIR   = $(Z_SRCLIBDIR)/$(Man_Type)/src

    # Sudoku_Type
    Sudoku_Type       = Sudoku
    Z_SUDOKU_SRCDIR   = $(Z_SRCLIBDIR)/$(Sudoku_Type)/src

    # though not a type, this is a rich double linked list which is implemented
    # with macros, to gain abstraction.
    # A compatible container should include:
    #   head/current/tail pointers
    #   int cur_idx; int num_items; 
    # for any item in the list, it is assumed prev/next pointers
    Z_LIST_SRCDIR      := $(Z_SRCLIBDIR)/list/src
    # it is extended with a list stack implementation (it requires a next pointer)

    # finally, a self library used by the author (ignore)
    Self_Type          = Self
    Z_SELF_SRCDIR      = $(Z_SRCLIBDIR)/$(Self_Type)/src

  # C applicarion namespace
  Z_SRCAPPDIR  = $(Z_SRCDIR)/app

    # shell
    Z_SHELL             = zs
    Z_SHELL_SRCDIR     := $(Z_SRCAPPDIR)/$(Z_SHELL)/src
    Z_SHELL_DATADIR    := $(SYSDATADIR)/$(Z_SHELL)

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
  #  - the new directory that overrides one or all of the above,
  #    should respect this hierarchy, that means:
  #      - the name of the compilation units should match
  #      - provide a Makefile that could handle the compilation

  Z_SRCMODULEDIR = $(Z_SRCDIR)/la-modules
  Z_SRCLALIBDIR  = $(Z_SRCDIR)/la-lib

#-----------------------------------------------#

Z_SYSLIBDIR  = $(SYSLIBDIR)/$(ZNAME)
Z_SYSINCDIR  = $(SYSINCDIR)/$(ZNAME)

# In this level we want a shell and an utility that will permit
# us to execute commands with SuperUser (su for sort) rights.
# This level should offer a way to do machine administration,
# that means in a Unix machine, and if we have the required rights, 
# that the time we gain access, it is the time we own the system.

#-----------------------------------------------#
#  C Implementation

  # we follow GNU cc, but we also care a bit for clang and especially tcc
  CC            := gcc

  # C11 is our minimum standard
  # (but we might be compiled by olders, but its not our buisness)
  CC_STD        := -std=c11

  BASE_FLAGS     = -I$(SYSINCDIR) -L$(Z_SYSLIBDIR)
  BASE_FLAGS    += -g -O2 -march=native -fdollars-in-identifiers
  BASE_FLAGS    += -DDECLARE=1 -DDONOT_DECLARE=0

  # this is for ld --version => 2.36.1 (allow multiply definitions (for AllocErrorHandler in static builds))
  ifneq (-$(CC), -tcc)
    BASE_FLAGS    += -z muldefs
  endif

  DEBUG_FLAGS   := -Wextra -Wshadow -Wall -Wunused-result -Wunused-function -Wunused-macros -Wno-override-init
  DEBUG_FLAGS   += -Werror-implicit-function-declaration -Wsign-compare
  DEBUG_FLAGS   += -Wpointer-arith -Wreturn-type -Wmissing-declarations -Wwrite-strings
  DEBUG_FLAGS   += -Wuninitialized

  ifeq (-$(CC), -gcc)
    DEBUG_FLAGS   += -Wimplicit-fallthrough -Wrestrict
  endif

  ifeq (-$(CC), -clang)
    BASE_FLAGS  +=  -Wno-unused-command-line-argument
    BASE_FLAGS  +=  -Wno-override-init
  endif

  DEBUG         := 0
  DEBUG_SYMBOLS := 1
  RPATH         := 1

  # we use rpath in this early stage, so we do not need to run ldconfig
  ifeq ($(RPATH), 1)
    BASE_FLAGS  += -Wl,-rpath=$(Z_SYSLIBDIR)
  endif

  ZARGS  = NS=$(ZNAME)
  MARGS :=
  MARGS  = API=$(API) REV=$(REV) VERSION=$(VERSION) DEBUG=$(DEBUG)
  MARGS += CC=$(CC) CC_STD=$(CC_STD)
  MARGS += SRCDIR=$(SRCDIR)
  MARGS += SYSDIR=$(SYSDIR) SYSDATADIR=$(SYSDATADIR) SYSTMPDIR=$(SYSTMPDIR)
  MARGS += Z_SHELL_DATADIR=$(Z_SHELL_DATADIR) Z_SHELL=$(Z_SHELL)
  MARGS += BASE_FLAGS='$(BASE_FLAGS)'
  MARGS += USERNAME=$(USERNAME) GROUPNAME=$(GROUPNAME)
  MARGS += HOME=$(HOME) UID=$(UID) GID=$(GID) TERM=$(TERM)
  ifeq ($(DEBUG_SYMBOLS), 1)
    MARGS += DEBUG_FLAGS='$(DEBUG_FLAGS)'
  endif


#----------------------------------------------------------#
cenv: Env
	@cd $(Z_CENV_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

clean-cenv:
	@cd $(Z_CENV_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean
#----------------------------------------------------------#
list: Env
	@cd $(Z_LIST_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

clean-list:
	@cd $(Z_LIST_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean
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
libustring-shared: Env cenv list
	@cd $(Z_USTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libustring-static: Env cenv list
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
libvstring-shared: Env cenv list libstring-shared libcstring-shared
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libvstring-static: Env cenv list libstring-static libcstring-static
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libvstring-shared:
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libvstring-static:
	@cd $(Z_VSTRING_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libdir-shared: Env cenv libstring-shared libvstring-shared libcstring-shared\
               liberror-shared libpath-shared libos-shared
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libdir-static: Env cenv libstring-static libvstring-static libcstring-static\
               liberror-static libpath-static libos-static
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libdir-shared:
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libdir-static:
	@cd $(Z_DIR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libauth-shared: Env cenv libstring-shared libcstring-shared libustring-shared\
                libio-shared libterm-shared liberror-shared libos-shared
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libauth-static: Env cenv libstring-static libcstring-static libustring-static\
                libio-static libterm-static liberror-static libos-static
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libauth-shared:
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libauth-static:
	@cd $(Z_AUTH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libproc-shared: Env cenv list libcstring-shared libsys-shared libstring-shared
	@cd $(Z_PROC_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libproc-static: Env cenv list libcstring-static libsys-static libstring-static
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
libsh-shared: Env cenv libstring-shared libio-shared libcstring-shared\
                       libfile-shared libproc-shared liberror-shared\
                       libdir-shared libsys-shared
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libsh-static: Env cenv libstring-static libio-static libcstring-static\
                       libfile-static libproc-static liberror-static\
                       libdir-static libsys-static
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libsh-shared:
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libsh-static:
	@cd $(Z_SH_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libos-shared: Env cenv libcstring-shared
	@cd $(Z_OS_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libos-static: Env cenv libcstring-static
	@cd $(Z_OS_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libos-shared:
	@cd $(Z_OS_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libos-static:
	@cd $(Z_OS_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#               libos-shared libio-shared libterm-shared librandom-shared libsys-shared
#----------------------------------------------------------#
libfile-shared: Env cenv libstring-shared libcstring-shared libvstring-shared libdir-shared libpath-shared\
                libos-shared libio-shared libterm-shared librandom-shared
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libfile-static: Env cenv libstring-static libcstring-static libvstring-static libdir-static libpath-static\
                libos-static libio-static libterm-static librandom-static
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libfile-shared:
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libfile-static:
	@cd $(Z_FILE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libargparse-shared: Env cenv libio-shared libstring-shared libcstring-shared\
                    libvstring-shared libdir-shared liberror-shared
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libargparse-static: Env cenv libio-static libstring-static libcstring-static\
                    libvstring-static libdir-static liberror-static
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libargparse-shared:
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libargparse-static:
	@cd $(Z_ARGPARSE_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libio-shared: Env cenv libstring-shared libustring-shared
	@cd $(Z_IO_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libio-static: Env cenv libstring-static libustring-static
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
libvmap-shared: Env cenv libcstring-shared
	@cd $(Z_VMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libvmap-static: Env cenv libcstring-static
	@cd $(Z_VMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libvmap-shared:
	@cd $(Z_VMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libvmap-static:
	@cd $(Z_VMAP_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
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
liberror-shared: Env cenv
	@cd $(Z_ERROR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

liberror-static: Env cenv
	@cd $(Z_ERROR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-liberror-shared:
	@cd $(Z_ERROR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-liberror-static:
	@cd $(Z_ERROR_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libsys-shared: Env cenv libcstring-shared libstring-shared libfile-shared\
               libsmap-shared libpath-shared
	@cd $(Z_SYS_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libsys-static: Env cenv libcstring-static libstring-static libfile-static\
               libsmap-static libpath-static
	@cd $(Z_SYS_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libsys-shared:
	@cd $(Z_SYS_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libsys-static:
	@cd $(Z_SYS_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
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
libvui-shared: Env cenv libcstring-shared libstring-shared\
               libvideo-shared libreadline-shared
	@cd $(Z_VUI_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libvui-static: Env cenv libcstring-static libstring-static\
               libvideo-static libreadline-static
	@cd $(Z_VUI_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libvui-shared:
	@cd $(Z_VUI_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libvui-static:
	@cd $(Z_VUI_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libspell-shared: Env cenv libcstring-shared libstring-shared libustring-shared\
                 liberror-shared libio-shared
	@cd $(Z_SPELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libspell-static: Env cenv libcstring-static libstring-static libustring-static\
                 liberror-static libio-static
	@cd $(Z_SPELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libspell-shared:
	@cd $(Z_SPELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libspell-static:
	@cd $(Z_SPELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libjson-shared: Env cenv libcstring-shared libstring-shared
	@cd $(Z_JSON_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libjson-static: Env cenv libcstring-static libstring-static
	@cd $(Z_JSON_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libjson-shared:
	@cd $(Z_JSON_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libjson-static:
	@cd $(Z_JSON_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libe-shared: Env cenv list libstring-shared libcstring-shared libvstring-shared libustring-shared\
             libpath-shared libfile-shared libdir-shared libterm-shared libio-shared\
             libre-shared libimap-shared libsmap-shared libla-shared\
             libproc-shared libvui-shared\
             libsys-shared libspell-shared libvideo-shared libreadline-shared
	@cd $(Z_E_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libe-static: Env cenv list libstring-static libcstring-static libvstring-static libustring-static\
             libpath-static libfile-static libdir-static libterm-static libio-static\
             libre-static libimap-static libsmap-static libla-static\
             libproc-static libvui-static\
             libsys-static libspell-static libvideo-static libreadline-static
	@cd $(Z_E_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libe-shared:
	@cd $(Z_E_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libe-static:
	@cd $(Z_E_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libi-shared: Env cenv list libstring-shared libcstring-shared\
             libvstring-shared libustring-shared libpath-shared libfile-shared\
             liberror-shared libvmap-shared libio-shared
	@cd $(Z_I_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libi-static: Env cenv list libstring-static libcstring-static\
             libvstring-static libustring-static libpath-static libfile-static\
             liberror-static libvmap-static libio-static
	@cd $(Z_I_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libi-shared:
	@cd $(Z_I_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libi-static:
	@cd $(Z_I_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libla-shared: Env cenv list libstring-shared libcstring-shared libimap-shared\
              libvstring-shared libustring-shared libpath-shared libfile-shared\
              liberror-shared libvmap-shared libio-shared libdir-shared libterm-shared
	@cd $(Z_LA_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libla-static: Env cenv list libstring-static libcstring-static libimap-static\
              libvstring-static libustring-static libpath-static libfile-static\
               liberror-static libvmap-static libio-static libdir-static libterm-static\
               libmd5-static libsha256-static libsha512-static\
               libbase64-static libbcrypt-static
	@cd $(Z_LA_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libla-shared:
	@cd $(Z_LA_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libla-static:
	@cd $(Z_LA_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
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
libvwm-shared: Env cenv list libstring-shared libcstring-shared\
               libustring-shared libdir-shared libfile-shared\
               libvideo-shared libio-shared libterm-shared\
               libvui-shared\
               libreadline-shared libsys-shared liberror-shared
	@cd $(Z_VWM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libvwm-static: Env cenv list libstring-static libcstring-static\
               libustring-static libdir-static libfile-static\
               libvideo-static libio-static libterm-static\
               libvui-static\
               libreadline-static libsys-static liberror-static
	@cd $(Z_VWM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libvwm-shared:
	@cd $(Z_VWM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libvwm-static:
	@cd $(Z_VWM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libv-shared: Env cenv libvwm-shared libla-shared
	@cd $(Z_V_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libv-static: Env cenv list libvwm-static libla-static
	@cd $(Z_V_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libv-shared:
	@cd $(Z_V_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libv-static:
	@cd $(Z_V_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libmd5-shared: Env cenv
	@cd $(Z_MD5_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libmd5-static: Env cenv
	@cd $(Z_MD5_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libmd5-shared:
	@cd $(Z_MD5_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libmd5-static:
	@cd $(Z_MD5_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libsha256-shared: Env cenv
	@cd $(Z_SHA256_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libsha256-static: Env cenv
	@cd $(Z_SHA256_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libsha256-shared:
	@cd $(Z_SHA256_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libsha256-static:
	@cd $(Z_SHA256_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libsha512-shared: Env cenv
	@cd $(Z_SHA512_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libsha512-static: Env cenv
	@cd $(Z_SHA512_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libsha512-shared:
	@cd $(Z_SHA512_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libsha512-static:
	@cd $(Z_SHA512_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
librandom-shared: Env cenv
	@cd $(Z_RANDOM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

librandom-static: Env cenv
	@cd $(Z_RANDOM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-librandom-shared:
	@cd $(Z_RANDOM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-librandom-static:
	@cd $(Z_RANDOM_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libbase64-shared: Env cenv
	@cd $(Z_BASE64_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libbase64-static: Env cenv
	@cd $(Z_BASE64_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libbase64-shared:
	@cd $(Z_BASE64_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libbase64-static:
	@cd $(Z_BASE64_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libbcrypt-shared: Env cenv
	@cd $(Z_BCRYPT_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libbcrypt-static: Env cenv
	@cd $(Z_BCRYPT_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libbcrypt-shared:
	@cd $(Z_BCRYPT_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libbcrypt-static:
	@cd $(Z_BCRYPT_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libcontain-shared: Env cenv
	@cd $(Z_CONTAIN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libcontain-static: Env cenv
	@cd $(Z_CONTAIN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libcontain-shared:
	@cd $(Z_CONTAIN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libcontain-static:
	@cd $(Z_CONTAIN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
# working library with two interfaces:
#   - a quite minimal http[s] client that can link against for both openssl and libressl
#   - a wrapper around libcurl, thus this it can get very rich as we like
# so the path isn't quite up yet, but the target stays for development reasons.
libnet-shared: Env cenv liburl-shared
	@cd $(Z_NET_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libnet-static: Env cenv liburl-static
	@cd $(Z_NET_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libnet-shared:
	@cd $(Z_NET_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libnet-static:
	@cd $(Z_NET_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
URL_EXTENTED               := 1
URL_USE_INTERNAL_FUNCTIONS := 1

URL_FLAGS = URL_EXTENTED=$(URL_EXTENTED) URL_USE_INTERNAL_FUNCTIONS=$(URL_USE_INTERNAL_FUNCTIONS)
liburl-shared: Env cenv
	@cd $(Z_URL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $(URL_FLAGS) shared

liburl-static: Env cenv
	@cd $(Z_URL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $(URL_FLAGS) static

clean-liburl-shared:
	@cd $(Z_URL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-liburl-static:
	@cd $(Z_URL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libdl-shared: Env cenv
	@cd $(Z_DL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libdl-static: Env cenv
	@cd $(Z_DL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libdl-shared:
	@cd $(Z_DL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libdl-static:
	@cd $(Z_DL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libmoon-shared: Env cenv
	@cd $(Z_MOON_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libmoon-static: Env cenv
	@cd $(Z_MOON_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libmoon-shared:
	@cd $(Z_MOON_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libmoon-static:
	@cd $(Z_MOON_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libsun-shared: Env cenv
	@cd $(Z_SUN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libsun-static: Env cenv
	@cd $(Z_SUN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libsun-shared:
	@cd $(Z_SUN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libsun-static:
	@cd $(Z_SUN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libman-shared: Env cenv
	@cd $(Z_MAN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libman-static: Env cenv
	@cd $(Z_MAN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libman-shared:
	@cd $(Z_MAN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libman-static:
	@cd $(Z_MAN_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
libsudoku-shared: Env cenv
	@cd $(Z_SUDOKU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libsudoku-static: Env cenv
	@cd $(Z_SUDOKU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libsudoku-shared:
	@cd $(Z_SUDOKU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libsudoku-static:
	@cd $(Z_SUDOKU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#

# Development and personal stuff (ignore this section)
libself-shared: Env cenv libstring-shared libcstring-shared\
                libvstring-shared libdir-shared libfile-shared\
                libpath-shared libre-shared e-shared la-shared\
                v-shared
	@cd $(Z_SELF_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libself-only-shared: Env cenv
	@cd $(Z_SELF_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) shared

libself-static: Env cenv libstring-static libcstring-static\
                libvstring-static libdir-static libfile-static\
                libpath-static libre-static la-static e-static\
                v-static
	@cd $(Z_SELF_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

libself-only-static: Env cenv
	@cd $(Z_SELF_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) static

clean-libself-shared:
	@cd $(Z_SELF_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-libself-static:
	@cd $(Z_SELF_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static

#----------------------------------------------------------#


# STD APPLICATIONS
#----------------------------------------------------------#
zs-shared: Env cenv libcstring-shared libsh-shared librline-shared\
           libsys-shared liberror-shared
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

zs-static: Env cenv libcstring-static libsh-static librline-static\
           libsys-static liberror-static libio-static libustring-static
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) $@

clean-zs-shared:
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-zs-static:
	@cd $(Z_SHELL_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
#----------------------------------------------------------#
zsu-shared: Env cenv libcstring-shared libstring-shared libproc-shared\
            liberror-shared libauth-shared
	@cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) zsu-shared
	@cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) install-shared
	@cd $(Z_SU_SRCDIR) && $(RM) zsu-shared

# cannot compile (a statically linked libpam is required)
# other warnings might make this target unsuitable, e.g.,
#   warning: Using 'initgroups' in statically linked applications
#   requires at runtime the shared libraries from the glibc version
#   used for linking
zsu-static: Env cenv libcstring-static libstring-static libproc-static\
            liberror-static libauth-static
	@cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) zsu-static
	@cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) install-static
	@cd $(Z_SU_SRCDIR) && $(RM) zsu-static

clean-zsu-shared:
	@cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-shared

clean-zsu-static:
	@cd $(Z_SU_SRCDIR) && $(MAKE) $(ZARGS) $(MARGS) clean-static
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
  # time
  ZA_TIME             = time
  ZA_TIME_SRCDIR     := $(Z_SRCAPPDIR)/$(ZA_TIME)/src

time-shared:
	@cd $(ZA_TIME_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

time-static:
	@cd $(ZA_TIME_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-time-shared:
	@cd $(ZA_TIME_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-time-static:
	@cd $(ZA_TIME_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
  #----------------------------------------------------------#
  # dir
  ZA_DIR             = dir
  ZA_DIR_SRCDIR     := $(Z_SRCAPPDIR)/$(ZA_DIR)/src

dir-shared: libdir-shared libfile-shared libpath-shared libio-shared\
            liberror-shared libargparse-shared
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

dir-static: libdir-static libfile-static libpath-static libio-static\
            liberror-static libargparse-static
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-dir-shared:
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-dir-static:
	@cd $(ZA_DIR_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
  #----------------------------------------------------------#
  # path
  ZA_PATH             = path
  ZA_PATH_SRCDIR     := $(Z_SRCAPPDIR)/$(ZA_PATH)/src

path-shared: libpath-shared libla-shared path-module-shared
	@cd $(ZA_PATH_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

path-static: libpath-static libla-static la-static
	@cd $(ZA_PATH_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-path-shared:
	@cd $(ZA_PATH_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-path-static:
	@cd $(ZA_PATH_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
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

i-shared: libargparse-shared libi-shared
	@cd $(ZA_I_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

i-static: libargparse-static libi-static
	@cd $(ZA_I_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-i-shared:
	@cd $(ZA_I_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-i-static:
	@cd $(ZA_I_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
#----------------------------------------------------------#
  # L
  ZA_LA          = la
  ZA_LA_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_LA)/src

lai-shared: libargparse-shared librline-shared libla-shared
	@cd $(ZA_LA_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

lai-static: libargparse-static librline-static libla-static
	@cd $(ZA_LA_SRCDIR) && $(MAKE) $(APP_FLAGS) static

la-shared: lai-shared modules-shared la-library packman

la-static: libsh-static libsys-static libos-static\
           libdir-static libfile-static librandom-static\
           libmd5-static libsha256-static libsha512-static\
           libio-static\
           lai-static la-library

clean-lai-shared:
	@cd $(ZA_LA_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-lai-static:
	@cd $(ZA_LA_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
#----------------------------------------------------------#
  # e
  ZA_E          = e
  ZA_E_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_E)/src

e-shared: libe-shared libargparse-shared libproc-shared libstring-shared liberror-shared
	@cd $(ZA_E_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

e-static: libe-static libargparse-static libproc-static libstring-static liberror-static
	@cd $(ZA_E_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-e-shared:
	@cd $(ZA_E_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-e-static:
	@cd $(ZA_E_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
#----------------------------------------------------------#
  # sys
  ZA_SYS          = sys
  ZA_SYS_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_SYS)/src

sys-shared: libsys-shared libstring-shared libfile-shared libio-shared\
            liberror-shared libargparse-shared
	@cd $(ZA_SYS_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

sys-static: libsys-static libstring-static libfile-static libio-static\
            liberror-static libargparse-static
	@cd $(ZA_SYS_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-sys-shared:
	@cd $(ZA_SYS_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-sys-static:
	@cd $(ZA_SYS_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
#----------------------------------------------------------#
  # v
  ZA_V          = v
  ZA_V_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_V)/src

v-shared: cenv libvwm-shared libstring-shared libfile-shared libio-shared libargparse-shared\
          libterm-shared zs-shared libv-shared libla-shared
	@cd $(ZA_V_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

v-static: cenv libvwm-static libstring-static libfile-static libio-static libargparse-static\
          libterm-static zs-static libv-static libla-static
	@cd $(ZA_V_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-v-shared:
	@cd $(ZA_V_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-v-static:
	@cd $(ZA_V_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static

#----------------------------------------------------------#
  # env
  ZA_ENV          = env
  ZA_ENV_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_ENV)/src

env-shared: cenv libcstring-shared libargparse-shared
	@cd $(ZA_ENV_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

env-static: cenv libcstring-static libargparse-static
	@cd $(ZA_ENV_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-env-shared:
	@cd $(ZA_ENV_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-env-static:
	@cd $(ZA_ENV_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static

#----------------------------------------------------------#
  # crypt
  ZA_CRYPT             = crypt
  ZA_CRYPT_SRCDIR     := $(Z_SRCAPPDIR)/$(ZA_CRYPT)/src

crypt-shared: libmd5-shared libsha256-shared libsha512-shared\
              libbase64-shared libbcrypt-shared crypt-module-shared
	@cd $(ZA_CRYPT_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

crypt-static: libmd5-static libsha256-static libsha512-static\
              libbase64-static libbcrypt-static
	@cd $(ZA_CRYPT_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-crypt-shared:
	@cd $(ZA_CRYPT_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-crypt-static:
	@cd $(ZA_CRYPT_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static

#----------------------------------------------------------#
  # contain
  ZA_CONTAIN          = contain
  ZA_CONTAIN_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_CONTAIN)/src

contain-shared: cenv libcontain-shared libargparse-shared
	@cd $(ZA_CONTAIN_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

contain-static: cenv libcontain-static libargparse-static
	@cd $(ZA_CONTAIN_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-contain-shared:
	@cd $(ZA_CONTAIN_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-contain-static:
	@cd $(ZA_CONTAIN_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static

#----------------------------------------------------------#
  # man
  ZA_MAN          = man
  ZA_MAN_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_MAN)/src

man:
	@cd $(ZA_MAN_SRCDIR) && $(MAKE) $(MARGS)

clean-man:
	@cd $(ZA_MAN_SRCDIR) && $(MAKE) $(MARGS) clean

#----------------------------------------------------------#
  # packman
  ZA_PACKMAN          = packman
  ZA_PACKMAN_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_PACKMAN)/src

packman:
	@cd $(ZA_PACKMAN_SRCDIR) && $(MAKE) $(MARGS)

clean-packman:
	@cd $(ZA_PACKMAN_SRCDIR) && $(MAKE) $(MARGS) clean

#----------------------------------------------------------#
  # sudoku
  ZA_SUDOKU          = sudoku
  ZA_SUDOKU_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_SUDOKU)/src

sudoku:
	@cd $(ZA_SUDOKU_SRCDIR) && $(MAKE) $(MARGS)

clean-sudoku:
	@cd $(ZA_SUDOKU_SRCDIR) && $(MAKE) $(MARGS) clean

#----------------------------------------------------------#
  # net
  ZA_NET              = net
  ZA_NET_SRCDIR      := $(Z_SRCAPPDIR)/$(ZA_NET)/src

net-shared: cenv libnet-shared
	@cd $(ZA_NET_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

net-static: cenv libnet-static
	@cd $(ZA_NET_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-net-shared:
	@cd $(ZA_NET_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-net-static:
	@cd $(ZA_NET_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static
#------------- ----------------------------------#
# Development (ignore)

clone-libc:
	$(SYSBINDIR)/Packman --install clone_libc
libpam:
	$(SYSBINDIR)/Packman --install pam

bootstrap: clean-libla-shared la-shared clone-libc libpam clean-libauth-shared zsu-shared net-shared

#----------------------------------------------------------#
shared: zero-clean-shared zero-shared
zero-shared:  Env cenv list\
              libstring-shared libustring-shared libcstring-shared\
              libvstring-shared libdir-shared\
              libproc-shared libauth-shared librline-shared\
              libsh-shared libargparse-shared libio-shared\
              libfile-shared libpath-shared libterm-shared\
              libimap-shared libsmap-shared libvideo-shared\
              libjson-shared libmd5-shared libsha256-shared\
              libsha512-shared librandom-shared libcontain-shared\
              liburl-shared libdl-shared\
              libsun-shared libmoon-shared\
              libman-shared\
              zs-shared zsu-shared\
              dir-shared file-shared io-shared term-shared\
              sys-shared path-shared crypt-shared env-shared\
              contain-shared time-shared

static: zero-clean-static zero-static
zero-static:  Env cenv list\
              libstring-static libustring-static libcstring-static\
              libvstring-static libdir-static\
              libproc-static libauth-static librline-static\
              libsh-static libargparse-static libio-static\
              libfile-static libpath-static libterm-static\
              libimap-static libsmap-static libvideo-static\
              libjson-static libmd5-static libsha256-static\
              libsha512-static librandom-static libcontain-static\
              liburl-static libdl-static\
              libsun-static libmoon-static\
              libman-static\
              zs-static\
              dir-static file-static io-static term-static\
              sys-static path-static crypt-static env-static\
              contain-static time-static

clean-shared: zero-clean-shared
zero-clean-shared:\
              clean-cenv clean-list\
              clean-libstring-shared clean-libustring-shared\
              clean-libcstring-shared clean-libvstring-shared\
              clean-libdir-shared\
              clean-libproc-shared clean-libauth-shared\
              clean-librline-shared clean-libsh-shared\
              clean-libargparse-shared clean-libio-shared\
              clean-libfile-shared clean-libpath-shared\
              clean-libterm-shared clean-libimap-shared\
              clean-libsmap-shared clean-libvideo-shared\
              clean-libre-shared clean-libspell-shared\
              clean-libe-shared clean-libi-shared clean-libjson-shared\
              clean-libreadline-shared clean-libsys-shared\
              clean-libvui-shared clean-libvmap-shared\
              clean-liberror-shared clean-libvwm-shared\
              clean-libla-shared clean-libv-shared\
              clean-libos-shared clean-libmd5-shared\
              clean-libsha256-shared clean-libsha512-shared\
              clean-libbase64-shared clean-libbcrypt-shared\
              clean-librandom-shared clean-libcontain-shared\
              clean-liburl-shared clean-libdl-shared\
              clean-libmoon-shared clean-libsun-shared\
              clean-libman-shared\
              clean-zs-shared clean-zsu-shared\
              clean-dir-shared clean-i-shared\
              clean-file-shared clean-io-shared clean-term-shared\
              clean-sys-shared clean-v-shared clean-e-shared\
              clean-path-shared clean-env-shared clean-contain-shared\
              clean-time-shared

clean-static: zero-clean-static
zero-clean-static:\
              clean-cenv clean-list\
              clean-libstring-static clean-libustring-static\
              clean-libcstring-static clean-libvstring-static\
              clean-libdir-static\
              clean-libproc-static clean-libauth-static\
              clean-librline-static clean-libsh-static\
              clean-libargparse-static clean-libio-static\
              clean-libfile-static clean-libpath-static\
              clean-libterm-static clean-libimap-static\
              clean-libsmap-static clean-libvideo-static\
              clean-libre-static clean-libspell-static\
              clean-libe-static clean-libi-static clean-libjson-static\
              clean-libreadline-static clean-libsys-static\
              clean-libvui-static clean-libvmap-static\
              clean-liberror-static clean-libvwm-static\
              clean-libla-static clean-libv-static\
              clean-libos-static clean-libmd5-static\
              clean-libsha256-static clean-libsha512-static\
              clean-libbase64-static clean-libbcrypt-static\
              clean-librandom-static clean-libcontain-static\
              clean-liburl-static clean-libdl-static\
              clean-libmoon-static clean-libsun-static\
              clean-libman-static\
              clean-zs-static clean-zsu-static\
              clean-dir-static clean-i-static\
              clean-file-static clean-io-static clean-term-static\
              clean-sys-static clean-v-static clean-e-static\
              clean-path-static clean-env-static clean-contain-static\
              clean-time-static

all: shared static

#----------- [ MODULES ] -----------------------#
MODULE_FLAGS := $(ZARGS) $(MARGS)
MODULE_FLAGS += SYSMODULEDIR=$(SYSZMODULEDIR)

modules-all:
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) all

modules-shared: libsh-shared libsys-shared libterm-shared\
                libpath-shared libfile-shared libdir-shared\
                libmd5-shared libsha256-shared libsha512-shared\
                librandom-shared libbase64-shared libio-shared\
                libos-shared libstring-shared liburl-shared\
                libdl-shared libman-shared libsun-shared\
                libmoon-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) shared

path-module-shared: libpath-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) path-module-shared
	
file-module-shared: libfile-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) file-module-shared

std-module-shared:
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) std-module-shared

os-module-shared:
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) os-module-shared

sys-module-shared: libsys-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) sys-module-shared

term-module-shared: libterm-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) term-module-shared

dir-module-shared: libdir-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) dir-module-shared

sh-module-shared: libsh-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) sh-module-shared

time-module-shared:
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) time-module-shared

crypt-module-shared: libmd5-shared libsha256-shared libsha512-shared\
                     libbase64-shared libbcrypt-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) crypt-module-shared

rand-module-shared: Env librandom-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) rand-module-shared

io-module-shared: Env libio-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) io-module-shared

url-module-shared: Env liburl-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) url-module-shared

dl-module-shared: Env libdl-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) dl-module-shared

sun-module-shared: Env libsun-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) sun-module-shared

moon-module-shared: Env libmoon-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) moon-module-shared

man-module-shared: Env libman-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) man-module-shared

net-module-shared: Env libnet-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) net-module-shared

sudoku-module-shared: Env libsudoku-shared
	@cd $(Z_SRCMODULEDIR) && make $(MODULE_FLAGS) sudoku-module-shared

#----------- [ LALIBRARY ] -----------------------#
LALIB_ARGS = SYSSCRIPTDIR=$(SYSZSCRIPTDIR)

la-library:
	@cd $(Z_SRCLALIBDIR) && make $(LALIB_ARGS)

#-----------------------------------------------#
# Development and personal stuff (ignore this section)
  ZA_SELF          = self
  ZA_SELF_SRCDIR  := $(Z_SRCAPPDIR)/$(ZA_SELF)/src

self-shared: cenv libstring-shared libdir-shared libargparse-shared\
             libvstring-shared libcstring-shared libe-shared\
             libvui-shared libjson-shared libauth-shared libself-shared
	@cd $(ZA_SELF_SRCDIR) && $(MAKE) $(APP_FLAGS) shared

self-editor-shared: cenv libe-shared
	@cd $(ZA_SELF_SRCDIR) && $(MAKE) DEBUG=1 $(APP_FLAGS) self-editor-shared
	
self-editor-static: cenv libe-static
	@cd $(ZA_SELF_SRCDIR) && $(MAKE) DEBUG=1 $(APP_FLAGS) self-editor-static
	
self-static: cenv libstring-static libdir-static libargparse-static\
             libvstring-static libcstring-static libe-static\
             libvui-static libjson-static libauth-static libself-static
	@cd $(ZA_SELF_SRCDIR) && $(MAKE) $(APP_FLAGS) static

clean-self-shared:
	@cd $(ZA_SELF_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-shared

clean-self-static:
	@cd $(ZA_SELF_SRCDIR) && $(MAKE) $(APP_FLAGS) clean-static

#----------------------------------------------------------#
Env: checkuid makeenv checkenv

checkuid:
	@if [ $(UID) = 0 ]; then echo "You can not run this makefile as the root user"; exit 1; fi

makeenv:
	@$(TEST) -d $(SYSDIR)        || $(MKDIR_P) $(SYSDIR)
	@$(TEST) -d $(SYSLIBDIR)     || $(MKDIR)   $(SYSLIBDIR)
	@$(TEST) -d $(SYSBINDIR)     || $(MKDIR)   $(SYSBINDIR)
	@$(TEST) -d $(SYSINCDIR)     || $(MKDIR)   $(SYSINCDIR)
	@$(TEST) -d $(SYSTMPDIR)     || $(MKDIR)   $(SYSTMPDIR)
	@$(TEST) -d $(SYSDATADIR)    || $(MKDIR)   $(SYSDATADIR)
	@$(TEST) -d $(Z_SYSDIR)      || $(MKDIR)   $(Z_SYSDIR)
	@$(TEST) -d $(Z_SYSBINDIR)   || $(MKDIR)   $(Z_SYSBINDIR)
	@$(TEST) -d $(Z_SYSLIBDIR)   || $(MKDIR)   $(Z_SYSLIBDIR)
	@$(TEST) -d $(Z_SYSINCDIR)   || $(MKDIR)   $(Z_SYSINCDIR)
	@$(TEST) -d $(SYSLADIR)      || $(MKDIR)   $(SYSLADIR)
	@$(TEST) -d $(SYSDSCRIPTDIR) || $(MKDIR)   $(SYSDSCRIPTDIR)
	@$(TEST) -d $(SYSZMODULEDIR) || $(MKDIR)   $(SYSZMODULEDIR)
	@$(TEST) -d $(SYSZSCRIPTDIR) || $(MKDIR)   $(SYSZSCRIPTDIR)
	@$(TEST) -d $(SYSMODULEDIR)  || $(MKDIR)   $(SYSMODULEDIR)
	@$(TEST) -d $(SYSSCRIPTDIR)  || $(MKDIR)   $(SYSSCRIPTDIR)

checkenv:
	@$(TEST) -w $(SYSDIR)        || exit 1
	@$(TEST) -w $(SYSBINDIR)     || exit 1
	@$(TEST) -w $(SYSLIBDIR)     || exit 1
	@$(TEST) -w $(SYSINCDIR)     || exit 1
	@$(TEST) -w $(SYSTMPDIR)     || exit 1
	@$(TEST) -w $(SYSDATADIR)    || exit 1
	@$(TEST) -w $(SYSLADIR)      || exit 1
	@$(TEST) -w $(SYSDSCRIPTDIR) || exit 1
	@$(TEST) -w $(SYSZMODULEDIR) || exit 1
	@$(TEST) -w $(SYSZSCRIPTDIR) || exit 1
	@$(TEST) -w $(SYSMODULEDIR)  || exit 1
	@$(TEST) -w $(SYSSCRIPTDIR)  || exit 1
	@$(TEST) -w $(Z_SYSBINDIR)   || exit 1
	@$(TEST) -w $(Z_SYSLIBDIR)   || exit 1
	@$(TEST) -w $(Z_SYSINCDIR)   || exit 1

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
SUDO := sudo
