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
    puts "System directory: $SystemDirPath (use NNACSSYSDIR to override)"
} else {
    set SystemDirPath $env(NNACSSYSDIR)
    puts "System directory: $SystemDirPath (taken from NNACSSYSDIR)"
}

# Let's configure dynamic linkage
if {$tcl_platform(platform) == "windows"} {
    # Nothing since common libraries are in bin
} else {
    if {![info exists env(LD_LIBRARY_PATH)]} {
	set env(LD_LIBRARY_PATH) [file join $SystemDirPath lib]
      } else {
	set env(LD_LIBRARY_PATH) \
	  "[file join $SystemDirPath lib]:$env(LD_LIBRARY_PATH)"
      }
}
if {![info exists env(NAEXFDIR)]} {
    # Not defined: produce from NNACSSYSDIR
    set env(NAEXFDIR) [file join $SystemDirPath lib exfuncs]
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
