<?php

//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
    include 'displayschemafunctions.php';
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

    <head>
        <title><?php echo HtmlTitles::DisplaySchema ?></title>
        <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
        <link href="displayschema.css" rel="stylesheet" type="text/css">
    </head>

    <body>

        <?php

            $sessionId = $_GET['sessionId'];
            $resName = $_GET['resId'];
            $schemaName = $_GET['schemaName'];
            $className = $_GET['className'];
            $index = $_GET['index'];
            $listIndex = 0;
            $totalEntries = 0;
            $maxEntries = Constants::MaxEntriesPerPage;

            try
            {
                $thisFile = __FILE__;
                $pos = strrpos($thisFile, '\\');
                if ($pos == false)
                    $pos = strrpos($thisFile, '/');
                $configFilePath = substr($thisFile, 0, $pos+1) . "../webconfig.ini";
                MgInitializeWebTier ($configFilePath);

                $userInfo = new MgUserInformation($sessionId);
                $userInfo->SetClientIp(GetClientIp());
                $userInfo->SetClientAgent(GetClientAgent());
                $site = new MgSiteConnection();
                $site->Open($userInfo);

                $featureSrvc = $site->CreateService(MgServiceType::FeatureService);
                $resourceSrvc = $site->CreateService(MgServiceType::ResourceService);
                $resId = new MgResourceIdentifier($resName);

                $schemaName = substr(strrchr($schemaName, "/"), 1);
                $classDef = $featureSrvc->GetClassDefinition($resId, $schemaName, $className);
                $geomName = $classDef->GetDefaultGeometryPropertyName();

                $qualifiedClassName = $schemaName . ":" . $className;
                // Calculate total number of entries.
                $totalEntries = GetFeatureCount($resId, $schemaName, $className, $resourceSrvc, $featureSrvc);

                $currentPage = ceil(($index+$maxEntries)/$maxEntries);
                $maxPage = ceil($totalEntries/$maxEntries);
                $lastEntry = ($maxPage-1)*$maxEntries;

                $propertyList = $classDef->GetProperties();
                echo '<h1>' . sprintf(DisplayHeadings::ClassName, $className) . '</h1>';

                echo 'Total Entries: ' . $totalEntries . '<br><br>';

                if($totalEntries>$maxEntries)
                {
                    DisplayPaging($index, $resName, $schemaName, $className, $sessionId, $maxEntries, $currentPage, $maxPage, $lastEntry);
                    echo '<br><br>';
                }

                $propertyNameTypeList = array();
                echo '<table class="data" cellspacing="0"><tr>';
                for($i=0; $i<$propertyList->GetCount(); $i++)
                {
                    $propertyDef = $propertyList->GetItem($i);
                    $property = $propertyList->GetItem($i)->GetName();
                    if(($property!=$geomName)&&($propertyDef->GetPropertyType()==100))
                    {
                        echo '<td class="heading">' . $property . '</td>';
                        $propertyNameTypeList[$listIndex][0] = $property;
                        $propertyNameTypeList[$listIndex++][1] = $propertyList->GetItem($i)->GetDataType();
                    }
                }
                echo '</tr>';

                try
                {
                    $query = BuildFeatureQueryOptions($classDef);
                    $featureReader = $featureSrvc->SelectFeatures($resId, $qualifiedClassName, $query);

                    // Find the correct index on featureReader
                    $count = $index;
                    while($count--!=0)
                        $featureReader->ReadNext();

                    // Output property values
                    for($j=0; $j<$maxEntries;$j++)
                    {
                        echo '<tr>';
                        if($featureReader->ReadNext()==false)
                            break;
                        for($k=0; $k<count($propertyNameTypeList);$k++)
                        {
                            try{
                                $property = GetPropertyName($featureReader, $propertyNameTypeList[$k][0], $propertyNameTypeList[$k][1]);

                                if(strlen($property))
                                    echo '<td nowrap>' . $property . '</td>';
                                else
                                    echo '<td>' . FormText::EmptyString . '</td>';
                            }
                            catch (MgException $joinE)
                            {
                                // To allow display for a left outer join. The secondary class records
                                // can be null if it contains less data than the primary class.
                            }
                        }
                        echo '</tr>';
                    }

                    $featureReader->Close();
                }
                catch (MgException $schemaE)
                {
                    // To prevent error msg when same className is found in multiple schemas
                }

                echo '</table><br>';

                if($totalEntries>$maxEntries)
                    DisplayPaging($index, $resName, $schemaName, $className, $sessionId, $maxEntries, $currentPage, $maxPage, $lastEntry);
            }
            catch (MgSessionExpiredException $s)
            {
                echo ErrorMessages::SessionExpired;
            }
            catch (MgException $e)
            {
                echo $e->GetExceptionMessage();
            }

        ?>

    </body>

</html>
