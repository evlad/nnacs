package provide win_plant 1.0

package require Tk
package require universal
package require win_textedit
package require win_trfunc
package require draw_nn

proc PlantWindowOk {w entry var} {
    PlantWindowApply $w $entry $var
    destroy $w
}

proc PlantWindowApply {w entry var} {
    upvar #0 $var fileName
    set fileName [$entry get]
    puts "PlantWindowApply: '$fileName'"

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

proc PlantWindowEdit {w title var} {
    upvar #0 $var fileName
    TextEditWindow $w "$title" $fileName
}

proc PlantWindowModified {w entry} {
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

# Select file for plant and store new value to var global variable.
proc PlantSelectTrFile {p sessionDir var} {
    global $var
    upvar #0 $var fileRelPath
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


# Create dialog window with controller settings.
# - p - parent widget;
# - sessionDir - current session directory (for relative paths mostly);
# - arref - name of global array variable where to store settings;
# - plantfile - index of traditional (not nn) controller file;
# Return: 1 if some changes took place and 0 otherwise.
proc PlantWindow {p sessionDir arref plantfile} {
    upvar $arref arvar

    set w $p.plant
    catch {destroy $w}
    toplevel $w
    wm title $w "Plant settings"

    global var_plantfile
    set var_plantfile [SessionRelPath $sessionDir $arvar($plantfile)]

    global $w.applyChanges
    set $w.applyChanges 0

    set f $w.p
    frame $f

    label $f.title -text "Объект управления"
    grid $f.title
    grid $f.title -row 0 -column 0 -columnspan 4

    label $f.lin_fl -text "Имя файла:" -anchor w
    entry $f.lin_fe -width 30 -textvariable var_plantfile
    button $f.lin_fsel -text "Выбор..." \
	-command "PlantSelectTrFile $w \"$sessionDir\" var_plantfile"

    set m $f.lin_fedit.m
    menubutton $f.lin_fedit -text "Изменить..."  -underline 0 \
	-direction below -menu $m -relief raised
    menu $m -tearoff 0
    $m add command -label "Тип звена" \
	-command "TrFuncEdit $w \"$sessionDir\" \"$var_plantfile\" $var_plantfile true"
    $m add command -label "Параметры" \
	-command "TrFuncEdit $w \"$sessionDir\" \"$var_plantfile\" $var_plantfile"
    $m add command -label "Как текст" \
	-command "TrFuncEdit $w \"$sessionDir\" \"$var_plantfile\" $var_plantfile false true"


    grid $f.lin_fl $f.lin_fe $f.lin_fsel $f.lin_fedit
    grid $f.lin_fl -sticky e

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
	if {$var_plantfile != $arvar($plantfile)} {
	    set arvar($plantfile) $var_plantfile
	    set changed 1
	}
    }
    return $changed
}
