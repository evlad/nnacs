# State space model implementation
package provide win_ssmodel 1.0

package require Tk
package require files_loc
package require universal

# Format of internal SSM representation:
# { name value ... }
# where name-value pairs are in arbitrary order (adopted to be array).
# Special name="name" is a name of StateSpaceModel (optional).
# Value may be list.

# Table of obliratory parameters their meaning:
# --------+------------------------------------------------
# name    | value
# --------+------------------------------------------------
# states  | number of elements in state vector (N dim)
# outputs | number of outputs (M dim)
# inputs  | number of inputs (K dim)
# a       | matrix A(N,N) - list of elements by rows
# b       | matrix B(N,K) - list of elements by rows
# c       | matrix C(M,N) - list of elements by rows
# d       | matrix D(M,K) - list of elements by rows
# x0      | vector of initial state
# --------+------------------------------------------------

# For example:
# { name "ssm1" states 2 inputs 1 outputs 2
#   a {{1 0.5} {0 2}} b {0 1} c {{1 0} {0 1}} d {0 0} x0 {0 0} }

# Load object from given text taken from file.
# - this - object internal data (array with keys) - result;
# - ftext - file contents to parse (.ssm format).
proc SSModelLoadConfig {thisvar ftext} {
    upvar $thisvar this

    foreach line $ftext {
	# Let's exclude empty items to get fields
	set fields {}
	foreach f [split $line] {
	    if {$f != {}} {
		lappend fields $f
	    }
	}
	# Let's try to find key in the line
	#puts "key_pos: $key_pos"
	foreach {key pos} $key_pos {
	    # Key presents somewhere after ;
	    set cindex [lsearch -exact $fields ";"]
	    if {$cindex > 0} {
		# There is a comment, let's find all keywords
		if {[lsearch -exact [lrange $fields $cindex end] $key] > 0} {
		    set this($key) [lindex $fields $pos]
		    #puts "[lindex $fields $pos] ==> $key=$this($key)"
		} 
	    }
	}
    }
}


# Display four matrices of the object
# - p - parent widget
# - thisvar - name of the array in calling context to list name=value pairs
# - descr - description of the function: name, type, label, parameters
# Returns: 1 - if there were changes; 0 - there were not changes in
# parameters
proc SSModelEditor {p thisvar} {
    set w $p.ssmeditor
    catch {destroy $w}
    toplevel $w

    upvar $thisvar this

    wm title $w "State Space Model"

    frame $w.common
    label $w.common.main_label -text $label -anchor w
    pack $w.common.main_label

    frame $w.dimensions
    

    # Let's find image to illustrate the function
    foreach ext {gif ppm pgm} {
	set imgfile [file join [TemplateDir] "$idname.$ext"]
	#puts "check for $imgfile"
	if {[file exists $imgfile]} {
	    set img [image create photo -file $imgfile]
	    label $w.common.image -image $img
	    pack $w.common.image -side top -padx 3 -pady 3
	    break
	}
    }
    pack $w.common -side top

    frame $w.parameters
    # This variable consist of all operations to store state of dialog
    set save_all_vars $w.save_all_vars
    global $save_all_vars
    set $save_all_vars ""
    foreach {key pos} $key_pos {
	label $w.parameters.label_$key -text $key
	set e $w.parameters.value_$key
	# This variable is global
	set evar $w.var_$key
	global $evar
	if {[info exists this($key)]} {
	    set $evar $this($key)
	} else {
	    set $evar ""
	}
	entry $e -width 12 -justify right -validate focus \
	    -vcmd {string is double %P}
	$e insert 0 [set $evar]
	set $save_all_vars "set $evar \[$e get\] ; [set $save_all_vars]"
	$e configure -invalidcommand "focusAndFlash %W [$e cget -fg] [$e cget -bg]"
	grid $w.parameters.label_$key $e
    }
    pack $w.parameters -side top

    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.ok -text [mc "OK"] -command "[set $save_all_vars] destroy $w"
    button $w.buttons.cancel -text [mc "Cancel"] -command "destroy $w"

    set m $w.buttons.probe.m
    menubutton $w.buttons.probe -text [mc "Response"] \
	-direction below -menu $m -relief raised
    menu $m -tearoff 0
    foreach probesignal {pulse step sin_4 sin_10 sin_20} {
	$m add command -label $probesignal \
	    -command "TrFuncProbeTemporal $w [list $descr] $probesignal"
    }
    #grid $w.buttons.probe -row 1 -column 0 -sticky n

    pack $w.buttons.ok $w.buttons.probe $w.buttons.cancel -side left -expand 1

    tkwait window $w

    # Assign stored state
    set changed 0
    foreach {key pos} $key_pos {
	set evar $w.var_$key
	global $evar
	if {$this($key) != [set $evar]} {
	    set this($key) [set $evar]
	    #puts "$key=$this($key)"
	    set changed 1
	}
    }
    return $changed
}


# Call file state space model editor
# - forceNew - boolean: true to create new file anyway
# - asText - boolean: true to edit as text file
proc SSModelEdit {p sessionDir title fileRelPath {forceNew false} {asText false}} {
    puts "SSModelEdit: $sessionDir $fileRelPath"
    set ftype [TrFuncNewType $p $sessionDir $fileRelPath $forceNew]
    if {$forceNew && $ftype == {}} {
	# User cancelled transfer function creation
	return
    }
    set fileName [SessionAbsPath $sessionDir $fileRelPath]
    # Now it's possible to edit the file

    # Let's determine type of the file
    switch -glob -- $fileName {
	*.tf {
	    set descr [TrFuncParseFile $fileName]
	    if {[llength $descr] == 4 &&
		[lindex $descr 0] != {} && [lindex $descr 1] != {} &&
		[lindex $descr 2] != {} && [lindex $descr 3] != {}} {
		# The whole definition is in the file
		set ftype trfunc
	    } elseif {[llength $descr] == 4 &&
		      [lindex $descr 0] != {}} {
		# Only idname was found: let's use template
		set descr [TrFuncParseTemplate [lindex $descr 0]]
		if {$descr != {}} {
		    set ftype trfunc
		} else {
		    set ftype undefined
		}
	    } else {
		set ftype undefined
	    }
	}
	*.cof {
	    set ftype cofunc
	}
	default {
	    set ftype undefined
	}
    }
    # Let's call edit method
    if {$asText} {
	set ftype undefined
    }
    switch -exact -- $ftype {
	trfunc {
	    # Load
	    array set params {}
	    set fd [open $fileName]
	    set ftext [split [read $fd] \n]
	    close $fd
	    # Parse
	    set idname [lindex $descr 0]
	    set type [lindex $descr 1]
	    set label [lindex $descr 2]
	    set key_pos [lindex $descr 3]
	    TrFuncLoadConfig params $descr $ftext
	    # Edit
	    if {[TrFuncEditor $p params $descr]} {
		set headLineFields [split [lindex $ftext 0]]
		set fd [open $fileName "w"]
		if {[lindex $headLineFields 0] != ";NeuCon" &&
		    [lindex $headLineFields 1] != "transfer" } {
		    puts $fd ";NeuCon transfer 1.0"
		    puts $fd "\[$type $idname\]"
		}
		# Save
		TrFuncSaveConfig params $descr $fd $ftext
		flush $fd
		close $fd
	    }
	    # otherwise no changes took place
	}
	cofunc {
	    # Load & parse
	    array set cof {main {CombinedFunction {}}}
	    catch {array set cof [CoFuncParseFile $fileName]}
	    puts "LOAD: $fileName [array get cof]"
	    # Edit
	    if {[CoFuncEditor $p cof]} {
		# Save
		puts "SAVE: $fileName [array get cof]"
		CoFuncComposeFile $fileName [array get cof]
	    }
	    # otherwise no changes took place
	}
	undefined {
	    TextEditWindow $p "$title" $fileName
	}
    }
}


# End of file
