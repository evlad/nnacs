package provide win_dcontrp 1.0

package require files_loc
package require draw_prim
package require par_file
package require win_textedit
package require win_nncontr
package require win_nntpar
package require win_rtseries
package require screenshot

# Draw panel contents in given canvas
proc dcontrpDrawPanel {this c} {
    set textFont [option get $c fontLargeBlock ""]

    $c create text 0.5c 0.8c -text "Обучение нейронной сети (НС-Р):" \
	-justify left -anchor nw -fill black -font "$textFont"

    DrawLargeBlock $c learn_reference "Уставка" 2.5c 2c
    DrawSmallBlock $c learn_checkpoint_r "r" 4.1c 2c
    DrawLargeBlock $c learn_error "Ошибка" 2.5c 3c
    DrawSmallBlock $c learn_checkpoint_e "e" 4.1c 3c
    DrawLargeBlock $c learn_nnc "\nНС-Р\n" 5.5c 2.5c
    DrawSmallBlock $c learn_checkpoint_nu "u'" 7c 2.5c
    DrawGather $c learn_cmp 8c 2.5c "w"
    DrawSmallBlock $c learn_checkpoint_u "u" 9c 2.5c
    DrawLargeBlock $c learn_control "Управление" 11c 2.5c
    DrawLargeBlock $c learn_training "Обучение" 8c 4c

    DrawDirection $c learn_reference "e" learn_checkpoint_r "w" last
    DrawDirection $c learn_checkpoint_r "e" learn_nnc "w" horOnly
    DrawDirection $c learn_error "e" learn_checkpoint_e "w" last
    DrawDirection $c learn_checkpoint_e "e" learn_nnc "w" horOnly
    DrawDirection $c learn_nnc "e" learn_checkpoint_nu "w" last
    DrawDirection $c learn_checkpoint_nu "e" learn_cmp "w" last
    DrawDirection $c learn_control "w" learn_checkpoint_u "e" last
    DrawDirection $c learn_checkpoint_u "w" learn_cmp "e" last
    DrawDirection $c learn_cmp "s" learn_training "n" last
    DrawDirection $c learn_training "w" learn_nnc "se" last {color "red"}

    $c create text 0.5c 4.8c -text "Проверка нейронной сети (НС-Р):" \
	-justify left -anchor nw -fill black -font "$textFont"

    DrawLargeBlock $c test_reference "Уставка" 2.5c 6c
    DrawSmallBlock $c test_checkpoint_r "r" 4.1c 6c
    DrawLargeBlock $c test_error "Ошибка" 2.5c 7c
    DrawSmallBlock $c test_checkpoint_e "e" 4.1c 7c
    DrawLargeBlock $c test_nnc "\nНС-Р\n" 5.5c 6.5c
    DrawSmallBlock $c test_checkpoint_nu "u'" 7c 6.5c
    DrawGather $c test_cmp 8c 6.5c "w"
    DrawSmallBlock $c test_checkpoint_u "u" 9c 6.5c
    DrawLargeBlock $c test_control "Управление" 11c 6.5c
    DrawLargeBlock $c test_training "Проверка" 8c 8c

    DrawDirection $c test_reference "e" test_checkpoint_r "w" last
    DrawDirection $c test_checkpoint_r "e" test_nnc "w" horOnly
    DrawDirection $c test_error "e" test_checkpoint_e "w" last
    DrawDirection $c test_checkpoint_e "e" test_nnc "w" horOnly
    DrawDirection $c test_nnc "e" test_checkpoint_nu "w" last
    DrawDirection $c test_checkpoint_nu "e" test_cmp "w" last
    DrawDirection $c test_control "w" test_checkpoint_u "e" last
    DrawDirection $c test_checkpoint_u "w" test_cmp "e" last
    DrawDirection $c test_cmp "s" test_training "n" last
}

# Read given descriptor until Test: and return both learn and test
# MSE.  At the EOF return {}.
# Example:
# Iteration 3   , MSE=4.45605 (4.51003) -> repeat with (0.01, 0.001, 0)
# Iteration 1   , MSE=1.03622 (1.04878) -> teach NN
#           Test: MSE=0.90438 (1.01375) -> grows
proc dcontrpReader {fd} {
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
proc dcontrpRun {p sessionDir parFile} {
    set cwd [pwd]
    puts "Run dcontrp in [SessionDir $sessionDir]"
    catch {cd [SessionDir $sessionDir]} errCode1
    if {$errCode1 != ""} {
	error $errCode1
	return
    } else {
	set exepath [file join [SystemDir] bin dcontrp]
	#if {![file executable $exepath]} {
	#    error "$exepath is not executable"
	#}

	global dcontrp_params
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
	    timeLen $dcontrp_params(finish_max_epoch) \
	    stopCmd "catch {close $pipe}" \
	    workDir [SessionDir $sessionDir]

	global etaHidden$pipe etaOutput$pipe
	set etaHidden$pipe $dcontrp_params(eta)
	set etaOutput$pipe $dcontrp_params(eta_output)
	RtSeriesWindow $p "NN training" "dcontrpReader $pipe" $params $series
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
proc dcontrpCheckPoint {p chkpnt sessionDir arrayName arrayIndex label} {
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
proc dcontrpLoadParams {parFile} {
    global dcontrp_params
    array set dcontrp_params {}
    ParFileFetch $parFile dcontrp_params
}


# Select data file, assign it to given global array item and return it.
proc dcontrpDataFile {p sessionDir arrayName arrayIndex} {
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
proc dcontrpCreateWindow {p title sessionDir} {
    set w $p.dcontrp

    # Don't create the window twice
    if {[winfo exists $w]} return

    toplevel $w
    wm title $w "Neural network controller training out-of-loop; (session $sessionDir)"
    wm iconname $w "$sessionDir"

    # 1. Use current session directory
    set curSessionDir $sessionDir
 
    set parFile [file join [SessionDir $curSessionDir] dcontrp.par]
    if {![file exists $parFile]} {
	# 2. Create default parameters from templates
	file copy -force [file join [TemplateDir] dcontrp.par] $parFile
    }

    # 3. Assign parameters from file
    global dcontrp_params
    array set dcontrp_params {}
    dcontrpLoadParams $parFile
    if {![info exists dcontrp_params(comment)]} {
	set dcontrp_params(comment) ""
    }

    global dcontrp_grSeries
    set dcontrp_grSeries {}

    # Headline
    set hl $w.headline
    frame $hl
    set titleFont [option get $hl headlineFont ""]
    button $hl.s -text "Сеанс $sessionDir" \
	-relief flat -padx 0 -pady 0 \
	-justify left -anchor nw -fg DarkGreen -font $titleFont \
	-command "TextEditWindow $w \"$parFile\" \"$parFile\" dcontrpLoadParams"
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
    entry $uc.e -textvariable dcontrp_params(comment) \
	-width 30 -relief flat -font $titleFont -bg white
    bind $uc.e <Return> "ParFileAssign \"$parFile\" dcontrp_params"
    bind $uc.e <FocusOut> "ParFileAssign \"$parFile\" dcontrp_params"
    pack $uc.l -side left
    pack $uc.e -side left -fill x -expand true
    pack $uc -side top -fill x -pady 2m

    # 4. Draw control system loop schema
    frame $w.controls
    pack $w.controls -side bottom -fill x -pady 2m
    set c $w.frame.c
    ScreenshotButton $w $w.controls.print $c [SessionDir $curSessionDir] "dcontrp"

    button $w.controls.run -text "Запустить" \
	-command "dcontrpRun $w \"$curSessionDir\" \"$parFile\""
    button $w.controls.log -text "Протокол"
    button $w.controls.series -text "Графики" \
	-command "GrSeriesWindow $w \"NN-C out-of-loop training series plot\" \"[SessionDir $curSessionDir]\""
    button $w.controls.close -text "Закрыть" \
	-command "array set dcontrp_params {} ; destroy $w"
    pack $w.controls.print $w.controls.run $w.controls.log \
	$w.controls.series $w.controls.close -side left -expand 1

    frame $w.frame
    pack $w.frame -side top -fill both -expand yes

    canvas $c -width 14c -height 9c -relief sunken -borderwidth 2 \
	-background white
    pack $c -side top -fill both -expand yes

    dcontrpDrawPanel {} $c

    # 5. Connect callbacks with visual parameters settings
    foreach {dataSource parName} {
	learn_reference in_r
	learn_error in_e
	learn_control in_u
	test_reference test_in_r
	test_error test_in_e
	test_control test_in_u } {
	$c.$dataSource configure \
	    -command "dcontrpDataFile $w \"$curSessionDir\" dcontrp_params $parName ; ParFileAssign \"$parFile\" dcontrp_params"
    }

    $c.learn_nnc configure \
	-command "NNContrWindow $w \"$curSessionDir\" dcontrp_params in_nnc_file out_nnc_file nnc_mode; ParFileAssign \"$parFile\" dcontrp_params"
    $c.test_nnc configure \
	-command "NNContrWindow $w \"$curSessionDir\" dcontrp_params in_nnc_file out_nnc_file nnc_mode; ParFileAssign \"$parFile\" dcontrp_params"

    $c.learn_training configure \
	-command "NNTeacherParWindow $w dcontrp_params \$OfflineNNTeacherPar; ParFileAssign \"$parFile\" dcontrp_params"

    # Assign name of check point output files
    foreach {chkpnt parname} {
	learn_checkpoint_r in_r
	learn_checkpoint_e in_e
	learn_checkpoint_nu nn_u
	learn_checkpoint_u in_u
	test_checkpoint_r test_in_r
	test_checkpoint_e test_in_e
	test_checkpoint_nu test_nn_u
	test_checkpoint_u test_in_u } {
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
	    -command "dcontrpCheckPoint $w $chkpnt \"$curSessionDir\" dcontrp_params $parname \{$label\}"
    }

    # 
    # Prepare template parameters file
    # Provide its name for editing by $w.controls.params
    # At $w.controls.run replaces several parameters by preset values
    # During the run display progress bar
    # After the run gather files and be ready to draw series
    # Drawing series may be tuned on and off in custom manner
}

# End of file
