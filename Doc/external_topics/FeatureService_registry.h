/**
\addtogroup FeatureProviderRegistry
<p>
  The feature provider registry contains most importantly the
  name of each available provider, version information and the name of the provider DLL.
  The full set of registry information elements and attributes is specified
  in the XML schema \link FeatureProviderRegistry_schema FeatureProviderRegistry \endlink.
</p>
<!-- Example (PHP) -->
\htmlinclude PHPExampleTop.html
<p>
  The FeatureProviders constructor takes the XML string
  containing the provider registry as an argument. It creates
  an array of FeatureProvider objects. Currently it only parses
  the Name elements from the registry information. 
</p>
\code
<?php
class FeatureProviders {
   var $xpath;
   var $featureProviders;
   var $logFileHandle;
   function arrayOfProviderNames() {
       return array_keys($this->featureProviders);
   }
   function FeatureProviders( $logFileHandle, $byteReader ) {
      $this->logFileHandle = $logFileHandle;
      $doc = new DOMDocument();
        $xmlStr = $byteReader->ToString();
        $doc->loadXML($xmlStr);
      $this->xpath = new DOMXPath($doc);
      $this->queryProviders();
   }
   function queryProviders() {
      $providers = $this->xpath->query("//FeatureProviderRegistry/FeatureProvider");
      $count = $providers->length;
      for($i=0; $i<$count; $i++) {
             $provider = $providers->item($i);
            $providerElements = $provider->childNodes;
            $connectionProperties = NULL;
             foreach($providerElements as $providerElement) {
                $providerElemTagname = $providerElement->tagName;
                if ($providerElemTagname == "Name") {
                   $providerName = $providerElement->nodeValue;
                   $featureProvider = new FeatureProvider($providerName);
                } 
             }
           $this->featureProviders[$providerName] = $featureProvider;
      }
   }
}
class FeatureProvider {
   var $providerName;
   var $connectionProperties = NULL;
   function FeatureProvider( $providerName ) {
      $this->providerName = $providerName;
   }
   function GetProviderName() {
      return $providerName;
   }
}
?>
\endcode
\htmlinclude ExampleBottom.html

<!-- Example (C#) -->
\htmlinclude CSharpExampleTop.html
<p>
	The GetProviderNames method gets the provider registry from Feature Services.
	The registry is in xml and the method uses an xpath expression to retrieve
	the provider names. It concatenates the provider names for use in a test expression.
</p>
<p>
	The following string is an example of the output from this program.
	"Autodesk.Oracle.3.2 Autodesk.Raster.3.2 Autodesk.SqlServer.3.2 OSGeo.ArcSDE.3.2 OSGeo.MySQL.3.2 OSGeo.ODBC.3.2 OSGeo.SDF.3.2 OSGeo.SHP.3.2 OSGeo.WFS.3.2 OSGeo.WMS.3.2"
<p>
	The utility method shows the use of System Xml classes to extract values
	from the xml.
</p>
\code
using OSGeo.MapGuide;
// The MgFeatureService example code shows how the MgFeatureService object is created.
private MgFeatureService featureService;
private String providerNamesActual;

private void GetProviderNames()
{
	System.Globalization.CultureInfo culture = new System.Globalization.CultureInfo(0x0409);
	String[] names;
	MgByteReader byteReader = featureService.GetFeatureProviders();
	String xmlContent = byteReader.ToString();
	names = GetXpathValuesRtnStrArr(xmlContent,
		"//FeatureProviderRegistry/FeatureProvider/Name");
	providerCountActual = names.GetLength(0);
	Array.Sort(names, StringComparer.Create(culture, false));
	providerNamesActual = StringArrayToString(names);
}

private String[] GetXpathValuesRtnStrArr(String xmlContent, String xpath)
{
	System.Collections.ArrayList strList = new System.Collections.ArrayList(10);
	String[] strArr;
	System.Xml.XmlDocument xmlDocument = new XmlDocument();
	xmlDocument.LoadXml(xmlContent);
	XmlNodeList nodeList;
	XmlElement root = xmlDocument.DocumentElement;
	nodeList = root.SelectNodes(xpath);
	int count = nodeList.Count;
	if (count == 0)
	{
		strArr = new String[] { "" };
		return strArr;
	}
	XmlNode node;
	for (int i = 0; i < count; i++)
	{
		node = nodeList.Item(i);
		strList.Add(node.FirstChild.Value);
	}
	strArr = (String[])strList.ToArray(typeof(string));
	return strArr;
}

GetProviderNames();
System.Console.WriteLine(providerNamesActual);
\endcode
\htmlinclude ExampleBottom.html

**/ 
