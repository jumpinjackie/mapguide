--TEST--
Sybase-CT bug #27843 (notices when query is a stored procedure)
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id: bug27843.phpt,v 1.3 2005/02/06 12:59:03 thekid Exp $ 
 */

  require('test.inc');

  $db= sybase_connect_ex();

  // Check if stored procedure already exists
  $sp_name= 'phpt_bug27843';
  var_dump(sybase_select_db('tempdb', $db));
  if (!sybase_select_single($db, 'select object_id("'.$sp_name.'")')) {
    echo "Stored procedure {$sp_name} not found, creating\n";
    var_dump(sybase_query('
      create proc '.$sp_name.'
      as
      begin
        select 1
      end
    '));
  } else {
    echo "Stored procedure {$sp_name} found, using\n";
    var_dump(TRUE);
  }
  
  // Execute stored procedure
  var_dump(sybase_select_ex($db, 'exec '.$sp_name));

  // Clean up after ourselves
  var_dump(sybase_query('drop proc '.$sp_name, $db));

  sybase_close($db);
?>
--EXPECTF--
bool(true)
Stored procedure %s
bool(true)
>>> Query: exec phpt_bug27843
<<< Return: resource
array(1) {
  [0]=>
  array(1) {
    ["computed"]=>
    int(1)
  }
}
bool(true)
