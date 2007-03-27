# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000-2005
#	Sleepycat Software.  All rights reserved.
#
# $Id: run_tests.tcl,v 1.4 2005/04/05 16:44:59 bostic Exp $

# Tcl script to run the XQuery regression test suite

source ../test/test.tcl
set ok [run xquery]
exit $ok
