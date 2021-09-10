NAME    := env

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
FLAGS += -largparse-$(VERSION) -lcstring-$(VERSION) -lio-$(VERSION)
STATIC_FLAGS  = $(FLAGS) -lstring-$(VERSION) -lustring-$(VERSION)
STATIC_FLAGS += -lerror-$(VERSION)

env: shared static

shared: clean-shared env-shared

static: clean-static env-static

clean-shared:clean-env-shared

clean-static: clean-env-static

#-----------------------------------------------#
ENV = $(NAME)

env-shared:
	$(CC) $(ENV).c $(FLAGS) -o $(SYSBINDIR)/$(ENV)-shared
	@$(LN_S) -vf $(ENV)-shared $(SYSBINDIR)/$(ENV)

env-static:
	$(CC) $(ENV).c $(STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(ENV)-static

clean-env-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(ENV)-shared || $(RM) $(SYSBINDIR)/$(ENV)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(ENV) || $(RM) $(SYSBINDIR)/$(ENV)

clean-env-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(ENV)-static || $(RM) $(SYSBINDIR)/$(ENV)-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
