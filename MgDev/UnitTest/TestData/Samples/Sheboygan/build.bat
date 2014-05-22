@echo off
if exist Sheboygan.mgp del /F Sheboygan.mgp
copy /Y ..\..\FeatureService\SDF\Sheboygan_BuildingOutlines.sdf Library\Samples\Sheboygan\Data\BuildingOutlines.FeatureSource_DATA_BuildingOutlines.sdf
copy /Y ..\..\FeatureService\SDF\Sheboygan_CityLimits.sdf Library\Samples\Sheboygan\Data\CityLimits.FeatureSource_DATA_CityLimits.sdf
copy /Y ..\..\FeatureService\SDF\Sheboygan_Parcels.sdf Library\Samples\Sheboygan\Data\Parcels.FeatureSource_DATA_Parcels.sdf
copy /Y ..\..\FeatureService\SDF\Sheboygan_VotingDistricts.sdf Library\Samples\Sheboygan\Data\VotingDistricts.FeatureSource_DATA_VotingDistricts.sdf
copy /Y ..\..\MappingService\UT_HydrographicPolygons.sdf Library\Samples\Sheboygan\Data\HydrographicPolygons.FeatureSource_DATA_HydrographicPolygons.sdf
copy /Y ..\..\MappingService\UT_Rail.sdf Library\Samples\Sheboygan\Data\Rail.FeatureSource_DATA_Rail.sdf
copy /Y ..\..\TileService\UT_RoadCenterLines.sdf Library\Samples\Sheboygan\Data\RoadCenterLines.FeatureSource_DATA_RoadCenterLines.sdf
7z a -tzip Sheboygan.mgp Library/ MgResourcePackageManifest.xml