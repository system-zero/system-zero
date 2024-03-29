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
FLAGS += -DDATADIR='"$(SYSDATADIR)"'
FLAGS += -lla-$(VERSION) -lio-$(VERSION) -largparse-$(VERSION) -lvmap-$(VERSION)
FLAGS += -lfile-$(VERSION) -lstring-$(VERSION) -lvstring-$(VERSION) -lustring-$(VERSION)
FLAGS += -lterm-$(VERSION) -lcstring-$(VERSION) -lrline-$(VERSION)
FLAGS += -lrandom-$(VERSION) -ldir-$(VERSION) -lpath-$(VERSION)

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
CSTRING_DECLARED = 0
STRING_DECLARED = 0
#-----------------------------------------------#
LA = $(NAME)

LA_STATIC_FLAGS  = -DSTATIC -lustring-$(VERSION)
LA_STATIC_FLAGS += -lvmap-$(VERSION) -limap-$(VERSION)
LA_STATIC_FLAGS += -lerror-$(VERSION) -los-$(VERSION)

PATH_MODULE := 1
ifeq ($(PATH_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_PATH_MODULE
  CSTRING_DECLARED = 1
endif

FILE_MODULE := 1
ifeq ($(FILE_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_FILE_MODULE
endif

# Enable std module by default
ifeq ($(STRING_DECLARED), 0)
  LA_STATIC_FLAGS += -lstring-$(VERSION)
  STRING_DECLARED = 1
endif
ifeq ($(CSTRING_DECLARED), 0)
  LA_STATIC_FLAGS += -lcstring-$(VERSION)
  CSTRING_DECLARED = 1
endif

TERM_MODULE := 1
ifeq ($(TERM_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_TERM_MODULE
  LA_STATIC_FLAGS += -lterm-$(VERSION)
endif

DIR_MODULE := 1
ifeq ($(DIR_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_DIR_MODULE
  LA_STATIC_FLAGS += -ldir-$(VERSION)
  ifeq ($(STRING_DECLARED), 0)
    LA_STATIC_FLAGS += -lstring-$(VERSION)
    STRING_DECLARED = 1
  endif
endif

SH_MODULE := 1
ifeq ($(SH_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_SH_MODULE
  LA_STATIC_FLAGS += -lsh-$(VERSION) -lproc-$(VERSION)
  ifeq ($(STRING_DECLARED), 0)
    LA_STATIC_FLAGS += -lstring-$(VERSION)
    STRING_DECLARED = 1
  endif
endif

OS_MODULE := 1
ifeq ($(OS_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_OS_MODULE
  LA_STATIC_FLAGS += -los-$(VERSION)
  ifeq ($(STRING_DECLARED), 0)
    LA_STATIC_FLAGS += -lstring-$(VERSION)
    STRING_DECLARED = 1
  endif
endif

SYS_MODULE := 1
ifeq ($(SYS_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_SYS_MODULE
  LA_STATIC_FLAGS += -lsys-$(VERSION) -lsmap-$(VERSION)
  ifeq ($(STRING_DECLARED), 0)
    LA_STATIC_FLAGS += -lstring-$(VERSION)
    STRING_DECLARED = 1
  endif
endif

TIME_MODULE := 1
ifeq ($(TIME_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_TIME_MODULE
  ifeq ($(STRING_DECLARED), 0)
    LA_STATIC_FLAGS += -lstring-$(VERSION)
    STRING_DECLARED = 1
  endif
endif

CRYPT_MODULE := 1
ifeq ($(CRYPT_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_CRYPT_MODULE
  LA_STATIC_FLAGS += -lmd5-$(VERSION) -lsha256-$(VERSION) -lsha512-$(VERSION)
  LA_STATIC_FLAGS += -lbase64-$(VERSION) -lbcrypt-$(VERSION)
  ifeq ($(STRING_DECLARED), 0)
    LA_STATIC_FLAGS += -lstring-$(VERSION)
    STRING_DECLARED = 1
  endif
endif

RAND_MODULE := 1
ifeq ($(RAND_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_RAND_MODULE
  LA_STATIC_FLAGS += -lrandom-$(VERSION)
endif

IO_MODULE := 1
ifeq ($(IO_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_IO_MODULE
  LA_STATIC_FLAGS += -lio-$(VERSION)
endif

URL_MODULE := 1
ifeq ($(URL_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_URL_MODULE
  LA_STATIC_FLAGS += -lurl-$(VERSION)
endif

DL_MODULE := 1
ifeq ($(DL_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_DL_MODULE
  LA_STATIC_FLAGS += -ldl-$(VERSION)
endif

MOON_SUN_MODULES := 1
ifeq ($(MOON_SUN_MODULES), 1)
  LA_STATIC_FLAGS += -DREQUIRE_MOON_SUN_MODULES
  LA_STATIC_FLAGS += -lsun-$(VERSION) -lmoon-$(VERSION) -lm
endif

MAN_MODULE := 1
ifeq ($(MAN_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_MAN_MODULE
  LA_STATIC_FLAGS += -lman-$(VERSION)
endif

NET_MODULE := 0
ifeq ($(NET_MODULE), 1)
  LA_STATIC_FLAGS += -DREQUIRE_NET_MODULE
  LA_STATIC_FLAGS += -lnet-$(VERSION) -lssl -lcrypto -lresolv -lpthread
endif

la-shared:$(SYSBINDIR)/$(LA)-shared

$(SYSBINDIR)/$(LA)-shared:
	$(CC) $(LA).c $(FLAGS) -ldl -o $(SYSBINDIR)/$(LA)-shared
	@$(LN_S) -vf $(LA)-shared $(SYSBINDIR)/$(LA)

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
