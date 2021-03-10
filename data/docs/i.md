# A tiny language that is derived from [tinyscript](https://github.com/totalspectrum/tinyscript).

## Syntax and Semantics (early draft).
```sh
# Comment

# Assignment
var i = 1

# `if` conditional: the block is executed if the condition is true
if (condition) { statement[s]... }

# `ifnot` conditional: the block is executed if the condition is zero
ifnot (condition) { ... }

# Both can get an `else` clause, that is executed when the first block is not
# executed.

# Note that a statement ends up with a semicolon or with a new line character.
# A backslash `\` at the end of the line is a continuation character, so the
# statement continues in the next line.

# Note also, that blocks are enclosed in braces and are mandatory.

# `while` loop
while (condition) { ... }
# You can use `break` to break out of the loop and `continue` to continue with
# the next iteration.

# Function declaration.
func name ([arg,..]) { ... }

# You can pass a string literal as an argument to a user or to a C function.
# Tinyscript does not have this feature. Those strings are being freed at the
# end of the evaluation or at the deinitialization.

# Assign to a string literal
var str = "alalal"

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

# Variables or Constants initialized with string literals, should be freed explicitly.
# The C-string interface offers some ways to create and manage C-strings. Those share
# many rules with C, and dangers too!

# functions can be defined in arbitrary nested level, in fact a whole unit can be a
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

```
A [test unit script](../tests/interpreter_semantics.i) serves as a description.

*Note* that recursion can be guaranteed, but it is also dictated by system's stack
limits, so it is vulnerable to stack overflows. Thus do not have to be used, other
with algorithms, that are not going to go to big depth. Iterations versions are also
going to execute a lot faster by a big margin.

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

# Functions
# print   -  print a "string ${sym} with interpolation expressions"
             it can optionally take `stderr` as a first argument to output to `stderr`,
             instead of `stdout`. Note that `sym` must be in visible scope.
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
Note that because tinyscript is intented to run on memory constrained environments,
does not have neither a single dependency.  This version  uses  dynamic allocation,
thus made  it much  easier to extend. Except its usefulness in internal applications,
already as it is now, it is being used mainly to prototype syntax and semantics, and
to be familiar with consepts by experimenting.

Also note, that it is probably a slow interpreter, but the core is about 1000 lines
of code, and it binds quite well in C. As such there no expectations and illusions,
only practicability and joy to have C scripting, by spending five minutes to learn
a language.

# Aplication Programming Interface.
```C
  #define I   In->self
  i_T *In = __init_i__ ();
  i_t *i = I.init_instance (In, IOpts());
  char *bytes = "func f (n) { println (n+1)}";
  retval = I.eval_string (i, bytes);
  __deinit_i__ (&In);
```
