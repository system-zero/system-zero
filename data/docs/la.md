A very small yet another programming language (yala) that compiles in C.

## Syntax and Semantics
```js
# comment

# Variable assignment

  var i = 0
  var double = 1.1

  - a variable should be initialized at declaration time (in absence of a useful
    value, it can be initialized as `none`).

# Function declaration

  func name (arg) {
    return arg * 2
  }

 - a function can be used as an argument to a function.

 - this is synonymous with the above

   var name = func (arg) {
     return arg * 2
   }

 - a function without an argument list can be declared as:

   func name { block }

- code blocks delimited with braces '{}' and are mandatory.

- statements are separated with a semicolon or with a new line character,
  and they can continue to the next line, if it is too long to fit on the
  screen.

# Conditionals:

# `if` conditional: the block is executed if the condition is true

  if (condition) { statement[s]... }

# `ifnot` conditional: the block is executed if the condition is zero

  ifnot (condition) { ... }

 - both can get an `else` clause, that is evalueated when the prior conditional
   block hasn't been executed.

 - both can be extended with an `else if[not]` conditional block:

     } else if (condition) { block }

   or

     } else ifnot (condition) { block }

   if the prior conditional block hasn't been executed.

# Loops:

# `while` loop

  while (condition) { block }

# `for` loop

  for (init_statement[[s], ...]; cond ; ctrl_statement[[s], ...]) { block }

# `loop` loop

  loop (num iterations) { block }

# alternatively

  loop (init_statement[[s], ...]; num iterations) { block }

# `forever` loop

  forever { block }

# alternatively

  forever (init_statement[[s], ...]) { block }

# `do/while` loop

  do { block } while (condition)

- the `break` statement breaks out of a loop.

- the `continue` statement continues with the next iteration.

# Constant types (those types can not be reassigned)

  const c = "constant"

# Variables can not be redeclared at the current scope.

# Assign with a string literal (multibyte (UTF-8) strings are handled).

  var str = "oh la la lala, it's the natural la"

  - you can get the underlying byte by using an index and can be negative

    str[-2]

    again: this has byte semantics, though there isn't a certainity if it is
    the right thing to do, as it might make also sense to return a character.

# This can be reassigned

  str = "that everyone brings"

  - in that case the previous value should be freed automatically

- you can pass a string literal as an argument to a user defined function, or
  to a C function.

- functions can get at most nine arguments.

# Comparison operators:

  == or is, != or isnot, >, < , >=, <=

# Arithmetic operators:

  +, -, *, /, % and [*+-/%]=

# Logical operators:

  && or and, || or or

# Bitwise operators:

  |, ^, >>, <<, and [|&]=


  - all the operators should have the same semantics with C.

# Functions can be defined in arbitrary nested level, in fact a whole unit can be a
  function

  func fu () {
    func fua () {
      func fub () {
      ...
      }
      ...
    }
    ...
  }

# lambda functions syntax:

  lambda ([([arg], ...)] {body}) ([arg], ...)

  var i = lambda ((x, y) {return x * y}) (10, 100)

  - it is like a function declaration without a name, enclosed in parentheses.

  - the parameter list can be omited if it is empty.

  - if there is no argument list, this lambda can be assigned in a variable,
    but its lifetime can not be guarranteed yet, though it might work too.
    The `func` keyword can be used for that.

  - lambdas like functions can be nested in arbitrary level.

#  print functions syntax:

  print[ln] ([file pointer object], "string ${expression} ...")

  - file pointer can be either `stdout` or `stderr`, or any file pointer
    object that was created with the fopen() function:

    fopen (filename, mode)

    which has the same semantics with C.

    - without a file pointer argument, default is to the standard output.

    - interpolation expression syntax:

      ${[%directive], symbol}

      or

      ${[%directive], (expression)}

      - a directive can be optional and can be any of the following:
        - %d as a decimal (this is the default, so it can be omited)
        - %s as a string
        - %p as a pointer address
        - %o as an octal (0 (zero) is prefixed in the output)
        - %x as a hexadecimal (0x is prefixed in the output)
        - %f as a double

  -  the `println()` is like `print`, but also emits a new line character.

# Array declaration syntax with direct assignment:

  array string ar[3] = "holidays", "in", "cambodia"

*NOTE* that this syntax might change lightly or use another variant in the
future.

  -the declaration syntax is:

    array [type] name[length]

    - a type can be omited and can be one of the following
      - integer (this is the default)
      - number
      - string

- in the language there is also a pointer type, that is an alias for the
  INTEGER_TYPE, and capable to hold a pointer address that can be passed
  to C functions.

  - the *current* assignment syntax for this form, is:

    ar_symbol[index] = ..., ..., ...

      where assignment starts at `index` and continues as long there are
      expressions, separated with comma, and as long it doesn't get out of
      bounds.

    - you can get an item from an array, using indices and can be negative:

      array[-2]

    - array indices are starting from zero, and -1 denotes the last item in
      the array

```

## keywords and Operators (reserved):
```sh
# var         -  variable definition
# const       -  constant definition
#                can not change state since initialization; an uninitialized
#                object is considered the one that has a value of `none`
# func        -  function definition
# lambda      -  lambda function
# array       -  array definition
# if          -  if conditional
# ifnot       -  ifnot conditional
# else        -  else clause
# else if     -  else if clause
# else ifnot  -  else ifnot clause
# while       -  while loop
# for         -  for loop
# loop        -  loop loop
# forever     -  forever loop
# do/while    -  do/while loop
# return      -  return statement
# break       -  break statement
# continue    -  continue statement
# exit        -  terminates evaluation
# *           -  multiplication
# /           -  division
# %           -  modulo
# +           -  addition
#                for strings this is a concatenation operator, an integer in that case
#                has character semantics
# -           -  subtract
# &           -  bit and
# |           -  bit or
# ^           -  bit xor
# >>          -  shift right
# <<          -  shift left
# and         -  logical and
# &&          -  likewise
# or          -  logical or
# ||          -  likewise
# is          -  equal
# ==          -  likewise
# isnot       -  not equal
# !=          -  likewise
# <           -  less than
# <=          -  less or equal than
# >           -  greater
# >=          -  greater or equal than
# +=          -  increment variable value and assign the result
#                if the first operand is a string then
#                  if the second operand is a string then appends this string
#                  else if the second operand is an integer type, then it
#                  appends a byte if it is within the ascii range or else a
#                  multibyte sequence to form the character
# -=          -  decrement variable   -||-
# *=          -  multiply  variable   -||-
# /=          -  divide    variable   -||-
# %=          -  modulo    variable   -||-
# |=          -  bit or    variable   -||-
# &=          -  bit and   variable   -||-

# Functions
# print and println -  print functions
# typeof            -  type of a value
#                      args: object
# typeAsString      -  type of a value as string represantation
#                      args: object
# typeofArray       -  type of an array value
#                      args: array
# typeArrayAsString -  type of an array value as string represantation
#                      args: array
# len               -  length of the object (for array and string types),
#                      note that this has byte semantics for string types
#                      args: object
# getcwd            -  returns the current working directory
#                      args:
# fopen             -  returns a file pointer
#                      args: filename, mode (C semantics)
# fflush            -  flush the specified stream
#                      args: file pointer

# those might change
# not               -  !value
# bool              -  !!value

# The following memory handling functions are not needed with current code.
# free    -  release memory
# alloc   -  allocate memory
# realloc -  reallocate memory

# Constant variables
# ok      - 0
# notok   - -1
# true    - 1
# false   - 0
# none    - (void *) 0
# Types
# (Integer|Number|String|Array|Object|Function|None)Type
#
# FILE pointers of standard streams
# stdout
# stderr
# (no standard input yet)
#
# Argument list variables
# __argc   - holds the length of the list, zero if it hasn't been set
# __argv   - string type array, that holds the items of the list if it has been set

# Info variables
# __file__ - current evaluated filename, if a string is evaluated defaults to "__string__"
# __func__ - current function name

# Semantics

  - standard keywords and functions can not be redefined and reassigned

  - function arguments that are memory types (like strings and arrays), are
    passed by reference

# Lexical Scope
  - standard scope (lookup for standard operators and functions first)

  - block scope (conditional statements and loops)

  - function scope

  - previous function scope -> ... -> ... global scope

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

- recursive functions though they should work properly, can be easily overflow
  the stack.

- the number type (typedef'ed as `double`) operations, need an expertise which
  is absent.

- normally releasing memory it happens automatically, but many cases are handled
  internally explicitly, as there is no a real mechanism underneath, just a very
  primitive one


# DEVELOPMENT

  - at this point (mid days of April of 2021), the natural route is to develop
    an ecosystem, and we're being faced with a dillema.

    As a start, we created a wrapper for the fopen() function. Instead of making
    a special `File Pointer` type, we choose to create an Object Type, that it
    might be used by others and for other purposes.

    But how this function or the others that will follow are going to be used?

    There is the old fashioned C's boring imperative way:

      somefunction (...)

    or the more natural humanish way, expressed in (pseudo) code as:

              sleep           at-hot-dogs            cow
      Me.what.  eat -> where .       cook -> what . rice . with ("yellowroot", ...)
               work               parents           goat

    means, possibly chained functions, that act on a kind of type, or object,
    or an environment, that is maintained a state of properties.

    In an internal implementation, this requires to pass as the first argument
    to the called method the specific object, and at the same time the functions
    should push something in the stack (usually the same object).

    I'm not sure if this humble machine can handle this complexity, and if we
    want to dedicate time to develop such a system, that should at least to be
    consistent. But it is appealling!

    Anyway, for functions like fopen(), this is not issue, as it is desirable
    to map one to one with the well known and established C interface.
