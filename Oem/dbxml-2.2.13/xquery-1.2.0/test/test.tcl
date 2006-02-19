# See the file LICENSE for redistribution information
#
# Copyright (c) 2002-2005
# 	Sleepycat Software.  All rights reserved.
#
# $Id: test.tcl,v 1.6 2005/04/20 18:31:59 bostic Exp $
#

source ./include.tcl

#
# Test if XQuery (external) utilities work to figure out their path.
#
set stat [catch {exec $util_path/eval -?} ret]
if { [string first "exec format error" $ret] != -1 } {
	set util_path $util_path/.libs
}

# "xqueryparams.tcl" is where the list of XQuery tests is maintained
# and where those tests and other files needed for testing are sourced.
source $test_path/xqueryparams.tcl

proc run_test { test { args "" } } {
	source ./include.tcl
	eval $test $args
}

# entry point
proc run { sub args } {
	source ./include.tcl
	global test_names

  set err 0

	if { [info exists test_names($sub)] != 1 } {
		puts stderr "Subsystem $sub has no tests specified in\
		    xqueryparams.tcl; skipping."
	}

	foreach test $test_names($sub) {
		if {[catch {run_test $test $args} res] != 0 } {
			puts $res
      set err 1
		}
	}

  return $err
}

