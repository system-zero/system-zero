NS :=
THIS_HEADER  = dlist.h

THISDIR := $(shell (cd . && pwd))

SYSDIR    := $(THISDIR)/../sys
SYSINCDIR  = $(SYSDIR)/include/$(NS)

dlist: $(SYSINCDIR)/$(THIS_HEADER)

header: Env clean-header $(SYSINCDIR)/$(THIS_HEADER)

$(SYSINCDIR)/$(THIS_HEADER):
	@$(CP) $(THIS_HEADER) $(SYSINCDIR)

clean: clean-header

clean-header:
	@$(TEST) ! -f $(SYSINCDIR)/$(THIS_HEADER) || $(RM) $(SYSINCDIR)/$(THIS_HEADER)

AR = ar rs
TEST = test
STAT = stat
GREP = grep
RM = rm -f
RM_R = rm -r -f
RMDIR = rmdir
MKDIR = mkdir
MKDIR_P = $(MKDIR) -p
CP = cp
CP_R = $(CP) -r
LN_S = ln -s
CHMOD = chmod
CHMOD_X = $(CHMOD) 0755
PRINTF = printf
INSTALL = install
MAKE = make
VALGRIND = valgrind
VALGRIND_ARGS = --leak-check=full --show-leak-kinds=all -v --track-origins=yes
GDB = gdb
GDB_ARGS = --quiet -ex "set logging file /tmp/gdb.txt" -ex "set logging on" -ex run --args
#----------------------------------------------------------#

Env: makeenv checkenv
makeenv:
	@$(TEST) -d $(SYSDIR)     || $(MKDIR_P) $(SYSDIR)
	@$(TEST) -d $(SYSINCDIR)  || $(MKDIR)   $(SYSINCDIR)

checkenv:
	@$(TEST) -w $(SYSDIR)     || exit 1
	@$(TEST) -w $(SYSINCDIR)  || exit 1
