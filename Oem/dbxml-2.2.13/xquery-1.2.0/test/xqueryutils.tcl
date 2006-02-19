# See the file LICENSE for redistribution information.
#
# Copyright (c) 1996-2005
#	Sleepycat Software.  All rights reserved.
#
# $Id: xqueryutils.tcl,v 1.3 2005/04/05 16:44:59 bostic Exp $
#
# XQUERY test system utilities
#
# NOTE taken from DB/DBXML test utilities
#

proc readFileUTF8 { filename } {
	set fd [open $filename r]
	fconfigure $fd -encoding utf-8
	set data [read $fd]
	close $fd
	return $data
}

proc readFile { filename } {
	set fd [open $filename r]
	fconfigure $fd -encoding binary
	set data [read $fd]
	close $fd
	return $data
}

proc xquery_error_check_bad { func result bad {txn 0}} {
	if { [binary_compare $result $bad] == 0 } {
		if { $txn != 0 } {
			$txn abort
		}
		flush stdout
		flush stderr
		error "FAIL:[timestamp] $func returned error value $bad"
	}
}

proc xquery_error_check_good { func result desired {txn 0} } {
	if { [binary_compare $desired $result] != 0 } {
		if { $txn != 0 } {
			$txn abort
		}
		flush stdout
		flush stderr
		error "FAIL:[timestamp]\
		    $func: expected \"$desired\", got \"$result\""
	}
}

proc binary_compare { data1 data2 } {
	if { [string length $data1] != [string length $data2] || \
	    [string compare -length \
	    [string length $data1] $data1 $data2] != 0 } {
		return 1
	} else {
		return 0
	}
}

proc timestamp {{opt ""}} {
	global __timestamp_start

	set now [clock seconds]

	# -c	accurate to the click, instead of the second.
	# -r	seconds since the Epoch
	# -t	current time in the format expected by db_recover -t.
	# -w	wallclock time
	# else	wallclock plus elapsed time.
	if {[string compare $opt "-r"] == 0} {
		return $now
	} elseif {[string compare $opt "-t"] == 0} {
		return [clock format $now -format "%y%m%d%H%M.%S"]
	} elseif {[string compare $opt "-w"] == 0} {
		return [clock format $now -format "%c"]
	} else {
		if {[string compare $opt "-c"] == 0} {
			set printclicks 1
		} else {
			set printclicks 0
		}

		if {[catch {set start $__timestamp_start}] != 0} {
			set __timestamp_start $now
		}
		set start $__timestamp_start

		set elapsed [expr $now - $start]
		set the_time [clock format $now -format ""]
		set __timestamp_start $now

		if { $printclicks == 1 } {
			set pc_print [format ".%08u" [__fix_num [clock clicks]]]
		} else {
			set pc_print ""
		}

		format "%02d:%02d:%02d$pc_print (%02d:%02d:%02d)" \
		    [__fix_num [clock format $now -format "%H"]] \
		    [__fix_num [clock format $now -format "%M"]] \
		    [__fix_num [clock format $now -format "%S"]] \
		    [expr $elapsed / 3600] \
		    [expr ($elapsed % 3600) / 60] \
		    [expr ($elapsed % 3600) % 60]
	}
}

proc __fix_num { num } {
	set num [string trimleft $num "0"]
	if {[string length $num] == 0} {
		set num "0"
	}
	return $num
}

