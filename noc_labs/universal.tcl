package provide universal 1.0

package require Tk

# unlist listValue outVar0 outVar1 ...
# Implies [lindex $listValue 0] -> outVar0
#         [lindex $listValue 1] -> outVar1
# and so on.
# Returns number of assigned variables
proc unlist {listValue args} {
    set i 0
    foreach param $args {
	if { $i >= [llength $listValue] } { return $i }
	#puts "-> $param"
	upvar 1 $param outVar
	set outVar [lindex $listValue $i]
	incr i
    }
    return $i
}

# draw hint
proc hint {p text} {
    #set w $p.hintWindow
    #toplevel $w
    bind $p <Enter> "puts \"$text\""
    # Nothing
    #bind $p <Leave> "puts \"$text\""
}

# focusAndFlash --
# Error handler for entry widgets that forces the focus onto the
# widget and makes the widget flash by exchanging the foreground and
# background colours at intervals of 200ms (i.e. at approximately
# 2.5Hz).
#
# Arguments:
# W -		Name of entry widget to flash
# fg -		Initial foreground colour
# bg -		Initial background colour
# count -	Counter to control the number of times flashed
proc focusAndFlash {W fg bg {count 9}} {
    focus -force $W
    if {$count<1} {
	$W configure -foreground $fg -background $bg
    } else {
	if {$count%2} {
	    $W configure -foreground $bg -background $fg
	} else {
	    $W configure -foreground $fg -background $bg
	}
	after 200 [list focusAndFlash $W $fg $bg [expr {$count-1}]]
    }
}

# Display file selection box for listed types of files
# - w - parent widget;
# - operation - open or new;
# - filepath - predefined file path;
# - types - { {label extension} ... } (OPTIONAL).
# Return: selected file path
proc fileSelectionBox {w operation filepath {types {{"Все файлы" *}}}} {
    if {[file isdirectory $filepath]} {
	set initdir $filepath
	set initfile ""
	set initext ""
    } else {
	set initdir [file dirname $filepath]
	set initfile [file tail $filepath]
	set initext [file extension $filepath]
    }
    if { $initext == "" } {
	# Let's use the first extension among
	set initext [lindex $types 0 1]
	if {$initext == "*"} {
	    # No extension
	    set initext ""
	}
    } else {
	foreach {name ext} $types {
	    set i [lsearch -exact $ext $initext]
	    if { $i >= 0 } {
		set initext [lindex $ext $i]
		break
	    }
	}
    }

    if {$operation == "open"} {
	set filepath [tk_getOpenFile -filetypes $types -parent $w \
			  -initialdir $initdir -initialfile $initfile \
			  -defaultextension $initext ]
    } else {
	set filepath [tk_getSaveFile -filetypes $types -parent $w \
			  -initialdir $initdir -initialfile $initfile \
			  -defaultextension $initext ]
    }
    return $filepath
}

# Return directory for temporal files.
proc temporalDirectory {} {
    # TODO: make OS dependent!
    if {[file isdirectory /tmp]} {
	return /tmp
    } elseif {[file isdirectory {C:\WINDOWS\TEMP}]} {
	return {C:\WINDOWS\TEMP}
    }
    # Return current directory
    return .
}

# List for temporal files
set tempFileList {}

# Return temporal file with given suffix at special location.
proc temporalFileName {suffix} {
    global tempFileList
    set tempFileCounter [llength $tempFileList]
    incr tempFileCounter
    set fileName [file join [temporalDirectory] nnacs_[pid]_${tempFileCounter}$suffix]
    lappend tempFileList $fileName
    return $fileName
}


# Remove all temporal file names at the end of program.
proc removeTemporalFiles {} {
    global tempFileList
    foreach fileName $tempFileList {
	file delete -force $fileName
    }
    set tempFileList {}
}

# Taken from Welch book
proc Scroll_Set {scrollbar geoCmd offset size} {
    if {$offset !=0.0 || $size != 1.0} {
	eval $geoCmd ;# Make sure it is visible
    }
    $scrollbar set $offset $size
}

# Taken from Welch book
proc Scrolled_Listbox {f args} {
    frame $f
    listbox $f.list \
	-xscrollcommand [list Scroll_Set $f.xscroll \
			     [list grid $f.xscroll -row 1 -column 0 \
				  -sticky we]] \
	-yscrollcommand [list Scroll_Set $f.yscroll \
			     [list grid $f.yscroll -row 0 -column 1 \
				  -sticky ns]]
    eval { $f.list configure } $args
    scrollbar $f.xscroll -orient horizontal \
	-command [list $f.list xview]
    scrollbar $f.yscroll -orient vertical \
	-command [list $f.list yview]
    grid $f.list -sticky news
    grid rowconfigure $f 0 -weight 1
    grid columnconfigure $f 0 -weight 1
    return $f.list
}


proc StatAnDataFile {p sessionDir fileRelPath} {
    set filePath [SessionAbsPath $sessionDir $fileRelPath]
    if {![file isfile $filePath]} {
	error "File $filePath does not exist"
	return
    }
    set cwd [pwd]
    puts "Run StatAn"
    catch {cd [SessionDir $sessionDir]} errCode1
    if {$errCode1 != ""} {
	error $errCode1
	return
    }

    if {[catch {exec [file join [SystemDir] bin StatAn] $filePath} \
	     output]} {
	error $output
	return
    }
    cd $cwd

    # Counter for unique names
    global statan
    if {![info exists statan]} {
	set statan 0
    } else {
	incr statan
    }
    set w $p.statan$statan
    catch {destroy $w}
    toplevel $w
    wm title $w "Statistics: $fileRelPath"
    set f $w
    label $f.l -text "Статистика по файлу:"
    pack $f.l

    button $f.b -text "Закрыть" -command "destroy $w"
    pack $f.b -side bottom

    set font [option get $w fontMono ""]
    text $f.t -state normal -background white -width 80 -height 8 \
	-font $font
    $f.t insert 0.end $output
    $f.t configure -state disabled
    pack $f.t -side top -fill both -expand true
}


# Display list of stack
proc backtrace {{file stdout}} {
    puts $file "backtrace:"
    for {set x [expr [info level]-1]} {$x > 0} {incr x -1} {
	puts $file "$x: [info level $x]"
    }
}


# Return $tagarray($tag) if there is a $tag in $tagarray or $defval
# otherwise.  tagarray is an arrayof $taglist.
proc setdef {taglist tag defval} {
    array set arr $taglist
    if {[info exists arr($tag)]} {
	return $arr($tag)
    } else {
	return $defval
    }
}


# Auxiliary procedure for lequal
proc lequalK {a b} {
    return $a
}

# Deep compare of two lists.
proc lequal {l1 l2} {
    if {[llength $l1] != [llength $l2]} {
        return 0
    }
    set l2 [lsort $l2]
    foreach elem $l1 {
        set idx [lsearch -exact -sorted $l2 $elem]
        if {$idx == -1} {
            return 0
        } else {
            set l2 [lreplace [lequalK $l2 [unset l2]] $idx $idx]
        }
    }
    return [expr {[llength $l2] == 0}]
}

# Deep compare of two arrays.
proc array_equal {a1_ a2_} {
    upvar a1_ a1 a2_ a2
    # Not equal names
    if {! [lequal [lsort [array names a1]] [lsort [array names a2]]] } {
	return 0
    }
    # Check elements for equality
    foreach n [array names a1] {
	if {![lequal $a1($n) $a2($n)]} {
	    return 0
	}
    }
    return 1
}


# Example of parameters dialog:
#global array_params
#array set array_params {par_a 12.3 par_c "Test value" par_b 32000 }
#set TestParameters {
#    -0 "Window title"
#    -1 "Первый подзаголовок"
#    par_a "Первый параметр"
#    par_b "Второй параметр"
#    -2 "Второй подзаголовок"
#    par_c "Третий параметр"
#}
#ParametersWindow .main.subwin array_params $TestParameters $TestParSpecifications
#...
#puts [array get array_params]

# Create dialog window with named parameters.
# - p - parent widget;
# - arref - name of global array variable where to store settings
#           under names mentioned in parlist;
# - parlist - list of parameters and their labels;
# - parspec - list of spcification for some parameters;
#    Examples for parspec:
#     {parname1 {oneof Value1 Value2 Value3}}
proc ParametersWindow {p arref parlist {parspec {}}} {
    upvar $arref arvar

    set w $p.nntpar
    catch {destroy $w}
    toplevel $w
    wm title $w "Parameters"

    global $w.parvalue
    array set pardescr $parlist
    array set parspecar $parspec

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
	    -0 { # Window title
		wm title $w $pardescr($par)
	    }
	    -* { # Title
		label $f.title$par -text $pardescr($par)
		grid $f.title$par
		grid $f.title$par -row $row -column 0 -columnspan 2
	    }
	    default {
		if {![info exists parspecar($par)]} {
		    label $f.label_$par -text $pardescr($par) -anchor w
		    entry $f.entry_$par -textvariable $w.parvalue($par) -width 10
		    grid $f.label_$par $f.entry_$par -sticky nw
		} else {
		    switch -glob -- [lindex $parspecar($par) 0] {
			oneof {
			    label $f.label_$par -text $pardescr($par) -anchor w
			    set optname [tk_optionMenu $f.menu_$par $w.parvalue($par) XX]
			    $optname delete 0
			    set i 0
			    foreach item [lrange $parspecar($par) 1 end] {
				$optname insert $i radiobutton -label $item -command \
				    "set $w.parvalue($par) $item"
				incr i
			    }
			    grid $f.label_$par $f.menu_$par -sticky nw
			}
			default {
			    label $f.label_$par -text "$pardescr($par) ???" -anchor w
			    entry $f.entry_$par -textvariable $w.parvalue($par) -width 10
			    grid $f.label_$par $f.entry_$par -sticky nw
			}
		    }
		}
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
	#puts "ParametersWindow: apply changes"

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
	#puts "ParametersWindow: no changes"
    }
    return $changed
}


# Return string as a decimal representation in subscript unicode characters.
proc subscriptString {index} {
    #set decnum [format "%u" $index]
    set subscriptMap {
	0 \u2080 1 \u2081 2 \u2082 3 \u2083 4 \u2084
	5 \u2085 6 \u2086 7 \u2087 8 \u2088 9 \u2089
    } 
    return [string map $subscriptMap $index]
}
