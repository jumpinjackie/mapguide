# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2006 Oracle.  All rights reserved.
#
# $Id: xmlparams.tcl,v 1.17 2006/10/30 17:46:10 bostic Exp $

set test_names(xml)	[list xml002 xml003 xml006 xml008 xml009 xml010 xml011 xml012 xml013 xml014 xml016 xml017]

set xmlsubs {xml}

foreach sub $xmlsubs {
	foreach test $test_names($sub) {
		source $test_path/$test.tcl
	}
}

source $test_path/xmlutils.tcl
source $test_path/xmlshelltest.tcl
source $db_test_path/testutils.tcl
source $db_test_path/upgrade.tcl

# The params file allows you to set default values for 
# parameters passed to test functions.  None are in use for 
# XML testing yet, but here's an example of the expected format.
# set parms(test001) {10000 0 "01" 0}

# XML shell script tests.  Each list entry is a {directory filename} pair,
# invoked with "/bin/sh filename".
set xmlshelltest_list {
	{ xmlscr001		chk.code }
}
