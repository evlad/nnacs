;NeuCon transfer 1.0
[TransferFunction]
; idname:  1st_order
; type:    TransferFunction
; key_pos: K 0 d1 1 d2 4
; formula: K*(z-d1)/(z-d2)
product 2
polyfrac 0
1 / 1   ; K
polyfrac 0
 -1 0 / -1 0.5 ; d1 d2
