# A tiny language that is derived from [tinyscript](https://github.com/totalspectrum/tinyscript).

## Syntax, Semantics and Extensions to tinyscript. Note that because tinyscript is
intented to run on memory constrained environments, does not have neither a single
dependency.  This version uses dynamic allocation,  thus made  it  much  easier to
extend. Except its usefulness in internal applications, it is  being  used  mainly
to prototype syntax and semantics.

```js
# Comment

# Assignment
var i = 1

# `if` conditional: the block is executed if the condition is true
if (condition) { statement[s]... }

# `ifnot` conditional: the block is executed if the condition is zero
ifnot (condition) { ... }

# Both can get an `else` clause, that is executed when the first block is not
# executed.

# Note that a statement ends up with a semicolon or in a new line character.
# A backslash `\` at the end of the line is a continuation character, so the
# statement continues in the next line. Tinyscript does not have this feature.

# Note also, that blocks are enclosed in braces and are mandatory.

# `while` loop
while (condition) { ... }
# You can use `break` to break out of the loop and `continue` to continue with
# the next iteration. Both they are not present upstream.

# Function declaration.
func name ([arg,..]) { ... }

# You can pass a string literal as an argument to a user or to a C function.
# Tinyscript does not have this feature. Those strings are being freed at the
# end of the evaluation or at the deinitialization.

# Assign to a string literal (not at tinyscript).
var str = "alalal"

# This can be freed and reassigned.
free (str)
str = "another"

# Redeclaring a variable is undefined behavior.
var i = 10
var i = 12
# It might work, but there is no guarrantee.

# Constant type (those types can not be reassigned) (not at tinyscript).
const c = "constant"
# But they might be modified, so not exactly a constant.

# Variables or Constants initialized with string literals, should be freed explicitly.
# The C-string interface offers some ways to create and manage C-strings. Those share
# many rules with C, and dangers too!
```

## All the keywords and Operators.
```js
# var     -  variable definition
# const   -  constant definition (not at tinyscript)
# if      -  if conditional
# ifnot   -  ifnot conditional (not at tinyscript)
# else    -  else clause
# while   -  while loop
# println -  print numbers and a new line  (not at tinyscript)
# print   -  print numbers
# func    -  function definition
# return  -  return statement
# break   -  break statement (not at tinyscript)
# continue-  continue statement (not at tinyscript)
# true    -  1
# false   -  0
# OK      -  0 (not at tinyscript)
# NOTOK   -  -1 (not at tinyscript)
# *       -  product
# /       -  division
# %       -  modulo
# +       -  summary
# -       -  subtract
# &       -  bit and
# |       -  bit or
# ^       -  bit xor
# >>      -  shift right
# <<      -  shift left
# =       -  equal (not used)
# is      -  equal (not at tinyscript)
# <>      -  not equal (not used)
# isnot   -  not equal (not at tinyscript)
# <       -  less
# <=      -  less or equal
# >       -  greater
# >=      -  greater or equal

# Upstream extensions (from stdlib).
# not         -  !value
# bool        -  !!value
# free        -  release memory  (as ts_free)
# alloc       -  allocate memory (as ts_malloc)

# Extended functions.
# realloc     -  reallocate memory
# println_str -  print string and also emit a new line
# print_str   -  print string

# C string interface (Cstring Type from this distribution)
# cstring_new -  C-string new (allocate memory)
# cstring_cp  -  C-string copy
# cstring_eq  -  C-string equals
# cstring_eq_n-  C-string equals first nth (like strncmp)
# cstring_dup -  C-string duplicate
# cstring_cat -  C-string append (string should be large)
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
Note, that it should be very slow interpreter, but the core code is about 1000 lines,
and binds perfectly in C. Values are passed as pointers (casted to intptr_t).
