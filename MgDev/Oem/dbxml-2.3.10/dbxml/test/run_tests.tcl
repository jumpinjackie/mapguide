# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2006 Oracle.  All rights reserved.
#
# $Id: run_tests.tcl,v 1.5 2006/10/30 17:46:09 bostic Exp $

# Tcl script to run the XML regression test suite

source ../test/test.tcl
set err [run_xml]

# TODO return codes for the query plan conversion scripts
convert ci
convert qp

exit $err
