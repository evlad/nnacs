pkg_mkIndex .
lappend auto_path .

source win_cofunc.tcl

proc CoFuncTest {} {
    global NullDev
    global tcl_platform
    global env
    global SystemDirPath

    if {$tcl_platform(platform) == "windows"} {
	set NullDev "NUL"
    } else {
	set NullDev "/dev/null"
    }

    # Let's find system directory
    if {![info exists env(NNACSSYSDIR)]} {
	# Not defined special place -> let's use the default one
	if {$tcl_platform(platform) == "windows"} {
	    set SystemDirPath {C:\Program Files\NNACS}
	} else {
	    set SystemDirPath [file join $env(HOME) nnacs]
	}
    } else {
	set SystemDirPath $env(NNACSSYSDIR)
    }
#    global SystemDirPath

    # Let's find scripts
    #set scriptsdir [file join $SystemDirPath scripts]
    #puts "Script directory: $scriptsdir"
    #pkg_mkIndex $scriptsdir
    #lappend auto_path $scriptsdir
    encoding system utf-8

    array set cof [CoFuncParseFile testdata/test.cof]
    #foreach n [array names cof] {
    #  puts "$n: $cof($n)"
    #}
    set changed [CoFuncEditor {} cof]
    puts "Changed=$changed; Result=[array get cof]"
}
