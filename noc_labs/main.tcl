package require Tk

set w ""
wm title . "Main menu of laboratory works"
wm iconname . "Labs main menu"
wm geometry . +100+100

package require files_loc
package require universal
package require win_dcsloop
package require win_dcontrp
package require win_dplantid
package require win_dcontrf
package require win_dcsloop_cusum
package require win_dtf
#package require win_series

set ScriptsDir [file join [SystemDir] scripts]
#set ScriptsDir {Z:\Home\nnacs-1.3b_win\noc_labs}
option readfile [file join $ScriptsDir nnacs.ad]

set menuContent {
    "dcsloop" "Моделирование системы автоматического управления"
    "dcontrp" "Обучение нейросетевого регулятора вне контура"
    "dplantid" "Обучение нейросетевой модели объекта управления"
    "dcontrf" "Обучение нейросетевого регулятора в контуре"
    "dcsloop" "Обнаружение разладки объекта в контуре"
    "dtf"     "Моделирование объекта в дискретном времени"
}

proc AboutWindow {p} {
    set w $p.text
    catch {destroy $w}
    toplevel $w
    wm title $w "About NNACS"
    wm iconname $w "About NNACS"
    wm geometry $w +300+300

    ## See Code / Dismiss buttons
    button $w.dismiss -text "OK" -command "destroy $w"
    pack $w.dismiss -side bottom

    text $w.text -yscrollcommand [list $w.scroll set] -setgrid 1 \
	-width 70  -height 13 -undo 1 -autosep 1 -wrap word
    scrollbar $w.scroll -command [list $w.text yview]
    pack $w.scroll -side right -fill y
    pack $w.text -expand yes -fill both

    focus $w.text
    $w.text tag configure center -justify center
    set family Courier
    $w.text tag configure moreinfo  -spacing1 3p -font "$family 10"

    $w.text insert 0.0 \
	{Учебно-исследовательский программый пакет "Нейросетевые системы автоматического управления"
NNACS - Neural network applications for control systems
} center
    global tcl_platform
    set platform [string totitle $tcl_platform(platform)]
    $w.text insert end "Версия: 1.8.3 для $platform\n" center
    $w.text insert end "Дата: 11 января 2024 года\n" center
    $w.text insert end {
Национальный исследовательский университет "МЭИ"
Институт информационных и вычислительных технологий (ИВТИ, ранее АВТИ)
Кафедра Управления и интеллектуальных технологий
} center

    $w.text insert end "\n\uf8e9 Елисеев Владимир Леонидович, 2001-2024\nEmail: YeliseevVL@mpei.ac.ru\n" center

    if {$tcl_platform(platform) == "windows"} {
        $w.text insert end {
В версии пакета на платформе Windows используется фунция vsscanf:

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, under the terms in LICENSE.TXT.

Kaya Kupferschmidt  (k.kupferschmidt@dimajix.de)} moreinfo
    }

    $w.text configure -state disabled
}

proc MenuProg1 {w label} {
    # Create or use session directory and remember it
    set prog [lindex [split "$label" \n] 0]
    set title [lindex [split "$label" \n] 1]
    set sessionDir [NewSession $w "dcsloop.par" "$title"]
    puts "Program: $prog,  session directory: [SessionDir $sessionDir]"
    if {$sessionDir != {}} {
	dcsloopCreateWindow $w "$title" "$sessionDir"
    }
}

proc MenuProg2 {w label} {
    # Create or use session directory and remember it
    set prog [lindex [split "$label" \n] 0]
    set title [lindex [split "$label" \n] 1]
    set sessionDir [NewSession $w "dcontrp.par" "$title"]
    puts "Program: $prog,  session directory: [SessionDir $sessionDir]"
    if {$sessionDir != {}} {
	dcontrpCreateWindow $w "$title" "$sessionDir"
    }
}

proc MenuProg3 {w label} {
    # Create or use session directory and remember it
    set prog [lindex [split "$label" \n] 0]
    set title [lindex [split "$label" \n] 1]
    set sessionDir [NewSession $w "dplantid.par" "$title"]
    puts "Program: $prog,  session directory: [SessionDir $sessionDir]"
    if {$sessionDir != {}} {
	dplantidCreateWindow $w "$title" "$sessionDir"
    }
}

proc MenuProg4 {w label} {
    # Create or use session directory and remember it
    set prog [lindex [split "$label" \n] 0]
    set title [lindex [split "$label" \n] 1]
    set sessionDir [NewSession $w "dcontrf.par" "$title"]
    puts "Program: $prog,  session directory: [SessionDir $sessionDir]"
    if {$sessionDir != {}} {
	dcontrfCreateWindow $w "$title" "$sessionDir"
    }
}

proc MenuProg5 {w label} {
    # Create or use session directory and remember it
    set prog [lindex [split "$label" \n] 0]
    set title [lindex [split "$label" \n] 1]
    set sessionDir [NewSession $w "dcsloop_cusum.par" "$title"]
    puts "Program: $prog,  session directory: [SessionDir $sessionDir]"
    if {$sessionDir != {}} {
	dcsloopCusumCreateWindow $w "$title" "$sessionDir"
    }
}

proc MenuProg6 {w label} {
    # Create or use session directory and remember it
    set prog [lindex [split "$label" \n] 0]
    set title [lindex [split "$label" \n] 1]
    set sessionDir [NewSession $w "dtf.par" "$title"]
    puts "Program: $prog,  session directory: [SessionDir $sessionDir]"
    if {$sessionDir != {}} {
	dtfCreateWindow $w "$title" "$sessionDir"
    }
}


proc ShowConsole {} {
    set workdir [UserBaseDir]
    global curUserDir tcl_platform
    if {[info exists curUserDir]} {
	if {[file isdirectory $curUserDir]} {
	    set workdir $curUserDir
	}
    }

    if {$tcl_platform(platform) == "windows"} {
	console show
    } elseif {$tcl_platform(os) == "Linux"} {
	# Run terminal
	foreach termprog xterm rxvt x-terminal-emulator {
	    if { [catch {exec $termprog &}] } {
		break
	    }
	    puts "Failed to run $termprog"
	}
	puts "Done"
    }
}


pack [button $w.user_button \
	  -text "Перед началом\nВыбор/создание нового пользователя" \
	  -command "NewUser \"$w\""] -fill x -side top -expand yes -pady 2

#set curUserDir "/home/user/labworks/EliseevVL"

set i 0
foreach {label title} $menuContent {
    incr i
    #set text "$label\n$title"
    set text "$title"
    pack [button $w.lab${i}_button -text "$text" \
	      -command "CheckGoodEnv \"$w\" ; MenuProg$i \"$w\" \"$text\""] -fill x -side top -expand yes -pady 2
}

bind . <C> {ShowConsole}
button $w.term_button -text "Консоль" -command ShowConsole
pack $w.term_button -fill x -side top -expand yes -pady 2

button $w.info_button -text "О программе" -command "AboutWindow \"$w\""
pack $w.info_button -fill x -side top -expand yes -pady 2

button $w.quit_button -text "Выход" -command { removeTemporalFiles ; exit }
pack $w.quit_button -side top -expand yes -pady 2

# End of file
