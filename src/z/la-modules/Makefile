
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
        std-module-shared term-module-shared dir-module-shared\
        sh-module-shared os-module-shared sys-module-shared\
        time-module-shared crypt-module-shared rand-module-shared

clean: clean-shared

clean-shared: clean-path-shared clean-file-shared clean-std-shared\
              clean-term-shared clean-dir-shared clean-sh-shared\
              clean-os-shared clean-sys-shared clean-time-shared\
              clean-crypt-shared clean-rand-shared

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
FILE_FLAGS = -lfile -lvmap -lstring -lvstring -lcstring -los -lerror -lsys
file-module-shared:
	$(CC) -o $(SYSMODULEDIR)/file-module.so $(SHARED_FLAGS) $(FILE_FLAGS) file/file-module.c

clean-file-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/file-module.so  || $(RM)  $(SYSMODULEDIR)/file-module.so

#-----------------------------------------------------------
STD_FLAGS = -lvmap -lstring -lcstring -lustring
std-module-shared:
	$(CC) -o $(SYSMODULEDIR)/std-module.so $(SHARED_FLAGS) $(STD_FLAGS) std/std-module.c

clean-std-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/std-module.so  || $(RM)  $(SYSMODULEDIR)/std-module.so

#-----------------------------------------------------------
TERM_FLAGS = -lio -lterm -lstring -lvmap -lcstring
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
SH_FLAGS = -lvmap -lvstring -lstring -lcstring -lsh
sh-module-shared:
	$(CC) -o $(SYSMODULEDIR)/sh-module.so $(SHARED_FLAGS) $(SH_FLAGS) sh/sh-module.c

clean-sh-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/sh-module.so  || $(RM)  $(SYSMODULEDIR)/sh-module.so
#-----------------------------------------------------------
TIME_FLAGS = -lstring -lvmap
time-module-shared:
	$(CC) -o $(SYSMODULEDIR)/time-module.so $(SHARED_FLAGS) $(TIME_FLAGS) time/time-module.c

clean-time-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/time-module.so  || $(RM)  $(SYSMODULEDIR)/time-module.so
#-----------------------------------------------------------
OS_FLAGS = -lvmap -lvstring -lstring -los
os-module-shared:
	$(CC) -o $(SYSMODULEDIR)/os-module.so $(SHARED_FLAGS) $(OS_FLAGS) os/os-module.c

clean-os-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/os-module.so  || $(RM)  $(SYSMODULEDIR)/os-module.so
#-----------------------------------------------------------
SYS_FLAGS = -lvmap -lstring -lsys
sys-module-shared:
	$(CC) -o $(SYSMODULEDIR)/sys-module.so $(SHARED_FLAGS) $(SYS_FLAGS) sys/sys-module.c

clean-sys-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/sys-module.so  || $(RM)  $(SYSMODULEDIR)/sys-module.so
#-----------------------------------------------------------
CRYPT_FLAGS = -lvmap -lstring -lerror -lmd5 -lsha256 -lsha512 -lbase64 -lbcrypt
crypt-module-shared:
	$(CC) -o $(SYSMODULEDIR)/crypt-module.so $(SHARED_FLAGS) $(CRYPT_FLAGS) crypt/crypt-module.c

clean-crypt-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/crypt-module.so  || $(RM)  $(SYSMODULEDIR)/crypt-module.so

#-----------------------------------------------------------
RAND_FLAGS = -lrandom-$(VERSION)
rand-module-shared:
	$(CC) -o $(SYSMODULEDIR)/rand-module.so $(SHARED_FLAGS) $(RAND_FLAGS) rand/rand-module.c

clean-rand-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/rand-module.so  || $(RM)  $(SYSMODULEDIR)/rand-module.so
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
