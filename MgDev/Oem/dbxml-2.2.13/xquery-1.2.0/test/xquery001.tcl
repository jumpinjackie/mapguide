# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000-2005
#	Sleepycat Software.  All rights reserved.
#
# $Id: xquery001.tcl,v 1.8 2005/06/07 16:10:44 jsnelson Exp $
#
# TEST	XQuery - W3C Use Cases

proc xquery001 { xmldiff } {
	puts "\nxquery001: XQuery W3C Use Cases ($xmldiff)"
	source ./include.tcl

  # We simply exec the C++ "eval" program which evaluates a specified query
  # that corresponds to a use case, read from a file

  set eval_prog "eval"

  # set up directores etc.
  set expected_dir [file join $test_path w3c_usecases "expected"]

  set actual_dir [file join $test_path w3c_usecases "actual"]
  file delete -force $actual_dir
	file mkdir $actual_dir

  xquery001_eval "XMP" "Experiences and Exemplars" $expected_dir $actual_dir $eval_prog $xmldiff
  xquery001_eval "TREE" "Queries that preserve Hierarchy" $expected_dir $actual_dir $eval_prog $xmldiff
  xquery001_eval "SEQ" "Queries based on Sequences" $expected_dir $actual_dir $eval_prog $xmldiff
  xquery001_eval "R" "Access to Relational Data" $expected_dir $actual_dir $eval_prog $xmldiff
  xquery001_eval "SGML" "Standard Generalized Markup Language" $expected_dir $actual_dir $eval_prog $xmldiff
  xquery001_eval "TEXT" "String Search" $expected_dir $actual_dir $eval_prog $xmldiff
  xquery001_eval "NS" "Queries using Namespaces" $expected_dir $actual_dir $eval_prog $xmldiff
  xquery001_eval "PARTS" "Recursive Parts Explosion" $expected_dir $actual_dir $eval_prog $xmldiff
  #xquery001_eval "STRONG" "Strongly typed data" $expected_dir $actual_dir $eval_prog $xmldiff
}

proc xquery001_eval { test_type description expected_dir actual_dir eval_program xmldiff } {
 source ./include.tcl

  puts "\n  $test_type ($description)"

  # the queries and expected results are file based
  set ret [catch { glob $test_path/w3c_usecases/$test_type*.xquery } files]
  if { $ret == 0 } {
    foreach query [lsort -dictionary $files] {

      # save the name of the use case
      set query_local [file tail $query]
      set idx [string last [file extension $query]  $query_local]
      set use_case [string range $query_local 0 [expr $idx-1]]

      puts -nonewline "    $use_case"

      # read expected result (in the subdir "expected", ".out" extension)
      set expected_file [file join $expected_dir ${use_case}.out]
      set expected [readFileUTF8 $expected_file]

      # generate a filename for the actual results
      #  NB using the (string) output would be quicker, but this leaves us
      #  with a file to look at when things go wrong AND it allows easier
      #  running of an XmlDiff program
      set actual_file [file join $actual_dir ${use_case}.out]

      # we simply execute the C++ "eval" program, redirecting the output to
      # a local file
#      set ret [catch { exec $util_path/$eval_program $query -o $actual_file} res]
      
#      puts "./$eval_program $query -o $actual_file"

      set ret [catch { exec ./$eval_program $query -o $actual_file} res]
      if {$ret == 0} {
        set actual [readFileUTF8 $actual_file]
        set err [string compare $expected $actual]
        if { $err != 0 } {
          puts -nonewline "\tFAILED"
        }

        # optional execution of an XmlDiff program
        if { [string length $xmldiff] } {
          set ret [catch {exec $xmldiff -n $expected_file $actual_file 2>/dev/null} res]
          if { $ret != 0 } {
            puts -nonewline "\t(Xmldiff FAILED)"
          } else {
            puts -nonewline "\t(Xmldiff OK)"
          }
        }

        # report the error (fatal)
        if { $err != 0 } {
         puts -nonewline "\n\n"
          xquery_error_check_good "\n$use_case\n" $actual $expected
        }
      } else {
        xquery_error_check_good "\tFAILED: $res" $ret 0
      }
      puts -nonewline "\n"
    }
  } else {
    xquery_error_check_good "ERROR no use cases found for type $test_type" 0 1
  }
}

