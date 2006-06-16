import schema namespace ipo = "http://www.example.com/IPO";
                        
count( 
  doc("ipo.xml")//shipTo[. instance of element(*, ipo:UKAddress)]
)
