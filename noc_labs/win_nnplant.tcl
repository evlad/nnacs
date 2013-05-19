package provide win_nnplant 1.0

package require Tk
package require universal
package require win_textedit
package require win_nnpedit

proc NNPlantWindowOk {w entry var} {
    NNPlantWindowApply $w $entry $var
    destroy $w
}

proc NNPlantWindowApply {w entry var} {
    upvar #0 $var fileName
    set fileName [$entry get]
    puts "NNPlantWindowApply: '$fileName'"

    # Restore normal attributes
    set normalBg [$w.buttons.cancel cget -bg]
    set normalFg [$w.buttons.cancel cget -fg]
    set activeBg [$w.buttons.cancel cget -activebackground]
    set activeFg [$w.buttons.cancel cget -activeforeground]
    $w.buttons.ok configure -bg $normalBg -fg $normalFg \
	-activebackground $activeBg -activeforeground $activeFg
    #$w.buttons.apply configure -bg $normalBg -fg $normalFg \
#	-activebackground $activeBg -activeforeground $activeFg
}

proc NNPlantWindowEdit {w title var} {
    upvar #0 $var fileName
    TextEditWindow $w "$title" $fileName
}

# Select file for neural net plant model and store new value to var
# global variable.
proc NNPlantSelectNNFile {p sessionDir var} {
    global $var
    upvar #0 $var fileRelPath
    set fileName [SessionAbsPath $sessionDir $fileRelPath]
    set nnfiletypes {
	{"Нейронные сети" {.nn}}
	{"Все файлы" *}
    }
    set fileName [fileSelectionBox $p open [file join SessionDir $fileName] $nnfiletypes]
    if {$fileName != {}} {
	set fileRelPath [SessionRelPath $sessionDir $fileName]
    }
}


# Display neural network where filepath is referred by var.
proc NNPlantViewNNFile {p sessionDir var} {
    global $var
    upvar #0 $var fileRelPath
    DisplayNeuralNetArch $p $fileRelPath [SessionAbsPath $sessionDir $fileRelPath]
}

# Create neural network where filepath is referred by var1.
proc NNPlantCreateNNFile {p sessionDir var1} {
    global $var1
    upvar #0 $var1 fileRelPath
    NNPEditWindow $p "Create neural network for plant" [SessionAbsPath $sessionDir $fileRelPath]
}

# Create dialog window with neural network settings.
# - p - parent widget;
# - sessionDir - current session directory (for relative paths mostly);
# - arref - name of global array variable where to store settings;
# - nnp_in - index of input neural network file;
# - nnp_out - index of output neural network file (may be {} to skip).
# Return: 1 if some changes took place and 0 otherwise.
proc NNPlantWindow {p sessionDir arref nnp_in nnp_out} {
    upvar $arref arvar

    set w $p.nnp
    catch {destroy $w}
    toplevel $w
    wm title $w "NN plant settings"

    global var_nnp_in var_nnp_out
    set var_nnp_in [SessionRelPath $sessionDir $arvar($nnp_in)]
    if {$nnp_out != {}} {
	set var_nnp_out [SessionRelPath $sessionDir $arvar($nnp_out)]
    } else {
	set var_nnp_out {}
    }
    global $w.applyChanges
    set $w.applyChanges 0

    set f $w.p
    frame $f

    label $f.title -text "Нейросетевая модель объекта"
    grid $f.title
    grid $f.title -row 0 -column 0 -columnspan 4

    label $f.in_fl -text "Исходная нейронная сеть:"
    entry $f.in_fe -width 30 -textvariable var_nnp_in
    button $f.in_fsel -text "Выбор..." \
	-command "NNPlantSelectNNFile $w \"$sessionDir\" var_nnp_in"
    button $f.in_fview -text "Показать..." \
	-command "NNPlantViewNNFile $w \"$sessionDir\" var_nnp_in"
    button $f.in_fcreate -text "Создать..." \
	-command "NNPlantCreateNNFile $w \"$sessionDir\" var_nnp_in"
    grid $f.in_fcreate -row 1 -column 3
    grid $f.in_fl $f.in_fe $f.in_fsel $f.in_fview
    grid $f.in_fl -sticky e

    if {$nnp_out != {}} {
	label $f.out_fl -text "Результат обучения:"
	entry $f.out_fe -width 30 -textvariable var_nnp_out
	button $f.out_fsel -text "Выбор..." \
	    -command "NNPlantSelectNNFile $w \"$sessionDir\" var_nnp_out"
	button $f.out_fview -text "Показать..." \
	    -command "NNPlantViewNNFile $w \"$sessionDir\" var_nnp_out"
	grid $f.out_fl $f.out_fe $f.out_fsel $f.out_fview
	grid $f.out_fl -sticky e
    }
    pack $f -side top

    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.ok -text "OK" \
	-command "set $w.applyChanges 1 ; destroy $w"
    button $w.buttons.cancel -text "Отмена" -command "destroy $w"
    pack $w.buttons.ok $w.buttons.cancel -side left -expand 1

    tkwait window $w

    set changed 0
    if {[set $w.applyChanges]} {
	puts "win_nnplant: apply changes"
	if {[set var_nnp_in] != $arvar($nnp_in)} {
	    set arvar($nnp_in) [SessionRelPath $sessionDir $var_nnp_in]
	    set changed 1
	}
	if {$nnp_out != {}} {
	    if {[set var_nnp_out] != $arvar($nnp_out)} {
		set arvar($nnp_out) [SessionRelPath $sessionDir $var_nnp_out]
		set changed 1
	    }
	}
    }
    if {$changed == 0} {
	puts "win_nnplant: no changes"
    }
    return $changed
}
