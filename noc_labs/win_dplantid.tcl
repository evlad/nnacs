#pkg_mkIndex .
#lappend auto_path .

package provide win_dplantid 1.0

package require files_loc
package require draw_prim
package require par_file
package require win_textedit
package require win_nnplant
package require win_nntpar
package require win_rtseries
package require screenshot

# Draw panel contents in given canvas
proc dplantidDrawPanel {this c} {
    #set textFont -*-helvetica-bold-r-*-*-14-*-*-*-*-*-koi8-r
    set textFont [option get $c fontLargeBlock ""]
    set delaynimg [image create photo -file \
		       [file join [SystemDir] templates simple_delayn.gif] ]

    $c create text 0.5c 0.8c -text "Обучение нейронной сети (НС-О):" \
	-justify left -anchor nw -fill black -font "$textFont"

    DrawLargeBlock $c learn_control "Управление" 3c 2c
    DrawSmallBlock $c learn_checkpoint_u "u" 5.1c 2c
    DrawLargeBlock $c learn_observ "Наблюдение" 3c 3c
    DrawSmallBlock $c learn_checkpoint_y "y" 5.1c 3c
    DrawLargeBlock $c learn_nnp "\nНС-О\n" 6.5c 2.5c
    DrawSmallBlock $c learn_checkpoint_nny "y'" 8c 2.5c
    DrawGather $c learn_cmp 8c 4.1c "n"
    DrawLargeBlock $c learn_training "Обучение" 9c 1c
    DrawImageBlock $c learn_delayn $delaynimg 5.1c 4.1c
    DrawSmallBlock $c learn_checkpoint_yd "y\"" 6.5c 4.1c

    DrawDirection $c learn_control "e" learn_checkpoint_u "w" last
    DrawDirection $c learn_checkpoint_u "e" learn_nnp "w" horOnly
    DrawDirection $c learn_observ "e" learn_checkpoint_y "w" last
    DrawDirection $c learn_checkpoint_y "e" learn_nnp "w" horOnly
    DrawDirection $c learn_nnp "e" learn_checkpoint_nny "w" last
    DrawDirection $c learn_checkpoint_nny "s" learn_cmp "n" last
    DrawDirection $c learn_checkpoint_y "s" learn_delayn "n" last
    DrawDirection $c learn_delayn "e" learn_checkpoint_yd "w" last
    DrawDirection $c learn_checkpoint_yd "e" learn_cmp "w" last
    DrawDirection $c learn_cmp "e" learn_training "s" hor
    DrawDirection $c learn_training "sw" learn_nnp "ne" last {color "red"}

    $c create text 0.5c 4.8c -text "Проверка нейронной сети (НС-О):" \
     	-justify left -anchor nw -fill black -font "$textFont"

    DrawLargeBlock $c test_control "Управление" 3c 6c
    DrawSmallBlock $c test_checkpoint_u "u" 5.1c 6c
    DrawLargeBlock $c test_observ "Наблюдение" 3c 7c
    DrawSmallBlock $c test_checkpoint_y "y" 5.1c 7c
    DrawLargeBlock $c test_nnp "\nНС-О\n" 6.5c 6.5c
    DrawSmallBlock $c test_checkpoint_nny "y'" 8c 6.5c
    DrawGather $c test_cmp 8c 8.1c "n"
    DrawLargeBlock $c test_check "Проверка" 10c 8.1c
    DrawImageBlock $c test_delayn $delaynimg 5.1c 8.1c
    DrawSmallBlock $c test_checkpoint_yd "y\"" 6.5c 8.1c

    DrawDirection $c test_control "e" test_checkpoint_u "w" last
    DrawDirection $c test_checkpoint_u "e" test_nnp "w" horOnly
    DrawDirection $c test_observ "e" test_checkpoint_y "w" last
    DrawDirection $c test_checkpoint_y "e" test_nnp "w" horOnly
    DrawDirection $c test_nnp "e" test_checkpoint_nny "w" last
    DrawDirection $c test_checkpoint_nny "s" test_cmp "n" last
    DrawDirection $c test_checkpoint_y "s" test_delayn "n" last
    DrawDirection $c test_delayn "e" test_checkpoint_yd "w" last
    DrawDirection $c test_checkpoint_yd "e" test_cmp "w" last
    DrawDirection $c test_cmp "e" test_check "w" hor
}


# Read given descriptor until Test: and return both learn and test
# MSE.  At the EOF return {}.
# Example:
# Iteration 3   , MSE=4.45605 (4.51003) -> repeat with (0.01, 0.001, 0)
# Iteration 1   , MSE=1.03622 (1.04878) -> teach NN
#           Test: MSE=0.90438 (1.01375) -> grows
proc dplantidReader {fd} {
    set i 0
    global etaHidden$fd etaOutput$fd
    while {[gets $fd line] >= 0} {
	#puts "reader: $line"
	switch -regexp -- $line {
	    {^Iteration.*repeat with} {
		regexp {MSE=([^ ]*) .*repeat with *\(([^,]*), ([^,]*),} $line dummy learnMSE etaHidden$fd etaOutput$fd
	    }
	    {^Iteration} {
		regexp {MSE=([^ ]*) } $line dummy learnMSE
	    }
	    {^[ ]*Test:} {
		regexp {MSE=([^ ]*) } $line dummy testMSE
		return "$learnMSE $testMSE [set etaHidden$fd] [set etaOutput$fd]"
	    }
	}
	incr i
    }
    return {}
}

# 8. Run the program in its session directory
proc dplantidRun {p sessionDir parFile} {
    set cwd [pwd]
    puts "Run dplantid in [SessionDir $sessionDir]"
    catch {cd [SessionDir $sessionDir]} errCode1
    if {$errCode1 != ""} {
	error $errCode1
	return
    } else {
	set exepath [file join [SystemDir] bin dplantid]
	#if {![file executable $exepath]} {
	#    error "$exepath is not executable"
	#}

	global dplantid_params
	set params {
	    winWidth 600
	    winHeight 250
	    yMin 1e-5
	    yMax 10
	    yScale log
	    timeLabel "Epoch:"
	}
	set series { LearnMSE TestMSE EtaHidden EtaOutput }
	set pipe [open "|\"$exepath\" \"$parFile\"" r]
	fconfigure $pipe -buffering line

	set pipepar [fconfigure $pipe]
	#puts $pipepar

	lappend params \
	    timeLen $dplantid_params(finish_max_epoch) \
	    stopCmd "catch {close $pipe}" \
	    workDir [SessionDir $sessionDir]

	global etaHidden$pipe etaOutput$pipe
	set etaHidden$pipe $dplantid_params(eta)
	set etaOutput$pipe $dplantid_params(eta_output)
	RtSeriesWindow $p "NN training" "dplantidReader $pipe" $params $series
	catch {close $pipe}

	#catch {exec  $parFile >$NullDev} errCode2
	#if {$errCode2 != ""} {
	#    error $errCode2
	#}
	cd $cwd
    }

    # 9. Refresh state of controls
    # TODO

    # 10. It's possible to display log
    set logFile [file rootname $parFile].log
    $p.controls.log configure \
	-command "TextEditWindow $p \"$logFile\" \"$logFile\""
}

# Add given checkpoint to plotter window or display the data file.
proc dplantidCheckPoint {p chkpnt sessionDir arrayName arrayIndex label} {
    upvar #0 $arrayName ar
    set fileName $ar($arrayIndex)
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
proc dplantidLoadParams {parFile} {
    global dplantid_params
    array set dplantid_params {}
    ParFileFetch $parFile dplantid_params
}


# Select data file, assign it to given global array item and return it.
proc dplantidDataFile {p sessionDir arrayName arrayIndex} {
    upvar #0 $arrayName ar
    set fileRelPath $ar($arrayIndex)
    set filePath [SessionAbsPath $sessionDir $fileRelPath]
    set dataFileTypes {
	{"Файлы данных" {.dat}}
	{"Все файлы" *}
    }
    set filePath [fileSelectionBox $p open $filePath $dataFileTypes]
    if {$filePath == {}} {
	return ""
    }
    set ar($arrayIndex) [set fileRelPath [SessionRelPath $sessionDir $filePath]]
    puts "selected: $fileRelPath"
    return $fileRelPath
}


# Create window with panel and controls.
proc dplantidCreateWindow {p title sessionDir} {
    set w $p.dplantid

    # Don't create the window twice
    if {[winfo exists $w]} return

    toplevel $w
    wm title $w "Neural network plant model training out-of-loop; (session $sessionDir)"
    wm iconname $w "$sessionDir"

    # 1. Use current session directory
    set curSessionDir $sessionDir
 
    set parFile [file join [SessionDir $curSessionDir] dplantid.par]
    if {![file exists $parFile]} {
	# 2. Create default parameters from templates
	file copy -force [file join [TemplateDir] dplantid.par] $parFile
    }

    # 3. Assign parameters from file
    global dplantid_params
    array set dplantid_params {}
    dplantidLoadParams $parFile
    if {![info exists dplantid_params(comment)]} {
	set dplantid_params(comment) ""
    }

    global dplantid_grSeries
    set dplantid_grSeries {}

    # Headline
    set hl $w.headline
    frame $hl
    set titleFont [option get $hl headlineFont ""]
    button $hl.s -text "Сеанс $sessionDir" \
	-relief flat -padx 0 -pady 0 \
	-justify left -anchor nw -fg DarkGreen -font $titleFont \
	-command "TextEditWindow $w \"$parFile\" \"$parFile\" dplantidLoadParams"
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
    entry $uc.e -textvariable dplantid_params(comment) \
	-width 30 -relief flat -font $titleFont -bg white
    bind $uc.e <Return> "ParFileAssign \"$parFile\" dplantid_params"
    bind $uc.e <FocusOut> "ParFileAssign \"$parFile\" dplantid_params"
    pack $uc.l -side left
    pack $uc.e -side left -fill x -expand true
    pack $uc -side top -fill x -pady 2m

    # 4. Draw control system loop schema
    frame $w.controls
    pack $w.controls -side bottom -fill x -pady 2m

    set c $w.frame.c
    ScreenshotButton $w $w.controls.print $c [SessionDir $curSessionDir] "dplantid"

    button $w.controls.run -text "Запустить" \
	-command "dplantidRun $w \"$curSessionDir\" \"$parFile\""
    button $w.controls.log -text "Протокол"
    button $w.controls.series -text "Графики" \
	-command "GrSeriesWindow $w \"NN-P out-of-loop training series plot\" \"[SessionDir $curSessionDir]\""
    button $w.controls.close -text "Закрыть" \
	-command "array set dplantid_params {} ; destroy $w"
    pack $w.controls.print $w.controls.run $w.controls.log \
	$w.controls.series $w.controls.close -side left -expand 1

    frame $w.frame
    pack $w.frame -side top -fill both -expand yes

    canvas $c -width 14c -height 9c -relief sunken -borderwidth 2 \
	-background white
    pack $c -side top -fill both -expand yes

    dplantidDrawPanel {} $c

    # 5. Connect callbacks with visual parameters settings
    foreach {dataSource parName} {
	learn_control in_u
	learn_observ in_y
	test_control test_in_u
	test_observ test_in_y } {
	$c.$dataSource configure \
	    -command "dplantidDataFile $w $curSessionDir dplantid_params $parName ; ParFileAssign \"$parFile\" dplantid_params"
    }

    # TODO: Change to NNPlantWindow
    $c.learn_nnp configure \
	-command "NNPlantWindow $w \"$curSessionDir\" dplantid_params in_nnp_file out_nnp_file; ParFileAssign \"$parFile\" dplantid_params"
    $c.test_nnp configure \
	-command "NNPlantWindow $w \"$curSessionDir\" dplantid_params in_nnp_file out_nnp_file; ParFileAssign \"$parFile\" dplantid_params"

    $c.learn_training configure \
	-command "NNTeacherParWindow $w dplantid_params \$OfflineNNTeacherPar; ParFileAssign \"$parFile\" dplantid_params"

    # Assign name of check point output files
    foreach {chkpnt parname} {
	learn_checkpoint_u in_u
	learn_checkpoint_y in_y
	learn_checkpoint_nny nn_y
	learn_checkpoint_yd tr_y
	test_checkpoint_u test_in_u
	test_checkpoint_y test_in_y
	test_checkpoint_nny test_nn_y
	test_checkpoint_yd test_tr_y } {
	set label [$c.$chkpnt cget -text]
	switch -glob $chkpnt {
	    learn_* {
		set label [format "learn(%s)" $label]
	    }
	    test_* {
		set label [format "test(%s)" $label]
	    }
	}
	$c.$chkpnt configure \
	    -command "dplantidCheckPoint $w $chkpnt \"$curSessionDir\" dplantid_params $parname \{$label\}"
    }

    # 
    # Prepare template parameters file
    # Provide its name for editing by $w.controls.params
    # At $w.controls.run replaces several parameters by preset values
    # During the run display progress bar
    # After the run gather files and be ready to draw series
    # Drawing series may be tuned on and off in custom manner
}

proc dplantidTest {} {
    set c .c
    canvas $c -width 14c -height 9c -relief sunken -borderwidth 2 \
	-background white
    pack $c -side top -fill both -expand yes

    dplantidDrawPanel {} $c
}

#dplantidTest

# End of file
