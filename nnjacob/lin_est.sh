#!/bin/bash

echo "Direct modeling"
dtf plant.tf pikes100.dat y_pikes.dat

echo "Jacobian estimation"
DJACOB_DELAY=0 djacob pikes100.dat y_pikes.dat >j_fact_0.dat
echo 0 0 0 >j_fact_0x.dat
sed 's/nan/0/g;s/inf/0/g' j_fact_0.dat >>j_fact_0x.dat

# gnuplot:
# plot 'pikes100.dat' w l, 'y_pikes.dat' w l, 'j_fact_0x.dat' u 3 w l

echo "Neural net plant output estimation"
echo 0 >pikes_advanced1.dat
cat pikes100.dat >>pikes_advanced1.dat
echo 0 >y_pikes_advanced1.dat
cat y_pikes.dat >>y_pikes_advanced1.dat
echo 0 >y_pikes_advanced2.dat
echo 0 >>y_pikes_advanced2.dat
cat y_pikes.dat >>y_pikes_advanced2.dat
paste pikes100.dat pikes_advanced1.dat y_pikes_advanced1.dat y_pikes_advanced2.dat >nnp_in.dat
EvalNN nnp_res.nn nnp_in.dat nnp_out.dat

# gnuplot:
## u(k), y(k+1), y~(k)
# plot 'pikes100.dat' w l, 'y_pikes.dat' w l, 'nnp_out.dat' w l
# with proper delays for Plant output and NN-P output
## u(k), y(k), y~(k)
# plot 'nnp_in.dat' u 1 w l, 'nnp_in.dat' u 3 w l, 'nnp_out.dat' w l

tail -n +2 nnp_out.dat >nn_y_pikes.dat
DJACOB_DELAY=0 djacob pikes100.dat nn_y_pikes.dat >j_nnp_0.dat
#echo 0 0 0 >j_nnp_0x.dat
sed 's/nan/0/g;s/inf/0/g' j_nnp_0.dat >j_nnp_0x.dat


#echo "Neural net plant output estimation"
#tail -n +2 pikes100.dat >pikes100_delayed1.dat
#echo 0 >y_pikes_advanced1.dat
#cat y_pikes.dat >>y_pikes_advanced1.dat
#paste pikes100_delayed1.dat pikes100.dat y_pikes.dat y_pikes_advanced1.dat >nnp_in.dat
## u(k), u(k-1), y(k), y(k-1) -> y(k+1)
#EvalNN nnp_res.nn nnp_in.dat nnp_out.dat
