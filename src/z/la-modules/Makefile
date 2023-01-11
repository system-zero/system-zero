
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
        time-module-shared crypt-module-shared rand-module-shared\
        io-module-shared url-module-shared dl-module-shared\
        sun-module-shared moon-module-shared man-module-shared\
        net-module-shared

clean: clean-shared

clean-shared: clean-path-shared clean-file-shared clean-std-shared\
              clean-term-shared clean-dir-shared clean-sh-shared\
              clean-os-shared clean-sys-shared clean-time-shared\
              clean-crypt-shared clean-rand-shared clean-io-shared\
              clean-url-shared clean-dl-shared\
              clean-sun-shared clean-moon-shared clean-man-shared\
              clean-net-shared

#-----------------------------------------------------------
MOD_FLAGS = -L$(SYSLIBDIR) -L$(SYSZLIBDIR) -I$(SYSINCDIR) -Wl,-rpath=$(SYSZLIBDIR) -g -O2 -fPIC
DEBUG_FLAGS   := -Wextra -Wshadow -Wall -Wunused-result -Wunused-function -Wunused-macros -Wno-override-init

DEBUG := 0
ifneq ($(DEBUG), 0)
  MOD_FLAGS += $(DEBUG_FLAGS)
endif

SHARED_FLAGS  = $(MOD_FLAGS) -shared
SHARED_FLAGS += -lla -lstring -lvmap -lvstring
#-----------------------------------------------------------
PATH_FLAGS = -lpath -lcstring
path-module-shared:
	$(CC) -o $(SYSMODULEDIR)/path-module.so $(SHARED_FLAGS) $(PATH_FLAGS) path/path-module.c

clean-path-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/path-module.so  || $(RM)  $(SYSMODULEDIR)/path-module.so

#-----------------------------------------------------------
FILE_FLAGS = -lfile -lcstring -los -lerror -lsys
file-module-shared:
	$(CC) -o $(SYSMODULEDIR)/file-module.so $(SHARED_FLAGS) $(FILE_FLAGS) file/file-module.c

clean-file-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/file-module.so  || $(RM)  $(SYSMODULEDIR)/file-module.so

#-----------------------------------------------------------
STD_FLAGS = -lcstring -lustring
std-module-shared:
	$(CC) -o $(SYSMODULEDIR)/std-module.so $(SHARED_FLAGS) $(STD_FLAGS) std/std-module.c

clean-std-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/std-module.so  || $(RM)  $(SYSMODULEDIR)/std-module.so

#-----------------------------------------------------------
TERM_FLAGS = -lio -lterm -lcstring
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
SH_FLAGS = -lcstring -lsh
sh-module-shared:
	$(CC) -o $(SYSMODULEDIR)/sh-module.so $(SHARED_FLAGS) $(SH_FLAGS) sh/sh-module.c

clean-sh-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/sh-module.so  || $(RM)  $(SYSMODULEDIR)/sh-module.so
#-----------------------------------------------------------
TIME_FLAGS = -lerror
time-module-shared:
	$(CC) -o $(SYSMODULEDIR)/time-module.so $(SHARED_FLAGS) $(TIME_FLAGS) time/time-module.c

clean-time-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/time-module.so  || $(RM)  $(SYSMODULEDIR)/time-module.so
#-----------------------------------------------------------
OS_FLAGS = -los
os-module-shared:
	$(CC) -o $(SYSMODULEDIR)/os-module.so $(SHARED_FLAGS) $(OS_FLAGS) os/os-module.c

clean-os-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/os-module.so  || $(RM)  $(SYSMODULEDIR)/os-module.so
#-----------------------------------------------------------
SYS_FLAGS = -lsys
sys-module-shared:
	$(CC) -o $(SYSMODULEDIR)/sys-module.so $(SHARED_FLAGS) $(SYS_FLAGS) sys/sys-module.c

clean-sys-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/sys-module.so  || $(RM)  $(SYSMODULEDIR)/sys-module.so
#-----------------------------------------------------------
CRYPT_FLAGS = -lerror -lmd5 -lsha256 -lsha512 -lbase64 -lbcrypt
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
IO_FLAGS = -lio
io-module-shared:
	$(CC) -o $(SYSMODULEDIR)/io-module.so $(SHARED_FLAGS) $(IO_FLAGS) io/io-module.c

clean-io-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/io-module.so  || $(RM)  $(SYSMODULEDIR)/io-module.so

#-----------------------------------------------------------
URL_FLAGS = -lpath -lurl
url-module-shared: Env
	$(CC) -o $(SYSMODULEDIR)/url-module.so $(SHARED_FLAGS) $(URL_FLAGS) url/url-module.c

clean-url-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/url-module.so  || $(RM)  $(SYSMODULEDIR)/url-module.so

#-----------------------------------------------------------
DL_FLAGS = -ldl-$(VERSION)
dl-module-shared: Env
	$(CC) -o $(SYSMODULEDIR)/dl-module.so $(SHARED_FLAGS) $(DL_FLAGS) dl/dl-module.c

clean-dl-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/dl-module.so  || $(RM)  $(SYSMODULEDIR)/dl-module.so

#-----------------------------------------------------------
SUN_FLAGS = -lstring -lvmap -lsun -lm
sun-module-shared: Env
	$(CC) -o $(SYSMODULEDIR)/sun-module.so $(SHARED_FLAGS) $(SUN_FLAGS) sun/sun-module.c

clean-sun-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/sun-module.so  || $(RM)  $(SYSMODULEDIR)/sun-module.so

#-----------------------------------------------------------
MOON_FLAGS = -lstring -lvmap -lmoon
moon-module-shared: Env
	$(CC) -o $(SYSMODULEDIR)/moon-module.so $(SHARED_FLAGS) $(MOON_FLAGS) moon/moon-module.c

clean-moon-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/moon-module.so  || $(RM)  $(SYSMODULEDIR)/moon-module.so

#-----------------------------------------------------------
MAN_FLAGS = -lman
man-module-shared: Env
	$(CC) -o $(SYSMODULEDIR)/man-module.so $(SHARED_FLAGS) $(MAN_FLAGS) man/man-module.c

clean-man-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/man-module.so  || $(RM)  $(SYSMODULEDIR)/man-module.so

#-----------------------------------------------------------
SUDOKU_FLAGS = -lsudoku -lterm -lvideo
sudoku-module-shared: Env
	$(CC) -o $(SYSMODULEDIR)/sudoku-module.so $(SHARED_FLAGS) $(SUDOKU_FLAGS) sudoku/sudoku-module.c

clean-sudoku-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/sudoku-module.so  || $(RM)  $(SYSMODULEDIR)/sudoku-module.so

#-----------------------------------------------------------
NET_FLAGS = -lstring -lvmap -lpath -lvstring -lerror -lnet -lurl -lssl -lcrypto -lresolv -lpthread
net-module-shared: Env clean-net-shared
	$(CC) -o $(SYSMODULEDIR)/net-module.so $(SHARED_FLAGS) $(NET_FLAGS) net/net-module.c

clean-net-shared:
	@$(TEST) ! -f $(SYSMODULEDIR)/net-module.so  || $(RM)  $(SYSMODULEDIR)/net-module.so

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
