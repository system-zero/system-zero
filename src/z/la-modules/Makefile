
THISDIR := $(shell (cd . && pwd))

CC            := gcc
CC_STD        := -std=c11

SYSKERNEL     := $(shell uname -s)
SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

DATADIR        =  $(THISDIR)/data

SYSDIR        := $(THISDIR)/../../../sys/$(SYSARCH)
SYSLIBDIR      = $(SYSDIR)/lib
SYSZLIBDIR     = $(SYSDIR)/lib/z
SYSINCDIR      = $(SYSDIR)/include
SYSMODULEDIR  := $(SYSLIBDIR)/la-modules

all: shared static

Env: makeenv checkenv

shared: Env clean-shared path-module-shared

static: clean-static

clean: clean-shared clean-static

clean-shared: clean-path-shared

clean-static:

#-----------------------------------------------------------
MOD_FLAGS = -L$(SYSLIBDIR) -L$(SYSZLIBDIR) -I$(SYSINCDIR) -g -O2 -lla
DEBUG_FLAGS   := -Wextra -Wshadow -Wall -Wunused-result -Wunused-function -Wunused-macros -Wno-override-init

DEBUG := 0
ifneq ($(DEBUG), 0)
  MOD_FLAGS += $(DEBUG_FLAGS)
endif

SHARED_FLAGS = $(MOD_FLAGS) -shared

PATH_FLAGS = -lpath -lcstring -lstring
path-module-shared:
	$(CC) -o $(SYSMODULEDIR)/path-module.so $(SHARED_FLAGS) $(PATH_FLAGS) path/path-module.c

clean-path-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/path-module.so  || $(RM)  $(SYSMODULEDIR)/path-module.so

#-----------------------------------------------------------
makeenv:
	@$(TEST) -d $(SYSMODULEDIR)     || $(MKDIR) -p $(SYSMODULEDIR)
checkenv:
	@$(TEST) -w $(SYSMODULEDIR)     || exit 1
#-----------------------------------------------------------
INSTALL = install
RM = rm -f
CP = cp
AR = ar rs
CP_R = $(CP) -r
TEST = test
LN = ln
LN_S = $(LN) -s
MKDIR = mkdir
MKDIR_P = $(MKDIR) -p
