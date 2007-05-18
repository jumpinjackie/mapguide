/**
\addtogroup FeatureProviderRegistry
<p>
  The feature provider registry contains most importantly the
  names of the available providers and for each provider, the
  names of the properties used to connect to it. The full set
  of registry information elements and attributes is specified
  in the XML schema \link FeatureProviderRegistry_schema FeatureProviderRegistry\endlink.
</p>
<h2>Example (PHP)</h2>
<p>
  The FeatureProviders constructor takes the XML string
  containing the provider registry as an argument. It creates
  an array of FeatureProvider objects. Currently it only parses
  the Name and ConnectionProperties elements from the registry
  information. It ignores the DisplayName, Description,
  Version, and FeatureDataObjectsVersion elements.
</p>
<pre>
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
   function getConnectionPropertyNames($providerName) {
      $arrayOfNames = $this->featureProviders[$providerName]->GetConnectionPropertyNames();
      return $arrayOfNames;
   }
    function logProvider($providerName) {
        fwrite($this->logFileHandle, "$providerName Connection Properties\\n");
        $provider = $this->featureProviders[$providerName];
        $connectionPropNames = $provider->GetConnectionPropertyNames();
        foreach($connectionPropNames as $connectionPropName) {
            $enumerable = $provider->IsEnumerable($connectionPropName);
            $protected = $provider->IsProtected($connectionPropName);
            $required = $provider->IsRequired($connectionPropName);
            fwrite($this->logFileHandle, "$connectionPropName: enumerable: $enumerable; protected: $protected; required: $required\\n");
        }
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
                } elseif ($providerElemTagname == "ConnectionProperties") {
                   $connectionPropertyNodes = $providerElement->childNodes;
                   foreach($connectionPropertyNodes as $connectionPropertyNode) {
                        $propertyAttributes = $connectionPropertyNode->attributes;
                        $enumerable = $propertyAttributes->getNamedItem("Enumerable");
                        $enumVal = $enumerable->value;
                        $protected = $propertyAttributes->getNamedItem("Protected");
                        $protectedVal = $protected->value;
                        $required = $propertyAttributes->getNamedItem("Required");
                        $requiredVal = $required->value;
                          $propertyElements = $connectionPropertyNode->childNodes;
                        $propertyName = NULL;
                          foreach($propertyElements as $propertyElement) {
                             $propertyTagname = $propertyElement->tagName;
                             if ($propertyTagname == "Name") {
                                $propertyName = $propertyElement->nodeValue;
                                break;
                            }
                        }
                        $connectionProperties[$propertyName] = array("enumerable" => $enumVal, "protected" => $protectedVal, "required" => $requiredVal);
                   }
                }
             }
            $featureProvider->AddConnectionProperties($connectionProperties);
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
   function AddConnectionProperties( $connectionProperties ) {
       $this->connectionProperties = $connectionProperties;
   }
   function GetConnectionPropertyNames() {
       return array_keys($this->connectionProperties);
   }
   function IsEnumerable($connectionPropertyName) {
       return $this->connectionProperties[$connectionPropertyName]["enumerable"];
   }
   function IsProtected($connectionPropertyName) {
      return $this->connectionProperties[$connectionPropertyName]["protected"];
   }
   function IsRequired($connectionPropertyName) {
      return $this->connectionProperties[$connectionPropertyName]["required"];
   }
}
?>
</pre>
**/ 