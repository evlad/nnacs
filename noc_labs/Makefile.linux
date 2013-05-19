EXESCRIPTS	=nnacs
TCLSCRIPTS	=nnacs.ad \
		data_file.tcl \
		draw_nn.tcl \
		draw_prim.tcl \
		files_loc.tcl \
		main.tcl \
		nnio.tcl \
		par_file.tcl \
		screenshot.tcl \
		universal.tcl \
		win_checkpnt.tcl \
		win_controller.tcl \
		win_dcontrf.tcl \
		win_dcontrp.tcl \
		win_dcsloop.tcl \
		win_dplantid.tcl \
		win_grseries.tcl \
		win_nncedit.tcl \
		win_nncontr.tcl \
		win_nnpedit.tcl \
		win_nnplant.tcl \
		win_nntpar.tcl \
		win_plant.tcl \
		win_rtseries.tcl \
		win_signal.tcl \
		win_signalgen.tcl \
		win_textedit.tcl \
		win_trfunc.tcl

install:	$(EXESCRIPTS) $(TCLSCRIPTS) install-templates
		install -d $(PREFIX)/bin $(PREFIX)/scripts
		install -m 775 -t $(PREFIX)/bin $(EXESCRIPTS)
		install -m 664 -t $(PREFIX)/scripts $(TCLSCRIPTS)

install-templates:
		make -C templates install
