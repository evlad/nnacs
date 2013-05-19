set grid
set output "nncf_trace.png"
set terminal png
set logscale
plot "cerr_trace.dat" u 3 w l, "iderr_trace.dat" u 3 w l
set output "nncf_work.png"
set terminal png
unset logscale
plot [9000:9200] "r_out.dat" w l, "ny.dat" w l
