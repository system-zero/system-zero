NAME    := Crypt

THISDIR := $(shell (cd . && pwd))

SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

SYSDIR    := $(THISDIR)/../sys
SYSLIBDIR  = $(SYSDIR)/lib/$(NS)
SYSBINDIR  = $(SYSDIR)/bin
SYSTMPDIR  = $(SYSDIR)/tmp

file: shared static

shared: clean-shared
	@$(PRINTF) "#!/bin/env La-shared\n\n" > $(SYSBINDIR)/$(NAME)
	@$(CAT) $(NAME).lai >> $(SYSBINDIR)/$(NAME)
	@$(CHMOD) 0755 $(SYSBINDIR)/$(NAME)
	@$(LN_S) -v $(NAME) $(SYSBINDIR)/$(NAME).md5sum
	@$(LN_S) -v $(NAME) $(SYSBINDIR)/$(NAME).sha256sum
	@$(LN_S) -v $(NAME) $(SYSBINDIR)/$(NAME).sha512sum
	@$(PRINTF) "installing $(NAME) to $(SYSBINDIR)\n"

static: clean-static
	@$(PRINTF) "#!/bin/env La-static\n\n" > $(SYSBINDIR)/$(NAME)-static
	@$(CAT) $(NAME).lai >> $(SYSBINDIR)/$(NAME)-static
	@$(CHMOD) 0755 $(SYSBINDIR)/$(NAME)-static
	@$(PRINTF) "installing $(NAME)-static to $(SYSBINDIR)\n"

clean-shared:
	@$(TEST) ! -f $(SYSBINDIR)/$(NAME) || $(RM) $(SYSBINDIR)/$(NAME)
	@$(TEST) ! -L $(SYSBINDIR)/$(NAME).md5sum || $(RM) $(SYSBINDIR)/$(NAME).md5sum
	@$(TEST) ! -L $(SYSBINDIR)/$(NAME).sha256sum || $(RM) $(SYSBINDIR)/$(NAME).sha256sum
	@$(TEST) ! -L $(SYSBINDIR)/$(NAME).sha512sum || $(RM) $(SYSBINDIR)/$(NAME).sha512sum

clean-static:
	@$(TEST) ! -f $(SYSBINDIR)/$(NAME)-static || $(RM) $(SYSBINDIR)/$(NAME)-static

#-----------------------------------------------#

TEST = test
CAT = cat
RM = rm
PRINTF = printf
CHMOD = chmod
LN_S = ln -s
