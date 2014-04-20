;NeuCon transfer 1.1
; Multiple inputs, multiple outputs transfer function sample
[TransferFunction]
inoutdim 2 3     ; 2 inputs, 3 outputs
inout 0 0        ; begin 1->1 function definition
polyfrac 0
 2 1 / 1 -1
inout 0 1        ; begin 1->2 function definition
product 2
polyfrac 0
1 / 1
polyfrac 0
 1 0.5 0 / 1 -2 2
inout 1 1        ; begin 2->2 function definition
polyfrac 0
0.3 / 1 0 0
inout 1 2        ; begin 2->3 function definition
polyfrac 0
1 / 1 0
; rest functions 1->3, 2->1 are zero
