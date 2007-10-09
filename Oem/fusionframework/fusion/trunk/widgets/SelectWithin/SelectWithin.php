<?php

//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

  $fusionMGpath = '../../MapGuide/php/';
  include $fusionMGpath . 'Common.php';

  $mapName = "";
  $sessionId = "";
  $layers = null;
  $queryInfo = false;   //seemed to be always set to true in MG version

  GetRequestParameters();

  try {
    $featureSrvc = $siteConnection->CreateService(MgServiceType::FeatureService);
    $renderingSrvc = $siteConnection->CreateService(MgServiceType::RenderingService);

    //load the map runtime state
    $map = new MgMap();
    $map->Open($resourceService, $mapName);

    $layers = explode(",", $layers);
    if (count($layers) > 0) {

      $layerNames = new MgStringCollection();
      for ($i = 0; $i < count($layers); $i++)
        $layerNames->Add($layers[$i]);

      // create a multi-polygon or a multi-geometry containing the input selected features
      $inputGeom = MultiGeometryFromSelection($featureSrvc, $resourceService, $map, $mapName);
      if ($inputGeom) {
        // Query all the features belonging the the layer list that intersects with the input geometries
        $fi = $renderingSrvc->QueryFeatures($map, $layerNames, $inputGeom, MgFeatureSpatialOperations::Intersects, -1);
        if ($fi) {
          $resultSel = $fi->GetSelection();
          if( $resultSel) {
            //return XML
            header("Content-type: text/xml");
            echo $resultSel->ToXml();
          }
        }
      }
    }
  } catch(MgException $e) {
    echo "\nException: " . $e->GetDetails();
    return;
  } catch(Exception $ne) {
    return;
  }

function MultiGeometryFromSelection($featureSrvc, $resourceSrvc, $map, $mapName)
{
    $sel = new MgSelection($map);
    $sel->Open($resourceSrvc, $mapName);
    $selLayers = $sel->GetLayers();
    $geomColl = new MgGeometryCollection();
    $agfRW = new MgAgfReaderWriter();
    $polyOnly = true;

    for($i = 0; $i < $selLayers->GetCount(); $i++)
    {
        $layer = $selLayers->GetItem($i);
        $filter = $sel->GenerateFilter($layer, $layer->GetFeatureClassName());
        $query = new MgFeatureQueryOptions();
        $query->SetFilter($filter);
        $featureSource = new MgResourceIdentifier($layer->GetFeatureSourceId());
        $features = $featureSrvc->SelectFeatures($featureSource, $layer->GetFeatureClassName(), $query);
        if($features)
        {
            $classDef = $features->GetClassDefinition();
            $geomPropName = $classDef->GetDefaultGeometryPropertyName();
            $j = 0;
            $isPoly = true;
            while($features->ReadNext())
            {
                $geomReader = $features->GetGeometry($geomPropName);
                $geom = $agfRW->Read($geomReader);
                if($j ++ == 0)
                {
                    $type = $geom->GetGeometryType();
                    if($type == MgGeometryType::MultiPolygon || $type == MgGeometryType::CurvePolygon || $type == MgGeometryType::MultiCurvePolygon)
                    {
                        $isPoly = false;
                        $polyOnly = false;
                    }
                    else if($type != MgGeometryType::Polygon)
                        break;
                }
                $geomColl->Add($geom);
            }
        }
    }
    if($geomColl->GetCount() == 0)
        return null;

    $gf = new MgGeometryFactory();
    if($polyOnly)
    {
        $polyColl = new MgPolygonCollection();
        for($i = 0; $i < $geomColl->GetCount(); $i++)
            $polyColl->Add($geomColl->GetItem($i));
        return $gf->CreateMultiPolygon($polyColl);
    }
    else
        return $gf->CreateMultiGeometry($geomColl);
}

function GetParameters($params)
{
    global $layers, $mapName, $sessionId, $queryInfo;

    $mapName = $params['mapname'];
    $sessionId = $params['session'];
    $layers = $params['layers'];
    if(isset($params['queryinfo']))
        $queryInfo = $params['queryinfo'] == "1";
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}
?>
