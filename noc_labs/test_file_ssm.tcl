# For example:
# { states 2 inputs 1 outputs 2
#   a {{1 0.5} {0 2}} b {0 1} c {{1 0} {0 1}} d {0 0} x0 {0 0} }

# Load object from given text taken from file.
# - this - object internal data (array with keys) - result;
# - flines - file contents to parse line by line (.ssm format).
proc SSModelLoadConfig {thisvar flines} {
    upvar $thisvar this

    set lineNo 0

    # Setup resulting variable
    set this(states) 0
    set this(inputs) 0
    set this(outputs) 0
    set this(a) {}
    set this(b) {}
    set this(c) {}
    set this(d) {}
    set this(x0) {}

    # Setup state of reading
    set dimensionsLine 0
    set aMatrixLine 0
    set bMatrixLine 0
    set cMatrixLine 0
    set dMatrixLine 0
    set x0VectorLine 0

    # For all lines in the text passed
    foreach line $flines {
	incr lineNo

	# Let's skip comments and empty lines
	set cindex [string first ";" $line]
	if {$cindex == 0} {
	    set line ""
	}
	if {$cindex > 0} {
	    set line [string range $line 0 [expr $cindex - 1]]
	}
	set line [string trim $line]
	if {[string length $line] > 0} {
	    # Some fields may be in the line
	    set fields {}
	    foreach f [split $line] {
		if {$f != {}} {
		    lappend fields $f
		}
	    }
	    if {$cindex == -1} {
		##puts "$lineNo (no comment): [join $fields]"
	    } else {
		##puts "$lineNo (comment at $cindex): [join $fields]"
	    }

	    if {$dimensionsLine == 0} {
		incr dimensionsLine
		set this(states) [lindex $fields 0]
		set this(outputs) [lindex $fields 1]
		set this(inputs) [lindex $fields 2]
	    } elseif {$aMatrixLine < $this(states)} {
		incr aMatrixLine
		set aRow {}
		for {set i 0} {$i < $this(states)} {incr i} {
		    lappend aRow [lindex $fields $i]
		}
		lappend this(a) $aRow
	    } elseif {$bMatrixLine < $this(states)} {
		incr bMatrixLine
		set bRow {}
		for {set i 0} {$i < $this(inputs)} {incr i} {
		    lappend bRow [lindex $fields $i]
		}
		lappend this(b) $bRow
	    } elseif {$cMatrixLine < $this(outputs)} {
		incr cMatrixLine
		set cRow {}
		for {set i 0} {$i < $this(states)} {incr i} {
		    lappend cRow [lindex $fields $i]
		}
		lappend this(c) $cRow
	    } elseif {$dMatrixLine < $this(outputs)} {
		incr dMatrixLine
		set dRow {}
		for {set i 0} {$i < $this(inputs)} {incr i} {
		    lappend dRow [lindex $fields $i]
		}
		lappend this(d) $dRow
	    } elseif {$x0VectorLine == 0} {
		incr x0VectorLine
		set x0Row {}
		for {set i 0} {$i < $this(states)} {incr i} {
		    lappend x0Row [lindex $fields $i]
		}
		set this(x0) $x0Row
	    }
	}
    }
}

# Parse text of the section and return tagged list of contents
# considering the format has given version.
proc SSModelParseSection {type name stext {version "1.0"}} {
    switch -exact $type {
	StateSpaceModel {
	    array set ssm {}
	    set ssm(name) $name
	    set ssm(version) $version
	    SSModelLoadConfig ssm $stext
	    return [array get ssm]
	}
	default {
	    puts "Undefined SSModel section $type: skipping it"
	    return {}
	}
    }
}

# Parse content of given file and return tagged list with all sections
# listed.
#set ssmlist {
#    name "ssm1" version "1.0" states 2 inputs 1 outputs 2
#    a {{1 0.5} {0 2}} b {0 1} c {{1 0} {0 1}} d {0 0} x0 {0 0}
#}
proc SSModelParseFile {filePath} {
    if [ catch {open $filePath} fd ] {
	error "Failed to open $filePath: $fd"
    }
    set ftext [split [read $fd] \n]
    set fmagic [split [lindex $ftext 0]]
    close $fd

    # Parse magic
    if {[lindex $fmagic 0] != ";NeuCon" ||
	[lindex $fmagic 1] != "StateSpaceModel"} {
	error "Bad $filePath file identification line"
    }
    array set ssm {}
    set ssmVersion [lindex $fmagic 2]

    set sections {}
    set iniSection {}
    foreach line [lrange $ftext 1 end] {
	#puts "$line"
	# let's find start of section []
	if [regexp {^\[\s*([^\s]+)\s*([^\s]*)\s*\]} $line all type name] {
	    # [$type $name]
	    if {$iniSection != {}} {
		# let's process previous section
		set s [SSModelParseSection $sectionType $sectionName $iniSection $ssmVersion]
		lappend sections $sectionName "$s"
		set iniSection {}
	    }
	    set sectionType $type
	    set sectionName $name
	} else {
	    # any other line
	    if {$sectionType != {}} {
		lappend iniSection "$line"
	    }
	    # else
	    #   skip lines before 1st section
	}
    }
    # let's process the last section
    set s [SSModelParseSection $sectionType $sectionName $iniSection $ssmVersion]
    lappend sections $sectionName "$s"

    if {2 != [llength $sections]} {
	error "Wrong format of $filePath - one section is expected only"
    }
    return [lindex $sections 1]
}

proc SSModelComposeFile {fd ssmContents} {
    array set ssm $ssmContents
    set fmagic ";NeuCon StateSpaceModel 1.0"
    set ssmHeadComment {;  State space discrete time model:
;    x(t+1) = A*x(t) + B*u(t)
;    y(t)   = C*x(t) + D*u(t)
;  Matrices: A(N,N), B(N,K), C(M,N), D(M,K)
;    N - state vector, M - outputs, K - inputs}
    puts $fd $fmagic
    puts $fd "\[StateSpaceModel\]"
    puts $fd $ssmHeadComment
    set N $ssm(states)
    set M $ssm(outputs)
    set K $ssm(inputs)
    puts $fd "$N $M $K\t; Dimensions N M K"
    puts $fd ";  Matrix A($N,$N), [expr $N * $N] numbers:"
    for {set i 0} {$i < $N} {incr i} {
	for {set j 0} {$j < $N} {incr j} {
	    puts -nonewline $fd " [format %8g [lindex $ssm(a) $i $j]]"
	}
	puts $fd ""
    }
    puts $fd ";  Matrix B($N,$K), [expr $N * $K] numbers:"
    for {set i 0} {$i < $N} {incr i} {
	for {set j 0} {$j < $K} {incr j} {
	    puts -nonewline $fd " [format %8g [lindex $ssm(b) $i $j]]"
	}
	puts $fd ""
    }
    puts $fd ";  Matrix C($M,$N), [expr $M * $N] numbers:"
    for {set i 0} {$i < $M} {incr i} {
	for {set j 0} {$j < $N} {incr j} {
	    puts -nonewline $fd " [format %8g [lindex $ssm(c) $i $j]]"
	}
	puts $fd ""
    }
    puts $fd ";  Matrix D($M,$K), [expr $M * $K] numbers:"
    for {set i 0} {$i < $M} {incr i} {
	for {set j 0} {$j < $K} {incr j} {
	    puts -nonewline $fd " [format %8g [lindex $ssm(d) $i $j]]"
	}
	puts $fd ""
    }
    puts $fd ";  Initial state x(0), $N numbers:"
    for {set i 0} {$i < $N} {incr i} {
	puts -nonewline $fd " [format %8g [lindex $ssm(x0) $i]]"
    }
    puts $fd ""
}


#set ssmlist [SSModelParseFile "../test_mimo/plant_8x2x2.ssm"]
set ssmlist [SSModelParseFile "test_in.ssm"]
puts $ssmlist
set fd [open "test_out.ssm" "w"]
SSModelComposeFile $fd $ssmlist
