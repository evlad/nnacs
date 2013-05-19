package provide data_file 1.0

# Read data series file into {{{col1}{min1
# max1}{name1}}...{{colN}{minN maxN}{nameN}}} resulting data
# structure.
proc GrSeriesReadFile {filepath} {
    if [ catch {open $filepath r} fd ] {
	error "Failed to open $filepath: $fd"
	return
    }
    # read all lines
    set contents [split [read -nonewline $fd] \n]
    close $fd

    set fname [file tail $filepath]
    puts "Reading data file $filepath:"

    # let's find number of columns in the file
    set line0 [lindex $contents 0]
    set tail $line0
    set numCol 0
    # parse first numeric field and get the rest part of line
    #puts "$tail"
    #while { [regexp {(\-?\d+(\.\d*)?([eE][+-]?\d+)?)\s+(.*)$} $tail match ] }
    set minmax {}
    while { [regexp {\s*(\-?\d+(\.\d*)?([eE][+-]?\d+)?)(.*)$} $tail match \
		 fpnum frac exp rest] } {
	set resData($numCol) {}
	incr numCol
	set tail $rest
	lappend minmax {}
	#puts "$numCol: fpnum=$fpnum frac=$frac exp=$exp rest=\"$rest\""
	#if [ expr $numCol > 10 ] {
	#    break
	#}
    }
    puts ">> number of columns: $numCol"
    set numRow 0
    foreach line $contents {
	set tail $line
	set iCol 0
	#puts "row=$numRow: $tail"
	while { [regexp {\s*(\-?\d+(\.\d*)?([eE][+-]?\d+)?)(.*)$} $tail match \
		     fpnum frac exp rest] } {
	    #set fNum [expr 1.0 * $fpnum]
	    scan $fpnum "%g" fNum
	    if { $iCol < $numCol } {
		lappend resData($iCol) $fNum
		if { {} == [lindex $minmax $iCol] } {
		    lset minmax $iCol "$fNum $fNum"
		} else {
		    if { $fNum < [lindex $minmax $iCol 0] } {
			lset minmax $iCol 0 $fNum
		    }
		    if { $fNum > [lindex $minmax $iCol 1] } {
			lset minmax $iCol 1 $fNum
		    }
		}
	    }
	    #puts "col=$iCol"
	    incr iCol
	    set tail $rest
	}
	while { $iCol < $numCol } {
	    # Fill absent values
	    lappend resData($iCol) [expr 0.0]
	    incr iCol
	}
	incr numRow
    }
    puts ">> number of rows: $numRow ([array names resData])"
    set resList {}
    for {set iCol 0} {$iCol < $numCol} {incr iCol} {
	lappend resList [list $resData($iCol) [lindex $minmax $iCol] \
			     $fname#$iCol ]
    }
    #puts $resList
    return $resList
}
