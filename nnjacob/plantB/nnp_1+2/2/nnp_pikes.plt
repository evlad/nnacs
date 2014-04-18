set terminal png
set output "nnp_pikes.png"
#set terminal postscript 'NimbusSanL-Regu' eps enhanced monochrome
#set output "nnp_pikes.eps"
set xlabel "Время, отсчеты (k)"
#set xlabel "Time, samples (k)"
set ylabel "Величина сигнала"
#set label "Signal value"
set grid
plot [0:90] '../../pikes30.dat' t "u(k)" w l, '../../ypikes30.dat' t "y(k)" w l, 'nnypikes30.dat' t "NN y(k)" w l
