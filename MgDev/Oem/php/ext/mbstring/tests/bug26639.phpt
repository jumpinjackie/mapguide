--TEST--
Bug #26639 (mb_convert_variables() clutters variables beyond the references)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$a = "あいうえお";
$b = $a;
mb_convert_variables("EUC-JP", "Shift_JIS", $b);
debug_zval_dump($a);
debug_zval_dump($b);
unset($a);
unset($b);

$a = "あいうえお";
$b = &$a;
mb_convert_variables("EUC-JP", "Shift_JIS", $b);
debug_zval_dump($a);
debug_zval_dump($b);
unset($a);
unset($b);

$a = "あいうえお";
$b = array($a);
$c = $b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);

$a = "あいうえお";
$b = array(&$a);
$c = $b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);

$a = "あいうえお";
$b = array($a);
$c = &$b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);

$a = "あいうえお";
$b = array(&$a);
$c = &$b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);

$a = array(array("あいうえお"));
$b = $a;
$c = $b;
mb_convert_variables("EUC-JP", "Shift_JIS", $c);
debug_zval_dump($b);
debug_zval_dump($c);
unset($a);
unset($b);
unset($c);
?>
--EXPECT--
string(10) "あいうえお" refcount(2)
string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(2)
string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(1)
string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(1)
array(1) refcount(2){
  [0]=>
  string(10) "あいうえお" refcount(2)
}
array(1) refcount(2){
  [0]=>
  string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(1)
}
array(1) refcount(2){
  [0]=>
  &string(10) "あいうえお" refcount(2)
}
array(1) refcount(2){
  [0]=>
  string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(1)
}
array(1) refcount(1){
  [0]=>
  string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(2)
}
array(1) refcount(1){
  [0]=>
  string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(2)
}
array(1) refcount(1){
  [0]=>
  string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(2)
}
array(1) refcount(1){
  [0]=>
  string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(2)
}
array(1) refcount(3){
  [0]=>
  array(1) refcount(1){
    [0]=>
    string(10) "あいうえお" refcount(1)
  }
}
array(1) refcount(2){
  [0]=>
  array(1) refcount(1){
    [0]=>
    string(10) "､｢､､､ｦ､ｨ､ｪ" refcount(1)
  }
}
