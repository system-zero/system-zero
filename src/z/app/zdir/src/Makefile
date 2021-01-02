NAME    := Dir

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
FLAGS += -ldir-$(VERSION) -lpath-$(VERSION) -lfile-$(VERSION) -largparse-$(VERSION) -lio-$(VERSION)

dir: shared static

shared: clean_shared\
    dir-make-shared\
    dir-rm-shared

static: clean_static\
    dir-make-static\
    dir-rm-static

clean_shared:\
    clean_dir_make_shared

clean_static:\
    clean_dir_make_static
#-----------------------------------------------#
D_MAKE = $(NAME).make

FLAGS += -lcstring-$(VERSION)

D_MAKE_STATIC_FLAGS = -lpath-$(VERSION)\
                      -lvstring-$(VERSION) -lstring-$(VERSION)

dir-make-shared:

	$(CC) $(D_MAKE).c $(FLAGS) -o $(SYSBINDIR)/$(D_MAKE)-shared
	@$(LN_S) -vf $(SYSBINDIR)/$(D_MAKE)-shared $(SYSBINDIR)/$(D_MAKE)

dir-make-static:
	$(CC) $(D_MAKE).c $(FLAGS) $(D_MAKE_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(D_MAKE)-static

clean_dir_make_shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(D_MAKE)-shared || $(RM) $(SYSBINDIR)/$(D_MAKE)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(D_MAKE) || $(RM) $(SYSBINDIR)/$(D_MAKE)

clean_dir_make_static:
	@$(TEST) ! -f $(SYSBINDIR)/$(D_MAKE)-static || $(RM) $(SYSBINDIR)/$(D_MAKE)-static
#-----------------------------------------------#
D_RM = $(NAME).rm

D_RM_STATIC_FLAGS = -lpath-$(VERSION) -lcstring-$(VERSION) -lvstring-$(VERSION) -lstring-$(VERSION)

dir-rm-shared:
	$(CC) $(D_RM).c $(FLAGS) -o $(SYSBINDIR)/$(D_RM)-shared
	@$(LN_S) -vf $(SYSBINDIR)/$(D_RM)-shared $(SYSBINDIR)/$(D_RM)

dir-rm-static:
	$(CC) $(D_RM).c $(FLAGS) $(D_RM_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(D_RM)-static

clean_dir_rm_shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(D_RM)-shared || $(RM) $(SYSBINDIR)/$(D_RM)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(D_RM) || $(RM) $(SYSBINDIR)/$(D_RM)

clean_dir_rm_static:
	@$(TEST) ! -f $(SYSBINDIR)/$(D_RM)-static || $(RM) $(SYSBINDIR)/$(D_RM)-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
