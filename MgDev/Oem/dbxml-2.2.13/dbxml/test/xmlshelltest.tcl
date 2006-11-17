# See the file LICENSE for redistribution information.
#
# Copyright (c) 2001-2005
#	Sleepycat Software.  All rights reserved.
#
# $Id: xmlshelltest.tcl,v 1.5 2005/04/05 16:44:32 bostic Exp $
#
# TEST	xmlscr###
# TEST	The scr### directories contain shell scripts that test a 
# TEST 	variety of things about the XML product, including things 
# TEST 	about the distribution itself.  These tests won't run on 
# TEST	most systems, so don't even try to run them.
#
# xmlshelltest.tcl:
#	Code to run XML shell scripts.
proc xmlshelltest { { run_one 0 }} {
	source ./include.tcl
	source $db_test_path/shelltest.tcl
	global xmlshelltest_list

	shelltest $run_one 
}

proc xmlscr001 {} { xmlshelltest 1 }

