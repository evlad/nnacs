
#ln -s ../xor5_out.dat .
#ln -s ../xor5_ini.nn xor5_ini.nn
dsum ../xor5_in.dat ../xor5_in_err.dat >xor5_in.dat
MAX_EPOCHS=10000 TrainNN xor5_ini.nn xor5_in.dat xor5_out.dat xor5_in.dat xor5_out.dat xor5_res.nn >mse.dat
EvalNN xor5_res.nn xor5_in.dat >xor5_res.dat
ErrInvNN xor5_res.nn xor5_in.dat xor5_out.dat xor5_in_err.dat xor5_out_err.dat >xor5_in_out_dist.dat
StatAn xor5_out.dat xor5_res.dat >xor5_statan.txt
