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

Z_SHELL         := zs
Z_SHELL_DATADIR := $(SYSDATADIR)/$(Z_SHELL)

FLAGS += -DVERSION=$(VERSION) -DVERSION_STRING='"$(VERSION)"'
FLAGS += -ldir-$(VERSION) -lpath-$(VERSION) -lfile-$(VERSION)
FLAGS += -largparse-$(VERSION) -lio-$(VERSION)
FLAGS += -lerror-$(VERSION) -lterm-$(VERSION)

dir: shared static

shared: clean-shared\
    dir-make-shared\
    dir-remove-shared\
    install-data

static: clean-static\
    dir-make-static\
    dir-remove-static\
    install-data

clean-shared:\
    clean-dir-make-shared\
    clean-dir-remove-shared

clean-static:\
    clean-dir-make-static\
    clean-dir-remove-static

install-data:
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(D_MAKE)   || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(D_MAKE)
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(D_REMOVE) || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(D_REMOVE)
	@$(CP) $(D_MAKE)/args   $(Z_SHELL_DATADIR)/commands/$(D_MAKE)
	@$(CP) $(D_REMOVE)/args $(Z_SHELL_DATADIR)/commands/$(D_REMOVE)
#-----------------------------------------------#
D_MAKE = $(NAME).make

FLAGS += -lcstring-$(VERSION)

D_MAKE_STATIC_FLAGS = -lpath-$(VERSION)\
                      -lvstring-$(VERSION) -lstring-$(VERSION) -lustring-$(VERSION)\
                      -los-$(VERSION) -lrandom-$(VERSION)

dir-make-shared:
	$(CC) $(D_MAKE).c $(FLAGS) -o $(SYSBINDIR)/$(D_MAKE)-shared
	@$(LN_S) -vf $(D_MAKE)-shared $(SYSBINDIR)/$(D_MAKE)

dir-make-static:
	$(CC) $(D_MAKE).c $(FLAGS) $(D_MAKE_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(D_MAKE)-static

clean-dir-make-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(D_MAKE)-shared || $(RM) $(SYSBINDIR)/$(D_MAKE)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(D_MAKE) || $(RM) $(SYSBINDIR)/$(D_MAKE)

clean-dir-make-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(D_MAKE)-static || $(RM) $(SYSBINDIR)/$(D_MAKE)-static
#-----------------------------------------------#
D_REMOVE = $(NAME).remove

D_REMOVE_STATIC_FLAGS = -lpath-$(VERSION) -lcstring-$(VERSION) -lvstring-$(VERSION)\
                    -lstring-$(VERSION) -lustring-$(VERSION)\
                    -los-$(VERSION)

dir-remove-shared:
	$(CC) $(D_REMOVE).c $(FLAGS) -o $(SYSBINDIR)/$(D_REMOVE)-shared
	@$(LN_S) -vf $(D_REMOVE)-shared $(SYSBINDIR)/$(D_REMOVE)

dir-remove-static:
	$(CC) $(D_REMOVE).c $(FLAGS) $(D_REMOVE_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(D_REMOVE)-static

clean-dir-remove-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(D_REMOVE)-shared || $(RM) $(SYSBINDIR)/$(D_REMOVE)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(D_REMOVE) || $(RM) $(SYSBINDIR)/$(D_REMOVE)

clean-dir-remove-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(D_REMOVE)-static || $(RM) $(SYSBINDIR)/$(D_REMOVE)-static
#-----------------------------------------------#

TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
MKDIR = mkdir
MKDIR_P = $(MKDIR) -p
CP = cp
CP_R = $(CP) -r
