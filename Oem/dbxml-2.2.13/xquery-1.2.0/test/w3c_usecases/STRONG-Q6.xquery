import schema namespace ipo="http://www.example.com/IPO";

for $p in doc("ipo.xml")//element(ipo:purchaseOrder),
    $b in $p/billTo
where not( $b instance of element(*, ipo:USAddress))
  and exists( $p//USPrice )
return $p
