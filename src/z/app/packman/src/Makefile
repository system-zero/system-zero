NAME    := Packman

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
	@$(CP) $(NAME) $(SYSBINDIR)
	@$(CHMOD) 755  $(SYSBINDIR)/$(NAME)
	@$(CP) specs/* $(SYSDATADIR)/$(NAME)/specs

clean:
	@$(TEST) ! -x $(SYSBINDIR)/$(NAME)        || $(RM) $(SYSBINDIR)/$(NAME)
	@$(TEST) ! -d $(SYSDATADIR)/$(NAME)/specs || $(RM_R) $(SYSDATADIR)/${NAME)/specs

install-data:
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(NAME)    || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(NAME)
	@$(CP) packman/* $(Z_SHELL_DATADIR)/commands/$(NAME)
#-----------------------------------------------------------
Env: makeenv checkenv
makeenv:
	@$(TEST) -d $(SYSSCRIPTDIR)             || $(MKDIR) -p $(SYSSCRIPTDIR)
	@$(TEST) -d $(SYSDATADIR)/$(NAME)/specs || $(MKDIR) -p $(SYSDATADIR)/$(NAME)/specs

checkenv:
	@$(TEST) -w $(SYSSCRIPTDIR)             || exit 1
	@$(TEST) -w $(SYSDATADIR)/$(NAME)       || exit 1
	@$(TEST) -w $(SYSDATADIR)/$(NAME)/specs || exit 1
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
