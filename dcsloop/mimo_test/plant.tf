;NeuCon transfer 1.1
; Multiple inputs, multiple outputs plant transfer function
[TransferFunction]
inoutdim 2 2     ; 2 inputs, 2 outputs
inout 0 0        ; begin 1->1 function definition
polyfrac 0
 1 / 1
inout 1 0        ; begin 2->1 function definition
polyfrac 0
 2 / 1
inout 0 1        ; begin 1->2 function definition
polyfrac 0
 3 / 1
inout 1 1        ; begin 2->2 function definition
polyfrac 0
 4 / 1
