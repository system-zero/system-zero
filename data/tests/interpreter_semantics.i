
func ok () {
  println_str ("[OK]")
}

func notok (msg, val) {
  print_str ("[NOTOK] ")
  print_str (msg);
  println (val)
}

println_str ("--[ BEG ]--")

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

  print_str ("test 1 - ")
  retval = fu (11)

  if (retval isnot 11) {
    notok ("awaiting 11 got: ", retval)
  } else {
    ok ()
  }

  print_str ("test 2 - ")
  retval = fua (11, 12)

  if (fua (11, 12) isnot 23) {
    notok ("awaiting 23 got: ", retval)
  } else {
    ok ()
  }

  print_str ("test 3 - ")
  retval = fub (10)

  if (retval isnot 11) {
    notok ("awaiting 11 got: ", retval)
  } else {
    ok ()
  }

  print_str ("test 4 - ")
  retval = fuc (10, 20, 30)

  if (retval isnot 60) {
    notok ("awaiting 60 got: ", retval)
  } else {
    ok ()
  }

  print_str ("test 5 - ")
  retval = fue (20)

  if (retval isnot 41) {
    notok ("awaiting 50 got: ", retval)
  } else {
    ok ()
  }

  print_str ("test 6 - ")
  if ((1 is 1) and (1 is 1) and ((1 is 1) and (2 is 2)) && \
      ((1 isnot 2) && (1 isnot 2))) {
    ok ()
  } else {
    println_str ("[NOTOK] logical AND")
  }

  print_str ("test 7 - ")
  if ((2 < 1) or (2 > 2) or (1 isnot 1) or (2 != 2) || \
      ((2 lt 2) || (1 gt 2) || (1 ge 2) || (2 le 1))) {
    println_str ("[NOTOK] logical OR")
  } else { ok () }

}

test ()

func test_array (len) {
  var retval = 0

  array x(len)

  func ar_length () {
    return x(-1)
  }

  print_str ("test 8 - ")
  retval = ar_length ()
  if (retval isnot len) {
    notok ("awaiting 10 got: ", retval)
  } else { ok () }

  func ar_set_at (idx, y) {
    x(idx) = y
    return x(idx)
  }

  print_str ("test 9 - ")
  retval = ar_set_at (len - 1, 20)
  if (retval isnot 20) {
    notok ("awaiting 20 got: ", retval)
  } else { ok () }

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

  print_str ("test 10 - ")
  retval = ar_sum ()
  if (retval isnot 4520) {
    notok ("awaiting 4520 got: ", retval)
  } else { ok () }
}

test_array (10)

println_str ("--[ END ] --")
