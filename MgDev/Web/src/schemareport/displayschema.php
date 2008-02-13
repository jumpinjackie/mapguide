<?php

//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

    include '../mapadmin/constants.php';
    include 'stringconstants.php';
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

    <head>
        <title><?php echo HtmlTitles::DisplaySchema ?></title>
        <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
        <link href="displayschema.css" rel="stylesheet" type="text/css">
    </head>

    <body onLoad="ShowFirstClass()">

        <?php

            $sessionId = $_GET['sessionId'];
            $resName = $_GET['resId'];
            $schemaName = $_GET['schemaName'];
            $className = $_GET['className'];
            $viewer = $_GET['viewer'];

            try
            {
                $thisFile = __FILE__;
                $pos = strrpos($thisFile, '\\');
                if ($pos == false)
                    $pos = strrpos($thisFile, '/');
                $configFilePath = substr($thisFile, 0, $pos+1) . "../webconfig.ini";
                MgInitializeWebTier ($configFilePath);

                $userInfo = new MgUserInformation($sessionId);
                $site = new MgSiteConnection();
                $site->Open($userInfo);

                $featureSrvc = $site->CreateService(MgServiceType::FeatureService);

                $resId = new MgResourceIdentifier($resName);

                $classCollection = $featureSrvc->GetClasses($resId, $schemaName);
                $firstClass =  substr(strrchr($classCollection->GetItem(0), ":"), 1);

                $xml = $featureSrvc->DescribeSchemaAsXml($resId, $schemaName);

                // Parse the xml for encoded characters, however, the 'xmlns' attribute under 'targetNamespace' element
                // cannot contain the translated or encoded characters. This replaces '-xffXX-' with 'ffXX' for that
                // particular string while the rest of the encoded chars are translated properly.
                $subXml = substr($xml, strpos($xml, 'xs:element'), -1);
                $subXml = preg_replace("/-x([A-Za-z0-9]{1,4})-/e", "html_entity_decode('&#'.hexdec('$1').';',ENT_NOQUOTES,'UTF-8')", $subXml);
                $xml = substr_replace($xml, $subXml, strpos($xml, 'xs:element'), -1);

                $nameSpaceStart = strpos($xml, 'targetNamespace="');
                $nameSpaceEnd = strpos($xml, '"', $nameSpaceStart+17);
                $nameSpace = substr($xml, $nameSpaceStart, $nameSpaceEnd-$nameSpaceStart);
                $nameSpaceModified = preg_replace("/-x([A-Za-z0-9]{1,4})-/e", "html_entity_decode('&#'.hexdec('$1').';',ENT_NOQUOTES,'UTF-8')", $nameSpace);

                $xml = str_replace($nameSpace, $nameSpaceModified, $xml);
                $xml = preg_replace("/-x([A-Za-z0-9]{1,4})-/e", "'$1'", $xml);

                $xsl_file = 'displayschema.xsl';

                // Create new object for the XML file
                $xml_object = new DomDocument();
                if(!$xml)
                    exit(ErrorMessages::XmlNotFound);
                $xml_object->loadXML($xml);

                // Create new object for the XSL stylesheet
                $xsl_object = new DomDocument();
                if(!file_exists($xsl_file))
                    exit(ErrorMessages::XslNotFound);
                $xsl_object->load($xsl_file);

                // Configure the transformer
                $proc = new XSLTProcessor;
                $proc->importStyleSheet($xsl_object);

                $proc->setParameter('', 'schemaName', $schemaName);
                $proc->setParameter('', 'className', $className);
                $proc->setParameter('', 'resName', $resName);
                $proc->setParameter('', 'sessionId', $sessionId);
                $proc->setParameter('', 'viewer', $viewer);

                // Set localized strings
                $proc->setParameter('', 'stringSchema', XslStrings::Schema);
                $proc->setParameter('', 'stringClassTitle', XslStrings::ClassTitle);
                $proc->setParameter('', 'stringViewData', XslStrings::ViewData);
                $proc->setParameter('', 'stringViewFeature', XslStrings::ViewFeature);
                $proc->setParameter('', 'stringDataProperties', XslStrings::DataProperties);
                $proc->setParameter('', 'stringGeometricProperties', XslStrings::GeometricProperties);
                $proc->setParameter('', 'stringPropertyName', XslStrings::PropertyName);
                $proc->setParameter('', 'stringPropertyType', XslStrings::PropertyType);
                $proc->setParameter('', 'stringHasMeasures', XslStrings::HasMeasures);
                $proc->setParameter('', 'stringHasElevation', XslStrings::HasElevation);
                $proc->setParameter('', 'stringNoGeometry', XslStrings::NoGeometry);

                $html = $proc->transformToXML($xml_object);

                print($html);
            }
            catch (MgException $e)
            {
                echo $e->GetMessage();
            }

        ?>

    </body>

    <script language="JavaScript">
        function Toggle(objId)
        {
            if(document.getElementById(objId).style.display == 'none')
            {
                document.getElementById(objId).previousSibling.firstChild.src = "./images/collapse_pane.png";
                document.getElementById(objId).style.display = 'block';
            }
            else
            {
                document.getElementById(objId).previousSibling.firstChild.src = "./images/expand_pane.png";
                document.getElementById(objId).style.display = 'none';
            }
        }

        function ShowFirstClass()
        {
            document.getElementById("Data_"+"<?php echo $firstClass?>").previousSibling.firstChild.src = "./images/collapse_pane.png";
            document.getElementById("Data_"+"<?php echo $firstClass?>").style.display = 'block';
            document.getElementById("Geom_"+"<?php echo $firstClass?>").previousSibling.firstChild.src = "./images/collapse_pane.png";
            document.getElementById("Geom_"+"<?php echo $firstClass?>").style.display = 'block';
        }
    </script>

</html>
