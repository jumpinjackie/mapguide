# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2006 Oracle.  All rights reserved.
#
# $Id: xml014.tcl,v 1.42.2.2 2007/01/18 19:36:13 gmf Exp $
#
# TEST	xml014
# TEST	modifyDocument
# TEST	Test use, and abuse of the XmlModify class as well as
# TEST	the ::modifyDocument() methods on XmlDocument
# TEST	Must cover matrix of operations and types, against both containers
# TEST  and document, with and without transactions.

proc xml014 { args } {
	source ./include.tcl
	puts "\nxml014: modifyDocument ($args)"

	# Determine whether procedure has been called within an environment,
	# and whether it is a transactional environment.
	# If we are using an env, then the filename should just be the test
	# number.  Otherwise it is the test directory and the test number.
	set eindex [lsearch -exact $args "-env"]
	set txnenv 0
	set tnum 14
	set oargs [eval {set_openargs} $args]

	if { $eindex == -1 } {
		set env NULL
	} else {
		incr eindex
		set env [lindex $args $eindex]
		set txnenv [is_txnenv $env]
		set testdir [get_home $env]
	}

  # assign the container type (default to whole document)
  set tindex [lsearch -exact $args "-storage"]
  if { $tindex == -1 } {
    set global_container_type $XmlContainer_WholedocContainer
  } else {
    incr tindex
    set global_container_type [lindex $args $tindex]
  }

    if { [lsearch -exact $args "-indexnodes"] >= 0 } {
	set global_container_flags $DBXML_INDEX_NODES
    } else {
	set global_container_flags 0
    }

    set basename $tnum

    xml014_1 $env $txnenv $basename.1 $oargs
    xml014_2 $env $txnenv $basename.2 $oargs
    xml014_3 $env $txnenv $basename.3 $oargs
    xml014_4 $env $txnenv $basename.4 $oargs
    xml014_5 $env $txnenv $basename.5 $oargs
    xml014_6 $env $txnenv $basename.6 $oargs
}

#
# generic modify routine for documents, with content checking
#
proc xml014_modify_document { txnenv testid document expected db container query op {type -1} {name ""} {content ""} {location -1} {new_encoding ""}} {
    source ./include.tcl

    puts -nonewline "\t\t$testid:"
    if { $op != "" } {
        puts -nonewline " $op \"$query\""
    }
    if { $new_encoding != "" } {
        puts -nonewline " new encoding: \"$new_encoding\""
    }
    puts ""

    wrap XmlQueryContext qc [$db createQueryContext $XmlQueryContext_LiveValues]
    wrap XmlUpdateContext uc [$db createUpdateContext]

    wrap XmlDocument xd [$db createDocument]
    $xd setContent $document
    $xd setName $testid

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        $container putDocument $txn $xd $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $xd $uc
    }

    delete xd

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $testid]
    } else {
        wrap XmlDocument xd [$container getDocument $testid]
    }
    new XmlValue xv $xd

    wrap XmlModify mod [$db createModify]

    if { $op != "" } {
        wrap XmlQueryExpression qe [$db prepare $query $qc]

        if { $op == "append" } {
            $mod addAppendStep $qe $type $name $content $location
        } else {
            if { $op == "remove"} {
                $mod addRemoveStep $qe
            } else {
                if { $op == "insertbefore"} {
                    $mod addInsertBeforeStep $qe $type $name $content
                } else {
                    if { $op == "insertafter"} {
                        $mod addInsertAfterStep $qe $type $name $content
                    } else {
                        if { $op == "rename"} {
                            $mod addRenameStep $qe $name
                        } else {
                            if { $op == "update"} {
                                $mod addUpdateStep $qe $content
                            }
                        }
                    }
                }
            }
        }
    }

    if {$new_encoding != ""} {
        $mod setNewEncoding $new_encoding
    }

    if { $txnenv == 1 } {
        $mod execute $txn $xv $qc $uc
    } else {
        $mod execute $xv $qc $uc
    }

    if { $op != "" } {
        delete qe
    }
    delete mod
    delete uc
    delete qc

    dbxml_error_check_good "$testid first check" [$xd getContentAsString] $expected

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    delete xv
    delete xd

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$db createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $testid]
    } else {
        wrap XmlDocument xd [$container getDocument $testid]
    }

    dbxml_error_check_good "$testid second check" [$xd getContentAsString] $expected

    delete xd

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }
}

#
# generic modify routine for documents, with content checking, using
# XmlResults for content rather than strings
#
proc xml014_modify_document_with_results { txnenv testid document expected mgr container query op {type -1} {name ""} {contentQuery ""} {location -1} {new_encoding ""}} {
    source ./include.tcl
    
    puts -nonewline "\t\t  $testid:"
    if { $op != "" } {
        puts -nonewline " $op \"$query\""
    }
    if { $contentQuery != "" } {
        puts -nonewline "\" $contentQuery\""
    }
    if { $new_encoding != "" } {
        puts -nonewline " new encoding: \"$new_encoding\""
    }
    puts ""
    
    wrap XmlQueryContext qc [$mgr createQueryContext $XmlQueryContext_LiveValues]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    
    wrap XmlDocument xd [$mgr createDocument]
    $xd setContent $document
    $xd setName $testid
    
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$mgr createTransaction]
        $container putDocument $txn $xd $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $xd $uc
    }
    
    delete xd
    
    # get results in the same transaction as the modify, if
    # specified

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $testid]
	if { $contentQuery != "" } {
	    wrap XmlResults res [$mgr query $txn $contentQuery $qc]
	} else {
	    wrap XmlResults res [$mgr createResults]
	}
    } else {
        wrap XmlDocument xd [$container getDocument $testid]
	if { $contentQuery != "" } {
	    wrap XmlResults res [$mgr query $contentQuery $qc]
	} else {
	    wrap XmlResults res [$mgr createResults]
	}
    }
    new XmlValue xv $xd

    wrap XmlModify mod [$mgr createModify]

    if { $op != "" } {
        wrap XmlQueryExpression qe [$mgr prepare $query $qc]

        if { $op == "append" } {
            $mod addAppendStep $qe $type $name $res $location
        } else {
            if { $op == "remove"} {
                $mod addRemoveStep $qe
            } else {
                if { $op == "insertbefore"} {
                    $mod addInsertBeforeStep $qe $type $name $res
                } else {
                    if { $op == "insertafter"} {
                        $mod addInsertAfterStep $qe $type $name $res
                    } else {
                        if { $op == "rename"} {
                            $mod addRenameStep $qe $name
                        }
                    }
                }
            }
        }
    }

    if {$new_encoding != ""} {
        $mod setNewEncoding $new_encoding
    }

    if { $txnenv == 1 } {
        set num [$mod execute $txn $xv $qc $uc]
    } else {
        set num [$mod execute $xv $qc $uc]
    }
    if { $op != "" } {
        delete qe
    }
    delete mod
    delete qc

    dbxml_error_check_good $testid [$xd getContentAsString] $expected

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    delete xv
    delete xd

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlDocument xd1 [$container getDocument $txn $testid]
    } else {
        wrap XmlDocument xd1 [$container getDocument $testid]
    }

    dbxml_error_check_good $testid [$xd1 getContentAsString] $expected

    # clean up -- remove document

    if { $txnenv == 1 } {
	$container deleteDocument $txn $xd1 $uc
    } else {
	$container deleteDocument $xd1 $uc
    }

    delete uc
    delete xd1

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }
}

#
# test all basic operations on an empty document
#
proc xml014_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t$basename: test basic document modification"

    xml_database db $testdir $env

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # append element
    xml014_modify_document $txnenv "xml014.1.1" $content1 $expected1_1 $db $container "/root" "append" $XmlModify_Element "new" "new content"

    # append attribute
    xml014_modify_document $txnenv "xml014.1.2.1" $content1 $expected1_2 $db $container "/root" "append" $XmlModify_Attribute "new" "foo"

    # attribute without value
    xml014_modify_document $txnenv "xml014.1.2.2" $content1 $expected1_2b $db $container "/root" "append" $XmlModify_Attribute "new" ""

    # append PI
    xml014_modify_document $txnenv "xml014.1.3" $content1 $expected1_3 $db $container "/root" "append" $XmlModify_ProcessingInstruction "newPI" "PIcontent"

    # append comment
    xml014_modify_document $txnenv "xml014.1.4" $content1 $expected1_4 $db $container "/root" "append" $XmlModify_Comment "" "comment content"

    # append text
    xml014_modify_document $txnenv "xml014.1.5" $content1 $expected1_5 $db $container "/root" "append" $XmlModify_Text "" "text content"

    # element without name
    xml014_modify_document $txnenv "xml014.3.6" $content1 $expected1_6 $db $container "/root" "append" $XmlModify_Element "" "val"

    # multiple siblings
    xml014_modify_document $txnenv "xml014.3.7" $content1 $expected1_7 $db $container "/root" "append" $XmlModify_Element "" "<a/><a/><b>text</b>"

    # multiple text siblings (note that the type can be Element or Text or Comment
    xml014_modify_document $txnenv "xml014.3.8" $content1 $expected1_8 $db $container "/root" "append" $XmlModify_Element "" "text1<!--comment--><a/>text2"

    delete container
    delete db
}

#
# Test basic operations on a document with a little structure
#
proc xml014_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: test more complex document modification"

    xml014_2_1 $env $txnenv $basename.1 $oargs
    xml014_2_2 $env $txnenv $basename.2 $oargs
}

#
# Test basic operations on a document with a little structure -
# no encoding changes
#
proc xml014_2_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t\t$basename: without encoding changes"

    xml_database db $testdir $env

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # REMOVE
    # remove attribute
    xml014_modify_document $txnenv "\t$basename.1" $content2 $expected2_1 $db $container "/root/b/@att1" "remove"

    # remove element
    xml014_modify_document $txnenv "\t$basename.2" $content2 $expected2_2 $db $container "/root/b\[text()='b content 2'\]" "remove"

    # remove comment
    xml014_modify_document $txnenv "\t$basename.3" $content2 $expected2_3 $db $container "/root/comment()" "remove"

    # remove text
    xml014_modify_document $txnenv "\t$basename.4" $content2 $expected2_4 $db $container "/root/a/text()" "remove"

    # APPEND (default (-1), 0, and non-zero positive)
    # append default
    xml014_modify_document $txnenv "\t$basename.5" $content2 $expected2_5 $db $container "/root" "append" $XmlModify_Element "new" ""

    # append at 0
    xml014_modify_document $txnenv "\t$basename.6" $content2 $expected2_6 $db $container "/root" "append" $XmlModify_Element "new" "" 0

    # append at 2
    xml014_modify_document $txnenv "\t$basename.7" $content2 $expected2_7 $db $container "/root" "append" $XmlModify_Element "new" "" 2

    # InsertBefore
    xml014_modify_document $txnenv "\t$basename.8" $content2 $expected2_8 $db $container "/root/a" "insertbefore" $XmlModify_Element "new" ""

    # InsertAfter
    xml014_modify_document $txnenv "\t$basename.9" $content2 $expected2_9 $db $container "/root/a" "insertafter" $XmlModify_Element "new" ""

    # RENAME (elem, attr)
    # element
    xml014_modify_document $txnenv "\t$basename.10" $content2 $expected2_10 $db $container "/root/a" "rename" -1 "x"

    # attribute
    xml014_modify_document $txnenv "\t$basename.11" $content2 $expected2_11 $db $container "/root/a/@att1" "rename" -1 "att2"

    # Update (need to handle normal text, mixed content, and also empty new text,
    #  which acts like a simple removal of all text
    # comment text
    xml014_modify_document $txnenv "\t$basename.12" $content2u $expected2_12 $db $container "/root/comment()" "update" -1 "" "new comment"

    # replace mixed content with single text node
    xml014_modify_document $txnenv "\t$basename.13" $content2u $expected2_13 $db $container "/root/a" "update" -1 "" "new a text"

    # remove text from mixed content
    xml014_modify_document $txnenv "\t$basename.14" $content2u $expected2_14 $db $container "/root/a" "update" -1 "" ""

    # new text for root
    xml014_modify_document $txnenv "\t$basename.15" $content2u $expected2_15 $db $container "/root" "update" -1 "" "new root text"

    # replace simple text node
    xml014_modify_document $txnenv "\t$basename.16" $content2u $expected2_16 $db $container "/root/b" "update" -1 "" "new b text"

    delete container
    delete db
}

#
# Test basic operations on a document with a little structure -
# including encoding changes
#
proc xml014_2_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t\t$basename: with encoding changes"

    xml_database db $testdir $env

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # remove attribute and ascii
    catch {
        xml014_modify_document $txnenv "\t$basename.1" $content2 $expected2_1_ascii $db $container "/root/b/@att1" "remove" -1 "" "" -1 "Ascii"
    } err
    if {$global_container_type == $XmlContainer_NodeContainer} {
        dbxml_error_check_good "$basename.1" [string match "*Cannot*NodeContainer*" $err] 1
    } else {
        dbxml_error_check_good "$basename.1" $err ""
    }

    # append default and windows-1252
    catch {
        xml014_modify_document $txnenv "\t$basename.2" $content2 $expected2_5_windows_1252 $db $container "/root" "append" $XmlModify_Element "new" "" -1 "Windows-1252"
    } err
    if {$global_container_type == $XmlContainer_NodeContainer} {
        dbxml_error_check_good "$basename.2" [string match "*Cannot*NodeContainer*" $err] 1
    } else {
        dbxml_error_check_good "$basename.2" $err ""
    }

    # attribute and iso8859-1
    catch {
        xml014_modify_document $txnenv "\t$basename.3" $content2 $expected2_11_iso8859_1 $db $container "/root/a/@att1" "rename" -1 "att2" "" -1 "ISO8859-1"
    } err
    if {$global_container_type == $XmlContainer_NodeContainer} {
        dbxml_error_check_good "$basename.3" [string match "*Cannot*NodeContainer*" $err] 1
    } else {
        dbxml_error_check_good "$basename.3" $err ""
    }

    # update and utf-8
    catch {
        xml014_modify_document $txnenv "\t$basename.4" $content2u $expected2_12 $db $container "/root/comment()" "update" -1 "" "new comment" -1 "UTF-8"
    } err
    if {$global_container_type == $XmlContainer_NodeContainer} {
        dbxml_error_check_good "$basename.4" [string match "*Cannot*NodeContainer*" $err] 1
    } else {
        dbxml_error_check_good "$basename.4" $err ""
    }

    delete container
    delete db
}

proc xml014_failcheck { val testid } {
    if { $val != 1 } { puts "FAIL:[timestamp] $testid should have thrown" }
}

#
# test some failure conditions
#
proc xml014_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t$basename: test illegal modifications"

    xml_database db $testdir $env

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # insert before/after on root element
    set ret [ catch { xml014_modify_document $txnenv "$basename.1" $content1 $content1 $db $container "/root" "insertbefore" $XmlModify_Element "new" ""} ]
    xml014_failcheck $ret "$basename.1"

    # insert before/after on root element
    set ret [ catch { xml014_modify_document $txnenv "$basename.2" $content1 $content1 $db $container "/root" "insertafter" $XmlModify_Element "new" ""} ]
    xml014_failcheck $ret $basename.2

    # remove root
    set ret [ catch { xml014_modify_document $txnenv "$basename.3" $content1 $content1 $db $container "/root" "remove"} ]
    xml014_failcheck $ret $basename.3

    # attribute without name
    set ret [ catch { xml014_modify_document $txnenv "$basename.4" $content1 $content1 $db $container "/root" "append" $XmlModify_Attribute "" "val"} ]
    xml014_failcheck $ret $basename.4

    # attribute without value
    # 2004/08/20 arw this is legal so moved to 14.1

    # element without name
    # 2004/12/06 gmf this is legal.  moved to 14.1

    # append to attribute
    set ret [ catch { xml014_modify_document $txnenv "$basename.7" $content2 $content1 $db $container "/root/a/@att1" "append" $XmlModify_Attribute "name" "val"} ]
    xml014_failcheck $ret $basename.7

    # append to comment
    set ret [ catch { xml014_modify_document $txnenv "$basename.8" $content2 $content1 $db $container "/root/comment()" "append" $XmlModify_Attribute "name" "val"} ]
    xml014_failcheck $ret $basename.8

    # append elem to attribute
    set ret [ catch { xml014_modify_document $txnenv "$basename.9" $content2 $content1 $db $container "/root/a/@att1" "append" $XmlModify_Element "name" "val"} ]
    xml014_failcheck $ret $basename.9

    # append elem to comment
    set ret [ catch { xml014_modify_document $txnenv "$basename.10" $content2 $content1 $db $container "/root/comment()" "append" $XmlModify_Element "name" "val"} ]
    xml014_failcheck $ret $basename.10

    # use unknown encoding
    catch { xml014_modify_document $txnenv "$basename.11" $content2 $content2 $db $container "" "" -1 "" "" -1 "MadeUp"} err
    if {$global_container_type == $XmlContainer_NodeContainer} {
        dbxml_error_check_good "$basename.11" [string match "*Cannot*NodeContainer*" $err] 1
    } else {
        dbxml_error_check_good "$basename.11" [string match "*Unsupported*encoding*" $err] 1
    }

    delete container
    delete db
}

# update document using DOM node methods
proc xml014_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl

    # get test instructions
    set index_file [file join $test_path document_set_14 index.xml]

    set id {}
    set program {}
    set base_args {}
    set ret [parse_index_file $index_file id program args]
    dbxml_error_check_good "WARNING \"$index_file\" was not recognized as a valid test driver" $ret 1

    # set some arguments to the external program (eval type and DOM update
    #  method are set inside the loop)
    set args {}

    # args to base test driver program (environment, test materials)
    set args [concat $args "--env $testdir"]
    set d [file dirname $index_file]
    set p [file normalize $d]
    set args [concat $args "--datadir $p"]

    # args to UpdateDocumentTest
    list set eval_types {}
    lappend eval_types "eager"
    lappend eval_types "lazy"

    set i 0
    foreach et $eval_types {
        incr i
        xml014_4_run $et $basename.$i $program $args $env $txnenv
    }
}

# actually does the work of the document update tests (calls external program)
proc xml014_4_run { eval_type test_id program base_args env txnenv } {
    puts "\t\t$test_id: $eval_type evaluation"

    set args $base_args
    set args [concat $args "--eval $eval_type"]

    # start each test with a clean environment
    source ./include.tcl
    xml_cleanup $testdir $env
    run_external_test_program $txnenv $test_id $program $args
}

proc xml014_5 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    puts "\n\t$basename: Former update bugs"
    
    xml014_5_1 $env $txnenv $basename.1 $oargs
    xml014_5_2 $env $txnenv $basename.2 $oargs
    xml014_5_3 $env $txnenv $basename.3 $oargs
    xml014_5_4 $env $txnenv $basename.4 $oargs
    xml014_5_5 $env $txnenv $basename.5 $oargs
    xml014_5_6 $env $txnenv $basename.6 $oargs
    xml014_5_7 $env $txnenv $basename.7 $oargs
    xml014_5_8 $env $txnenv $basename.8 $oargs
    xml014_5_9 $env $txnenv $basename.9 $oargs
}

# test a putDocument of a standalone document that's been modified
# using XmlModify.  The content is put from DOM in this instance.
# 2.0.9 had a bug in this area [#11807]
proc xml014_5_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t\t$basename: test put of DOM from a modified document"

    xml_database db $testdir $env

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
    }

    # create the original doc
    wrap XmlDocument xd [$db createDocument]
    set content  "<category name=\"Home\" id=\"root\"><category id=\"49\" name=\"test &amp;\"/></category>";
    $xd setContent $content
    $xd setName $basename

    # modify the doc
    new XmlValue xv $xd

    wrap XmlQueryContext qc [$db createQueryContext $XmlQueryContext_LiveValues]
    wrap XmlUpdateContext uc [$db createUpdateContext]

    wrap XmlModify mod [$db createModify]
    wrap XmlQueryExpression qe [$db prepare "//category\[@id='49'\]" $qc]
    $mod addAppendStep $qe $XmlModify_Element "" "<category id=\"under 49\" name=\"test2 &amp;\" />"

    if { $txnenv == 1 } {
        wrap XmlTransaction xtxn [$db createTransaction]
        $mod execute $xtxn $xv $qc $uc
        $container putDocument $xtxn $xd $uc
        $xtxn commit
        delete xtxn
    } else {
        $mod execute $xv $qc $uc
        $container putDocument $xd $uc
    }
    delete xd
    # validate content
    set expected "<category name=\"Home\" id=\"root\"><category id=\"49\" name=\"test &amp;\"><category id=\"under 49\" name=\"test2 &amp;\"/></category></category>"
    wrap XmlDocument xd1 [$container getDocument $basename]
    dbxml_error_check_good $basename [$xd1 getContentAsString] $expected
    delete xd1

    delete qe
    delete uc
    delete qc
    delete mod
    delete container
    delete db
}

# use the same test data as 14_5_1, above, but make the document
# persistent.  This validates a fix to SR [#12489]
proc xml014_5_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t\t$basename: test adding a child element to a node with an entity in an attr val"

    xml_database db $testdir $env

    set docname "tdoc"
    set content  "<category name=\"Home\" id=\"root\"><category id=\"49\" name=\"test &amp;\"/></category>";

    wrap XmlUpdateContext uc [$db createUpdateContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
	$container putDocument $xtxn $docname $content $uc
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
	$container putDocument $docname $content $uc
    }

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
	wrap XmlDocument xd [$container getDocument $xtxn $docname]
    } else {
	wrap XmlDocument xd [$container getDocument $docname]
    }

    new XmlValue xv $xd
    wrap XmlQueryContext qc [$db createQueryContext $XmlQueryContext_LiveValues]

    wrap XmlModify mod [$db createModify]
    wrap XmlQueryExpression qe [$db prepare "//category\[@id='49'\]" $qc]
    $mod addAppendStep $qe $XmlModify_Element "" "<category id=\"under 49\" name=\"test2 &amp;\" />"

    if { $txnenv == 1 } {
        $mod execute $xtxn $xv $qc $uc
	$container updateDocument $xtxn $xd $uc
	$xtxn commit
	delete xtxn
    } else {
        $mod execute $xv $qc $uc
	$container updateDocument $xd $uc
    }
    delete xv
    delete xd
    # validate content (no txn)
    set expected "<category name=\"Home\" id=\"root\"><category id=\"49\" name=\"test &amp;\"><category id=\"under 49\" name=\"test2 &amp;\"/></category></category>"
    wrap XmlDocument xd1 [$container getDocument $docname]
    dbxml_error_check_good $basename [$xd1 getContentAsString] $expected
    delete xd1

    delete qe
    delete uc
    delete qc
    delete mod
    delete container
    delete db
}

# Test the fix to SR [#12848], which was an error in the
# NsUtil::nsBetweenId() method. If enough elements are inserted
# into the same place in the middle of a child list, the bug is
# exposed.
proc xml014_5_3 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t\t$basename: Inserting many elements into the middle of a child list"

    xml_database db $testdir $env

    set docname "foo"
    set content "<some_table><first_node>nothing</first_node></some_table>";

    wrap XmlUpdateContext uc [$db createUpdateContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $container putDocument $xtxn $docname $content $uc
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
        $container putDocument $docname $content $uc
    }

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlDocument xd [$container getDocument $xtxn $docname]
    } else {
        wrap XmlDocument xd [$container getDocument $docname]
    }

    new XmlValue xv $xd
    wrap XmlQueryContext qc [$db createQueryContext $XmlQueryContext_LiveValues]

    wrap XmlModify mod [$db createModify]
    wrap XmlQueryExpression qe [$db prepare "some_table/first_node" $qc]
    $mod addInsertAfterStep $qe $XmlModify_Element "" "<sample_node><sub_node_1>text</sub_node_1><sub_node_2>text</sub_node_2><sub_node_3>text</sub_node_3><sub_node_4>text</sub_node_4></sample_node>"

    # Perform the modification 100 times
    for {set x 0} {$x < 100} {incr x} {
	set num 0
        if { $txnenv == 1 } {
            catch { set num [$mod execute $xtxn $xv $qc $uc] } ret
        } else {
            catch { set num [$mod execute $xv $qc $uc] } ret
        }
        dbxml_error_check_good "$basename - one mod" $num 1
        dbxml_error_check_good "$basename - no exceptions" $ret 1
    }

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }
    delete xv
    delete xd

    # validate content (no txn)
    wrap XmlResults res [$db query "collection(\"$basename.dbxml\")//sample_node" $qc]
    dbxml_error_check_good "$basename - correct modifications" [$res size] 100
    delete res

    delete qe
    delete uc
    delete qc
    delete mod
    delete container
    delete db
}

# Test the fix to SR [#13820], where indexes weren't being updated
# properly (bug in the KeyStash), and the node level was maintained
# properly (bug in NsUtil::nsCopyNode()).
proc xml014_5_4 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t\t$basename: Append element, and check index update"

    xml_database db $testdir $env

    set docname "foo"
    set content "<users/>";

    wrap XmlUpdateContext uc [$db createUpdateContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlContainer container [$db createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $container addIndex $xtxn "" "users" "node-element-presence" $uc
        $container putDocument $xtxn $docname $content $uc
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$db createContainer $basename.dbxml $oargs $global_container_type]
        $container addIndex "" "users" "node-element-presence" $uc
        $container putDocument $docname $content $uc
    }

    wrap XmlQueryContext qc [$db createQueryContext $XmlQueryContext_LiveValues]
    wrap XmlQueryExpression query [$db prepare "collection('$basename.dbxml')/users" $qc]

    # Run the query to get the element to modify
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlResults xr [$query execute $xtxn $qc]
    } else {
        wrap XmlResults xr [$query execute $qc]
    }

    dbxml_error_check_good "$basename - first query correct" [$xr size] 1

    # Perform the append modification
    wrap XmlModify mod [$db createModify]
    wrap XmlQueryExpression qe [$db prepare "." $qc]
    $mod addAppendStep $qe $XmlModify_Element "" "<user/>"

    if { $txnenv == 1 } {
        $mod execute $xtxn $xr $qc $uc
    } else {
        $mod execute $xr $qc $uc
    }

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }
    delete mod
    delete qe
    delete xr

    # Run the query again to validate
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$db createTransaction]
        wrap XmlResults xr [$query execute $xtxn $qc]
    } else {
        wrap XmlResults xr [$query execute $qc]
    }

    dbxml_error_check_good "$basename - second query correct" [$xr size] 1

    if { $txnenv == 1 } {
        $xtxn commit
        delete xtxn
    }
    delete xr
    delete query
    delete qc

    delete uc
    delete container
    delete db
}



# SR [#14503].  This problem occurred when an attribute was
# removed and another added, when the element had more than
# one attribute in the first place.  The bug was that the
# re-used attribute list entry was not properly zero'd and
# a double-delete occurred
proc xml014_5_5 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t\t$basename: test removing and adding attr when node has more than one"

    xml_database mgr $testdir $env

    set docname "doc"
    set content  "<y id='1' id2='99' />"
    set docname1 "doc1"
    set content1  "<z id='1' id2='2' />"

    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs $global_container_type]
	$container putDocument $xtxn $docname $content $uc
	$container putDocument $xtxn $docname1 $content1 $uc
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
	$container putDocument $docname $content $uc
	$container putDocument $docname1 $content1 $uc
    }

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
	wrap XmlDocument xd [$container getDocument $xtxn $docname]
	wrap XmlDocument xd1 [$container getDocument $xtxn $docname1]
    } else {
	wrap XmlDocument xd [$container getDocument $docname]
	wrap XmlDocument xd1 [$container getDocument $docname1]
    }

    new XmlValue xv $xd
    new XmlValue xv1 $xd1
    wrap XmlQueryContext qc [$mgr createQueryContext $XmlQueryContext_LiveValues]

    wrap XmlModify mod [$mgr createModify]
    wrap XmlQueryExpression qe1 [$mgr prepare "/y/@id" $qc]
    wrap XmlQueryExpression qe2 [$mgr prepare "/y" $qc]
    $mod addRemoveStep $qe1
    $mod addAppendStep $qe2 $XmlModify_Attribute "id" "newid"

    wrap XmlModify mod1 [$mgr createModify]
    wrap XmlQueryExpression qe1_1 [$mgr prepare "/z/@id" $qc]
    wrap XmlQueryExpression qe1_2 [$mgr prepare "/z/@id2" $qc]
    wrap XmlQueryExpression qe2_1 [$mgr prepare "/z" $qc]
    $mod1 addAppendStep $qe2_1 $XmlModify_Attribute "id3" "new"
    $mod1 addAppendStep $qe2_1 $XmlModify_Attribute "id4" "new"
    $mod1 addAppendStep $qe2_1 $XmlModify_Attribute "id5" "new"
    $mod1 addAppendStep $qe2_1 $XmlModify_Attribute "id6" "new1"
    $mod1 addRemoveStep $qe1_1
    $mod1 addRemoveStep $qe1_2

    if { $txnenv == 1 } {
        $mod execute $xtxn $xv $qc $uc
        $mod1 execute $xtxn $xv1 $qc $uc
	$container updateDocument $xtxn $xd $uc
	$xtxn commit
	delete xtxn
    } else {
        $mod execute $xv $qc $uc
        $mod1 execute $xv1 $qc $uc
	$container updateDocument $xd $uc
    }
    delete xv
    delete xv1
    delete xd
    delete xd1

    # validate content (no txn)
    set expected "<y id2=\"99\" id=\"newid\"/>"
    wrap XmlDocument xd_new [$container getDocument $docname]
    dbxml_error_check_good $basename [$xd_new getContentAsString] $expected
    delete xd_new

    set expected "<z id3=\"new\" id4=\"new\" id5=\"new\" id6=\"new1\"/>"
    wrap XmlDocument xd1_new [$container getDocument $docname1]
    dbxml_error_check_good $basename [$xd1_new getContentAsString] $expected
    delete xd1_new

    delete qe1
    delete qe2
    delete qe1_1
    delete qe1_2
    delete qe2_1
    delete uc
    delete qc
    delete mod
    delete mod1
    delete container
    delete mgr

}

# This is only in the 2.3 development branch, and
# was found while investigating [#15141].  15141 was
# a bug in the bulk get buffer reallocation code that 
# resulted from large nodes due to really large node ids.
# The same test data tickled this bug.
proc xml014_5_6 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t\t$basename: test appending relatively complex content"
    xml_database mgr $testdir $env

    set content  "<datapkg><dir name='DNA'/><dir name='RNA'/><dir name='carbohydrate'/><dir name='other'/><dir name='protein'/></datapkg>"

    set newContent { "<file name='f'><_StorageUnit time='Mon May  8 13:20:12 2006' release='1.02' packageName='foo' root='tempData' originator='xmlIO'><parent><key1 tag='molType'>DNA</key1><key2 tag='ccpCode'>A</key2></parent><ChemCompVar _ID='_390' isDefaultVar='true' formalCharge='0' isParamagnetic='false' isAromatic='true' linking='none' varMsdCode='AdF10'><descriptor>deprot:H1</descriptor><stereoSmiles>Nc1ncnc2ncnc123</stereoSmiles><chemAtoms1/><chemAtoms2/><chemAtoms3/><chemAtoms4/><chemAtoms5/></ChemCompVar></_StorageUnit><!--End of Memops Data--></file>"

	"<file name='g'/>"
    }
    
    set expected "<datapkg><dir name=\"DNA\"><file name=\"f\"><_StorageUnit originator=\"xmlIO\" packageName=\"foo\" release=\"1.02\" root=\"tempData\" time=\"Mon May  8 13:20:12 2006\"><parent><key1 tag=\"molType\">DNA</key1><key2 tag=\"ccpCode\">A</key2></parent><ChemCompVar _ID=\"_390\" formalCharge=\"0\" isAromatic=\"true\" isDefaultVar=\"true\" isParamagnetic=\"false\" linking=\"none\" varMsdCode=\"AdF10\"><descriptor>deprot:H1</descriptor><stereoSmiles>Nc1ncnc2ncnc123</stereoSmiles><chemAtoms1/><chemAtoms2/><chemAtoms3/><chemAtoms4/><chemAtoms5/></ChemCompVar></_StorageUnit><!--End of Memops Data--></file><file name=\"g\"/></dir><dir name=\"RNA\"/><dir name=\"carbohydrate\"/><dir name=\"other\"/><dir name=\"protein\"/></datapkg>"

    set docname "doc"

    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs $global_container_type]
	$container putDocument $xtxn $docname $content $uc
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
	$container putDocument $docname $content $uc
    }

    wrap XmlQueryContext qc [$mgr createQueryContext $XmlQueryContext_LiveValues]
    $qc setDefaultCollection $basename.dbxml
    set query "collection()//dir\[@name='DNA'\]"

    foreach newstring $newContent {
	if {$txnenv == 1} {
	    wrap XmlTransaction xtxn [$mgr createTransaction]
	    wrap XmlResults res [$mgr query $xtxn $query $qc]
	} else {
	    wrap XmlResults res [$mgr query $query $qc]
	}
	wrap XmlModify mod [$mgr createModify]
	wrap XmlQueryExpression qe [$mgr prepare "." $qc]
	$mod addAppendStep $qe $XmlModify_Element "" $newstring
	if { $txnenv == 1 } {
	    set num [$mod execute $xtxn $res $qc $uc]
	    $xtxn commit
	    delete xtxn
	} else {
	    set num [$mod execute $res $qc $uc]
	}
	delete mod
	delete qe
	delete res
    }
    # validate content (no txn)
    wrap XmlDocument xd_new [$container getDocument $docname]
    set str [$xd_new getContentAsString]
    dbxml_error_check_good $basename $str $expected

    delete xd_new
    delete uc
    delete qc
    delete container
    delete mgr
}

# This is new in 2.3(.8), reported from the Forums
# [#15291]
# It has to do with removing an element with leading text
# when it's next sibling is also an element.  The assertion
# failure is triggered by a problem with the order of events
# when modifying the transient navigational links in the updated
# NsDom nodes.
proc xml014_5_7 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t\t$basename: test removing element with leading text"
    xml_database mgr $testdir $env

    set content  "<root>  <a id='1'/><a id='2'/>  </root>"

    set docname "doc"

    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    if {$txnenv == 1} {
	set oargs [expr $oargs + $DBXML_TRANSACTIONAL]
        wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
        wrap XmlTransaction xtxn [$mgr createTransaction]
	$container putDocument $xtxn $docname $content $uc
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
	$container putDocument $docname $content $uc
    }

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlDocument doc [$container getDocument $xtxn $docname]
    } else {
        wrap XmlDocument doc [$container getDocument $docname]
    }


    wrap XmlQueryContext qc [$mgr createQueryContext $XmlQueryContext_LiveValues]
    wrap XmlModify mod [$mgr createModify]
    if {$txnenv == 1} {
	wrap XmlQueryExpression qe [$mgr prepare $xtxn "/root/a" $qc]
    } else {
	wrap XmlQueryExpression qe [$mgr prepare "/root/a" $qc]
    }
    $mod addRemoveStep $qe

    new XmlValue xv $doc

    if {$txnenv == 1} {
	set num [$mod execute $xtxn $xv $qc $uc]
	$xtxn commit
	delete xtxn
    } else {
	set num [$mod execute $xv $qc $uc]
    }
    dbxml_error_check_good $basename $num 2
    delete mod
    delete xv
    delete doc
    delete uc
    delete qc
    delete container
    delete mgr
}

# This is new in 2.3(.8), reported from the Forums
# [#15321]
# The code that updates the last descendant node is flawed.
# It walks the parent tree, unconditionally resetting last
# descendant.  It needs to stop when it encounters a node
# that has a next sibling element
proc xml014_5_8 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t\t$basename: test flawed last descendant update"
    xml_database mgr $testdir $env

    # "a" has siblings, "b" and "c"  This will be the
    # query that will fail with the bug.
    set content  "<root><a><d/><e/></a><b/><c/></root>"
    set docname "doc"
    set query "collection()/root/a/following-sibling::*"

    # don't bother with transactions -- they are not involved
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    $container putDocument $docname $content $uc

    wrap XmlDocument doc [$container getDocument $docname]
    wrap XmlQueryContext qc [$mgr createQueryContext $XmlQueryContext_LiveValues]
    $qc setDefaultCollection $basename.dbxml
    wrap XmlModify mod [$mgr createModify]
    # append a child to /root/a/d.  This updates the last descendant
    # of root and a incorrectly (if the bug is present)
    wrap XmlQueryExpression qe [$mgr prepare "/root/a/d" $qc]
    $mod addAppendStep $qe $XmlModify_Element "new" ""
    new XmlValue xv $doc

    set num [$mod execute $xv $qc $uc]
    dbxml_error_check_good $basename $num 1

    # with the bug, this results in one result (e), when it should
    # be 2 (c and d)
    wrap XmlResults res [$mgr query $query $qc]
    dbxml_error_check_good $basename [$res size] 2
    # ensure value
    new XmlValue xv1 
    $res next $xv1
    dbxml_error_check_good $basename [$xv1 asString] "<b/>"
    
    delete xv1
    delete res
    delete mod
    delete xv
    delete doc
    delete uc
    delete qc
    delete container
    delete mgr
}

# This is new in 2.3.8, and was exhibited by the examples
# [#15333]
# The code that inserts a new element along with content was
# flawed.  It first created the new element, but forgot to
# clear a couple of parameters, and change the rest of the
# operation to append.
proc xml014_5_9 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t\t$basename: test bad insertafter of new element and content"
    xml_database mgr $testdir $env

    # Just need simple content
    set content  "<root><a>aval</a><b/><c/></root>"
    set newcontent  "<root><a>aval</a><new><n/><n/>newval</new><b/><c/></root>"
    set docname "doc"
    set query "collection()/root/a"

    # don't bother with transactions -- they are not involved
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    $container putDocument $docname $content $uc

    wrap XmlQueryContext qc [$mgr createQueryContext $XmlQueryContext_LiveValues]
    $qc setDefaultCollection $basename.dbxml
    wrap XmlModify mod [$mgr createModify]
    wrap XmlQueryExpression qe [$mgr prepare $query $qc]
    $mod addInsertAfterStep $qe $XmlModify_Element "new" "<n/><n/>newval"

    new XmlValue xv
    set num [$mod execute $xv $qc $uc]
    dbxml_error_check_good $basename $num 1

    wrap XmlDocument doc [$container getDocument $docname]
    dbxml_error_check_good $basename [$doc getContentAsString] $newcontent
    
    delete mod
    delete xv
    delete doc
    delete uc
    delete qc
    delete container
    delete mgr
}


# test modifications with XmlResults
proc xml014_6 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    puts "\t$basename: test modifications using XmlResults"

    xml014_6_1 $env $txnenv $basename.1 $oargs
    xml014_6_2 $env $txnenv $basename.2 $oargs
}

proc xml014_6_1 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t\t$basename: basic modifications using XmlResults"

    xml_database mgr $testdir $env

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    }
    $container addAlias "cont"


    # append element -- duplicate of /root/a
    xml014_modify_document_with_results $txnenv "xml014.6.1" $rcontent1 \
	$expected_r1 $mgr $container "/root" "append" $XmlModify_Element \
	"" "collection('cont')/root/a"

    # insert a copy of "a" as sibling
    xml014_modify_document_with_results $txnenv "xml014.6.2" $rcontent1 \
	$expected_r2 $mgr $container "/root/a" "insertbefore" $XmlModify_Element \
	"" "collection('cont')/root/a"

    # insert empty element
    xml014_modify_document_with_results $txnenv "xml014.6.3" $rcontent1 \
	$expected_r3 $mgr $container "/root/b" "insertafter" $XmlModify_Element \
	"d" ""

    # insert after comment
    xml014_modify_document_with_results $txnenv "xml014.6.4" $rcontent1 \
	$expected_r4 $mgr $container "/root/comment()" "insertafter" \
	$XmlModify_Element "d" "collection('cont')/root/a"

    # insert multiple siblings, no name
    xml014_modify_document_with_results $txnenv "xml014.6.5" $rcontent1 \
	$expected_r5 $mgr $container "/root/a/c" "append" \
	$XmlModify_Element "" "collection('cont')/root/node()"

    # try to insert attribute
    set exc ""
    catch { xml014_modify_document_with_results $txnenv "xml014.6.4" $rcontent2 \
\		"will_fail" $mgr $container "/root/a" "append" $XmlModify_Attribute \
\		"" "collection('cont')/root/a/c/@cattr"
    } exc

    dbxml_error_check_bad xml014.6.4 $exc ""

    delete container
    delete mgr
}


# test less basic modifications using XmlResults
proc xml014_6_2 { {env "NULL"} {txnenv 0} {basename $tnum} oargs } {
    source ./include.tcl
    source $test_path/xml014_globals.tcl
    puts "\t\t$basename: complex modifications using XmlResults"
    set testid "xml014.6.2"

    xml_database mgr $testdir $env

    set docName "doc"
    # try inserting content that was removed in earlier step.  This should
    # work, and can be used as a "move" mechanism.
    set contentQuery "collection('cont')/root/b"
    set queryRemove "/root/b"
    set queryInsert "/root/a"
    set new_encoding ""

    if {$txnenv == 1} {
        wrap XmlTransaction xtxn [$mgr createTransaction]
        wrap XmlContainer container [$mgr createContainer $xtxn $basename.dbxml $oargs $global_container_type]
        $xtxn commit
        delete xtxn
    } else {
        wrap XmlContainer container [$mgr createContainer $basename.dbxml $oargs $global_container_type]
    }
    $container addAlias "cont"

    
    wrap XmlQueryContext qc [$mgr createQueryContext $XmlQueryContext_LiveValues]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]
    
    wrap XmlDocument xd [$mgr createDocument]
    $xd setContent $rcontent1
    $xd setName $docName
    
    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$mgr createTransaction]
        $container putDocument $txn $xd $uc
        $txn commit
        delete txn
    } else {
        $container putDocument $xd $uc
    }
    
    delete xd
    
    # get results in the same transaction as the modify, if
    # specified

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $docName]
	if { $contentQuery != "" } {
	    wrap XmlResults res [$mgr query $txn $contentQuery $qc]
	} else {
	    wrap XmlResults res [$mgr createResults]
	}
    } else {
        wrap XmlDocument xd [$container getDocument $docName]
	if { $contentQuery != "" } {
	    wrap XmlResults res [$mgr query $contentQuery $qc]
	} else {
	    wrap XmlResults res [$mgr createResults]
	}
    }

    new XmlValue xv $xd

    wrap XmlModify mod [$mgr createModify]

    # get query expressions
    wrap XmlQueryExpression qeRemove [$mgr prepare $queryRemove $qc]
    wrap XmlQueryExpression qeInsert [$mgr prepare $queryInsert $qc]

    $mod addRemoveStep $qeRemove
    $mod addInsertBeforeStep $qeInsert  $XmlModify_Element "" $res

    if {$new_encoding != ""} {
        $mod setNewEncoding $new_encoding
    }

    if { $txnenv == 1 } {
        set num [$mod execute $txn $xv $qc $uc]
    } else {
        set num [$mod execute $xv $qc $uc]
    }

    dbxml_error_check_good $testid $num 2
    dbxml_error_check_good $testid [$xd getContentAsString] $expected_r6

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    delete qeRemove
    delete qeInsert
    delete mod
    delete qc
    delete res
    delete xv
    delete xd


    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlDocument xd1 [$container getDocument $txn $docName]
    } else {
        wrap XmlDocument xd1 [$container getDocument $docName]
    }

    dbxml_error_check_good $testid [$xd1 getContentAsString] $expected_r6

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    delete xd1
    delete uc


    #
    # do same combo, but move a comment...
    #
    set contentQuery "collection('cont')/root/comment()"
    set queryRemove "/root/comment()"
    set queryAppend "/root"

    wrap XmlQueryContext qc1 [$mgr createQueryContext]
    wrap XmlUpdateContext uc [$mgr createUpdateContext]

    if { $txnenv == 1 } {
        wrap XmlTransaction txn [$mgr createTransaction]
        wrap XmlDocument xd [$container getDocument $txn $docName]
	if { $contentQuery != "" } {
	    wrap XmlResults res [$mgr query $txn $contentQuery $qc1]
	} else {
	    wrap XmlResults res [$mgr createResults]
	}
    } else {
        wrap XmlDocument xd [$container getDocument $docName]
	if { $contentQuery != "" } {
	    wrap XmlResults res [$mgr query $contentQuery $qc1]
	} else {
	    wrap XmlResults res [$mgr createResults]
	}
    }


    wrap XmlModify mod [$mgr createModify]

    # get query expressions
    wrap XmlQueryExpression qeRemove1 [$mgr prepare $queryRemove $qc1]
    wrap XmlQueryExpression qeAppend [$mgr prepare $queryAppend $qc1]

    $mod addRemoveStep $qeRemove1
    $mod addAppendStep $qeAppend  $XmlModify_Comment "" $res 0
    if {$new_encoding != ""} {
        $mod setNewEncoding $new_encoding
    }
    new XmlValue xv1 $xd

    if { $txnenv == 1 } {
        set num [$mod execute $txn $xv1 $qc1 $uc]
    } else {
        set num [$mod execute $xv1 $qc1 $uc]
    }
    dbxml_error_check_good $testid $num 2
    dbxml_error_check_good $testid [$xd getContentAsString] $expected_r7

    # clean up -- remove document

    if { $txnenv == 1 } {
	$container deleteDocument $txn $docName $uc
    } else {
	$container deleteDocument $docName $uc
    }

    if { $txnenv == 1 } {
        $txn commit
        delete txn
    }

    delete mod
    delete qeRemove1
    delete qeAppend
    delete xv1
    delete res
    delete qc1
    delete uc
    delete container
    delete mgr
}
