This 'd' namespace is for development (early draft). Its format is matter
for itself development (development (programming) interface).

It also stores packages intented for further and testing development.

#
  Generate:
      libc:
          add "as" token as my header name when they require me
          status: done

  Make:
      handle properly the priority on dependencies (this will probably
      detect also circular dependencies at the same time)

      record dependencies on shared/static libraries (the library as a key)

      source other specs  m.source (s; from : ns, dir : ......)

  La:
     emphasize that we do not want chained functions to have size effects
     (objects should not be possible to be modified)

     associate objects with a namespace (lookup for functions in this ns)

  Zlibrary:
     handle it similarly to libc

  E:
     normal:
         'D' now deletes from the cursor to the end of line
              do: delete also till the begining of line if there is only
                  whitespace in between
#
