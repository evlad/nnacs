set grid
set output "jacob_nnp_1+2.png"
set terminal png
plot '../pikes100.dat' w l, 'j_ideal.dat' u 3 w l, 'j_nnp_1+2_1.dat' u 3 w l, 'j_nnp_1+2_2.dat' u 3 w l, 'j_nnp_1+2_3.dat' u 3 w l
