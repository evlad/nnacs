package provide files_loc 1.0

package require universal

# Strip absolute path to the relative one if possible.
# Examples:
# basedir="/home/evlad/noc_labs/1"
# abspath="/home/evlad/noc_labs/2/u.dat"
# => relpath="../2/u.dat"
# basedir="/home/evlad/noc_labs/1"
# abspath="u.dat"
# => relpath="u.dat"
proc RelPath {basedir abspath} {
    # Do not normalize implicitly current directory location
    if {[file split $abspath] == $abspath} {
	return $abspath
    }
    set cwd [pwd]
    cd $basedir
    set bdparts [file split [file normalize $basedir]]
    set apparts [file split [file normalize $abspath]]
    set rpparts {}
    set i 0
    # Skip common head
    while {[lindex $apparts $i] == [lindex $bdparts $i]} {
	incr i
    }
    # All rest base dir parts should be replaced by ".."
    foreach bd [lrange $bdparts $i end] {
	lappend rpparts ".."
    }
    # Then all absolute path parts should be appended
    foreach ap [lrange $apparts $i end] {
	lappend rpparts $ap
    }
    cd $cwd
    return [eval file join $rpparts]
}


# Relative path to the given session dir.
proc SessionRelPath {sessionDir path} {
    switch -glob [file pathtype $path] {
	absolute {
	    return [RelPath [SessionDir $sessionDir] $path]
	}
	*relative {
	    return $path
	}
    }
}


# Return the shortest absolute path.
proc AbsPath {basedir relpath} {
    return [file join $basedir [RelPath $basedir $relpath]]
}


# Absolute path to the given session dir.
proc SessionAbsPath {sessionDir path} {
    switch -glob [file pathtype $path] {
	absolute {
	    return $path
	}
	*relative {
	    return [AbsPath [SessionDir $sessionDir] $path]
	}
    }
}


# Create or select new user directory
proc NewUser {w {user ""}} {
    set basedir [UserBaseDir]
    puts "Labworks base directory: $basedir"
    global curUserDir
    if {$user != ""} {
	set curUserDir [file join $basedir $user]
    } else {
	set curUserDir [ChooseUserLabWorkDir $w $basedir]
    }
    if {$curUserDir eq "" || [file isfile $curUserDir]} {
	return ""
    }
    set curUserDir [file join $basedir $curUserDir]
    puts "User sessions directory: $curUserDir"
    if {![file exists $curUserDir]} {
	file mkdir $curUserDir
    }

    # Append user name to the title of main menu
    set title [wm title .]
    set semicol [string last : $title]
    set userName [file tail $curUserDir]
    if {-1 == $semicol} {
	wm title . "$title: $userName"
    } else {
	wm title . "[string range $title 0 $semicol] $userName"
    }

    return $curUserDir
}

# Check environment conditions before starting lab works
proc CheckGoodEnv {w} {
    global curUserDir
    if {![info exists curUserDir] || ![file isdirectory $curUserDir]} {
	NewUser "$w"
	#error "Current user is not defined"
    }
}

# Return base directory of user data
proc UserBaseDir {} {
    global env
    if {![info exists env(NNACSUSERDIR)]} {
	# Not defined special place -> let's use the default one
	set dir [file join $env(HOME) labworks]
	puts "User directory: $dir (use NNACSUSERDIR)"
    } else {
	set dir $env(NNACSUSERDIR)
	puts "User directory: $dir (see NNACSUSERDIR)"
    }
    file mkdir $dir
    return $dir
}

# Return system directory
proc SystemDir {} {
    global SystemDirPath
    set dir $SystemDirPath
    return $dir
}

# Return directory path to the given session directory
proc SessionDir {s} {
    global curUserDir
    if {$curUserDir eq "" || ![file isdirectory $curUserDir]} {
	error "Current user is not defined"
    }
    set sessionDir [file join $curUserDir $s]
    if {![file exists $sessionDir]} {
	file mkdir $sessionDir
    } elseif {![file isdirectory $sessionDir]} {
	error "Session is not a directory"
    }
    return $sessionDir
}

# Return directory path to the template directory
proc TemplateDir {} {
    set td [file join [SystemDir] templates]
    return $td
}

# Scan labwork directory $basedir and fill list $l with item.
proc ScanUserLabWorkDirs {l basedir} {
    catch {
	$l delete 0 end
    }
    foreach d [lsort [glob -nocomplain -types d -directory $basedir -tails -- *]] {
	$l insert end $d
    }
    catch {
	$l selection clear 0 end
	$l selection set 0
	$l yview moveto 0.0
    }
}

# Create new directory $newdir in $basedir and set it as current item
# in list $l.
proc CreateUserLabDir {l basedir newdir} {
    if { [catch {file mkdir [file join $basedir $newdir]} msg] } {
	message $msg
	return
    }
    ScanUserLabWorkDirs $l $basedir
    set i [lsearch [$l get 0 end] $newdir]
    $l selection clear 0 end
    $l selection set $i
    $l see $i

}

# Select currect selection as a directory for user.  List to select is
# given by $l. $w is a dialog window.
proc SelectUserLabDir {w l} {
    global $w.seldir
    if {{} != [$l curselection]} {
	set $w.seldir [$l get [$l curselection]]
	destroy $w
    }
}


# Remove currently selected item in listbox $l in directory $basedir.
proc DeleteUserLabDir {l basedir} {
    set dir [$l get [$l curselection]]
    set subitems [llength [glob -nocomplain -directory [file join $basedir $dir] -- *]]
    if {$subitems > 0} {
	set ret [tk_messageBox -type yesno -icon question -default no -message "Вы хотите удалить ВСЮ работу пользователя $dir ($subitems сеансов)?"]
    } else {
	# Empty directory can be removed without question
	set ret "yes"
    }
    if {$ret == "yes"} {
	file delete -force [file join $basedir $dir]
	ScanUserLabWorkDirs $l $basedir
    }
}

# Dialog window to manage (select/create/delete) user work directories
# which resides in $basedir.
proc ChooseUserLabWorkDir {p basedir} {
    set w $p.userdir

    # Don't create the window twice
    if {[winfo exists $w]} return

    toplevel $w
    wm title $w "Choose user work directory"
    wm iconname $w "Choose user dir"

    set fl $w.flist
    set l [Scrolled_Listbox $fl -width 20 -height 10 \
	       -selectmode single]

    global $w.seldir
    set $w.seldir {}

    global $w.newdir
    set $w.newdir "Student"

    puts "Newdir: [set $w.newdir]"

    set fc $w.fctrl
    frame $fc
    pack $fc -side right -fill y
    button $fc.ok -text "Выбрать" -command "SelectUserLabDir $w $l"
    button $fc.create -text "Создать" -command "CreateUserLabDir $l \"$basedir\" \[set $w.newdir\]"
    entry $fc.newdir -textvariable $w.newdir
    button $fc.remove -text "Удалить" -command "DeleteUserLabDir $l \"$basedir\""
    button $fc.cancel -text "Отмена" -command "set $w.seldir {}; destroy $w"

    pack $fc.cancel -side bottom -fill x
    foreach b {ok create newdir remove} {
	pack $fc.$b -side top -fill x
    }

    # Select and show the first item
    ScanUserLabWorkDirs $l "$basedir"

    if {[$l curselection] != {} } {
	set $w.seldir [$l get [$l curselection]]
    }

    pack $fl -expand true -fill both

    focus $w
    grab $w
    tkwait window $w

    #puts "Done: [set $w.seldir]"
    return [set $w.seldir]
}


proc NewSessionOk {w l} {
    global curSessionDir
    # Here the first the digits (or the first word) must be taken
    set curSessionDir [lindex [split [$l get [$l curselection]]] 0]
    if {$curSessionDir != {}} {
	destroy $w
    }
}

proc NewSessionCopy {w l} {
    global curSessionDir
    # Here the first the digits (or the first word) must be taken
    set curSessionDir "NEW [lindex [split [$l get [$l curselection]]] 0]"
    if {$curSessionDir != {}} {
	destroy $w
    }
}

# Create or select session directory of given type (markfile should present)
proc NewSession {p markfile {title ""}} {
    global curUserDir curSessionDir

    if {[catch {glob -tails -directory $curUserDir -types d \
		    "\[0-9\]\[0-9\]\[0-9\]"} sessionList]} {
	set curSessionDir NEW
	set lastNum "000"
    } else {
	set lastNum [lindex [lsort $sessionList] end]

	# Let's allow user to select session
	set matchedSessionList {}
	foreach sessionDir $sessionList {
	    if {[file exists [file join [SessionDir $sessionDir] $markfile]]} {
		lappend matchedSessionList $sessionDir
		# It's possible to append comments too
	    }
	}
	set sessionList [lsort $matchedSessionList]
	if {$sessionList == {}} {
	    set curSessionDir NEW
	} else {
	    # There are matched sessions
	    set w $p.selectSession

	    # Don't create the window twice
	    if {[winfo exists $w]} return

	    toplevel $w
	    wm title $w "Select session"
	    wm iconname $w "Select"

	    global curSessionDir
	    set curSessionDir {}

	    if {$title != ""} {
		label $w.title -text "$title\nВыберите сеанс:"
	    } else {
		label $w.title -text "Выберите сеанс:"
	    }
	    pack $w.title -side top -fill x

	    frame $w.list
	    set l [Scrolled_Listbox $w.list.sessions -width 30 -height 15 \
		       -selectmode single]
	    foreach it  $sessionList {
		set comment [ParFileFetchParameter \
				 [file join [SessionDir $it] $markfile] \
				 comment]
		$l insert end "$it - $comment"
	    }
	    # Select and show the last item
	    $l selection set end
	    $l yview moveto 1.0

	    pack $w.list.sessions -side left -expand true -fill both
	    pack $w.list -side top -expand true -fill both -pady 2m

	    frame $w.buttons
	    pack $w.buttons -side bottom -fill x -pady 2m
	    button $w.buttons.ok -text "OK" -command "NewSessionOk $w $l"
	    button $w.buttons.new -text "Новый" -command "set curSessionDir NEW ; destroy $w"
	    button $w.buttons.copy -text "Копия" -command "NewSessionCopy $w $l"
	    button $w.buttons.cancel -text "Отмена" -command "destroy $w"
	    pack $w.buttons.ok $w.buttons.new $w.buttons.copy $w.buttons.cancel -side left -expand 1
	    bind $l <Double-1> "NewSessionOk $w $l"

	    tkwait window $w

	    if {$curSessionDir == {}} {
		return {}
	    }
	}
    }
    #puts "curSessionDir=$curSessionDir"
    switch -glob $curSessionDir {
	NEW* {
	    set origSession [lindex [split $curSessionDir] 1]
	    scan $lastNum "%d" lastNum
	    set curSessionDir [format "%03d" [incr lastNum]]
	    if {$origSession != {}} {
		puts "copy $origSession to $curSessionDir"
		catch {file copy [SessionDir $origSession] [file join $curUserDir $curSessionDir]} err
		if {$err != {}} {
		    error "Failed to make copy of session $origNum: $err"
		    return {}
		}
	    }
	}
    }
    #puts "==> curSessionDir=$curSessionDir"
    return $curSessionDir
}
