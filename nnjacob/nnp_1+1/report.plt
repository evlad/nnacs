set grid
set output "jacob_nnp_1+1.png"
set terminal png
plot '../pikes100.dat' w l, 'j_ideal.dat' u 3 w l, 'j_nnp_1+1_1.dat' u 3 w l, 'j_nnp_1+1_2.dat' u 3 w l, 'j_nnp_1+1_3.dat' u 3 w l
