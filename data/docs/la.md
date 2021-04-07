A very small programming language that compiles in C.

## Syntax and Semantics
```sh
# comment

# Variable assignment (they should be initialized at declaration time)

var i = 0
var double = 1.1

# Function declaration

func name (arg) {
  return arg * 2
}

# A function can be assigned in a variable

var f = name

# and can be used as an argument to a function.

# `if` conditional: the block is executed if the condition is true

if (condition) { statement[s]... }

# Code blocks delimited with braces '{}' and are mandatory.

# Statements ending with a semicolon or with a new line character.
# The backslash '\' character followed by a new line character,
# denotes that the statement continues in the next line.

# `ifnot` conditional: the block is executed if the condition is zero

ifnot (condition) { ... }

# Both can get an `else` clause, that is executed when the first block is not
# executed.

# `while` loop

while (condition) { block }

# `for` loop

for (init_statement[[s], ...]; cond ; ctrl_statement[[s], ...]) { block }

# `loop` loop

loop (num iterations) { block }

# alternativelly

loop (init_statement[[s], ...]; num iterations) {block}

# `forever` loop

forever { block }

# alternativelly

forever (init_statement[[s], ...]) { block }

# The `break` keyword breaks out of a loop, while the `continue` keyword
# continues with the next iteration.

# Constant types (those types can not be reassigned)

const c = "constant"

# Variables can not be redeclared at the current scope.

# Assign to a string literal (multibyte (UTF-8) strings are handled).

var str = "oh la la lala, it's the natural la"

# This can be reassigned.

str = "that everyone brings"

# You can pass a string literal as an argument to a user defined function, or to a
# C function.

# Functions can get at most nine arguments.

# Comparison operators:

  == or is, != or isnot, >, < , >=, <=

# Arithmetic operators:

  +, -, *, /, % and [*+-/%]=

# Logical operators:

   && or and, || or or

# Bitwise operators:

   |, ^, >>, <<, and [|&]=

# All the operators should have the same semantics with C.

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
# The parameter list can be omited if it is empty. If there is no argument
# list, this lambda can be assigned in a variable, but its lifetime can not
# be guarranteed. The `func` keyword can be used for that.
# Lambdas like functions can be nested in arbitrary level.

#  print function
#  prints a "string ${sym} with interpolation expressions". It can optionally
#  take `stderr`, as its first argument, to redirect output to standard error
#  stream, instead of standard output. Note that `sym` must be in visible scope.
#  ${sym} expressions, can take an optional parameter to indicate a directive,
#  like ${%s, sym}. Valid directives are:
#    - %s to print the symbol as a string
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

# Array declaration and direct assignment:

array string ar[3] = "holidays", "in", "cambodia"

# The declaration syntax is: array [type] name[length].
# If type is omited then it defaults to INTEGER_TYPE.
# Valid types are: integer, number, string. In the language there is also a
# pointer type, that is an alias for the INTEGER_TYPE, and capable to hold an
# object, that its address can be passed to C functions.

# The assignment syntax is: ar_symbol[index] = ..., ..., ...
# Assignment starts at index and continues as long there are expressions,
# separated with comma, and as long it doesn't get out of bounds.

# Array indices are starting from zero.
```

## keywords and Operators.
```sh
# var     -  variable definition
# const   -  constant definition
# func    -  function definition
# lambda  -  lambda function
# array   -  array definition
# if      -  if conditional
# ifnot   -  ifnot conditional
# else    -  else clause
# while   -  while loop
# for     -  for loop
# return  -  return statement
# break   -  break statement
# continue-  continue statement
# exit    -  terminates evaluation
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
# print
# typeof  -  type of a value
# len     -  length of the object (for ARRAY and STRING types)
# not     -  !value
# bool    -  !!value

# The following memory handling functions are not needed with current code.
# free    -  release memory
# alloc   -  allocate memory
# realloc -  reallocate memory

# Aplication Programming Interface.

```C
  #define I   In->self
  i_T *In = __init_i__ ();
  i_t *i = I.init_instance (In, IOpts());
  char *bytes = "func f (n) { var i = 10; print ("i is ${i}\n)}";
  retval = I.eval_string (i, bytes);
  __deinit_i__ (&In);
```

# Quirks.

The core of the language it is pretty small, and tries to do as many with as
less possible, so specific components (like the tokenizer) are weak in places,
or simply refuse to add overhead to handle unexpected style.
So it can be considered as a strict in syntax language, that might enforces a
style. However it should be okey if practicing consistency.

- every token should be separated with at least a space, though it might work
  without that rule, but the parser can not handle all the cases, like in x-1
  where -1 is considered as a number.

- there is no "else if". In that case it should be coded as a new "if", like:
    if (expr) {
      ...
    } else {
      if (expr) {
        ...
      }
      ... and so on
    }
  The result is the same.

- recursive functions though they should work properly, can be easily overflow
  the stack.

- the number type (typedef'ed as `double`) operations, need an expertise which
  is absent.
