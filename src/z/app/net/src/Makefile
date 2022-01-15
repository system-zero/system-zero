NAME    := Net

THISDIR := $(shell (cd . && pwd))

SYSARCH := $(shell uname -m)
SYS     := $(SYSKERNEL)-$(SYSARCH)

SYSDIR    := $(THISDIR)/../../../../../sys/$(SYSARCH)
SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
SYSBINDIR  = $(SYSDIR)/bin
SYSTMPDIR  = $(SYSDIR)/tmp
SYSDATADIR = $(SYSDIR)/../data

Z_SHELL         := zs
Z_SHELL_DATADIR := $(SYSDATADIR)/$(Z_SHELL)

net: shared static

shared: clean-shared\
    net-fetch-shared\
    install-data

static: clean-static\
    net-fetch-static\
    install-data

clean-shared:\
    clean-net-fetch-shared

clean-static:\
    clean-net-fetch-static

install-data:
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(N_fetch) || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(N_fetch)
	@$(CP) $(N_fetch)/args  $(Z_SHELL_DATADIR)/commands/$(N_fetch)
#-----------------------------------------------#
N_fetch = Net.fetch
net-fetch-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(N_fetch)
	@$(CAT) $(N_fetch).lai >> $(SYSBINDIR)/$(N_fetch)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(N_fetch)
	@$(PRINTF) "installing Net.fetch to $(SYSBINDIR)\n"

net-fetch-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(N_fetch)-static
	@$(CAT) $(N_fetch).lai >> $(SYSBINDIR)/$(N_fetch)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(N_fetch)-static
	@$(PRINTF) "installing Net.fetch-static to $(SYSBINDIR)\n"

clean-net-fetch-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(N_fetch) || $(RM) $(SYSBINDIR)/$(N_fetch)

clean-net-fetch-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(N_fetch)-static || $(RM) $(SYSBINDIR)/$(N_fetch)-static

#-----------------------------------------------#

TEST = test
CAT = cat
RM = rm
PRINTF = printf
CHMOD = chmod
MKDIR = mkdir
MKDIR_P = $(MKDIR) -p
CP = cp
CP_R = $(CP) -r
