package require Tk


proc SSModelDisplayData {f varN varM varK x0 A B C D} {
    upvar #0 $varN n
    upvar #0 $varM m
    upvar #0 $varK k

    set debugCells 0

    frame $f

    set r 0

    set c 0
    label $f.lA -text "A:"
    grid $f.lA -row $r -column $c
    incr c
    for {set j 0} {$j < $n} {incr j} {
	for {set i 0} {$i < $n} {incr i} {
	    global $f.A_${j}_$i
	    if $debugCells {
		set $f.A_${j}_$i "A\[$j,$i\]"
	    } elseif {![info exist $f.A_${j}_$i]} {
		set $f.A_${j}_$i [lindex $A $j $i]
	    }
	    entry $f.eA_${j}_$i -width 10 -textvariable $f.A_${j}_$i
	    grid $f.eA_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
	}
    }

    incr c $n
    label $f.lB -text "B:"
    grid $f.lB -row $r -column [expr $c]
    incr c
    for {set j 0} {$j < $n} {incr j} {
	for {set i 0} {$i < $k} {incr i} {
	    global $f.B_${j}_$i
	    if $debugCells {
		set $f.B_${j}_$i "B\[$j,$i\]"
	    } elseif {![info exist $f.B_${j}_$i]} {
		set $f.B_${j}_$i [lindex $B $j $i]
	    }
	    entry $f.eB_${j}_$i -width 10 -textvariable $f.B_${j}_$i
	    grid $f.eB_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
	}
    }
    incr r $n

    incr r
    set c 0
    label $f.lC -text "C:"
    grid $f.lC -row $r -column $c
    incr c
    for {set j 0} {$j < $m} {incr j} {
	for {set i 0} {$i < $n} {incr i} {
	    global $f.C_${j}_$i
	    if $debugCells {
		set $f.C_${j}_$i "C\[$j,$i\]"
	    } elseif {![info exist $f.C_${j}_$i]} {
		set $f.C_${j}_$i [lindex $C $j $i]
	    }
	    entry $f.eC_${j}_$i -width 10 -textvariable $f.C_${j}_$i
	    grid $f.eC_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
	}
    }

    incr c $n
    label $f.lD -text "D:"
    grid $f.lD -row $r -column [expr $c]
    incr c
    for {set j 0} {$j < $m} {incr j} {
	for {set i 0} {$i < $k} {incr i} {
	    global $f.D_${j}_$i
	    if $debugCells {
		set $f.D_${j}_$i "D\[$j,$i\]"
	    } elseif {![info exist $f.D_${j}_$i]} {
		set $f.D_${j}_$i [lindex $D $j $i]
	    }
	    entry $f.eD_${j}_$i -width 10 -textvariable $f.D_${j}_$i
	    grid $f.eD_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
	}
    }
    incr r $m

    set c 0
    label $f.lx0 -text "x0:"
    grid $f.lx0 -row $r -column $c
    incr c
    for {set i 0} {$i < $n} {incr i} {
	global $f.x0_$i
	if $debugCells {
	    set $f.x0_$i "x0\[$i\]"
	} elseif {![info exist $f.x0_$i]} {
	    set $f.x0_$i [lindex $x0 $i]
	}
	entry $f.ex0_$i -width 10 -textvariable $f.x0_$i
	grid $f.ex0_$i -row [expr $r] -column [expr $c + $i]
    }
    incr r

    pack $f
}

# Display parameters of the object
# - p - parent widget
# - thisvar - name of the array in calling context to list name=value pairs
# Returns: 1 - if there were changes; 0 - there were not changes in
# parameters
proc SSModelEditor {p thisvar} {
    set w $p.ssmodel
    catch {destroy $w}
    toplevel $w

    #set debugCells 0

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

    frame $w.dimensions
    set f $w.dimensions
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
    pack $f -anchor nw

    #frame $w.data

    SSModelDisplayData $w.data $w.states $w.outputs $w.inputs \
	$this(x0) $this(a) $this(b) $this(c) $this(d)

    # set f $w.data

    # set r 0

    # set c 0
    # label $f.la -text "A:"
    # grid $f.la -row $r -column $c
    # incr c
    # for {set j 0} {$j < $n} {incr j} {
    # 	for {set i 0} {$i < $n} {incr i} {
    # 	    global a_${j}_$i
    # 	    if $debugCells {
    # 		set a_${j}_$i "A\[$j,$i\]"
    # 	    } else {
    # 		set a_${j}_$i [lindex $this(a) $j $i]
    # 	    }
    # 	    entry $f.ea_${j}_$i -width 10 -textvariable a_${j}_$i
    # 	    grid $f.ea_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
    # 	}
    # }

    # incr c $n
    # label $f.lb -text "B:"
    # grid $f.lb -row $r -column [expr $c]
    # incr c
    # for {set j 0} {$j < $n} {incr j} {
    # 	for {set i 0} {$i < $k} {incr i} {
    # 	    global b_${j}_$i
    # 	    if $debugCells {
    # 		set b_${j}_$i "B\[$j,$i\]"
    # 	    } else {
    # 		set b_${j}_$i [lindex $this(b) $j $i]
    # 	    }
    # 	    entry $f.eb_${j}_$i -width 10 -textvariable b_${j}_$i
    # 	    grid $f.eb_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
    # 	}
    # }
    # incr r $n

    # incr r
    # set c 0
    # label $f.lc -text "C:"
    # grid $f.lc -row $r -column $c
    # incr c
    # for {set j 0} {$j < $m} {incr j} {
    # 	for {set i 0} {$i < $n} {incr i} {
    # 	    global c_${j}_$i
    # 	    if $debugCells {
    # 		set c_${j}_$i "C\[$j,$i\]"
    # 	    } else {
    # 		set c_${j}_$i [lindex $this(c) $j $i]
    # 	    }
    # 	    entry $f.ec_${j}_$i -width 10 -textvariable c_${j}_$i
    # 	    grid $f.ec_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
    # 	}
    # }

    # incr c $n
    # label $f.ld -text "D:"
    # grid $f.ld -row $r -column [expr $c]
    # incr c
    # for {set j 0} {$j < $m} {incr j} {
    # 	for {set i 0} {$i < $k} {incr i} {
    # 	    global d_${j}_$i
    # 	    if $debugCells {
    # 		set d_${j}_$i "D\[$j,$i\]"
    # 	    } else {
    # 		set d_${j}_$i [lindex $this(d) $j $i]
    # 	    }
    # 	    entry $f.ed_${j}_$i -width 10 -textvariable d_${j}_$i
    # 	    grid $f.ed_${j}_$i -row [expr $r + $j] -column [expr $c + $i]
    # 	}
    # }
    # incr r $m

    # set c 0
    # label $f.lx0 -text "x0:"
    # grid $f.lx0 -row $r -column $c
    # incr c
    # for {set i 0} {$i < $n} {incr i} {
    # 	global x0_$i
    # 	if $debugCells {
    # 	    set x0_$i "x0\[$i\]"
    # 	} else {
    # 	    set x0_$i [lindex $this(x0) $i]
    # 	}
    # 	global x0_$i
    # 	entry $f.ex0_$i -width 10 -textvariable x0_$i
    # 	grid $f.ex0_$i -row [expr $r] -column [expr $c + $i]
    # }
    # incr r

    #pack $f
}

set ssm {
    name "ssm1" states 2 inputs 1 outputs 2
    a {{1 0.5} {0 2}} b {0 1} c {{1 0} {0 1}} d {0 0} x0 {0 0}
}

array set ssmAr $ssm

SSModelEditor "" ssmAr
