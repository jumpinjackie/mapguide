(: Name: complex-deletes-q13 :)
(: Description: Delete the attributes names "west-attr-2", "center-attr-2", and "south-attr-2". :)

(: insert-start :)
declare variable $input-context external;
(: insert-end :)

delete nodes $input-context//(@west-attr-2 | @center-attr-2 | @south-attr-2)
