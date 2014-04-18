set terminal png
set output "nnp_rand.png"
#set terminal postscript 'NimbusSanL-Regu' eps enhanced monochrome
#set output "nnp_rand.eps"
set xlabel "Время, отсчеты (k)"
#set xlabel "Time, samples (k)"
set ylabel "Величина сигнала"
#set label "Signal value"
set grid
plot [40:100] [-2:3] '../../0ev/u.dat' t "u(k)" w l, '../../0ev/ny.dat' t "y(k)" w l, 'nn_y.dat' t "NN y(k)" w l
