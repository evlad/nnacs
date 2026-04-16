package provide win_dtf 1.0

#set font "-*-helvetica-*-r-*-*-14-*-*-*-*-*-koi8-*"

package require files_loc
package require draw_prim
package require par_file
package require win_textedit
package require win_controller
package require win_plant
package require win_signal


# Load parameters from given file.
proc dtfLoadParams {parFile} {
    global dtf_params
    array set dtf_params {}
    ParFileFetch $parFile dtf_params
}

# Create window with panel and controls.
proc dtfCreateWindow {p title sessionDir} {
    set w $p.dtf

    # Don't create the window twice
    if {[winfo exists $w]} return

    toplevel $w
    wm title $w "Discrete time open loop modeling; (session $sessionDir)"
    wm iconname $w "$sessionDir"

    # 1. Use current session directory
    set curSessionDir $sessionDir
 
    set parFile [file join [SessionDir $curSessionDir] dtf.par]
    if {![file exists $parFile]} {
	# 2. Create default parameters from templates
	file copy -force [file join [TemplateDir] dtf.par] $parFile
    }

    # 3. Assign parameters from file
    global dtf_params
    array set dtf_params {}
    dtfLoadParams $parFile
    if {![info exists dtf_params(comment)]} {
	set dtf_params(comment) ""
    }

    global dtf_grSeries
    set dtf_grSeries {}

    # Headline
    set hl $w.headline
    frame $hl
    set titleFont [option get $hl headlineFont ""]
    button $hl.s -text "Сеанс $sessionDir" \
	-relief flat -padx 0 -pady 0 \
	-justify left -anchor nw -fg DarkGreen -font $titleFont \
	-command "TextEditWindow $w \"$parFile\" \"$parFile\" dtfLoadParams"

    label $hl.t -text $title \
	-justify left -anchor nw -fg DarkGreen -font $titleFont
    pack $hl.s $hl.t -side left
    pack $hl -side top -fill x -pady 2m

    # User comment
    set uc $w.usercomment
    frame $uc
    set titleFont [option get $hl headlineFont ""]
    label $uc.l -text "Описание:" \
	-justify left -anchor nw -fg DarkGreen -font $titleFont
    entry $uc.e -textvariable dtf_params(comment) \
	-width 30 -relief flat -font $titleFont -bg white
    bind $uc.e <Return> "ParFileAssign \"$parFile\" dtf_params"
    bind $uc.e <FocusOut> "ParFileAssign \"$parFile\" dtf_params"
    pack $uc.l -side left
    pack $uc.e -side left -fill x -expand true
    pack $uc -side top -fill x -pady 2m


    # Assign name of check point output files
#    foreach {chkpnt parname} {
#	checkpoint_r out_r
#	checkpoint_n out_n
#	checkpoint_u out_u
#	checkpoint_e out_e
#	checkpoint_y out_ny} {
#	set label [$c.$chkpnt cget -text]
#	$c.$chkpnt configure \
#	    -command "dcsloopCheckPoint $w $chkpnt \"$curSessionDir\" $dcsloop_params($parname) \{$label\}"
#   }

    # 4. Draw control system loop schema
    frame $w.controls
    pack $w.controls -side bottom -fill x -pady 2m

    set c $w.frame.c
    ScreenshotButton $w $w.controls.print $c [SessionDir $curSessionDir] "dcsloop"

    button $w.controls.run -text "Запустить" \
	-command "dtfRun $w \"$curSessionDir\" \"$parFile\""
    button $w.controls.log -text "Протокол"
    button $w.controls.series -text "Графики" \
	-command "GrSeriesWindow $w \"Open loop modeling series plot\" \"[SessionDir $curSessionDir]\""
    button $w.controls.close -text "Закрыть" \
	-command "array set dtf_params {} ; destroy $w"
    pack $w.controls.print $w.controls.run $w.controls.log \
	$w.controls.series $w.controls.close -side left -expand 1

    frame $w.frame
    pack $w.frame -side top -fill both -expand yes

    canvas $c -width 14c -height 7c -relief sunken -borderwidth 2 \
	-background white
    pack $c -side top -fill both -expand yes
}
