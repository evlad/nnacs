#!/bin/sh
# \
exec wish "$0" ${1+"$@"}

pkg_mkIndex .
lappend auto_path .
package require universal

global array_params
array set array_params {par_a 12.3 par_c "Test value" par_b 32000 par_d file }
set TestParameters {                                      
    -0 "Window title"
    -1 "Первый подзаголовок"
    par_a "Первый параметр"
    par_b "Второй параметр"
    -2 "Второй подзаголовок"
    par_c "Третий параметр"
    par_d "Четвертый параметр"
    par_e "Пятый параметр неизвестный тип"
}
set TestParSpecifications {
  par_d {oneof "stream" "file"}
  par_e {dummytype}
}

button .b -text [mc "Press me"] -command { ParametersWindow .b array_params $TestParameters $TestParSpecifications ; puts [array get array_params] }
pack .b
