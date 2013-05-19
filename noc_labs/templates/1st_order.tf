;NeuCon transfer 1.0
[TransferFunction]
; idname:  1st_order
; type:    TransferFunction
; label:   Звено 1-го порядка
; key_pos: K 0 d1 0 d2 1 d3 4
; formula: K*(d1*z+d2)/(z+d3)
product 2
polyfrac 0
1 / 1   ; K
polyfrac 0
 1 0 / 1 -0.5 ; d1 d2 d3
