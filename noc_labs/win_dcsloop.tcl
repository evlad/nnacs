package provide win_dcsloop 1.0

#set font "-*-helvetica-*-r-*-*-14-*-*-*-*-*-koi8-*"

package require files_loc
package require draw_prim
package require par_file
package require win_textedit
package require win_controller
package require win_plant
package require win_signal
package require screenshot

# Draw panel contents in given canvas
proc dcsloopDrawPanel {this c} {
    set textFont [option get $c fontLargeBlock ""]
    $c create text 0.5c 0.6c -text "Контур автоматического управления:" \
	-justify left -anchor nw -fill black -font "$textFont"

    DrawLargeBlock $c reference "Уставка" 1.8c 4c
    DrawSmallBlock $c checkpoint_r "r" 3.5c 4c
    DrawGather $c cerr 4.5c 4c "s"
    DrawSmallBlock $c checkpoint_e "e" 5.4c 4c
    DrawLargeBlock $c controller "Регулятор" 7.1c 4c
    DrawSmallBlock $c checkpoint_u "u" 8.8c 4c
    DrawLargeBlock $c plant "Объект" 10.4c 4c
    DrawGather $c nadd 12c 4c "none"
    DrawLargeBlock $c noise "Помеха" 12c 1.7c
    DrawSmallBlock $c checkpoint_n "n" 12c 3c
    DrawSmallBlock $c checkpoint_y "y" 12c 5.3c

    DrawDirection $c reference "e" checkpoint_r "w" last
    DrawDirection $c checkpoint_r "e" cerr "w" last
    DrawDirection $c cerr "e" checkpoint_e "w" last
    DrawDirection $c checkpoint_e "e" controller "w" last
    DrawDirection $c controller "e" checkpoint_u "w" last
    DrawDirection $c checkpoint_u "e" plant "w" last
    DrawDirection $c plant "e" nadd "w" last

    DrawDirection $c noise "s" checkpoint_n "n" last
    DrawDirection $c checkpoint_n "s" nadd "n" last
    DrawDirection $c nadd "s" checkpoint_y "n" last

    DrawDirection $c checkpoint_y "w" cerr "s" hor

#    DrawArrow $c 6c 5c 9c 5c middle
#    DrawArrow $c 9c 5c 11c 5c last
#    DrawArrow $c 11c 2c 11c 5c last
}


# 8. Run the program in its session directory
proc dcsloopRun {p sessionDir parFile} {
    set cwd [pwd]
    puts "Run dcsloop"
    catch {cd [SessionDir $sessionDir]} errCode1
    if {$errCode1 != ""} {
	error $errCode1
	return
    }

    global NullDev
    catch {exec [file join [SystemDir] bin dcsloop] $parFile >$NullDev} errCode2
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
proc dcsloopCheckPoint {p chkpnt sessionDir fileName label} {
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
proc dcsloopLoadParams {parFile} {
    global dcsloop_params
    array set dcsloop_params {}
    ParFileFetch $parFile dcsloop_params
}

# Create window with panel and controls.
proc dcsloopCreateWindow {p title sessionDir} {
    set w $p.dcsloop

    # Don't create the window twice
    if {[winfo exists $w]} return

    toplevel $w
    wm title $w "Control system loop modeling; (session $sessionDir)"
    wm iconname $w "$sessionDir"

    # 1. Use current session directory
    set curSessionDir $sessionDir
 
    set parFile [file join [SessionDir $curSessionDir] dcsloop.par]
    if {![file exists $parFile]} {
	# 2. Create default parameters from templates
	file copy -force [file join [TemplateDir] dcsloop.par] $parFile
    }

    # 3. Assign parameters from file
    global dcsloop_params
    array set dcsloop_params {}
    dcsloopLoadParams $parFile
    if {![info exists dcsloop_params(comment)]} {
	set dcsloop_params(comment) ""
    }

    global dcsloop_grSeries
    set dcsloop_grSeries {}

    # Headline
    set hl $w.headline
    frame $hl
    set titleFont [option get $hl headlineFont ""]
    button $hl.s -text "Сеанс $sessionDir" \
	-relief flat -padx 0 -pady 0 \
	-justify left -anchor nw -fg DarkGreen -font $titleFont \
	-command "TextEditWindow $w \"$parFile\" \"$parFile\" dcsloopLoadParams"

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
    entry $uc.e -textvariable dcsloop_params(comment) \
	-width 30 -relief flat -font $titleFont -bg white
    bind $uc.e <Return> "ParFileAssign \"$parFile\" dcsloop_params"
    bind $uc.e <FocusOut> "ParFileAssign \"$parFile\" dcsloop_params"
    pack $uc.l -side left
    pack $uc.e -side left -fill x -expand true
    pack $uc -side top -fill x -pady 2m

    # 4. Draw control system loop schema
    frame $w.controls
    pack $w.controls -side bottom -fill x -pady 2m

    set c $w.frame.c
    ScreenshotButton $w $w.controls.print $c [SessionDir $curSessionDir] "dcsloop"

    button $w.controls.run -text "Запустить" \
	-command "dcsloopRun $w \"$curSessionDir\" \"$parFile\""
    button $w.controls.log -text "Протокол"
    button $w.controls.series -text "Графики" \
	-command "GrSeriesWindow $w \"Control loop modeling series plot\" \"[SessionDir $curSessionDir]\""
    button $w.controls.close -text "Закрыть" \
	-command "array set dcsloop_params {} ; destroy $w"
    pack $w.controls.print $w.controls.run $w.controls.log \
	$w.controls.series $w.controls.close -side left -expand 1

    frame $w.frame
    pack $w.frame -side top -fill both -expand yes

    canvas $c -width 14c -height 7c -relief sunken -borderwidth 2 \
	-background white
    pack $c -side top -fill both -expand yes

    #set t "Моделирование системы автоматического управления"
    #set textFont [option get $c fontLargeBlock ""]
    #$c create text 0.5c 0.2c -text "$title\nСеанс $sessionDir" \
	#	-justify left -anchor nw -fill DarkGreen -font "$textFont"

    dcsloopDrawPanel {} $c

    # 5. Connect callbacks with visual parameters settings
    # (reference+noise, modelling length) including selection of tf
    $c.reference configure \
	-command "SignalWindow $w \"$curSessionDir\" refer dcsloop_params input_kind in_r refer_tf stream_len ; ParFileAssign \"$parFile\" dcsloop_params"
    $c.controller configure \
	-command "ContrWindow $w \"$curSessionDir\" dcsloop_params contr_kind lincontr_tf nncontr nnc_mode ; ParFileAssign \"$parFile\" dcsloop_params"
    $c.plant configure \
	-command "PlantWindow $w \"$curSessionDir\" dcsloop_params linplant_tf ; ParFileAssign \"$parFile\" dcsloop_params"
    $c.noise configure \
	-command "SignalWindow $w \"$curSessionDir\" noise dcsloop_params input_kind in_n noise_tf stream_len ; ParFileAssign \"$parFile\" dcsloop_params"

    # Assign name of check point output files
    foreach {chkpnt parname} {
	checkpoint_r out_r
	checkpoint_n out_n
	checkpoint_u out_u
	checkpoint_e out_e
	checkpoint_y out_ny} {
	set label [$c.$chkpnt cget -text]
	$c.$chkpnt configure \
	    -command "dcsloopCheckPoint $w $chkpnt \"$curSessionDir\" $dcsloop_params($parname) \{$label\}"
    }

    # 
    # Prepare template parameters file
    # Provide its name for editing by $w.controls.params
    # At $w.controls.run replaces several parameters by preset values
    # During the run display progress bar
    # After the run gather files and be ready to draw series
    # Drawing series may be tuned on and off in custom manner
}

#proc dcsloopSingle {} {
#    NewUser "" user1
#    dcsloopCreateWindow ""
#}

#dcsloopSingle

# End of file
