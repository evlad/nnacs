cmake_minimum_required(VERSION 2.8)

project(noc_labs)

if(UNIX)
install(PROGRAMS nnacs.tcl DESTINATION bin RENAME nnacs)
install(PROGRAMS nndisplay.tcl DESTINATION bin RENAME nndisplay)
endif(UNIX)

if(WIN32)
install(PROGRAMS nnacs.tcl DESTINATION bin)
install(PROGRAMS nndisplay.tcl DESTINATION bin)
install(PROGRAMS setnnacs.bat DESTINATION bin)
endif(WIN32)

set(TCL_SCRIPTS data_file.tcl
	draw_nn.tcl
	draw_prim.tcl
	files_loc.tcl
	balloon.tcl
	main.tcl
	nnio.tcl
	par_file.tcl
	progressbar.tcl
	screenshot.tcl
	ssm_file.tcl
	universal.tcl
	getoptbyrs.tcl
	win_checkpnt.tcl
	win_cofunc.tcl
	win_controller.tcl
	win_cusum.tcl
	win_dcontrf.tcl
	win_dcontrp.tcl
	win_dcsloop.tcl
	win_dcsloop_cusum.tcl
	win_distr2d.tcl
	win_dplantid.tcl
	win_dtf.tcl
	win_grseries.tcl
	win_nncedit.tcl
	win_nncontr.tcl
	win_nnpedit.tcl
	win_nnplant.tcl
	win_nntpar.tcl
	win_model.tcl
	win_plant.tcl
	win_rtseries.tcl
	win_series.tcl
	win_signal.tcl
	win_signalgen.tcl
	win_ssmodel.tcl
	win_textedit.tcl
	win_trfunc.tcl)

set(TEMPLATES
	templates/1st_order.tf
	templates/2nd_order.tf
	templates/delay1.tf
	templates/delay2.tf
	templates/delay3.tf
	templates/gain.tf
	templates/integral.tf
	templates/pid_kkk.tf
	templates/pid_ktt.tf
	templates/mimo_2i3o.tf

	templates/1st_order.gif
	templates/2nd_order.gif
	templates/delay1.gif
	templates/delay2.gif
	templates/delay3.gif
	templates/gain.gif
	templates/integral.gif
	templates/pid_kkk.gif
	templates/pid_ktt.gif
	templates/mimo_2i3o.gif

	templates/simple_delayn.gif
	templates/ssmodel.gif

	templates/dcontrf.par
	templates/dcontrp.par
	templates/dcsloop.par
	templates/dcsloop_cusum.par
	templates/dplantid.par
	templates/dtf.par)

install(FILES ${TCL_SCRIPTS} nnacs.ad DESTINATION scripts)
install(FILES ${TEMPLATES} DESTINATION templates)

install(DIRECTORY labworks/Examples DESTINATION labworks
        PATTERN ".svn" EXCLUDE)
