# build
package require Tk

proc ProgressBarCreate {w} {
    canvas $w.c -width 200 -height 20 -bd 1 -relief groove -highlightt 0
    $w.c create rectangle 0 0 0 20 -tags bar -fill navy
    pack $w.c -padx 10 -pady 10 -side top
}

button .reset -text "Reset" -command "ProgressBarReset"
button .run -text "Run" -command "Run"

ProgressBarCreate ""
pack .reset .run -side left

#focus -force .c
#raise .c

proc Run {} {
    for {set i 0} {$i < 101} {incr i} {
	ProgressBarSet $i
	after 100
	update
    }
}

proc ProgressBarReset {} {
    ProgressBarSet 0
}

proc ProgressBarSet {percent} {
    .c coords bar 0 0 [expr {int($percent * 2)}] 20
}
