package provide win_model 1.0

package require Tk
package require universal
package require win_nnplant
package require draw_nn


# Create dialog window with plant model settings.
# - p - parent widget;
# - sessionDir - current session directory (for relative paths mostly);
# - arref - name of global array variable where to store settings;
# - nnpfile - index of NN plant file;
# Return: 1 if some changes took place and 0 otherwise.
proc ModelWindow {p sessionDir arref nnpfile} {
    upvar $arref arvar

    set w $p.model
    catch {destroy $w}
    toplevel $w
    wm title $w "Model settings"

    #puts "arvar($trcfile)=$arvar($trcfile)"

    global var_nnpfile
    set var_nnpfile [SessionRelPath $sessionDir $arvar($nnpfile)]

    global $w.applyChanges
    set $w.applyChanges 0

    set f $w.p
    frame $f

    label $f.title -text "Модель объекта"
    grid $f.title
    grid $f.title -row 0 -column 0 -columnspan 4

    label $f.nnp_title -text "Нейросетевая модель"
    label $f.nnp_fl -text "Имя файла:"
    entry $f.nnp_fe -width 30 -textvariable var_nnpfile
    button $f.nnp_fsel -text "Выбор..." \
	-command "NNPlantSelectNNFile $w \"$sessionDir\" var_nnpfile"
    button $f.nnp_fview -text "Показать..." \
	-command "NNPlantViewNNFile $w \"$sessionDir\" var_nnpfile"

    grid $f.nnp_fl $f.nnp_fe $f.nnp_fsel $f.nnp_fview
    grid $f.nnp_fl -sticky e

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
	puts "win_model: apply changes"
	if {$var_nnpfile != $arvar($nnpfile)} {
	    set arvar($nnpfile) [SessionRelPath $sessionDir $var_nnpfile]
	    set changed 1
	}
    }
    if {$changed == 0} {
	puts "win_model: no changes"
    }
    return $changed
}

#font create myDefaultFont -family Freesans -size 11
#option add *font myDefaultFont
#option readfile noc_labs.ad

#set myvar "../d.cf"
#set myvar "pid.tf"
#puts $myvar
#ModelWindow "" "Plant function" myvar
#puts $myvar
