NAME    := Contain

NS      :=
API     := 0
REV     := 0
VERSION := $(API).$(REV)

# macOS doesn't have realpath by default (available through coreutils)
# THISDIR := $(shell realpath .)
THISDIR := $(shell (cd . && pwd))

CC            := gcc
CC_STD        := -std=c11

SYSKERNEL     := $(shell uname -s)
SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

SYSDIR    := $(THISDIR)/../sys
SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
SYSBINDIR  = $(SYSDIR)/bin
SYSINCDIR  = $(SYSDIR)/include/$(NS)
SYSTMPDIR  = $(SYSDIR)/tmp

#BASE_FLAGS  := -g -O2 -march=native -fvisibility=hidden -fdollars-in-identifiers
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

STATIC_CFLAGS := -static

FLAGS         := -I$(SYSINCDIR) -L$(SYSLIBDIR) $(BASE_FLAGS)

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

FLAGS += -DSYSDIR='"$(SYSDIR)"' -DSRCDIR='"$(SRCDIR)"'
FLAGS += -DVERSION=$(VERSION) -DVERSION_STRING='"$(VERSION)"'
FLAGS += -lcontain-$(VERSION)
FLAGS += -lstring-$(VERSION) -los-$(VERSION)
FLAGS += -largparse-$(VERSION) -lio-$(VERSION)
FLAGS += -lerror-$(VERSION)

contain: shared static

shared: clean-shared\
    contain-new-shared

static: clean-static\
    contain-new-static

clean-shared:\
    clean-contain-new-shared

clean-static:\
    clean-contain-new-static
#-----------------------------------------------#
C_NEW = $(NAME).new

C_NEW_STATIC_FLAGS  = -lcstring-$(VERSION) -lstring-$(VERSION) -los-$(VERSION)
C_NEW_STATIC_FLAGS += -lustring-$(VERSION) -ldir-$(VERSION) -lfile-$(VERSION)
C_NEW_STATIC_FLAGS += -lterm-$(VERSION) -lpath-$(VERSION) -lvstring-$(VERSION)
C_NEW_STATIC_FLAGS += -lrandom-$(VERSION)

contain-new-shared:
	$(CC) $(C_NEW).c $(FLAGS) -o $(SYSBINDIR)/$(C_NEW)-shared
	@$(LN_S) -vf $(C_NEW)-shared $(SYSBINDIR)/$(C_NEW)

contain-new-static:
	$(CC) $(C_NEW).c $(FLAGS) $(C_NEW_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(C_NEW)-static

clean-contain-new-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(C_NEW)-shared || $(RM) $(SYSBINDIR)/$(C_NEW)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(C_NEW) || $(RM) $(SYSBINDIR)/$(C_NEW)

clean-contain-new-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(C_NEW)-static || $(RM) $(SYSBINDIR)/$(C_NEW)-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
