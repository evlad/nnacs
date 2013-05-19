;NeuCon transfer 1.0
; $Id: test.tf,v 1.1 2002-02-14 14:31:50 vlad Exp $
; Time form of a discrete PID controller
[TransferFunction]
product 2	 ; Kp*(1 + (z/z-1)/Ti + (z2-2z+1/z(z-1)*Td)
polyfrac 0	 ; <=== Common proportional coefficient
 2 /  1		 ; Kp
sum 3
polyfrac 0       ; <=== Proportional term
 1 /  1
product 2	 ; <=== Integral term
polyfrac 0
 1 /  2		 ; 1/Ti
polyfrac 0
 1 0 /  1 -1
product 2	 ; <=== Differencial term
polyfrac 0
 2 /  1		 ; Td
polyfrac 0
 1 -2 1 / 1 -1 0
