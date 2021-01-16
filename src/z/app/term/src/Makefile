NAME    := Term

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
FLAGS += -lterm-$(VERSION) -largparse-$(VERSION) -lio-$(VERSION)
FLAGS += -lstring-$(VERSION) -lcstring-$(VERSION) -lustring-$(VERSION)

file: shared static

shared: clean-shared\
    term-sane-shared

static: clean-static\
    term-sane-static

clean-shared:\
    clean-term-sane-shared

clean-static:\
    clean-term-sane-static
#-----------------------------------------------#
T_SANE = $(NAME).sane

T_SANE_STATIC_FLAGS :=
# -largparse-$(VERSION)
# -ldir-$(VERSION) -lpath-$(VERSION)\
# -lvstring-$(VERSION) -lstring-$(VERSION)\

term-sane-shared:
	$(CC) $(T_SANE).c $(FLAGS) -o $(SYSBINDIR)/$(T_SANE)-shared
	@$(LN_S) -vf $(SYSBINDIR)/$(T_SANE)-shared $(SYSBINDIR)/$(T_SANE)
	@$(LN_S) -vf $(SYSBINDIR)/$(T_SANE)-shared $(SYSBINDIR)/Term.raw

term-sane-static:
	$(CC) $(T_SANE).c $(FLAGS) $(T_SANE_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(T_SANE)-static
	@$(LN_S) -vf $(SYSBINDIR)/$(T_SANE)-static $(SYSBINDIR)/Term.raw-static

clean-term-sane-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(T_SANE)-shared || $(RM) $(SYSBINDIR)/$(T_SANE)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(T_SANE) || $(RM) $(SYSBINDIR)/$(T_SANE)
	@$(TEST) ! -L $(SYSBINDIR)/Term.raw  || $(RM) $(SYSBINDIR)/Term.raw

clean-term-sane-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(T_SANE)-static || $(RM) $(SYSBINDIR)/$(T_SANE)-static
	@$(TEST) ! -L $(SYSBINDIR)/Term.raw-static  || $(RM) $(SYSBINDIR)/Term.raw-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
