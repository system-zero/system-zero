First draft of an extended supported interface:
```js
  # Anonymous functions.

  var v = lambda (x, y) { return x * y } (10, 100) # => 1000

  # A lambda function, it is like a function without a name, but it is called
  # immediately. After the call release the resources. It is illegal to store
  # a lambda in a variable. It is also illegal to omit the argument list after
  # the body, even if it is an empty list, so a pair of parentheses is obligatory.

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


```
