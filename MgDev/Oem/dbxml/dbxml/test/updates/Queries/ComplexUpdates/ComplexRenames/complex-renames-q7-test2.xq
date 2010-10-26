(: Name: complex-renames-q7 :)
(: Description: Rename all attributes on the "west" element. :)

(: insert-start :)
declare variable $input-context external;
(: insert-end :)

let $a := $input-context//near-north/*/@*
return
  <result count="{count($a)}">{for $b in $a order by name($b), string($b) return <attr>{$b}</attr>}</result>,

let $a := $input-context//@at1
return
  <result count="{count($a)}">{for $b in $a order by name($b), string($b) return <attr>{$b}</attr>}</result>,

let $a := $input-context//@mark
return
  <result count="{count($a)}">{for $b in $a order by name($b), string($b) return <attr>{$b}</attr>}</result>

