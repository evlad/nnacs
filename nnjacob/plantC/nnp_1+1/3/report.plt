set grid
set output "nnp_trace.png"
set terminal png
set logscale
set title "Learning curves"
plot "nnp_trace.dat" u 3 w l, "nnp_trace.dat" u 6 w l
set output "nnp_res_ev.png"
set terminal png
set title "Test set performance"
unset logscale
plot "tr_y_ev.dat" w l, "nn_y_ev.dat" w l
set output "nnp_pikes.png"
set terminal png
set title "Pikes set performance"
unset logscale
plot "../../pikes30.dat" w l, "../../ypikes30.dat" w l, "nnp_out.dat" w l

