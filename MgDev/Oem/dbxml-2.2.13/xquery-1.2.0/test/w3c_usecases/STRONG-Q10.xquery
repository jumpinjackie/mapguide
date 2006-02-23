import schema namespace ipo="http://www.example.com/IPO";

declare function local:deadbeat( $b as element(billTo, ipo:USAddress) )
  as xs:boolean
{
   $b/name = doc("http://www.usa-deadbeats.com/current")/deadbeats/row/name
}; 

for $p in doc("ipo.xml")//element(ipo:purchaseOrder)
where $p/@orderDate = current-date()
  and local:deadbeat( $p/element(billTo) )
return <warning>{ $p/billTo/name, "is a known deadbeat!" }</warning> 
