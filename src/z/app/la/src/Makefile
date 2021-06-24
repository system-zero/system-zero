NAME    := La

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
FLAGS += -lla-$(VERSION) -lio-$(VERSION) -largparse-$(VERSION)
FLAGS += -lfile-$(VERSION) -lstring-$(VERSION) -lvstring-$(VERSION)
FLAGS += -lterm-$(VERSION) -lcstring-$(VERSION)

la: shared static

shared: clean-shared\
    la-shared

static: clean-static\
    la-static

clean-shared:\
    clean-la-shared

clean-static:\
    clean-la-static
#-----------------------------------------------#
LA = $(NAME)

LA_STATIC_FLAGS  = -lustring-$(VERSION) -ldir-$(VERSION)
LA_STATIC_FLAGS += -lpath-$(VERSION) -lvmap-$(VERSION) -limap-$(VERSION)
LA_STATIC_FLAGS += -lerror-$(VERSION)

PATH_MODULE := 1
ifeq ($(PATH_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_PATH_MODULE
  LA_STATIC_FLAGS += -lcstring-$(VERSION) -lpath-$(VERSION)
endif

la-shared:
	$(CC) $(LA).c $(FLAGS) -ldl -o $(SYSBINDIR)/$(LA)-shared
	@$(LN_S) -vf $(SYSBINDIR)/$(LA)-shared $(SYSBINDIR)/$(LA)

la-static:
	$(CC) $(LA).c $(FLAGS) $(LA_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(LA)-static

clean-la-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(LA)-shared || $(RM) $(SYSBINDIR)/$(LA)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(LA)        || $(RM) $(SYSBINDIR)/$(LA)

clean-la-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(LA)-static || $(RM) $(SYSBINDIR)/$(LA)-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
