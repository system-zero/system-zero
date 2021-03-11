# ignore 

var m1 = "Testing Instrunctions\n\n"
var m2 = "free strings and allocate memory again\n\n"

var RELEASE       = 1
var DONOT_RELEASE = 0
var MAX_MSG_SIZE  = 256

func f1 {
  func msg_release (m) {
    free (m)
  }

  func msg_and_release (ma, mb, rel) {
    print ("msg_and_release [TODO: __FUNC__]\n")
    print ("${ma} ${mb}\n")

    if (rel) {
      print ("release condition met ${rel}\n")
      msg_release (ma)
      msg_release (mb)
    }
  }

  msg_and_release (m1, m2, RELEASE)

  m1 = "Re initialize them "
  m1 = "and print again\n"

  msg_and_release (m1, m2, DONOT_RELEASE)
}

f1()
