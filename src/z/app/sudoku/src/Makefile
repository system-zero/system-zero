NAME    := Sudoku

THISDIR := $(shell (cd . && pwd))

SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

SYSDIR    := $(THISDIR)/../sys
SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
SYSBINDIR  = $(SYSDIR)/bin
SYSTMPDIR  = $(SYSDIR)/tmp
SYSDATADIR = $(SYSDIR)/../data

Z_SHELL         := zs
Z_SHELL_DATADIR := $(SYSDATADIR)/$(Z_SHELL)

sudoku: shared static

shared: clean-shared\
    sudoku-shared\
    install-data

static: clean-static\
    sudoku-static\
    install-data

clean-shared:\
    clean-sudoku-shared

clean-static:\
    clean-sudoku-static

install-data:
	@$(TEST) -d $(Z_SHELL_DATADIR)/commands/$(S)   || $(MKDIR_P) $(Z_SHELL_DATADIR)/commands/$(S)
	@$(CP) $(S)/args  $(Z_SHELL_DATADIR)/commands/$(S)
#-----------------------------------------------#
S = Sudoku
sudoku-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(S)
	@$(CAT) $(S).lai >> $(SYSBINDIR)/$(S)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(S)
	@$(PRINTF) "installing Sudoku to $(SYSBINDIR)\n"

sudoku-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(S)-static
	@$(CAT) $(S).lai >> $(SYSBINDIR)/$(S)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(S)-static
	@$(PRINTF) "installing Sudoku-static to $(SYSBINDIR)\n"

clean-sudoku-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(S) || $(RM) $(SYSBINDIR)/$(S)

clean-sudoku-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(S)-static || $(RM) $(SYSBINDIR)/$(S)-static

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
