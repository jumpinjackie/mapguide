import schema namespace ipo="http://www.example.com/IPO";

declare function local:comments-for-element( $e as element() )
  as ipo:comment*
{
  let $c := $e/element(ipo:comment)
  return $c
};

for $p in doc("ipo.xml")//element(ipo:purchaseOrder)
where $p/shipTo/name="Helen Zoe"
  and $p/@orderDate = xs:date("1999-12-01")
return 
  <comments name="{$p/shipTo/name}" date="{$p/@orderDate}">
   {
     local:comments-for-element( $p )
   }
   {
     for $i in $p//item
     return local:comments-for-element( $i )
   }
  </comments>
