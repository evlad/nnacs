;NeuCon transfer 1.0
[TransferFunction]
; idname:  2nd_order
; type:    TransferFunction
; label:   Звено 2-го порядка
; key_pos: K 0 d1 0 d2 1 d3 2 d4 5 d5 6
; formula: K*(d1*z^2+d2*z+d3)/(z^2+d4*z+d5)
product 2
polyfrac 0
1 / 1   ; K
polyfrac 0
 1 0 0 / 1 -2 2 ; d1 d2 d3 d4 d5
