NAME    := Path

THISDIR := $(shell (cd . && pwd))

SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

SYSDIR    := $(THISDIR)/../sys
SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
SYSBINDIR  = $(SYSDIR)/bin
SYSTMPDIR  = $(SYSDIR)/tmp

path: shared static

shared: clean-shared\
    path-basename-shared\
    path-dirname-shared\
    path-real-shared

static: clean-static\
    path-basename-static\
    path-dirname-static\
    path-real-static

clean-shared:\
    clean-path-basename-shared\
    clean-path-dirname-shared\
    clean-path-real-shared\

clean-static:\
    clean-path-basename-static\
    clean-path-dirname-static\
    clean-path-real-static
#-----------------------------------------------#
P_basename = Path.basename
path-basename-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(P_basename)
	@$(CAT) $(P_basename).lai >> $(SYSBINDIR)/$(P_basename)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(P_basename)
	@$(PRINTF) "installing Path.basename to $(SYSBINDIR)\n"

path-basename-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(P_basename)-static
	@$(CAT) $(P_basename).lai >> $(SYSBINDIR)/$(P_basename)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(P_basename)-static
	@$(PRINTF) "installing Path.basename-static to $(SYSBINDIR)\n"

clean-path-basename-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(P_basename) || $(RM) $(SYSBINDIR)/$(P_basename)

clean-path-basename-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(P_basename)-static || $(RM) $(SYSBINDIR)/$(P_basename)-static

#-----------------------------------------------#
P_dirname = Path.dirname
path-dirname-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(P_dirname)
	@$(CAT) $(P_dirname).lai >> $(SYSBINDIR)/$(P_dirname)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(P_dirname)
	@$(PRINTF) "installing Path.dirname to $(SYSBINDIR)\n"

path-dirname-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(P_dirname)-static
	@$(CAT) $(P_dirname).lai >> $(SYSBINDIR)/$(P_dirname)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(P_dirname)-static
	@$(PRINTF) "installing Path.dirname-static to $(SYSBINDIR)\n"

clean-path-dirname-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(P_dirname) || $(RM) $(SYSBINDIR)/$(P_dirname)

clean-path-dirname-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(P_dirname)-static || $(RM) $(SYSBINDIR)/$(P_dirname)-static

#-----------------------------------------------#
P_real = Path.real
path-real-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(P_real)
	@$(CAT) $(P_real).lai >> $(SYSBINDIR)/$(P_real)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(P_real)
	@$(PRINTF) "installing Path.real to $(SYSBINDIR)\n"

path-real-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(P_real)-static
	@$(CAT) $(P_real).lai >> $(SYSBINDIR)/$(P_real)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(P_real)-static
	@$(PRINTF) "installing Path.real-static to $(SYSBINDIR)\n"

clean-path-real-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(P_real) || $(RM) $(SYSBINDIR)/$(P_real)

clean-path-real-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(P_real)-static || $(RM) $(SYSBINDIR)/$(P_real)-static

#-----------------------------------------------#

TEST = test
CAT = cat
RM = rm
PRINTF = printf
CHMOD = chmod
