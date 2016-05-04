#!/bin/bash
rm -f Sheboygan.mgp
cp -f ../../FeatureService/SDF/Sheboygan_BuildingOutlines.sdf Library/Samples/Sheboygan/Data/BuildingOutlines.FeatureSource_DATA_BuildingOutlines.sdf
cp -f ../../FeatureService/SDF/Sheboygan_CityLimits.sdf Library/Samples/Sheboygan/Data/CityLimits.FeatureSource_DATA_CityLimits.sdf
cp -f ../../FeatureService/SDF/Sheboygan_Parcels.sdf Library/Samples/Sheboygan/Data/Parcels.FeatureSource_DATA_Parcels.sdf
cp -f ../../FeatureService/SDF/Sheboygan_VotingDistricts.sdf Library/Samples/Sheboygan/Data/VotingDistricts.FeatureSource_DATA_VotingDistricts.sdf
cp -f ../../MappingService/UT_HydrographicPolygons.sdf Library/Samples/Sheboygan/Data/HydrographicPolygons.FeatureSource_DATA_HydrographicPolygons.sdf
cp -f ../../MappingService/UT_Rail.sdf Library/Samples/Sheboygan/Data/Rail.FeatureSource_DATA_Rail.sdf
cp -f ../../TileService/UT_RoadCenterLines.sdf Library/Samples/Sheboygan/Data/RoadCenterLines.FeatureSource_DATA_RoadCenterLines.sdf
zip -r Sheboygan.mgp Library/ MgResourcePackageManifest.xml
