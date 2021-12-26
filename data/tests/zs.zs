#!/bin/env zs

$VAR=/bin/foo
$VAR=$(which ls) # allow to override
$V=/us
$VA=r
${VAR} ${V}${VA}
$V1=local
$V=${V}${VA}/${V1}
IO.println ${V}
IO.println $(which ls)
IO.println ${V}/$(which ls)

$VS="a1 @ ! - + . '"
IO.println ${VS}
IO.println ok
$VV="${V} ${VA} /${V1}" # allow expansion into strings
IO.println ${VV}
$VVV=${VV}
IO.println ${VVV}
$var=$(ls / | grep proc)
IO.println dada
IO.println -- -${var}
