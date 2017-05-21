package require Tk

#source balloon.tcl

proc SSModelDisplayData {f varN varM varK x0 A B C D} {
    upvar #0 $varN n
    upvar #0 $varM m
    upvar #0 $varK k

    set debugCells 0

    frame $f

    # resource values
    set cellwidth 8
    set defvalue 0
    #array set bgc {A pink B LightCyan C Lavender D PaleGreen x0 Bisque}
    array set bgc {A MistyRose B Lavender C Lavender D MistyRose x0 Honeydew}

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
	    } elseif {![info exist $f.A_${j}_$i]} {
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
	    #balloon $f.eA_${j}_$i -text "A([expr 1+$j],[expr 1+$i])"
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
	    } elseif {![info exist $f.B_${j}_$i]} {
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
	    #balloon $f.eB_${j}_$i -text "B([expr 1+$j],[expr 1+$i])"
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
	    } elseif {![info exist $f.C_${j}_$i]} {
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
	    #balloon $f.eC_${j}_$i -text "C([expr 1+$j],[expr 1+$i])"
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
	    } elseif {![info exist $f.D_${j}_$i]} {
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
	    #balloon $f.eD_${j}_$i -text "D([expr 1+$j],[expr 1+$i])"
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
	} elseif {![info exist $f.x0_$i]} {
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
	#balloon $f.ex0_$i -text "x0([expr 1+$i])"
	grid $f.ex0_$i -row [expr $r] -column [expr $c + $i]
    }
    incr r

    pack $f
}

# Display parameters of the object
# - p - parent widget
# - thisvar - name of the array in calling context to list name=value pairs
# Returns: 1 - if there were changes; 0 - there were no changes in
# parameters
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
    label $f.lstates -text [mc "States:"]
    label $f.linputs -text [mc "Inputs:"]
    label $f.loutputs -text [mc "Outputs:"]

    foreach x {states inputs outputs} {
	spinbox $f.e$x -width 3 -from 1 -to 10 -textvariable $w.$x \
	    -command "destroy $w.data ; \
		SSModelDisplayData $w.data $w.states $w.outputs $w.inputs \
		    {$this(x0)} {$this(a)} {$this(b)} {$this(c)} {$this(d)}"
	grid $f.l$x $f.e$x
    }

    # Let's find image to illustrate the function
    #set imgfile [file join [TemplateDir] "ssmodel.gif"]
    set imgfile templates/ssmodel.gif
    #puts "check for $imgfile"
    if {[file exists $imgfile]} {
	set img [image create photo -file $imgfile]
	label $w.info.picture -image $img
    }
    grid $w.info.dimensions $w.info.picture -padx 5

    pack $w.info -anchor nw

    SSModelDisplayData $w.data $w.states $w.outputs $w.inputs \
	$this(x0) $this(a) $this(b) $this(c) $this(d)
}

set ssm {
x0 {0 0 0 0 0 0 0 0} d {{0 0} {0 0}} outputs 2 states 8 a {{1.828 -0.8344 0 0 0 0 0 0} {1 0 0 0 0 0 0 0} {0 0 1.929 -0.9315 0 0 0 0} {0 0 1 0 0 0 0 0} {0 0 0 0 1.936 -0.938 0 0} {0 0 0 0 1 0 0 0} {0 0 0 0 0 0 1.791 -0.8009} {0 0 0 0 0 0 1 0}} name {} b {{0.5 0} {0 0} {0.25 0} {0 0} {0 0.25} {0 0} {0 0.25} {0 0}} version 1.0 c {{0.2401 0.2261 0 0 0.2291 0.2242 0 0} {0 0 0.207 0.2022 0 0 0.1858 0.1726}} inputs 2
}

#    name "ssm1" states 2 inputs 1 outputs 2
#    a {{1 0.5} {0 2}} b {0 1} c {{1 0} {0 1}} d {0 0} x0 {0 0}

array set ssmAr $ssm

SSModelEditor "" ssmAr
