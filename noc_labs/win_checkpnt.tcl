proc CheckPntFileDialog {w ent operation filepath} {
    #   Type names		Extension(s)	Mac File Type(s)
    #
    #---------------------------------------------------------
    set types {
	{"Space separated value data"  {.dat}	}
	{"All files"			*	}
    }
    set initdir [file dirname $filepath]
    set initfile [file tail $filepath]
    set initext [file extension $filepath]
    if { $initext == "" } {
	set initext .dat
    } else {
	foreach {name ext} $types {
	    set i [lsearch -exact $ext $initext]
	    if { $i >= 0 } {
		set initext [lindex $ext $i]
		break
	    }
	}
    }
    #puts "Extension: $initext"

    if {$operation == "open"} {
	set filepath [tk_getOpenFile -filetypes $types -parent $w \
			  -initialdir "$initdir" -initialfile "$initfile" \
			  -defaultextension $initext ]
    } else {
	set filepath [tk_getSaveFile -filetypes $types -parent $w \
			  -initialdir "$initdir" -initialfile "$initfile" \
			  -defaultextension $initext ]
    }
    if {[string compare $filepath ""]} {
	$ent delete 0 end
	$ent insert 0 $filepath
	$ent xview end
    }
}

proc CheckPntWindowOk {w entry var} {
    CheckPntWindowApply $w $entry $var
    destroy $w
}

proc CheckPntWindowApply {w entry var} {
    upvar #0 $var fileName
    set fileName [$entry get]
    puts "CheckPntWindowApply: '$fileName'"

    # Restore normal attributes
    set normalBg [$w.buttons.cancel cget -bg]
    set normalFg [$w.buttons.cancel cget -fg]
    set activeBg [$w.buttons.cancel cget -activebackground]
    set activeFg [$w.buttons.cancel cget -activeforeground]
    $w.buttons.ok configure -bg $normalBg -fg $normalFg \
	-activebackground $activeBg -activeforeground $activeFg
    $w.buttons.apply configure -bg $normalBg -fg $normalFg \
	-activebackground $activeBg -activeforeground $activeFg
}

proc CheckPntWindowEdit {w title var} {
    upvar #0 $var fileName
    TextEditWindow $w "$title" $fileName
}

proc CheckPntWindowModified {w entry} {
    #puts "Modified: $w $entry"
    # Set attributes of modified text contents
    set modifiedFg white
    set modifiedBg red
    $w.buttons.ok configure -bg $modifiedBg -fg $modifiedFg \
	-activebackground $modifiedBg -activeforeground $modifiedFg
    $w.buttons.apply configure -bg $modifiedBg -fg $modifiedFg \
	-activebackground $modifiedBg -activeforeground $modifiedFg
    return 1
}

# p - parent widget
# title - description of the given transfer function
# var - name of variable where to store filename
proc CheckPntWindow {p title var} {
    upvar #0 $var globalFileName
    set fileName $globalFileName
    set w $p.checkpnt
    catch {destroy $w}
    toplevel $w
    wm title $w $title

    frame $w.file
    label $w.file.label -text "File name:" -anchor w
    entry $w.file.entry -width 40 -textvariable fileName
    $w.file.entry insert 0 $fileName
    button $w.file.button -text "Browse..." \
	-command "CheckPntFileDialog $w $w.file.entry open $fileName"
    pack $w.file.label $w.file.entry -side left
    pack $w.file.button -side left -pady 5 -padx 10
    pack $w.file -side top

    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.ok -text "OK" \
	-command "CheckPntWindowOk $w $w.file.entry globalFileName"
    button $w.buttons.apply -text "Apply" \
	-command "CheckPntWindowApply $w $w.file.entry globalFileName"
    button $w.buttons.edit -text "Edit..." \
	-command "TextEditWindow $w TITLE \$fileName"
    button $w.buttons.view -text "View..."
    button $w.buttons.cancel -text "Cancel" -command "destroy $w"
    pack $w.buttons.ok $w.buttons.apply $w.buttons.edit $w.buttons.view \
	$w.buttons.cancel -side left -expand 1

    $w.file.entry configure -validate key -vcmd "CheckPntWindowModified $w %W"
    focus $w.file.entry
}


proc CheckPntTest {} {
#	-command "CheckPntWindowEdit $w $title fileName" 

#font create myDefaultFont -family Freesans -size 11
#option add *font myDefaultFont
option readfile nnacs.ad

source win_textedit.tcl

#font configure default -family Freesans -size 11

#set myvar "../d.cf"
set myvar "r.dat"
#puts $myvar
CheckPntWindow "" "Reference" myvar
#puts $myvar
}
