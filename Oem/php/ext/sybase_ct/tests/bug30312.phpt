--TEST--
Sybase-CT bug #30312 (sybase_unbuffered_query calls)
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id: bug30312.phpt,v 1.1 2005/02/06 11:58:53 thekid Exp $
 */

  require('test.inc');
  error_reporting(error_reporting() & !E_NOTICE);       // Suppress notices

  $db= sybase_connect_ex();

  $query= sybase_unbuffered_query('select getdate()');
  $array= sybase_fetch_array($query);
  var_dump($array[0]);

  $query= sybase_unbuffered_query('select getdate()');
  $array= sybase_fetch_array($query);
  var_dump($array[0]);
?>
--EXPECTF--
string(%d) "%s  %d %d %d:%d%s"
string(%d) "%s  %d %d %d:%d%s"
