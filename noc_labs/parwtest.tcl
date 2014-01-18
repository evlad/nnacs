#!/bin/sh
# \
exec wish "$0" ${1+"$@"}

pkg_mkIndex .
lappend auto_path .
package require universal

global array_params
array set array_params {par_a 12.3 par_c "Test value" par_b 32000 }
set TestParameters {                                      
    -0 "Window title"
    -1 "Первый подзаголовок"
    par_a "Первый параметр"
    par_b "Второй параметр"
    -2 "Второй подзаголовок"
    par_c "Третий параметр"
}

button .b -text "Press me" -command { ParametersWindow .b array_params $TestParameters ; puts [array get array_params] }
pack .b
