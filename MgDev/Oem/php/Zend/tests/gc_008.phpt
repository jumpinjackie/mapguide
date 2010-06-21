--TEST--
GC 008: Unreferensed object cycle
--FILE--
<?php
$a = new stdClass();
$a->a = new stdClass();
$a->a->a = $a->a;
var_dump($a->a);
var_dump(gc_collect_cycles());
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
object(stdClass)#2 (1) {
  ["a"]=>
  object(stdClass)#2 (1) {
    ["a"]=>
    *RECURSION*
  }
}
int(0)
int(1)
ok
