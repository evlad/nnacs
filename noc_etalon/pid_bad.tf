;NeuCon transfer 1.0
; $Id: pid_bad.tf,v 1.1 2008-06-08 21:04:15 evlad Exp $
; Speed form of a discrete PID controller
[TransferFunction]
sum 3		 ; Kp + Ki*(z/z-1) + Kd*(z2-2z+1/z(z-1))
polyfrac 0	 ; <=== Proportional term
 0.5 /  1		 ; Kp
product 2	 ; <=== Integral term
polyfrac 0
 0.8 /  1		 ; Ki
polyfrac 0
 1 0 /  1 -1
product 2	 ; <=== Differencial term
polyfrac 0
 0.1 /  1		 ; Kd
polyfrac 0
 1 -2 1 / 1 -1 0
