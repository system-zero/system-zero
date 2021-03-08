
func ok () {
  println_str ("[OK]")
}

func notok (msg, val) {
  print_str ("[NOTOK] ")
  print_str (msg);
  println (val)
}

println_str ("--[ BEG ]--")

func g () {

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
}

g ()

println_str ("--[ END ] --")
