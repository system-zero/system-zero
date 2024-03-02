NAME    := Time

THISDIR := $(shell (cd . && pwd))

SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

SYSDIR    := $(THISDIR)/../../../../../sys/$(SYSARCH)
SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
SYSBINDIR  = $(SYSDIR)/bin
SYSTMPDIR  = $(SYSDIR)/tmp
SYSDATADIR = $(SYSDIR)/../data

Z_SHELL         := zs
Z_SHELL_DATADIR := $(SYSDATADIR)/$(Z_SHELL)

time: shared static

shared: clean-shared\
    time-now-shared\
    time-now_to_string-shared\
    date-shared\
    install-data

static: clean-static\
    time-now-static\
    time-now_to_string-static\
    date-static\
    install-data

clean-shared:\
    clean-time-now-shared\
    clean-time-now_to_string-shared\
    clean-date-shared

clean-static:\
    clean-time-now-static\
    clean-time-now_to_string-static\
    clean-date-static

install-data:
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(T_now)   || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(T_now)
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(T_now_to_string) || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(T_now_to_string)
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(Date)    || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(Date)

#-----------------------------------------------#
T_now = Time.now
time-now-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(T_now)
	@$(CAT) $(T_now).lai >> $(SYSBINDIR)/$(T_now)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(T_now)
	@$(PRINTF) "installing Time.now to $(SYSBINDIR)\n"

time-now-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(T_now)-static
	@$(CAT) $(T_now).lai >> $(SYSBINDIR)/$(T_now)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(T_now)-static
	@$(PRINTF) "installing Time.now-static to $(SYSBINDIR)\n"

clean-time-now-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(T_now) || $(RM) $(SYSBINDIR)/$(T_now)

clean-time-now-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(T_now)-static || $(RM) $(SYSBINDIR)/$(T_now)-static
#-----------------------------------------------#
T_now_to_string = Time.now.to_string
time-now_to_string-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(T_now_to_string)
	@$(CAT) $(T_now_to_string).lai >> $(SYSBINDIR)/$(T_now_to_string)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(T_now_to_string)
	@$(PRINTF) "installing $(T_now_to_string) to $(SYSBINDIR)\n"

time-now_to_string-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(T_now_to_string)-static
	@$(CAT) $(T_now_to_string).lai >> $(SYSBINDIR)/$(T_now_to_string)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(T_now_to_string)-static
	@$(PRINTF) "installing $(T_now_to_string)-static to $(SYSBINDIR)\n"

clean-time-now_to_string-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(T_now_to_string) || $(RM) $(SYSBINDIR)/$(T_now_to_string)

clean-time-now_to_string-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(T_now_to_string)-static || $(RM) $(SYSBINDIR)/$(T_now_to_string)-static

Date = Date
date-shared: time-now_to_string-shared
	@$(SYMLNK_V) -f $(T_now_to_string) $(SYSBINDIR)/$(Date)

date-static: time-now_to_string-static
	@$(SYMLNK_V) -f $(T_now_to_string)-static $(SYSBINDIR)/$(Date)-static

clean-date-shared:
	@$(TEST) ! -L $(SYSBINDIR)/$(Date) || $(RM) $(SYSBINDIR)/$(Date)

clean-date-static:
	@$(TEST) ! -L $(SYSBINDIR)/$(Date)-static || $(RM) $(SYSBINDIR)/$(Date)-static
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
SYMLNK = ln -s
SYMLNK_V = $(SYMLNK) -v

