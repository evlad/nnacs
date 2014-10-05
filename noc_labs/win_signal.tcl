package provide win_signal 1.0

package require Tk
package require universal
package require win_textedit
package require win_grseries
package require win_trfunc
package require win_signalgen

proc SignalWindowOk {w entry var} {
    SignalWindowApply $w $entry $var
    destroy $w
}

proc SignalWindowApply {w entry var} {
    upvar #0 $var fileName
    set fileName [$entry get]
    puts "SignalWindowApply: '$fileName'"

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

proc SignalWindowEdit {w title var} {
    upvar #0 $var fileName
    TextEditWindow $w "$title" $fileName
}

proc SignalWindowModified {w entry} {
    #puts "Modified: $w $entry"
    # Set attributes of modified text contents
    set modifiedFg white
    set modifiedBg red
    $w.buttons.ok configure -bg $modifiedBg -fg $modifiedFg \
	-activebackground $modifiedBg -activeforeground $modifiedFg
#    $w.buttons.apply configure -bg $modifiedBg -fg $modifiedFg \
#	-activebackground $modifiedBg -activeforeground $modifiedFg
    return 1
}

# Display graphics series where filepath is referred by var.
proc SignalViewDataFile {p sessionDir var} {
    global $var
    upvar #0 $var fileRelPath
    GrSeriesWindow $p $fileRelPath [SessionAbsPath $sessionDir $fileRelPath]
}


# Display statistics
proc SignalStatAnDataFile {p sessionDir var} {
    global $var
    upvar #0 $var fileRelPath
    StatAnDataFile $p $sessionDir $fileRelPath
}


# Select file for filter and store new value to var global variable.
proc SignalSelectFiltFile {p sessionDir var} {
    global $var
    upvar #0 $var fileRelPath
    #puts "sessionDir=$sessionDir"
    #puts "fileRelPath=$fileRelPath"
    set fileName [SessionAbsPath $sessionDir $fileRelPath]
    set trfuncfiletypes {
	{"Линейные звенья" {.tf}}
	{"Произвольные функции" {.cof}}
	{"Все файлы" *}
    }
    set fileName [fileSelectionBox $p open $fileName $trfuncfiletypes]
    if {$fileName != {}} {
	set fileRelPath [SessionRelPath $sessionDir $fileName]
    }
}


# Select file with data series and store new value to var global
# variable.
proc SignalSelectDataFile {p sessionDir var} {
    global $var
    upvar #0 $var fileRelPath
    set fileName [SessionAbsPath $sessionDir $fileRelPath]
    set datafiletypes {
	{"Файлы данных" {.dat}}
	{"Все файлы" *}
    }
    set fileName [fileSelectionBox $p open $fileName $datafiletypes]
    if {$fileName != {}} {
	set fileRelPath [SessionRelPath $sessionDir $fileName]
    }
}


# Create dialog window with signal settings.
# - p - parent widget;
# - sessionDir - current session directory (for relative paths mostly);
# - signal - type of the signal ("refer" or "noise");
# - arref - name of global array variable where to store settings;
# - sigsrc - index of signal data source parameter ("stream" or "file");
# - datafile - index of arbitrary data series file;
# - filtfile - index of series generated by filter over white noise;
# - filtlen - index of series length to generate.
# Return: 1 if some changes took place and 0 otherwise.
proc SignalWindow {p sessionDir signal arref sigsrc datafile filtfile filtlen} {
    upvar $arref arvar

    set w $p.$signal
    catch {destroy $w}
    toplevel $w

    global var_sigsrc var_datafile_$signal var_filtfile_$signal var_filtlen
    set var_sigsrc $arvar($sigsrc)
    set var_filtlen $arvar($filtlen)
    set var_filtfile_$signal [SessionRelPath $sessionDir $arvar($filtfile)]
    set var_datafile_$signal [SessionRelPath $sessionDir $arvar($datafile)]

    global $w.applyChanges
    set $w.applyChanges 0

    set f $w.p
    frame $f

    switch -exact $signal {
	refer {
	    wm title $w "Reference settings"
	    label $f.title -text "Сигнал уставки"
	}
	noise {
	    wm title $w "Noise settings"
	    label $f.title -text "Сигнал помехи"
	}
	default {
	    wm title $w "Signal settings"
	    label $f.title -text "Сигнал"
	}
    }

    grid $f.title
    grid $f.title -row 0 -column 0 -columnspan 4

    radiobutton $f.data_rb -text "Произвольный ряд в файле" \
	-variable var_sigsrc -value "file"
    label $f.data_fl -text "Имя файла:"
    entry $f.data_fe -width 30 -textvariable var_datafile_$signal
    button $f.data_fsel -text "Выбор..." \
	-command "SignalSelectDataFile $w \"$sessionDir\" var_datafile_$signal"

    set m $f.data_foper.m
    menubutton $f.data_foper -text "Операции" \
	-direction below -menu $m -relief raised
    menu $m -tearoff 0
    $m add command -label "Показать..." \
	-command "SignalViewDataFile $w \"$sessionDir\" var_datafile_$signal"
    $m add command -label "Создать..." \
	-command "SignalGenWindow $w \"$sessionDir\" var_datafile_$signal"
    $m add command -label "Статистика..." \
	-command "SignalStatAnDataFile $w \"$sessionDir\" var_datafile_$signal"

#SignalViewDataFile $w $sessionDir var_datafile_$signal

    grid $f.data_rb
    grid $f.data_fl $f.data_fe $f.data_fsel $f.data_foper -sticky news
    grid $f.data_rb -sticky nw
    grid $f.data_fl -sticky e

    radiobutton $f.filt_rb -text "Фильтр на белый шум" \
	-variable var_sigsrc -value "stream"
    label $f.filt_fl -text "Имя файла:" -anchor w
    entry $f.filt_fe -width 30 -textvariable var_filtfile_$signal
    button $f.filt_fsel -text "Выбор..." \
	-command "SignalSelectFiltFile $w \"$sessionDir\" var_filtfile_$signal"

    set m $f.filt_fedit.m
    menubutton $f.filt_fedit -text "Изменить..." -underline 0 \
	-direction below -menu $m -relief raised

    menu $m -tearoff 0
    $m add command -label "Тип звена" \
	-command "TrFuncEdit $w \"$sessionDir\" [set var_filtfile_$signal] @var_filtfile_$signal true"
    $m add command -label "Параметры" \
	-command "TrFuncEdit $w \"$sessionDir\" [set var_filtfile_$signal] @var_filtfile_$signal"
    $m add command -label "Как текст" \
	-command "TrFuncEdit $w \"$sessionDir\" [set var_filtfile_$signal] @var_filtfile_$signal false true"

    label $f.filt_ll -text "Длина ряда:" -anchor w
    entry $f.filt_le -width 8 -textvariable var_filtlen

    grid $f.filt_rb
    grid $f.filt_fl $f.filt_fe $f.filt_fsel $f.filt_fedit
    grid $f.filt_ll $f.filt_le
    grid $f.filt_rb -sticky nw
    grid $f.filt_fl -sticky e
    grid $f.filt_ll -sticky e
    grid $f.filt_le -sticky w

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
	puts "win_signal: apply changes"
	if {$var_sigsrc != $arvar($sigsrc)} {
	    set arvar($sigsrc) $var_sigsrc
	    set changed 1
	}
	if {[set var_datafile_$signal] != $arvar($datafile)} {
	    set arvar($datafile) [SessionRelPath $sessionDir [set var_datafile_$signal]]
	    set changed 1
	}
	if {[set var_filtfile_$signal] != $arvar($filtfile)} {
	    set arvar($filtfile) [SessionRelPath $sessionDir [set var_filtfile_$signal]]
	    set changed 1
	}
	if {$var_filtlen != $arvar($filtlen)} {
	    set arvar($filtlen) $var_filtlen
	    set changed 1
	}
    }
    if {$changed == 0} {
	puts "win_signal: no changes"
    }
    return $changed
}
