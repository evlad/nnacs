package provide win_nncontr 1.0

package require Tk
package require universal
package require win_textedit
package require win_nncedit

proc NNContrWindowOk {w entry var} {
    NNContrWindowApply $w $entry $var
    destroy $w
}

proc NNContrWindowApply {w entry var} {
    upvar #0 $var fileName
    set fileName [$entry get]
    puts "NNContrWindowApply: '$fileName'"

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

proc NNContrWindowEdit {w title var} {
    upvar #0 $var fileName
    TextEditWindow $w "$title" $fileName
}

# Select file for neural net controller and store new value to var
# global variable.
proc NNContrSelectNNFile {p sessionDir var} {
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
proc NNContrViewNNFile {p sessionDir var} {
    global $var
    upvar #0 $var fileRelPath
    DisplayNeuralNetArch $p $fileRelPath [SessionAbsPath $sessionDir $fileRelPath]
}

# Create neural network where filepath is referred by var1 and inputs by var2.
proc NNContrCreateNNFile {p sessionDir var1 var2} {
    global $var1 $var2
    upvar #0 $var1 fileRelPath
    upvar #0 $var2 nncInputs
    NNCEditWindow $p "Create neural network for controller" [SessionAbsPath $sessionDir $fileRelPath] $nncInputs
}

# Create dialog window with neural network settings.
# - p - parent widget;
# - sessionDir - current session directory (for relative paths mostly);
# - arref - name of global array variable where to store settings;
# - nnc_in - index of input neural network file;
# - nnc_out - index of output neural network file;
# - nncinputs - index of nn controller inputs architecture.
# Return: 1 if some changes took place and 0 otherwise.
proc NNContrWindow {p sessionDir arref nnc_in nnc_out nncinputs} {
    upvar $arref arvar

    set w $p.nnc
    catch {destroy $w}
    toplevel $w
    wm title $w "NN controller settings"

    global var_nnc_in var_nnc_out var_nncinputs
    set var_nnc_in [SessionRelPath $sessionDir $arvar($nnc_in)]
    set var_nnc_out [SessionRelPath $sessionDir $arvar($nnc_out)]
    set var_nncinputs $arvar($nncinputs)

    global $w.applyChanges
    set $w.applyChanges 0

    set f $w.p
    frame $f

    label $f.title -text "Нейросетевой регулятор"
    grid $f.title
    grid $f.title -row 0 -column 0 -columnspan 4

    label $f.in_fl -text "Исходная нейронная сеть:"
    entry $f.in_fe -width 30 -textvariable var_nnc_in
    button $f.in_fsel -text "Выбор..." \
	-command "NNContrSelectNNFile $w \"$sessionDir\" var_nnc_in"
    button $f.in_fview -text "Показать..." \
	-command "NNContrViewNNFile $w \"$sessionDir\" var_nnc_in"
    button $f.in_fcreate -text "Создать..." \
	-command "NNContrCreateNNFile $w \"$sessionDir\" var_nnc_in var_nncinputs"
    label $f.inp_l -text "Входы:"
    frame $f.inputs
    foreach {n v} {re "e+r" eee "e+e+..." ede "e+de"} {
	radiobutton $f.inputs.$n -variable var_nncinputs -value $v -text $v
	pack $f.inputs.$n -padx 2 -side left
    }
    grid $f.inp_l -row 1 -column 0
    grid $f.inputs -row 1 -column 1 -columnspan 2
    grid $f.in_fcreate -row 1 -column 3
    grid $f.in_fl $f.in_fe $f.in_fsel $f.in_fview
    grid $f.in_fl -sticky e
    grid $f.inp_l -sticky e
    grid $f.inputs -sticky w

    label $f.out_fl -text "Результат обучения:"
    entry $f.out_fe -width 30 -textvariable var_nnc_out
    button $f.out_fsel -text "Выбор..." \
	-command "NNContrSelectNNFile $w \"$sessionDir\" var_nnc_out"
    button $f.out_fview -text "Показать..." \
	-command "NNContrViewNNFile $w \"$sessionDir\" var_nnc_out"
    grid $f.out_fl $f.out_fe $f.out_fsel $f.out_fview
    grid $f.out_fl -sticky e

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
	puts "win_nncontr: apply changes"
	if {$var_nncinputs != $arvar($nncinputs)} {
	    set arvar($nncinputs) $var_nncinputs
	    set changed 1
	}
	if {[set var_nnc_in] != $arvar($nnc_in)} {
	    set arvar($nnc_in) [SessionRelPath $sessionDir $var_nnc_in]
	    set changed 1
	}
	if {[set var_nnc_out] != $arvar($nnc_out)} {
	    set arvar($nnc_out) [SessionRelPath $sessionDir $var_nnc_out]
	    set changed 1
	}
    }
    if {$changed == 0} {
	puts "win_nncontr: no changes"
    }
    return $changed
}
