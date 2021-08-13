NAME    := File

THISDIR := $(shell (cd . && pwd))

SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

SYSDIR    := $(THISDIR)/../sys
SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
SYSBINDIR  = $(SYSDIR)/bin
SYSTMPDIR  = $(SYSDIR)/tmp

file: shared static

shared: clean-shared\
    file-exists-shared\
    file-size-shared \
    file-readlink-shared\
    file-stat-shared\
    file-copy-shared

static: clean-static\
    file-exists-static\
    file-size-static\
    file-readlink-static\
    file-stat-static\
    file-copy-shared

clean-shared:\
    clean-file-exists-shared\
    clean-file-size-shared\
    clean-file-readlink-shared\
    clean-file-stat-shared\
    clean-file-copy-shared

clean-static:\
    clean-file-exists-static\
    clean-file-size-static\
    clean-file-readlink-static\
    clean-file-stat-static\
    clean-file-copy-static

#-----------------------------------------------#
F_exists = File.exists
file-exists-shared:
	@$(PRINTF) "#!La-shared\n\n" > $(SYSBINDIR)/$(F_exists)
	@$(CAT) $(F_exists).lai >> $(SYSBINDIR)/$(F_exists)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_exists)
	@$(PRINTF) "installing File.exists to $(SYSBINDIR)\n"

file-exists-static:
	@$(PRINTF) "#!La-static\n\n" > $(SYSBINDIR)/$(F_exists)-static
	@$(CAT) $(F_exists).lai >> $(SYSBINDIR)/$(F_exists)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_exists)-static
	@$(PRINTF) "installing File.exists-static to $(SYSBINDIR)\n"

clean-file-exists-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_exists) || $(RM) $(SYSBINDIR)/$(F_exists)

clean-file-exists-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_exists)-static || $(RM) $(SYSBINDIR)/$(F_exists)-static

#-----------------------------------------------#
F_size = File.size
file-size-shared:
	@$(PRINTF) "#!La-shared\n\n" > $(SYSBINDIR)/$(F_size)
	@$(CAT) $(F_size).lai >> $(SYSBINDIR)/$(F_size)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_size)
	@$(PRINTF) "installing File.size to $(SYSBINDIR)\n"

file-size-static:
	@$(PRINTF) "#!La-static\n\n" > $(SYSBINDIR)/$(F_size)-static
	@$(CAT) $(F_size).lai >> $(SYSBINDIR)/$(F_size)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_size)-static
	@$(PRINTF) "installing File.size-static to $(SYSBINDIR)\n"

clean-file-size-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_size) || $(RM) $(SYSBINDIR)/$(F_size)

clean-file-size-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_size)-static || $(RM) $(SYSBINDIR)/$(F_size)-static

#-----------------------------------------------#
F_readlink = File.readlink
file-readlink-shared:
	@$(PRINTF) "#!La-shared\n\n" > $(SYSBINDIR)/$(F_readlink)
	@$(CAT) $(F_readlink).lai >> $(SYSBINDIR)/$(F_readlink)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_readlink)
	@$(PRINTF) "installing File.readlink to $(SYSBINDIR)\n"

file-readlink-static:
	@$(PRINTF) "#!La-static\n\n" > $(SYSBINDIR)/$(F_readlink)-static
	@$(CAT) $(F_readlink).lai >> $(SYSBINDIR)/$(F_readlink)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_readlink)-static
	@$(PRINTF) "installing File.readlink-static to $(SYSBINDIR)\n"

clean-file-readlink-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_readlink) || $(RM) $(SYSBINDIR)/$(F_readlink)

clean-file-readlink-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_readlink)-static || $(RM) $(SYSBINDIR)/$(F_readlink)-static

#-----------------------------------------------#
F_stat = File.stat
file-stat-shared:
	@$(PRINTF) "#!La-shared\n\n" > $(SYSBINDIR)/$(F_stat)
	@$(CAT) $(F_stat).lai >> $(SYSBINDIR)/$(F_stat)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_stat)
	@$(PRINTF) "installing File.stat to $(SYSBINDIR)\n"

file-stat-static:
	@$(PRINTF) "#!La-static\n\n" > $(SYSBINDIR)/$(F_stat)-static
	@$(CAT) $(F_stat).lai >> $(SYSBINDIR)/$(F_stat)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_stat)-static
	@$(PRINTF) "installing File.stat-static to $(SYSBINDIR)\n"

clean-file-stat-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_stat) || $(RM) $(SYSBINDIR)/$(F_stat)

clean-file-stat-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_stat)-static || $(RM) $(SYSBINDIR)/$(F_stat)-static

#-----------------------------------------------#
F_copy = File.copy
file-copy-shared:
	@$(PRINTF) "#!La-shared\n\n" > $(SYSBINDIR)/$(F_copy)
	@$(CAT) $(F_copy).lai >> $(SYSBINDIR)/$(F_copy)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_copy)
	@$(PRINTF) "installing File.copy to $(SYSBINDIR)\n"

file-copy-static:
	@$(PRINTF) "#!La-static\n\n" > $(SYSBINDIR)/$(F_copy)-static
	@$(CAT) $(F_copy).lai >> $(SYSBINDIR)/$(F_copy)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_copy)-static
	@$(PRINTF) "installing File.copy-static to $(SYSBINDIR)\n"

clean-file-copy-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_copy) || $(RM) $(SYSBINDIR)/$(F_copy)

clean-file-copy-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_copy)-static || $(RM) $(SYSBINDIR)/$(F_copy)-static

#-----------------------------------------------#

TEST = test
CAT = cat
RM = rm
PRINTF = printf
CHMOD = chmod
######
#NAME    := File
#
#NS      :=
#API     := 0
#REV     := 0
#VERSION := $(API).$(REV)
#
## macOS doesn't have realpath by default (available through coreutils)
## THISDIR := $(shell realpath .)
#THISDIR := $(shell (cd . && pwd))
#
#CC            := gcc
#CC_STD        := -std=c11
#
#SYSKERNEL     := $(shell uname -s)
#SYSARCH       := $(shell uname -m)
#SYS           := $(SYSKERNEL)-$(SYSARCH)
#
#SYSDIR    := $(THISDIR)/../sys
#SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
#SYSBINDIR  = $(SYSDIR)/bin
#SYSINCDIR  = $(SYSDIR)/include/$(NS)
#SYSTMPDIR  = $(SYSDIR)/tmp
#
##BASE_FLAGS  := -g -O2 -march=native -fvisibility=hidden -fdollars-in-identifiers
#BASE_FLAGS  := -g -O2 -march=native -fdollars-in-identifiers
#
#DEBUG_FLAGS := -Wextra -Wshadow -Wall -Wunused-result -Wunused-function -Wunused-macros -Wno-override-init
#
## -Wno-override-init: is for designated initializers and both compilers complain.
## it is included in extra in gcc, but clang needs this unconditionally
#ifneq ($(DEBUG), 1)
#  ifeq (-$(CC), -clang)
#    BASE_FLAGS  +=  -Wno-override-init
#  endif
#endif
#
#ifneq (-$(CC), -tcc)
#  DEBUG_FLAGS += -Werror
#  # while compiling tinyexpr tcc warns with: assigment discards qualifiers
#  # from pointer target type
#  # -Wno-error=discarded-qualifiers doesn't have effect
#endif
#
## DEBUG_FLAGS += -fsanitize=undefined
## if turn on -Wpedantic gcc gives: ISO C forbids braced-groups within expressions
## (this is for statement expressions)
#
#STATIC_CFLAGS := -static
#
#FLAGS         := -I$(SYSINCDIR) -L$(SYSLIBDIR) $(BASE_FLAGS)
#
#CHECK_PTR_BOUNDS_FLAGS := -mmpx -fcheck-pointer-bounds
#
#CHECK_PTR_BOUNDS := 0
#
#ifneq ($(CHECK_PTR_BOUNDS), 0)
#  FLAGS += $(CHECK_PTR_BOUNDS_FLAGS)
#endif
#
#ifeq (-$(CC), -gcc)
#  FLAGS += -Wno-implicit-fallthrough
#  # for gcc < 7 version
#  #  FLAGS += -Wimplicit-fallthrough=0
#endif
#
#DEBUG := 0
#ifneq ($(DEBUG), 0)
#  FLAGS += $(DEBUG_FLAGS)
#endif
#
#FLAGS += -DVERSION=$(VERSION) -DVERSION_STRING='"$(VERSION)"'
#FLAGS += -lfile-$(VERSION) -largparse-$(VERSION) -lio-$(VERSION)
#
#file: shared static
#
#shared: clean-shared\
#    file-size-shared
#
#static: clean-static\
#    file-size-static
#
#clean-shared:\
#    clean-file-size-shared
#
#clean-static:\
#    clean-file-size-static
##-----------------------------------------------#
#F_SIZE = $(NAME).size
#
#F_SIZE_STATIC_FLAGS = -ldir-$(VERSION) -lpath-$(VERSION)\
#                      -lvstring-$(VERSION) -lstring-$(VERSION)\
#                      -lcstring-$(VERSION) -largparse-$(VERSION)\
#                      -lustring-$(VERSION) -lerror-$(VERSION)
#
#file-size-shared:
#	$(CC) $(F_SIZE).c $(FLAGS) -o $(SYSBINDIR)/$(F_SIZE)-shared
#	@$(LN_S) -vf $(SYSBINDIR)/$(F_SIZE)-shared $(SYSBINDIR)/$(F_SIZE)
#
#file-size-static:
#	$(CC) $(F_SIZE).c $(FLAGS) $(F_SIZE_STATIC_FLAGS) $(STATIC_CFLAGS) -o $(SYSBINDIR)/$(F_SIZE)-static
#
#clean-file-size-shared:
#	@$(TEST) ! -f $(SYSBINDIR)/$(F_SIZE)-shared || $(RM) $(SYSBINDIR)/$(F_SIZE)-shared
#	@$(TEST) ! -L $(SYSBINDIR)/$(F_SIZE) || $(RM) $(SYSBINDIR)/$(F_SIZE)
#
#clean-file-size-static:
#	@$(TEST) ! -f $(SYSBINDIR)/$(F_SIZE)-static || $(RM) $(SYSBINDIR)/$(F_SIZE)-static
##-----------------------------------------------#
#
#TEST = test
#RM = rm
#LN = ln
#LN_S = $(LN) -s
