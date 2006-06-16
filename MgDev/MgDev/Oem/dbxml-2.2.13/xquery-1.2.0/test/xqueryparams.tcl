# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000-2005
#	Sleepycat Software.  All rights reserved.
#
# $Id: xqueryparams.tcl,v 1.3 2005/04/05 16:44:59 bostic Exp $

# list of TCL files for the "xquery" test suite
set test_names(xquery)	[list xquery001]

set subs {xquery}
foreach sub $subs {
	foreach test $test_names($sub) {
		source $test_path/$test.tcl
	}
}

source $test_path/xqueryutils.tcl
