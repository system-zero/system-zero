var run_invalid_memory_read_tests = 0
var run_fileptr_tests = 0
var run_valgrind_tests = 1
var run_development_tests = 1

var test_num = 0

array RESULTS[256];
RESULTS[*] = ok

var EXPECTED_TO_FAIL = 1

func assert_true (msg, expr) {
  test_num += 1
  print ("[${test_num}] ${%s, msg} - ")
  if (expr is false) {
    println (stderr, "[NOTOK] awaiting true got false")
    RESULTS[test_num - 1] = notok
    return
  }

  println ("[OK]")
}

func assert_true_msg (msg, msg_on_error, expr) {
  test_num += 1
  print ("[${test_num}] ${%s, msg} - ")
  if (expr is false) {
    RESULTS[test_num - 1] = notok
    println (stderr, "[NOTOK] ${%s, msg_on_error}")
    return
  }

  println ("[OK]")
}

func assert_false (msg, expr) {
  test_num += 1
  print ("[${test_num}] ${%s, msg} - ")
  if (expr is true) {
    RESULTS[test_num - 1] = notok
    println (stderr, "[NOTOK] awaiting false got true")
    return
  }

  println ("[OK]")
}

func assert_equal (msg, a_expr, b_expr) {
  test_num += 1
  print ("[${test_num}] ${%s, msg} - ")
  if (a_expr isnot b_expr) {
    RESULTS[test_num - 1] = notok
    println (stderr, "[NOTOK] awaiting equality")
    return
  }

  println ("[OK]")
}

func assert_not_equal (msg, a_expr, b_expr) {
  test_num += 1
  print ("[${test_num}] ${%s, msg} - ")
  if (a_expr is b_expr) {
    RESULTS[test_num - 1] = notok
    println (stderr, "[NOTOK] awaiting equality")
    return
  }

  println ("[OK]")
}

func assert_equal_msg (msg, msg_on_error, a_expr, b_expr) {
  test_num += 1
  print ("[${test_num}] ${%s, msg} - ")
  if (a_expr isnot b_expr) {
    RESULTS[test_num - 1] = notok
    println (stderr, "[NOTOK] ${%s, msg_on_error}")
    return
  }

  println ("[OK]")
}

func expected_to_fail (msg, msg_on_error, a_expr, b_expr) {
  test_num += 1
  print ("[${test_num}] ${%s, msg} - ")
  if (a_expr isnot b_expr) {
    RESULTS[test_num - 1] = EXPECTED_TO_FAIL
    println (stderr, "[NOTOK] ${%s, msg_on_error}")
    return
  }

  println ("[OK]")
}

func semantics () {
  var retval = 0
  var a = 1

  func fu (a) {
    return a
  }

  retval = fu (11)
  assert_true ("testing scope", retval is 11)

  var b = 2

  func fua (a, b) {
    return b + a
  }

  retval = fua (11, 12)
  assert_true ("testing scope", retval is 23)

  func fub (b) {
    return a + b
  }

  retval = fub (10)
  assert_true ("testing scope", retval is 11)

  func fuc (a, b, c) {
    func fud (a, b, c) {
      return a + b + c
    }

    return fud (a, b, c)
  }

  retval = fuc (10, 20, 30)
  assert_true ("testing function nesting and scope", retval is 60)

  var fue = func (c) {
    var fuda = func (b) {
      return a + b + c
    }

    return fuda (c)
  }

  retval = fue (20)
  assert_true ("testing function nesting and scope", retval is 41)

  assert_true ("testing logical AND", 1 is 1 and 1 is 1 and
      (1 is 1 and 2 is 2) && (1 isnot 2 && 1 isnot 2))

  assert_false ("testing logical OR", 2 < 1 or 2 > 2 or 1 isnot 1 or 2 != 2 ||
      2 < 2 || 1 > 2 || 1 > 2 || 2 < 1)

  func fibo_tail (n, a, b) {
    ifnot (n) {
      return a
    }

    if (n is 1) {
      return b
    }

    return fibo_tail (n - 1, b, a + b)
  }

  retval = fibo_tail (92, 0, 1)
  assert_true ("testing recursive function", retval is 7540113804746346429)

  func fibo_recursive (n) {
    if (n < 2) {
      return n
    }

    return fibo_recursive (n - 1) + fibo_recursive (n - 2)
   }

  retval = fibo_recursive (12)
  assert_true ("testing recursive function", retval is 144)

  retval = lambda ((x, y) {
    if (x <= 100) {return x * 2} else {return y * 2}}) (100, 200)

  assert_true ("testing lambda", retval is 200)

  retval = lambda ((x, y) {
    if (x <= 100) {return x * 2} else {return y * 2}}) (101, 200)

  assert_true ("testing single lambda", retval is 400)

  retval = lambda ((x) {return x * 2}) (10) +
           lambda ((y) {return y / 2}) (20) +
           lambda ((z) {return z % 2}) (21) + 11

  assert_true ("testing lambdas", retval is 42)

  retval = lambda ((x, y) {
    var xl = x + y
    return lambda ((k) { return k * 2}) (x) +
           lambda ((z) {
             var i = lambda ((x) {return x + 100}) (z)
             return (z * 2) + i }) (xl)
  }) (50, 100)

  assert_true ("testing complex nested lambdas", retval is 650)

  retval -= 50
  assert_true ("testing binary assignment operator [-=]", retval is 600)
  retval /= 20
  assert_true ("testing binary assignment assignment operator [/=]", retval is 30)
  retval *= 20
  assert_true ("testing binary assignment assignment operator [*=]", retval is 600)
  retval += -600
  assert_true ("testing binary assignment assignment operator [+=]", retval is 0)

  var v1 = (1 << 0)
  var v2 = (1 << 1)
  var v3 = (1 << 2)

  retval |= v1
  assert_true ("testing binary assignment assignment operator [|=]", retval is 1)
  retval |= v2
  assert_true ("testing binary assignment assignment operator [|=]", retval is 3)
  retval &= v2
  assert_true ("testing binary assignment assignment operator [&=]", retval is 2)
  retval |= v3
  assert_true ("testing binary assignment assignment operator [|=]", retval is 6)
  retval &= ~(v3|v1|v2)
  assert_true ("testing binary assignment assignment operator [&=]", retval is 0)
  retval |= (v3|v1|v2)
  assert_false ("testing binary assignment assignment operator [|=]",
      (retval & v1 is 0) or (retval & v2 is 0) or (retval & v3 is 0))
  assert_false ("testing bitwise operators",
      (retval & v1 != 1) or (retval & v2 != 2) or (retval & v3 != 4))

  func loop_test_break (limit) {
    var i = 1
    while (i) {
      if (i is limit) { break };
      i += 1;
    }

    return i
  }

  retval = loop_test_break (50)
  assert_true ("testing break statement", retval is 50)

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

  var xi = [1, 10, 20, 30, 40]
  retval = loop_test_continue (xi)
  assert_true ("testing continue statement", retval is 81)

  var sum = lambda ({ var s = 0;
    for (var i = 1; i < 1000 ; s += i, i += 1) {}
    return s
   }) ()

  assert_true ("testing `for` loop", sum is 499500)
  sum = lambda ({ var s = 0
    for (var i = 1; i < 1000 ; i += 1) {
      if (i is 100) { continue }
      s += i
    }
    return s
   }) ()

  assert_true ("testing `for` loop continue statement", sum is 499400)

  sum = lambda ({
    var s = 0
    for (var i = 1; i < 1000 ; s += i, i += 1) {
      if (i is 100) { break }
    }
    return s
   }) ()

  assert_true ("testing `for` loop break statement", sum is 4950)

  sum = lambda ({
    var s = 0
    for (var i = 1; i < 1000 ; s += i, i += 1) {
      if (i is 100) { return s }
    }
    return s
   }) ()

  assert_true ("testing `for` loop return statement", sum is 4950)

  sum = lambda ({
    var s = 0; var i = 0;
    loop (1000) {s += i; i += 1}
    return s
   }) ()

  assert_true ("testing `loop` loop first form", sum is 499500)

  sum = lambda ({
    var s = 0;
    loop (var i = 0; 1000) {s += i; i += 1}
    return s
   }) ()

  assert_true ("testing `loop` loop second form", sum is 499500)

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

  assert_true ("testing `loop` loop continue statement", sum is 499400)

  sum = lambda ({
    var s = 0; var i = 0;
    loop (1000) {if (i is 100) { break }; s += i; i += 1}
    return s
   }) ()

  assert_true ("testing `loop` loop break statement", sum is 4950)

  sum = lambda ({
    var s = 0; var i = 0;
    loop (1000) { if (i is 100) { return s }; s += i; i += 1}
    return s
   }) ()

  assert_true ("testing `loop` loop return statement", sum is 4950)

  sum = lambda ({
    var s = 0; var i = 0;
    forever {s += i; i += 1; if (i is 1000) { break }}
    return s
   }) ()

  assert_true ("testing `forever` loop first form", sum is 499500)

  sum = lambda ({
    var s = 0;
    forever (var i = 0) {s += i; i += 1; if (i is 1000) { break }}
    return s
   }) ()


  assert_true ("testing `forever` loop second form", sum is 499500)

  sum = lambda ({
   var s = 0; var i = 0
    do {
      s += i
      i += 1
    } while (i < 1000)

    return s
   }) ()


  assert_true ("testing `do/while` loop", sum is 499500)

  sum = lambda ({ var s = 0; var i = 0;
    do {
      if (i is 100) { i += 1; continue }
      s += i
      i += 1
    } while (i < 1000)
    return s
   }) ()

  assert_true ("testing `do/while` loop continue statement", sum is 499400)

  sum = lambda ({
    var s = 0; var i = 0
    do {
      if (i is 100) { break }
      s += i
      i += 1
    } while (i < 1000)
    return s
   }) ()

  assert_true ("testing `do/while` loop break statement", sum is 4950)

  sum = lambda ({
    var s = 0; var i = 0;
    do {
      s += i
      i += 1
      if (i is 100) { return s }
    } while (i < 1000)
    return s
   }) ()

  assert_true ("testing `do/while` loop return statement", sum is 4950)

  var n = 10

  var nifelse = lambda ((n) {
    if (n isnot 10) {
      return false
    } else {
      if (n isnot 10) {
        return false
      } else {
        if (n isnot 10) {
          return false
        } else {
          if (n isnot 10) {
            return false
          } else {
            if (n is 10) {
              return true
            } else {
              return false
            }
          }
        }
      }
    }
  })

  assert_true ("testing nested if/else",  nifelse (n))
  assert_false("testing nested if/else",  nifelse (n - 1))

  var felseifnot = lambda ((n) {
    if (n isnot 10) {
      return false
    } else ifnot (n is 10) {
      return false
    } else {
      ifnot (n is 10) {
        return false
      } else if (n is 10) {
        return true
      }
    }
  })

  assert_true ("testing else if[not]", felseifnot (n))
  assert_false ("testing else if[not]", felseifnot (n + 1))

  var r = (-12 + -30)
  assert_true ("testing signed addition", r is -42)

  r = (-12 - -30)
  assert_true ("testing signed substraction", r is 18)

  r = (-168 / -4)
  assert_true ("testing signed division", r is 42)

  r = (-12 * -30)
  assert_true ("testing signed multiplication", r is 360)

  r = (2 * -30)
  assert_true ("testing mixed unsigned and signed multiplication", r is -60)

  r = (-12 % -30)
  assert_true ("testing mixed unsigned and signed modulo", r is -12)

  var c = 'α'
  assert_true ("testing char '' (utf8)", c is 945)

  c = '\''
  assert_true ("testing single quote as char", c is 39)

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

    var r = f1 (f2, 20, 30, f3, f4)

    assert_true ("testing function references as function arguments", r is 231)

    var fassign = func (s) {return s * 2}
    r = fassign (40)
    assert_true ("testing function assignment to a variable", r is 80)

    func tassign (f, i) {return f (i)}
    r = tassign (fassign, 400)

    var fu = func (x, y) {return x * y}
    r = fu (10, 100)
    assert_true ("testing assignment function to a variable", r is 1000)
  }

  funptr ()

  var astr = "ταυtoughstuff"
  var bstr = "ταυtoughstuffenough"
  var cstr = "ταυtoughstuffenough"

  assert_not_equal ("testing string equality", astr, bstr)
  assert_equal ("testing string equality", bstr, cstr)

  retval = len (astr);
  assert_true ("testing string length", retval is 16)

  var str = "asdf"
  retval = str[-1]
  assert_true ("testing string indices [get]", retval is 'f')

  str[-1] = 'g'
  retval = str[-1]
  assert_true ("testing string indices [set]", retval is 'g')

  retval = 200 - (str[0] + 3)
  assert_true ("testing string indices [statement]", retval is 100)

  var str_h = "ba"
  str_h += "μπάκι"
  str_h += 'α'
  var s_str = " and μπακακάκια"
  str_h += s_str

  assert_true ("testing string appending operator [+=]", str_h is "baμπάκια and μπακακάκια")

  assert_true_msg ("testing string appending operator [+=]",
     "probably a freed variable that shouldn't be freed",  s_str is " and μπακακάκια")

  var str_hh = "κοάξ"

  str_h = '"' + "Βρεκεκεκὲ" + 'ξ' + ' ' + "κοὰξ" + ' ' + str_hh + '"'

  assert_true ("testing string concatenation using the [+] operator",
      str_h is "\"Βρεκεκεκὲξ κοὰξ κοάξ\"")

  retval = __argc is 1
  assert_true  ("testing __argc and __argv", retval)
  assert_equal ("testing __argc and __argv", len (__argv), 1)
  assert_equal ("testing __argc and __argv", __argv[0], "data/tests/la-semantics.i")

  assert_equal_msg ("testing __func__",
      format ("awaiting |assert_equal_msg|, got |${%s, __func__}|"),
      __func__, "assert_equal_msg")

  loadfile ("loaded")

  retval = loaded_f (10)
  assert_true ("testing loadfile function", retval is 20)

  var fmt = format ("${%s, load_va} ${%s, format (load_vc)}")
  assert_true ("testing format function with an embedded call",  fmt is "string is 6 bytes length")

  fmt = format ("${%s, load_va} ${%s, load_vd}")
  assert_true ("testing format function",  fmt is "string is 6 bytes length")
  fmt = format ("${%s, load_va} ${%s, load_vd}")
  assert_true ("testing format function",  fmt is "string is 6 bytes length")

  var dollars = "$$$$$$ $$$$$$ are 12 dollars"
  assert_true ("testing format function with escaped characters", load_vf is dollars)

  var fmtexp = format ("${10 * 100 / 2 - 500 + 100 - [1, 2, 50][2] - 8}")
  assert_true ("testing expressions to the format function", "42" is fmtexp)

  assert_true ("testing stdin and fileno", 0 is fileno (stdin))

  public var v_visible = "visible variable"

  public func f_visible (x) {
    return x * 2
  }

  public array a_visible[2] = [1, 2]
  public const c_visible = "visible constant"
}

semantics ()

assert_equal ("testing public variable attribute", v_visible, "visible variable")
assert_equal ("testing public function attribute", f_visible (22), 44)
assert_equal ("testing public array attribute", a_visible[0], 1)
assert_equal ("testing public constant attribute", c_visible, "visible constant")

func doubles () {
  var d = 113.3131
  var da = 113.3131

  assert_equal ("testing Double vars equality", d, da)

  func d1 (d) {
    return d + 12
  }

  func d2 (d) {
    return d - 12
  }

  var df = d2
  var dfr = df (da)
  var d2r = d2 (da)

  assert_equal ("testing Double function reference", dfr, d2r)

  var dd = d1 (da)
  var dd2r = (df (da) + dd) - da;
  assert_equal ("testing operations on doubles", da, dd2r)

  da = 1.3131

  dd = d1 (da)
  dd2r = (df (da) + dd) - da;
  expected_to_fail ("testing expected to fail operations on doubles",
      format ("awaiting ${%f, da} got ${%f, dd2r}"), da, dd2r)
}

doubles ()

func test_array (length) {
  var retval = 0

  var ar = ["a", "b"]
  assert_true ("testing array", typeof (ar) is ArrayType)
  assert_true ("testing array", typeofArray (ar) is StringType)
  assert_true ("testing array get", "a" is ar[0])
  assert_true ("testing anonymous array", typeofArray ([1.1, 2.2, 3.3]) is NumberType)
  assert_true ("testing anonymous array", 23 is [1, 10, 23, 43][2])

  array x[length] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

  retval = len (x)
  assert_true ("testing array length", retval is length)

  array y[length] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

  assert_true ("testing array equality", x is y)

  y[0] = 11

  assert_false ("testing array inequality", x is y)

  func ar_set_at (xx, idx, y) {
    xx[idx] = y
    return xx[idx]
  }

  retval = ar_set_at (x, length - 1, 20)
  assert_true ("testing array set|get methods", (retval is 20) and
      (x[length - 1] is 20))

  func ar_sum (xx) {
    var sum = 0
    var idx = 0
    var length = len (xx)
    for (idx = 0; idx < length; idx += 1) {
      sum += xx[idx]
    }

    return sum
  }

  x[0:8] = [100, 200, 300, 400, 500, 600, 700, 800, 900]

  assert_equal ("testing array sum", 4520, ar_sum (x))

  x[6:7] = [10, 20]

  assert_equal ("testing array sum and setting indices", 3050, ar_sum (x))

  array string xs[4] = ["through", "the", "ocean", "drive"]

  assert_equal ("testing for string equality for string array members",
      "ocean", xs[2])

  assert_not_equal ("testing for string inequality for string array members",
      xs[1], xs[2])

  assert_false ("testing for inequality for array members",
      xs[1] is 1)

  array i_ar[4]
  i_ar[*] = 31
  assert_equal ("testing array set '*' operator", ar_sum (i_ar), 124)

  func fibo_array (n) {
    array f[n + 2]
    f[0:1] = [0, 1]

    var i = 2
    while (i <= n) {
      f[i] = f[i - 1] + f[i - 2]
      i += 1
    }

    return f[n]
  }

  retval = fibo_array (92)
  assert_equal ("testing fibonacci array implementation",
      retval, 7540113804746346429)
}

test_array (10)

func test_maps {
  var m = {}

  m.a = 1
  m.b = "string"
  m.c = func { return this.a + 999 }

  assert_true ("testing map",
      m.a is 1 and
      m.b is "string" and
      m.c () is 1000)

  var map = {
    private "one" = 1,
    "two" = 2,
    private "three" = "three",
    private "fun" = func { return (this.one + this.two) * 10000 }
    "threefun" = func { return this.three },
    "fn" = func (x) { return x * this.fun () }
  }

  assert_true ("testing map functions", map.fn (2) is 60000 and map.two is 2)

  map.new = "new"
  assert_true ("testing map extension", map.new is "new")

  map.newfun = func (x) { return (x * 2) + this.one }
  assert_true ("testing map new function and visibility", map.newfun (10) is 21)

  func x (m) {
    return m.two
  }

  assert_true ("testing map as a function argument", x (map) is 2 and x (map) is map.two)

  func xx () {
    var m = {"1" = 11, "2" = 2, "k" = 10}
    return m
  }

  var mapf = xx ()
  assert_true ("testing returning map from a function", mapf.1 is 11 and
      typeof (mapf) is MapType)

  func mapfa () {
    return {"11" = 11, "12" = 12}
  }

  var el = mapfa () 
  assert_true ("testing returning anonymous map from a function", el.11 is 11)

  func x (a) {
    return a.key
  }

  assert_true ("testing anonymous map", x ({"key" = 22, "aa" = 2}) is 22)

}

test_maps ()

func types () {
  var str = "string"
  var int = 1
  var num = 1.0
  array i_ar[1] = [0]
  array string s_ar[1] = ["str"]
  array number n_ar[1] = [10.0]
  var map = {}

  var type = none

  assert_true ("testing NoneType[s]", typeof (type) is NoneType and
     typeAsString (none) is "NoneType")
  type = typeof (str)
  assert_true ("testing StringType[s]", type is StringType and
     typeAsString (str) is "StringType")
  type = typeof (int)
  assert_true ("testing IntegerType[s]", type is IntegerType and
      typeAsString (1) is "IntegerType")
  type = typeof (num)
  assert_true ("testing NumberType[s]", type is NumberType and
      typeAsString (1.1) is "NumberType")
  type = typeof (i_ar)
  assert_true ("testing ArrayType[s]", type is ArrayType and
      typeAsString (i_ar) is "ArrayType")
  type = typeofArray (i_ar)
  assert_true ("testing Array Integer sub types", type is IntegerType and
      typeAsString (i_ar[0]) is "IntegerType")
  type = typeofArray (s_ar)
  assert_true ("testing array String sub types", type is StringType and
      typeAsString (s_ar[0]) is "StringType")
  type = typeofArray (n_ar)
  assert_true ("testing array Number sub types", type is NumberType and
      typeAsString (n_ar[0]) is "NumberType")
  assert_true ("testing map type", typeof (map) is MapType and
      typeAsString (map) is "MapType")
}

types ()

func valgrind_tests () {
  func xx () {
    array x[2] = [1, 2]
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

  var s = "asdf"[0]

  func vv () {
    return "asdf"
  }

  var f = format ("${%s, vv()}")

  var saa = ("a" is "a" and "v" is "v" or "x" is "x")
  var sab = "z" + 'x' + 'c' + 'v'
}

if (run_valgrind_tests) {
  test_num += 1
  valgrind_tests ()
  println ("[${test_num}] tests for memory leaks, that should run under valgrind")
}

func invalid_memory_read_tests {
  func invalid_memory_read_tests1 {
    func fff () {
      return fopen ("/tmp/la_test_fopen", "w");
    }

    func p (fp) {
      println (fp, "fptype ${%s, typeAsString (fp)}")
    }

    var f = fff()
    p (f)
  }

  func invalid_memory_read_tests2 {
    func fff () {
      return fopen ("/tmp/la_test_fopen", "w");
    }

    func p (fp) {
      println (fp, "fptype ${%s, typeAsString (fp)}")
    }

    p (fff ())
  }

  func invalid_memory_read_tests3 {
    func fff () {
      var fp = fopen ("/tmp/la_test_fopen", "w");
      return fp
    }

    func p (fp) {
      println (fp, "fptype ${%s, typeAsString (fp)}")
    }

    p (fff ())
  }

  func invalid_memory_read_tests4 {
    func fff () {
      var fp = fopen ("/tmp/la_test_fopen", "w");
      return fp
    }

    func p (fp) {
      println (fp, "fptype ${%s, typeAsString (fp)}")
    }

    var f = fff ()
    p (f)
  }

  invalid_memory_read_tests1 ()
  invalid_memory_read_tests2 ()
  invalid_memory_read_tests3 ()
  invalid_memory_read_tests4 ()
}

if (run_invalid_memory_read_tests and run_fileptr_tests) {
  test_num += 1
  invalid_memory_read_tests ()
  println ("[${test_num}] tests for invalid memory read tests")
}

func summary () {
  var num_passed = 0
  var num_failed = 0
  var num_expected_to_fail = 0
  var output = format ("====- TEST SUMMARY -====\nNUM TESTS: ${test_num}\n   PASSED:")
  var failed_output = ""
  var expected_to_fail_output = ""

  for (var i = 0; i < test_num; i += 1) {
    if (RESULTS[i] is ok) {
      num_passed += 1
      continue
    }

    if (RESULTS[i] is notok) {
      num_failed += 1
      failed_output += format ("[${i + 1}] ")
      continue
    }

    num_expected_to_fail += 1
    num_failed += 1
    failed_output += format ("[${i + 1}] ")
    expected_to_fail_output += format ("[${i + 1}] ")
  }

  output += format (" ${num_passed}\n   FAILED: ${num_failed}, ${%s, failed_output}\n")
  output += format (" EXPECTED: ${num_expected_to_fail}, ${%s, expected_to_fail_output}")
  println ("${%s, output}")
}

summary ()

func development () {
  var term = term_new ()
  println ("press q to exit")
  if (notok is term_raw_mode (term)) {
    return notok
  }

  var fd = fileno (stdin)
  var key = 0
  while (key isnot 'q') {
    if (key is notok) {
      return notok
    }

    key = getkey (fd)
  }

  return term_sane_mode (term)
}

if (run_development_tests) {
  if (notok is development ()) {
    println ("terminal error")
  }
}
