;NeuCon transfer 1.1
; Multiple inputs, multiple outputs transfer function sample
[TransferFunction]
; Пример объекта с 2 входами и 3 выходами
; idname:  mimo_2i3o
; type:    TransferFunction
; label:   Multiple-input multiple-output block
; key_pos:
; formula:
inoutdim 2 3     ; 2 inputs, 3 outputs
inout 0 0        ; begin 1->1 function definition
polyfrac 0
 2 / 1             ; simple gain
inout 0 1        ; begin 1->2 function definition
polyfrac 0
 1.5 0 / 1 -0.5    ; 1st order
inout 0 2        ; begin 1->3 function definition
polyfrac 0
 0.7 0 0 / 1 -2 2  ; 2nd order
inout 1 0        ; begin 2->1 function definition
product 2          ; product of two transfer functions
polyfrac 0
 1 0 / 1 -0.5      ; 1st order
polyfrac 0
 2 0.1 / 1 -0.7    ; 1st order
inout 1 1        ; begin 2->2 function definition
polyfrac 0
1 / 1 0            ; delay for 1 time sample
inout 1 2        ; begin 2->3 function definition
polyfrac 0
1 / 1 0 0 0        ; delay for 3 time samples
