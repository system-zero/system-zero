NAME    := File

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

FLAGS += -DVERSION=$(VERSION) -DVERSION_STRING='"$(VERSION)"'
FLAGS += -lfile-$(VERSION) -largparse-$(VERSION) -lio-$(VERSION)

file: shared static

shared: clean-shared\
    file-size-shared

static: clean-static\
    file-size-static

clean-shared:\
    clean-file-size-shared

clean-static:\
    clean-file-size-static
#-----------------------------------------------#
F_SIZE = $(NAME).size

F_SIZE_STATIC_FLAGS = -ldir-$(VERSION) -lpath-$(VERSION)\
                      -lvstring-$(VERSION) -lstring-$(VERSION)\
                      -lcstring-$(VERSION) -largparse-$(VERSION)\
                      -lustring-$(VERSION)

file-size-shared:
	$(CC) $(F_SIZE).c $(FLAGS) -o $(SYSBINDIR)/$(F_SIZE)-shared
	@$(LN_S) -vf $(SYSBINDIR)/$(F_SIZE)-shared $(SYSBINDIR)/$(F_SIZE)

file-size-static:
	$(CC) $(F_SIZE).c $(FLAGS) $(F_SIZE_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(F_SIZE)-static

clean-file-size-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_SIZE)-shared || $(RM) $(SYSBINDIR)/$(F_SIZE)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(F_SIZE) || $(RM) $(SYSBINDIR)/$(F_SIZE)

clean-file-size-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_SIZE)-static || $(RM) $(SYSBINDIR)/$(F_SIZE)-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
