set grid
set title 'Test of dstatsb functioning'
set xlabel 'Time'
plot [0:11] [0:7] \
     'dstatsb_test.dat' t 'Series' w l, \
     'dstatsb_test_slbase=0.dat' u 9:1 t 'Mean (sliding base=0)' w l, \
     'dstatsb_test_slbase=2.dat' u 3:1 t 'Mean (sliding base=2)' w l, \
     'dstatsb_test_slbase=3.dat' u 3:1 t 'Mean (sliding base=3)' w l, \
     'dstatsb_test_slbase=4.dat' u 3:1 t 'Mean (sliding base=4)' w l
