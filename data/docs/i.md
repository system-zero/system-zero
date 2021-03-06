# A tiny language that is derived from [tinyscript](https://github.com/totalspectrum/tinyscript).

The main advantage of this interpreter, is that it can be modified and integrated
in any environment and desire. As far I know, there aren't public users nor  that
exists a normal language specification. The main usage of tinyscript is  embedded
memory constrained systems, so it had to be compact. The main core is little more
than a thousand of lines of code, but despite its compact size, it is capable  to
support even complex tasks, since it binds enough well with C.

This development stressed quite bit the original code to do things that it wasn't
destined to do, so to provide flexibility and a bit of a desired expressionism.
It is also uses dynamic allocations from the heap, while the original code  uses
a stack allocated memory arena at initialization, thus avoids any out of  memory
operations. It also provides multibyte support, lambda functions, signed integers,
the ability to pass and create literal strings, nested functions, new keywords and
common established operators, incompatible print function that supports ${symbol}
interpolation expressions, ..., and quite of many internal changes. This version
doesn't have any static declared symbols, as it creates and passes in every function
an indepented instance of the interpreter.

As such, tinyscript scripts, may not run without modifications, but not many.

Note, that it is probably a slow interpreter, as such there no major  expectations
and illusions, only practicability and joy, to have scripting in C applications, by
spending five minutes to learn a language.

The syntax is strict in places, to easy parsing and to do as much as possible with
as less we can. As such there are cases, that might be mishandled, and even if they
are valid syntax, they might be considered as an error, or vice versa.
Generally speaking, all tokens should be space separated, except  after an open  or
before a closed parenthesis.

*Note* that recursion can be guaranteed, but it is also dictated by system's stack
limits, so it is vulnerable to stack overflows. Thus do not have to be used, other
with algorithms, that are not going to go to big depth. Iterations versions are also
going to execute a lot faster by a big margin.

A [test unit script](../tests/interpreter_semantics.i) serves as a description of the
semantics of the language, while it is early to be considered as stable.

## Syntax and Semantics
```sh
# Comment

# Assignment
var i = 1

# `if` conditional: the block is executed if the condition is true
if (condition) { statement[s]... }

# `ifnot` conditional: the block is executed if the condition is zero
ifnot (condition) { ... }

# Both can  get an `else` clause, that is executed when the first block is not
# executed.

# Note that a statement ends up with a semicolon or with a new line character.
# A backslash `\` at the end of the line is a continuation  character, so  the
# statement continues in the next line. Note that in this case,  '\' should be
# followed immediately be a new line character, otherwise  it is considered an
# error.

# Note also, that blocks are enclosed in braces and are mandatory.

# `while` loop
while (condition) { ... }
# You can use `break` to break out of the loop and `continue` to continue with
# the next iteration.

# Function declaration.
func name ([arg,..]) { ... }

# You can pass a string literal as an argument to a user defined function, or to a
# C function. Those strings are being freed at the end of the evaluation or at the
# deinitialization.

# Assign to a string literal
var str = "oh lala"

# This can be freed and reassigned.
free (str)
str = "another"

# Redeclaring a variable is undefined behavior.
var i = 10
var i = 12
# It might work, but there is no guarrantee.

# Constant type (those types can not be reassigned)
const c = "constant"
# But they might be modified (for now), so not exactly a constant.

# Variables or Constants that are initialized with string literals, should be freed
# explicitly.

# The C-string interface offers some ways to create and manage C-strings. Those share
# many rules with C, and the same dangers too!

# Comparison operators:
# == or is, != or isnot, > or gt, < or lt, >= or ge, <= or le

# Arithmetic operators: +, -, *, /, % and [*+-/%]=

# Logical operators: && or and, || or or

# Bitwise operators: |, ^, >>, <<, and [|&]=

# All the operators should have the same semantics with C.
#
# Functions can be defined in arbitrary nested level, in fact a whole unit can be a
# function

func fu () {
  func fua () {
    func fub () {
    ...
    }
    ...
  }
  ...
}

# lambda
#   syntax: lambda ([([arg], ...)] {body}) ([arg], ...)
# It is like a function declaration without a name, enclosed in parentheses.
#
# print function
#  prints a "string ${sym} with interpolation expressions". It can optionally
#  take `stderr`, as its first argument, to redirect output to standard error
#  stream, instead of standard output. Note that `sym` must be in visible scope.
#  ${sym} expressions, can take an optional parameter to indicate a directive,
#  like ${%s, sym}. Valid directives are:
#    - %s to print the symbol as a C string
#    - %p to print the symbol as a pointer address
#    - %o to print the symbol as an octal (a 0 (zero) is prefixed in the output)
#    - %x to print the symbol as a hexadecimal (a 0x is prefixed in the output)
#    - %d to print the symbol as a decimal (this is the default, so it can be omited)
#  Note that there should be a space after the comma, otherwise it is considered
#  as an error.
#  If `sym` is enclosed in parentheses then it is considered and evaluated as an
#  expression, e.g., var i = 10; print ("i is ${%d, (i * 2)}\n")
```

## keywords and Operators.
```sh
# var     -  variable definition
# const   -  constant definition
# if      -  if conditional
# ifnot   -  ifnot conditional
# else    -  else clause
# while   -  while loop
# func    -  function definition
# return  -  return statement
# break   -  break statement
# continue-  continue statement
# true    -  1
# false   -  0
# OK      -  0
# NOTOK   -  -1
# *       -  multiplication
# /       -  division
# %       -  modulo
# +       -  addition
# -       -  subtract
# &       -  bit and
# |       -  bit or
# ^       -  bit xor
# >>      -  shift right
# <<      -  shift left
# and     -  logical and
# &&      -  likewise
# or      -  logical or
# ||      -  likewise
# is      -  equal
# ==      -  likewise
# isnot   -  not equal
# !=      -  likewise
# lt      -  less than
# <       -  likewise
# le      -  less or equal
# <=      -  likewise
# gt      -  greater than
# >       -  likewise
# ge      -  greater or equal
# >=      -  likewise
# +=      -  increment variable value and assign the result
# -=      -  decrement variable   -||-
# *=      -  multiply  variable   -||-
# /=      -  divide    variable   -||-
# %=      -  modulo    variable   -||-
# |=      -  bit or    variable   -||-
# &=      -  bit and   variable   -||-

# Functions
# not     -  !value
# bool    -  !!value
# free    -  release memory
# alloc   -  allocate memory
# array   -  create an integer array
# realloc -  reallocate memory

# for testing reasons a C string interface (Cstring Type from this distribution)
# cstring_new -  C-string new (allocate memory)
# cstring_cp  -  C-string copy
# cstring_eq  -  C-string equals
# cstring_eq_n-  C-string equals first nth (like strncmp)
# cstring_dup -  C-string duplicate
# cstring_cat -  C-string append (allocated string should be large enough)
# cstring_cmp_n - like strcmp
# cstring_substr - substring of a string
# cstring_bytelen - cstring length
# cstring_byte_mv - almost like memmove (stops at the first null byte)
# cstring_byte_cp - almost like memcpy (stops at the first null byte)
# cstring_byte_cp_all - like memcpy
# cstring_new_with - C-string new with a given string
# cstring_trim_end - trim from the end
# cstring_byte_in_str - like strchr
# cstring_bytes_in_str - like strstr
# cstring_byte_in_str_r - like strrchr
# cstring_byte_null_in_str - find the null byte

# Other bindings from this distribution, might be added in the future.
```

# Aplication Programming Interface.
```C
  #define I   In->self
  i_T *In = __init_i__ ();
  i_t *i = I.init_instance (In, IOpts());
  char *bytes = "func f (n) { var i = 10; print ("i is ${i}\n)}";
  retval = I.eval_string (i, bytes);
  __deinit_i__ (&In);
```
