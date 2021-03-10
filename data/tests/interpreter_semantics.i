print ("--[ BEG ]--\n")

func test () {

  var retval = 0
  var a = 1
  var b = 2

  func fu (a) {
    return a
  }

  func fua (a, b) {
    return b + a
  }

  func fub (b) {
    return a + b
  }

  func fuc (a, b, c) {
    func fud (a, b, c) {
      return a + b + c
    }

    return fud (a, b, c)
  }

  func fue (c) {
    func fuda (b) {
      return a + b + c
    }

    return fuda (c)
  }

  func fibo_tail (n, a, b) {
    ifnot (n) {
      return a
    }

    if (n is 1) {
      return b
    }

    return fibo_tail (n -1, b,  a + b)
  }

  func fibo_recursive (n) {
    if (n < 2) {
      return n
    }

    return fibo_recursive (n - 1) + fibo_recursive (n - 2)
   }

  retval = fu (11)
  print ("test 1 - ")

  if (retval isnot 11) {
    print (stderr, "[NOTOK] awaiting 11 got: ${retval}\n");
  } else {
    print ("[OK]\n")
  }

  print ("test 2 - ")
  retval = fua (11, 12)

  if (fua (11, 12) isnot 23) {
    print (stderr, "[NOTOK] awaiting 23 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  print ("test 3 - ")
  retval = fub (10)

  if (retval isnot 11) {
    print (stderr, "[NOTOK] awaiting 11 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  print ("test 4 - ")
  retval = fuc (10, 20, 30)

  if (retval isnot 60) {
    print (stderr, "[NOTOK] awaiting 60 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  print ("test 5 - ")
  retval = fue (20)

  if (retval isnot 41) {
    print (stderr, "[NOTOK] awaiting 41 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  print ("test 6 - ")
  if ((1 is 1) and (1 is 1) and ((1 is 1) and (2 is 2)) && \
      ((1 isnot 2) && (1 isnot 2))) {
    print ("[OK]\n")
  } else {
    print (stderr, "[NOTOK] logical AND\n")
  }

  print ("test 7 - ")
  if ((2 < 1) or (2 > 2) or (1 isnot 1) or (2 != 2) || \
      ((2 lt 2) || (1 gt 2) || (1 ge 2) || (2 le 1))) {
    print (stderr, "[NOTOK] logical OR\n")
  } else {
    print ("[OK]\n")
  }

  print ("test 8 - ")
  retval = fibo_tail (92, 0, 1)
  if (retval isnot 7540113804746346429) {
    print (stderr, "[NOTOK] awaiting 7540113804746346429 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  print ("test 9 - ")
  retval = fibo_recursive (12)
  if (retval isnot 144) {
    print (stderr, "[NOTOK] awaiting 144 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }
}

test ()

func test_array (len) {
  var retval = 0

  array x(len)

  func ar_length () {
    return x(-1)
  }

  print ("test 10 - ")
  retval = ar_length ()
  if (retval isnot len) {
    print (stderr, "[NOTOK] awaiting 10 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  func ar_set_at (idx, y) {
    x(idx) = y
    return x(idx)
  }

  print ("test 11 - ")
  retval = ar_set_at (len - 1, 20)
  if (retval isnot 20) {
    print (stderr, "[NOTOK] awaiting 20 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

  func ar_sum () {
    var sum = 0
    var idx = 0
    while (idx < ar_length ()) {
	    sum = sum + x(idx)
    	idx = idx + 1
    }
    return sum
  }

  x(0) = 100, 200, 300, 400, 500, 600, 700, 800, 900

  print ("test 12 - ")
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
      f (i) = f (i - 1) + f(i - 2)
      i = i + 1
    }

    return f (n)
  }

  print ("test 13 - ")
  retval = fibo_array (92)
  if (retval isnot 7540113804746346429) {
    print (stderr, "[NOTOK] awaiting 7540113804746346429 got: ${retval}\n")
  } else {
    print ("[OK]\n")
  }

}

test_array (10)

print ("--[ END ] --\n")
