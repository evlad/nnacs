;NeuCon transfer 1.0
[TransferFunction]
; idname:  3rd_order
; type:    TransferFunction
; label:   Звено 3-го порядка
; key_pos: K 0 d1 0 d2 1 d3 2 d4 3 d5 6 d6 7 d7 8
; formula: K*(d1*z^3+d2*z^2+d3*z+d4)/(z^3+d5*z^2+d6*z+d7)
product 2
polyfrac 0
1 / 1   ; K
polyfrac 0
 1 0 0 0 / 1 -2 2 -1 ; d1 d2 d3 d4 d5 d6 d7
