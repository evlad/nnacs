;NeuCon transfer 1.0
[TransferFunction]
; idname:  pid_ktt
; type:    TransferFunction
; label:   ПИД регулятор (Kp Ti Td)
; key_pos: Kp 0 Ti 2 Td 2
; formula: Kp*(1 + (1/Ti)*(z/z-1) + (1/Td)*(z2-2z+1/z(z-1)))
product 2 
polyfrac 0
 1.0 / 1   ; Kp
sum 3
polyfrac 0 ; <=== Proportional term
 1 / 1
product 2  ; <=== Integral term
polyfrac 0
 1.0 / 1.0  ; Ti
polyfrac 0
 1 0 / 1 -1
product 2  ; <=== Differential term
polyfrac 0
 1.0 / 1.0  ; Td
polyfrac 0
 1 -2 1 / 1 -1 0
