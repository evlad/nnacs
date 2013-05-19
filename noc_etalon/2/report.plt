set grid
set output "nncp_trace.png"
set terminal png
set logscale
plot "nncp_trace.dat" u 3 w l, "nncp_trace.dat" u 6 w l
set output "nncp_eval.png"
set terminal png
unset logscale
plot "../0ev/u.dat" w l, "nn_u_ev.dat" w l
