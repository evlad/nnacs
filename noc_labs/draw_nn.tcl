package provide draw_nn 1.0

package require Tk
package require universal

# Return architecture in format acceptable for DrawNeuralNetArch
proc ReadNeuralNetFile {filepath} {
    if [ catch {open $filepath r} fd ] {
	error "Failed to open $filepath: $fd"
	return
    }
    # read all lines
    set contents [split [read -nonewline $fd] \n]
    close $fd

    set limits {}
    foreach line $contents {
	if [regexp {^\s*;} $line match] {
	    # comment - let's skip it
	    continue
	}
	if [regexp {^\s*\[\s*NeuralNet\s*[^\]]*\]} $line match] {
	    if {[info exists nnName]} {
		# error
		puts stderr "Not the only NeuralNet in file!"
		return
	    }
	    regexp {^\s*\[\s*NeuralNet\s+([^\]]*)\]} $line match nnName
	    set lineNo 0
	    continue
	}
	if {![info exists nnName]} {
	    # before [NeuralNet] anything is allowed
	    continue
	}
	# let's count lines inside the section
	incr lineNo
	switch -exact $lineNo {
	    1 {
		regexp {^\s*(\d+)\s+(\d+)} $line match nInputs nInpRep
	    }
	    2 {
		regexp {^\s*(\d+)} $line match nOutRep
	    }
	    3 {
		regexp {^\s*(\d+)} $line match nFeedback
	    }
	    4 {
		regexp {^\s*(\d+)} $line match nHidLayers
		set iHidLayer 0
		set listLayers {}
	    }
	    default {
		if {[info exists iHidLayer]} {
		    if {$iHidLayer < $nHidLayers} {
			regexp {^\s*(\d+)} $line match num
			lappend listLayers "$num tanh"
			incr iHidLayer
		    } else {
			unset iHidLayer
		    }
		}
		if {$lineNo == [expr $nHidLayers + 5]} {
		    regexp {^\s*(\w+)\s+(\d+)} $line match eOutType nOutputs
		} else {
		    # matching for two floating point numbers (min max)
		    if {[regexp {^\s*[-+]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][-+]?[0-9]+)?\s+[-+]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][-+]?[0-9]+)?} $line match]} {
			lappend limits "$match"
		    } else {
			#puts "not matched: $line"
		    }
		}
	    }
	}
    }
    if {[info exists nnName] && [info exists nInputs] && \
	    [info exists nInpRep] && \
	    [info exists nOutRep] && [info exists nFeedback] && \
	    [info exists nHidLayers] && [info exists listLayers] && \
	    [info exists eOutType] && [info exists nOutputs]} {
	set nnArch "[expr $nInputs * $nInpRep + $nOutputs * $nOutRep]"
	lappend nnArch [expr $nHidLayers + 1]
	lappend listLayers "$nOutputs $eOutType"
	set nnArch "$nnArch $listLayers"
	lappend nnArch "idim $nInputs irep $nInpRep odim $nOutputs orep $nOutRep name $nnName"
	lappend nnArch "$limits"

	#puts "readnn: $nnArch"
	return $nnArch
    }
    puts stderr "Wrong NeuralNet format!"
    return {}
}

# nnarch = { { Inputs [InputLabels] }
#            nLayers
#            { HidNeurons1 HidType1 }
#            { HidNeurons2 HidType2 }
#            ...
#            { Outputs OutputType [OutputLabels] }
#            [ {
#                idim nInputs
#                irep nInpReps
#                odim nOutputs
#                orep nOutReps
#                name NNName
#              }
#              [ { {min max}
#                  ... 
#                }
#              ]
#            ]
#          },
# where type is "linear" or "tanh" and InputLabels
# or/and OutputLabels may be absent.
# For exmaple:
# 3 3 {5 tanh} {3 tanh} {1 linear} {idim 1 irep 1 odim 1 orep 2 name Plant}
# {{-1 1} {-1 1} {-1 1} {-1 1}}
proc DrawNeuralNetArch {c nnarch} {
    set totalW [winfo width $c]
    set totalH [winfo height $c]
    if { $totalW <= 1 && $totalH <= 1} {
	# Just for tests because method above gives totalW=1 totalH=1
	set totalW [$c cget -width]
	set totalH [$c cget -height]
    }
    # Base point to shift the picture
    set x 0
    set y 2
    # To prevent neurons slightly to go out of picture
    set totalH [expr $totalH - $y * 2]

    #puts "drawnn: $nnarch"

    set inputs [lindex $nnarch 0 0]
    set inputLabels [lindex $nnarch 0 1]
    set nlayers [lindex $nnarch 1]
    #puts "nlayers: $nlayers"
    set layers [lrange $nnarch 2 [expr 1 + $nlayers]]
    #puts "layers: $layers"
    set outputLayer [lindex $layers end]
    set outputLabels [lindex $outputLayer 2]

    if {[llength $nnarch] > [expr 2 + $nlayers]} {
	#puts "ioports: [lindex $nnarch [expr 2 + $nlayers]]"
	array set ioports [lindex $nnarch [expr 2 + $nlayers]]
	set inputDim $ioports(idim)
	set inputRep $ioports(irep)
	set outputDim $ioports(odim)
	set outputRep $ioports(orep)
    }

    # Total number of limits
    set nLimits [expr $inputs + [lindex $outputLayer 0]]
    set limits {}
    if {[llength $nnarch] > [expr 3 + $nlayers]} {
	set limits [lindex $nnarch [expr 3 + $nlayers]]
	#puts "custom limits: $limits"
    }
    if {[llength $limits] < $nLimits} {
	# Prepare default limits (fill rest)
	for {set i [llength $limits]} {$i < $nLimits} {incr i} {
	    lappend limits {-1 1}
	}
    }
    #puts "final limits: $limits"

    # Distance in pixels between end of line and label
    set labelHorOffset 3

    # Distance in pixels between horizontal line and min/max label
    set limitVerOffset 3

    set MaxNeuronsInLayer $inputs
    foreach layer $layers {
	set num [lindex $layer 0]
	if {$num > $MaxNeuronsInLayer} {
	    set MaxNeuronsInLayer $num
	}
    }

    # Calculate size of neuron (and distance between them)
    if {$MaxNeuronsInLayer == 1} {
	set NeuronSize [expr int($totalH / 8)]
    } else {
	set NeuronSize [expr int($totalH / (2 * $MaxNeuronsInLayer - 1))]
    }
    set HalfNS [expr int($NeuronSize / 2)]

    # Calculate distance between layers
    set LayersDist [expr int($totalW / (2 + $nlayers))]
    #set LayersDist [expr int($totalW / (2 + [llength $layers]))]
    set HalfLD [expr int($LayersDist / 2)]

    # Make new list of layers considering input one with special type
    # of nodes: dots
    set layers [linsert $layers 0 "$inputs dot"]

    # Counter for limits
    set iLimit 0

    # Draw lines
    set iL 0
    foreach layer $layers {
	set num [lindex $layer 0]
	set type [lindex $layer 1]
	set yL [expr $y + ($totalH - $NeuronSize * (2 * $num - 1)) / 2]
	if {![info exists prevNum]} {
	    # Inputs
	    for {set iN 0} {$iN < $num} {incr iN} {
		set finalX [expr $x + $LayersDist]
		set finalY [expr $yL + $HalfNS + 2 * $iN * $NeuronSize]
		$c create line $HalfLD $finalY $finalX $finalY
		#$c create oval [expr $finalX - 2] [expr $finalY - 2] \
		#    [expr $finalX + 2] [expr $finalY + 2] -fill black
		set label [lindex $inputLabels $iN]
		if {$label != ""} {
		    $c create text [expr $HalfLD - $labelHorOffset] $finalY \
			-justify right -anchor e -text $label
		}
		set minLimit [lindex $limits $iLimit 0]
		$c create text [expr $HalfLD + $labelHorOffset] \
		    [expr $finalY + $limitVerOffset] \
		    -justify left -anchor nw -fill "DarkBlue" -text $minLimit
		set maxLimit [lindex $limits $iLimit 1]
		$c create text [expr $HalfLD + $labelHorOffset] \
		    [expr $finalY - $limitVerOffset] \
		    -justify left -anchor sw -fill "DarkRed" -text $maxLimit
		incr iLimit
	    }
	} else {
	    set yP [expr $y + ($totalH - $NeuronSize * (2 * $prevNum - 1)) / 2]
	    for {set iP 0} {$iP < $prevNum} {incr iP} {
		for {set iN 0} {$iN < $num} {incr iN} {
		    set startX [expr $x + $iL * $LayersDist]
		    set startY [expr $yP + $HalfNS + 2 * $iP * $NeuronSize]
		    set finalX [expr $x + ($iL + 1) * $LayersDist]
		    set finalY [expr $yL + $HalfNS + 2 * $iN * $NeuronSize]
		    $c create line $startX $startY $finalX $finalY
		    #$c create oval \
		#	[expr $finalX - $HalfNS] [expr $finalY - $HalfNS] \
		#	[expr $finalX + $HalfNS] [expr $finalY + $HalfNS]
		}
	    }
	}
	incr iL
	set prevNum $num
    }
    # Ouputs
    for {set iN 0} {$iN < $prevNum} {incr iN} {
	set yL [expr $y + ($totalH - $NeuronSize * (2 * $num - 1)) / 2]
	set startX [expr $x + $iL * $LayersDist]
	set startY [expr $yL + $HalfNS + 2 * $iN * $NeuronSize]
	$c create line $startX $startY [expr $startX + $HalfLD] $startY
	set label [lindex $outputLabels $iN]
	if {$label != ""} {
	    $c create text [expr $startX + $HalfLD + $labelHorOffset] $startY \
		-justify left -anchor w -text $label
	}
	set minLimit [lindex $limits $iLimit 0]
	$c create text [expr $startX + $HalfLD - $labelHorOffset] \
	    [expr $finalY + $limitVerOffset] \
	    -justify left -anchor ne -fill "DarkBlue" -text $minLimit
	set maxLimit [lindex $limits $iLimit 1]
	$c create text [expr $startX + $HalfLD - $labelHorOffset] \
	    [expr $finalY - $limitVerOffset] \
	    -justify left -anchor se -fill "DarkRed" -text $maxLimit
	incr iLimit
    }

    # Draw neurons
    unset prevNum
    set iL 0
    foreach layer $layers {
	set num [lindex $layer 0]
	set type [lindex $layer 1]
	set yL [expr $y + ($totalH - $NeuronSize * (2 * $num - 1)) / 2]
	for {set iN 0} {$iN < $num} {incr iN} {
	    set finalX [expr $x + ($iL + 1) * $LayersDist]
	    set finalY [expr $yL + $HalfNS + 2 * $iN * $NeuronSize]
	    switch -exact $type {
		dot {
		    $c create oval [expr $finalX - 2] [expr $finalY - 2] \
			[expr $finalX + 2] [expr $finalY + 2] -fill black
		}
		tanh {
		    $c create oval \
			[expr $finalX - $HalfNS] [expr $finalY - $HalfNS] \
			[expr $finalX + $HalfNS] [expr $finalY + $HalfNS] \
			-fill white
		    $c create line $finalX $finalY \
			$finalX [expr $finalY - int($HalfNS/2)] \
			[expr $finalX + int(2*$HalfNS/3)] \
			[expr $finalY - int($HalfNS/2)] \
			-smooth bezier -fill brown
		    $c create line $finalX $finalY \
			$finalX [expr $finalY + int($HalfNS/2)] \
			[expr $finalX - int(2*$HalfNS/3)] \
			[expr $finalY + int($HalfNS/2)] \
			-smooth bezier -fill brown
		}
		linear {
		    $c create oval \
			[expr $finalX - $HalfNS] [expr $finalY - $HalfNS] \
			[expr $finalX + $HalfNS] [expr $finalY + $HalfNS] \
			-fill white
		    $c create line \
			[expr $finalX - int($HalfNS/2)] \
			[expr $finalY + int($HalfNS/2)] \
			[expr $finalX + int($HalfNS/2)] \
			[expr $finalY - int($HalfNS/2)] \
			-fill blue
		}
	    }
	}
	incr iL
    }
}

proc DisplayNeuralNetArch {p title nnFilePath} {
    set w $p.display_nnarch
    catch {destroy $w}
    toplevel $w

    wm title $w $title

    button $w.close -text "Закрыть" -command "destroy $w"
    canvas $w.c -width 500 -height 250
    pack $w.close -side bottom -expand 1
    pack $w.c -fill both -expand yes
    DrawNeuralNetArch $w.c [ReadNeuralNetFile $nnFilePath]
}

proc TestDrawNeuralNetArch {{nnarch {6 {7 "tanh"} {4 "tanh"} {3 "linear"}}}} {
    canvas .c -width 400 -height 200
    #canvas .c -width 800 -height 500
    pack .c -fill both -expand yes
    #puts "[winfo width .c]"
    #while {[winfo width .c] == 1} {
    #    # wait
    #}
    DrawNeuralNetArch .c $nnarch
}

#TestDrawNeuralNetArch
#TestDrawNeuralNetArch [ReadNeuralNetFile testdata/test.nn]
#TestDrawNeuralNetArch {{8 {i1 i2 i3 i4 i5 i6 i7 i8}} {4 "tanh"} {1 "linear" {o1}}}

#set nna1 [ReadNeuralNetFile "/home/evlad/labworks/SumError1/010/nncSe_ini.nn"]
#puts "nna1: $nna1"
#set nna2 [DecorateNNCArch $nna1]
#puts "nna2: $nna2"
#TestDrawNeuralNetArch $nna2

#set nna1 [ReadNeuralNetFile "/home/evlad/labworks/SumError1/013/nnp_ini.nn"]
#puts "nna1: $nna1"
#set nna2 [DecorateNNPArch $nna1]
#puts "nna2: $nna2"
#TestDrawNeuralNetArch $nna2

# Decorate labels as for general (abstract) NN.
# Takes on input an NN architecture without labels.
# Return architecture in format acceptable for DrawNeuralNetArch.
proc ANNDecorateNNArch {nnarch} {
    #puts "nnarch: $nnarch"
    set ninputs [lindex $nnarch 0 0]
    set nlayers [lindex $nnarch 1]
    set layers [lrange $nnarch 2 [expr 1 + $nlayers]]
    #puts "layers: $layers"
    set rest [lrange $nnarch [expr 2 + $nlayers] end]
    #puts "rest: $rest"
    array set props [lindex $rest 0]

    set inputDim $props(idim)
    set inputRep $props(irep)
    set outputDim $props(odim)
    set outputRep $props(orep)

    set inputlabels {}
    for {set i 0} {$i < $inputRep} {incr i} {
	if {$inputDim > 1} {
	    for {set j 1} {$j <= $inputDim} {incr j} {
		if {$i == 0} {
		    lappend inputlabels "x[subscriptString $j](k)"
		} else {
		    lappend inputlabels "x[subscriptString $j](k-$i)"
		}
	    }
	} else {
	    if {$i == 0} {
		lappend inputlabels "x(k)"
	    } else {
		lappend inputlabels "x(k-$i)"
	    }
	}
    }
    for {set i 0} {$i < $outputRep} {incr i} {
	if {$outputDim > 1} {
	    for {set j 1} {$j <= $outputDim} {incr j} {
		if {$i == 0} {
		    lappend inputlabels "y[subscriptString $j](k)"
		} else {
		    lappend inputlabels "y[subscriptString $j](k-$i)"
		}
	    }
	} else {
	    if {$i == 0} {
		lappend inputlabels "x(k)"
	    } else {
		lappend inputlabels "x(k-$i)"
	    }
	}
    }

    set newarch {}
    set outputlabels {}
    if {$outputDim > 1} {
	for {set j 1} {$j <= $outputDim} {incr j} {
	    lappend outputlabels "y[subscriptString $j]'(k)"
	}
    } else {
	lappend outputlabels "y'(k)"
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
    #puts "newarch: $newarch"
    return "$newarch $rest"
}
