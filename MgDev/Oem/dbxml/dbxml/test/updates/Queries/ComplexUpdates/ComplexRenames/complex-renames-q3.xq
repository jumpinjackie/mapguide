(: Name: complex-renames-q3 :)
(: Description: Rename the element called "center". :)

(: insert-start :)
declare variable $input-context external;
(: insert-end :)

rename node $input-context/far-north/north/near-north/center as "middle"
