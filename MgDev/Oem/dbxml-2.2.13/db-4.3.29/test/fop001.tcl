# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000-2004
#	Sleepycat Software.  All rights reserved.
#
# $Id: fop001.tcl,v 1.21 2004/09/22 18:01:04 bostic Exp $
#
# TEST	fop001.tcl
# TEST	Test file system operations, combined in a transaction. [#7363]
proc fop001 { method args } {
	source ./include.tcl

	set args [convert_args $method $args]
	set omethod [convert_method $method]

	puts "\nFop001: ($method)\
	    Multiple file system ops in one transaction."

	set exists {a b}
	set noexist {foo bar}
	set open {}
	set cases {}
	set ops {rename remove open open_create open_excl truncate}

	# Set up all sensible two-op cases (op1 succeeds).
	foreach retval { 0 "file exists" "no such file" } {
		foreach op1 {rename remove open open_excl \
		    open_create truncate} {
			foreach op2 $ops {
				append cases " " [create_tests $op1 $op2 \
				    $exists $noexist $open $retval]
			}
		}
	}

	# Set up evil two-op cases (op1 fails).  Omit open_create
	# and truncate from op1 list -- open_create always succeeds
	# and truncate requires a successful open.
	foreach retval { 0 "file exists" "no such file" } {
		foreach op1 { rename remove open open_excl } {
			foreach op2 $ops {
				append cases " " [create_badtests $op1 $op2 \
					$exists $noexist $open $retval]
			}
		}
	}

	# The structure of each case is:
	# {{op1 {names1} result end1} {op2 {names2} result}}
	# A result of "0" indicates no error is expected.
	# Otherwise, the result is the expected error message.
	#
	# The "end1" variable indicates whether the first txn
	# ended with an abort or a commit, and is not used
	# in this test.
	#
	# Comment this loop out to remove the list of cases.
#	set i 1
#	foreach case $cases {
#		puts "\tFop001:$i: $case"
#		incr i
#	}

	set testid 0

	# Run all the cases
	foreach case $cases {
		env_cleanup $testdir
		incr testid

		# Extract elements of the case
		set op1 [lindex [lindex $case 0] 0]
		set names1 [lindex [lindex $case 0] 1]
		set res1 [lindex [lindex $case 0] 2]

		set op2 [lindex [lindex $case 1] 0]
		set names2 [lindex [lindex $case 1] 1]
		set res2 [lindex [lindex $case 1] 2]

		puts "\tFop001.$testid: $op1 ($names1), then $op2 ($names2)."

		# Create transactional environment.
		set env [berkdb_env -create -home $testdir -txn]
		error_check_good is_valid_env [is_valid_env $env] TRUE

		# Create two databases
		set dba [eval {berkdb_open \
		    -create} $omethod $args -env $env -auto_commit a]
		error_check_good dba_open [is_valid_db $dba] TRUE
		error_check_good dba_put [$dba put -auto_commit 1 a] 0
		error_check_good dba_close [$dba close] 0

		set dbb [eval {berkdb_open \
		    -create} $omethod $args -env $env -auto_commit b]
		error_check_good dbb_open [is_valid_db $dbb] TRUE
		error_check_good dbb_put [$dbb put -auto_commit 1 b] 0
		error_check_good dbb_close [$dbb close] 0

		foreach end {abort commit} {
			# Start transaction
			set txn [$env txn]

			# Execute and check operation 1
			set result1 [do_op $omethod $op1 $names1 $txn $env $args]
			if {$res1 == 0} {
				error_check_good op1_should_succeed $result1 $res1
			} else {
				set error [extract_error $result1]
				error_check_good op1_wrong_failure $error $res1
			}

			# Execute and check operation 2
			set result2 [do_op $omethod $op2 $names2 $txn $env $args]
			if {$res2 == 0} {
				error_check_good op2_should_succeed $result2 $res2
			} else {
				set error [extract_error $result2]
				error_check_good op2_wrong_failure $error $res2
			}

			# End transaction
			error_check_good txn_$end [$txn $end] 0

			# If the txn was aborted, we still have the original two
			# databases.
			if {$end == "abort"} {
				error_check_good a_exists \
				    [file exists $testdir/a] 1
				error_check_good b_exists \
				    [file exists $testdir/b] 1
			}
		}

		# Close any open db handles.  We had to wait until now
		# because you can't close a database inside a transaction.
		set handles [berkdb handles]
		foreach handle $handles {
			if {[string range $handle 0 1] == "db" } {
				error_check_good db_close [$handle close] 0
			}
		}
		# Clean up for next case
		error_check_good env_close [$env close] 0
		error_check_good envremove [berkdb envremove -home $testdir] 0
		env_cleanup $testdir
	}
}


