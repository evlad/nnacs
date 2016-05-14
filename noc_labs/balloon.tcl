package provide balloon 1.0;
# It's made by Vitus Wagner

namespace eval ::balloon {
   proc this {} "return [namespace current];";

   variable state;

   array unset state;
   array set state {};

   proc balloon {w args} {
     variable state;

     if {![info exists state($w.background)]} {
	 	array set state [getDefaults $w] 
     }
	 
     foreach {option value} $args {
       set var  [string range $option 1 end];

       switch -exact -- $option {
         -bg         -
         -background -
         -fg         -
         -foreground {
           if {[string match "f*" $var]} {
             set var  foreground;
           } else {
             set var  background;
           }

           if {[catch {winfo rgb $parent $value;}]} {
             return -code error "expected valid $var colour name or value, but got \"$value\"";
           }
         }
         -dismissdelay -
         -showdelay    {
           if {![string is integer -strict $value]} {
             return -code error "expected integer delay value in ms, but got \"$value\"";
           }
         }
         -label        {}
         -text         {}
         -textvariable {}
         default  {
           return -code error "bad option \"$option\": must be -background, -dismissdelay, -foreground, -label, -showdelay, or -text";
         }
       }

       set state($w.$var)  $value;
     }

	 if {![info exists state($w.showdelay)]} {
	 	set state($w.showdelay) 500
	 }	
     if {$state($w.showdelay) == -1} {
       bind $w <Any-Enter> {};
       bind $w <Any-Leave> {};
       return;
     }
	 
     if {$state($w.showdelay)} {
       bind $w <Any-Enter> [list \
         after \
           $state($w.showdelay) \
           [list [namespace code showCB] %W] \
       ];
       bind $w <Any-Leave> [list [namespace code destroyCB] %W];
     }
	 bind $w <Destroy> [list [namespace code clearState] %W]
     return;
   }

   proc clearState {w} {
   	variable state
	array unset state $w.*
   }	
   proc destroyCB {w} {
     variable state;

     catch {destroy $w.balloon;};

     if {[info exists state($w.id)] && ($state($w.id) != "")} {
       catch {after cancel $state($w.id);};

       set state($w.id)  "";
     }

     return;
   }

   proc showCB {w} {
     if {[eval winfo containing [winfo pointerxy .]] != $w} {
       return;
     }

     variable state;

     set top    $w.balloon;
     set width  0;
     set height 0;

     catch {destroy $top;}

    if {!$state($w.showdelay)} {
      return;
    }

     toplevel $top \
       -background  $state($w.foreground) \
	   -class Balloon;

     wm withdraw         $top;
     wm overrideredirect $top 1;
     wm sizefrom         $top program;
     wm resizable        $top 0 0;

     if {$state($w.label) != ""} {
       pack [label $top.label \
         -text       $state($w.label) \
         -background $state($w.background) \
         -foreground $state($w.foreground) \
         -anchor     w \
         -justify    left \
       ] -side top -fill x -expand 0;

       update idletasks;

       set width  [winfo reqwidth $top.label];
       set height [winfo reqheight $top.label];
     }

     if {($state($w.text) != "") ||
         ($state($w.textvariable) != "")} {
       if {$state($w.textvariable) != ""} {
         upvar 0 $state($w.textvariable) textvariable;

         set state($w.text) $textvariable;
       }

       pack [message $top.text \
         -text       $state($w.text) \
         -background $state($w.background) \
         -foreground $state($w.foreground) \
         -justify    left \
       ] -side top -fill x -expand 0;

       update idletasks;

       catch {
         if {$width < [winfo reqwidth $top.text]} {
           set width [winfo reqwidth $top.text];
         }

         incr height [winfo reqheight $top.text];
       }
     }

     catch {
       update idletasks;

       if {[winfo pointerx $w]+$width > [winfo screenwidth $w]} {
         set x [expr {[winfo screenwidth $w] - 10 - $width}];
       } else {
         set x [expr {[winfo pointerx $w] + 10}];
       }

       wm geometry $top \
         ${width}x${height}+${x}+[expr {[winfo pointery $w]+10}];
       wm deiconify $top;

       raise $top;

       set state($w.id) [after \
         $state($w.dismissdelay) \
         [list [namespace code destroyCB] $w] \
       ];
     }

     return;
   }
	proc getDefaults {w} {
      lappend res $w.label  "" $w.text "" $w.textvariable "";
	  toplevel $w.balloon -class Balloon
	  foreach {var option class} {
	  	font font Font
		background background Background
		foreground foreground Foreground
		dismissdelay dismissDelay DismissDelay
		showdelay showDelay ShowDelay
	  } {
	  	lappend res $w.$var [option get $w.balloon $option $class]
	  }
	  destroy $w.balloon
	  return $res
	}   
   namespace export -clear balloon;
 }

  option add *Balloon*Font  {{San Serif} 8 bold}  widgetDefault
  option add *Balloon.borderWidth 1 widgetDefault
  option add *Balloon.relief solid widgetDefault
  option add *Balloon.background   lightyellow widgetDefault
  option add *Balloon.dismissDelay 5000 widgetDefault
  option add *Balloon.foreground   black;
  option add *Balloon.showDelay    500;
  option add *Balloon.Message.Aspect 10000;
  
 namespace import -force ::balloon::*;
