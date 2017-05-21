#pkg_mkIndex .
#lappend auto_path .
package provide win_nncedit 1.0

package require Tk
package require draw_nn
package require screenshot

proc NNCEditSave {w filepath nncinputs} {
    set f $w.nnarch
    upvar #0 $f.inputdim_var inputdim
    upvar #0 $f.outputdim_var outputdim
    upvar #0 $f.inputrep_var inputrep
    upvar #0 $f.outputrep_var outputrep
    upvar #0 $f.numlayers_var numlayers
    upvar #0 $f.numneurons1_var numneurons1
    upvar #0 $f.numneurons2_var numneurons2
    upvar #0 $f.numneurons3_var numneurons3
    upvar #0 $f.outputfunc_var outputfunc
    upvar #0 $f.inputlabels_var inputlabels
    upvar #0 $f.outputlabels_var outputlabels

    if {$inputdim == {}} {
	set inputdim 1
    }
    if {$outputdim == {}} {
	set outputdim 1
    }

    set args {}
    if {$nncinputs == "e+r+se"} {
	lappend args $inputdim 3
    } elseif {$nncinputs == "e+r" || $nncinputs == "e+de"} {
	lappend args $inputdim 2
    } else {
	lappend args $inputdim $inputrep
    }
    array set act {linear 0 tanh 1}
    lappend args $outputdim $outputrep 0 $act($outputfunc) $numlayers
    for {set iL 1} {$iL <= $numlayers} {incr iL} {
	eval lappend args \$numneurons$iL
    }
    set exefile [file join [SystemDir] bin MakeNN]
    puts "Run MakeNN: \"$exefile\" \"$filepath\" Controller $args"
    global NullDev
    catch {eval exec \"$exefile\" \"$filepath\" Controller $args >$NullDev} errCode
    if {$errCode != ""} {
	error $errCode
    }
}

proc NNCEditOk {w filepath nncinputs} {
    NNCEditSave $w $filepath $nncinputs
    destroy $w
}

proc NNCEditDoResize {w nncinputs} {
    global NNCEditDoResize_redo
    # To avoid redrawing the plot many times during resizing,
    # cancel the callback, until the last one is left.
    if { [info exists NNCEditDoResize_redo] } {
        after cancel ${NNCEditDoResize_redo}
    }
    set redo [after 50 "NNCEditDoPlot $w $nncinputs"]
}

proc NNCEditDoPlot {w nncinputs} {
    set c $w.nnpicture.c
    set f $w.nnarch

    upvar #0 $f.inputdim_var inputdim
    upvar #0 $f.outputdim_var outputdim
    upvar #0 $f.inputrep_var inputrep
    upvar #0 $f.outputrep_var outputrep
    upvar #0 $f.numlayers_var numlayers
    upvar #0 $f.numneurons1_var numneurons1
    upvar #0 $f.numneurons2_var numneurons2
    upvar #0 $f.numneurons3_var numneurons3
    upvar #0 $f.outputfunc_var outputfunc
    upvar #0 $f.inputlabels_var inputlabels
    upvar #0 $f.outputlabels_var outputlabels

    set inputs [expr $inputrep * $inputdim + $outputrep * $outputdim]
    set nnarch {}
    lappend nnarch [list $inputs]
    lappend nnarch [expr $numlayers + 1]
    for {set i 1} {$i <= $numlayers} {incr i} {
	eval set numneurons \$numneurons$i
	lappend nnarch "$numneurons tanh"
    }
    lappend nnarch [list $outputdim $outputfunc]

    # Let's append map of inputs
    lappend nnarch [list "idim" $inputdim "irep" $inputrep "odim" $outputdim "orep" $outputrep]

    # Let's append limits
    set limits {}
    for {set i 0} {$i < [expr $inputs + 1]} {incr i} {
	lappend limits {-1 1}
    }
    lappend nnarch $limits

    $c delete all

    # nnarch = {{Inputs [InputLabels]} {HidNeurons1 HidType1}
    # {HidNeurons2 HidType2} ...  {Outputs OutputType OutputLabels}},
    # where type is "linear" or "tanh" and InputLabels or/and
    # OutputLabels may be absent.
    #puts "nncedit: $nnarch"
    DrawNeuralNetArch $c [NNCDecorateNNArch $nnarch]
}


proc NNCEditModified {w} {
    # Set attributes of modified text contents
    set modifiedFg white
    set modifiedBg red
    $w.buttons.ok configure -bg $modifiedBg -fg $modifiedFg \
	-activebackground $modifiedBg -activeforeground $modifiedFg
    $w.buttons.save configure -bg $modifiedBg -fg $modifiedFg \
	-activebackground $modifiedBg -activeforeground $modifiedFg
}

proc NNCEditFieldChange {w p nncinputs} {
    if {![string is integer -strict $p]} {
	return false
    }
    after 200 "NNCEditDoPlot $w $nncinputs"
    return true
}

# f - widget where hidden neurons are listed
# p - value to validate and consider
proc NNCEditHiddenLayersChange {w p nncinputs} {
    if {![string is integer -strict $p]} {
	return false
    }
    set f $w.nnarch
    foreach i {1 2 3} {
	set state normal
	if { $p < $i } {
	    set state disabled
	}
	$f.numneurons${i} configure -state $state
	$f.numneurons${i}_l configure -state $state
    }
    after 200 "NNCEditDoPlot $w $nncinputs"
    return true
}

# w - parent
# title - text to show
# filepath - file to store neural network
# nncinputs - "e+r", "e+r+se", "e+de", "e+e+..."
proc NNCEditWindow {p title filepath nncinputs} {
    set w $p.textedit
    catch {destroy $w}
    toplevel $w
    wm title $w $title

    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.ok -text [mc "OK"] -command "NNCEditOk $w \"$filepath\" $nncinputs"
    button $w.buttons.save -text [mc "Save"] -command "NNCEditSave $w \"$filepath\" $nncinputs"
    button $w.buttons.cancel -text [mc "Cancel"] -command "destroy $w"
    pack $w.buttons.ok $w.buttons.save $w.buttons.cancel -side left -expand 1

    set f [frame $w.nnarch]
    pack $f -side left -fill y -padx 0 -anchor n

    # Setup variables
    global $f.inputrep_var $f.outputrep_var $f.numlayers_var \
	$f.numneurons1_var $f.numneurons2_var $f.numneurons3_var \
	$f.outputfunc_var $f.inputlabels_var $f.outputlabels_var

    set nnarch {}
    if {[file exists $filepath]} {
	set nnarch [ReadNeuralNetFile $filepath]
    }
    if {$nnarch == {}} {
	# Default parameters
	set $f.inputrep_var 2
	set $f.outputrep_var 0
	set $f.numlayers_var 2
	set $f.numneurons1_var 7
	set $f.numneurons2_var 5
	set $f.numneurons3_var 5
	set $f.outputfunc_var "linear"
    } else {
	set $f.inputrep_var 2
	set $f.outputrep_var 0

	# Taken from .nn file
	set nlayers [expr [lindex $nnarch 1] - 1]
	set $f.numlayers_var $nlayers
	foreach {i defv} {1 7  2 7  3 5} {
	    if {$i <= $nlayers} {
		set $f.numneurons${i}_var [lindex $nnarch [expr 1 + $i] 0]
	    } else {
		set $f.numneurons${i}_var $defv
	    }
	}
	set $f.outputfunc_var [lindex $nnarch [expr 2 + $nlayers] 1]
    }
    set $f.inputlabels_var {i1 i2 i3 i4 i5 i6 i7 i8 i9}
    set $f.outputlabels_var {o1 o2 o3 o4 o5 o6 o7 o8 o9}

    grid [label $f.inputdim_l -text [mc "Input dimension"] -justify left] \
	[spinbox $f.inputdim -from 1 -to 10 -width 4 -validate key \
	     -justify right -textvariable $f.inputdim_var \
	     -vcmd "NNCEditFieldChange $w %P $nncinputs"]
    grid [label $f.outputdim_l -text [mc "Output dimension"] -justify left] \
	[spinbox $f.outputdim -from 1 -to 10 -width 4 -validate key \
	     -justify right -textvariable $f.outputdim_var \
	     -vcmd "NNCEditFieldChange $w %P $nncinputs"]

    grid [label $f.inputrep_l -text [mc "Inputs (w/repeat)"] -justify left] \
	[spinbox $f.inputrep -from 1 -to 100 -width 4 -validate key \
	     -justify right -textvariable $f.inputrep_var \
	     -vcmd "NNCEditFieldChange $w %P $nncinputs"]
    grid [label $f.outputrep_l -text [mc "Outputs (w/repeat)"] -justify left] \
	[spinbox $f.outputrep -from 0 -to 100 -width 4 -validate key \
	     -justify right -textvariable $f.outputrep_var \
	     -vcmd "NNCEditFieldChange $w %P $nncinputs"]

    switch -exact $nncinputs {
	"e+r+se" {
	    set $f.inputrep_var 3
	    $f.inputrep_l configure -state disabled
	    $f.inputrep configure -state disabled
	}
	"e+r" {
	    set $f.inputrep_var 2
	    $f.inputrep_l configure -state disabled
	    $f.inputrep configure -state disabled
	}
	"e+de" {
	    set $f.inputrep_var 2
	    $f.inputrep_l configure -state disabled
	    $f.inputrep configure -state disabled
	}
	"e+e+..." {
	    set $f.inputrep_var 3
	}
    }
    $f.outputrep_l configure -state disabled
    $f.outputrep configure -state disabled

    grid [label $f.numlayers_l -text [mc "Hidden layers"] -justify left] \
	[spinbox $f.numlayers -from 0 -to 3 -width 4 -validate key \
	     -textvariable $f.numlayers_var -justify right]
    foreach i {1 2 3} {
	upvar 0 $f.numlayers_var numlayers
	grid [label $f.numneurons${i}_l -text [format [mc "Hidden layer #%d"] $i] \
		  -justify left] \
	    [spinbox $f.numneurons${i} -from 1 -to 100 -width 4 -validate key \
		 -justify right -textvariable $f.numneurons${i}_var \
		 -vcmd "NNCEditFieldChange $w %P $nncinputs"]
	if {$numlayers < $i} {
	    $f.numneurons${i}_l configure -state disabled
	    $f.numneurons${i} configure -state disabled
	}
    }
    $f.numlayers configure -vcmd "NNCEditHiddenLayersChange $w %P $nncinputs"
    grid [labelframe $f.outputfunc -text [mc "Output function"]] \
	-column 0 -columnspan 2 -sticky we

    radiobutton $f.outputfunc.linear -text [mc "Linear"] \
	-variable $f.outputfunc_var -value "linear" -justify left \
	-command "NNCEditDoPlot $w $nncinputs"
    radiobutton $f.outputfunc.sigmoid -text [mc "Sigmoid"] \
	-variable $f.outputfunc_var -value "tanh" -justify left \
	-command "NNCEditDoPlot $w $nncinputs"
    pack $f.outputfunc.linear $f.outputfunc.sigmoid -side top -fill x

    foreach i "$f.inputrep $f.outputrep $f.numlayers 
$f.numneurons1 $f.numneurons2 $f.numneurons3" {
	bind $i <<Modified>> "NNCEditModified $w"
    }

    grid columnconfigure $f 0 -weight 1
    grid columnconfigure $f 1 -weight 3

    set c $w.nnpicture.c
    frame $w.nnpicture
    grid [canvas $c -background white -width 500 -height 200] -sticky news
    grid columnconfigure $w.nnpicture 0 -weight 1
    grid rowconfigure $w.nnpicture 0 -weight 1
    pack $w.nnpicture -expand yes -fill both -side right

    bind $c <Configure> "NNCEditDoResize $w $nncinputs"

#    $f.text edit modified 0
#    bind $f.text <<Modified>> \
#	"NNCEditModified $w %W ; TextSyntaxHighlight $ftype %W"
}


# Decorate labels as for NN controller.
# Takes on input an NN architecture without labels.
# Return architecture in format acceptable for DrawNeuralNetArch.
proc NNCDecorateNNArch {nnarch} {
    set ninputs [lindex $nnarch 0 0]
    set nlayers [lindex $nnarch 1]
    set layers [lrange $nnarch 2 [expr 1 + $nlayers]]
    set ioports [lindex $nnarch [expr 2 + $nlayers]]
    set rest [lrange $nnarch [expr 3 + $nlayers] end]

    #puts "ioports: [lindex $nnarch [expr 2 + $nlayers]]"
    array set iop $ioports
    set inputDim $iop(idim)
    set inputRep $iop(irep)
    set outputDim $iop(odim)
    set outputRep $iop(orep)

    if {$inputRep == 1} {
	# Old style for scalar inputs
	switch -exact $ninputs {
	    3 { set inputlabels {"r(k)" "e(k)" "∫e(k)"} }
	    2 { set inputlabels {"r(k)" "e(k)"} }
	    1 { set inputlabels {"e(k)"} }
	    default {
		set inputlabels {}
		for {set i 1} {$i <= $ninputs} {incr i} {
		    lappend inputlabels "${i}?(k)"
		}
	    }
	}
    } else {
	# New style
	if {$inputDim == 1} {
	    # Scalar inputs case
	    switch -exact $inputRep {
		3 { set inputlabels {"r(k)" "e(k)" "∫e(k)"} }
		2 { set inputlabels {"r(k)" "e(k)"} }
		1 { set inputlabels {"e(k)"} }
		default {
		    set inputlabels {}
		    for {set i 1} {$i <= $ninputs} {incr i} {
			lappend inputlabels "${i}?(k)"
		    }
		}
	    }
	} else {
	    # Vector inputs case
	    switch -exact $inputRep {
		3 { # r+e+Se
		    set inputlabels {}
		    for {set j 1} {$j <= $inputDim} {incr j} {
			lappend inputlabels "r[subscriptString $j](k)"
		    }
		    for {set j 1} {$j <= $inputDim} {incr j} {
			lappend inputlabels "e[subscriptString $j](k)"
		    }
		    for {set j 1} {$j <= $inputDim} {incr j} {
			lappend inputlabels "∫e[subscriptString $j](k)"
		    }
		}
		2 { # r+e
		    set inputlabels {}
		    for {set j 1} {$j <= $inputDim} {incr j} {
			lappend inputlabels "r[subscriptString $j](k)"
		    }
		    for {set j 1} {$j <= $inputDim} {incr j} {
			lappend inputlabels "e[subscriptString $j](k)"
		    }
		}
		1 { # e
		    set inputlabels {}
		    for {set j 1} {$j <= $inputDim} {incr j} {
			lappend inputlabels "r[subscriptString $j](k)"
		    }
		    for {set j 1} {$j <= $inputDim} {incr j} {
			lappend inputlabels "e[subscriptString $j](k)"
		    }
		}
		default {
		    # undefined
		    set inputlabels {}
		    for {set i 1} {$i <= $inputRep} {incr i} {
			for {set j 1} {$j <= $inputDim} {incr j} {
			    lappend inputlabels "${i}[subscriptString $j]?(k)"
			}
		    }
		}
	    }
	}
    }

    # Strange case: outputs on input for controller - impossible
    # except for working in parallel with other kind of controller
    for {set i 0} {$i < $outputRep} {incr i} {
	if {$outputDim > 1} {
	    for {set j 1} {$j <= $outputDim} {incr j} {
		if {$i == 0} {
		    lappend inputlabels "u[subscriptString $j](k)"
		} else {
		    lappend inputlabels "u[subscriptString $j](k-$i)"
		}
	    }
	} else {
	    if {$i == 0} {
		lappend inputlabels "u(k)"
	    } else {
		lappend inputlabels "u(k-$i)"
	    }
	}
    }

    set newarch {}
    set outputlabels {}
    if {$outputDim > 1} {
	for {set j 1} {$j <= $outputDim} {incr j} {
	    lappend outputlabels "u[subscriptString $j]'(k)"
	}
    } else {
	lappend outputlabels "u'(k)"
    }

    lappend newarch [list $ninputs $inputlabels]
    lappend newarch $nlayers
    for {set i 0} {$i < $nlayers} {incr i} {
	if {$i == [expr $nlayers - 1]} {
	    # The last layer is an output one
	    lappend newarch [list [lindex $layers $i 0] [lindex $layers $i 1] \
				 $outputlabels]
	} else {
	    # Just copy
	    lappend newarch [lindex $layers $i]
	}
    }

    lappend newarch $ioports
    return "$newarch $rest"
}


proc NNCDisplayNeuralNetArch {p title nnFilePath} {
    set w $p.display_nnarch
    catch {destroy $w}
    toplevel $w

    wm title $w $title

    canvas $w.c -background white -width 500 -height 200
    pack $w.c -fill both -expand yes -side top

    button $w.close -text [mc "Close"] -command "destroy $w"
    ScreenshotButton $w $w.print_button $w.c [file dirname $nnFilePath] "nncarch"
    pack $w.print_button $w.close -side left

    DrawNeuralNetArch $w.c [NNCDecorateNNArch [ReadNeuralNetFile $nnFilePath]]
}
