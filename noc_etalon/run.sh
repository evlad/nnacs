#!/bin/sh

# See README for some details

# OPTIONAL: to generate new data sets (training and evaluation):
#( cd 0tr ; dcsloop dcsloop.par )
#( cd 0ev ; dcsloop dcsloop.par )

# NN plant prediction model training
( cd 1 ; dplantid dplantid.par )

# NN controller out-of-loop training
( cd 2 ; dcontrp dcontrp.par )

# NN controller in-loop training (optimisation)
( cd 3 ; dcontrf dcontrf.par )

# NN controller in-loop test and compare with Wiener controller
( cd 4 ; dcsloop dcsloop.par ; dcsloop dcsloop_woc.par )

# End of file
