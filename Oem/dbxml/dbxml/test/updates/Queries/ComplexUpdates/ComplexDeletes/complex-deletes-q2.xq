(: Name: complex-deletes-q2 :)
(: Description: Delete trailing comments of document element. :)

(: insert-start :)
declare variable $input-context external;
(: insert-end :)

delete nodes $input-context/comment()[. >> $input-context/*]
