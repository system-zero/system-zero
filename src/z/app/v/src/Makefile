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
FLAGS += -largparse-$(VERSION) -lio-$(VERSION) -lvui-$(VERSION)

v: shared static

shared: clean-shared\
    v-wm-shared\
    v-util-shared

static: clean-static\
    v-wm-static\
    v-util-static

clean-shared:\
    clean-v-wm-shared\
    clean-v-util-shared

clean-static:\
    clean-v-wm-static\
    clean-v-util-static
#-----------------------------------------------#
V = $(NAME)
V_FLAGS = $(FLAGS)
V_FLAGS += -lv-$(VERSION) -lutil
V_FLAGS += -largparse-$(VERSION) -lla-$(VERSION) -lpath-$(VERSION)
V_FLAGS += -ldir-$(VERSION) -lsys-$(VERSION) -lfile-$(VERSION)
V_FLAGS += -lterm-$(VERSION) -lerror-$(VERSION) -lstring-$(VERSION)
V_FLAGS += -lvstring-$(VERSION) -lreadline-$(VERSION) -lvwm-$(VERSION)
V_FLAGS += -lcstring-$(VERSION)

V_STATIC_FLAGS  = -lio-$(VERSION) -lustring-$(VERSION) -lreadline-$(VERSION)
V_STATIC_FLAGS += -lvideo-$(VERSION) -lvui-$(VERSION) -lvmap-$(VERSION)
V_STATIC_FLAGS += -limap-$(VERSION) -lsmap-$(VERSION)

v-util-shared:
	$(CC) $(V).c $(V_FLAGS) -o $(SYSBINDIR)/$(V)-shared
	@$(LN_S) -vf $(SYSBINDIR)/$(V)-shared $(SYSBINDIR)/$(V)

v-util-static:
	$(CC) $(V).c $(V_FLAGS) $(V_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(V)-static

clean-v-util-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(V)-shared || $(RM) $(SYSBINDIR)/$(V)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(V) || $(RM) $(SYSBINDIR)/$(V)

clean-v-util-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(V)-static || $(RM) $(SYSBINDIR)/$(V)-static

#-----------------------------------------------#
V_WM = $(NAME).wm

V_WM_FLAGS := $(FLAGS)
V_WM_FLAGS += -lvwm-$(VERSION)
V_WM_FLAGS += -lcstring-$(VERSION) -lstring-$(VERSION) -lustring-$(VERSION)
V_WM_FLAGS += -lterm-$(VERSION) -lfile-$(VERSION) -lsmap-$(VERSION)

V_WM_STATIC_FLAGS  = -ldir-$(VERSION) -lreadline-$(VERSION) -lvideo-$(VERSION)
V_WM_STATIC_FLAGS += -lvstring-$(VERSION) -lpath-$(VERSION)
V_WM_STATIC_FLAGS += -lsys-$(VERSION) -lerror-$(VERSION) -lsmap-$(VERSION)
V_WM_STATIC_FLAGS += -lvui-$(VERSION)
v-wm-shared:
	$(CC) $(V_WM).c $(V_WM_FLAGS) -o $(SYSBINDIR)/$(V_WM)-shared
	@$(LN_S) -vf $(SYSBINDIR)/$(V_WM)-shared $(SYSBINDIR)/$(V_WM)

v-wm-static:
	$(CC) $(V_WM).c $(V_WM_FLAGS) $(V_WM_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(V_WM)-static

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
