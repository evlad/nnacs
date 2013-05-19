package provide draw_prim 1.0

package require Tk

# Oval for adding or subtracting signals depending minusps.
# It may be "none" for adding and "n", "s", "w", "e" for subtracting
# from given direction.
proc DrawGather {c name x y minusps} {
    set hsize 10
    set x1 [expr [$c canvasx $x] - $hsize]
    set y1 [expr [$c canvasy $y] - $hsize]
    set x2 [expr [$c canvasx $x] + $hsize]
    set y2 [expr [$c canvasy $y] + $hsize]
    $c create oval $x1 $y1 $x2 $y2 -tags $name
    if { $minusps == "n" } {
	$c create arc $x1 $y1 $x2 $y2 -start 45 -extent 90 -style pieslice \
	    -fill black
    } else {
	$c create arc $x1 $y1 $x2 $y2 -start 45 -extent 90 -style pieslice
    }
    if { $minusps == "s" } {
	$c create arc $x1 $y1 $x2 $y2 -start 225 -extent 90 -style pieslice \
	    -fill black
    } else {
	$c create arc $x1 $y1 $x2 $y2 -start 225 -extent 90 -style pieslice
    }
    if { $minusps == "e" } {
	$c create arc $x1 $y1 $x2 $y2 -start 315 -extent 90 -style pieslice \
	    -fill black
    }
    if { $minusps == "w" } {
	$c create arc $x1 $y1 $x2 $y2 -start 135 -extent 90 -style pieslice \
	    -fill black
    }
}

proc DrawLargeBlock {c name label x y} {
    button $c.$name -text $label -padx 1m -pady 1m
    set font [option get $c.$name fontBlock ""]
    if { $font != "" } {
	$c.$name config -font $font
    }
    $c create window $x $y -window $c.$name -tags $name
}

proc DrawSmallBlock {c name label x y} {
    button $c.$name -text $label -padx 1m -pady 0
    set font [option get $c.$name fontBlock ""]
    if { $font != "" } {
	$c.$name config -font $font
    }
    $c create window $x $y -window $c.$name -tags $name
}

proc DrawImageBlock {c name img x y} {
    set w [image width $img]
    set h [image height $img]
    set i [$c create image $x $y -image $img]
    set xp [lindex [$c coords $i] 0]
    set yp [lindex [$c coords $i] 1]
    $c create rect [expr $xp - $w / 2 - 2] [expr $yp - $h / 2 - 2] \
	[expr $xp + $w / 2 + 2] [expr $yp + $h / 2 + 2] -tags $name
}

# Convert given bounding box to point specification:
#  n - north (center of the top side)
#  s - north (center of the bottom side)
#  w - west (center of the left side)
#  e - east (center of the right side)
#  nw, ne, sw, se - corners are also available
# Returns {x y}
proc BBoxSpecPoint {bb ps} {
    set xw [lindex $bb 0]
    set yn [lindex $bb 1]
    set xe [lindex $bb 2]
    set ys [lindex $bb 3]
    if { $ps == "n" || $ps == "nw" || $ps == "ne" } {
	set y $yn
    } elseif { $ps == "s" || $ps == "sw" || $ps == "se" } {
	set y $ys
    }
    if { $ps == "w" || $ps == "nw" || $ps == "sw" } {
	set x $xw
    } elseif { $ps == "e" || $ps == "ne" || $ps == "se" } {
	set x $xe
    }

    if { $ps == "n" || $ps == "s" } {
	    set x [expr ($xw + $xe) / 2]
    }
    if { $ps == "w" || $ps == "e" } {
	set y [expr ($yn + $ys) / 2]
    }
    return [list $x $y]
}

# Draw arrow from one to another point.
# ps1, ps2 - point specification.  May be "n", "s", "w", "e" as well
# as "[ns][we]".
# dirflag - arrow flag for direct arrow link and 6 additional values:
# - "hor" and "ver", which mean arrow should go to target point
# horizontally (vertically) the first;
# - "horOnly" and "verOnly", which mean arrow should go to target
# point horizontally (vertically) only and avoiding another direction
# at all;
# - "horMiddle" and "verMiddle" allow to draw three segments with
# horizontal or vertical the middle one.  Coordinate of the middle
# segment may be given as additional parameter with name midCoord,
# otherwise it calculated as middle between end points.
# Named args:
# - "midCoord" - coordinate of middle segment (horMiddle, verMiddle);
# - "color" - color of the line and arrow.
proc DrawDirection {c block1 ps1 block2 ps2 dirflag {namedArgs {}}} {
    set bb1 [$c bbox $block1]
    set bb2 [$c bbox $block2]
    set p1 [BBoxSpecPoint $bb1 $ps1]
    set p2 [BBoxSpecPoint $bb2 $ps2]
    array set args $namedArgs
    if {[info exists args(color)]} {
	set color $args(color)
    } else {
	set color "black"
    }
    switch -exact $dirflag {
	"horOnly" {
	    DrawArrow $c [lindex $p1 0] [lindex $p1 1] \
		[lindex $p2 0] [lindex $p1 1] last $color
	}
	"horMiddle" {
	    if {[info exists args(midCoord)]} {
		set ymid $args(midCoord)
	    } else {
		set ymid [expr ( [lindex $p1 1] + [lindex $p2 1] ) / 2]
	    }
	    DrawArrow $c [lindex $p1 0] [lindex $p1 1] \
		[lindex $p1 0] $ymid last $color
	    DrawArrow $c [lindex $p1 0] $ymid \
		[lindex $p2 0] $ymid middle $color
	    DrawArrow $c [lindex $p2 0] $ymid \
		[lindex $p2 0] [lindex $p2 1] last $color
	}
	"hor" {
	    DrawArrow $c [lindex $p1 0] [lindex $p1 1] \
		[lindex $p2 0] [lindex $p1 1] middle $color
	    DrawArrow $c [lindex $p2 0] [lindex $p1 1] \
		[lindex $p2 0] [lindex $p2 1] middle $color
	}
	"verOnly" {
	    DrawArrow $c [lindex $p1 0] [lindex $p1 1] \
		[lindex $p1 0] [lindex $p2 1] last $color
	}
	"verMiddle" {
	    if {[info exists args(midCoord)]} {
		set xmid $args(midCoord)
	    } else {
		set xmid [expr ( [lindex $p1 0] + [lindex $p2 0] ) / 2]
	    }
	    DrawArrow $c [lindex $p1 0] [lindex $p1 1] \
		$xmid [lindex $p1 1] last $color
	    DrawArrow $c $xmid [lindex $p1 1] \
		$xmid [lindex $p2 1] middle $color
	    DrawArrow $c $xmid [lindex $p2 1] \
		[lindex $p2 0] [lindex $p2 1] last $color
	}
	"ver" {
	    DrawArrow $c [lindex $p1 0] [lindex $p1 1] \
		[lindex $p1 0] [lindex $p2 1] middle $color
	    DrawArrow $c [lindex $p1 0] [lindex $p2 1] \
		[lindex $p2 0] [lindex $p2 1] middle $color
	}
	default {
	    DrawArrow $c [lindex $p1 0] [lindex $p1 1] \
		[lindex $p2 0] [lindex $p2 1] $dirflag $color
	}
    }
}

# arrflag - may be "none", "first", "last" and "middle";
# color - may be default (black).
proc DrawArrow {c x1 y1 x2 y2 arrflag {color "black"}} {
    if {$color == {}} {
	set color "black"
    }
    if { $arrflag == "middle" } {
	$c create line $x1 $y1 $x2 $y2 -arrow none -fill $color
	set midx [expr ([$c canvasx $x1] + 2 * [$c canvasx $x2]) / 3 ]
	set midy [expr ([$c canvasy $y1] + 2 * [$c canvasy $y2]) / 3 ]
	$c create line $x1 $y1 $midx $midy -arrow last -fill $color
    } else {
	$c create line $x1 $y1 $x2 $y2 -arrow $arrflag -fill $color
    }
}
