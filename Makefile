API      := 0
REV      := 0
VERSION  :=  $(API).$(REV)

# macOS doesn't have realpath by default (available through coreutils)
# THISDIR := $(shell realpath .)
THISDIR       := $(shell (cd . && pwd))

CC            := gcc
CC_STD        := -std=c11

SYSKERNEL     := $(shell uname -s)
SYSARCH       := $(shell uname -m)
SYS           := $(SYSKERNEL)-$(SYSARCH)

SRCDIR         = $(THISDIR)/src

SYSDIR        := $(THISDIR)/sys/$(SYSARCH)

DEBUG         := 1

MARGS          = API=$(API) REV=$(REV) VERSION=$(VERSION)
MARGS         += SYSDIR=$(SYSDIR) DEBUG=$(DEBUG)

#----------------------------------------------------------#
zero-shared:
	@cd $(SRCDIR) && $(MAKE) NS=z $(MARGS) $@

zero-static:
	@cd $(SRCDIR) && $(MAKE) NS=z $(MARGS) $@
