package provide screenshot 1.0

package require Tk

# w - widget
# c - canvas to screenshot
# workDir - where to store file
# rootName - name of the file with appended number and file extension
# type - image type to store
proc ScreenshotAction {w c workDir rootName type} {
    # Calculate next free number, considering file names have format
    # ${rootName}##.*, where ## - two digits
    set ls [glob -nocomplain -tails -directory "$workDir" -types f "$rootName\[0-9\]\[0-9\].*"]
    set lastName [lindex [lsort $ls] end]
    if {$lastName == ""} {
	set nextNum 0
    } elseif {[regexp "^${rootName}(..)\..*\$" "$lastName" rest lastNum]} {
	scan $lastNum "%d" nextNum
	incr nextNum
    } else {
	set nextNum 0
    }

    # Let's compose path
    set fileName [format "%s%02d" "$rootName" $nextNum]
    set rootPath [file join "$workDir" "$fileName"]
    switch $type {
	postscript {
	    set ext "ps"
	    set filePath "$rootPath.$ext"
	    # Margins one the page (mm)
	    set xmar 20
	    set ymar 20
	    $c postscript -colormode color -file $filePath \
		-pagewidth [expr 297 - $xmar].m \
		-pageheight [expr 210 - $ymar].m
	}
	jpeg {
	    set ext "jpg"
	    set filePath "$rootPath.$ext"
	    set img [image create photo -format window -data $c]
	    $img write -format $type "$filePath"
	}
	default {
	    # type and file name extension are the same
	    set ext $type
	    set filePath "$rootPath.$ext"
	    set img [image create photo -format window -data $c]
	    $img write -format $type "$filePath"
	}
    }
    puts "Screenshot is in $filePath"
    tk_messageBox -parent $w -icon info -type ok \
	-title "Screenshot: $fileName.$ext" \
	-message  "Снимок сохранен в файл\n$filePath"
}


# Create button and make an action for screenshooting on it.
# w - toplevel window path
# b - path of button to create
# c - canvas widget for screenshooting
# workDir - where to create screenshot file
# rootName - name of the file with appended number and file extension
proc ScreenshotButton {w b c workDir rootName} {
    set m $b.m
    menubutton $b -text "Снимок окна" -direction below -menu $m -relief raised
    menu $m -tearoff 0
    # These formats presents in raw Tk
    set imgfmts {postscript gif}
    if {0 == [catch {package require Img} res]} {
	# It's possible to use wide variety of image formats when Img
	# is available
	lappend imgfmts png jpeg bmp
    }
    foreach imgfmt $imgfmts {
	$m add command -label $imgfmt \
	    -command "ScreenshotAction $w $c \"$workDir\" \"$rootName\" $imgfmt"
    }
}
