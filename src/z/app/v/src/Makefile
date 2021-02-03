NAME    := V

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
FLAGS += -largparse-$(VERSION) -lio-$(VERSION)

v: shared static

shared: clean-shared\
    v-wm-shared

static: clean-static\
    v-wm-static

clean-shared:\
    clean-v-wm-shared

clean-static:\
    clean-v-wm-static
#-----------------------------------------------#
V_WM = $(NAME).wm

FLAGS += -lvwm-$(VERSION)
FLAGS += -lcstring-$(VERSION) -lstring-$(VERSION) -lustring-$(VERSION)
FLAGS += -lterm-$(VERSION) -lfile-$(VERSION)

V_WM_STATIC_FLAGS  = -ldir-$(VERSION) -lreadline-$(VERSION) -lvideo-$(VERSION)
V_WM_STATIC_FLAGS += -lvstring-$(VERSION) -lpath-$(VERSION)

v-wm-shared:
	$(CC) $(V_WM).c $(FLAGS) -o $(SYSBINDIR)/$(V_WM)-shared
	@$(LN_S) -vf $(SYSBINDIR)/$(V_WM)-shared $(SYSBINDIR)/$(V_WM)

v-wm-static:
	$(CC) $(V_WM).c $(FLAGS) $(V_WM_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(V_WM)-static

clean-v-wm-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(V_WM)-shared || $(RM) $(SYSBINDIR)/$(V_WM)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(V_WM) || $(RM) $(SYSBINDIR)/$(V_WM)

clean-v-wm-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(V_WM)-static || $(RM) $(SYSBINDIR)/$(V_WM)-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
