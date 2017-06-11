# State space model implementation
package provide win_ssmodel 1.0

package require Tk
package require files_loc
package require universal
package require ssm_file
package require balloon


# Format of internal SSM representation:
# { name value ... }
# where name-value pairs are in arbitrary order (adopted to be array).
# Special name="name" is a name of StateSpaceModel (optional).
# Special name="version" is a version of StateSpaceModel file, "1.0" for example (optional).
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
# { states 2 inputs 1 outputs 2
#   a {{1 0.5} {0 2}} b {0 1} c {{1 0} {0 1}} d {0 0} x0 {0 0} }


# Display frame $f with grid of editable cells
# - varN - name of variable with number of states
# - varM - name of variable with number of outputs
# - varK - name of variable with number of inputs
# - x0, A, B, C, D - vector and matrices of initial values for the grid
# Result in global variables:
#  $f.A_(0..$N-1)_(0..$N-1)
#  $f.B_(0..$N-1)_(0..$K-1)
#  $f.C_(0..$M-1)_(0..$N-1)
#  $f.D_(0..$M-1)_(0..$K-1)
#  $f.x0_(0..$N-1)
proc SSModelDisplayData {f varN varM varK x0 A B C D} {
    upvar #0 $varN n
    upvar #0 $varM m
    upvar #0 $varK k

    # set to 1 to display index in every cell
    set debugCells 0

    frame $f

    # resource values
    set cellwidth 8
    set defvalue 0
    array set bgc {A pink B LightCyan C Lavender D PaleGreen x0 Bisque}

    set r 0
    set c 0

    label $f.lA -text "A:" -background $bgc(A)
    grid $f.lA -row $r -column $c
    incr c
    for {set j 0} {$j < $n} {incr j} {
	for {set i 0} {$i < $n} {incr i} {
	    global $f.A_${j}_$i
	    if $debugCells {
		set $f.A_${j}_$i "A\[$j,$i\]"
	    } else {
		set cellval [lindex $A $j $i]
		if {{} == $cellval} {
		    set $f.A_${j}_$i $defvalue
		} else {
		    set $f.A_${j}_$i $cellval
		}
	    }
	    entry $f.eA_${j}_$i -width $cellwidth \
		-validate key -vcmd {string is double %P} \
		-textvariable $f.A_${j}_$i \
		-background $bgc(A)
	    balloon $f.eA_${j}_$i -text "A([expr 1+$j],[expr 1+$i])"
	    grid $f.eA_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
	}
    }

    incr c $n
    label $f.lB -text "B:" -background $bgc(B)
    grid $f.lB -row $r -column [expr $c]
    incr c
    for {set j 0} {$j < $n} {incr j} {
	for {set i 0} {$i < $k} {incr i} {
	    global $f.B_${j}_$i
	    if $debugCells {
		set $f.B_${j}_$i "B\[$j,$i\]"
	    } else {
		set cellval [lindex $B $j $i]
		if {{} == $cellval} {
		    set $f.B_${j}_$i $defvalue
		} else {
		    set $f.B_${j}_$i $cellval
		}
	    }
	    entry $f.eB_${j}_$i -width $cellwidth \
		-validate key -vcmd {string is double %P} \
		-textvariable $f.B_${j}_$i \
		-background $bgc(B)
	    balloon $f.eB_${j}_$i -text "B([expr 1+$j],[expr 1+$i])"
	    grid $f.eB_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
	}
    }
    incr r $n

    incr r
    set c 0
    label $f.lC -text "C:" -background $bgc(C)
    grid $f.lC -row $r -column $c
    incr c
    for {set j 0} {$j < $m} {incr j} {
	for {set i 0} {$i < $n} {incr i} {
	    global $f.C_${j}_$i
	    if $debugCells {
		set $f.C_${j}_$i "C\[$j,$i\]"
	    } else {
		set cellval [lindex $C $j $i]
		if {{} == $cellval} {
		    set $f.C_${j}_$i $defvalue
		} else {
		    set $f.C_${j}_$i $cellval
		}
	    }
	    entry $f.eC_${j}_$i -width $cellwidth \
		-validate key -vcmd {string is double %P} \
		-textvariable $f.C_${j}_$i \
		-background $bgc(C)
	    balloon $f.eC_${j}_$i -text "C([expr 1+$j],[expr 1+$i])"
	    grid $f.eC_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
	}
    }

    incr c $n
    label $f.lD -text "D:" -background $bgc(D)
    grid $f.lD -row $r -column [expr $c]
    incr c
    for {set j 0} {$j < $m} {incr j} {
	for {set i 0} {$i < $k} {incr i} {
	    global $f.D_${j}_$i
	    if $debugCells {
		set $f.D_${j}_$i "D\[$j,$i\]"
	    } else  {
		set cellval [lindex $D $j $i]
		if {{} == $cellval} {
		    set $f.D_${j}_$i $defvalue
		} else {
		    set $f.D_${j}_$i $cellval
		}
	    }
	    entry $f.eD_${j}_$i -width $cellwidth \
		-validate key -vcmd {string is double %P} \
		-textvariable $f.D_${j}_$i \
		-background $bgc(D)
	    balloon $f.eD_${j}_$i -text "D([expr 1+$j],[expr 1+$i])"
	    grid $f.eD_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
	}
    }
    incr r $m

    set c 0
    label $f.lx0 -text "x0:" -background $bgc(x0)
    grid $f.lx0 -row $r -column $c
    incr c
    for {set i 0} {$i < $n} {incr i} {
	global $f.x0_$i
	if $debugCells {
	    set $f.x0_$i "x0\[$i\]"
	} else {
	    set cellval [lindex $x0 $i]
	    if {{} == $cellval} {
		set $f.x0_$i $defvalue
	    } else {
		set $f.x0_$i $cellval
	    }
	}
	entry $f.ex0_$i -width $cellwidth \
	    -validate key -vcmd {string is double %P} \
	    -textvariable $f.x0_$i \
	    -background $bgc(x0)
	balloon $f.ex0_$i -text "x0([expr 1+$i])"
	grid $f.ex0_$i -row [expr $r] -column [expr $c + $i]
    }
    incr r

    pack $f
}

# Display parameters of the State-Space object
# - p - parent widget
# - thisvar - name of the array in calling context to list name=value pairs
# Returns:
#  1 - if there were changes;
#  0 - there were no changes in any parameters
proc SSModelEditor {p thisvar} {
    set w $p.ssmodel
    catch {destroy $w}
    toplevel $w

    upvar $thisvar this
    set n $this(states)
    set m $this(outputs)
    set k $this(inputs)

    # +---+--+--+----+--+--+
    # |A: |  |  | B: |  |  |
    # +---+--+--+----+--+--+
    # |   |  |  |    |  |  |
    # +---+--+--+----+--+--+
    # |C: |  |  | D: |  |  |
    # +---+--+--+----+--+--+
    # |x0:|  |  |
    # +---+--+--+

    global $w.states $w.inputs $w.outputs
    set $w.states $n
    set $w.outputs $m
    set $w.inputs $k

    frame $w.info

    frame $w.info.dimensions
    set f $w.info.dimensions
    label $f.lstates -text "States:"
    label $f.linputs -text "Inputs:"
    label $f.loutputs -text "Outputs:"

    foreach x {states inputs outputs} {
	spinbox $f.e$x -width 3 -from 1 -to 10 -textvariable $w.$x \
	    -command "destroy $w.data ; \
		SSModelDisplayData $w.data $w.states $w.outputs $w.inputs \
		    {$this(x0)} {$this(a)} {$this(b)} {$this(c)} {$this(d)}"
	grid $f.l$x $f.e$x
    }

    # Let's find image to illustrate the function
    set imgfile [file join [TemplateDir] "ssmodel.gif"]
    #set imgfile templates/ssmodel.gif
    if {[file exists $imgfile]} {
	set img [image create photo -file $imgfile]
	label $w.info.picture -image $img
    }
    grid $w.info.dimensions $w.info.picture -padx 5

    pack $w.info -anchor nw

    SSModelDisplayData $w.data $w.states $w.outputs $w.inputs \
	$this(x0) $this(a) $this(b) $this(c) $this(d)


    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m

    global $w.ok_pressed
    set $w.ok_pressed 0

    button $w.buttons.ok -text "OK" \
	-command "global $w.ok_pressed; set $w.ok_pressed 1; destroy $w"
    button $w.buttons.cancel -text "Cancel" -command "destroy $w"
    pack $w.buttons.ok $w.buttons.cancel -side left -expand 1

    tkwait window $w

    set changed 0
    if {[set $w.ok_pressed]} {

	# Check for changed dimensions
	foreach {key evar} "states $w.states outputs $w.outputs inputs $w.inputs" {
	    if {$this($key) != [set $evar]} {
		set this($key) [set $evar]
		set changed 1
	    }
	}

	if {$changed } {
	    set n $this(states)
	    set m $this(outputs)
	    set k $this(inputs)
	}

	# Store all values from grid into $this
	set f $w.data

	set new_a {}
	for {set j 0} {$j < $n} {incr j} {
	    set new_row {}
	    for {set i 0} {$i < $n} {incr i} {
		global $f.A_${j}_$i
		if {[info exist $f.A_${j}_$i]} {
		    lappend new_row [set $f.A_${j}_$i]
		}
	    }
	    lappend new_a $new_row
	}
	if {$this(a) != $new_a} {
	    set this(a) $new_a
	    set changed 1
	}

	set new_b {}
	for {set j 0} {$j < $n} {incr j} {
	    set new_row {}
	    for {set i 0} {$i < $k} {incr i} {
		global $f.B_${j}_$i
		if {[info exist $f.B_${j}_$i]} {
		    lappend new_row [set $f.B_${j}_$i]
		}
	    }
	    lappend new_b $new_row
	}
	if {$this(b) != $new_b} {
	    set this(b) $new_b
	    set changed 1
	}

	set new_c {}
	for {set j 0} {$j < $m} {incr j} {
	    set new_row {}
	    for {set i 0} {$i < $n} {incr i} {
		global $f.C_${j}_$i
		if {[info exist $f.C_${j}_$i]} {
		    lappend new_row [set $f.C_${j}_$i]
		}
	    }
	    lappend new_c $new_row
	}
	if {$this(c) != $new_c} {
	    set this(c) $new_c
	    set changed 1
	}

	set new_d {}
	for {set j 0} {$j < $m} {incr j} {
	    set new_row {}
	    for {set i 0} {$i < $k} {incr i} {
		global $f.D_${j}_$i
		if {[info exist $f.D_${j}_$i]} {
		    lappend new_row [set $f.D_${j}_$i]
		}
	    }
	    lappend new_d $new_row
	}
	if {$this(d) != $new_d} {
	    set this(d) $new_d
	    set changed 1
	}

	set new_x0 {}
	for {set i 0} {$i < $n} {incr i} {
	    global $f.x0_$i
	    if {[info exist $f.x0_$i]} {
		lappend new_x0 [set $f.x0_$i]
	    }
	}
	if {$this(x0) != $new_x0} {
	    set this(x0) $new_x0
	    set changed 1
	}
    }

    return $changed
}


# End of file
