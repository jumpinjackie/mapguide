# See the file LICENSE for redistribution information.
#
# Copyright (c) 2001,2006 Oracle.  All rights reserved.
#
# $Id: xmlshelltest.tcl,v 1.7 2006/10/30 17:46:10 bostic Exp $
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

