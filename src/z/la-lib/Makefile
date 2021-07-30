
THISDIR := $(shell (cd . && pwd))

CC            := gcc
CC_STD        := -std=c11

SYSKERNEL     := $(shell uname -s)
SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

DATADIR        =  $(THISDIR)/data

SYSDIR        := $(THISDIR)/../../../sys/$(SYSARCH)
SYSZLIBDIR     = $(SYSDIR)/lib/z
SYSSCRIPTDIR   = $(SYSLIBDIR)/la-lib

all: Env
	@$(CP) argparse/argparse.lai $(SYSSCRIPTDIR)
	@$(CP) packman/packman.lai   $(SYSSCRIPTDIR)

Env: makeenv checkenv
makeenv:
	@$(TEST) -d $(SYSSCRIPTDIR)     || $(MKDIR) -p $(SYSSCRIPTDIR)
checkenv:
	@$(TEST) -w $(SYSSCRIPTDIR)     || exit 1
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
