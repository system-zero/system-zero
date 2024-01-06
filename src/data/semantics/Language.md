This is a programming language, that was written (foremost) to be the scripting
language for this system (that can evaluate and run code at the runtime).

A free form nice syntax with the flexibility of common established semantics by now.
[a bad implementation with a bit of clever in places code - and an ugly first draft
of this current document]
As an excuse (which holds trueth), is that it is mostly a prototype (a useful one though),
and that the basic functionality was implemented in a couple of months.

The code that should support the below claims with accuracy lies at [language/l-semantics.](language/l-semantics.lai)
[Note that probably needs a consideration and minor revisions to match the exact latest
develpment and decisions]

[Also note that the implementation isnot the safest ever, as it is a hand written language
from someone, that never ever has done something like this before]

Note that the following it is the first implementation and the gaining conscience
should be applied in a next iteration. My opinion it is that this went a bit way
too far at once.

Basic DataTypes:

  - NullType      : (void *) 0 (declared as `null`)
  - NumberType    : double
  - IntegerType   : integer
  - StringType    : string type (container that holds C strings and understands utf8)
  - ArrayType     : array
  - MapType       : map (that resembles json)
  - ListType      : list (can hold any type, accessible as an array, implemented as a linked list)
  - FilePtrType   : FILE wrapper type
  - FdType        : FILDES wrapper type
  - BooleanType   : boolean
  - FunctionType  : user defined functions
  - CFunctionType : C functions
  - ObjectType    : C objects

keywords and Operators (reserved keywords):
```sh
#  var         -  variable definition
#  const       -  constant definition
#  func        -  function definition
#  Type        -  user defined type
#  New         -  initializer for user defined types
#  if          -  if conditional
#  ifnot       -  ifnot conditional
#  else        -  else clause
#  else if     -  else if clause
#  else ifnot  -  else ifnot clause
#  then/orelse -  control single conditional statements and expressions
#  while       -  while loop
#  for         -  for loop
#  forever     -  forever loop
#  repeat/until-  repeat/until loop
#  return      -  return statement
#  break       -  break statement
#  continue    -  continue statement
#  override    -  overriding attribute
#  *           -  multiplication
#  /           -  division
#  %           -  modulo
#  +           -  addition
#                 for strings this is a concatenation operator. In that case
#                 an integer operand, it has the value of the string representation
#                 and may be valid, if it is in the UTF8 range. If it is not
#                 the result is unspecified.
#  -           -  subtract
#  &           -  bit and
#  |           -  bit or
#  ^           -  bit xor
#  >>          -  shift right
#  <<          -  shift left
#  and         -  logical and
#  &&          -  likewise
#  or          -  logical or
#  ||          -  likewise
#  is          -  equal
#  ==          -  likewise
#  isnot       -  not equal
#  !=          -  likewise
#  <           -  less than
#  <=          -  less or equal than
#  >           -  greater
#  >=          -  greater or equal than
#  +=          -  increment variable value and assign the result
#                 if the first operand is a string then:
#                   if the second operand is a string then appends this string
#                   else if the second operand is an integer type, then it
#                   appends a byte if it is within the ascii range or else a
#                   multibyte sequence to form the character
#                 for arrays this appends the second operand to all the members
#                   of the array
#  -=          -  decrement variable value and assign the result (likewise with += for arrays)
#  *=          -  multiply                    -||-
#  /=          -  divide                      -||-
#  %=          -  modulo                      -||-
#  |=          -  bit or                      -||-
#  &=          -  bit and                     -||-
#  ^=          -  bit xor                     -||-
#  :=          -  pass the current value to the next function and assign the result
#  ok          - 0
#  notok       - -1
#  true        - 1
#  false       - 0
```
Comments.

  Single line comments that start with '# ', and end up to the end of the line.
  The space after the hash symbol it is (almost) significant,  as there is code
  that uses comments for application annotations or macros, so it can be easily
  misinterpreted.

    [Note about the significant whitespace:
      while it is mostly about a free form syntax (we appreciate indentation and
      it is practiced rigorously in our code (in C and in our language)), there is
      not any kind of enforcement: but we might use them to denote blocks in future,
      since consistency should be rewarded with a way

  Multiline comments that start with '##[', and end up with '##]'. Those can nest.

```js
  # Variable declaration:

  var v;
  # or
  var c

  # An explicit semicolon, denotes the end of a statement, but also a new line
  # can denote the end of a statement.

  # A statement, and based on the context it may span into multiply lines (even
  # complicated ones, though there is some sensitivity to match sane established
  # breaking points (almost standard way) - for such cases - but there isn't such
  # a strong guarantee that all (usually) corner combinations might work on such
  # a long statement, but such code has already been written and it works).

  # Multiply statements in one line without a semicolon at the end, may work (usually)
  # or may not work, as there are a couple of obvious ambiquities, so in that
  # cases a semicolon is required.

  # Note that it is preferable to leave out the semicolon and fix any potential
  # cases that are not handled.

  # If a variable is not initialized with some value at the declaration time,
  # as it happened above, it is initialized with the `null` value, so `v' and
  # `c' they have the value of `null`.

  # Give it a value:

  v = 1

  # variables can not be redeclared at the current scope

  var v = 100 # this should fail

  # You may assign multiply variables, as long they are separated with a comma:

  var xxx,
      yyy,
      ccc

  # Any symbol can be reassigned with a new value, unless it is attributed as
  # `const`. Symbols are associated with a value but do not have types, just
  # the inferred type of the associated value.

  const vv = 1

  # In any attempt to change value to constant types, the interpreter should
  # raise an error. However, it is possible to redeclare a symbol in the same
  # scope as `const`, when both old and new values point to the same C or user
  # defined function, which then it is considered as a function alias, so:

  const basename = path_basename

  # can be repeated usually in another compilation unit.
  # In all the other cases it is considered a fatal error.

  # As you may not know the value of a constant until the runtime, it may left
  # uninitialized, until the first time that will be initialized with a value
  # other than `null`, so:

  const cv

  # has the value of null and can be assigned. From that assignment point though it
  # can not be modified.


  # The following is a function declaration. Functions and function arguments
  # do not have types.
  func name (arg) {
    return arg * 2
  }

  # The function body, is delimited by a pair of braces '{}'.
  # In the above case the braces could be left out, as the function body consists
  # of one single statement, so this is valid:

  func name (arg) return arg * 2

  # Every block creates a new local scope, which is invisible to the outer scope,
  # and accessible only from the nested blocks.

  # Blocks are considered also the loops and conditional statements and expressions.

  # A Function can be redefined, unless it is attributed with the const
  # keyword.

  const func xx (x) return x * 2

  # If there are no function armuments the pair of '()' parens could be ommited.

  func sayhowl return "howl"

  # A function can be used as an argument to a function

  func ref (y) return y * 2
  func fun (f, y) return f (y)
  fun (ref, 11)  # => 22

  # Functions can be nested in arbitrary level:
  func fuc (a, b, c) {
    func fud (a, b, c) {
      return a + b + c
    }

    return fud (a, b, c)
  }

  # Functions can call themeselves:

  func fibo_tail (n, a, b) {
    return a ifnot n

    return b if n is 1

    return fibo_tail (n - 1, b, a + b)
  }

  # or

  func fibo_recursive (n) {
    return n if n < 2
    return fibo_recursive (n - 1) + fibo_recursive (n - 2)
  }

  # however the stack can be easily exhausted with some thousands of calls.
  # Thus it is not guarranteed. However the compiler performs a tail call optimization
  # if the function explicitly recall itself as `self`. However the return statement
  # __should__ be in a tail call position, and this is the only guarrantee.

  # In the above second function, the statement is not in a tail position, so it
  # can overflow with some thousand recursive calls.

  # But the first form it is a tail call, and so the return statement it can be
  # written as:

  return self (n - 1, b, a + b)

  # instead of:

  return fibo_tail (n - 1, b, a + b)

  # and so in this case, it can't overflow.

  # Functions always return a value and functions that don't return a value.
  # This value is `null` by default. For C functions this is guarranteed by the
  # function signature, which is always a VALUE type.

  # Functions can be called on the fly.

  var v = func (x, y) { return x * y } (10, 100) # => 1000

  # Such anonymous function, it is like a function without a name, but it is called
  # immediately. After the call releases its resources. It is illegal to omit the 
  # argument list after the body, even if it is an empty list, so a pair of parentheses
  # is obligatory after the body.

  # Such functions, can be nested in arbitrary level, though they can be complicated
  # to parse, but legal:

  var r = func (x, y) {
    var xl = x + y

    return func (k) {
      return k * 2
    } (x) +

    func (z) {
      var i =
        func (x) {
          return x + 100
        } (z)

      return (z * 2) + i
    } (xl)

  } (50, 100)

  println ("${r}") # => 650

  # Note that this interface is only used to develop instant logic, like the statement
  # expressions in C11 (though with arguments).


  # Statements and loops:

  # if/else if/else
  func ifelseif (x) {
    if x is "x" {
      return "x"
    } else if x is "y" {
      return "y"
    } else {
      return "xy"
    }
  }

  println (ifelseif ("")) # => xy

  # As it has been said the language supports an ifnot conditional, that
  # it is true when the condition evaluates to zero:

  ifnot 10 - 10 println ("zero")

  # for loop (C semantics)
  var sum = 1
  func forfun (x) {
    for (var i = 1; i <= x; i++) {
      sum += i
    }
    return sum
  }

  println (forfun (10)) # 55

  # likewise with a while:

  sum = 0
  func forwhile (x) {
    var i = 1
    while i <= x {
      sum += i
      i++
    }
  }

  # for nth times { block }

  sum = 1
  for 10 times sum *= 2
  println (sum)   # 1024

  # forever loop (like a for (;;) or while (true) in C):

  #  forever { block }

  # repeat/until loop

  func repeatuntil (x) {
    var i = 0
    repeat {
      sum += i
      i++
    } until i < x
  }

  print ("all the results should be ${sum}\n")

  # For all the cases the pair of '{}' braces are mandatory, unless their
  # body consists of a single statement or expression, so this is valid:

  for (var i = 0; i < 10; i++) println (i)

  # Above we saw that the print functions can use interpolation syntax
  # for formated strings, By default it determinates the convertion based
  # on the type of the value. But directives can be used also:

  # This is a map declaration which it is a memory managment type, so it is
  # associated with a memory address:

  var damap = { key : 1, second : "two" }
  println ("${%p, damap}") # this it will print the hexadecimal address of
  # the value.
    # The supported set of directives:
    #  - %d as a decimal
    #  - %s as a string
    #  - %p as a pointer address
    #  - %o as an octal (0 (zero) is prefixed in the output)
    #  - %x as a hexadecimal (0x is prefixed in the output)
    #  - %f as a double

  # Directives can be ommited, but the default it is %s for strings or %d for
  # anything else.

  # Maps

  # We saw above a map declaration. A MapType it is a container, that has the
  # the semantics of an associative array. That means a `key' it is associated
  # with a `value'.

  # A key can be defined in one of the following three ways:

  # - as a string.
  # - as an identifier. In this case it is constructed only by valid identifiers
  #   [_a-zA-Z] and digits.
  # - as an integer
  # - as an expression, using $(expession) form

  # A key length can be upto 255 bytes.

  # A value can be constructed by any valid expression, so it can be of any type.

  # A map declaration, consists of a series of a key and a value, separated
  # with a comma. The comma can be left out if the declaration continues to
  # the next line, like in the next code, which also saws some more properties.

  var identifier = "key"
  var dadamap = {
    private
    "private_prop" : "I'm invisible to the outer scope"
    metoo : "and visible only to the map methods."
    $(identifier) : "an expression as a key"

    public
    visible : "Now I'm visible again until the next private attribute."
    "again visible" : "Visibility it is still public."

    private
    back_to_privacy : "So and the next properties until a public attribute."
    it_is_still_private : 1

    public
    "exposed_fun" : func {
      println ("I am a function method, and I can see you all.")
      return this.private_prop + " " + this.metoo
      # Some Interpreted Languages they refer to this self object as self,
      # some as this. We use this, and which has sence only inside map methods,
      # at the time of the construction
    }
    summary : 0
  }

  # Testing for string equality for a public property.
  println (dadamap."again visible" is "Visibility it is still public.") # => 1

  # But this will raises an error:
  println (dadamap.back_to_privacy)
  # SYNTAX ERROR: back_to_privacy, symbol has private scope

  # Accessing map properties is through a dot ('.'), the same way C access its
  # structures.

  # You can append a property or a method to a map at runtime:

  dadamap.sumfun = func (x) {
    dadamap.summary += x
    return dadamap.summary
    # note that it is not legal to use "this" here, as in this case it
    # could be possible to access private members
  }
  println (dadamap.sumfun (10)) # => 10

  # But it is not possible to override a method. This it will raises an error:
  dadamap.sumfun = 100
  # SYNTAX ERROR: you can not override a method

  # Unless doing it explicitly:
  override dadamap.sumfun = 100
  println (dadamap.sumfun) # => 100

  # But how can you access map members, when the key is constructed at runtime?

  var x = "question!"
  dadamap.$(x) = " Does really has a value?"
  dadamap.$("answer") = "Dubious. But nothing is being lost forever."

  println (dadamap.$("question!"))
  println (dadamap.$("ans" + "wer")) # string concatenation

  # If it wasn't for that, we had to use 2 C map functions (setter/getter) from
  # the "std" module, for any of those expressions/statements.

  # There is a convienent way to loop over a map:

  for key, value in dadamap println ("${key} : ${value}")
  # The private fields, should not be printed in this case.

  # Strings

  # String literal are delimeted by double quotes ('"') or back quotes ('`').

  # Both are multi-line strings but the difference is that double quoted strings
  # are subject for backslash substitution.

  # In that case the characters are interpeted like:

  #  \a  audible bell
  #  \b  back feed
  #  \e  escaper
  #  \f  form feed
  #  \n  new line
  #  \r  carriage return
  #  \t  tab
  #  \v  vertical tab
  #  \\  backslash itself
  #  \"  to appear a double quote in double quoted strings
  #  \`  to appear a back quote in back quoted strings

  # Loop over strings.
  # Two special forms of the `for` loop, can be used as an iterator that can
  # loop over strings:

  # First form is iteration over the bytes:

  # for c in str { block }

  var byte_semantics = "byte samantics"
  for c in byte_semantics println (c)

  # in this case 'c' holds the integer value of the underlying byte.

  # Second form is iteration over the characters:

  # for c, b, w in str { block }

  var aristofanis = "Βρεκεκεκὲξ κοὰξ κοάξ"

  for c, v, w in aristofanis
    println ("character integer represantion |${c}|, as string |${v}|, cell width ${w}")

  # in this case 'c' holds the integer value of the underlying byte,
  # while 'v' holds the string representation and 'w' holds the cell
  # width of the character.

  # String literals are enclosed within double quotes. A double quote can be
  # included in the string, but it needs to be escaped by a backslash.
  var iamastring = "I start with a double quote ('\") and end up with a double
quote ('\"). I can interpret backslash escapes, such as new line \n
tab\t back feed \b ring a bell \a, form feed \n, vertical tab \v,
carriage return \r, escape \e or the backslash itself \\\\.
Probably this will be a very messy output."

  # You can access a string by using indices like in C. And again like C it has
  # byte semantics, that means it points to the underlying byte.

  var la = "la"
  println (la[0] is 'l') # => 1

  # Here also we see that individual characters can be enclosed in single quotes,
  # that they should point to the associated integer value, again like C does.

  # 'a' => 97

  # But unlike C, this is not limited to the ascii range. This works the same
  # in UTF-8 encoding:

  println ('α') # => 945

  # in this case the value is the codepoint of the UTF8 byte sequence.

  # They can also be specified in hexadecimal notation using this form:

  '\x{3b1}' => 945

  # There are also backquoted strings, that are interpreted litterally, and they
  # have attributes, that change the interpretation, like:

  var code_string_for_evaluation = `
    var v = "asdf"
    func f (x) {
      v += x
      return v
    }

    println (f ('g'))

  `S4

  eval (code_string_for_evaluation)

  # That string will be stripped by maximum 4 leading bytes of whitespace,
  # to maintain indentation.

  println (code_string_for_evaluation)

  # Another useful attribute is F (for format), that can interpret the string
  # as an interpolation expression.

  # Other attributes may added in future, but for those cases there is a
  # more expressive way implememted in the language, that can filter a value
  # through a series of function calls and expressions.

  # Arrays.

  var ar = ["a", "b"] # StringType array with two members

  # Arrays have a fixed size and type. In the above code those have been
  # determinated by the parsing. The first element gives the type. It is an
  # error to mix types in an array, except for memory managment types, like
  # strings or maps, that can have null elements.

  # Here is how you can declare an array with a fixed size, and associate it
  # with a type:

  var integer[4] int_ar

  # This creates an integer array with four elements. By default the value
  # of an integer array element is initialized to 0, while 0.0 is for double
  # arrays, an empty string for strings and null for other datatypes.
  # The following types are supported:

  #  integer
  #  number
  #  map
  #  string
  #  list
  #  array (arrays of arrays can be nested in an arbitrary depth)

  # Now it can be initialized with an algorithm:

  for (var i = 0; i < len (int_ar); i++) int_ar[i] = i

  # Here we've used the `len` C native function, that returns the length
  # of the datatypes. In that case it is the number of elements of the array,
  # for strings is the number of bytes, for maps is the number of keys.

  for (var i = 0; i < len (int_ar); i++) println (int_ar[i])

  # We can access array elements, like C does.
  # Indices start from zero and can be negative, where -1 points to the
  # last element. The interpreter should throw an out of bounds error when
  # the index is equal or greater than the length of the elements.

  # The same can be written more compactly like:

  for v in int_ar println (v)

  # if we need the index too, then:

  for i, v in int_ar println ("${i} : ${v}")

  # This is an array declaration and assignment syntax with a predefined length
  # and type at the same time. This should execute faster, as the parser do not
  # have to determinate the type and the size.

  var string[3] ar = ["holidays", "in", "cambodia"]

  # An array can be assigned to a range of indices:

  int_ar[0:3] = [1, 1, 1, 1] # explicit range [from first_idx to the last_idx] 

  # or

  int_ar[0:] = [1, 1, 1, 1]  # if the second idx is ommited, then it is assumed array length - 1

  # In any case, if the number of expressions doesn't match or any idx is >= length
  # the array, the interpreter should raise an OUT_OF_BOUNDS error.

  # For such cases a more short form exists:

  int_ar[*] = 1

  # This syntax should be attributed at SLang Programming Language.

  # Lists.
  # This datatype is like the ArrayType with the exception, that is heterogenous and
  # and it can hold any type.
  # It has the same (almost) array semantics, when accessing them or when iterate on them,
  # they differ only the way they are created, and which is using a pair of "{[" "]}"
  # tokens (note that "{[" or "]}", is one token so '{' must be followed by '[' and
  # ']' must be followed by '}'.

  # Create an empty list:

  var l = {[]}

  # append an item

  append 1 in l

  # and access it

  println (l[0])
  l[0] += 10

  # iterate over

  for v in l println (v)


  # If As Expression and if cond then do this orelse do that, kind of Code Expressions
  # and statements.

  # This is a dyadic operation, which is a "do this or do that" entity. An
  # orelse evaluates the next entity (the next dyadic operation). Evaluation
  # stops, when there are no more orelse to evaluate, or when a condition of
  # the executed dyadic operation found true. In this case the next _single_
  # statement is evaluated and no more than one.

  # When it is being used as an expression this litteraly is:
  # if cond then return expr orelse return sexpr, as it produces and returns
  # a value. Again a single expression is evaluated and no more than one.
  # Theoreticaly it is not possible to influence the outter environment within
  # an expression as it has not has such mechanics, though currently they can:
  #  - through a function call
  #  - with the ++,-- operators

  var condition = 0
  if condition then println ("not zero") orelse println ("zero")
  # The `end` can be ommited here, because it follows a new line.

  # Also an `orelse` can be ommited:
  ifnot condition then println ("zero")

  # It is illegal to declare a variable in such conditionals, as in such
  # case, it might produce side effects to the rest of the code.

  # It is also illegal to use all kind of loops or block operations that
  # create a new scope.

  # Those conditionals are dyadic operations and they can be followed by
  # other if[not]/then/[orelse]/[end], in a linear way:
    # if cond then
    #   if cond then
    #     ifnot cond ...

  # This exact syntax can be used as an expression. The only difference is
  # that instead of single statements, evaluates single expressions:

  # in a variable assignment:
  var x = null
  var v = if x is null then "null" orelse "notnull" # => "null"
  # in a function call argument list:
  func f (arg)  return arg * 2

  var res = f (if v isnot null then 1 orelse 0) # => an argument of value 1

  # to get an array index:
  var ar = [1, 2, 3]
  println (ar[if v is null then 0 orelse 2]) # => 3

  # to access a map key with an expression:
  var m = {"key" : 1, "f" : 2}
  println (m.$(if v isnot null then "f" orelse "key")) # => 1

  # as an operand to binary operations:
  var s = "Βρεκεκεκὲξ" + (if v is null then " κοὰξ" orelse "") +  " κοάξ"
  println (s) # => "Βρεκεκεκὲξ κοὰξ κοάξ"
  # note here that the expression is surrounded with parentheses, otherwise
  # without them, the `orelse` would continue with the add operation and the
  # result would be different (without the last κοάξ).

  # Chaining with a Sequence of Functions Calls and Continuational Expressions.

  # The language supports a mechanism, where the current value, becomes the
  # first argument to the next function in the chain, or the last result value.

  var r = ar: len () # => 3

  # Here what is assigned is the length of the array value. The len() function
  # it takes an object argument. In this case the `ar' value is pushed to the
  # stack and becomes the first argument.

  # Lets use some functions from the std-module.
  import ("std")

  func fdouble (d) return d * 2

  var rs = r: fdouble (): to_string (16)  # 0x6
  # here the `r' value is passed as an argument to the user defined function
  # fdouble(), and the result of the call is passed as a first argument to
  # function integer_to_string() from the std-module. This function gets a
  # second argument which is the base. Here is in base 16. Note that in this
  # case the underlying type it is prepended to the function name.

  # Any valid expression is a valid value.
  var s = "97"
  println (s: to_integer ()) # => 97
  println ('a' is s: to_integer ()) # => true
  var is_eq = "asdf": eq ("asdfg") #  => false
  var is_neq = "asdf": eq_n ("asdfg", 4) # => true
  println (("10": to_integer () * 12 + 52 - 24) : to_string (2)) # => 10010100
  var ary = "fa:fb:fc": tokenize (":") # => ["fa", "fb", "fc"]
  println ({"k" : 1, "l" : 2}: len ()) # => 2
  println ((1 is 100): to_string (10)) # "0"
  # (note that the first expression is surrounded with parentheses, otherwise
  # without them, a binary operation would be performed, between `1' and the
  # result of (100: to_string (10)).

  # Functions should accept at least one argument.

  # The value on the stack, can be an operand of a conditional expression,
  # which is an `if/then/[orelse]/end' construct. Here the `end` keyword is
  # obligatory to avoid ambiguities.

  var xm = {"k" : 1, "l" : 2 };
  println (xm.k: if x is 1 then x + 10 end) # => 11
  # here `x' is the symbol that is associated with the value on the stack,
  # and which it can be used at the evaluation.

  # change the value of the key and retry:
  xm.k = 100
  println (xm.k: if x is 1 then x + 10 end) # => 100
  # since the condition was false and there wasn't an `orelse`, the result
  # it is the current input value on the stack.

  # Qualifiers
  # The language supports a mechanism to pass additional information to the
  # called functions. This it has been copied by the SLang Programming Language,
  # and it works the same with user defined functions and with C functions.

  # This extra information gets passed with a map, within the expression list
  # on a function call, and when after the argument list it follows a semicolon:

  func q (x) {
    var v = qualifier ("key", 10)
    return v * x
  }
  # the `qualifier` function, first checks if a qualifier with the name "key"
  # exists. If it couldn't been found then it uses as default value the second
  # argument.

  println (q (100)) # => 1000
  println (q (100; key : 200)) # => 20000

  func qq (x) {
    var m = qualifiers ()
    if null is m then m = {key : 10}
    return x * m.key
  }
  # similarly, the `qualifiers` function, checks for a pending set of qualifiers,
  # and it returns null it there isn't, orelse a map.

  println (qq (100)) # => 1000
  println (qq (100; {"key" : 200}) # => 2000

  func qqq (x) {
    return qualifier_exists ("key")
  }
  # while the `qualifier_exists' function checks for the existance of a key,
  # and it returns `false` if `qualifiers' is null or the key doesn't exists
  # orelse returns `true`.

  # Syntax forms:
    # literal forms:
      # fun (args; {key : val, next_key : val})
      # fun (args; key : val, nextkey : next val)
    # expression forms:
      # fun (args; map_symbol)
      # fun (args; map_expression)

    # examples
    func q_fun (x) {
      var y = qualifier ("y", 100)
      return (qualifier ("key", 900) + y) * x
    }

    println (q_fun (10))               # => 10000
    println (q_fun (10; {key : 9900})) # => 100000
    println (q_fun (10; key : 9900))   # => 100000

    var m = {y : 1100}
    println (q_fun (10; m))            # => 20000

    func fm () {
      return {y : 2100}
    }
    println (q_fun (10; fm ()))        # => 30000

  # Again, this mechanism should be attributed to the SLang Programming Language,
  # and it is exposed with the exact interface.


  # User Defined Types.
  # These are like Maps, with a couple of differences that are described below.

  # A new type is defined by using the `Type` keyword. The leading identifier
  # character should be capitalized.

  Type Typename {
    private
    key
    init : func (x) this.key = x

    public
      fun : func return this.key
  }

  # A user defined type is initialized with the `New` keyword:

  var s = New Typename (100)

  # Then it can be called like a MapType:

  println (s.fun ()) # => 100

  # Types should provide an `init` method. This can be attributed as `private`
  # or as `public`. In the latter case the state can be reinitialized at the
  # runtime by the user, without the `New` keyword.

  # Declared fields are not obligated to provide a value on declaration
  # time. In this case these properties are initialized as `null`.

  # Control statements.

  # A continue statement into a body of a loop, continues with the next iteration.

  # A break statement returns control to an outter scope, of the current loop
  # state. This by default is one (the current one) loop level. However it can
  # be set explicitly (with a given `count` that follows the statement), to break
  # to `count` outer loop levels.

  for (var i = 1; i < 10; i++)
    for (var j = 0; j < 100; j++)
      if j is 5 break 2

  # this will pass control to the first loop level. In other words "break"
  # and "break 1" is synonymous.
  # If the given `count` is greater than the existing loop level, a syntax error
  # is raised. Maximum `count` is 9 nested loops.

  # (for both `continue` and `break`) if they are not into a loop state then a
  # syntax error is raised.

  # However both can be expressed in a boolean context. That means that they
  # get executed only if the condition is true:

  for (var i = 1; i < 10; i++) {
    continue ifnot i is 5
    println ("only ${i}")
  } # => "only 5"

  # It accepts only a single expression to evaluate, and it doesn't accepts
  # clauses. It works for both `if` and `ifnot`.

  # Finally, it has to be seen the same way as an `else if[not]`, so an `if`
  # or an `ifnot`, should follow `break` or `continue` to the same line.

  # The return statement

  # A return may appear in a function to return control to the caller of the
  # function.

  # A single return that is followed by a semicolon or a new line it breaks
  # execution without returning back a value. In this case the value is `null`.

  func ret1 return
  println (ret1 ()) # => null

  # A return that is followed by an expression and a semicolon or
  # a new line, returns the value of the expression.

  func ret2 () return true
  println (ret2 ()) # => true

  # A return that is followed by an if[not], it breaks function
  # execution without returning a value, if the next expression is
  # evaluated to true.

  func ret3 (x) {
    return if x is 10
    return [x]
  }

  println (ret3 (10)) # => null
  println (typeAsString (ret3 (1))) # => "ArrayType"

  # A return that is followed by a single token expression, which is
  # followed by an if[not], it returns the value of the expression
  # if the next expression is evaluated to true:

  func ret4 (x) {
    var y = x * 2
    return y if x is 10
    return y * 2
  }

  println (ret4 (10)) # => 20
  println (ret4 (100)) # => 400
  # note that a map property or method, or an array item it is not
  # a single token expression, so those are not valid code:
    #  return map.prop if true
    #  return array[0] if true

  # Prefix and postfix [in|de]crement operators ++, --:
  # Those should have the same semantics with C.
  # For arrays of Integer and Number types, and only for the postfix
  # operators, they [in|de]crement all the members of the array.

  # Lexical Scope and visibility order

  # - standard scope (lookup for standard operators and internal functions first)

  # - block scope (conditional statements and loops)

  # - function scope

  # - previous function scope -> ... -> ... -> global scope

  # if the symbol can not be found, then the compiler raises an error.

  # By default symbols are private to their local scope, unless the symbol has been
  # declared as `public`. In that case the symbol belongs to `global` scope and
  # should be visible from any scope.

  # For Maps though it is the reverse. By default all the fields are accessible
  # from their caller, unless they had been attributed as `private`. In that case
  # all the followed fields also belong to the private scope and are accessible
  # only from the functions of the map, by using the reserved `this` keyword that
  # holds the value of the underlying map. The `public` keyword make them accessible
  # again, with the same semantics with the above, that is, all the consecutive
  # fields will be accessible outside of the map, untill a field will be attributed
  # as `private`.

  # This Map specific mechanism, is just for to hide internal implementation details,
  # and to separate them from the exposed interface.

  # In a nutshell, the `private` and `public` keywords can be used inside of a Map,
  # or else the `public` keyword can be used to expose a symbol in the `global`
  # namespace. The `private` attribute to a variable is reduntant, as this is the
  # default.

  # When accessing submaps using a colon (':') then the `this` is a reference to
  # the submap:

   map.submap:method (...)

  # Print functions syntax:

  #  print[ln] ([file pointer object], "string ${expression} ...")

  #  file pointer can be either `stdout` or `stderr`, or any file pointer object
  #  that has been created with the fopen() function:

  var fp = fopen (filename, mode)  # same semantics with C

  # Without a file pointer argument, default is to redirect to the standard output.

  # Comparisons.

  # - a value of `null` it is the only that can be compared with all the other
  #   types for truethness

  # - an ArrayType can be compared also with only other arrays

  # - an IntegerType can be compared also with other integers and with a number,
  #   and vice versa

  # - a StringType can be compared also with only other strings

  # Limits and semantics

  # - standard keywords and functions can not be redefined and reassigned

  # - maximum length of any identifier is 255 bytes

  # - functions can get at most nine arguments

  # - function arguments that are memory types (like strings, arrays and maps),
  #   are passed by reference and so can be modified by the called function

  # - valid identifiers are [_a-zA-Z] and may include digits after the leading
  #   byte (with an exception to map members that may start with a digit)

  # - a valid array can contain only members with the same type, except if it is
  #   an array of arrays, which can contain different types of arrays and which
  #   can be nested in arbitrary depth

  # - syntax errors are fatal and terminate execution

  # Integer types can be specified in:
  #  - binary notation (base 2) that start with "0b" and consists with 0|1 characters
  #  - octal notation (base 8) that start with "0" and consists with [0-7] characters
  #  - base 10
  #  - hexadecimal notation (base 16) that start with "0[xX]" and consists with
  #    [0-9][a-f][A-F] characters

  # finally:

  non_existing_function ()

  # this should throw an error, and it should terminate execution of the current
  # interpreter instance, with an error constant less than zero (a zero value
  # indicates success).

  # The interpreter in that case, it should print (to the standard error by default),
  # a message that will explain the error, and then it should try to print the
  # error line and with some lines offset, that raised the error.

  # All errors should propagated internally, from the current error point, back
  # to the very first function that started the evaluation. There is no kind of
  # exception handling. There are some thoughts, that the only thing that can
  # be supported probably and has some sence, it is a mechanism that could pause
  # execution to the try breaking point and then to provide an interactive session.
  # At that try point, the state has to be saved first. The mechanism will have
  # to expose internal information, about values or for evaluation parsing points
  # or function bodies. And finally it has to support an interactive session, with
  # options to abort, debug or even to... re-evaluate, by providing even the failed
  # function with a new body. This worths some invenstment.
```

# imclude syntax and semantics:

  include ("fname")

  If `fname` is not an absolute path, then it is relative to the current
  evaluated unit. If that fails, then it is relative to the current directory,
  else it is relative to the `__loadpath` intrinsic string array variable. If
  the unit couldn't be found, then an error terminates execution.

# evalfile syntax and semantics:

  var val = evalfile ("fname")

  evalfile is like include, with some differences described below.

  Files that are loaded with `evalfile` always reevaluated, thus they can
  provide different code.

  Such units should provide a return statement that return a value, like they
  were functions.

  This is like Lua:

    var m = {}
    m.f = func (...) { ... }
    return m

  Such units are always freed after evaluation, so they should return memory
  managment types that survive from the releasing.

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

```

# Standard Functions.

# print and println -  print functions
# format            -  format a string with the same semantics with the print[ln]
#                      functions
#                      args: string
# include           -  include a compilation file unit
#                      args: a filename
# evalfile          -  immediately evaluate filename contents
#                      args: a filename
# import            -  load a compiled C module
#                      args: a module name
# exit              -  terminates evaluation of the current evaluated instance,
#                      as it deoes only returns control back to the caller. So
#                      it actually doesn't exits to the system environment.
#                      args: integer as an exit value
# qualifiers           returns current set of qualifiers
#                      args:
# qualifier_exists     returns a boolean value
#                      args: a key name
# qualifier            returns the value of a qualifier
#                      args: a key name and a default value
# typeof            -  type of a value
#                      args: object
#                      The returned Type can be any of the followings intege type
#                      constants:
#                      (Integer|Number|String|Array|Map|Object|[C]Function|Null)Type
# typeAsString      -  type of a value as string represantation
#                      args: object
#                      The returned string can be any of the above but formated
#                      as strings.
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
# is_defined         - returns true if the given symbols has been defined, or false otherwise 
#                      args: a string with the symbol name
# errno_string       - returns a string represantation of a system error
#                      args: error number
# errno_name         - returns a string represantation of a system error name
#                      args: error number
# set_errno          - sets errno and returns the error number
#                      args: error number

# Standard Integer Constants.
# ok         -   0
# notok      -  -1
# true       -   1
# false      -   0
# null       -   (void *) 0

# Standard variables
# errno      - holds current errno number

# FILE Pointers Of Standard Streams.
# stdout
# stderr
# stdin

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
# a MapType, that has the exact name, but the leading char is capitalized.

  # Std Module
    # IntegerType   Map.set (MapType map, StringType key, Value v)
    # ValueType     Map.get (MapType map, StringType key)
    # StringType[]  Map.keys (MapType map)
    # IntegerType   Map.remove (MapType map, StringType key)
    # IntegerType   Map.key_exists (MapType map, StringType key)

    # BooleanType   Array.any (ArrayType ar, Value expression)
    # StringType    Array.join (StringType Array ar, StringType or NullType delim)
    # ArrayType     Array.sort (ArrayType ar)
    # IntegerType[] Array.where (ArrayType ar, Value expression)
    # ArrayType     Array.prepend (ArrayType ar, Value expression)

    # IntegerType   String.eq (StringType a, StringType b)
    # IntegerType   String.eq_n (StringType a, StringType b, IntegerType n)
    # IntegerType   String.cmp (StringType a, StringType b)
    # IntegerType   String.cmp_n (StringType a, StringType b, IntegerType n)
    # StringType    String.advance (StringType str, IntegerType num)
    # StringType[]  String.tokenize (StringType str, StringType token)
    # NumberType    String.to_number (StringType str)
    # StringType    String.to_upper (StringType str)
    # StringType    String.to_lower (StringType str)
    # IntegerType[] String.to_array (StringType str)
    # IntegerType   String.to_integer (StringType str)
    # StringType    String.byte_in_str (StringType str, IntegerType byte)
    # StringType    String.delete_substr (StringType str, StringType substr)
    # StringType    String.advance_on_byte (StringType str, IntegerType c)
    # StringType    String.advance_after_bytes (StringType str, StringType bytes, IntegerType repeat)
    # StringType    String.trim_byte_at_end (StringType a, StringType b)

    # IntegerType   Integer.eq (IntegerType a, IntegerType b)
    # StringType    Integer.char (IntegerType c)
    # StringType    Integer.to_string (IntegerType i, IntegerType base)

  # Path Module
    # StringType    Path.real (StringType path)
    # StringType[]  Path.split (StringType path)
    # StringType    Path.concat (StringType a, StringType b)
    # StringType    Path.dirname (StringType path)
    # StringType    Path.extname (StringType path)
    # StringType    Path.basename (StringType path)
    # IntegerType   Path.is_absolute (StringType path)
    # StringType    Path.basename_sans_extname (StringTYpe path)

  # File Module
    # IntegerType   File.new (StringType file, IntegerType mode)
    # MapType       File.stat (StringType file)
    # MapType       File.lstat (StringType file)
    # StringType    File.mode (StringType file)
    # IntegerType   File.size (StringType file)
    # IntegerType   File.chown (StringType file, IntegerType uid, IntegerType gid)
    # IntegerType   File.chmod (StringType file, IntegerType mode)
    # IntegerType   File.write (StringType file, StringType str)
    # IntegerType   File.append (StringType file, StringType str)
    # IntegerType   File.exists (StringType file)
    # IntegerType   File.is_rwx (StringType file)
    # IntegerType   File.is_reg (StringType file)
    # IntegerType   File.is_lnk (StringType file)
    # IntegerType   File.is_fifo (StringType file)
    # IntegerType   File.is_sock (StringType file)
    # TmpnameType   File.tmpname () it can be used as argument to write and read functions
    # IntegerType   File.is_readable (StringType file)
    # IntegerType   File.is_writable (StringType file)
    # IntegerType   File.is_executable (StringType file)
    # StringType[]  File.readlines (StringType file)
    # IntegerType   File.writelines (StringType file, StringType[] array)
    # StringType    File.read_num_bytes (StringType file, IntegerType num)
    # IntegerType   File.access (StringType file, IntegerType mode)
    # IntegerType   File.mkfifo (StringType file, IntegerType mode)
    # IntegerType   File.symlink (StringType src, StringType dest)
    # IntegerType   File.hardlink (StringType src, StringType dest)
    # StringType    File.readlink (StringType file)
    # StringType    File.type_to_string (IntegerType mode)
    # StringType    File.mode_to_string (IntegerType mode)
    # StringType    File.mode_to_octal_string (IntegerType mode)
    # IntegerType   File.copy (StringType src, StringType dest; [qualifiers])
       qualifiers: (they are trying to mimic cp(1) options)
         force: [0|1], update: [0|1], backup: [0|1], preserve: [0|1|2],
         recursive: [0|1], dereference  [0|1],
         interactive: [0|1] if it is set, it turns off `force',
         verbose: [0|1|2|3] 1: errors 2: like cp(1) 3: with a percent indicator
         all: same as preserve and recursive
    # IntegerType   File.remove (StringType file)
       qualifiers:
         force: [0|1], recursive: [0|1], interactive: [0|1], verbose: [0|1]
    # IntegerType   File.rename (StringType src, StringType dest)
       qualifiers:
         force: [0|1], interactive: [0|1], verbose: [0|1], backup: [0|1]

    # Constants
    # IntegerType   F_OK, X_OK, W_OK, R_OK
    # IntegerType   S_IRWXU, S_IRUSR, S_IWUSR, S_IXUSR
    # IntegerType   S_IRWXG, S_IRGRP, S_IWGRP, S_IXGRP
    # IntegerType   S_IRWXO, S_IROTH, S_IWOTH, S_IXOTH

  # Term Module
    # ObjectType    Term.new ()
    # IntegerType   Term.getkey (IntegerType fd)
    # IntegerType   Term.raw_mode (ObjectType term)
    # IntegerType   Term.sane_mode (ObjectType term)

  # Dir Module
    # IntegerType   Dir.make (StringType dir, IntegerType mode)
    # IntegerType   Dir.remove (StringType dir)
    # IntegerType   Dir.change (StringType dir)
    # StringType    Dir.current ()
    # IntegerType   Dir.is_directory (StringType dir)
    # IntegerType   Dir.make_parents (StringType dir, IntegerType mode)
    # IntegerType   Dir.remove_parents (StringType dir)

  # Os Module
    # StringType    Os.arch ()
    # StringType    Os.platform ()
    # IntegerType   Os.sleep (NumberType secs)
    # IntegerType   Os.getuid ()
    # IntegerType   Os.getgid ()
    # IntegerType   Os.getpid ()
    # IntegerType   Os.setenv (StringType name, StringType value, IntegerType overwrite)
    # StringType    Os.getenv (StringType name)
    # IntegerType   Os.unsetenv (StringType name)
    # StringType[]  Os.environ ()
    # StringType    Os.getpwdir (IntegerType uid)
    # IntegerType   Os.getgrgid (StringType name)
    # IntegerType   Os.getpwuid (StringType name)
    # StringType    Os.getgrname (IntegerType gid)
    # StringType    Os.getpwname (IntegerType uid)

  # Sys Module
    # StringType    Sys.get (StringType sys_setting)
    # IntegerType   Sys.set (StringType asname, StringType sys_setting, IntegerType replaceanyold) 
    # StringType    Sys.which (StringType exec)

  # Time Module
    # IntegerType   Time.now ()                               # time(NULL)
    # MapType       Time.gmt (null or IntegerType time)       # gmtime()
    # MapType       Time.local (null or IntegerType time)     # localtime()
    # StringType    Time.to_string (null or IntegerType time) # ctime()
    # IntegerType   Time.to_seconds (MapType tm)              # mktime()
    # StringType    Time.format (StringType fmt, null or MapType tm) # strftime()

  # Crypt Module
    # StringType   Crypt.md5sum (StringType str)
    # StringType   Crypt.sha256sum (StringType str)
    # StringType   Crypt.sha512sum (StringType str)
    # StringType   Crypt.md5sum_file (StringType file)
    # StringType   Crypt.sha256sum_file (StringType file)
    # StringType   Crypt.sha512sum_file (StringType file)
    # StringType   Crypt.base64_encode (StringType str)
    # StringType   Crypt.base64_decode (StringType str)
    # StringType   Crypt.base64_encode_file (StringType str)
    # StringType   Crypt.base64_decode_file (StringType str)
    # StringType   Crypt.hash_passwd (StringType passwd) # Bcrypt implementation
    # IntegerType  Crypt.verify_passwd_hash (StringType passwd, StringType hash)

  # Rand Module
    # IntegerType  Rand.new ()
    # StringType   Rand.get_entropy_bytes (IntegerType size)

  # Io Module
    # FdType       Io.open (StringType filename, IntegerType flags, IntegerType mode)
    # IntegerType  Io.isatty (FdType fd)
    # StringType   Io.fd.read (FdType fd)
    # IntegerType  Io.fd.write (FdType fd, StringType buf)

    # Constants:
    # IntegerType O_RDONLY, O_WRONLY, O_RDWR
    # IntegerType O_CREAT,  O_APPEND, O_TRUNC
    # IntegerType O_NOCTTY, O_CLOEXEC, O_NONBLOCK, O_EXCL, O_DIRECTORY, O_NOFOLLOW
    # and if defined
    # IntegerType O_LARGEFILE, O_SYNC, O_ASYNC

    # those are defined at file module also
    # IntegerType S_IRWXU, S_IRUSR, S_IWUSR, S_IXUSR,
    # IntegerType S_IRWXG, S_IRGRP, S_IWGRP, S_IXGRP,
    # IntegerType S_IRWXO, S_IROTH, S_IWOTH, S_IXOTH

  # Url Module
    # MapType Url.parse (StringType url)
      # .scheme   : StringType  f|ht|tp|s
      # .host     : StringType  host
      # .path     : StringType  path
      # .fragment : StringType  #fragment
      # .query    : StringType  ?query
      # .user     : StringType  user
      # .pass     : StringType  passwd
      # .port     : IntegerType port

  # Dl Module
    # MapType Dl.dump_dependencies (StringType object)
      # .dependencies : StringType[]
      # .endian       : IntegerType endianess
      # .bits         : IntegerType (64|32) bits
    # Constants:
      # LITTLE_ENDIAN
      # BIG_ENDIAN


  # Sun Module
   # StringType Sun.compute ()
     # qualifiers
       # verbose : [0|1], year : IntegerType, month : IntegerType,
       # day : IntegerType, lon : NumberType, lat : NumberType, utc : IntegerType

  # Moon Module
   # StringType Moon.phase ()
     # qualifiers
       # verbose : [0|1], year : IntegerType, month : IntegerType,
       # day : IntegerType

  # Below modules are outside the standard interface and require the Make utility
  # to build.

  # Re Module
   # IntegerType Re.match (StringType string, StringType pat)

  # Search Module
   # IntegerType|NullType|ArrayType Search.file (StringType file, StringType pat)
     # qualifiers
       # recursive : [0:1]
       # max_depth : IntegerType
       # without_filename : StringType
       # with_line_number : [0:1]
       # tostdout : [0:1]
     # this is like grep options
     # ifnot tostdout, then the returned value is either NULL or an array

  # Uuid Module
   # StringType Uuid.generate ()

  # Find Module
   # ArrayType|IntegerType Find.dir (StringType dir, StringType type)
     # qualifiers
       # max_depth : IntegerType
       # append_indicator : [0|1]
       # show_hidden : [0:1]
       # long_format : [0:1]
       # reverse : [0:1]
       # sort_by_mtime : [0:1]
       # sort_by_atime : [0:1]
       # sort_by_ctime : [0:1]
       # sort_by_size  : [0:1]
       # tostdout  : [0:1]
       # realpath  : [0:1]
       # match_uid : [0:1]
       # match_gid : [0:1]
       # only_executables : [0:1]

     # this is like find options plus extensions
     # ifnot tostdout, then the returned value is either NULL or an array

  # Syncdir Module
   # IntegerType Sync.dir (StringType src, StringType dest)
     # qualifiers
       # verbose : [0|1]
       # interactive : [0:1]
       # dryrun : [0:1]
       # exclude_dirs  : ArrayType
       # exclude_files : ArrayType

  # Pager Module
   # ObjectType (pager) Pager.new (ArrayType lines)
     # qualifiers
       # first_row  : IntegerType
       # first_col  : IntegerType
       # last_row   : IntegerType
       # last_col   : IntegerType
       # tabwidth   : IntegerType
       # has_statusline : [0:1]
       # term       : ObjectType term

   # ObjectType Pager.new_from_stdin (ArrayType lines)
     # qualifiers likewise with above

   # IntegerType Pager.main (ObjectType pager)

  # Archive Module
   # IntegerType Archive.extract.file (StringType file)

  # Par Module
   # ObjectType (par) Par.new ()
     # qualifiers
       # width    : IntegerType
       # tabwidth : IntegerType
       # just     : IntegerType

   # IntegerType Par.parse_argv (ObjectType par, ArrayType argv)

   # StringType|IntegerType Par.process (ObjectType par)
     # qualifiers
       # tostdout  : [0:1]
     # ifnot tostdout, then the returned value is either NULL or an array

  # Generate Errno Module
   # MapType Generate.errno ()

  # Clock Module
   # IntegerType Clock.settime (IntegerType seconds)

   # ObjectType (rtc) Clock.readhw ()

   # IntegerType Clock.sethw (ObjectType rtc)

  # Dev Module
   # IntegerType Dev.blksize (StringType device)
     # qualifiers
       # print : [0:1]

  # Convert Module
   # IntegerType Convert.string_to_hexstring (StringType s)
   # IntegerType Convert.hexstring_to_string (StringType s)

  # System Module (appended functions)
   # IntegerType System.sleep (IntegerType seconds)
   # IntegerType System.to.memory ()

   # IntegerType System.mount (StringType device, StringType mountpoint, StringType fstype)
     # qualifiers
       # mount_data : StringType
       # mount_flags: IntegerType
       # read_only  : [0|1]
       # no_exec    : [0|1]
       # no_dev     : [0|1]
       # no_suid    : [0|1]

  # Scdoc Module
   # StringType Scdoc.parse (StringType input)

  # Filetype Module
   # StringType filetype (StringType file)

  (notes on modules):
     - most of them are self explanatory, as they correspond to standard C functions

     - almost all of them are wrappers around the underlying internal libraries
       - all of them match the function name and most of them the function arguments
         of their counterparts

       - not all of them they call their equivalents

       - with the same consistent way, they are being used in C which writes
         this code

     - those are all excuses to avoid writting extented documentation

# Library
  # Argparse Interface

    include ("argparse")
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
      ARG_LITERAL
      ARG_VALUE_APPEND

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

      # when the ARG_VALUE_APPEND flag has been set, then the result is a LisType

    # All the options that do not correspond to none of the given options, are
      stored in the argparse.argv, while the argparse.argc holds the length of
      that array of strings.

    # The add() method accepts a "defval" qualifier which becomes the default value
      if the specified argument hasn't been used.

# Aplication Programming Interface.

```C
  /* the following static variable and the corresponded macro, normally
   * has been defined by the cenv.h header unit, and which is responsible
   * to create this specific environment, and it is here for just clarity
   * and not as the only way, it might be as well in function scope, here
   * it is accessible by the compilation unit */

  static  la_T __LA__      // holds the instances and expose function pointers
                           // and s|getters for its properties

  #define La  __LA__.self  // handy

  la_T *LaN = __init_la__ ();  // Interpreter Instantiation

  __LA__ = *LaN;

  // new instance (indepented)
  la_t *la = La.init_instance (LaN, LaOpts(.argc = argc, .argv = argv));
                                         // assumed a main function here

  char *bytes = "println (\"howl unixversion\")";  // evaluation string
  int retval = La.eval_string (la, bytes);         // for this instance

  __deinit_la__ (&LaN); // release all the sources
```

The whole interface is exposed as a structure with function pointers in the
sample unit, that map to their corresponded static declared functions in the
library unit, as there isn't and probably will never be an API documentation,
unless a god exists.

[Note that this a copy of the previous document, which it was under heavy (for
quite some time) development, and which is located in data/docs/la.md at the
root of the distribution].
