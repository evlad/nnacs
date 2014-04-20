;NeuCon transfer 1.1
; Multiple inputs, multiple outputs noise filter transfer function
[TransferFunction]
inoutdim 1 2     ; 2 inputs, 2 outputs
inout 0 0        ; begin 1->1 function definition
polyfrac 0
 0.1 / 1
inout 0 1        ; begin 1->2 function definition
polyfrac 0
 0.2 / 1
