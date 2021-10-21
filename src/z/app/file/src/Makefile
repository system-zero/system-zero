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
    file-copy-shared\
    file-symlink-shared\
    file-chmod-shared\
    file-chown-shared\
    file-remove-shared\
    file-rename-shared\
    file-move-shared

static: clean-static\
    file-exists-static\
    file-size-static\
    file-readlink-static\
    file-stat-static\
    file-copy-static\
    file-symlink-static\
    file-chmod-static\
    file-chown-static\
    file-remove-static\
    file-rename-static\
    file-move-static

clean-shared:\
    clean-file-exists-shared\
    clean-file-size-shared\
    clean-file-readlink-shared\
    clean-file-stat-shared\
    clean-file-copy-shared\
    clean-file-symlink-shared\
    clean-file-chmod-shared\
    clean-file-chown-shared\
    clean-file-remove-shared\
    clean-file-rename-shared\
    clean-file-move-shared

clean-static:\
    clean-file-exists-static\
    clean-file-size-static\
    clean-file-readlink-static\
    clean-file-stat-static\
    clean-file-copy-static\
    clean-file-symlink-static\
    clean-file-chmod-static\
    clean-file-chown-static\
    clean-file-remove-static\
    clean-file-rename-shared\
    clean-file-move-shared

#-----------------------------------------------#
F_exists = File.exists
file-exists-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_exists)
	@$(CAT) $(F_exists).lai >> $(SYSBINDIR)/$(F_exists)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_exists)
	@$(PRINTF) "installing File.exists to $(SYSBINDIR)\n"

file-exists-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_exists)-static
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
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_size)
	@$(CAT) $(F_size).lai >> $(SYSBINDIR)/$(F_size)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_size)
	@$(PRINTF) "installing File.size to $(SYSBINDIR)\n"

file-size-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_size)-static
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
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_readlink)
	@$(CAT) $(F_readlink).lai >> $(SYSBINDIR)/$(F_readlink)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_readlink)
	@$(PRINTF) "installing File.readlink to $(SYSBINDIR)\n"

file-readlink-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_readlink)-static
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
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_stat)
	@$(CAT) $(F_stat).lai >> $(SYSBINDIR)/$(F_stat)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_stat)
	@$(PRINTF) "installing File.stat to $(SYSBINDIR)\n"

file-stat-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_stat)-static
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
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_copy)
	@$(CAT) $(F_copy).lai >> $(SYSBINDIR)/$(F_copy)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_copy)
	@$(PRINTF) "installing File.copy to $(SYSBINDIR)\n"

file-copy-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_copy)-static
	@$(CAT) $(F_copy).lai >> $(SYSBINDIR)/$(F_copy)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_copy)-static
	@$(PRINTF) "installing File.copy-static to $(SYSBINDIR)\n"

clean-file-copy-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_copy) || $(RM) $(SYSBINDIR)/$(F_copy)

clean-file-copy-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_copy)-static || $(RM) $(SYSBINDIR)/$(F_copy)-static

#-----------------------------------------------#
F_symlink = File.symlink
file-symlink-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_symlink)
	@$(CAT) $(F_symlink).lai >> $(SYSBINDIR)/$(F_symlink)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_symlink)
	@$(PRINTF) "installing File.symlink to $(SYSBINDIR)\n"

file-symlink-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_symlink)-static
	@$(CAT) $(F_symlink).lai >> $(SYSBINDIR)/$(F_symlink)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_symlink)-static
	@$(PRINTF) "installing File.symlink-static to $(SYSBINDIR)\n"

clean-file-symlink-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_symlink) || $(RM) $(SYSBINDIR)/$(F_symlink)

clean-file-symlink-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_symlink)-static || $(RM) $(SYSBINDIR)/$(F_symlink)-static

#-----------------------------------------------#
F_chmod = File.chmod
file-chmod-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_chmod)
	@$(CAT) $(F_chmod).lai >> $(SYSBINDIR)/$(F_chmod)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_chmod)
	@$(PRINTF) "installing File.chmod to $(SYSBINDIR)\n"

file-chmod-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_chmod)-static
	@$(CAT) $(F_chmod).lai >> $(SYSBINDIR)/$(F_chmod)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_chmod)-static
	@$(PRINTF) "installing File.chmod-static to $(SYSBINDIR)\n"

clean-file-chmod-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_chmod) || $(RM) $(SYSBINDIR)/$(F_chmod)

clean-file-chmod-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_chmod)-static || $(RM) $(SYSBINDIR)/$(F_chmod)-static

#-----------------------------------------------#
F_chown = File.chown
file-chown-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_chown)
	@$(CAT) $(F_chown).lai >> $(SYSBINDIR)/$(F_chown)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_chown)
	@$(PRINTF) "installing File.chown to $(SYSBINDIR)\n"

file-chown-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_chown)-static
	@$(CAT) $(F_chown).lai >> $(SYSBINDIR)/$(F_chown)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_chown)-static
	@$(PRINTF) "installing File.chown-static to $(SYSBINDIR)\n"

clean-file-chown-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_chown) || $(RM) $(SYSBINDIR)/$(F_chown)

clean-file-chown-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_chown)-static || $(RM) $(SYSBINDIR)/$(F_chown)-static

#-----------------------------------------------#
F_remove = File.remove
file-remove-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_remove)
	@$(CAT) $(F_remove).lai >> $(SYSBINDIR)/$(F_remove)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_remove)
	@$(PRINTF) "installing File.remove to $(SYSBINDIR)\n"

file-remove-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_remove)-static
	@$(CAT) $(F_remove).lai >> $(SYSBINDIR)/$(F_remove)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_remove)-static
	@$(PRINTF) "installing File.remove-static to $(SYSBINDIR)\n"

clean-file-remove-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_remove) || $(RM) $(SYSBINDIR)/$(F_remove)

clean-file-remove-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_remove)-static || $(RM) $(SYSBINDIR)/$(F_remove)-static

#-----------------------------------------------#
F_rename = File.rename
file-rename-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_rename)
	@$(CAT) $(F_rename).lai >> $(SYSBINDIR)/$(F_rename)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_rename)
	@$(PRINTF) "installing File.rename to $(SYSBINDIR)\n"

file-rename-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_rename)-static
	@$(CAT) $(F_rename).lai >> $(SYSBINDIR)/$(F_rename)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_rename)-static
	@$(PRINTF) "installing File.rename-static to $(SYSBINDIR)\n"

clean-file-rename-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_rename) || $(RM) $(SYSBINDIR)/$(F_rename)

clean-file-rename-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_rename)-static || $(RM) $(SYSBINDIR)/$(F_rename)-static

#-----------------------------------------------#
F_move = File.move
file-move-shared:
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(F_move)
	@$(CAT) $(F_move).lai >> $(SYSBINDIR)/$(F_move)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_move)
	@$(PRINTF) "installing File.move to $(SYSBINDIR)\n"

file-move-static:
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(F_move)-static
	@$(CAT) $(F_move).lai >> $(SYSBINDIR)/$(F_move)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(F_move)-static
	@$(PRINTF) "installing File.move-static to $(SYSBINDIR)\n"

clean-file-move-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_move) || $(RM) $(SYSBINDIR)/$(F_move)

clean-file-move-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(F_move)-static || $(RM) $(SYSBINDIR)/$(F_move)-static

#-----------------------------------------------#

TEST = test
CAT = cat
RM = rm
PRINTF = printf
CHMOD = chmod
