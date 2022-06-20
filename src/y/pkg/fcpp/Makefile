NAME           = fcpp

BASE_FLAGS    := -g -O2 -march=native -fdollars-in-identifiers -Wno-char-subscripts

DEBUG_FLAGS   := -Wextra -Wshadow -Wall -Wunused-result -Wunused-function -Wunused-macros -Wno-override-init
DEBUG_FLAGS   += -Werror-implicit-function-declaration -Wsign-compare
DEBUG_FLAGS   += -Wpointer-arith -Wreturn-type -Wmissing-declarations -Wwrite-strings
DEBUG_FLAGS   += -Wuninitialized

ifeq (-$(CC), -gcc)
  DEBUG_FLAGS += -Wimplicit-fallthrough -Wrestrict
endif

ifeq (-$(CC), -clang)
  BASE_FLAGS  +=  -Wno-unused-command-line-argument
  BASE_FLAGS  +=  -Wno-override-init
endif

SHARED_CFLAGS = -shared -fPIC
SHARED_LIB    = lib$(NAME).so

STATIC_CFLAGS = -static
STATIC_LIB    = lib$(NAME).a

shared:
	$(CC) $(NAME).c $(BASE_FLAGS) $(DEBUG_FLAGS) $(SHARED_CFLAGS) -o $(SHARED_LIB)

static:
	$(CC) $(NAME).c $(BASE_FLAGS) $(DEBUG_FLAGS) $(STATIC_CFLAGS) -c -o lib$(NAME).o
	@$(AR) $(STATIC_LIB) lib$(NAME).o
	@$(RM) libfcpp.o

main-shared:
	$(CC) main.c -L. -l$(NAME) $(BASE_FLAGS) $(DEBUG_FLAGS) -o $(NAME)

main-static:
	$(CC) main.c -L. -l$(NAME) $(BASE_FLAGS) $(DEBUG_FLAGS) $(STATIC_CFLAGS) -o $(NAME)-static

#-----------------------------------------#
AR = ar rs
RM = rm
