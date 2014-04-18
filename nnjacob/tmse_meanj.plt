set terminal png
set output "tmse_meanj.png"
#set terminal postscript 'NimbusSanL-Regu' eps enhanced monochrome
#set output "tmse_meanj.eps"
#set title "Оценка якобиана с помощью обратного\nраспространения ошибки"
set xlabel "Среднеквадратическая ошибка на тестовой выборке"
set ylabel "Значение якобиана"
#set xlabel "Test MSE"
#set ylabel "Jacobian estimation through BPE"
set grid
#plot [0.008:0.022] [0:9] 'plantA/tmse_meanj.dat' u 1:2 t "A" w point, 'plantB/tmse_meanj.dat' u 1:2 t "B" w point, 'plantC/tmse_meanj.dat' u 1:2 t "C" w point
#plot 'plantA/tmse_meanj.dat' u 1:2:3 t "A" w labels point offset character 0,character 1 tc rgb "red", 'plantB/tmse_meanj.dat' u 1:2:3 t "B" w labels point offset character 0,character 1 tc rgb "green", 'plantC/tmse_meanj.dat' u 1:2:3 t "C" w labels point offset character 0,character 1 tc rgb "blue"
plot [0.008:0.05] [0:9] 'plantA/tmse_meanj.dat' u 1:2 t "A" w point, 'plantB/tmse_meanj.dat' u 1:2 t "B" w point, 'plantC/tmse_meanj.dat' u 1:2 t "C" w point, 371.9447842*x-3.961274522 t "" w l lc rgb "red", 8855.316412*x-87.67030049 t "" w l lc rgb "green", 3813.632032*x-40.62666464  t "" w l lc rgb "blue"
#A: 371.9447842*x-3.961274522
#B: 8855.316412*x-87.67030049
#C: 3813.632032*x-40.62666464
