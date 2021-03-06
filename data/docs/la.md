A very small yet another programming language that compiles in C.

The main purpose of this project is to describe the semantics and syntax
of a Programming Language, composed with some of most common and very well
established and easy to recognize (as they exposed similarly to almost all
(or at least to those derived from C)), programming consepts. It tries to
incorporate the very minimum significant datatypes (strings, arrays, maps
(dictionaries which can be used to implement object oriented techniques),
with an obvious syntax and easy to get it in minutes.

The following is an early draft, but looks quite close to the final reference,
but with a couple of exceptions, restrictions and unhandled cases that would
be good to have.

There is a reference implementation, that should obey the syntax and semantics,
that is owe to pass all the tests, except some expected to fail ones, notably
operations on doubles, where an expertise is missing and neither exists the
desirable will to gain this knowledge. This can wait, as and as an author, i
never had to use the type, except in some quite basic calculations and these
few.
The reference interpreter is being used in two cases internally in this system.
This is to support saving and restore editor sessions and virtual window managment
sessions, which by alone already is precious.

The syntax and the semantics of the language, should feel familiar with already
established programming languages consepts, and should obey the principle of the
least surpise and should not violate expectations. If it does not or if it does,
this should be considered as a bug. It is important to note that this language,
doesn't bring even the singlest new consept in the programming language universe.
It is written for C and follows C where it makes sense, and had been influenced
a lot by the S-Lang programming language, which is quite like C.

## Syntax and Semantics
```js
# A `#' denotes the begining of a comment that ends up to the end of line.

# Variable assignment

  var i = 0
  var double = 1.1

  - a variable should be initialized at declaration time (in absence of a useful
    value, it can be initialized as `null`).

# Function declaration

  func name (arg) {
    return arg * 2
  }

 # this is synonymous with the above

   var name = func (arg) {
     return arg * 2
   }

# a function can be used as an argument to a function

# a function without an argument list can be declared as:

   func name { block }

# code blocks are delimited with braces '{}', and are mandatory

# statements are separated with a semicolon or with a new line character,
  and they can spawn to multiply lines based on the context.

# Conditionals:

# `if` conditional: the block is executed if the condition is true

  if (condition) { statement[s]... }

# `ifnot` conditional: the block is executed if the condition is zero

  ifnot (condition) { ... }

# both can get an `else` clause, that is evaluated when the prior conditional
  block hasn't been executed

# both can be extended with an `else if[not]` conditional block:

   } else if (condition) { block }

 or

   } else ifnot (condition) { block }

 if the prior conditional expression evaluated to zero

# Loops:

# `while` loop

  while (condition) { block }

# `for` loop (same semantics with C)

  for (init_statement[[s], ...]; cond ; ctrl_statement[[s], ...]) { block }

  Special forms of the `for` loop that work as an iterator, can be found to
  the specific datatype section.

# `loop` loop (loop for nth times)

  loop (num iterations) { block }

# alternatively

  loop (init_statement[[s], ...]; num iterations) { block }

# `forever` loop (like a for (;;) or while (true) in C)

  forever { block }

# alternatively

  forever (init_statement[[s], ...]) { block }

# `do/while` loop (same semantics with C)

  do { block } while (condition)

# a `break` statement breaks out of a loop

# a `continue` statement continues with the next iteration

# Constant types (those types can not be reassigned)

  const c = "constant"
  c = 1  # this should fail

# variables can not be redeclared at the current scope

  var v = 1
  var v = 100 # this should fail

# assignment with a string literal (multibyte (UTF-8) strings are handled)

  var str = "oh la la lala, it's the natural la"
  var mb  = "Είναι το φυσικό ΛΑ που φαίρνει η κάθε ύπαρξη"

# you can get the underlying byte by using an index and can be negative

  str[-2]

  again: this has byte semantics, though there isn't a certainity if it is
  the right thing to do, as it might make also sense to return a character,
  but at least it is the well known C way

# variables can be reassigned if they are not declared as `const`

  str = "that everyone brings"

  in that case, the previous value should be freed automatically by the compiler

# you can pass a string literal as an argument to a user defined function, or
  to a C function

  call_fun ("with a string literal as an argument")

  in this case this string should be freed automatically

# functions can get at most nine arguments (already too too many, so it looks
  like a waste)

# Comparison operators:

  == or is, != or isnot, >, < , >=, <=

  we use `is` and `isnot` and pay a price for this with en expensive switch block
  just to feel like humans, and really after years of using them in C and now
  in the language, it really offers a more natural humanish way to flow with
  the written code and the underlying thought, that for certain helps a lot
  as it gives clarity to the code

# Arithmetic operators:

  +, -, *, /, % and [*+-/%]=

# Logical operators:

  && or and, || or or

 again: the `and` and the `or` keywords, instead of && and ||, it adds clarity
 to the code, and shows clear the intention of a logical operation, which some
 of them sometimes can be complex. Also slowly the reading of the code becomes
 a real reading!  As languages that use a syntax that you need (at least at the
 begining) to desipher them first, and for as long it takes to become a second
 nature to write and especially read from another human, as it doesn't show at
 all intentions, or you might need to be a genious to understand them as fast
 as should be and not more.

# Bitwise operators:

  |, ^, >>, <<, and [|&]=

# all the operators should have the same semantics with C

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

# lambda function syntax:

  lambda ([([arg], ...)] {body}) ([arg], ...)

  var i = lambda ((x, y) {return x * y}) (10, 100)

# it is like a function declaration without a name, but enclosed in parentheses

# the parameter list can be omited if it is empty.

# if there is no argument list, this lambda can be assigned in a variable,
  but its lifetime can not be guarranteed yet, though it might work too.
  The `func` keyword can be used for that.

# lambdas, like functions, can be nested in arbitrary level

# loadfile syntax and semantics:

  loadfile ("fname")

  If `fname` is not an absolute path, then it is relative to the current
  evaluated unit. If that fails, then it is relative to the current directory,
  else it is relative to the `__loadpath` intrinsic string array variable. If
  the unit couldn't be found, then an error terminates execution.

# import syntax and semantics:

  import ("modulename")

  If `modulename` is not an absolute path, then it is relative to the current
  evaluated unit. If that fails, then it is relative to the current directory,
  else it composed in turn with the members of the `__importpath` intrinsic
  string array variable.
  If after the search, it couldn't be found then an import error terminates execution.

  The imported compiled module name is composed as `modulename`-module.so.
  A compatible unit should provide two functions with the following signatures:

    1. public int __init_modulename_module__ (la_t *);
    2. public void __deinit_modulename_module__ (la_t *);

  For convienence and at the top of the compilation unit, could be used the
  following:

    #define REQUIRE_LA_TYPE DECLARE
    #include <z/cenv.h>

  And then to the __init_modulename_module__(la_t *this) public function, the
  following macro:

    __INIT_MODULE(this);

  Note that for static builds the `import` function is still available, but it
  only checks if the desired interface has been exposed to the interpreter, thus
  it should include any desirable module on build time, and the initialization 
  for the module, should be done at runtime after any new instance. The `__importpath`
  intrinsic variable is still available but has no effect on static builds.

  Modules should expose a MapType with the same name with the module name, with
  the first character capitalized. This Map should expose the functions as its
  methods.

# print functions syntax:

  print[ln] ([file pointer object], "string ${expression} ...")

  file pointer can be either `stdout` or `stderr`, or any file pointer object
  that has been created with the fopen() function:

    var fp = fopen (filename, mode)  # same semantics with C

  Without a file pointer argument, default is to redirect to the standard output.

  interpolation expression syntax:

    ${[%directive], symbol or expression}

    - a directive can be optional and can be any of the followings:
      - %d as a decimal (this is the default, so it can be omited)
      - %s as a string
      - %p as a pointer address
      - %o as an octal (0 (zero) is prefixed in the output)
      - %x as a hexadecimal (0x is prefixed in the output)
      - %f as a double

# the `println()` function is like `print`, but also emits a new line character.

# Array declaration:

  # first form
    var ar = [1, 2, 3]

    this is an anonymous array that get assigned to the symbol `ar`. It takes
    its type of the first element, which in this case is an integer type.

    Such "on the fly" arrays can be used elsewhere in expressions, and they should
    be freed automatically.

  # second form
    Array declaration and assignment syntax with a predefined length and type:
    (note that with current code, this method should execute faster than the
    above, since the above requires first a "light" parsing, to determinate
    the length of the array, and then redo the "real" evaluation)

    var string[3] ar = ["holidays", "in", "cambodia"]

  - the declaration syntax is:

    var type[length] varname

    - a `type` can be one of the followings
      - integer
      - number
      - string
      - map
      - array

# Array assignment syntax:

  # first form
    ar_symbol[first index : [last index]] = [first index, [...,...,] last_index]

      where assignment starts at `first index` and stops at `last index`.
      If `last index` is omited then `last index` assumed (array length - 1).

      The number of expressions should match, or else it will result to an
      OUT_OF_BOUNDS error.

  # second form

    ar_symbol = [..., ...]

      in this case the mumber of expressions is considered the length of the
      array. If not, the interpreter will throw an OUT_OF_BOUNDS error.

  # you can set _all_ the array items, with a specific value using:

    ar[*] = value

    *Note* that this syntax should attributed to S-Lang programming language.

  # you can get or set an item from or to an array, using indices that can
    be negative.  Array indices are starting from zero, and -1 denotes the
    last item in the array.

  # A special form of the `for` loop, can be used as an array iterator:

    for |i| in array { block }

    In this case 'i' holds the index of the array, and which initially starts
    from zero.

# Maps. This is the only container type in the language, and which associates (maps)
  a "key" with a "value". This is a hybrid type, similar to associative arrays
  and structures in other languages and almost similar to json format. Note that
  this is an unordered list, and there is no guarrantee that keys and values, will
  will have the order of the declaration, when looping over a map.

  # Map Declaration

  var m = {}  # empty container

  var mm = {
    "one" : 1,
    "two" : "string",
    "1"   : 1,
    "fn"  : func (x) { return this.one * x },
    private "privatevar" : "accessible only by the members of the map"
  }

  - the `this` keyword is being used _only_ from the members of a map, and it
    provides access to the other fields, as it holds the value of the underlying
    map

  - keys are valid string identifiers and may start with a digit or an underscore

  # Accessing maps. This is done by using a dot ('.') after a symbol that refers
    to a Map Type.

    mm.one
    mm.fn (10)
    mm.privatevar = "something" # this should fail

  # extending maps

    mm.new = 1

  # in those cases those fields are accessible from their scope.
    Members can be attributed as `private` only at the construction time,
    not even within the members at runtime.

  # A special form of the `for` loop, can be used as an iterator that can
    loop over Maps:

    for |k, v| in map { block }

    In this case 'k' holds the key of the map, and 'v' its associated
    value.

    Note, since a Map is an unordered list, there is no guarrantee of the order.

# Types.
  These are user defined types, which are like Maps, with a couple of differences
  that are described below.

  A new type is defined by using the `Type` keyword, with an identifier that the
  leading character should be capitalized.

    Type Typename {  ... }

  A type is initialized by using the `New` keyword like:

    var s = New Typename ([init arguments])

  Then it can be called like a MapType.

  Types should provide an `init` method. This can be declared as `private` or as
  `public`. In the latter case the state can be reinitialized at the runtime
  by the user, without the `New` keyword.

  Declared fields are not obligated to provide a value. In this case these properties
  are initialized as `null`.

# Strings
  # Two special forms of the `for` loop, can be used as an iterator that can
    loop over strings:

    first form is iteration over the bytes:

      for |c| in str { block }

      in this case 'c' holds the integer value of the underlying byte

    second form is iteration over the characters:

      for |c, b, w| in str { block }

      in this case 'c' holds the integer value of the underlying byte,
      while 'b' holds the string representation and 'w' holds the cell
      width of the character

```

## keywords and Operators (reserved keywords):
```sh
# var         -  variable definition
# const       -  constant definition
#                a constant can not reassigned since initialization; an uninitialized
#                object is considered the one that has a value of `null`
# func        -  function definition
# lambda      -  lambda function
# Type        -  user defined type
# New         -  initializer for user defined types
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
# *           -  multiplication
# /           -  division
# %           -  modulo
# +           -  addition
#                for strings this is a concatenation operator. In that case
#                an integer operand, it has the value of the string representation
#                and may be valid, if it is in the UTF8 range. If it is not
#                the result is unspecified.
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
#                if the first operand is a string then:
#                  if the second operand is a string then appends this string
#                  else if the second operand is an integer type, then it
#                  appends a byte if it is within the ascii range or else a
#                  multibyte sequence to form the character
# -=          -  decrement variable and assign the result
# *=          -  multiply          -||-
# /=          -  divide            -||-
# %=          -  modulo            -||-
# |=          -  bit or            -||-
# &=          -  bit and           -||-

# Predefined types:
# integer, string, map, number, array (those actually will throw a bit different
# error (if it is attempted to use them as an identifier), as they are being used
# on array declarations)

# Standard Functions.
# print and println -  print functions
# format            -  format a string with the same semantics with the print[ln]
#                      functions
#                      args: string
# loadfile          -  load a filename for evaluation
#                      args: a filename
# import            -  load a compiled C module
#                      args: a module name
# exit              -  terminates evaluation of the current evaluated instance,
#                      as it deoes only returns control back to the caller. So
#                      it actually doesn't exits to the system environment.
#                      args: integer as an exit value
# typeof            -  type of a value
#                      args: object
#                      The returned Type can be any of the followings intege type
#                      constants:
#                      (Integer|Number|String|Array|Map|Object|[C]Function|Null)Type
# typeAsString      -  type of a value as string represantation
#                      args: object
#                      The returned string can be any of the above but formated
#                      as stings.
# typeofArray       -  type of an array value
#                      args: array
# typeArrayAsString -  type of an array value as string represantation
#                      args: array
# len               -  length of the object (for array, map and string types),
#                      note that this has byte semantics for string types
#                      args: object
# fopen             -  returns a file pointer
#                      args: filename, mode (C semantics)
# fflush            -  flush the specified stream
#                      args: file pointer
# fileno             - returns the underlying file descriptor of a stream or
#                      a file pointer object
#                      args: file pointer
# errno_string       - returns a string represantation of a system error
#                      args: error number
# errno_name         - returns a string represantation of a system error name
#                      args: error number

# Standard Integer Constants.
# ok         -  0
# notok      -  -1
# true       -  1
# false      -  0
# null       -  (void *) 0
#
# FILE Pointers Of Standard Streams.
# stdout
# stderr
# stdin
#
# Argument list variables
# __argc     - holds the length of the list, zero if it hasn't been set
# __argv     - string type array, that holds the items of the list if it
#              has been set and its length should correspond to `__argc`.

# Info variables
# __file__     - current evaluated filename. If a string is evaluated defaults
#                to "__string__"
# __func__     - current function name
# __loadpath   - string type array with directories as members, to lookup up when
#                loading scripts
# __importpath - string type array with directories as members, to lookup when
#                importing C modules

# C Modules
# They are initialized with the `import` function on shared targets, or as
# builtins on static targets. When importing a module, the leading char is
# in lower case, e.g., import ("path"). Those exposing a public variable of
# a MapType, that has the exact name but the leading char is capitalized.

  # Path Module Interface
    # StringType    Path.real (StringType path)
    # StringType[]  Path.split (StringType path)
    # StringType    Path.dirname (StringType path)
    # StringType    Path.extname (StringType path)
    # StringType    Path.basename (StringType path)
    # IntegerType   Path.is_absolute (StringType path)
    # StringType    Path.basename_sans_extname (StringTYpe path)

  # File Module Interface
    # MapType       File.stat (StringType file)
    # IntegerType   File.size (StringType file)
    # IntegerType   File.chown (StringType file, IntegerType uid, IntegerType gid)
    # IntegerType   File.chmod (StringType file, IntegerType mode)
    # IntegerType   File.exists (StringType file)
    # IntegerType   File.access (StringType file, IntegerType mode)
    # IntegerType   File.mkfifo (StringType file, IntegerType mode)
    # IntegerType   File.remove (StringType file)
    # IntegerType   File.rename (StringType src, StringType dest)
    # IntegerType   File.symlink (StringType src, StringType dest)
    # IntegerType   File.hardlink (StringType src, StringType dest)
    # StringType    File.readlink (StringType file)
    # StringType    File.mode_to_string (IntegerType mode)
    # StringType    File.mode_to_octal_string (IntegerType mode)

    # Constants
    # IntegerType   F_OK, X_OK, W_OK, R_OK
    # IntegerType   S_IRWXU, S_IRUSR, S_IWUSR, S_IXUSR
    # IntegerType   S_IRWXG, S_IRGRP, S_IWGRP, S_IXGRP
    # IntegerType   S_IRWXO, S_IROTH, S_IWOTH, S_IXOTH

  # String Module Interface
    # IntegerType   String.eq (StringType a, StringType b)
    # IntegerType   String.eq_n (StringType a, StringType b, IntegerType n)
    # IntegerType   String.cmp_n (StringType a, StringType b, IntegerType n)
    # StringType    String.advance (StringType str, IntegerType num)
    # StringType[]  String.tokenize (StringType str, StringType token)
    # StringType    String.character (IntegerType c)
    # NumberType    String.to_number (StringType str)
    # IntegerType   String.to_integer (StringType str)
    # StringType    String.byte_in_str (StringType str, IntegerType byte)
    # StringType    String.from_integer (IntegerType i, IntegerType base)
    # StringType    String.advance_on_byte (StringType str, IntegerType c)

  # Std Module Interface
    # IntegerType   Map.set (MapType map, StringType key, Value v)
    # ValueType     Map.get (MapType map, StringType key)
    # StringType[]  Map.keys (MapType map)
    # IntegerType   Map.remove (MapType map, StringType key)
    # IntegerType   Map.key_exists (MapType map, StringType key)

    # IntegerType[] Array.where (ArrayType ar, Value expression)

  # Term Module Interface
    # ObjectType    Term.new ()
    # IntegerType   Term.getkey (IntegerType fd)
    # IntegerType   Term.raw_mode (ObjectType term)
    # IntegerType   Term.sane_mode (ObjectType term)

  # Dir Module Interface
    # IntegerType   Dir.make (StringType dir, IntegerType mode)
    # IntegerType   Dir.remove (StringType dir)
    # IntegerType   Dir.change (StringType dir)
    # StringType    Dir.current ()
    # IntegerType   Dir.is_directory (StringType dir)
    # IntegerType   Dir.make_parents (StringType dir, IntegerType mode)
    # IntegerType   Dir.remove_parents (StringType dir)

# Library
  # Argparse Interface

    loadfile ("argparse")
    var argparse = New Argparse (num_options, argparse_flags, message)
    argparse.add (variableName, shortopt, longopt, description, type, arg_flags)
    var retval = argparse.process (argv, start_index)

    # argparse_flags exposed constants
      ARGPARSE_NO_VERBOSE
      ARGPARSE_DONOT_EXIT_ON_UNKNOWN
      ARGPARSE_DONOT_PRINT_AND_EXIT_ON_HELP
      ARGPARSE_SHORT_H_ISNOT_HELP

    # arg_flags exposed constants
      ARG_VALUE_REQUIRED
      ARG_VALUE_OPTIONAL

    # retval can be `ok` that denotes success, otherwise it returns `notok`.
      In the latter case, and if `verbose` has been set (on by default), it
      prints the error message, otherwise it can be retrieved as a string, by
      accessing the err_message property:

         argparse.err_message

      If argparse.exit is true then it denotes that a help argument has been
      given.

    # Results are stored in argparse.results and the value of the variableName
      can be retrieved by accessing the appropriate field:

         argparse.results.variableName

    # Types can be one (for now) of the followings:

      IntegerType, StringType, BooleanType

    # All the options that do not correspond to none of the given options, are
      stored in the argparse.argv, while the argparse.argc holds the length of
      that array of strings.

# Semantics

  - standard keywords and functions can not be redefined and reassigned

  - function arguments that are memory types (like strings, arrays and maps),
    are passed by reference and so can be modified by the callee function

  - valid identifiers are [_a-zA-Z] and may include digits after the leading
    byte (with an exception to map members that may start with a digit)

  - when assigning a map type or a subtype map to a variable from another map,
    creates a new copy of the map

  - a valid array can contain only members with the same type, except if it is
    an array of arrays, which can contain different types of arrays and which
    can be nested in arbitrary depth

  - arrays are one dimensional arrays (for simplification), though it is not
    hard to emulate multi dimensional ones if there is a symmetry

  - syntax errors are fatal and terminate execution

  - integer types can be specified in:
    - binary notation (base 2) that start with "0b" and consists with 0|1 characters
    - octal notation (base 8) that start with "0" and consists with [0-7] characters
    - base 10
    - hexadecimal notation (base 16) that start with "0[xX]" and consists with
      [0-9][a-f][A-F] characters

  - single characters can be specified as integers, when are enclosed in signle
    quotes:

      'a' => 97

    those doesn't limit for characters in the ASCII range, in that case it
    return the codepoint of the UTF8 byte sequence:

      'α' => 945

    They can be also specified in hexadecimal notation using this form:

      '\x{3b1}' => 945

   - multiline literal strings are enclosed also in double quotes, but they
     can continue to the next line if an escape '\' character is encountered
     before the new line character. In any other case it is considered an error.

   - variables can be declared in consecutive manner, separated with a comma
     and they can spawn to multiply lines, e.g.,:

       var i = 0, ii = 1,
         a = null,
         integer[2] b = [1, 2],
         m = {"key" : "value"}

# Lexical Scope and visibility order
  - standard scope (lookup for standard operators and internal functions first)

  - block scope (conditional statements and loops)

  - function scope

  - previous function scope -> ... -> ... -> global scope

  if the symbol can not be found, then the compiler throws an error.

  By default symbols are private to their local scope, unless the symbol has been
  declared as `public`. In that case the symbol belongs to `global` scope and
  should be visible from any scope.

  For Maps though it is the reverse. By default all the fields are accessible
  from their caller, unless they had been attributed as `private`. In that case
  all the followed fields also belong to the private scope and are accessible
  only from the functions of the map, by using the reserved `this` keyword that
  holds the value of the underlying map. The `public` keyword make them accessible
  again, with the same semantics with the above, that is, all the consecutive
  fields will be accessible outside of the map, untill a field will be attributed
  as `private`.

  This Map specific mechanism, is just for to hide internal implementation details,
  and to separate them from the exposed interface.

  In a nutshell, the `private` and `public` keywords can be used inside of a Map,
  or else the `public` keyword can be used to expose a symbol in the `global`
  namespace. The `private` attribute to a variable is reduntant, as this is the
  default.

# Comparisons.

  - a value of `null` it is the only that can be compared with all the other
    types for truethness

  - an ArrayType can be compared also with only other arrays

  - an IntegerType can be compared also with other integers and with a number,
    and vice versa

  - a StringType can be compared also with only other strings

# Limits

  - maximum number of function arguments is nine

  - maximum length of any identifier is 63 bytes

# Types (integer type constants)

  NullType    : (void *) 0 (declared as `null`)
  NumberType  : double
  IntegerType : integer (wide as ptrdiff_t)
  StringType  : string type (container that holds C strings)
  ArrayType   : array
  MapType     : map

# Aplication Programming Interface.

```C
  /* the following static variable and the corresponded macro, normally
   * has been defined by the cenv.h header unit, that is responsible
   * to create the environment, but is posted here for clarity */
  static  la_T __LA__;	
  #define La  __LA__.self

  la_T *LaN = __init_la__ ();
  __LA__ = *LaN;                       // assumed a main function here
  la_t *la = La.init_instance (LaN, LaOpts(.argc = argc, .argv = argv));

  char *bytes = "func f (n) {println (\"result is ${n * 2}\")}";
  int retval = La.eval_string (la, bytes);

  __deinit_la__ (&LaN);
```

  The whole interface is exposed as a structure with function pointers, at the
  `la.h` unit, that map to their corresponded private functions, as there isn't
  and probably will never be an API documentation.

# Quirks.

The core of the language it is pretty small, and tries to do as many with as
less possible, so specific components (like the tokenizer) are weak in places,
or simply refuse to add overhead to handle unexpected style.
So it can be considered as a strict in syntax language, that might enforces a
style. However it should be okey if practicing consistency.

- every token should be separated with at least a space, though it might work
  without that rule, but the parser can not handle all the cases, like in (x-1)
  where -1 is considered as a number and should be coded as (x - -1). It is
  actually the only known case that is not handled correctly (but they might be
  others). No matter the parser, it is believed that a space that it separates
  tokens, it adds visually clarity.

- recursive functions though they should work properly, can be easily overflow
  the stack, as the compiler doesn't perform any kind of tail call optimizations
  (this is a week point, and the worst is that this is quite difficult to change,
  so it is better to avoid code with recursive function calls beyond some thousand
  of calls, instead it is prefered the imperative version).

- the number type (typedef'ed as `double`) operations, need an expertise which
  is absent to the author. So this isn't going to change soon, but the mechanism
  it is there.

- normally releasing memory it happens automatically, but many cases are handled
  internally explicitly, as there is no a real mechanism underneath, just a very
  primitive one. However running the extensive test suite and the applications
  that use the language under valgring, it doesn't reveal any memory leaks, but
  this is just a sign that quite probably is false.

  again: this is just a reference implementation, and has no ambition other
  than to be a reference implemtation and be usefull which already is anyway.

# DEVELOPMENT

  - at this point (mid days of April of 2021), the natural route is to develop
    an ecosystem, and we're being faced with a dillema.

    As a start, we created a wrapper for the fopen() function. Instead of making
    a special `File Pointer` type, we choose to create an Object Type, that it
    might be used by others and for other purposes.

    But how this function or the others that will follow, are going to be used?

    There is the old fashioned C's boring imperative way:

      somefunction (...)

    or the more natural humanish way, expressed in (pseudo) code as:

              sleep           at-hot-dogs            cow
      Me.what.  eat -> where .       cook -> what . rice . with ("yellowroot", ...)
               work               parents           goat

    means, possibly chained functions, that act on a kind of type, or object,
    or an environment, while it is maintained a state of properties.

    In an internal implementation, this requires to pass as the first argument
    to the called method the specific object, and at the same time the functions
    should push something in the stack (usually the same object).

    I'm not sure if this humble machine can handle this complexity, and if we
    want to dedicate time to develop such a system, that should at least to be
    consistent. But it is appealling!

    Anyway, for functions like fopen(), this is not issue, as it is desirable
    to map one to one with the well known and established C interface.

And as the last note. This documentation written in a single unit, owes to be
enough and should cover all the aspects of the language. If it is not enough,
then it should be considered rather as an error of the language itself, as one
of the main purposes of this is to be exceptional easy to start writting within
minutes.
