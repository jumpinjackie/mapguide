(: Name: complex-deletes-q7 :)
(: Description: Delete all text nodes that come directly after a comment or PI. :)

(: insert-start :)
declare variable $input-context external;
(: insert-end :)

let $a := $input-context//far-south/preceding::text()
return
  <result count="{count($a)}">{$a}</result>,
let $a := $input-context//far-south/following::text()
return
  <result count="{count($a)}">{$a}</result>
