#!/bin/sh
# \
exec wish "$0" ${1+"$@"}

if {$tcl_platform(platform) == "windows"} {
    set NullDev "NUL"
} else {
    set NullDev "/dev/null"
}

encoding system utf-8

# Let's find system directory
if {![info exists env(NNACSSYSDIR)]} {
    # Not defined special place -> let's use the default one
    if {$tcl_platform(platform) == "windows"} {
	set SystemDirPath {C:\Program Files\NNACS}
    } else {
	set SystemDirPath [file join $env(HOME) nnacs]
    }
    puts "System directory: $SystemDirPath (use NNACSSYSDIR)"
} else {
    set SystemDirPath $env(NNACSSYSDIR)
    puts "System directory: $SystemDirPath (see NNACSSYSDIR)"
}

# Let's find scripts
set scriptsdir [file join $SystemDirPath scripts]
puts "Script directory: $scriptsdir"
pkg_mkIndex $scriptsdir
lappend auto_path $scriptsdir

set mainpath [file join $scriptsdir main.tcl]
if {[file exists $mainpath]} {
    source $mainpath
} else {
    error "$mainpath not found"
}

# End of file
