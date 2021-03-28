A small language that hopefully can been taught in minutes and compiles in C.

## Syntax and Semantics
```sh
# comment

# Variable assignment (they should be initialized at declaration)
var i = 1

# Function declaration
func name (arg) {
  return arg * 2
}

# A function can be assigned in a variable
var f = name
# and can be used as an argument to a function.

# Code blocks delimited with braces '{}' and are mandatory.
# Statements ending with a semicolon or with a new line character.
# The backslash '\' character followed by a new line character,
# denotes that the statement continues in the next line.

# `if` conditional: the block is executed if the condition is true
if (condition) { statement[s]... }

# `ifnot` conditional: the block is executed if the condition is zero
ifnot (condition) { ... }

# Both can get an `else` clause, that is executed when the first block is not
# executed.

# `while` loop
while (condition) { ... }
# You can use `break` to break out of the loop and `continue` to continue with
# the next iteration.

# Constant types (those types can not be reassigned)
const c = "constant"
# but they might be modified (for now), so not exactly a constant.

# Assign to a string literal
var str = "oh la la lala, it's the natural la"

# This can be freed and reassigned.
free (str)
str = "that everyone brings"

# Variables or Constants that are initialized with string literals, should be
# freed explicitly by the user.

# You can pass a string literal as an argument to a user defined function, or to a
# C function. Those strings are being freed at the end of the evaluation or at the
# deinitialization.

# Redeclaring a variable is undefined behavior.
var i = 10
var i = 12

# Comparison operators:
# == or is, != or isnot, >, < , >=, <=

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

# lambda functions
# syntax: lambda ([([arg], ...)] {body}) ([arg], ...)

var i = lambda ((x, y) {return x * y}) (10, 100)

# It is like a function declaration without a name, enclosed in parentheses.
# They can be nested, and though can be assigned in a variable, their lifetime
# can not be guarranteed.

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
#    - %f to print the symbol as a double
#  Note that there should be a space after the comma, otherwise it is considered
#  as an error.
#  If `sym` is enclosed in parentheses then it is considered and evaluated as an
#  expression,

var i = 10; print ("i is ${%d, (i * 2)}\n")
```

## keywords and Operators.
```sh
# var     -  variable definition
# const   -  constant definition
# func    -  function definition
# lambda  -  lambda function
# if      -  if conditional
# ifnot   -  ifnot conditional
# else    -  else clause
# while   -  while loop
# return  -  return statement
# break   -  break statement
# continue-  continue statement
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
# <       -  less than
# <=      -  less or equal than
# >       -  greater
# >=      -  greater or equal than
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

# Aplication Programming Interface.

C functions can get at most nine arguments.

```C
  #define I   In->self
  i_T *In = __init_i__ ();
  i_t *i = I.init_instance (In, IOpts());
  char *bytes = "func f (n) { var i = 10; print ("i is ${i}\n)}";
  retval = I.eval_string (i, bytes);
  __deinit_i__ (&In);
```
