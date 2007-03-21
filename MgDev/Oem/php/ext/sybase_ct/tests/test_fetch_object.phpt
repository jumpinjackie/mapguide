--TEST--
Sybase-CT sybase_fetch_object
--SKIPIF--
<?php 
  require('skipif.inc');
?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id: test_fetch_object.phpt,v 1.1 2004/07/11 16:10:03 thekid Exp $
 */

  require('test.inc');
  
  // {{{ class article
  class article { }
  // }}}
  
  // {{{ resource fetch_object(resource db, [mixed arg= NULL])
  //     Fetches a resultset and returns it as an object
  function fetch_object($db, $arg= NULL) {
    return sybase_fetch_object(sybase_query('select
      1 as "id",
      "Hello" as "caption",
      "timm" as "author",
      getdate() as "lastchange"
    ', $db), $arg);
  }
  // }}}

  $db= sybase_connect_ex();
  
  // Test with stdClass
  var_export(fetch_object($db)); echo "\n";
  
  // Test with userland class
  var_export(fetch_object($db, 'article')); echo "\n";

  // Test with object
  var_export(fetch_object($db, new article())); echo "\n";

  // Test with non-existant class
  var_export(fetch_object($db, '***')); echo "\n";
    
  sybase_close($db);
?>
--EXPECTF--
class stdClass {
  %s $id = 1;
  %s $caption = 'Hello';
  %s $author = 'timm';
  %s $lastchange = '%s';
}
class article {
  %s $id = 1;
  %s $caption = 'Hello';
  %s $author = 'timm';
  %s $lastchange = '%s';
}
class article {
  %s $id = 1;
  %s $caption = 'Hello';
  %s $author = 'timm';
  %s $lastchange = '%s';
}

Notice: sybase_fetch_object(): Sybase:  Class *** has not been declared in %s/test_fetch_object.php on line %d
class stdClass {
  %s $id = 1;
  %s $caption = 'Hello';
  %s $author = 'timm';
  %s $lastchange = '%s';
}
