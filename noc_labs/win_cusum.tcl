package provide win_cusum 1.0

package require Tk
package require universal

# Create dialog window with cumulative sum method settings.
# - p - parent widget;
# - sessionDir - current session directory (for relative paths mostly);
# - arref - name of global array variable where to store settings;
# - sigma0 - index of initial signal variation;
# - sigma1 - index of nominal disorder signal variation;
# - h_sol - index of solution border;
# - k_const - index of ???;
# - det_int - index of interval used to detect disorder.
# Return: 1 if some changes took place and 0 otherwise.
proc CusumWindow {p sessionDir arref sigma0 sigma1 h_sol k_const det_int} {
    upvar $arref arvar

    set w $p.cusum
    catch {destroy $w}
    toplevel $w
    wm title $w "Cusum settings"

    global var_sigma0 var_sigma1 var_h_sol var_k_const var_det_int
    set var_sigma0 $arvar($sigma0)
    set var_sigma1 $arvar($sigma1)
    set var_h_sol $arvar($h_sol)
    set var_k_const $arvar($k_const)
    set var_det_int $arvar($det_int)

    global $w.applyChanges
    set $w.applyChanges 0

    set f $w.p
    frame $f

    label $f.title -text "Параметры алгоритма кумулятивных сумм"
    grid $f.title
    grid $f.title -row 0 -column 0 -columnspan 4

    #sigma0 sigma1 h_sol det_int
    grid [label $f.l_sigma0 -text "Стандартное значение \u03c3\u2080"] \
	[entry $f.e_sigma0 -textvariable var_sigma0 \
	     -vcmd {string is double %P}] \
	-sticky e

    grid [label $f.l_sigma1 -text "Номинальная разладка \u03c3\u2081"] \
	[entry $f.e_sigma1 -textvariable var_sigma1 \
	     -vcmd {string is double %P}] \
	-sticky e

    #grid [label $f.l_k_const -text "Kconst"] \
    #	[entry $f.e_k_const -textvariable var_k_const \
    #	    -vcmd {string is double %P}]] \
    #   -sticky e

    grid [label $f.l_h_sol -text "Решающая граница"] \
	[entry $f.e_h_sol -textvariable var_h_sol \
	     -vcmd {string is double %P}] \
	-sticky e

    grid [label $f.l_det_int -text "Интервал второго срабатывания"] \
	[entry $f.e_det_int -textvariable var_det_int \
	     -vcmd {string is integer %P}] \
	-sticky e

    foreach v {sigma0 sigma1 h_sol det_int} {
	set e $f.e_$v
	$e configure -width 10 -justify right -validate focus \
	    -invalidcommand "focusAndFlash %W [$e cget -fg] [$e cget -bg]"
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
	puts "win_cusum: apply changes"
	if {$var_sigma0 != $arvar($sigma0)} {
	    set arvar($sigma0) $var_sigma0
	    set changed 1
	}
	if {$var_sigma1 != $arvar($sigma1)} {
	    set arvar($sigma1) $var_sigma1
	    set changed 1
	}
	if {$var_k_const != $arvar($k_const)} {
	    set arvar($k_const) $var_k_const
	    set changed 1
	}
	if {$var_h_sol != $arvar($h_sol)} {
	    set arvar($h_sol) $var_h_sol
	    set changed 1
	}
	if {$var_det_int != $arvar($det_int)} {
	    set arvar($det_int) $var_det_int
	    set changed 1
	}
    }
    if {$changed == 0} {
	puts "win_cusum: no changes"
    }
    return $changed
}
