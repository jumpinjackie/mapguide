import schema namespace ipo="http://www.example.com/IPO";

declare function local:names-match( $s as element(ipo:purchaseOrder/shipTo), 
                                    $b as element(ipo:purchaseOrder/billTo) )
  as xs:boolean
{
     $s/name = $b/name
};
 
for $p in doc("ipo.xml")//element(ipo:purchaseOrder)
where not( local:names-match( $p/shipTo, $p/billTo ) )
return $p