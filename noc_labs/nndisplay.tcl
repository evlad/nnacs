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
    #puts "System directory: $SystemDirPath (use NNACSSYSDIR to override)"
} else {
    set SystemDirPath $env(NNACSSYSDIR)
    #puts "System directory: $SystemDirPath (taken from NNACSSYSDIR)"
}

# Let's find scripts
set scriptsdir [file join $SystemDirPath scripts]
#puts "Script directory: $scriptsdir"
pkg_mkIndex $scriptsdir
lappend auto_path $scriptsdir

package require getoptbyrs
package require draw_nn
package require win_nnpedit
package require win_nncedit

#puts "argc: $argc"
#puts "argv: $argv"

### Parse command line

# Optional: level of verbosity
global bVerbose
set bVerbose [getopt argv --verbose]
if {$bVerbose != 0} {
    puts "Going to verbose mode."
}

# Optional: help message
set bHelp [getopt argv --help]
if {$bHelp || [llength $argv] == 0} {
    puts {Display neural network architecture described in file passed by.
Syntax: nndisplay [Options...] [<File.nn>]
Options:
  --help                - this help message
  --nnfile|-f <File.nn> - path to the neural network description file
  --nntype|-t <Type>    - type of neural network: auto,plant,controller,general
  --width <PixWidth>    - width of window to display neural network schema
  --height <PixHeight>  - height of window to display neural network schema
  --inlabel <InName>    - name of the input signal (x by default)
  --outlabel <OutName>  - name of the output signal (y by default)
  --instartindex <Num>  - start index of input subscripts (1 by default)
  --outstartindex <Num> - start index of output subscripts (1 by default)
  --scalers [on|off]    - show scalers (on by default)
  --verbose             - produce verbose output}
    exit 0
}

# System environment:
if {$bVerbose} {
    if {![info exists env(NNACSSYSDIR)]} {
	puts "System directory: $SystemDirPath (use NNACSSYSDIR to override)"
    } else {
	puts "System directory: $SystemDirPath (taken from NNACSSYSDIR)"
    }
    puts "Script directory: $scriptsdir"
}

# Optional: name of the input signal
getopt argv (--inlabel) inlabel "x"

# Optional: name of the input signal
getopt argv (--outlabel) outlabel "y"

# Optional: start index of input subscripts
getopt argv (--instartindex) instartindex 1

# Optional: start index of output subscripts
getopt argv (--outstartindex) outstartindex 1


# Optional: show scalers
getopt argv (--scalers) scalers "on"
switch -exact $scalers {
    on {
      set showScalers 1
    }
    off {
      set showScalers 0
    }
    default {
	puts "Wrong argument to show scalers or not; on or off are expected"
	exit 2
    }
}


# Optional: type of neural network to display
getopt argv (-t|--nntype) nnType auto
switch -exact $nnType {
    general {
	# general neural network: just inputs and and outputs
	if {$bVerbose} {
	    puts "General neural network"
	}
    }
    auto {
	# automatic detection
	if {$bVerbose} {
	    puts "Try to interpret neural network type automatically"
	}
    }
    controller {
	# interpret as a contoller considering inputs in such manner
	if {$bVerbose} {
	    puts "Try to interpret neural network as a controller"
	}
    }
    plant {
	# interpret as a plant model considering inputs in such manner
	if {$bVerbose} {
	    puts "Try to interpret neural network as a plant model"
	}
    }
    default {
	puts "Wrong or undefined type of neural network!"
	puts "auto, general, plant, controller are allowed"
	exit 2
    }
}

# Optional: width of window (pixels)
getopt argv --width winWidth 500

# Optional: height of window (pixels)
getopt argv --height winHeight 200

# Obligatory: file to neural network file
if {[getopt argv (-f|--nnfile) nnFilePath ""]} {
    # Nothing
} elseif {[llength $argv] > 0} {
    set nnFilePath [lindex $argv 0]
    set argv [lrange $argv 1 end]
} else {
    puts "Neural network file path is not defined!"
    exit 1
}

if {$bVerbose} {
    puts "Neural network file to display: $nnFilePath"
}
set nnArch [ReadNeuralNetFile $nnFilePath]
if {$bVerbose} {
    puts "Neural network architecture: $nnArch"
}

# Try to determine type of neural network by its name
if {$nnType == "auto"} {
    # automatic detection
    set nLayers [lindex $nnArch 1]
    set nnPropsList [lindex $nnArch [expr $nLayers + 2]]
    array set nnProps $nnPropsList
    if {$bVerbose} {
	puts "Neural network name: $nnProps(name)"
    }
    switch -glob -- [string tolower $nnProps(name)] {
	{*plant*} {
	    set nnType plant
	}
	{*object*} {
	    set nnType plant
	}
	{*controller*} {
	    set nnType controller
	}
	default {
	    set nnType general
	    if {$bVerbose} {
		puts "Can't determine type of neural network, using general"
	    }
	}
    }
}

# Prepare neural network architecture decoration
switch -exact $nnType {
    plant {
	set nnArchDecor [NNPDecorateNNArch [ReadNeuralNetFile $nnFilePath]]
	set nnPictRoot "nnparch"
    }
    controller {
	set nnArchDecor [NNCDecorateNNArch [ReadNeuralNetFile $nnFilePath]]
	set nnPictRoot "nncarch"
    }
    general {
	set nnArchDecor [ANNDecorateNNArch [ReadNeuralNetFile $nnFilePath] $inlabel $outlabel $instartindex $outstartindex]
	set nnPictRoot "nn_arch"
    }
}

if {$bVerbose} {
    puts "Decorated neural network architecture: $nnArchDecor"
}

### Main GUI
wm title . "$nnFilePath"
wm iconname . [file tail $nnFilePath]
#wm geometry . +100+100

set w ""
canvas $w.c -background white -width $winWidth -height $winHeight
pack $w.c -fill both -expand yes -side top

button $w.close -text [mc "Close"] -command "destroy ."
ScreenshotButton . $w.print_button $w.c [file dirname $nnFilePath] $nnPictRoot
pack $w.print_button $w.close -side left

DrawNeuralNetArch $w.c $nnArchDecor $showScalers

# End of file
