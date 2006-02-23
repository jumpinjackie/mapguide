# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000-2005
#	Sleepycat Software.  All rights reserved.
#
# $Id: run_tests.tcl,v 1.3 2005/04/05 16:44:29 bostic Exp $

# Tcl script to run the XML regression test suite

source ../test/test.tcl
set err [run_xml]

# TODO return codes for the query plan conversion scripts
convert ci
convert qp

exit $err
