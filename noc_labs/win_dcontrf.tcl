#pkg_mkIndex .
#lappend auto_path .

package provide win_dcontrf 1.0

#set font "-*-helvetica-*-r-*-*-14-*-*-*-*-*-koi8-*"

package require files_loc
package require draw_prim
package require par_file
package require win_textedit
package require win_controller
package require win_plant
package require win_signal
package require win_nncontr
package require win_nnplant
package require win_rtseries
package require screenshot

# Draw panel contents in given canvas
proc dcontrfDrawPanel {this c} {
    #set textFont -*-helvetica-bold-r-*-*-14-*-*-*-*-*-koi8-r
    set textFont [option get $c fontLargeBlock ""]

    $c create text 0.5c 0.6c -text "Обучение нейронной сети регулятора (НС-Р) в контуре:" \
	-justify left -anchor nw -fill black -font "$textFont"

    DrawLargeBlock $c reference "Уставка" 1.8c 4.5c
    DrawSmallBlock $c checkpoint_r "r" 3.5c 4.5c
    DrawGather $c cerr 4.5c 4.5c "s"
    DrawSmallBlock $c checkpoint_e "e" 5.4c 4.5c
    DrawLargeBlock $c teacher "Обучение" 7.5c 2.2c
    DrawLargeBlock $c controller "  НС-Р  " 7.1c 4.5c
    DrawSmallBlock $c checkpoint_u "u" 8.8c 4.5c
    DrawLargeBlock $c plant "Объект" 10.4c 4.5c
    DrawLargeBlock $c nnplant "  НС-О  " 10.4c 3c
    DrawGather $c nadd 12c 4.5c "none"
    DrawLargeBlock $c noise "Помеха" 9.4c 5.5c
    DrawSmallBlock $c checkpoint_y "y" 13c 4.5c
    DrawSmallBlock $c checkpoint_n "n" 11c 5.5c
    DrawSmallBlock $c checkpoint_nny "y'" 12c 3c

    DrawDirection $c reference "e" checkpoint_r "w" last
    DrawDirection $c checkpoint_r "e" cerr "w" last
    DrawDirection $c cerr "e" checkpoint_e "w" last
    DrawDirection $c checkpoint_e "e" controller "w" last
    DrawDirection $c controller "e" checkpoint_u "w" last
    DrawDirection $c checkpoint_u "e" plant "w" last
    DrawDirection $c checkpoint_u "n" nnplant "w" ver
    DrawDirection $c plant "e" nadd "w" last
    DrawDirection $c checkpoint_e "n" teacher "e" ver
    DrawDirection $c teacher "e" nnplant "nw" last {color "red"}
    DrawDirection $c nnplant "sw" controller "ne" last {color "red"}
    DrawDirection $c nnplant "e" checkpoint_nny "w" last

    DrawDirection $c noise "e" checkpoint_n "w" last
    DrawDirection $c nadd "e" checkpoint_y "w" last
    DrawDirection $c checkpoint_n "e" nadd "s" hor
    DrawDirection $c checkpoint_y "n" nnplant "s" horMiddle

    DrawDirection $c checkpoint_y "s" cerr "s" horMiddle {midCoord 6.2c}
}


# Read given descriptor to get control and identification MSE.  At the
# EOF return {}.
# Example:
# 1: Control MSE= 0.1165 delta=+0.1164858  Ident.MSE= 1.8818 delta=+1.8817559
# 2: Control MSE= 0.1238 delta=+0.0072690  Ident.MSE= 1.9260 delta=+0.0442196
# 3: Control MSE= 0.1149 delta=-0.0088581  Ident.MSE= 1.8474 delta=-0.0786195
# IMPORTANT: net is dead due to data are exhausted.
proc dcontrfReader {fd} {
    set i 0
    global cMSE$fd idMSE$fd
    while {[gets $fd line] >= 0} {
	#puts "reader: $line"
	switch -regexp -- $line {
	    {^[^:]*: Control MSE=.* Ident\.MSE=} {
		regexp {Control MSE= *([^ ]*) *delta= *([^ ]*) *Ident\.MSE= *([^ ]*) *delta= *([^ ]*)} $line allMatch cMSE cDelta idMSE idDelta
		# Check for [+-]nan and inf
		set specnums {([Ii][Nn][Ff]|[+-]?[Nn][Aa][Nn])}
		foreach var {cMSE cDelta idMSE idDelta} {
		    if {[regexp $specnums [set $var]]} {
			return {}
		    }
		}
		if {[info exists cMSE$fd]} {
		    set cMSE$fd [expr [set cMSE$fd] + $cDelta]
		} else {
		    set cMSE$fd $cDelta
		}
		if {[info exists idMSE$fd]} {
		    set idMSE$fd [expr [set idMSE$fd] + $idDelta]
		} else {
		    set idMSE$fd $idDelta
		}
		# Not very precise
		#return "[set cMSE] [set idMSE]"
		# More precise
		return "[set cMSE$fd] [set idMSE$fd]"
	    }
	    IMPORTANT: {
		return {}
	    }
	}
	incr i
    }
    return {}
}

# 8. Run the program in its session directory
proc dcontrfRun {p sessionDir parFile} {
    set cwd [pwd]
    puts "Run dcontrf in [SessionDir $sessionDir]"
    catch {cd [SessionDir $sessionDir]} errCode1
    if {$errCode1 != ""} {
	error $errCode1
	return
    } else {
	set exepath [file join [SystemDir] bin dcontrf]
	#if {![file executable $exepath]} {
	#    error "$exepath is not executable"
	#}

	global dcontrf_params
	set params {
	    winWidth 600
	    winHeight 250
	    yMin 1e-3
	    yMax 1e2
	    yScale log
	    timeLabel "Epoch:"
	}
	set series { ControlMSE IdentifMSE }
	set pipe [open "|\"$exepath\" \"$parFile\"" r]
	fconfigure $pipe -buffering line

	set pipepar [fconfigure $pipe]
	#puts $pipepar

	switch $dcontrf_params(input_kind) {
	    stream {
		set stream_len $dcontrf_params(stream_len)
		set nnc_auf $dcontrf_params(nnc_auf)
		set timeLen [expr 1 + int($stream_len / (0.0 + $nnc_auf))]
	    }
	    file {
		set timeLen $dcontrf_params(max_epochs)
	    }
	    default {
		# Just to avoid error with scale step calculation
		set timeLen 1
	    }
	}
	lappend params \
	    timeLen $timeLen \
	    stopCmd "catch {close $pipe}" \
	    workDir [SessionDir $sessionDir]

	global etaHidden$pipe etaOutput$pipe
	set etaHidden$pipe $dcontrf_params(eta)
	set etaOutput$pipe $dcontrf_params(eta_output)
	RtSeriesWindow $p "NN training" "dcontrfReader $pipe" $params $series
	catch {close $pipe}

	cd $cwd
    }

    # 9. Refresh state of controls
    # TODO

    # 10. It's possible to display log
    set logFile [file rootname $parFile].log
    $p.controls.log configure \
	-command "TextEditWindow $p \"$logFile\" \"$logFile\""

    # blink once to emphasize Log button
    set storedBg [$p.controls.log cget -background]
    $p.controls.log configure -background "red"
    after 1500 "$p.controls.log configure -background \"$storedBg\""

    cd $cwd
}

# Add given checkpoint to plotter window or display the data file.
proc dcontrfCheckPoint {p chkpnt sessionDir fileName label} {
    set filePath [SessionAbsPath $sessionDir $fileName]

    if {![file exists $filePath]} return

    if {[GrSeriesCheckPresence $p]} {
	set wholeData [GrSeriesReadFile $filePath]
	# Avoid adding one series several times
	if {0 <= [GrSeriesAddSeries $p "[lindex $wholeData 0]" \
		      $label $filePath]} {
	    GrSeriesRedraw $p
	} else {
	    # If series already plotted then let's show statistics,
	    StatAnDataFile $p $sessionDir $fileName
	}
    } else {
	# If plot does not exist let's show statistics
	StatAnDataFile $p $sessionDir $fileName
    }
}

# Load parameters from given file.
proc dcontrfLoadParams {parFile} {
    global dcontrf_params
    array set dcontrf_params {}
    ParFileFetch $parFile dcontrf_params
}

# Create window with panel and controls.
proc dcontrfCreateWindow {p title sessionDir} {
    set w $p.dcontrf

    # Don't create the window twice
    if {[winfo exists $w]} return

    toplevel $w
    wm title $w "Neural network controller training in loop; (session $sessionDir)"
    wm iconname $w "$sessionDir"

    # 1. Use current session directory
    set curSessionDir $sessionDir
 
    set parFile [file join [SessionDir $curSessionDir] dcontrf.par]
    if {![file exists $parFile]} {
	# 2. Create default parameters from templates
	file copy -force [file join [TemplateDir] dcontrf.par] $parFile
    }

    # 3. Assign parameters from file
    global dcontrf_params
    array set dcontrf_params {}
    dcontrfLoadParams $parFile
    if {![info exists dcontrf_params(comment)]} {
	set dcontrf_params(comment) ""
    }

    global dcontrf_grSeries
    set dcontrf_grSeries {}

    # Headline
    set hl $w.headline
    frame $hl
    set titleFont [option get $hl headlineFont ""]
    button $hl.s -text "Сеанс $sessionDir" \
	-relief flat -padx 0 -pady 0 \
	-justify left -anchor nw -fg DarkGreen -font $titleFont \
	-command "TextEditWindow $w \"$parFile\" \"$parFile\" dcontrfLoadParams"
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
    entry $uc.e -textvariable dcontrf_params(comment) \
	-width 30 -relief flat -font $titleFont -bg white
    bind $uc.e <Return> "ParFileAssign \"$parFile\" dcontrf_params"
    bind $uc.e <FocusOut> "ParFileAssign \"$parFile\" dcontrf_params"
    pack $uc.l -side left
    pack $uc.e -side left -fill x -expand true
    pack $uc -side top -fill x -pady 2m

    # 4. Draw control system loop schema
    frame $w.controls
    pack $w.controls -side bottom -fill x -pady 2m
    set c $w.frame.c
    ScreenshotButton $w $w.controls.print $c [SessionDir $curSessionDir] "dcontrf"

    button $w.controls.run -text "Запустить" \
	-command "dcontrfRun $w \"$curSessionDir\" \"$parFile\""
    button $w.controls.log -text "Протокол"
    button $w.controls.series -text "Графики" \
	-command "GrSeriesWindow $w \"NN-C in-loop training series plot\" \"[SessionDir $curSessionDir]\""
    button $w.controls.close -text "Закрыть" \
	-command "array set dcontrf_params {} ; destroy $w"
    pack $w.controls.print $w.controls.run $w.controls.log \
	$w.controls.series $w.controls.close -side left -expand 1

    frame $w.frame
    pack $w.frame -side top -fill both -expand yes

    canvas $c -width 16c -height 7c -relief sunken -borderwidth 2 \
	-background white
    pack $c -side top -fill both -expand yes

    dcontrfDrawPanel {} $c

    # 5. Connect callbacks with visual parameters settings
    # (reference+noise, modelling length) including selection of tf
    $c.reference configure \
	-command "SignalWindow $w \"$curSessionDir\" refer dcontrf_params input_kind in_r refer_tf stream_len ; ParFileAssign \"$parFile\" dcontrf_params"
    $c.controller configure \
	-command "ContrWindow $w \"$curSessionDir\" dcontrf_params contr_kind lincontr_tf nncontr nnc_mode ; ParFileAssign \"$parFile\" dcontrf_params"
    $c.plant configure \
	-command "PlantWindow $w \"$curSessionDir\" dcontrf_params linplant_tf ; ParFileAssign \"$parFile\" dcontrf_params"
    $c.noise configure \
	-command "SignalWindow $w \"$curSessionDir\" noise dcontrf_params input_kind in_n noise_tf stream_len ; ParFileAssign \"$parFile\" dcontrf_params"
    $c.teacher configure \
	-command "NNTeacherParWindow $w dcontrf_params \$OnlineNNTeacherPar; ParFileAssign \"$parFile\" dcontrf_params"
    $c.controller configure \
	-command "NNContrWindow $w \"$curSessionDir\" dcontrf_params in_nnc_file out_nnc_file nnc_mode; ParFileAssign \"$parFile\" dcontrf_params"
    $c.nnplant configure \
	-command "NNPlantWindow $w \"$curSessionDir\" dcontrf_params in_nnp_file {}; ParFileAssign \"$parFile\" dcontrf_params"

    # Assign name of check point output files
    foreach {chkpnt parname} {
	checkpoint_r out_r
	checkpoint_n out_n
	checkpoint_u out_u
	checkpoint_e out_e
	checkpoint_y out_ny
	checkpoint_nny out_nn_y} {
	set label [$c.$chkpnt cget -text]
	$c.$chkpnt configure \
	    -command "dcontrfCheckPoint $w $chkpnt \"$curSessionDir\" $dcontrf_params($parname) \{$label\}"
    }

    # 
    # Prepare template parameters file
    # Provide its name for editing by $w.controls.params
    # At $w.controls.run replaces several parameters by preset values
    # During the run display progress bar
    # After the run gather files and be ready to draw series
    # Drawing series may be tuned on and off in custom manner
}

#proc dcontrfSingle {} {
#    NewUser "" user1
#    dcontrfCreateWindow ""
#}

#dcontrfSingle

proc dcontrfTest {} {
    set c .c
    canvas $c -width 16c -height 7c -relief sunken -borderwidth 2 \
	-background white
    pack $c -side top -fill both -expand yes

    dcontrfDrawPanel {} $c
}

#dcontrfTest

# End of file
