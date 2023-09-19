NAME    := Packman
DBNAME  := packman

NS      :=
API     := 0
REV     := 0
VERSION := $(API).$(REV)

# macOS doesn't have realpath by default (available through coreutils)
# THISDIR := $(shell realpath .)
THISDIR := $(shell (cd . && pwd))

SYSKERNEL     := $(shell uname -s)
SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

SYSDIR    := $(THISDIR)/../sys
SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
SYSBINDIR  = $(SYSDIR)/bin
SYSINCDIR  = $(SYSDIR)/include/$(NS)
SYSDATADIR = $(SYSDIR)/../data
SYSTMPDIR  = $(SYSDIR)/tmp

Z_SHELL         := zs
Z_SHELL_DATADIR := $(SYSDATADIR)/$(Z_SHELL)

packman: Env install-data
	@$(PRINTF) "installing $(NAME) to $(SYSBINDIR)\n"
	@$(CP) $(NAME)    $(SYSBINDIR)
	@$(CHMOD) 755     $(SYSBINDIR)/$(NAME)
	@$(TEST) -d       $(SYSDATADIR)/$(DBNAME)/specs  || $(MKDIR) $(SYSDATADIR)/$(DBNAME)/specs
	@$(CP) specs/*    $(SYSDATADIR)/$(DBNAME)/specs
	@$(TEST) -d       $(SYSDATADIR)/$(DBNAME)/data   || $(MKDIR) $(SYSDATADIR)/$(DBNAME)/data
	@$(TEST) ! -d data || $(CP_R)  data/*  $(SYSDATADIR)/$(DBNAME)/data
	@$(TEST) -d       $(SYSDATADIR)/$(DBNAME)/db     || $(MKDIR) $(SYSDATADIR)/$(DBNAME)/db

clean:
	@$(TEST) ! -x $(SYSBINDIR)/$(NAME)        || $(RM)   $(SYSBINDIR)/$(NAME)
	@$(TEST) ! -d $(SYSDATADIR)/$(DBNAME)/specs || $(RM_R) $(SYSDATADIR)/$(DBNAME)/specs
	@$(TEST) ! -d $(SYSDATADIR)/$(DBNAME)/data  || $(RM_R) $(SYSDATADIR)/$(DBNAME)/data

install-data:
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(NAME)    || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(NAME)
	@$(CP) packman/* $(Z_SHELL_DATADIR)/commands/$(NAME)
#-----------------------------------------------------------
Env: makeenv checkenv
makeenv:
	@$(TEST) -d $(SYSSCRIPTDIR)               || $(MKDIR) -p $(SYSSCRIPTDIR)
	@$(TEST) -d $(SYSDATADIR)/$(DBNAME)/specs || $(MKDIR) -p $(SYSDATADIR)/$(DBNAME)/specs
	@$(TEST) -d $(SYSDATADIR)/$(DBNAME)/db    || $(MKDIR) -p $(SYSDATADIR)/$(DBNAME)/db

checkenv:
	@$(TEST) -w $(SYSSCRIPTDIR)               || exit 1
	@$(TEST) -w $(SYSDATADIR)/$(DBNAME)       || exit 1
	@$(TEST) -w $(SYSDATADIR)/$(DBNAME)/specs || exit 1
	@$(TEST) -w $(SYSDATADIR)/$(DBNAME)/db    || exit 1
#-----------------------------------------------------------
TEST = test
CP = cp
CP_R = $(CP) -r
RM = rm
RM_R = $(RM) -r
LN = ln
LN_S = $(LN) -s
PRINTF = printf
CHMOD = chmod
MKDIR = mkdir
MKDIR_P = $(MKDIR) -p
