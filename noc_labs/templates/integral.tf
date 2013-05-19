;NeuCon transfer 1.0
[TransferFunction]
; idname:  integral
; type:    TransferFunction
; label:   Суммирующее звено
; key_pos: K 0 d 3
; formula: K/(z-d)
product 2
polyfrac 0
1 / 1   ; K
polyfrac 0
 -1 / -1 0.1 ; d
