print ("--[ BEG ]--\n")

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
    print (stderr, "[NOTOK] awaiting 11 got: ${retval}\n");
  } else {
    print ("[OK]\n")
  }

  var b = 2

  func fua (a, b) {
    return b + a
  }

  test_num += 1
  print ("[${test_num}] testing scope - ")
  retval = fua (11, 12)
  if (retval isnot 23) {
    print (stderr, "[NOTOK] awaiting 23 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  func fub (b) {
    return a + b
  }

  test_num += 1
  print ("[${test_num}] testing scope - ")
  retval = fub (10)
  if (retval isnot 11) {
    print (stderr, "[NOTOK] awaiting 11 got: ${retval}\n")
  } else {
    print ("[OK]\n")
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
    print (stderr, "[NOTOK] awaiting 60 got: ${retval}\n")
  } else {
    print ("[OK]\n")
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
    print (stderr, "[NOTOK] awaiting 41 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing logical AND - ")
  if ((1 is 1) and (1 is 1) and ((1 is 1) and (2 is 2)) && \
      ((1 isnot 2) && (1 isnot 2))) {
    print ("[OK]\n")
  } else {
    print (stderr, "[NOTOK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing logical OR - ")
  if ((2 < 1) or (2 > 2) or (1 isnot 1) or (2 != 2) || \
      ((2 lt 2) || (1 gt 2) || (1 ge 2) || (2 le 1))) {
    print (stderr, "[NOTOK]\n")
  } else {
    print ("[OK]\n")
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
    print (stderr, "[NOTOK] awaiting 7540113804746346429 got: ${retval}\n")
  } else {
    print ("[OK]\n")
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
    print (stderr, "[NOTOK] awaiting 144 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing lambda - ")
  retval = lambda ((x, y) {
    if (x <= 100) {return x * 2} else {return y * 2}}) (100, 200)
  if (retval isnot 200) {
    print (stderr, "[NOTOK] awaiting 200 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing single lambda - ")
  retval = lambda ((x, y) {
    if (x <= 100) {return x * 2} else {return y * 2}}) (101, 200)
  if (retval isnot 400) {
    print (stderr, "[NOTOK] awaiting 400 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing lambdas - ")
  retval = lambda ((x) {return x * 2}) (10) + \
           lambda ((y) {return y / 2}) (20) + \
           lambda ((z) {return z % 2}) (21) + 11
  if (retval isnot 42) {
    print (stderr, "[NOTOK] awaiting 42 got: ${retval}\n")
  } else {
    print ("[OK]\n")
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
    print (stderr, "[NOTOK] awaiting 650 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator -= - ")
  retval -= 50
  if (retval isnot 600) {
    print (stderr, "[NOTOK] awaiting 600 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator /= - ")
  retval /= 20
  if (retval isnot 30) {
    print (stderr, "[NOTOK] awaiting 30 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator *= - ")
  retval *= 20
  if (retval isnot 600) {
    print (stderr, "[NOTOK] awaiting 600 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator += - ")
  retval += -600
  if (retval isnot 0) {
    print (stderr, "[NOTOK] awaiting 0 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  var v1 = (1 << 0)
  var v2 = (1 << 1)
  var v3 = (1 << 2)

  test_num += 1
  print ("[${test_num}] testing binary assignment operator |= - ")
  retval |= v1
  if (retval isnot 1) {
    print (stderr, "[NOTOK] awaiting 1 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator |= - ")
  retval |= v2
  if (retval isnot 3) {
    print (stderr, "[NOTOK] awaiting 3 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator &= - ")
  retval &= v2
  if (retval isnot 2) {
    print (stderr, "[NOTOK] awaiting 2 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator |= - ")
  retval |= v3
  if (retval isnot 6) {
    print (stderr, "[NOTOK] awaiting 6 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator &= - ")
  retval &= ~(v3|v1|v2)
  if (retval isnot 0) {
    print (stderr, "[NOTOK] awaiting 0 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing binary assignment operator |= - ")
  retval |= (v3|v1|v2)
  if ((retval & v1 is 0) or (retval & v2 is 0) or (retval & v3 is 0)) {
    print (stderr, "[NOTOK] awaiting 0 < retval, got: 0\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing bitwise operators - ")
  if ((retval & v1 != 1) or (retval & v2 != 2) or (retval & v3 != 4)) {
    print (stderr, "[NOTOK] awaiting 1 - 2 - 4\n")
  } else {
    print ("[OK]\n")
  }

  var n = 10

  test_num += 1
  print ("[${test_num}] test nested if/else - ")
  if (n isnot 10) {
    print (stderr, "[NOTOK] awaiting n != 10\n")
  } else {
    if (n isnot 10) {
      print (stderr, "[NOTOK] awaiting n != 10\n")
    } else {
      if (n isnot 10) {
        print (stderr, "[NOTOK] awaiting n != 10\n")
      } else {
        if (n isnot 10) {
          print (stderr, "[NOTOK] awaiting n != 10\n")
        } else {
          if (n is 10) {
            print ("[OK]\n")
          } else {
            print (stderr, "[NOTOK] awaiting n == 10\n")
          }
        }
      }
    }
  }

  var r = (-12 + -30)
  test_num += 1
  print ("[${test_num}] testing signed addition - ")
  if (r isnot -42) {
    print (stderr, "[NOTOK] awaiting -42 got ${r}\n")
  } else {
    print ("[OK]\n")
  }

  r = (-12 - -30)
  test_num += 1
  print ("[${test_num}] testing signed division - ")
  if (r isnot 18) {
    print (stderr, "[NOTOK] awaiting 18 got ${r}\n")
  } else {
    print ("[OK]\n")
  }

  r = (-12 * -30)
  test_num += 1
  print ("[${test_num}] testing singed multiplication - ")
  if (r isnot 360) {
    print (stderr, "[NOTOK] awaiting 360 got ${r}\n")
  } else {
    print ("[OK]\n")
  }

  r = (2 * -30)
  test_num += 1
  print ("[${test_num}] testing mixed unsigned and signed multiplication - ")
  if (r isnot -60) {
    print (stderr, "[NOTOK] awaiting -60 got ${r}\n")
  } else {
    print ("[OK]\n")
  }

  r = (-12 % -30)
  test_num += 1
  print ("[${test_num}] testing signed modulo - ")
  if (r isnot -12) {
    print (stderr, "[NOTOK] awaiting -12 got ${r}\n")
  } else {
    print ("[OK]\n")
  }

  var c = 'α'
  test_num += 1
  print ("[${test_num}] testing char '' (utf8) - ")
  if (c isnot 945) {
    print (stderr, "[NOTOK] awaiting 945 got ${c}\n")
  } else {
    print ("[OK]\n")
  }
}

semantics ()

func test_array (len) {
  var retval = 0

  array x(len) = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10

  func ar_length () {
    return x(-1)
  }

  test_num += 1
  print ("[${test_num}] testing array length - ")
  retval = ar_length ()
  if (retval isnot len) {
    print (stderr, "[NOTOK] awaiting ${len} got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  func ar_set_at (idx, y) {
    x(idx) = y
    return x(idx)
  }

  test_num += 1
  print ("[${test_num}] testing array set index - ")
  retval = ar_set_at (len - 1, 20)
  if (retval isnot 20) {
    print (stderr, "[NOTOK] awaiting 20 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  test_num += 1
  print ("[${test_num}] testing array get index - ")
  retval = x(len - 1)
  if (retval isnot 20) {
    print (stderr, "[NOTOK] awaiting 20 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  func ar_sum () {
    var sum = 0
    var idx = 0
    var len = ar_length ()
    while (idx < len) {
	    sum += x(idx)
    	idx += 1
    }
    return sum
  }

  x(0) = 100, 200, 300, 400, 500, 600, 700, 800, 900

  test_num += 1
  print ("[${test_num}] testing array sum - ")
  retval = ar_sum ()
  if (retval isnot 4520) {
    print (stderr, "[NOTOK] awaiting 4520 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  func fibo_array (n) {
    array f (n + 2)
    f(0) = 0, 1

    var i = 2
    while (i <= n) {
      f (i) = f (i - 1) + f (i - 2)
      i += 1
    }

    return f (n)
  }

  test_num += 1
  print ("[${test_num}] testing fibonacci array implementation - ")
  retval = fibo_array (92)
  if (retval isnot 7540113804746346429) {
    print (stderr, "[NOTOK] awaiting 7540113804746346429 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }
}

test_array (10)

print ("--[ END ] --\n")
