package provide win_rtseries 1.0

package require Tk
package require Plotchart
package require universal
package require screenshot


# p - parent widget;
# title - window title;
# rtreader - procedure to take one more time sample of data as list;
# rtparam - common named parameters (timeLen,[winWidth,winHeight,deltaTime,yMin,yMax,yStep,yScale,stopCmd,getCmd,timeLabel,workDir])
# rtseries - list of runtime series labels;
proc RtSeriesWindow {p title rtreader rtparam rtseries} {
    set w $p.rtseries
    catch {destroy $w}
    toplevel $w
    wm title $w $title

    array set par $rtparam

    global $w.stop_flag
    set $w.stop_flag 0

    set width [setdef $rtparam winWidth 600 ]
    set height [setdef $rtparam winHeight 300 ]

    set c $w.plotter
    pack [canvas $c -background white -width $width -height $height \
	      -borderwidth 3] \
	-expand yes -fill both -side top

    # Use infoline panel as a bottom of the main canvas (with tag INFO)
    set il $c

    ScreenshotButton $w $w.print_button $c [setdef $rtparam workDir [pwd]] "rtplot"

    button $w.stop_button -text "Остановить" \
	-command "RtSeriesStop $w \"$par(stopCmd)\" ; set $w.stop_flag 1"
    pack $w.print_button $w.stop_button -side left

    set timeLen [setdef $rtparam timeLen 1000]
    set timeGrid [expr pow(10,int(0.3+log10($timeLen)))/2]
    set timeLabel [setdef $rtparam timeLabel "Time:"]

    if {"log" == [setdef $rtparam yScale "linear"]} {
	set yMin [setdef $rtparam yMin 1e-3]
	set yMax [setdef $rtparam yMax 1e+3]
	set s [::Plotchart::createXLogYPlot $c "0.0 $timeLen $timeGrid" \
		   "$yMin $yMax 1"]
    } else {
	# Linear by default
	set yMin [setdef $rtparam yMin 0]
	set yMax [setdef $rtparam yMax 1.5]
	set yStep [setdef $rtparam yMax 0.5]
	set s [::Plotchart::createXYPlot $c "0.0 $timeLen $timeGrid" \
		   "$yMin $yMax $yStep"]
    }

    # Palette a'la Gnuplot
    set colors {red green blue magenta cyan yellow black brown grey violet}
    set j 0
    foreach rts $rtseries {
	$s dataconfig series$j -colour [lindex $colors $j]
	$s legend series$j $rts
	incr j
    }

    set dt [setdef $rtparam deltaTime 1]
    set i 0
    set tcur 0.0

    update
    while {[set $w.stop_flag] == 0 && {} != [set input [eval $rtreader]]} {
	#puts "$i# $input"
	update
	# Info line composition
	$il delete INFO
	set xtext 10
	set ytext [expr [$il cget -height] - 10] ; #$fontm(-ascent)
	set msg [format "%s%6g" $timeLabel $tcur]
	set ilt [$il create text $xtext $ytext -text $msg -anchor nw -font fixed -tags INFO]
	set ilfont [$il itemcget $ilt -font]
	incr xtext [expr [font measure $ilfont $msg] + 15]

	# Series drawing and info line update
	set j 0
	foreach rts $rtseries {
	    set val [lindex $input $j]
	    #puts "$i: $j $val"
	    if {$val != {}} {
		$s plot series$j $tcur $val
		set msg "$rts: "
		$il create text $xtext $ytext -text $msg -anchor nw \
		    -fill [lindex $colors $j] -font $ilfont -tags INFO
		incr xtext [font measure $ilfont $msg]
		set msg [format "%-6.3e" $val]
		$il create text $xtext $ytext -text $msg -anchor nw -font $ilfont -tags INFO
		incr xtext [expr [font measure $ilfont $msg] + 15]
	    }
	    incr j
	}
	incr i
	set tcur [expr {$tcur + $dt}]
    }
    $w.stop_button configure -text "Закрыть" -command "destroy $w"
    # reader returned {} or break occured
}


# Call given command cmd to stop processing.
proc RtSeriesStop {w cmd} {
    eval $cmd
    # The next button event will close the window
    $w.stop_button configure -text "Закрыть" -command "destroy $w"
}


# Call to stop process
proc RtSeriesTestStopCmd {} {
    puts "RtSeriesTestStopCmd"
}

# parindex - parameter index (0,1,2...);
# parname - parameter's name (label);
# input - result of the reader;
# Returns fetched value.
proc RtSeriesTestGetValue {parindex parname input} {
    return [lindex $input $parname]
}

# Return: list of input data
proc RtSeriesTestReader {} {
    # Delay just for tests
    after 50
    gets stdin line
    return [lrange [split $line] 1 end]
}

proc RtSeriesTest {} {
    #canvas .c -width 400 -height 250
    #pack .c

    set w ""
    #set c $w.graphics.c
    #frame $w.graphics
    #grid [canvas $c -background white -width 600 -height 300] -sticky news
    #grid columnconfigure $w.graphics 0 -weight 1
    #grid rowconfigure $w.graphics 0 -weight 1
    #pack $w.graphics -expand yes -fill both -side top

    #set c $w.c
    #pack [canvas $c -background white -width 600 -height 300] \
#	-expand yes -fill both -side top
    #button $w.stop -text "Остановить" -command "RtSeriesStop {} {puts STOP}"
    #pack $w.stop

    set fname "testdata/nncp_trace.dat"
    set params {
	winWidth 500
	winHeight 250
	timeLen 1000
	deltaTime 5
	stopCmd {puts Stop}
	yMin 0
	yMax 1.5
	yStep 0.5
	yScale linear ; # maybe "linear" or "log"
    }
    set series {
	LearnME LearnSDE LearnMSE TestME TestSDE TestMSE NormLearnMSE NormTestMSE
    }
    RtSeriesWindow {} "RtSeriesTest" RtSeriesTestReader $params $series
    puts Finish
}

#wish win_rtseries.tcl <testdata/nncp_trace.dat
#pkg_mkIndex .
#lappend auto_path .
#RtSeriesTest
