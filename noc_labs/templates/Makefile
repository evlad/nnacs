PAR_TEMPLATES	=dcsloop.par \
		dcontrp.par \
		dcontrf.par \
		dplantid.par
TF_TEMPLATES	=1st_order.tf \
		2nd_order.tf \
		delay1.tf \
		delay2.tf \
		delay3.tf \
		gain.tf \
		integral.tf \
		pid_kkk.tf \
		pid_ktt.tf
PICTURES	=$(TF_TEMPLATES:.tf=.gif) \
		simple_delayn.gif
TEMPLATES	=$(PAR_TEMPLATES) $(TF_TEMPLATES) $(PICTURES)

install:	$(TEMPLATES)
		install -d $(PREFIX)/templates
		install -m 664 $(TEMPLATES) $(PREFIX)/templates
