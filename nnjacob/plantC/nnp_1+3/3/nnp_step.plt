set terminal png
set output "nnp_step.png"
#set terminal postscript 'NimbusSanL-Regu' eps enhanced monochrome
#set output "nnp_step.eps"
set xlabel "Время, отсчеты (k)"
#set xlabel "Time, samples (k)"
set ylabel "Величина сигнала"
#set label "Signal value"
set grid
plot [0:100] [-1.5:1.5] '../../../step200.dat' t "u(k)" w l, '../../ystep200.dat' t "y(k)" w l, 'nnystep200.dat' t "NN y(k)" w l
