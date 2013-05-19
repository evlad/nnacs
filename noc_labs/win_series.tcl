package provide win_series 1.0

package require files_loc

package require win_textedit
package require win_grseries

set seriesInstance 1

# Create window with panel and controls.
proc seriesCreateWindow {p title} {
    global seriesInstance
    incr seriesInstance
    set w $p.series$seriesInstance

    # Don't create the window twice
    if {[winfo exists $w]} return

    toplevel $w
    wm title $w "Series visualization, analysis and edit tool"
    wm iconname $w "Series$seriesInstance"

    button $w.close -text "Закрыть" -command "destroy $w"
    pack $w.close
}
