package provide win_nntpar 1.0

package require Tk
package require universal

# Good for dcontrp and dplantid
set OfflineNNTeacherPar {
    -0 "Offline NN training parameters"
    -1 "Learning parameters"
    eta "Learning rate coefficient for hidden neurons"
    eta_output "Learning rate coefficient for output neurons"
    alpha "Inertia coefficient (momentum)"
    -2 "Learning stop parameters"
    finish_on_value "Lower limit of the error on the test subset"
    finish_on_decrease "Lower threshold for changing the error on test subset"
    finish_on_grow "The limiting number of epochs with increasing test error"
    finish_max_epoch "The limiting number of learning epochs"
    -3 "Visualization parameters"
    rtseries_yMax "Upper bound of error axis"
    rtseries_yMin "Lower bound of error axis"
}

# Good for dcontrf
set OnlineNNTeacherPar {
    -0 "Online NN training parameters"
    -1 "Learning parameters"
    nnc_auf "Update period for NN-C in time samples"
    eta "Learning rate coefficient for hidden neurons"
    eta_output "Learning rate coefficient for output neurons"
    alpha "Inertia coefficient (momentum)"
    -2 "Learning stop parameters"
    skip_growing "Skip in the beginning the number of epochs with increasing error"
    finish_decrease "Lower limit of change in control error"
    finish_on_grow "The limiting number of epochs with increasing control error"
    max_epochs "The limiting number of learning epochs"
    -3 "Visualization parameters"
    rtseries_yMax "Upper bound of error axis"
    rtseries_yMin "Lower bound of error axis"
}
