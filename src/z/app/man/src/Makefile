NAME    := Man

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

man: shared static

shared: clean-shared\
    man-page-shared\
    install-data

static: clean-static\
    man-page-static\
    install-data

clean-shared:\
    clean-man-page-shared

clean-static:\
    clean-man-page-static

install-data:
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(Man_page) || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(Man_page)
	@$(CP) $(Man_page)/args  $(Z_SHELL_DATADIR)/commands/$(Man_page)
#-----------------------------------------------#
Man_page = Man.page
man-page-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(Man_page)
	@$(CAT) $(Man_page).lai >> $(SYSBINDIR)/$(Man_page)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(Man_page)
	@$(PRINTF) "installing Man.page to $(SYSBINDIR)\n"

man-page-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(Man_page)-static
	@$(CAT) $(Man_page).lai >> $(SYSBINDIR)/$(Man_page)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(Man_page)-static
	@$(PRINTF) "installing Man.page-static to $(SYSBINDIR)\n"

clean-man-page-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(Man_page) || $(RM) $(SYSBINDIR)/$(Man_page)

clean-man-page-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(Man_page)-static || $(RM) $(SYSBINDIR)/$(Man_page)-static

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
