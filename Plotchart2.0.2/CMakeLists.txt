cmake_minimum_required(VERSION 3.5)

project(PlotChart2.0.2)

set(TCL_SCRIPTS plot3d.tcl
      plotannot.tcl
      plotaxis.tcl
      plotbind.tcl
      plotbusiness.tcl
      plotchart.tcl
      plotconfig.tcl
      plotcontour.tcl
      plotgantt.tcl
      plotobject.tcl
      plotpack.tcl
      plotpriv.tcl
      plotspecial.tcl
      plottable.tcl
      scaling.tcl)

install(FILES ${TCL_SCRIPTS} DESTINATION scripts)
