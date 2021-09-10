NAME    := IO

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
FLAGS += -lio-$(VERSION) -largparse-$(VERSION) -lstring-$(VERSION) -lcstring-$(VERSION)

io: shared static

shared: clean-shared\
    io-print-shared

static: clean-static\
    io-print-static

clean-shared:\
    clean-io-print-shared

clean-static:\
    clean-io-print-static
#-----------------------------------------------#
I_PRINT = $(NAME).print

I_PRINT_STATIC_FLAGS := -lcstring-$(VERSION) -lustring-$(VERSION) -lerror-$(VERSION)

io-print-shared:
	$(CC) $(I_PRINT).c $(FLAGS) -o $(SYSBINDIR)/$(I_PRINT)-shared
	@$(LN_S) -vf $(I_PRINT)-shared $(SYSBINDIR)/$(I_PRINT)
	@$(LN_S) -vf $(I_PRINT)-shared $(SYSBINDIR)/$(I_PRINT)ln
	@$(LN_S) -vf $(I_PRINT)-shared $(SYSBINDIR)/$(I_PRINT)ln_raw
	@$(LN_S) -vf $(I_PRINT)-shared $(SYSBINDIR)/$(I_PRINT)_raw

io-print-static:
	$(CC) $(I_PRINT).c $(FLAGS) $(I_PRINT_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(I_PRINT)-static
	@$(LN_S) -vf $(I_PRINT)-static $(SYSBINDIR)/$(I_PRINT)ln-static
	@$(LN_S) -vf $(I_PRINT)-static $(SYSBINDIR)/$(I_PRINT)_raw-static
	@$(LN_S) -vf $(I_PRINT)-static $(SYSBINDIR)/$(I_PRINT)ln_raw-static

clean-io-print-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(I_PRINT)-shared || $(RM) $(SYSBINDIR)/$(I_PRINT)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(I_PRINT)        || $(RM) $(SYSBINDIR)/$(I_PRINT)
	@$(TEST) ! -L $(SYSBINDIR)/$(I_PRINT)_raw    || $(RM) $(SYSBINDIR)/$(I_PRINT)_raw
	@$(TEST) ! -L $(SYSBINDIR)/$(I_PRINT)ln      || $(RM) $(SYSBINDIR)/$(I_PRINT)ln
	@$(TEST) ! -L $(SYSBINDIR)/$(I_PRINT)ln_raw  || $(RM) $(SYSBINDIR)/$(I_PRINT)ln_raw

clean-io-print-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(I_PRINT)-static       || $(RM) $(SYSBINDIR)/$(I_PRINT)-static
	@$(TEST) ! -L $(SYSBINDIR)/$(I_PRINT)ln-static     || $(RM) $(SYSBINDIR)/$(I_PRINT)ln-static
	@$(TEST) ! -L $(SYSBINDIR)/$(I_PRINT)_raw-static   || $(RM) $(SYSBINDIR)/$(I_PRINT)_raw-static
	@$(TEST) ! -L $(SYSBINDIR)/$(I_PRINT)ln_raw-static || $(RM) $(SYSBINDIR)/$(I_PRINT)ln_raw-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
