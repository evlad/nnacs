# { name "ssm1" states 2 inputs 1 outputs 2
#   a {{1 0.5} {0 2}} b {0 1} c {{1 0} {0 1}} d {0 0} x0 {0 0} }

# Load object from given text taken from file.
# - this - object internal data (array with keys) - result;
# - ftext - file contents to parse (.ssm format).
proc SSModelLoadConfig {thisvar ftext} {
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
    foreach line [split $ftext \n] {
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
		puts "$lineNo (no comment): [join $fields]"
	    } else {
		puts "$lineNo (comment at $cindex): [join $fields]"
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

	# Let's try to find key in the line
	#puts "key_pos: $key_pos"
#	foreach {key pos} $key_pos {
	    # Key presents somewhere after ;
#	    set cindex [lsearch -exact $fields ";"]
#	    if {$cindex > 0} {
#		# There is a comment, let's find all keywords
#		if {[lsearch -exact [lrange $fields $cindex end] $key] > 0} {
#		    set this($key) [lindex $fields $pos]
#		    #puts "[lindex $fields $pos] ==> $key=$this($key)"
#		} 
#	    }
#	}
    }
}

set ftext {;  State space discrete time model:
;    x(t+1) = A*x(t) + B*u(t)
;    y(t)   = C*x(t) + D*u(t)
;  Matrices: A(N,N), B(N,K), C(M,N), D(M,K)
;    N - state vector, M - outputs, K - inputs
2 2 1    ; Dimensions N M K
;  Matrix A(2,2), 4 numbers:
 1 0.5
 0 2
;  Matrix B(2,1), 2 numbers:
 0
 1
;  Matrix C(2,2), 4 numbers:
 1 0
 0 1
;  Matrix D(2,1), 2 numbers:
 0
 0
;  Initial state x(0):
 0 0
}

#puts $ftext

array set dst {name "test"}
SSModelLoadConfig dst $ftext

puts [array get dst]
