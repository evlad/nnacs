package provide win_nntpar 1.0

package require Tk
package require universal

# Good for dcontrp and dplantid
set OfflineNNTeacherPar {
    -1 "Параметры обучения"
    eta "Скорость обучения скрытых нейронов"
    eta_output "Скорость обучения выходных нейронов"
    alpha "Коэффициент инерции обучения (моментум)"
    -2 "Параметры останова"
    finish_on_value "Нижняя граница ошибки на тестовой выборке"
    finish_on_decrease "Нижняя граница изменения ошибки на тестовой выборке"
    finish_on_grow "Предельное количество эпох с ростом тестовой ошибки"
    finish_max_epoch "Предельное количество эпох обучения"
}

# Good for dcontrf
set OnlineNNTeacherPar {
    -1 "Параметры обучения"
    nnc_auf "Шаг обновления НС-Р по времени"
    eta "Скорость обучения скрытых нейронов"
    eta_output "Скорость обучения выходных нейронов"
    alpha "Коэффициент инерции обучения (моментум)"
    -2 "Параметры останова"
    skip_growing "Пропустить в начале указанное количество эпох с ростом ошибки"
    finish_decrease "Нижняя граница изменения ошибки управления"
    finish_on_grow "Предельное количество эпох с ростом ошибки управления"
    max_epochs "Предельное количество эпох обучения"
}

# Create dialog window with NN training parameters.
# - p - parent widget;
# - arref - name of global array variable where to store settings
#           under next names:
# - parlist - list of parameters and their labels.
proc NNTeacherParWindow {p arref parlist} {
    upvar $arref arvar

    set w $p.nntpar
    catch {destroy $w}
    toplevel $w
    wm title $w "NN training parameters"

    global $w.parvalue
    array set pardescr $parlist

    # Make local copy
    foreach par [array names pardescr] {
	if {[info exists arvar($par)]} {
	    set $w.parvalue($par) $arvar($par)
	} else {
	    set $w.parvalue($par) {}
	}
    }

    set f $w.p
    frame $f

    set row 0
    foreach {par parlabel} $parlist {
	switch -glob -- $par {
	    -* { # Title
		label $f.title$par -text $pardescr($par)
		grid $f.title$par
		grid $f.title$par -row $row -column 0 -columnspan 2
	    }
	    default {
		label $f.label_$par -text $pardescr($par) -anchor w
		entry $f.entry_$par -textvariable $w.parvalue($par) -width 10
		grid $f.label_$par $f.entry_$par -sticky nw
	    }
	}
	incr row
    }

    pack $f -side top

    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m

    button $w.buttons.ok -text "OK" \
	-command "set $w.applyChanges 1 ; destroy $w"
    button $w.buttons.cancel -text "Отмена" -command "destroy $w"
    pack $w.buttons.ok $w.buttons.cancel -side left -expand 1

    global $w.applyChanges
    set $w.applyChanges 0

    tkwait window $w

    set changed 0
    if {[set $w.applyChanges]} {
	puts "win_nntpar: apply changes"

	# Return changed values back
	foreach par [array names pardescr] {
	    if {[info exists arvar($par)]} {
		if {$arvar($par) != [set $w.parvalue($par)]} {
		    set arvar($par) [set $w.parvalue($par)]
		    set changed 1
		}
	    } elseif {[set $w.parvalue($par)] != {}} {
		set arvar($par) [set $w.parvalue($par)]
		set changed 1
	    }
	}
    }
    if {$changed == 0} {
	puts "win_nntpar: no changes"
    }
    return $changed
}
