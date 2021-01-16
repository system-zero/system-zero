NAME = zs

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

FLAGS += -ldir-$(VERSION) -lvstring-$(VERSION) -lcstring-$(VERSION)
FLAGS += -lsh-$(VERSION) -lproc-$(VERSION) -lrline-$(VERSION)
FLAGS += -lio-$(VERSION) -lstring-$(VERSION)

Z_ZS_STATIC_FLAGS := -lpath-$(VERSION) -lustring-$(VERSION)

zs: zs-shared zs-static

zs-shared:
	$(CC) $(NAME).c -DTMPDIR='"$(SYSTMPDIR)"' $(FLAGS) -o $(SYSBINDIR)/$(NAME)-shared
	@$(LN_S) -f $(SYSBINDIR)/$(NAME)-shared $(SYSBINDIR)/$(NAME)

zs-static:
	$(CC) $(NAME).c -DTMPDIR='"$(SYSTMPDIR)"' $(FLAGS) $(Z_ZS_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(NAME)-static

clean: clean-shared clean-static

clean-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(NAME)-shared || $(RM) $(SYSBINDIR)/$(NAME)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(NAME)        || $(RM) $(SYSBINDIR)/$(NAME)

clean-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(NAME)-static || $(RM) $(SYSBINDIR)/$(NAME)-static

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
