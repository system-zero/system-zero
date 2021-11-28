This a tiny Programming Language, that has been implement to supply
this system, with an extension Language that can be used at runtime.

The second purpose and a tiny ambition, is through research to understand
and finally design, a basic Programming Language Interface, that could be
interpreted by [m]any Programming Language[s].

That means that it should be composed with established concepts, such with
a familiar syntax, and very well defined semantics, instantly understood by
any programmer, and easy to interpreted by other Languages, sometimes even
without the need of a special interpretation, but as is.

The general idea is, that the exact same code, written in a nutreal way, can
be interpreted by as many offers an implementation.

There is a reference implementation, that should obey this basic abstraction.

However, it still remains a language with an opinion, and its own prefered way
to express, but this is out of the basic scope, so the two concepts (hopefully
well defined) should be presented separately.

The following is an early draft, but looks quite close to the final reference.

## Syntax and Semantics

Note:
This for now, it is written as an early description, and it is based on valid
code, and with some inline comments for the purpose of development of a final
reference.

This is a mix of established C, and well defined concepts by many dynamical
Languages.

The basic aim here is to find a balance, between simplicity and capability.
So, we have to define this, with the absolutelly minimum set of features is
possible, but at the same time those features should be enough to make this
useful and practical, otherwise it doesn't really make a sence.

Basic DataTypes:

  - StringType (with UTF8 support)
  - IntegerType
  - NumberType (double)
  - MapType
  - ArrayType

  - ObjectType (C objects)
  - User Defined Functions
  - C Functions
  - NullType

Comments.
  Single line comments that start with ('#') and end up to the end of the line.

```js
  var v;
  # or
  var c
  # both are valid. An explicit semicolon, denotes the end of a statement.
  # A new line also can denote the end of a statement, when they didn't left
  # any tokens to consume. Thus a statement, and based on the context of the
  # may span into multiply lines.
  # Multiply statements in one line without a semicolon at the end, may work
  # or may not work, as there are a couple of obvious ambiquities, so it can
  # not be guarranteed. However this is valid in this specific implementation:

  var a = 10 var b = "a" println (b) if (a) { b = "b" }

  # are four statements, that are being parsed correctly, but easily can be
  # misinterpreted.

  # If a variable is not initialized with some value at the declaration time,
  # as it happened above, it is initialized with the `null` value, so `v' and
  # `c' they have the value of `null`.

  # Give it a value:
  v = 1
  # Any symbol can be reassigned with a new value, unless it is attributed as
  # `const`. Symbols are associated with a value but do not have types, just
  # the type of the associated value.

  const vv = 1

  # In any attempt to change value to constant types, the interpreter should
  # raise an error.

  # As you may not know the value of a constant untill the runtime, it may left
  # uninitialized, untill the first time that will be initialized with a value
  # other than `null`.

  # You may assign multiply variables, as long they are separated by a comma:
  var xxx,
      yyy,
      ccc;


  # The following is a function declaration. Functions and function arguments
  # do not have types.
  func name (arg) {
    return arg * 2
  }
  # The function body, is delimited by a pair of braces '{}'.

  # All the blocks are delimited with a pair of braces '{}' and are mandatory,
  # for this basic interface at least.

  # Every block creates a new local scope, which is invisible to the outer scope,
  # and accessible only from the nested blocks.

  # You can associate a function with a symbol and use it with the same way
  # you use a function.

  var funname = func (arg) {
    return arg * 2
  }

  # A function can be used as an argument to a function
  func ref (y) { return y * 2 }
  func fun (f, y) { return f (y) }

  fun (ref, 11)  # => 22

  # A function without an argument list can be declared without the leading
  # parentheses, as:
  func fu { println (fun (ref, 22)) }
  fu ()   # this it will print 44 and a new line at the end, unlike the `print`
          # function that do not, and which otherwise behaves like `println`.

  # Functions can be nested in arbitrary level:
  func fuc (a, b, c) {
    func fud (a, b, c) {
      return a + b + c
    }

    return fud (a, b, c)
  }

  # Functions can call themeselves:

  func fibo_tail (n, a, b) {
    ifnot (n) { return a }
    # the language supports an `ifnot` conditional, which evaluates to true
    # when the expression is zero. SLang has it too, others have something
    # similar. Very useful for explicit expressions.

    if (n == 1) { return b }

    return fibo_tail (n - 1, b, a + b)
  }

  println (fibo_tail (92, 0, 1)) # => 7540113804746346429

  # or

  func fibo_recursive (n) {
    if (n < 2) { return n }
    return fibo_recursive (n - 1) + fibo_recursive (n - 2)
  }

  println (fibo_recursive (12)) # => 144
  # however the stack can be easily exhausted with some thousands of calls.
  # In the standard own interface, the compiler performs a tail call optimization
  # if explicitly call itself as `self`. However the return statement should be
  # a tail call, e.g., the last one in the scope.
  # In the above case it's not a tail call though, so hypothetically can overflow.
  # But the first form it is a tail call, and so the return statement it could be
  # written as:
  #   return self (n - 1, b, a + b)
  # instead of:
  #  return fibo_tail (n - 1, b, a + b)
  # In the first case the language can perfom a tail call optimization.

  # Functions can be anonymous.

  var v = lambda (x, y) { return x * y } (10, 100) # => 1000

  # A lambda function, it is like a function without a name, but it is called
  # immediately. After the call releases its resources. It is illegal to store
  # a lambda in a variable. It is also illegal to omit the argument list after
  # the body, even if it is an empty list, so a pair of parentheses is obligatory
  # after the body.

  # Lambdas like functions, can be nested in arbitrary level, though they
  # can be complicated to parse, but legal:

  var r = lambda (x, y) {
    var xl = x + y

    return lambda (k) {
      return k * 2
    } (x) +

    lambda (z) {
      var i =
        lambda (x) {
          return x + 100
        } (z)

      return (z * 2) + i
    } (xl)

  } (50, 100)

  println ("${r}") # => 650

  # Below statements and loops, is some classic C.

  # if/else if/else
  func ifelseif (x) {
    if (x == "x") {
      return "x"
    } else if (x == "y") {
      return "y"
    } else {
      return "xy"
    }
  }

  println (ifelseif ("")) # => xy

  # for
  var sum = 1
  func forfun (x) {
    for (var i = 0; i < x; i += 1) {
      sum += i
    }
  }

  println (forfun (10)) # this it will print (null) as the function it
  # didn't return a value. But functions always return a value and functions
  # that don't return a value. This value is `null` by default. For C functions
  # this is guarranteed by the function signature, which is always a VALUE type.

  # now print the sum
  println (sum) # => 46

  # likewise with a while:

  sum = 1
  func forwhile (x) {
    var i = 0
    while (i < x) {
      sum += i
      i += 1
    }
  }

  println (sum) # => 46

  # and the do/while loop
  func forwhiledo (x) {
    var i = 0
    do {
      sum += i
      i += 1
    }  while (i < x)
  }

  print ("all the results should be ${sum}\n")
  # Here we saw that the print functions can use interpolation syntax
  # for formated strings, By default it determinates the convertion based
  # on the type of the value. But directives can be used also:

  # This is a map declaration which it is a memory managment type, so it is
  # associated with a memory address:

  var damap = { "key" : 1, "second" : "two" }
  println ("${%p, damap}") # this it will print the hexadecimal address of
    # the value.
    # The supported set of directives:
    #  - %d as a decimal
    #  - %s as a string
    #  - %p as a pointer address
    #  - %o as an octal (0 (zero) is prefixed in the output)
    #  - %x as a hexadecimal (0x is prefixed in the output)
    #  - %f as a double

  # Maps

  # We saw above a map declaration. A MapType it is a container, that has the
  # the semantics of an associative array. That means a `key' it is associated
  # with a `value'.

  # A key can be defined in one of the following three ways:

  # - as a string.
  # - as an identifier. In this case it is constructed only by valid identifiers
  #   [_a-zA-Z] and digits.
  # - as an expression, using $(expession)

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
    "again visible" : "Visibility it is public by default."

    private
    "back_to_privacy" : "So and the next properties until a public attribute."
    "summary" : 0

    public
    "exposed_fun" : func {
      println ("I am a function method, and I can saw you them all.")
      return this.private_prop + " " + this.metoo
      # Some Interpreted Languages they refer to this self object as self,
      # some as this. We use this, and which has sence only inside map methods.
    }
  }

  # Testing for string equality for a public property.
  println (dadamap."again visible" == "Visibility it is public by default.") # => 1

  # But this will raises an error:
    # println (dadamap.back_to_privacy)
    # SYNTAX ERROR: back_to_privacy, symbol has private scope

  # Accessing map properties is through a dot ('.'), the same way C access its
  # structures. The properties can be eithers strings, identifiers or expressions.

  # You can append a property or a method to a map at runtime:

  dadamap.sumfun = func (x) {
    this.summary += x
    return this.summary
  }

  println (dadamap.sumfun (10)) # => 10

  # But it is not possible to override a method. This it will raises an error:
    # dadamap.sumfun = 100
    # SYNTAX ERROR: you can not override a method

  # Unless doing it explicitly:
  override dadamap.sumfun = 100
  println (dadamap.sumfun) # => 100

  # But how can you access map members, when the key is constructed at runtime?

  var x = "question!"
  dadamap.$(x) = " Does really has a value?"
  dadamap.$("answer") = "Dubious. But nothing is lost forever."

  println (dadamap.$("question!"))
  println (dadamap.$("ans" + "wer")) # string concatenation

  # If it wasn't for that, we had to use 2 C map functions (setter/getter) from
  # the "std" module, for any of those expressions/statements. To that same
  # C module, there are specific to maps functions that may assist.

  # As the last.
  # There is a convienent way to loop over a map, but it is outside of this
  # basic interface for now:

  for |key, value| in dadamap { println ("${key} : ${value}") }
  # The private fields, should not be printed in this case.


  # Untill now we saw many uses of strings, like string concatenation, or that
  # strings can be checked for equality.

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
  println (la[0] == 'l') # => 1
  # Here also we see that individual characters can be enclosed in single quotes,
  # that they should point to the associated integer value, again like C does.
  # But unlike C, this is not limited to the ascii range. This works the same:

  println ('α') # => 945

  # There are also backquoted strings, with different semantics, but they are
  # out of scope for now. Those strings are interpreted litterally, and they
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

  # Other attributes may added in future, but for those cases there is a
  # more expressive way implememted in the language, that can filter a value
  # through a series of function calls and expressions.

  # SLang also has backquoted strings, with a couple of attributes that tune
  # the behavior at the parsing time, without to have to call a filter later
  # to transform the string.


  # Finally the "std" modules includes functions that deal with strings.
  # There are also two special forms of the for loop, that can be used to loop
  # over strings, one with byte semantics and one with character semantics.
  # Again this doesn't belong for now to this interface.

  var byte_semantics = "byte samntics"
  for |c| in byte_semantics {
    println (c)
  }

  var aristofanis = "Βρεκεκεκὲξ κοὰξ κοάξ"

  for |c, v, w| in aristofanis {
    println ("character integer represantion |${c}|, as string |${v}|, cell width ${w}")
  }

  # Arrays are pretty straight forward and with established semantics.

  var ar = ["a", "b"] # StringType array with two members

  # But arrays have a fixed size and type. In the above code those have been
  # determinated by the parsing. The first element gives the type. It is an
  # error to mix types in an array, except for memory managment types, like
  # strings or maps can have null elements.

  # Here is how you can declare an array with a fixed size, and associate it
  # with a type:

  var integer[4] int_ar

  # Simply creates an integer array with four elements. By default the value
  # of an integer array element is initialized to 0, while 0.0 is for double
  # arrays, an empty string for strings and null for other datatypes.
  # The following types are supported:

  #  integer
  #  number
  #  map
  #  string
  #  array  (arrays of arrays can be nested in an arbitrary depth)

  # Now it can be initialized with an algorithm:
  for (var i = 0; i < len (int_ar); i += 1) {
    int_ar[i] = i
  }

  # Here we've used the `len` C native function, that returns the length
  # of the datatypes. In that case it is the number of elements of the array,
  # for strings is the number of bytes, for maps is the number of keys.

  for (var i = 0; i < len (int_ar); i += 1) {
    println (int_ar[i])
    # here we see that for single values, the print functions do not require
    # double quotes around the argument, neither a special syntax.

    # Also we saw how to access array elements, which is the exact C way.
    # Indices start from zero and can be negative, where -1 points to the
    # last element. The interpreter should throw an out of bounds error when
    # the index is equal or greater than the length of the elements.
  }

  # The same can be written more compactly like:
  for |v| in int_ar { println (v) }
  # But this kind of loop belongs to the extended interface for now.

  # The above forms can be mixed:

  var string[3] ar = ["holidays", "in", "cambodia"]
  # This is an array declaration and assignment syntax with a predefined length
  # and type at the same time. This should execute faster, as the parser do not
  # have to determinate the type and the size.

  # An array can be assigned to a range of indices:
  int_ar[0:3] = [1, 1, 1, 1] # explicit range [from first_idx to the last_idx] 
  # or
  int_ar[0:] = [1, 1, 1, 1]  # if the second idx is ommited, then assumed array length - 1

  # In any case if the number of expressions doesn't match or any idx is >= length
  # the array, the interpreter should raise an OUT_OF_BOUNDS error.

  # For such cases a more short form exists:
  int_ar[*] = 1
  # This syntax should be attributed at SLang Programming Language.

  ## This is the end of the first draft.
  # Left to do:

  # Flow control statements (break/continue/return)
  # Logical operations (briefly the same exact C way)
  # Bitwise operations (briefly the same exact C way)
  # Import C code (briefly the same way as other Interpreted Languages)
  # LoadFile with code (basically the same way)

  # And answers to questions:
    # - var s = "string"; var sa = s; s[0] = 'd'
    # should `sa' be modified?
    # Such questions are answered by the implementation for now,
    # but they should be documented one by one.

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
  # be supported probably and has some sence and it has imo, it is a mechanism
  # that could pause execution to the try breaking point and then to provide
  # an interactive session. At any that try point, the state has to be saved
  # first. The mechanism will have to expose internal information, about values
  # or for evaluation parsing points or function bodies. And finally it has to
  # support an interactive session, with options to abort, debug or even to...
  # re-evaluate, by providing even the failed function with a new body.
  # This for sure worths some invenstment.
```

And that is the first draft about the first basic interface that resembles the
C way, with the obvious differences, basically the absent of type declarations.

This zero point syntax and semantics, should obey to the least surprise
principal, so they should not violate expectations.
If a specific point, it does not obey or if it does, then naturrally it should
be considered as either a bug that should be fixed, or should not be supported.

The next section is a very first draft of a document that describes the
experiences towards a more humanized kind of code expressions, that could
litterally express the underlying thought and so and the intention, and
which is the single most precious jewel when reading and re-writting code.

Those mentioned expressions have been implemented, but this is really a dynamic
development, and so it needs a bit of time to understand it, and then find the best balance,
without ambiguities. Most of them though are already established and they will
never change, but doubts might be mentioned in cases, which it will simply means
that there simple doubts.

```js

# Development Items (all stand by its own):

#  - if cond then do this orelse do that, kind of Code Expressions. See below.

#  - instead of:
#      if condition is true then do [control statements {return, break, continue}]
#    to:
#      [control statement] [argument] if condition
#    or in a more literal form:
#      do this if true expressions

    forever
      break if true

#   See the section for the appropriate statement for its syntax and any argument
#   that might take.

# - replace the '==', '!=', '&&' and '||' operators with the: `is`, `isnot`,
#   `and`, and `or` key operators:

    if (1 is 1 and 2 is 3 or 3 is 3 and 4 isnot 5) println (ok)

# - use the `ok' and `notok' keywords to denote success or failure. The
#   underlying value is '0' and '-1' internally, which is the same as C.

# - loops can have as a body a single statement that fit in a single line.
#   In this case the braces can be ommited:

    while (true) break
#   or
    while (true)
      break
#  But no more than a statement than fit in a single line, and which may
#  have a loops that produce another loop, as long all these they fit in
#  a single line.
#  See the specific section, below.

# if cond then do this orelse do that, kind of Code Expressions.

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

  # [development: strict environemt, as it is with private/public to denotes some
  # constraints to possibilities, like not be able to use the ++,-- operators.
  # For future this might allows to call only functions that evaluate only expressions,
  # or functions that do not have code that influence or it is influenced by the
  # outter environment.]

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
    # here the `is` reserved keyword is same as '=='
    # also the `isnot` keyword is same as '!='.

  # in a function call argument list:
  func f (arg) {
    return arg * 2
  }

  var res = f (if v is null then 1 orelse 0) # => an argument of value 1

  # to get an array index:
  var ar = [1, 2, 3]
  println (ar[if v isnot null then 0 orelse 2]) # => 3

  # to access a map key with an expression:
  var m = {"key" : 1, "f" : 2}
  println (m.$(if v isnot null then "f" orelse "key")) # => 1

  # as an operand to binary operations:
  var s = "Βρεκεκεκὲξ" + (if v is null then " κοὰξ" orelse "") +  " κοάξ"
  println (s) # => "Βρεκεκεκὲξ κοὰξ κοάξ"
  # note here that the expression is surrounded with parentheses, otherwise
  # without them, the `orelse` would continue with the add operation and the
  # result would be different (without the last κοάξ).

# All the loops can be written as single statements that fit in a single
# line without braces:

  func f (x) return x * 3
  while (true) break
  for (var i = 0; i < 10; i++) for (var j = 0; j < 10; j++) println (i * j)
  # the same above could be splitted as:
  for (var i = 0; i < 10; i++)
    for (var j = 0; j < 10; j++) println (i * j)
  # that means that if the next token after the for init statements is a new
  # line, then the next line is considered as a single statement. Searching
  # for a single statement stops at the new line character or at a semicolon
  # character that is not in a string or to another loop construct statements.
  # If a token found that it doesn't denote the end of evaluated string, then
  # (for now) a warning is issued about extra statements.
    # [development: this can be an instance option -> be_strict]

  # One line functions.
  # Likewise, a function can define its body as a single line body, if it is
  # not surrounded by a pair of braces '{}':
  func xx (x) return x * 2
  func xxx (x)
    return x * 2

  # Currently the body can contain multiply statements, and evaluated correctly
  # as long they have clear semantices and which they usually have. There are
  # cases though that multiply statements in a single line without semicolons at
  # the end, they produce unintented evaluation. For that reason only a statement
  # can be guarranteed.

```

The next is a first draft of a document that describes the syntax and semantics of the language.
of the language. This development is towards to a more functional environment,
and to a more concise code and expressions.
This is under development. Many concepts have been stabilized or are almost ready,
though there are still left to be implemented.

```js

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

  # Functions should accept at least one argument and functions can be also
  # lambdas:
  println (12342 :
      lambda (x) { return x: to_string (10) } ():
      lambda (x) { return x: to_integer () } ())  # 12342

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
  println (q (100; {"key" : 200})) # => 20000

  func qq (x) {
    var m = qualifiers ()
    if null is m then m = {"key" : 10}
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

  # Note that only one set of qualifiers can be active at the running instance.
  # Because of that the called function, should use the interface, before any
  # new function call. (this might be best to change)

  # Again, this mechanism should be attributed to the SLang Programming Language,
  # and it is exposed with the exact interface.


  # User Defined Types.
  # These are like Maps, with a couple of differences that are described below.

  # A new type is defined by using the `Type` keyword. The leading identifier
  # character should be capitalized.

  Type Typename {
  private
    "key"
    "init" : func (x) this.key = x
  public
    "fun" : func return this.key
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

  for (var i = 1; i < 10; i += 1) {
    for (var j = 0; j < 100; j += 1) {
      if (j is 5) {
        break 2
      }
    }
  }
  # this will pass control to the first loop level. In other words "break"
  # and "break 1" is synonymous.
  # If the given `count` is greater than the existing loop level, a syntax error
  # is raised. Maximum `count` is 9 nested loops.

  # If botth `continue` and `break`, is not into a loop state then a syntax error
  # is raised.

  # However both can be expressed in a boolean context. That means that they
  # get executed only if the condition is true:
  for (var i = 1; i < 10; i += 1) {
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
  # execution without returning back a value. In this case the
  # value becomes `null`.

  func ret1 () return
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

  # Recursive functions and stack overflows.
  # Because calling recursively a function, it easily can overflow the stack
  # with some thousand calls, the language supports a `self` keyword after a
  # return statement, that it can be used to call the own function again. In
  # this case a tail call optimization it is performed. If the statement it is
  # not a tail call, a syntax error is raised:
  func rec (n) {
    return n ifnot (n)
    return self (--n)
  }

  println (rec (2000000))



```
The first level/point aims mostly, to investigate syntactical ways to imitate
the exact human's mind usual roolllingg flow that happens the exact time that
the underlying thought it is expressed in code, through our fingertrips while
watching the code that it is expressed with words now in our visual editor.

The hard part is that the intented words and expressions, should correspond to
expected semantics, without ambiguities that could make the mind to stop for a
second while rolling an expression.

As such it is a reasearch to unification of the intention with the expression.

As this is a very first draft, the following (which was the first document that has been produced
during the phase of development in research to settle to syntax and semantics),
it is still almost relevant.

## Syntax and Semantics
```js
# Note that this is the document that had been produced during the scratch
# days and nights of the development. As such it doesn't attempt to sepatate
# concepts (though it repeats some info, we'll leave it around, untill at least
# a time that will not have any reference. Tagged as LAnguageInterpreterDoc-0.0.
# Hopefully will fulfil this aid promise, at least a bit.

# LAID-0.0 (started at the dawn (ended at 04/09/21 late afternoon))

# A `#' denotes the begining of a comment that ends up to the end of line.

# Variable assignment

  var i = 0
  var double = 1.1

  If a variable is not initialized at declaration time, it is initialized as null.

# Function declaration

  func name (arg) {
    return arg * 2
  }

  the following is synonymous

  var name = func (arg) {
    return arg * 2
  }

# A function can be used as an argument to a function

   func ref (y) { return y * 2 }
   func fun (f, y) { return f (y) }

   fun (ref, 11)  => 22

# A function without an argument list can be declared as:

   func name { block }

# Code blocks are delimited with braces '{}', and are mandatory.

# Statements are separated with a semicolon or with a new line character,
  and they can span to multiply lines based on the context. Note that it
  is allowed for multiply statements in a single line, and are separated
  based on the context, though there might be unhandled cases, like:

    var a = 10 var b = "a" println (b) if (a) { b = "b" }

  while the above is legal and it is being parsed correctly (four statements),
  this it isn't:

    var a = 10 var b = "a" println (b) if (a) { b = "b" } println (b)

  If in doubt, use a semicolon as a separator. This works now:

    var a = 10 var b = "a" println (b) if (a) { b = "b" }; println (b)

  In any case multiply statements in a single line, it is a bad practice.

# Conditionals:
# `if` conditional: the block is executed if the condition is true

  if (condition) { statement[s]... }

# `ifnot` conditional: the block is executed if the condition is zero

  ifnot (condition) { ... }

  Both can get an `else` clause, that is evaluated when the prior conditional
  block hasn't been executed.

  Both can be extended with an `else if[not]` conditional block:

    } else if (condition) { block }

  or

    } else ifnot (condition) { block }

  if the prior conditional expression evaluated to zero.

# if[not]/then/orelse/[end] conditional:

  full form:

    if cond then do_something orelse do_something [end]

  orelse can be ommited:

    if cond then do_something

  They can be followed by other if/then/orelse:

    if cond then
      if cond then
        ifnot cond ...

  so this is dyadic and linear.

  The mechanism doesn't create a new scope, as it happens with all the
  loops and block delimited conditionals, which they have to do (because
  they have to manage their local symbols and to resolve them properly,
  and release them at exit).

  So it doesn't accept variable declarations, as it doesn't make sence
  to create a variable in a block with a single statement.

  Such conditionals they have to end up with a new line character or the
  `end` keyword.

  For parsing reasons, as they have to consume extra clauses, they don't
  accept loops and block conditionals.

  This exact syntax can be used as an expression, which is explained
  in the [If As Expression](# If As expression) section below.

# Loops:

  while loop (same semantics with C):

    while (condition) { block }

  for loop (same semantics with C):

    for (init_statement[[s], ...]; cond ; ctrl_statement[[s], ...]) { block }

  Special forms of the `for` loop that work as an iterator, can be found to
  their specific datatype section.

  loop loop (loop for nth times):

    loop (num iterations) { block }

  alternatively

    loop (init_statement[[s], ...]; num iterations) { block }

  forever loop (like a for (;;) or while (true) in C):

    forever { block }

  alternatively

    forever (init_statement[[s], ...]) { block }

  do/while loop (same semantics with C):

    do { block } while (condition)

  A continue statement into a body of a loop, continues with the next iteration.

    for (var i = 1; i < 10; i += 1) {
      if (i is 5) {
        continue
      }
      ...
    }

  A break statement returns control to an outter scope, of the current loop
  state. This by default is one (the current one) loop level. However it can be
  set explicitly (with a given `count` that follows the statement), to break to
  `count` outer loop levels.

    for (var i = 1; i < 10; i += 1) {
      if (i is 5) {
        break
      }
      ...
    }

  or
    for (var i = 1; i < 10; i += 1) {
      for (var j = 0; j < 100; j += 1) {
        if (j is 5) {
          break 2
        }
      }
    }

    this will pass control to the first loop level. In other words "break"
    and "break 1" is synonymous.

    If the given `count` is greater than the existing loop level, a syntax error
    is raised. Maximum `count` is 9.

  If a `continue` or a `break`, is a statement that is not into a loop state,
  then a syntax error is raised.

  However both can be expressed in a boolean context. That means that they get
  executed only if the condition is true:

    for (var i = 1; i < 10; i += 1) {
      break if (i is 5)
    }

  or

    for (var i = 1; i < 10; i += 1) {
      continue ifnot (i is 5)
    }

  In absence of a known term, as this is unique (from what I know), we can
  describe this concept as a "Boolean single statement" or something like this.

  It accepts only a single expression to evaluate, and it doesn't accepts clauses.
  It works both for `if` and `ifnot`.

  Finally, it has to be seen the same way as an `else if`, so an `if` or an
  `ifnot`, should follow `break` or `continue` to the same line.

  (notes: that this should execute faster, than with an equivalent traditional
     way and order. That is because, this mechanism it doesn't create a new
     state/scope, unlike the `if[not]` conditional statements that they have
     to do to manage symbols scope. Plus there is no need to parse the code
     with a way that can interpret correctly `else if` and|or `else` clauses.

     Also note that it is believed that this is a more natural mind flow,
     as you might feel it (albeit with some bits of attention), when you
     will try to spell it, either loudly or in your mind.

       "if condition then break" in contrast with "break if condition"
  )

# Constant types (those types can not be reassigned)

  const c = "constant"
  c = 1  # this should fail

  if the value is not yet know, then it can be initialized with null,
  and then it can be reassigned.

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

    lambda [([arg], ...)]   {body}  ([arg], ...)

    var i = lambda (x, y) { return x * y } (10, 100)

  A lambda function, it is like a function without a name, but it is called
  immediately.

    lambda (x, y) { return x * y } (4, 22)

  It is illegal to omit an argument list after the declaration,  even if it
  is empty. If it is this case they could constructed like:

    lambda { ... body ... } ()

  Lambdas like functions, can be nested in arbitrary level, though they
  can be complicated to parse, but legal:

    var r = lambda (x, y) {
      var xl = x + y

      return lambda (k) {
        return k * 2
      } (x) +

      lambda (z) {
        var i =
          lambda (x) {
            return x + 100
          } (z)

        return (z * 2) + i
      } (xl)

    } (50, 100)

    println ("${r}") => 650

# Arrays
  Array declaration:

  first form:

    var ar = [1, 2, 3]

    this is an anonymous array that get assigned to the symbol `ar`. It takes
    its type of the first element, which in this case is an integer type.

    Such "on the fly" arrays can be used elsewhere in expressions, and they should
    be freed automatically.

  second form:

    Array declaration and assignment syntax with a predefined length and type:
    (note that with current code, this method should execute faster than the
    above, since the above requires first a "light" parsing, to determinate
    the length of the array, and then redo the "real" evaluation)

    var string[3] ar = ["holidays", "in", "cambodia"]

  the declaration syntax is:

    var type[length] varname

    - a `type` can be one of the followings
      - integer
      - number
      - string
      - map
      - array

  Array assignment syntax:

  First form:

    ar_symbol[first index : [last index]] = [first index, [...,...,] last_index]

      where assignment starts at `first index` and stops at `last index`.
      If `last index` is omited then `last index` assumed (array length - 1).

      The number of expressions should match, or else it will result to an
      OUT_OF_BOUNDS error.

  Second form:

    ar_symbol = [..., ...]

      in this case the mumber of expressions is considered the length of the
      array. If not, the interpreter will throw an OUT_OF_BOUNDS error.

  You can set _all_ the array items, with a specific value using:

    ar[*] = value

    *Note* that this syntax should attributed to S-Lang programming language.

  You can get or set an item from or to an array, using indices that can
  be negative.  Array indices are starting from zero, and -1 denotes the
  last item in the array.

  Two special forms of the `for` loop, can be used as array iterators:

    first form:

      for |elem| in array { block }

      In this case 'elem' holds the value of each array elememt.

    second form:

      for |i, elem| in array { block }

      In this case 'i' holds the index of the array, and which initially starts
      from zero.
      This form is being used when both the index and the corresponded element
      are needed inside the block.

# Maps. This is the only container type in the language, and which associates (maps)
  a "key" with a "value". This is a hybrid type, similar to associative arrays
  and structures in other languages and almost similar to json format. Note that
  this is an unordered list, and there is no guarrantee that keys and values, will
  will have the order of the declaration, when looping over a map.

  Map Declaration:

    var m = {}  # empty container

    var mm = {
      "one" : 1,
      "two" : "string",
      "1"   : 1,
      "fn"  : func (x) { return this.one * x },
      private "privatevar" : "accessible only by the members of the map"
    }

  the `this` keyword is being used _only_ from the members of a map, and it
  provides access to the other fields, as it holds the value of the underlying
  map.

  On submaps methods, `this` is a reference to the parent map, unless they are
  accessing using a colon (':'), thus on:

    map.submap:method (...)

  `this` is a reference to submap

  Keys are valid string identifiers and may start with a digit or an underscore

  By default it is not possible to override a method of a map, unless
  the statement it is attributed explicitly with the `override` keyword:

    var m = { "f" : func { return 1 } }
    func c { return 11 }
    override m.f = c
    m.f () => 11
    m.f = c # error

  Accessing maps. This is done by using a dot ('.') after a symbol that refers
  to a Map Type.

    mm.one
    mm.fn (10)
    mm.privatevar = "something" # this should fail

  Extending maps:

    mm.new = 1

    in those cases those fields are accessible from their scope.
    Members can be attributed as `private` only at the construction time,
    not even within the members at runtime.

  When the name of the key is not known and it is constructed at runtime,
  a dollar ('$') expression can be used:

    var v = mm.$("new")
    var keyname = "new"
    mm.$(keyname) += 1

    If it wasn't for that, a standard function had to be used and in the
    last statement two C function calls, one to get the variable value and
    one to set the value. Plus it would be much more verbose.

  A special form of the `for` loop, can be used as an iterator that can
  loop over Maps:

    for |k, v| in map { block }

    in this case 'k' holds the key of the map, and 'v' its associated value.
    Note, since a Map is an unordered list, there is no guarrantee of the order.

# Strings

  String literal are delimeted by double quotes ('"') or back quotes ('`').

  Both are multi-line strings but the difference is that double qouted strings
  are subject for backslash substitution.

  In that case the characters are interpeted like:

    \a  audible bell
    \b  back feed
    \e  escaper
    \f  form feed
    \n  new line
    \r  carriage return
    \t  tab
    \v  vertical tab
    \\  backslash itself
    \"  to appear a double quote in double quoted strings
    \`  to appear a back quote in back quoted strings

  Loop over strings.
    Two special forms of the `for` loop, can be used as an iterator that can
    loop over strings:

    First form is iteration over the bytes:

      for |c| in str { block }

      in this case 'c' holds the integer value of the underlying byte.

    Second form is iteration over the characters:

      for |c, b, w| in str { block }

      in this case 'c' holds the integer value of the underlying byte,
      while 'b' holds the string representation and 'w' holds the cell
      width of the character.

  Print functions syntax:

    print[ln] ([file pointer object], "string ${expression} ...")

    file pointer can be either `stdout` or `stderr`, or any file pointer object
    that has been created with the fopen() function:

      var fp = fopen (filename, mode)  # same semantics with C

    Without a file pointer argument, default is to redirect to the standard output.

    interpolation expression syntax:

      ${[%directive,] symbol or expression}

      A directive can be optional and can be any of the followings:
        - %d as a decimal
        - %s as a string
        - %p as a pointer address
        - %o as an octal (0 (zero) is prefixed in the output)
        - %x as a hexadecimal (0x is prefixed in the output)
        - %f as a double

    Without a given directive, then it is determinated by the underlying type
    of the value.

    The `println()` function is like `print`, but also emits a new line character.

# Qualifiers
  The language supports a mechanism to pass additional information to functions,
  called `qualifiers`, which has been copied by the SLang Programming Language,
  which works the same with user defined functions and with C functions.

  This extra information gets passed with a map, within the expression list
  on a function call, and when after the argument list follows a semicolon:

    fun (...; {...})

  There are three relative functions that can be used within a function:

    qualifiers () # this returns a map or null if no qualifiers has been
                    passed to the function.

    qualifier_exists ("qual") # this returns a boolean value; `true` if
                               `qual` exists or `false` otherwise

    qualifier ("qual", default_val) # this returns the value of the key `qual`
                                      if exists or `default_val` otherwise

  Examples:

    func q (x) {
      var av = qualifier ("key", 10)
      return av * x
    }

    func qq (x) {
      var m = qualifiers ()
      return x * m.key
    }

    func qqq (x) {
      var y = qualifier_exists ("key") + 9
      return x * y
    }

    func qqqq (x) {
      return qqq (x; qualifiers ())
    }

    var m = {"key" : 1000}
    var qa = q (10)                 => 100
    var qb = q (10; {"key" : 100})  => 1000
    var qc = q (10; m)              => 10000
    var qd = qqq (10)               => 90
    var qe = qqq (10; m)            => 100
    var qf = qqqq (10)              => 90
    var qg = qqqq (10; m)           => 100

  Note that only one set of qualifiers can be active at the running instance.

  Again, this mechanism should be attributed to the SLang Programming Language,
  and it is exposed with the exact interface.

# Types.
  These are user defined types, which are like Maps, with a couple of differences
  that are described below.

  A new type is defined by using the `Type` keyword, with an identifier that the
  leading character should be capitalized.

    Type Typename {  ... }

  A type is initialized by using the `New` keyword like:

    var s = New Typename ([init arguments])

  Then it can be called like a MapType.

  Types _should_ provide an `init` method. This can be attributed as `private` or as
  `public`. In the latter case the state can be reinitialized at the runtime
  by the user, without the `New` keyword.

  Declared fields are not obligated to provide a value on declaration
  time. In this case these properties are initialized as `null`.

# If As Expression

  The exact if[not]/then/orelse/end syntax and semantics for statements, can be
  used to get a value of a single expression, thus it evaluates expressions
  instead of statements.

  An if[not[ can be used in any expression. All the bellow are valid code:

  variable assignment:

    var v = if null then null orelse "notnull"  => null

  in a function call argument list:

    fun (if true then 1 orelse 0)  => an argument of value 1

  to get an array index:

    ar[if true then 3 orelse 0]  =>  the third index

  to access a map key with an expression:

    m.$(if false then "kkk" orelse "key")  => the key key

  as an operand to binary operations:

    "Βρεκεκεκὲξ" + (if true then " κοὰξ" orelse "") +  " κοάξ"
          => "Βρεκεκεκὲξ κοὰξ κοάξ"

  note here that the expression is surrounded with parentheses, otherwise
  without them, the `orelse` would continue with the add operation and the
  result would be different (without the last κοάξ).

# Chaining with a Sequence of Functions Calls and Continuational Expressions.

  The language supports a mechanism, where the current value, becomes the
  first argument to the next function, or the last result value.

  At this stage is capable to satisfy the following code:

    import ("std")
    var s = "97"
    s: to_integer ()          => 97
    'a' is s: to_integer ()   => true
    "asdf": eq ("asdfg")      => false
    "asdf": eq_n ("asdfg", 4) => true
    111:to_string (16)        => 0x67
    ("10": to_integer () * 12 + 52 - 24) : to_string (2) => 10010100
    "fa:fb:fc": tokenize (":") => ["fa", "fb", "fc"]
    [1, 2, 3]: len ()          => 3
    var xm = {"k" : 1, "l" : 2}
    var xk = xm:keys ()        => an array of the unordered xm keys ("k" and "l")
    (1 is 100): to_string (10) => 0 (note that the first expression is surrounded
    with parentheses, otherwise without them, a binary operation would be performed,
    between `1' and the result of (100: to_string (10))

  Those functions correspond to the exposed by the `std' module function
  names, that are concatenated based on the underlying type of the value.
  For instance and if it is StringType, then a "string_" is prepended.
  But they can be any function:

    func x (y) { return (y * 2): to_string (16) }
    println (26: x())      => 0x34 (in base 16)

  They can be also lambdas:

    12342 : lambda (x) { return x: to_string (10) } (): lambda (x) { return x: to_integer () } () - 12300

  This mechanism supports a conditional expression, which is an `if/then/orelse/end'
  construct. Here the `end` key keyword is obligatory to avoid ambiguities.

    (expr): if identifier condition then expr [orelse expr] end

    note that `identifier` is the symbol associated with the value of `expr',
    and it is the value that is being evaluated in the condition.

  Examples:

    12: if x < 12 then x + 22 orelse x + 32 end => 44
    ("asdf" is "asdf"): if v then ok orelse notok end => ok
    "asdf" : if x:eq ("asdf") then ok orelse notok end } => ok
    var s = null
    s: if x is null then notok orelse ok end  => notok
    s = 10
    s: if x is 20 then x * 2 end

    note that, if in the last expression, and since the condition hasn't been
    met, and there wasn't an orelse, the result is the input value itself.

The return statement

  A return may appear in a function to return control to the caller of the function.

  A single return that is followed by a semicolon or a new line it breaks execution
  but returns no value. In that case the returned value of the function call is null.

  The second form:

    return expression

    in this case returns the value of expression

  The third form:

    return if[not] expression

    in this case it returns only if the expression is evaluated to true, again
    without a specific value

  The fourth form:

    return single-token-value if[not] expression

    in this case returns the value of single-token-value only if the expression
    is evaluated to true. Note that a map property or method, or an array item
    it is not a single token value. These are not valid:

      return map.prop if true
      return array[0] if true

    also note that that an `if expression` is a valid value for return:

      return if true then 1 orelse 2

    or to get a map property:

      return map.$(if true then "thisprop" orelse "thatprop")

    or to get an array element:

      return array[ifnot true then 0 orelse 1]

    and those are the reasons for the requirenment for the last form, to be
    followed by a single token. Otherwise there is no way to determinate
    where the expression could stop, other than to evaluate the expression.
    But the expression should be evaluated only if the condition is true,
    and not beforehand.

# loadfile syntax and semantics:

  loadfile ("fname")

  If `fname` is not an absolute path, then it is relative to the current
  evaluated unit. If that fails, then it is relative to the current directory,
  else it is relative to the `__loadpath` intrinsic string array variable. If
  the unit couldn't be found, then an error terminates execution.

# evalfile syntax and semantics:

  var val = evalfile ("fname")

  evalfile is like loadfile, with some differences described below.

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

## keywords and Operators (reserved keywords):
```sh
# var         -  variable definition
# const       -  constant definition
#                a constant can not be reassigned since initialization; an uninitialized
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
# then/orelse -  control single conditional statements and expressions
# while       -  while loop
# for         -  for loop
# loop        -  loop loop
# forever     -  forever loop
# do/while    -  do/while loop
# return      -  return statement
# break       -  break statement
# continue    -  continue statement
# override    -  overriding attribute
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
# ^=          -  bit xor           -||-

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
# a MapType, that has the exact name but the leading char is capitalized.

  # Std Module Interface
    # IntegerType   Map.set (MapType map, StringType key, Value v)
    # ValueType     Map.get (MapType map, StringType key)
    # StringType[]  Map.keys (MapType map)
    # IntegerType   Map.remove (MapType map, StringType key)
    # IntegerType   Map.key_exists (MapType map, StringType key)

    # IntegerType[] Array.where (ArrayType ar, Value expression)

    # IntegerType   String.eq (StringType a, StringType b)
    # IntegerType   String.eq_n (StringType a, StringType b, IntegerType n)
    # IntegerType   String.cmp_n (StringType a, StringType b, IntegerType n)
    # StringType    String.advance (StringType str, IntegerType num)
    # StringType[]  String.tokenize (StringType str, StringType token)
    # NumberType    String.to_number (StringType str)
    # IntegerType   String.to_integer (StringType str)
    # StringType    String.byte_in_str (StringType str, IntegerType byte)
    # StringType    String.advance_on_byte (StringType str, IntegerType c)
    # StringType    String.trim_byte_at_end (StringType a, StringType b)

    # IntegerType   Integer.eq (IntegerType a, IntegerType b)
    # StringType    String.char (IntegerType c)
    # StringType    Integer.to_string (IntegerType i, IntegerType base)

  # Path Module Interface
    # StringType    Path.real (StringType path)
    # StringType[]  Path.split (StringType path)
    # StringType    Path.concat (StringType a, StringType b)
    # StringType    Path.dirname (StringType path)
    # StringType    Path.extname (StringType path)
    # StringType    Path.basename (StringType path)
    # IntegerType   Path.is_absolute (StringType path)
    # StringType    Path.basename_sans_extname (StringTYpe path)

  # File Module Interface
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
    # MapType       File.stat (StringType file)
    # MapType       File.lstat (StringType file)
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
    # IntegerType   File.access (StringType file, IntegerType mode)
    # IntegerType   File.mkfifo (StringType file, IntegerType mode)
    # IntegerType   File.symlink (StringType src, StringType dest)
    # IntegerType   File.hardlink (StringType src, StringType dest)
    # StringType    File.readlink (StringType file)
    # StringType    File.type_to_string (IntegerType mode)
    # StringType    File.mode_to_string (IntegerType mode)
    # StringType    File.mode_to_octal_string (IntegerType mode)

    # Constants
    # IntegerType   F_OK, X_OK, W_OK, R_OK
    # IntegerType   S_IRWXU, S_IRUSR, S_IWUSR, S_IXUSR
    # IntegerType   S_IRWXG, S_IRGRP, S_IWGRP, S_IXGRP
    # IntegerType   S_IRWXO, S_IROTH, S_IWOTH, S_IXOTH

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

  # Os Module Interface
    # StringType    Os.arch ()
    # StringType    Os.platform ()
    # IntegerType   Os.sleep (NumberType secs)
    # IntegerType   Os.getuid ()
    # IntegerType   Os.getgid ()
    # IntegerType   Os.getpid ()
    # IntegerType   Os.setenv (StringType name, StringType value, IntegerType overwrite)
    # StringType    Os.getenv (StringType name)
    # StringType[]  Os.environ ()
    # StringType    Os.getpwdir (IntegerType uid)
    # IntegerType   Os.getgrgid (StringType name)
    # IntegerType   Os.getpwuid (StringType name)
    # StringType    Os.getgrname (IntegerType gid)
    # StringType    Os.getpwname (IntegerType uid)

  # Sys Module Interface
    # StringType    Sys.get (StringType sys_setting)
    # IntegerType   Sys.set (StringType asname, StringType sys_setting, IntegerType replaceanyold) 
    # StringType    Sys.which (StringType exec)

  # Time Module Interface
    # IntegerType   Time.now ()                               # time(NULL)
    # MapType       Time.gmt (null or IntegerType time)       # gmtime()
    # MapType       Time.local (null or IntegerType time)     # localtime()
    # StringType    Time.to_string (null or IntegerType time) # ctime()
    # IntegerType   Time.to_seconds (MapType tm)              # mktime()
    # StringType    Time.format (StringType fmt, null or MapType tm) # strftime()

  # Crypt Module Interface
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

  # Rand Module Interface
    # IntegerType  Rand.new ()
    # StringType   Rand.get_entropy_bytes (IntegerType size)

  # Io Module Interface
    # FdType       Io.open (StringType filename, IntegerType flags, IntegerType mode)
    # IntegerType  Io.isatty (FdType fd)
    # StringType   Io.fd.read (FdType fd)
    # IntegerType  Io.fd.write (FdType fd, StringType buf)

  (notes:
     - most of them are self explanatory, as they correspond to standard C functions

     - almost all of them are wrappers around the underlying internal libraries
       - all of them match the function name and most of them the function arguments
         of their counterparts

        - not all of them they call their equivalents

        - with the same consistent way, they are being used in C which writes
          this code

     - those are all excuses to avoid writting extented documentation

  # Constants:
    # Exposed by the io module:
      O_RDONLY, O_WRONLY, O_RDWR
      O_CREAT,  O_APPEND, O_TRUNC
      O_NOCTTY, O_CLOEXEC, O_NONBLOCK, O_EXCL, O_DIRECTORY, O_NOFOLLOW
      # and if defined
      O_LARGEFILE, O_SYNC, O_ASYNC

      # those are defined at file module also
      S_IRWXU, S_IRUSR, S_IWUSR, S_IXUSR,
      S_IRWXG, S_IRGRP, S_IWGRP, S_IXGRP,
      S_IRWXO, S_IROTH, S_IWOTH, S_IXOTH

    # Exposed by the file module (the S_I* above and):
      F_OK, X_OK, W_OK, R_OK

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

# Semantics

  - standard keywords and functions can not be redefined and reassigned

  - function arguments that are memory types (like strings, arrays and maps),
    are passed by reference and so can be modified by the called function

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

  - single characters can be specified as integers, when are enclosed in single
    quotes:

      'a' => 97

    this doesn't limited for characters in the ASCII range:

      'α' => 945

    in this case the value is the codepoint of the UTF8 byte sequence.

    They can be also specified in hexadecimal notation using this form:

      '\x{3b1}' => 945

   - multiline literal strings are enclosed also in double quotes, but they
     can continue to the next line if an escape '\' character is encountered
     before the new line character. In any other case it is considered an error.

   - variables can be declared in consecutive manner, separated with a comma
     and they can span to multiply lines, e.g.,:

       var i = 0, ii = 1,
         a = null,
         integer[2] b = [1, 2],
         m = {"key" : "value"}

# Lexical Scope and visibility order

  - standard scope (lookup for standard operators and internal functions first)

  - block scope (conditional statements and loops)

  - function scope

  - previous function scope -> ... -> ... -> global scope

  if the symbol can not be found, then the compiler raises an error.

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

  - maximum length of any identifier is 255 bytes

# Types (integer type constants)

  NullType    : (void *) 0 (declared as `null`)
  NumberType  : double
  IntegerType : integer (wide as ptrdiff_t)
  StringType  : string type (container that holds C strings)
  ArrayType   : array
  MapType     : map
  ObjectType  : C objects

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
style. However it should be okey, if practicing consistency.

- every token should be separated with at least a space, though it might work
  without that rule, but the parser can not handle all the cases, like in (x-1)
  where -1 is considered as a number and should be coded as (x - -1). It is
  actually the only known case that is not handled correctly (but they might be
  others). No matter the parser, it is believed that a space that it separates
  tokens, it adds visual clarity, and it feels more like a sentence where the words
  are separated.

- recursive functions though they should work properly, can be easily overflow
  the stack, as the compiler doesn't perform any kind of tail call optimizations
  (this is a week point, and the worst is that this is quite difficult to change,
  so it is better to avoid code with recursive function calls beyond some thousand
  of calls, instead it is prefered the imperative version). [update: now there is
  a way to avoid stack exhaustion by using tail calls when calling recursively a
  function]

- the number type (typedef'ed as `double`) operations, need an expertise which
  is absent to the author. So this isn't going to change soon, but the mechanism
  it is there.

- normally releasing memory it happens automatically, but many cases are handled
  internally explicitly, as there is no a real mechanism underneath, just a very
  primitive one. However running the extensive test suite and the applications
  that use the language under valgring, it doesn't reveal any memory leaks, but
  this is just a sign that quite probably is false.

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

    Update at the end of July.
    So we did, and we've introduced such a mechanism, albeit in early development,
    and not in stable state. The difficulty actually is not really to maintain
    the environment, but to handle properly the resources. Otherwise it is quite
    easy. We have used ':' though and not a '.' dot, which has a bit of different
    semantics, to access and call the methods.

    But we have yet to clarify the semantics. What to do in this instance:

      var s = "string"
      s: to_integer ()

    I'm inclined to say that `s' will not have to be modified. So maybe the prudent
    thing to do here, that such a chain of function call, it has to be used as
    a closure, without the ability to modify the participated objects directly,
    so and with no direct side effects. However, I think that it will never can be
    possible to guarrante that the called methods, don't have code that influence
    the outter environment. So probably the objects that enter in such a closed
    state, should be copies of the originals.

    For the above case and code, and when the intention is to modify `s', perhaps
    a new operator could be used, like:

      s := to_integer ()

    or something like this, where the result of the chained functions will be
    reassigned to `s'.

    Of course such functions should always return a proper value. Our functions
    they return something always. For the C native functions this is guarranteed
    by the function signature, that always they return a VALUE type. For user
    defined functions, this is always at least a `null` value.

    Our mechanism currently also allows binary operations at the end of the
    chain sequence:

       "10": to_Integer () * 12 + 52 - 24

    In this case, it can be seen as like arithmetic operations or other possible
    transformations, of the final object. Of course the result of the evaluation
    it can be later reused in a new chain (this works even today, the first 12
    hours of development):

      ("10": to_integer () * 12 + 52 - 24): to_string (2) => 10010100 (2 is for base 2)

    This is the only way to sanity. If it wasn't for the parantheses around the
    expression, what would be the meaning without them:

      "10": to_integer () * 12 + 52 - 24: to_string (2)

    To some of the very best of parsers, capable to parse every little everything,
    and at the best of the case would be to trasform `24' into a string, which is
    besides the side influence, it is also hard for the human mind to parse the
    intention, so yes at the end looks like an obvious nightmare.

  And that is the end.


And as the last note. This documentation written in a single unit, owes to be
enough and should cover all the aspects of the language. If it is not enough,
then it should be considered rather as an error of the language itself, as one
of the main purposes of this is to be exceptional easy to start writting within
minutes.

Details about the implementation.
(written in the first days of August of 2021)

This interpreter might be very slow, when it competes with others. I never did
it (benchmark), and probably I never going to do it, but it has to be slow, or
else something is strange.

The code it takes a huuge amount of optimizations at almost at every bit of
its implementation, and here in this domain, every little bit little detail
matters quite a bit of bits.

There was never a design from the scratch, it was written without any sense of
what i would be faced. Added almost every bit, bit by bit and block by block
as was still keeping on to the next bits of thoughts of what i wanted next.
_The whole thing is a big hack_, like to steal in black jack.

There isn't a proper lexer/parser/vm. It is just a bunch of (sometimes clever
though) ways to handle the complexity of an interpreter. There is a weakness
to understand things in theory, as I have to face with the facts, to realize
the theory. Admittedly now, it is a way different than when this thing was
started (at the beginning of the spring of 2021), and quite many things would
be different, if I was started all over again. But the fact is that the code
handles and implements quite satisfactory the syntax and the semantics of a
Programming Language, and it does it without discounts to the exact way I was
looking from an interpreter. As it was the _implentation_ the main intention.
Albeit it does it with a probably unorthodoxical way. And actually it is not
that bad in performance, as it executes more than a thousand of lines of code,
which much of it is complex and loops, in a fraction of a second, in a very
old 32bit netbook computer. And this is enough. Plus it runs on ridiculously
low memory resources.

All the allocated resources are releasing soon as we get out of scope, pretty
much the same C way. So and joking some bits, it really it's the first kind
of a garbage collector of a dynamic language, that really has a real kind of
a deterministic behavior. The question when i was handled this slowly to the
route of the development, was: Do I need this more? If not release it as soon
it goes out of scope.

I mean that was the idea, albeit a bit of total nonsense. This might be the
only thing that still i do not know the way to handle (memory resources) if i
was attempting a rewrite. But i like the idea of "release uneeded resources
as soon as you don't need them anymore". It simplifies a lot of things, and
can end up to a coding expression style, that fits better with that kind of
thing. Albeit it looks that it hurts performance pretty badly. With this so
fragile code state, this can not change drammatically the execution speed,
but if the mind knows that wants to implement this kind of the thing from
the very beggining, then it could influence the scratch code design a lot,
so it might reduce many instructions and conditionals jumps.

At the end, there are many many indirections which are the reusable libraries
of this system.
There is a big convienence to reuse libraries, but if you want speed the first
thing to do is to use own code for specific operations, rather a generic code.
So there is a huge price here.

## Appendix:
At September Equinox day of 2021, the syntax and semantics, are nearly stable.
Very few will change, and a few left to be implemented.
Of course the logical aim here is that both the syntax and semantics will be
freezed in the eternity.

So and as a conclusion, it is the result of the implementation that matters mostly here,
and not the actual implementation, which is not good. This has to be rewritten
normally, but since we live in an abnormal situation and since that seems to
work, quite probably it won't never happen. Though there is a desire.

## The Aim.

Of course to resemble C, with more than a couple of convienences of course, but
with a dynamical way.

Now the underlying C way to write the underlying code and the exposed interface,
the dynamical interface (this language), and the exposed to the user interface
(commands), they should match. All of them.

This allows interconnection with the underlying source code. You all you have
to do is to take the path to the corresponded library -> Class.lib|method and
to the corresponded function.

For instance in our C code we use File.remove (): The library|Class File exposes
this method: This language exposes (through the file module) the same function
with the same name, method and access as File.remove (): And finnally there is
a command with the exact same name and access.

The arguments of the command line, are translated through the usage of qualifiers,
at the end as arguments to the corresponded File Class, that acts on the file
given argument.

This method, it has a default char *argument (the one to be removed), but also
awaits an environment, through designated initializers which are standardised at
least in C11.

```C
typedef struct file_remove_opts {
  int
    force,
    verbose,
    maxdepth,
    curdepth,
    recursive,
    interactive;

  FILE
    *out_stream,
    *err_stream;

  FileInteractive on_interactive;

} file_remove_opts;

#define FileRemoveOpts(...) (file_remove_opts) { \
  .force = OPT_NO_FORCE,                         \
  .verbose = OPT_VERBOSE_ON_ERROR,               \
  .maxdepth = OPT_MAXDEPTH,                      \
  .curdepth = 0,                                 \
  .recursive = OPT_NO_RECURSIVE,                 \
  .interactive = OPT_NO_INTERACTIVE,             \
  .out_stream = stdout,                          \
  .err_stream = stderr,                          \
  .on_interactive = NULL,                        \
  __VA_ARGS__}
```

and here is the output of the File.remove --help command:
```sh

Usage: File.remove [option[s]] file

  -f,--force        if a destination file cannot be opened, remove it and try again
  -R,--recursive    copy recursively
  -i,--interactive  prompt before overwrite
  -v,--verbose=     be verbose
  -h,--help         show this message
```

As you see the command has a required file as argument - same with the underlying
function that does the requested operation - and optional tuning to a default
behavior through the arguments: that become qualifiers in this language: that
finnaly passed with the C's above mechanism, as an environment. There is always
a default which is up to either to the user or to a standard that has this duty
to cherry pick established behavior which has been builted through experience
in real conditions. Of course the user has to have the ability to modify those
semantics as they fit to the specific user personality, with an own responsibility.

So the Aim here is to emphasize the importance of such a mechanism for someone
that wants to take that repsonsibility.

Again, we are talking about the mechanism, not with the actual implementation.

And of course the belief that the source code and the exposed interface its not
that separated. A knowledge of a system, whatever that system is, an operating
computer system, or the ultimate system that govern us all, or our way of living,
give us the feeling that belong to us. Not with the property semantics but with
the servant/server semantics that implements a wish based on the communication.

I'm here, you know me, you can use me and if you care you can modify me to fix
some bugs, or to extend me to with an extented communication protocol, so to try
to make even more things from them we already can do or to do them with a much
more efficient way or even with a much more fancy way, or with a more compact
way, which it can be also a more secure way, or with your own way anyway. At the
very least i'm trying to make it easier for you to know where to find me, if you
want and inclined and for any reason anyway.

This care it is believed that slowly connects the source and the exposed end
execution interface with such way that they can be used interchangeable which
it is in the reality (they actually depend to each other, with a different way
of cource - the source is interested for the development of the mechanics, the
execution environment uses those mechanics to do its thing). Which it is almost
perfect in my humble opinion, if you ever someone will ever ask me.

Plus this gives confidence to the own being, that is fascinated by the way the
things works and feels the magic (which is not that magic - it is just mechanics) -
when sees the result of an own code.

Plus the knowledge and the access to open and free bits of the source code, it
returns a big gift, that is the trust to the system.

In any case, the aim is to offer a mechanism that is trying to show the path
to the user, to find the way into the system that lives and uses.
That's all folks.
