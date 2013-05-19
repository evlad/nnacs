package provide par_file 1.0

# Assign all parameters set in params array to fields in $filepath
# saving structure of the file and comments.
proc ParFileAssign {filepath params} {
    upvar $params parArray

    file rename -force $filepath $filepath.bak
    if [catch {open $filepath.bak r} fd1] {
	error "Failed to read $filepath.bak"
	return
    }
    if [catch {open $filepath w} fd2] {
	error "Failed to create $filepath"
	close $fd1
	return
    }

    # Scan all lines of the file and substitute value from params if
    # name matches
    set fileContents [split [read $fd1] \n]
    close $fd1
    while {[llength $fileContents] > 0} {
	# Strip final empty lines
	if {[lindex $fileContents end] == {}} {
	    set fileContents [lrange $fileContents 0 end-1]
	} else {
	    break
	}
    }

    set lineNo 0
    set foundNames {}
    foreach line $fileContents {
	incr lineNo
	switch -regexp -- $line {
	    {^\s*#.*$} {
		# Comment line - leave it as is
	    }
	    {^\s*$} {
		# Empty line - leave it as is
	    }
	    {^\s*\w+\s*=.*$} {
		# Parameter name = value - lets try to find substitute
		if {[regexp {\s*(\w+)\s*=\s*(.*)$} $line input name value]} {
		    # Remember encountered name
		    lappend foundNames $name
		    if {[info exists parArray($name)]} {
			# Let's replace by new value
			set line "$name = $parArray($name)"
		    }
		}
	    }
	    default {
		# Something strange - leave it as is or produce warning
		puts stderr "$filepath:$lineNo: suspicious line '$line'"
	    }
	} 
	puts $fd2 $line
    }
    # Sort and remove possible duplicates among encountered names
    set foundNames [lsort -unique $foundNames]
    #puts "foundNames: $foundNames"
    #puts "arrayNames: [array names parArray]"

    # Let's add array items which were not found in the file
    foreach index [array names parArray] {
	set foundName [lsearch -sorted -increasing -inline $foundNames $index]
	if {$foundName == {}} {
	    # There is a name which was not found among names
	    # encountered in the parameter file
	    puts $fd2 "$index = $parArray($index)"
	}
    }

    close $fd2
    file delete $filepath.bak
}

# Fetch variables from file to given array params
proc ParFileFetch {filepath params} {
    upvar $params parArray

    if [catch {open $filepath r} fd] {
	error "Failed to read $filepath"
	return
    }

    # Scan all lines of the file and fetch parameter values
    set fileContents [split [read $fd] \n]
    close $fd
    while {[llength $fileContents] > 0} {
	# Strip final empty lines
	if {[lindex $fileContents end] == {}} {
	    set fileContents [lrange $fileContents 0 end-1]
	} else {
	    break
	}
    }
    set lineNo 0
    foreach line $fileContents {
	incr lineNo
	switch -regexp -- $line {
	    {^\s*#.*$} {
		# Comment line - leave it as is
	    }
	    {^\s*$} {
		# Empty line - leave it as is
	    }
	    {^\s*\w+\s*=.*$} {
		# Parameter name = value - lets try to find substitute
		if {[regexp {\s*(\w+)\s*=\s*(.*)$} $line input name value]} {
		    set parArray($name) $value
		}
	    }
	    default {
		# Something strange - leave it as is or produce warning
		puts stderr "$filepath:$lineNo: suspicious line '$line'"
	    }
	} 
    }
}


# Fetch one parameter value from given file and return it.
proc ParFileFetchParameter {filepath parname} {
    if [catch {open $filepath r} fd] {
	error "Failed to read $filepath"
	return {}
    }

    # Scan all lines of the file and fetch parameter values
    set fileContents [split [read $fd] \n]
    close $fd
    set parvalue {}
    foreach line $fileContents {
	switch -regexp -- $line {
	    {^\s*\w+\s*=.*$} {
		# Parameter name = value - lets try to find substitute
		if {[regexp {\s*(\w+)\s*=\s*(.*)$} $line input name value]} {
		    if {$parname == $name} {
			set parvalue $value
		    }
		}
	    }
	} 
    }
    return $parvalue
}


proc ParFileAssignTest {} {
    array set params {aa 177 dd "another text" ee "NEW TEXT"}

    file copy -force testdata/orig.test.par testdata/test.par
    ParFileAssign testdata/test.par params
}

proc ParFileFetchTest {parfile} {
    array set params {}

    ParFileFetch $parfile params
    puts [array get params]
}

#ParFileFetchTest testdata/orig.test.par
#ParFileAssignTest
#ParFileFetchTest testdata/test.par

# Etalon output
set TestOutput {
testdata/orig.test.par:6: suspicious line 'bla-bla-bla'
dd {very long text} bb 666 cc 909 aa 123
testdata/test.par:6: suspicious line 'bla-bla-bla'
foundNames: aa bb cc dd
arrayNames: dd ee aa
testdata/test.par:6: suspicious line 'bla-bla-bla'
dd {another text} bb 666 ee {NEW TEXT} cc 909 aa 177
}
