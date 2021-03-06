
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

all: shared

Env: makeenv checkenv

shared: Env clean-shared path-module-shared file-module-shared\
        string-module-shared std-module-shared term-module-shared\
        dir-module-shared sh-module-shared

clean: clean-shared

clean-shared: clean-path-shared clean-file-shared clean-string-shared\
              clean-std-shared clean-term-shared clean-dir-shared\
              clean-sh-shared

#-----------------------------------------------------------
MOD_FLAGS = -L$(SYSLIBDIR) -L$(SYSZLIBDIR) -I$(SYSINCDIR) -g -O2 -fPIC -lla
DEBUG_FLAGS   := -Wextra -Wshadow -Wall -Wunused-result -Wunused-function -Wunused-macros -Wno-override-init

DEBUG := 0
ifneq ($(DEBUG), 0)
  MOD_FLAGS += $(DEBUG_FLAGS)
endif

SHARED_FLAGS = $(MOD_FLAGS) -shared

#-----------------------------------------------------------
PATH_FLAGS = -lpath -lcstring -lstring
path-module-shared:
	$(CC) -o $(SYSMODULEDIR)/path-module.so $(SHARED_FLAGS) $(PATH_FLAGS) path/path-module.c

clean-path-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/path-module.so  || $(RM)  $(SYSMODULEDIR)/path-module.so

#-----------------------------------------------------------
FILE_FLAGS = -lfile -lvmap -lstring -lvstring
file-module-shared:
	$(CC) -o $(SYSMODULEDIR)/file-module.so $(SHARED_FLAGS) $(FILE_FLAGS) file/file-module.c

clean-file-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/file-module.so  || $(RM)  $(SYSMODULEDIR)/file-module.so

#-----------------------------------------------------------
STRING_FLAGS = -lcstring -lstring -lustring
string-module-shared:
	$(CC) -o $(SYSMODULEDIR)/string-module.so $(SHARED_FLAGS) $(STRING_FLAGS) string/string-module.c

clean-string-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/string-module.so  || $(RM)  $(SYSMODULEDIR)/string-module.so

#-----------------------------------------------------------
STD_FLAGS = -lvmap -lstring -lcstring
std-module-shared:
	$(CC) -o $(SYSMODULEDIR)/std-module.so $(SHARED_FLAGS) $(STD_FLAGS) std/std-module.c

clean-std-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/std-module.so  || $(RM)  $(SYSMODULEDIR)/std-module.so

#-----------------------------------------------------------
TERM_FLAGS = -lio -lterm -lstring -lvmap
term-module-shared:
	$(CC) -o $(SYSMODULEDIR)/term-module.so $(SHARED_FLAGS) $(TERM_FLAGS) term/term-module.c

clean-term-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/term-module.so  || $(RM)  $(SYSMODULEDIR)/term-module.so

#-----------------------------------------------------------
DIR_FLAGS = -ldir
dir-module-shared:
	$(CC) -o $(SYSMODULEDIR)/dir-module.so $(SHARED_FLAGS) $(DIR_FLAGS) dir/dir-module.c

clean-dir-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/dir-module.so  || $(RM)  $(SYSMODULEDIR)/dir-module.so

#-----------------------------------------------------------
SH_FLAGS = -lvmap -lvstring -lstring -lsh
sh-module-shared:
	$(CC) -o $(SYSMODULEDIR)/sh-module.so $(SHARED_FLAGS) $(SH_FLAGS) sh/sh-module.c

clean-sh-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/sh-module.so  || $(RM)  $(SYSMODULEDIR)/sh-module.so
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
