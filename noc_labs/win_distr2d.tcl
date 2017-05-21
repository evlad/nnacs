package provide win_distr2d 1.0

package require Tk
package require Plotchart
package require data_file
package require universal
package require screenshot


# Calculate grid step by simple procedure
proc Distr2DGridStep {min max} {
    if {$min == $max} {
	return 1.0
    }
    return [ expr pow(10, int(log10(abs($max - $min)))) ]
}

# Take on input minimum and maximum values on input as well as grid
# step.  Changes grid step to reasonable value (to have at least 4
# grid lines) and changes minimum and maximum values accordingly to
# place them at grid point.  Ticks for grid are calculated too.
proc Distr2DAxis {minIn maxIn gridIn minOut maxOut gridOut ticksOut} {
    upvar $minOut min  $maxOut max  $gridOut grid  $ticksOut ticks

    #puts "min=$minIn max=$maxIn grid=$gridIn -> [expr int(abs($maxIn - $minIn) / $grid)]"
    set grid $gridIn
    if { [expr int(abs($maxIn - $minIn) / $grid)] < 4 } {
	set grid [expr $grid * 0.5]
    }
    if { [expr int(abs($maxIn - $minIn) / $grid)] < 4 } {
	set grid [expr $grid * 0.5]
    }
    
    set min [expr {$grid * int($minIn / $grid)}]
    if { $min > $minIn } {
	set min [expr $min - $grid]
    }
    set max [expr {$grid * int($maxIn / $grid)}]
    if { $max < $maxIn } {
	set max [expr $max + $grid]
    }
    #puts "mm: $minIn .. $maxIn -> $min .. $max"
    set ticks {}
    for { set i 0 } { [expr $i * $grid + $min] <= $max } { incr i } {
	set tick [expr $i * $grid + $min]
	if { $min <= $tick && $tick <= $max } {
	    lappend ticks $tick
	}
    }
}

# Returns xmin xmax ymin ymax
proc Distr2DMinMax {dataSeries} {
    set xmin 0
    set xmax 0
    for { set iS 0 } { $iS < [llength $dataSeries] } { incr iS } {
	# find X range
	set iDim 0
	if { [info exists xmin] } {
	    if { [lindex $dataSeries $iS $iDim 1 0] < $xmin } {
		set xmin [lindex $dataSeries $iS $iDim 1 0]
	    }
	} else {
	    set xmin [lindex $dataSeries $iS $iDim 1 0]
	}
	if { [info exists xmax] } {
	    if { [lindex $dataSeries $iS $iDim 1 1] > $xmax } {
		set xmax [lindex $dataSeries $iS $iDim 1 1]
	    }
	} else {
	    set xmax [lindex $dataSeries $iS $iDim 1 1]
	}
	# find Y range considering min,max pair presents
	set iDim 1
	if { [info exists ymin] } {
	    if { [lindex $dataSeries $iS $iDim 1 0] < $ymin } {
		set ymin [lindex $dataSeries $iS $iDim 1 0]
	    }
	} else {
	    set ymin [lindex $dataSeries $iS $iDim 1 0]
	}
	if { [info exists ymax] } {
	    if { [lindex $dataSeries $iS $iDim 1 1] > $ymax } {
		set ymax [lindex $dataSeries $iS $iDim 1 1]
	    }
	} else {
	    set ymax [lindex $dataSeries $iS $iDim 1 1]
	}
    }
    return [list $xmin $xmax $ymin $ymax]
}

proc Distr2DPlot {c} {
    global $c.props
    upvar #0 $c.props props

    #puts "Distr2DPlot $c"

    if { ! [info exists props(dataSeries)] } return
    set dataSeries $props(dataSeries)

    puts "Number of series: [llength $dataSeries]"
    if {[llength $dataSeries] == 0 } return

    # xmin,xmax,ymin,ymax - outer bounding box of data series
    # $c.view_* - view area

    unlist [Distr2DMinMax $dataSeries] xmin xmax ymin ymax

    #puts "outer x: $xmin $xmax"
    #puts "outer y: $ymin $ymax"

    # Store bounds of view area
    foreach v {xmin xmax ymin ymax} {
	global $c.view_$v
	if {![info exists $c.view_$v] || [set $c.view_$v] == {}} {
	    #puts "assign: set $c.view_$v \$$v"
	    eval set $c.view_$v \$$v
	}
    }

    #puts "view x: $xmin $xmax"
    #puts "view y: $ymin $ymax"

    # Copy xmin,xmax,ymin,ymax to props since they are needed to make
    # ZoomAll action
    foreach v {xmin xmax ymin ymax} {
	eval set props($v) \$$v
    }

    #eval set ${v}grid [Distr2DGridStep \$$c.view_${v}min \$$c.view_${v}max]
    set xgrid [Distr2DGridStep [set $c.view_xmin] [set $c.view_xmax]]
    set ygrid [Distr2DGridStep [set $c.view_ymin] [set $c.view_ymax]]

    if {[set $c.view_xmin] == [set $c.view_xmax]} {
	set $c.view_xmin [expr [set $c.view_xmin] - $xgrid]
	set $c.view_xmax [expr [set $c.view_xmax] + $xgrid]
    }

    if {[set $c.view_ymin] == [set $c.view_ymax]} {
	set $c.view_ymin [expr [set $c.view_ymin] - $ygrid]
	set $c.view_ymax [expr [set $c.view_ymax] + $ygrid]
    }

    #puts "xgrid: $xgrid"
    #puts "ygrid: $ygrid"
    set xticks {}
    set yticks {}
    Distr2DAxis [set $c.view_xmin] [set $c.view_xmax] $xgrid \
	xmin xmax xgrid xticks
    Distr2DAxis [set $c.view_ymin] [set $c.view_ymax] $ygrid \
	ymin ymax ygrid yticks

    # Store slightly changed view limits back
    foreach v {xmin xmax ymin ymax} {
	eval set \$c.view_$v \$$v
    }

    #puts "plot x: $xmin $xmax"
    #puts "plot y: $ymin $ymax"

    # ticks to grid nodes
    set xgrid_matrix {}
    set ygrid_matrix {}
    foreach y $yticks {
	lappend xgrid_matrix $xticks
	set yrow {}
	foreach x $xticks {
	    lappend yrow $y
	}
	lappend ygrid_matrix $yrow
    }
    #puts ${xgrid_matrix}
    #puts ${ygrid_matrix}

    global $c.bDrawLegend $c.bDrawGrid
    upvar #0 $c.bDrawLegend bDrawLegend $c.bDrawGrid bDrawGrid

    set pixwidth [winfo width $c]
    # Palette a'la Gnuplot
    set colors {red green blue magenta cyan yellow black brown grey violet}
    set s [::Plotchart::createXYPlot $c \
	       [list $xmin $xmax $xgrid] \
	       [list $ymin $ymax $ygrid]]

    # line or rectangle - black anyway
    #$s legendconfig -legendtype line

    # $xmin $xmax
    if { $bDrawGrid } {
	$s grid ${xgrid_matrix} ${ygrid_matrix}
    }
    set xstep 1.0
    for { set iS 0 } { $iS < [llength $dataSeries] } { incr iS } {
	# Draw series iS
	set iColor [expr {$iS % [llength $colors]}]
	$s dotconfig series$iS -colour [lindex $colors $iColor] \
	    -scalebyvalue off -outline off
	###$s dataconfig series$iS -colour [lindex $colors $iColor]
	if { $bDrawLegend } {
	    set xname [lindex $dataSeries $iS 0 2 0]
	    set yname [lindex $dataSeries $iS 1 2 0]
	    $s legend series$iS "$yname/$xname"
	}

	# considering the screen has limited number of pixels it's not
	# wise to draw too much data points
	#set iStep [expr int([llength [lindex $dataSeries $iS 0]] / $pixwidth) - 1]
	#if { $iStep <= 0 } {
	    set iStep 1
	#}
	#puts "iSeries=$iS -> plotting step $iStep"
	for { set i 0 } \
	    { $i < [llength [lindex $dataSeries $iS 0 0]] && \
		  $i < [llength [lindex $dataSeries $iS 1 0]] } \
	    { incr i $iStep } {
		$s dot series$iS \
		    [lindex $dataSeries $iS 0 0 $i] \
		    [lindex $dataSeries $iS 1 0 $i] \
		    0
	    }
	#puts "Plotted $i points"
    }
}

# Force external redraw
proc Distr2DRedraw {p} {
    set c $p.grseries.graphics.c
    Distr2DDoPlot $c
}


proc Distr2DDoPlot {c} {
    # Clean up the contents (see also the note below!)
    $c delete all

    # (Re)draw
    Distr2DPlot $c
}

proc Distr2DDoResize {c} {
    global Distr2DDoResize_redo
    # To avoid redrawing the plot many times during resizing,
    # cancel the callback, until the last one is left.
    if { [info exists Distr2DDoResize_redo] } {
        after cancel ${Distr2DDoResize_redo}
    }
    set redo [after 50 "Distr2DDoPlot $c"]
}


# Add series in format {data} or {{data}{min max}{name}}.
# - name is a label to display in legend.
# - utag is an unique tag which allows to prevent addition of the series
# with the same tag.
# Return index of the added series.  -1 means failure.
proc Distr2DAddSeriesXY {p xseries yseries {xname ""} {yname ""} {utag ""}} {
    if {[llength $xseries] == 0 || [llength $yseries] == 0} {
	puts "Can not add empty series"
	return -1
    }

    #puts "series: $series"
    #puts "name: $name"

    set c $p.grseries.graphics.c

    global $c.props $c.utags $c.utagCounter
    upvar #0 $c.props props $c.utags utags

    if {$utag != "" && [info exists utags($utag)]} {
	puts "Can not add twice the same series $utag"
	return -1
    }
    if {$utag == ""} {
	# Let's generate utag automatically
	if {![info exists $c.utagCounter]} {
	    set $c.utagCounter 0
	}
	set utag "$c.utag[incr $c.utagCounter]"
    }

    if {[info exists props(dataSeries)]} {
	set retindex [llength $props(dataSeries)]
    } else {
	set retindex 0
    }

    if {[llength $xseries] > 1 && [llength [lindex $xseries 0]] > 1 &&
	[llength [lindex $xseries 1]] >= 2} {
	# Consider the series has format {{data}{min max}...}
	set series1 $xseries
	if {$xname != ""} {
	    lset series1 2 $xname
	}
    } else {
	set series1 $xseries
	lappend series1 [Distr2DSeriesMinMax $xseries] $xname
    }

    if {[llength $yseries] > 1 && [llength [lindex $yseries 0]] > 1 &&
	[llength [lindex $yseries 1]] >= 2} {
	# Consider the series has format {{data}{min max}...}
	set series2 $yseries
	if {$yname != ""} {
	    lset series2 2 $yname
	}
    } else {
	set series2 $yseries
	lappend series2 [Distr2DSeriesMinMax $yseries] $yname
    }

    lappend props(dataSeries) [list $series1 $series2]

    set utags($utag) $retindex
    return $retindex
}

proc Distr2DSeriesMinMax {series} {
    # Let's find min and max
    set vmin [lindex $series 0]
    set vmax [lindex $series 0]
    foreach v $series {
	if { $v < $vmin } {
	    set vmin $v
	}
	if { $v > $vmax } {
	    set vmax $v
	}
    }
    return [list $vmin $vmax]
}

proc Distr2DViewAll {c args} {
    global $c.props
    upvar #0 $c.props props
    set dim $args
    if { $dim == {} } {
	set dim {x y}
    }
    foreach v $dim {
	#eval puts \"${v} range: \$props(${v}min) \$props(${v}max)\"
	if {[info exists props(${v}min)] && [info exists props(${v}max)]} {
	    global $c.view_${v}min
	    global $c.view_${v}max
	    set $c.view_${v}min $props(${v}min)
	    set $c.view_${v}max $props(${v}max)
	}
	# else
	#   After the first plot they will be assigned
    }
    #Distr2DDoPlot $c
}

proc Distr2DDestroy {w} {
    set c $w.graphics.c

    global $c.props $c.utags $c.utagCounter
    array unset $c.props
    array unset $c.utags
    unset $c.utagCounter

    foreach v {xmin xmax ymin ymax} {
	global $c.view_$v
	unset $c.view_$v
    }

    global $c.bDrawLegend $c.bDrawGrid
    unset $c.bDrawLegend $c.bDrawGrid
}

# Return index of added series or -1 in case of failure.
proc Distr2DAddFile {p workDir {filePath ""}} {
    set w $p.grseries
    if {$filePath != ""} {
	if {![file exists $filePath]} {
	    error "Failed to open $filePath"
	    return -1
	}
    } else {
	set dataFileTypes {
	    { "Файлы данных" {.dat} }
	    { "Все файлы" * }
	}
	set filePath [fileSelectionBox $w open [file join $workDir ""] $dataFileTypes]
	if {$filePath == ""} {
	    # User cancel
	    return -1
	}
    }
    set wholeData [Distr2DReadFile $filePath]
    set label [SessionRelPath $workDir $filePath]
    set retindex [Distr2DAddSeries $p "[lindex $wholeData 0]" $label]
    Distr2DRedraw $p
    return $retindex
}


# Check whether graphics series window exist for given parent.
proc Distr2DCheckPresence {p} {
    set w $p.grseries
    if {[catch {$w cget -menu} rc]} {
	# It's not a toplevel or does not exist
	return 0
    } else {
	# There is a toplevel with such name
	return 1
    }
}

# View the whole plot in both dimensions, x and y.
proc Distr2DZoomAll {p} {
    set c $p.grseries.graphics.c
    Distr2DViewAll $c x
    Distr2DViewAll $c y
    Distr2DDoPlot $c
}


# p - parent widget
# title - window title
# path - first data file or working directory
proc Distr2DWindow {p title {path ""} {extproc ""}} {
    #set dataByColumns = ReadSeries $filepath
    set w $p.grseries
    catch {destroy $w}
    toplevel $w
    wm title $w $title

    # Determine work directory for screenshots
    if {$path == ""} {
	set workDir [pwd]
	set filepath $path
    } elseif {[file isdirectory $path]} {
	set workDir $path
	set filepath ""
    } else {
	set workDir [file dirname $path]
	set filepath $path
    }

    if {$extproc != {}} {
	$extproc create $p
    }

    set c $w.graphics.c
    frame $w.graphics
    grid [canvas $c -background white -width 500 -height 500] -sticky news
    grid columnconfigure $w.graphics 0 -weight 1
    grid rowconfigure $w.graphics 0 -weight 1
    pack $w.graphics -expand yes -fill both -side top

    global $c.props
    upvar #0 $c.props props
    if {![array exists props]} {
	array set props {}
    }
    if { $filepath != "" } {
	set props(dataSeries) [Distr2DReadFile $filepath]
    }
    #puts "Create $c.props: [array get props]"

    # Array of unique tags
    global $c.utags
    if {![array exists $c.utags]} {
	array set $c.utags {}
    }

    # Counter of undefined utags
    global $c.utagCounter
    set $c.utagCounter 0

    global $c.bDrawLegend
    set $c.bDrawLegend 1

    global $c.bDrawGrid
    set $c.bDrawGrid 1

    bind $c <Configure> "Distr2DDoResize $c"

    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m

    ScreenshotButton $w $w.buttons.print $c $workDir "grplot"

    set m $w.buttons.curves.m
    menubutton $w.buttons.curves -text [mc "Series"] \
	-direction below -menu $m -relief raised
    menu $m -tearoff 0
    $m add command -label [mc "Add..."] \
	-command "Distr2DAddFile $p \"$workDir\""
    # To implement Delete action one needs to implement associative
    # indexing of data series based on utag.  Also,
    # Distr2DUpdateSeries must be considered.
    # GUI part may start with next code:
    #$m add cascade -label [mc "Remove"] -menu $m.delete
    #set d [menu $m.delete -tearoff 0]
    #$m.delete add command -label "$label" -command "Distr2DDeleteSeries $p \"$utag\""

    set o $w.buttons.options
    frame $o
    checkbutton $o.grid -text [mc "Grid"] \
	-variable $c.bDrawGrid -command "Distr2DDoPlot $c"
    checkbutton $o.legend -text [mc "Legend"] \
	-variable $c.bDrawLegend -command "Distr2DDoPlot $c"
    global xmin xmax ymin ymax
    button $o.xlabel -text "X:" -command "Distr2DViewAll $c x" -pady 0
    button $o.ylabel -text "Y:" -command "Distr2DViewAll $c y" -pady 0
    foreach v {xmin xmax ymin ymax} {
	global $c.view_$v
	#upvar 0 $c.view_$v view_$v
	entry $o.$v -textvariable $c.view_$v -width 8 -relief sunken
    }
    hint $o.xlabel "Press to set the whole X range"
    hint $o.ylabel "Press to set the whole Y range"

    # after entries to make exact focus order by Tab/Shift-Tab
    button $w.buttons.redraw -text [mc "Update"] -command "Distr2DDoPlot $c"
    button $w.buttons.close -text [mc "Close"] \
	-command "Distr2DDestroy $w ; destroy $w"

    grid $o.grid $o.xlabel $o.xmin $o.xmax -sticky w
    grid $o.legend $o.ylabel $o.ymin $o.ymax -sticky w

    pack $w.buttons.print $w.buttons.curves $o $w.buttons.redraw \
	$w.buttons.close -side left -expand 1
}

proc Distr2DTest {} {
    set r1data [DataSeriesReadFile testdata/r1.dat]
    set u1data [DataSeriesReadFile testdata/u1.dat]

    set c3data [DataSeriesReadFile testdata/c3.dat]

    Distr2DAddSeriesXY "" "[lindex $r1data 0]" "[lindex $u1data 0]" "r" "u"
    Distr2DAddSeriesXY "" "[lindex $c3data 0]" "[lindex $c3data 1]" "c1" "c2"
    Distr2DWindow "" "2D distribution plot"
}
