;NeuCon transfer 1.0
[TransferFunction]
; idname:  pid_kkk
; type:    TransferFunction
; label:   ПИД регулятор (Kp Ki Kd)
; key_pos: Kp 0 Ki 0 Kd 0
; formula: Kp + Ki*(z/z-1) + Kd*(z2-2z+1/z(z-1))
sum 3
polyfrac 0 ; <=== Proportional term
 0.5 / 1   ; Kp
product 2  ; <=== Integral term
polyfrac 0
 0.8 / 1   ; Ki
polyfrac 0
 1 0 / 1 -1
product 2  ; <=== Differential term
polyfrac 0
 0.1 / 1   ; Kd
polyfrac 0
 1 -2 1 / 1 -1 0
