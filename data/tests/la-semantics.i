println ("--[ BEG ]--")

var test_num = 0

func semantics () {

  var retval = 0
  var a = 1

  func fu (a) {
    return a
  }

  test_num += 1
  print ("[${test_num}] testing scope - ")
  retval = fu (11)
  if (retval isnot 11) {
    println (stderr, "[NOTOK] awaiting 11 got: ${retval}");
  } else {
    println ("[OK]")
  }

  var b = 2

  func fua (a, b) {
    return b + a
  }

  test_num += 1
  print ("[${test_num}] testing scope - ")
  retval = fua (11, 12)
  if (retval isnot 23) {
    println (stderr, "[NOTOK] awaiting 23 got: ${retval}")
  } else {
    println ("[OK]")
  }

  func fub (b) {
    return a + b
  }

  test_num += 1
  print ("[${test_num}] testing scope - ")
  retval = fub (10)
  if (retval isnot 11) {
    println (stderr, "[NOTOK] awaiting 11 got: ${retval}")
  } else {
    println ("[OK]")
  }

  func fuc (a, b, c) {
    func fud (a, b, c) {
      return a + b + c
    }

    return fud (a, b, c)
  }

  test_num += 1
  print ("[${test_num}] testing function nesting and scope - ")
  retval = fuc (10, 20, 30)
  if (retval isnot 60) {
    println (stderr, "[NOTOK] awaiting 60 got: ${retval}")
  } else {
    println ("[OK]")
  }

  func fue (c) {
    func fuda (b) {
      return a + b + c
    }

    return fuda (c)
  }

  test_num += 1
  print ("[${test_num}] testing function nesting and scope - ")
  retval = fue (20)
  if (retval isnot 41) {
    println (stderr, "[NOTOK] awaiting 41 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing logical AND - ")
  if ((1 is 1) and (1 is 1) and ((1 is 1) and (2 is 2)) && \
      ((1 isnot 2) && (1 isnot 2))) {
    println ("[OK]")
  } else {
    println (stderr, "[NOTOK]")
  }

  test_num += 1
  print ("[${test_num}] testing logical OR - ")
  if ((2 < 1) or (2 > 2) or (1 isnot 1) or (2 != 2) || \
      ((2 < 2) || (1 > 2) || (1 > 2) || (2 < 1))) {
    println (stderr, "[NOTOK]")
  } else {
    println ("[OK]")
  }

  func fibo_tail (n, a, b) {
    ifnot (n) {
      return a
    }

    if (n is 1) {
      return b
    }

    return fibo_tail (n - 1, b, a + b)
  }

  test_num += 1
  print ("[${test_num}] testing recursive function - ")
  retval = fibo_tail (92, 0, 1)
  if (retval isnot 7540113804746346429) {
    println (stderr, "[NOTOK] awaiting 7540113804746346429 got: ${retval}")
  } else {
    println ("[OK]")
  }

  func fibo_recursive (n) {
    if (n < 2) {
      return n
    }

    return fibo_recursive (n - 1) + fibo_recursive (n - 2)
   }

  test_num += 1
  print ("[${test_num}] testing recursive function - ")
  retval = fibo_recursive (12)
  if (retval isnot 144) {
    println (stderr, "[NOTOK] awaiting 144 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing lambda - ")
  retval = lambda ((x, y) {
    if (x <= 100) {return x * 2} else {return y * 2}}) (100, 200)
  if (retval isnot 200) {
    println (stderr, "[NOTOK] awaiting 200 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing single lambda - ")
  retval = lambda ((x, y) {
    if (x <= 100) {return x * 2} else {return y * 2}}) (101, 200)
  if (retval isnot 400) {
    println (stderr, "[NOTOK] awaiting 400 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing lambdas - ")
  retval = lambda ((x) {return x * 2}) (10) + \
           lambda ((y) {return y / 2}) (20) + \
           lambda ((z) {return z % 2}) (21) + 11
  if (retval isnot 42) {
    println (stderr, "[NOTOK] awaiting 42 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing complex nested lambdas - ")
  retval = lambda ((x, y) {
    var xl = x + y
    return lambda ((k) { return k * 2}) (x) + \
           lambda ((z) {
             var i = lambda ((x) {return x + 100}) (z)
             return (z * 2) + i }) (xl)
  }) (50, 100)
  if (retval isnot 650) {
    println (stderr, "[NOTOK] awaiting 650 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator -= - ")
  retval -= 50
  if (retval isnot 600) {
    println (stderr, "[NOTOK] awaiting 600 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator /= - ")
  retval /= 20
  if (retval isnot 30) {
    println (stderr, "[NOTOK] awaiting 30 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator *= - ")
  retval *= 20
  if (retval isnot 600) {
    println (stderr, "[NOTOK] awaiting 600 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator += - ")
  retval += -600
  if (retval isnot 0) {
    println (stderr, "[NOTOK] awaiting 0 got: ${retval}")
  } else {
    println ("[OK]")
  }

  var v1 = (1 << 0)
  var v2 = (1 << 1)
  var v3 = (1 << 2)

  test_num += 1
  print ("[${test_num}] testing binary assignment operator |= - ")
  retval |= v1
  if (retval isnot 1) {
    println (stderr, "[NOTOK] awaiting 1 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator |= - ")
  retval |= v2
  if (retval isnot 3) {
    println (stderr, "[NOTOK] awaiting 3 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator &= - ")
  retval &= v2
  if (retval isnot 2) {
    println (stderr, "[NOTOK] awaiting 2 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator |= - ")
  retval |= v3
  if (retval isnot 6) {
    println (stderr, "[NOTOK] awaiting 6 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator &= - ")
  retval &= ~(v3|v1|v2)
  if (retval isnot 0) {
    println (stderr, "[NOTOK] awaiting 0 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator |= - ")
  retval |= (v3|v1|v2)
  if ((retval & v1 is 0) or (retval & v2 is 0) or (retval & v3 is 0)) {
    println (stderr, "[NOTOK] awaiting 0 < retval, got: 0")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing bitwise operators - ")
  if ((retval & v1 != 1) or (retval & v2 != 2) or (retval & v3 != 4)) {
    println (stderr, "[NOTOK] awaiting 1 - 2 - 4")
  } else {
    println ("[OK]")
  }

  func loop_test_break (limit) {
    var i = 1;
    while (i) {
      if (i is limit) { break };
      i += 1;
    }

    return i
  }

  test_num += 1
  print ("[${test_num}] testing break statement - ")
  retval = loop_test_break (50)
  if (retval isnot 50) {
    println (stderr, "[NOTOK] awaiting 50 got: ${retval}")
  } else {
    println ("[OK]")
  }

  func loop_test_continue (ar) {
    var i = 0;
    var sum = 0;
    while (i < len (ar)) {
      if (i is 2) { i += 1; continue }
      sum += ar[i]
      i += 1;
    }

    return sum
  }

  test_num += 1
  print ("[${test_num}] testing continue statement - ")
  array integer xi[5] = 1, 10, 20, 30, 40
  retval = loop_test_continue (xi)
  if (retval isnot 81) {
    println (stderr, "[NOTOK] awaiting 81 got: ${retval}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `for` loop - ")
  var sum = lambda ({ var s = 0;
    for (var i = 1; i < 1000 ; s += i, i += 1) {}
    return s
   }) ()

  if (sum isnot 499500) {
    println (stderr, "[NOTOK] awaiting 499500 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `for` loop continue statement - ")
  sum = lambda ({ var s = 0
    for (var i = 1; i < 1000 ; i += 1) {
      if (i is 100) { continue }
      s += i
    }
    return s
   }) ()

  if (sum isnot 499400) {
    println (stderr, "[NOTOK] awaiting 499400 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `for` loop break statement - ")
  sum = lambda ({
    var s = 0
    for (var i = 1; i < 1000 ; s += i, i += 1) {
      if (i is 100) { break }
    }
    return s
   }) ()

  if (sum isnot 4950) {
    println (stderr, "[NOTOK] awaiting 4950 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `for` loop return statement - ")
  sum = lambda ({
    var s = 0
    for (var i = 1; i < 1000 ; s += i, i += 1) {
      if (i is 100) { return s }
    }
    return s
   }) ()

  if (sum isnot 4950) {
    println (stderr, "[NOTOK] awaiting 4950 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `loop` loop first form - ")
  sum = lambda ({
    var s = 0; var i = 0;
    loop (1000) {s += i; i += 1}
    return s
   }) ()

  if (sum isnot 499500) {
    println (stderr, "[NOTOK] awaiting 499500 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `loop` loop second form - ")
  sum = lambda ({
    var s = 0;
    loop (var i = 0; 1000) {s += i; i += 1}
    return s
   }) ()

  if (sum isnot 499500) {
    println (stderr, "[NOTOK] awaiting 499500 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `loop` loop continue statement - ")
  sum = lambda ({
    var s = 0; var i = 0;
    loop (1000) {
      if (i is 100) {
        i += 1
        continue
      }
      s += i
      i += 1
    }

    return s
   }) ()

  if (sum isnot 499400) {
    println (stderr, "[NOTOK] awaiting 499400 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `loop` loop break statement - ")
  sum = lambda ({
    var s = 0; var i = 0;
    loop (1000) {if (i is 100) { break }; s += i; i += 1}
    return s
   }) ()

  if (sum isnot 4950) {
    println (stderr, "[NOTOK] awaiting 4950 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `loop` loop return statement - ")
  sum = lambda ({
    var s = 0; var i = 0;
    loop (1000) { if (i is 100) { return s }; s += i; i += 1}
    return s
   }) ()

  if (sum isnot 4950) {
    println (stderr, "[NOTOK] awaiting 4950 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `forever` loop first form - ")
  sum = lambda ({
    var s = 0; var i = 0;
    forever {s += i; i += 1; if (i is 1000) { break }}
    return s
   }) ()

  if (sum isnot 499500) {
    println (stderr, "[NOTOK] awaiting 499500 got: ${sum}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing `forever` loop second form - ")
  sum = lambda ({
    var s = 0;
    forever (var i = 0) {s += i; i += 1; if (i is 1000) { break }}
    return s
   }) ()

  if (sum isnot 499500) {
    println (stderr, "[NOTOK] awaiting 499500 got: ${sum}")
  } else {
    println ("[OK]")
  }

  var n = 10

  test_num += 1
  print ("[${test_num}] test nested if/else - ")
  if (n isnot 10) {
    println (stderr, "[NOTOK] awaiting n != 10")
  } else {
    if (n isnot 10) {
      println (stderr, "[NOTOK] awaiting n != 10")
    } else {
      if (n isnot 10) {
        println (stderr, "[NOTOK] awaiting n != 10")
      } else {
        if (n isnot 10) {
          println (stderr, "[NOTOK] awaiting n != 10")
        } else {
          if (n is 10) {
            println ("[OK]")
          } else {
            println (stderr, "[NOTOK] awaiting n == 10")
          }
        }
      }
    }
  }

  var r = (-12 + -30)
  test_num += 1
  print ("[${test_num}] testing signed addition - ")
  if (r isnot -42) {
    println (stderr, "[NOTOK] awaiting -42 got ${r}")
  } else {
    println ("[OK]")
  }

  r = (-12 - -30)
  test_num += 1
  print ("[${test_num}] testing signed division - ")
  if (r isnot 18) {
    println (stderr, "[NOTOK] awaiting 18 got ${r}")
  } else {
    println ("[OK]")
  }

  r = (-12 * -30)
  test_num += 1
  print ("[${test_num}] testing singed multiplication - ")
  if (r isnot 360) {
    println (stderr, "[NOTOK] awaiting 360 got ${r}")
  } else {
    println ("[OK]")
  }

  r = (2 * -30)
  test_num += 1
  print ("[${test_num}] testing mixed unsigned and signed multiplication - ")
  if (r isnot -60) {
    println (stderr, "[NOTOK] awaiting -60 got ${r}")
  } else {
    println ("[OK]")
  }

  r = (-12 % -30)
  test_num += 1
  print ("[${test_num}] testing signed modulo - ")
  if (r isnot -12) {
    println (stderr, "[NOTOK] awaiting -12 got ${r}")
  } else {
    println ("[OK]")
  }

  var c = 'α'
  test_num += 1
  print ("[${test_num}] testing char '' (utf8) - ")
  if (c isnot 945) {
    println (stderr, "[NOTOK] awaiting 945 got ${c}")
  } else {
    println ("[OK]")
  }

  c = '\''
  test_num += 1
  print ("[${test_num}] testing single quote as char - ")
  if (c isnot 39) {
    println (stderr, "[NOTOK] awaiting 39 got ${c}")
  } else {
    println ("[OK]")
  }

  func funptr {
    func f1 (f, i, ia, f1, f2) {
      return f (i, ia, f1, f2);
    }

    func f5 (i) {
      return i - i + 1
    }

    func f4 (i, f) {
      return (i * 2) + f (i)
    }

    func f3 (i, f, f1) {
      return (i + 100) + f (i, f1)
    }

    func f2 (i, ia, f, f1) {
      return (i * 2) + f (ia, f1, f5);
    }

    test_num += 1
    print ("[${test_num}] testing function references as function arguments - ")
    var r = f1 (f2, 20, 30, f3, f4)
    if (r isnot 231) {
      println (stderr, "[NOTOK] awaiting 931 got ${r}")
    } else {
      println ("[OK]")
    }

    test_num += 1
    print ("[${test_num}] testing function assignment to a variable - ")
    var fassign = func (s) {return s * 2}
    r = fassign (40)
    if (r isnot 80) {
      println (stderr, "[NOTOK] awaiting 80 got ${r}")
    } else {
      println ("[OK]")
    }

    func tassign (f, i) {return f (i)}
    test_num += 1
    print ("[${test_num}] testing function variable as an argument - ")
    r = tassign (fassign, 400)
    if (r isnot 800) {
      println (stderr, "[NOTOK] awaiting 800 got ${r}")
    } else {
      println ("[OK]")
    }

    var fu = func (x, y) {return x * y}
    test_num += 1
    print ("[${test_num}] testing assignment function to a variable - ")
    r = fu (10, 100)
    if (r isnot 1000) {
      println (stderr, "[NOTOK] awaiting 1000 got ${r}")
    } else {
      println ("[OK]")
    }


  }

  funptr ()

  test_num += 1
  print ("[${test_num}] testing string equality - ")
  var astr = "ταυtoughstuff"
  var bstr = "ταυtoughstuffenough"
  var cstr = "ταυtoughstuffenough"

  if (astr is bstr) {
    println (stderr, "[NOTOK] awaiting inequality")
  }

  if (bstr isnot cstr) {
    println (stderr, "[NOTOK] awaiting equality")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing string length - ")
  retval = len (astr);
  if (retval isnot 16) {
    println (stderr, "[NOTOK] awaiting 16, got ${retval}")
  } else {
    println ("[OK]")
  }


}

semantics ()

func doubles () {
  var d = 113.3131
  var da = 113.3131

  test_num += 1
  print ("[${test_num}] testing Double vars equality - ")
  if (d isnot da) {
    println (stderr, "[NOTOK] awaiting equality")
  } else {
    println ("[OK]")
  }

  func d1 (d) {
    return d + 12
  }

  func d2 (d) {
    return d - 12
  }

  var df = d2
  var dfr = df (da)
  var d2r = d2 (da)

  test_num += 1
  print ("[${test_num}] testing Double function reference - ")
  if (dfr isnot d2r) {
    println (stderr, "[NOTOK] awaiting ${%f, dfr} to be equal with ${%f, d2r}")
  } else {
    println ("[OK]")
  }

  test_num += 1
  print ("[${test_num}] testing actions on doubles - ")
  var dd = d1 (da)
  var dd2r = (df (da) + dd) - da;
  if (da isnot dd2r) {
    println (stderr, "[NOTOK] awaiting ${%f, d} got ${%f, dd2r}")
  } else {
    println ("[OK]")
  }

  da = 1.3131
  test_num += 1
  print ("[${test_num}] testing failed actions on doubles - ")
  dd = d1 (da)
  dd2r = (df (da) + dd) - da;
  if (da isnot dd2r) {
    println (stderr, "[NOTOK] *this should fail*, awaiting ${%f, da} got ${%f, dd2r}")
  } else {
    println ("[OK]")
  }
}

doubles ()

func test_array (length) {
  var retval = 0

  array x[length] = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10

  test_num += 1
  print ("[${test_num}] testing array length - ")
  retval = len (x)
  if (retval isnot length) {
    println (stderr, "[NOTOK] awaiting ${len} got: ${retval}")
  } else {
    println ("[OK]")
  }

  array y[length] = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10

  test_num += 1
  print ("[${test_num}] testing for array equality - ")
  if (x != y) {
    println (stderr, "[NOTOK] awaiting equality")
  }

  y[0] = 11
  if (x == y) {
    println (stderr, "[NOTOK] awaiting inequality")
  } else {
    println ("[OK]")
  }

  func ar_set_at (xx, idx, y) {
    xx[idx] = y
    return xx[idx]
  }

  test_num += 1
  print ("[${test_num}] testing array set|get methods - ")
  retval = ar_set_at (x, length - 1, 20)
  if (retval isnot 20 or (x[length - 1] isnot 20)) {
    println (stderr, "[NOTOK] awaiting 20 got: ${(x[len - 1])}")
  } else {
    println ("[OK]")
  }

  func ar_sum (xx) {
    var sum = 0
    var idx = 0
    var length = len (xx)
    while (idx < length) {
      sum += xx[idx]
      idx += 1
    }
    return sum
  }

  x[0] = 100, 200, 300, 400, 500, 600, 700, 800, 900

  test_num += 1
  print ("[${test_num}] testing array sum - ")
  retval = ar_sum (x)
  if (retval isnot 4520) {
    println (stderr, "[NOTOK] awaiting 4520 got: ${retval}")
  } else {
    println ("[OK]")
  }

  array string xs[4] = "through", "the", "ocean", "drive"
  test_num += 1
  print ("[${test_num}] testing for string equality for string array members - ")
  if ("ocean" isnot xs[2]) {
    println (stderr, "[NOTOK] awaiting equality")
  }

  if (xs[1] is xs[2]) {
    println (stderr, "[NOTOK] awaiting inequality")
  }

  if (xs[1] is 1) {
    println (stderr, "[NOTOK] awaiting inequality")
  } else {
    println ("[OK]")
  }

  func fibo_array (n) {
    array f[n + 2]
    f[0] = 0, 1

    var i = 2
    while (i <= n) {
      f[i] = f[i - 1] + f[i - 2]
      i += 1
    }

    return f[n]
  }

  test_num += 1
  print ("[${test_num}] testing fibonacci array implementation - ")
  retval = fibo_array (92)
  if (retval isnot 7540113804746346429) {
    println (stderr, "[NOTOK] awaiting 7540113804746346429 got: ${retval}")
  } else {
    println ("[OK]")
  }

}

test_array (10)

func types () {
  var str = "string"
  var int = 1
  var num = 1.0
  array i_ar[0]
  array string s_ar[0]
  array number n_ar[0]

  var type = none
  test_num += 1
  print ("[${test_num}] testing types - ")
  if (typeof (type) isnot NoneType) {
    println (stderr, "[NOTOK] awaiting NoneType got: ${(typeof (type))}")
  }
  type = typeof (str)
  if (type isnot StringType) {
    println (stderr, "[NOTOK] awaiting StringType got: ${type}")
  } 
  type = typeof (int)
  if (type isnot IntegerType) {
    println (stderr, "[NOTOK] awaiting IntegerType got: ${type}")
  } 
  type = typeof (num)
  if (type isnot NumberType) {
    println (stderr, "[NOTOK] awaiting NumberType got: ${type}")
  } 
  type = typeof (i_ar)
  if (type isnot ArrayType) {
    println (stderr, "[NOTOK] awaiting ArrayType got: ${type}")
  } 
  type = typeof (s_ar)
  if (type isnot ArrayType) {
    println (stderr, "[NOTOK] awaiting ArrayType got: ${type}")
  } 
  type = typeof (n_ar)
  if (type isnot ArrayType) {
    println (stderr, "[NOTOK] awaiting ArrayType got: ${type}")
  } 
  type = typeofArray (i_ar)
  if (type isnot IntegerType) {
    println (stderr, "[NOTOK] awaiting IntegerType got: ${%s, (typeArrayAsString(i_ar))}")
  } 
  type = typeofArray (s_ar)
  if (type isnot StringType) {
    println (stderr, "[NOTOK] awaiting StringType got: ${%s, (typeArrayAsString(s_ar))}")
  } 
  type = typeofArray (n_ar)
  if (type isnot NumberType) {
    println (stderr, "[NOTOK] awaiting NumberType got: ${%s, (typeArrayAsString(n_ar))}")
  } 
  println ("[OK]")
}

types ()

func valgrind_tests () {
  func xx () {
    array x[2] = 1, 2
    return x
  }

  var ar = xx ()
  ar[1] = 3

  var sa = "A"
  sa = "v"

  func xxx () {
    var s ="xxx"
    return s
  }

  sa = xxx ()
  var sb = xxx ()

  func xxxx () {
    return "xxxx"
  }

  sb = xxxx ()
  var r = sb is sa
}

test_num += 1
valgrind_tests ()
println ("[${test_num}] tests for memory leaks, that should run under valgrind")

println ("--[ END ] --")
