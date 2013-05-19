package provide win_signalgen 1.0

package require Tk
#package require tcllib
package require win_grseries
package require win_nntpar

# What is to do to generate the series with given parameters?
proc clear_series.gen {p arref filepath} {
    if {![catch {open $filepath w} fd]} {
	close $fd
	puts "clear_series.gen - DONE"
    }
}


#
# Random Gaussian distributed series
#

# What is to enter?
set rnd_gauss.par_gui {
    -1 "Нормально распределенный случайный ряд"
    length "Длина ряда"
    mean "Среднее значение"
    variance "Среднеквадратическое отклонение"
}

# Which values are default ones? (==> *_par_arr)
set rnd_gauss.par_initial {
    length 100
    mean 0.0
    variance 1.0
}

# What is to do to generate the series with given parameters?
proc rnd_gauss.gen {p arref filepath} {
    global $arref
    upvar #0 $arref arr
    #set tmpfilepath [::fileutil::tempfile drand]
    catch {exec [file join [SystemDir] bin drand] >> $filepath \
	       $arr(length) $arr(mean) $arr(variance) } errCode
    puts "Run drand: $errCode"
}


#
# Random unified distributed series
#

# What is to enter?
set rnd_unified.par_gui {
    -1 "Равномерно распределенный случайный ряд"
    length "Длина ряда"
    minampl "Минимальное значение амплитуды"
    maxampl "Максимальное значение амплитуды"
}

# Which values are default ones? (==> *_par_arr)
set rnd_unified.par_initial {
    length 100
    minampl -1.0
    maxampl 1.0
}

# What is to do to generate the series with given parameters?
proc rnd_unified.gen {p arref filepath} {
    global $arref
    upvar #0 $arref arr
    catch {exec [file join [SystemDir] bin dranduni] >> $filepath \
	       $arr(length) $arr(minampl) $arr(maxampl) } errCode
    puts "Run dranduni: $errCode"
}


#
# Random unified distributed meander series
#

# What is to enter?
set rnd_meander.par_gui {
    -1 "Равномерно распределенный случайный меандр"
    length "Длина ряда"
    minhalfper "Минимальная длина ступени"
    maxhalfper "Максимальная длина ступени"
    minampl "Минимальное значение амплитуды"
    maxampl "Максимальное значение амплитуды"
}

# Which values are default ones? (==> *_par_arr)
set rnd_meander.par_initial {
    length 100
    minhalfper 1
    maxhalfper 20
    minampl -1.0
    maxampl 1.0
}

# What is to do to generate the series with given parameters?
proc rnd_meander.gen {p arref filepath} {
    global $arref
    upvar #0 $arref arr
    #set tmpfilepath [::fileutil::tempfile drandmea]
    catch {exec [file join [SystemDir] bin drandmea] >> $filepath \
	       $arr(length) $arr(maxhalfper) $arr(maxhalfper) \
	       $arr(minampl) $arr(maxampl) } errCode
    puts "Run drandmea: $errCode"
}


#
# Meander series
#

# What is to enter?
set meander.par_gui {
    -1 "Меандр"
    length "Длина ряда"
    halfperiod "Полупериод"
    phaseshift "Фазовый сдвиг"
}

# Which values are default ones? (==> *_par_arr)
set meander.par_initial {
    length 100
    halfperiod 10
    phaseshift 0
}

# What is to do to generate the series with given parameters?
proc meander.gen {p arref filepath} {
    global $arref
    upvar #0 $arref arr
    catch {exec [file join [SystemDir] bin dmeander] >> $filepath \
	       $arr(length) $arr(halfperiod) $arr(phaseshift) } errCode
    puts "Run dmeander: $errCode"
}


#
# Harmonic series
#

# What is to enter?
set harmonic.par_gui {
    -1 "Синусоида"
    length "Длина ряда"
    period "Период"
    phaseshift "Фазовый сдвиг"
}

# Which values are default ones? (==> *_par_arr)
set harmonic.par_initial {
    length 100
    period 20
    phaseshift 0
}

# What is to do to generate the series with given parameters?
proc harmonic.gen {p arref filepath} {
    global $arref
    upvar #0 $arref arr
    catch {exec [file join [SystemDir] bin dsin] >> $filepath \
	       $arr(length) $arr(period) $arr(phaseshift) } errCode
    puts "Run dsin: $errCode"
}


#
# Constant value series
#

# What is to enter?
set constant.par_gui {
    -1 "Постоянное значение"
    length "Длина ряда"
    ampl "Амплитуда"
}

# Which values are default ones? (==> *_par_arr)
set constant.par_initial {
    length 100
    ampl 0.0
}

# What is to do to generate the series with given parameters?
proc constant.gen {p arref filepath} {
    global $arref
    upvar #0 $arref arr
    catch {exec [file join [SystemDir] bin dsteps] >> $filepath \
	       $arr(ampl) $arr(length) } errCode
    puts "Run dsteps: $errCode"
}



# General generator function which uses $n.* data and procedures to
# append (or edit) file pointed by path in signal_target_gen_filepath.
proc SignalGenAction {p n} {
    global $n.par_gui $n.par_initial $n.par_arr
    upvar #0 signal_target_gen_filepath filepath

    if {[info exists $n.par_initial]} {
	#puts "$n.par_arr: [array get $n.par_arr]"
	NNTeacherParWindow $p $n.par_arr [set $n.par_gui]
    }
    $n.gen $p $n.par_arr $filepath
    set wholeData [GrSeriesReadFile $filepath]
    GrSeriesUpdateSeries $p 0 [lindex $wholeData 0]
    GrSeriesViewAll $p.grseries.graphics.c x
    GrSeriesViewAll $p.grseries.graphics.c y
    GrSeriesRedraw $p
}


# Method to call from GrSeries (widget $p)
proc SignalGenExtGrSeries {cmd p} {
    set f $p.grseries.signalgen
    switch $cmd {
	create {
	    frame $f
	    set buttons {
		clear_series "Очистить ряд"
		rnd_gauss "Случайный нормальный"
		rnd_unified "Случайный равномерный"
		rnd_meander "Случайный меандр"
		constant "Постоянное значение"
		meander "Меандр"
		harmonic "Синусоида"
#		delta_pulse "Дельта-импульсы"
	    }
	    foreach {n t} $buttons {
		button $f.$n -text $t -command "SignalGenAction $p $n"
		global $n.par_initial
		if {[info exists $n.par_initial]} {
		    puts "$n.par_initial => $n.par_arr"
		    global $n.par_arr
		    array set $n.par_arr [set $n.par_initial]
		    puts "$n.par_arr: [array get $n.par_arr]"
		}
	    }

	    pack $f.clear_series $f.rnd_gauss $f.rnd_unified $f.rnd_meander \
		$f.constant $f.meander $f.harmonic -fill x
	    # $f.delta_pulse
	    pack $f -side left -fill y
	}
    }
}


proc SignalGenWindow {p workDir var_fileRelPath} {
    upvar #0 $var_fileRelPath fileRelPath
    set filepath [SessionAbsPath "$workDir" "$fileRelPath"]
    # Create empty file
    if {![catch {open "$filepath" a} fd]} {
	# Create if does not exist
	close $fd
    }
    # else Failed to read/create
    global signal_target_gen_filepath
    set signal_target_gen_filepath "$filepath"

    GrSeriesWindow $p "Create $filepath series" "$filepath" SignalGenExtGrSeries
}
