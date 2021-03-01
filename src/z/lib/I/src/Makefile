THIS_LIBNAME = i

NS      :=
API     := 0
REV     := 0
VERSION := $(API).$(REV)

# macOS doesn't have realpath by default (available through coreutils)
# THISDIR := $(shell realpath .)
THISDIR := $(shell (cd . && pwd))

THIS_LIBVERS  :=  $(VERSION)

THIS_HEADER     = $(THIS_LIBNAME).h
THIS_LIBSRC     = lib$(THIS_LIBNAME).c
LIBNAME         = $(THIS_LIBNAME)-$(THIS_LIBVERS)
THIS_LIB        = lib$(LIBNAME)

CC            := gcc
CC_STD        := -std=c11

SYSKERNEL     := $(shell uname -s)
SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

DATADIR    =  $(THISDIR)/data

SYSDIR    := $(THISDIR)/../../sys
SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
SYSBINDIR  = $(SYSDIR)/bin
SYSINCDIR  = $(SYSDIR)/include/$(NS)
SYSDATADIR = $(SYSDIR)/data
SYSTMPDIR  = $(SYSDIR)/tmp

THIS_SHAREDLIB := $(SYSLIBDIR)/$(THIS_LIB).so
THIS_STATICLIB := $(SYSLIBDIR)/$(THIS_LIB).a

BASE_FLAGS  := -g -O2 -march=native -fdollars-in-identifiers

DEBUG_FLAGS := -Wextra -Wshadow -Wall -Wunused-result -Wunused-function -Wunused-macros -Wno-override-init

# -Wno-override-init: is for designated initializers and both compilers complain.
# it is included in extra in gcc, but clang needs this unconditionally
ifneq ($(DEBUG), 1)
  ifeq (-$(CC), -clang)
    BASE_FLAGS  +=  -Wno-override-init
  endif
endif

ifneq (-$(CC), -tcc)
  DEBUG_FLAGS += -Werror
  # while compiling tinyexpr tcc warns with: assigment discards qualifiers
  # from pointer target type
  # -Wno-error=discarded-qualifiers doesn't have effect
endif

# DEBUG_FLAGS += -fsanitize=undefined
# if turn on -Wpedantic gcc gives: ISO C forbids braced-groups within expressions
# (this is for statement expressions)

SHARED_CFLAGS := -shared -fPIC
STATIC_CFLAGS := -static

FLAGS         := $(BASE_FLAGS)

CHECK_PTR_BOUNDS_FLAGS := -mmpx -fcheck-pointer-bounds

CHECK_PTR_BOUNDS := 0

ifneq ($(CHECK_PTR_BOUNDS), 0)
  FLAGS += $(CHECK_PTR_BOUNDS_FLAGS)
endif

ifeq (-$(CC), -gcc)
  FLAGS += -Wno-implicit-fallthrough
  # for gcc < 7 version
  #  FLAGS += -Wimplicit-fallthrough=0
endif

DEBUG := 0
ifneq ($(DEBUG), 0)
  FLAGS += $(DEBUG_FLAGS)
endif

LIBFLAGS := -I$(SYSINCDIR) -L$(SYSLIBDIR) $(FLAGS)
LIBFLAGS += -lstring-$(VERSION) -lcstring-$(VERSION) -lvstring-$(VERSION)
LIBFLAGS += -lustring-$(VERSION) -lpath-$(VERSION) -lfile-$(VERSION)
LIBFLAGS += -lerror-$(VERSION) -lvmap-$(VERSION)

all: shared static

prereq: Env

shared: prereq header $(THIS_SHAREDLIB)
$(THIS_SHAREDLIB):
	$(CC) -x c $(THIS_LIBSRC) $(LIBOPTS) $(LIBFLAGS) $(SHARED_CFLAGS) -o $(THIS_SHAREDLIB)
	@$(LN_S) -vf $(THIS_SHAREDLIB) $(SYSLIBDIR)/lib$(THIS_LIBNAME).so

static: prereq header $(THIS_STATICLIB)
$(THIS_STATICLIB):
	$(CC) -x c $(THIS_LIBSRC) $(LIBOPTS) $(LIBFLAGS) $(STATIC_CFLAGS) -c -o $(THIS_LIB).o
	@$(AR) $(THIS_STATICLIB) $(THIS_LIB).o
	@$(RM) $(THIS_LIB).o

header: clean-header $(SYSINCDIR)/$(THIS_HEADER)

$(SYSINCDIR)/$(THIS_HEADER):
	@$(CP) $(THIS_HEADER) $(SYSINCDIR)

clean: clean-shared clean-static clean-header

clean-shared: clean-header
	@$(TEST) ! -f $(THIS_SHAREDLIB) || $(RM) $(THIS_SHAREDLIB)
	@$(TEST) ! -L $(SYSLIBDIR)/lib$(THIS_LIBNAME).so || $(RM) $(SYSLIBDIR)/lib$(THIS_LIBNAME).so

clean-static: clean-header
	@$(TEST) ! -f $(THIS_STATICLIB) || $(RM)  $(THIS_STATICLIB)

clean-header:
	@$(TEST) ! -f $(SYSINCDIR)/$(THIS_HEADER) || $(RM) $(SYSINCDIR)/$(THIS_HEADER)

AR = ar rs
TEST = test
STAT = stat
GREP = grep
RM = rm -f
RM_R = rm -r -f
RMDIR = rmdir
MKDIR = mkdir
MKDIR_P = $(MKDIR) -p
CP = cp
CP_R = $(CP) -r
LN_S = ln -s
CHMOD = chmod
CHMOD_X = $(CHMOD) 0755
PRINTF = printf
INSTALL = install
MAKE = make
VALGRIND = valgrind
VALGRIND_ARGS = --leak-check=full --show-leak-kinds=all -v --track-origins=yes
GDB = gdb
GDB_ARGS = --quiet -ex "set logging file /tmp/gdb.txt" -ex "set logging on" -ex run --args
#----------------------------------------------------------#

Env: makeenv checkenv
makeenv:
	@$(TEST) -d $(SYSDIR)     || $(MKDIR_P) $(SYSDIR)
	@$(TEST) -d $(SYSLIBDIR)  || $(MKDIR)   $(SYSLIBDIR)
	@$(TEST) -d $(SYSINCDIR)  || $(MKDIR)   $(SYSINCDIR)
	@$(TEST) -d $(SYSBINDIR)  || $(MKDIR)   $(SYSBINDIR)

checkenv:
	@$(TEST) -w $(SYSDIR)     || exit 1
	@$(TEST) -w $(SYSLIBDIR)  || exit 1
	@$(TEST) -w $(SYSINCDIR)  || exit 1
	@$(TEST) -w $(SYSBINDIR)  || exit 1
