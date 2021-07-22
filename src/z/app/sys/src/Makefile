NAME    := Sys

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
FLAGS += -lio-$(VERSION) -largparse-$(VERSION) -lstring-$(VERSION) -lsys-$(VERSION)
FLAGS += -lfile-$(VERSION) -lsmap-$(VERSION) -lerror-$(VERSION)

sys: shared static

shared: clean-shared\
    sys-chroot-shared

static: clean-static\
    sys-chroot-static

clean-shared:\
    clean-sys-chroot-shared

clean-static:\
    clean-sys-chroot-static
#-----------------------------------------------#
SYS_CHROOT = $(NAME).chroot

SYS_CHROOT_STATIC_FLAGS := -lcstring-$(VERSION) -lustring-$(VERSION)
SYS_CHROOT_STATIC_FLAGS += -ldir-$(VERSION) -lvstring-$(VERSION) -lpath-$(VERSION)\
                           -los-$(VERSION)

sys-chroot-shared:
	$(CC) $(SYS_CHROOT).c $(FLAGS) -o $(SYSBINDIR)/$(SYS_CHROOT)-shared
	@$(LN_S) -vf $(SYSBINDIR)/$(SYS_CHROOT)-shared $(SYSBINDIR)/$(SYS_CHROOT)

sys-chroot-static:
	$(CC) $(SYS_CHROOT).c $(FLAGS) $(SYS_CHROOT_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(SYS_CHROOT)-static

clean-sys-chroot-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(SYS_CHROOT)-shared || $(RM) $(SYSBINDIR)/$(SYS_CHROOT)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(SYS_CHROOT)        || $(RM) $(SYSBINDIR)/$(SYS_CHROOT)

clean-sys-chroot-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(SYS_CHROOT)-static       || $(RM) $(SYSBINDIR)/$(SYS_CHROOT)-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
