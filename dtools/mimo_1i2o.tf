;NeuCon transfer 1.1
; Multiple inputs, multiple outputs transfer function sample
[TransferFunction]
inoutdim 1 3     ; 1 inputs, 3 outputs
inout 0 0        ; begin 1->1 function definition
polyfrac 0
 1 / 1		 ; pure input-to-output
inout 0 1        ; begin 1->2 function definition
polyfrac 0
 1 / 1 -0.9
inout 0 2        ; begin 1->3 function definition
polyfrac 0
 1 / 1 -0.9 0.18
