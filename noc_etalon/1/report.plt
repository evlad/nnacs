set grid
set output "nnp_trace.png"
set terminal png
set logscale
plot "nnp_trace.dat" u 3 w l, "nnp_trace.dat" u 6 w l
set output "nnp_res_ev.png"
set terminal png
unset logscale
plot "tr_y_ev.dat" w l, "nn_y_ev.dat" w l
