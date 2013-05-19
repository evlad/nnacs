#pkg_mkIndex .
#lappend auto_path .
package provide win_nnpedit 1.0

package require Tk
package require draw_nn

proc NNPEditSave {w filepath} {
    set f $w.nnarch
    upvar #0 $f.inputrep_var inputrep
    upvar #0 $f.outputrep_var outputrep
    upvar #0 $f.numlayers_var numlayers
    upvar #0 $f.numneurons1_var numneurons1
    upvar #0 $f.numneurons2_var numneurons2
    upvar #0 $f.numneurons3_var numneurons3
    upvar #0 $f.outputfunc_var outputfunc
    upvar #0 $f.inputlabels_var inputlabels
    upvar #0 $f.outputlabels_var outputlabels

    set args {}
    lappend args 1 $inputrep

    array set act {linear 0 tanh 1}
    lappend args 1 $outputrep 0 $act($outputfunc) $numlayers
    for {set iL 1} {$iL <= $numlayers} {incr iL} {
	eval lappend args \$numneurons$iL
    }
    set exefile [file join [SystemDir] bin MakeNN]
    puts "Run MakeNN: \"$exefile\" \"$filepath\" Plant $args"
    global NullDev
    catch {eval exec \"$exefile\" \"$filepath\" Plant $args >$NullDev} errCode
    if {$errCode != ""} {
	error $errCode
    }
}

proc NNPEditOk {w filepath} {
    NNPEditSave $w $filepath
    destroy $w
}

proc NNPEditDoResize {w} {
    global NNPEditDoResize_redo
    # To avoid redrawing the plot many times during resizing,
    # cancel the callback, until the last one is left.
    if { [info exists NNPEditDoResize_redo] } {
        after cancel ${NNPEditDoResize_redo}
    }
    set redo [after 50 "NNPEditDoPlot $w"]
}

proc NNPEditDoPlot {w} {
    set c $w.nnpicture.c
    set f $w.nnarch

    upvar #0 $f.inputrep_var inputrep
    upvar #0 $f.outputrep_var outputrep
    upvar #0 $f.numlayers_var numlayers
    upvar #0 $f.numneurons1_var numneurons1
    upvar #0 $f.numneurons2_var numneurons2
    upvar #0 $f.numneurons3_var numneurons3
    upvar #0 $f.outputfunc_var outputfunc
    upvar #0 $f.inputlabels_var inputlabels
    upvar #0 $f.outputlabels_var outputlabels

    # Common output of any neural controller
    set outputlabels {"y'(k+1)"}

    for {set i 0} {$i < $inputrep} {incr i} {
	if {$i == 0} {
	    set inputlabels "u(k)"
	} else {
	    lappend inputlabels "u(k-$i)"
	}
    }
    for {set i 0} {$i < $outputrep} {incr i} {
	if {$i == 0} {
	    lappend inputlabels "y(k)"
	} else {
	    lappend inputlabels "y(k-$i)"
	}
    }

    set inputs [expr $inputrep + $outputrep]
    set nnarch {}
    lappend nnarch [list $inputs $inputlabels]
    for {set i 1} {$i <= $numlayers} {incr i} {
	eval set numneurons \$numneurons$i
	lappend nnarch "$numneurons tanh"
    }
    lappend nnarch [list 1 $outputfunc $outputlabels]

    $c delete all
    # nnarch = {Inputs {HidNeurons1 HidType1} {HidNeurons2 HidType2} ...
    # {Outputs OutputType}}, where type is "linear" or "tanh"
    DrawNeuralNetArch $c $nnarch
}


proc NNPEditModified {w} {
    # Set attributes of modified text contents
    set modifiedFg white
    set modifiedBg red
    $w.buttons.ok configure -bg $modifiedBg -fg $modifiedFg \
	-activebackground $modifiedBg -activeforeground $modifiedFg
    $w.buttons.save configure -bg $modifiedBg -fg $modifiedFg \
	-activebackground $modifiedBg -activeforeground $modifiedFg
}

proc NNPEditFieldChange {w p} {
    if {![string is integer -strict $p]} {
	return false
    }
    after 200 "NNPEditDoPlot $w"
    return true
}

# f - widget where hidden neurons are listed
# p - value to validate and consider
proc NNPEditHiddenLayersChange {w p} {
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
    after 200 "NNPEditDoPlot $w"
    return true
}

# w - parent
# title - text to show
# filepath - name of variable where to store filename
proc NNPEditWindow {p title filepath} {
    set w $p.textedit
    catch {destroy $w}
    toplevel $w
    wm title $w $title

    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.ok -text "OK" -command "NNPEditOk $w \"$filepath\""
    button $w.buttons.save -text "Сохранить" -command "NNPEditSave $w \"$filepath\""
    button $w.buttons.cancel -text "Отмена" -command "destroy $w"
    pack $w.buttons.ok $w.buttons.save $w.buttons.cancel -side left -expand 1

    set f [frame $w.nnarch]
    pack $f -side left -fill y -padx 0 -anchor n

    # Setup variables
    global $f.inputrep_var $f.outputrep_var $f.numlayers_var \
	$f.numneurons1_var $f.numneurons2_var $f.numneurons3_var \
	$f.outputfunc_var

    set nnarch {}
    if {[file exists $filepath]} {
	set nnarch [ReadNeuralNetFile $filepath]
    }
    if {$nnarch == {}} {
	# Default parameters
	set $f.inputrep_var 2
	set $f.outputrep_var 3
	set $f.numlayers_var 2
	set $f.numneurons1_var 7
	set $f.numneurons2_var 5
	set $f.numneurons3_var 5
	set $f.outputfunc_var "linear"
    }
    set $f.inputlabels_var {i1 i2 i3 i4 i5 i6 i7 i8 i9}
    set $f.outputlabels_var {o1 o2 o3 o4 o5 o6 o7 o8 o9}

    grid [label $f.inputrep_l -text "Входы (с повтором)" -justify left] \
	[spinbox $f.inputrep -from 1 -to 100 -width 4 -validate key \
	     -justify right -textvariable $f.inputrep_var \
	     -vcmd "NNPEditFieldChange $w %P"]
    grid [label $f.outputrep_l -text "Выходы (с повтором)" -justify left] \
	[spinbox $f.outputrep -from 1 -to 100 -width 4 -validate key \
	     -justify right -textvariable $f.outputrep_var \
	     -vcmd "NNPEditFieldChange $w %P"]
    grid [label $f.numlayers_l -text "Скрытые слои" -justify left] \
	[spinbox $f.numlayers -from 0 -to 3 -width 4 -validate key \
	     -textvariable $f.numlayers_var -justify right]
    upvar 0 $f.numlayers_var numlayers
    foreach i {1 2 3} {
	grid [label $f.numneurons${i}_l -text "Скрытый слой №$i" \
		  -justify left] \
	    [spinbox $f.numneurons${i} -from 1 -to 100 -width 4 -validate key \
		 -justify right -textvariable $f.numneurons${i}_var \
		 -vcmd "NNPEditFieldChange $w %P"]
	if {$numlayers < $i} {
	    $f.numneurons${i}_l configure -state disabled
	    $f.numneurons${i} configure -state disabled
	}
    }
    $f.numlayers configure -vcmd "NNPEditHiddenLayersChange $w %P"
    grid [labelframe $f.outputfunc -text "Выходная функция"] \
	-column 0 -columnspan 2 -sticky we

    radiobutton $f.outputfunc.linear -text "Линейная" \
	-variable $f.outputfunc_var -value "linear" -justify left \
	-command "NNPEditDoPlot $w"
    radiobutton $f.outputfunc.sigmoid -text "Сигмоида" \
	-variable $f.outputfunc_var -value "tanh" -justify left \
	-command "NNPEditDoPlot $w"
    pack $f.outputfunc.linear $f.outputfunc.sigmoid -side top -fill x

    foreach i "$f.inputrep $f.outputrep $f.numlayers 
$f.numneurons1 $f.numneurons2 $f.numneurons3" {
	bind $i <<Modified>> "NNPEditModified $w"
    }

    grid columnconfigure $f 0 -weight 1
    grid columnconfigure $f 1 -weight 3

    set c $w.nnpicture.c
    frame $w.nnpicture
    grid [canvas $c -background white -width 400 -height 200] -sticky news
    grid columnconfigure $w.nnpicture 0 -weight 1
    grid rowconfigure $w.nnpicture 0 -weight 1
    pack $w.nnpicture -expand yes -fill both -side right

    bind $c <Configure> "NNPEditDoResize $w"

#    $f.text edit modified 0
#    bind $f.text <<Modified>> \
#	"NNPEditModified $w %W ; TextSyntaxHighlight $ftype %W"
}

# test
#NNPEditWindow "" "Text editor title" "testdata/nnp_ini.nn"
