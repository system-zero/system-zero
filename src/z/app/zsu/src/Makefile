NAME = zsu

NS      :=
API     := 0
REV     := 0
VERSION := $(API).$(REV)

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

FLAGS += -lauth-$(VERSION) -lcstring-$(VERSION) -lstring-$(VERSION) -lustring-$(VERSION) -lproc-$(VERSION) -lpam
FLAGS += -lio-$(VERSION) -lterm-$(VERSION) -lerror-$(VERSION)

SHARED_FLAGS = $(FLAGS)
STATIC_FLAGS = $(FLAGS) $(STATIC_CFLAGS) -ldl

VSU_DIR := $(SYSTMPDIR)/zsu

zsu: zsu-shared zsu-static

zsu-shared:
	$(CC) -DVSU_DIR='"$(VSU_DIR)"' $(NAME).c $(SHARED_FLAGS) -o $(NAME)-shared
	@$(LN_S) -f $(SYSBINDIR)/$(NAME)-shared $(SYSBINDIR)/$(NAME)

install-shared: zsu-shared
	@$(INSTALL) -v -g root -o root -m 4755 $(NAME)-shared $(SYSBINDIR)/$(NAME)-shared

# cannot compile (a statically linked libpam is required)
zsu-static:
	$(CC) -DVSU_DIR='"$(VSU_DIR)"' $(NAME).c $(STATIC_FLAGS) -o $(NAME)-static

install-static: zsu-static
	@$(INSTALL) -v -g root -o root -m 4755 $(NAME)-static $(SYSBINDIR)/$(NAME)-static

#----------------------------------------------------------#
clean: clean-shared clean-static

clean-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(NAME)-shared || $(RM) $(SYSBINDIR)/$(NAME)-shared
	@$(TEST) ! -L $(SYSBINDIR)/$(NAME)        || $(RM) $(SYSBINDIR)/$(NAME)

clean-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(NAME)-static || $(RM) $(SYSBINDIR)/$(NAME)-static

#----------------------------------------------------------#
TEST = test
RM = rm
LN = ln
LN_S = $(LN) -s
INSTALL=install
