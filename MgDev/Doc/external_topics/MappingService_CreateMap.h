/**
\addtogroup CreateAMap
<!-- Example (C#) -->
\htmlinclude CSharpExampleTop.html
\code
using OSGeo.MapGuide;

private MgFeatureService featureService;

String mapCRS = "LOCAL_CS[\"Non-Earth (Mile)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Mile\", 1609.3472186944373],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
double localCSMaxX = 10000000;
double localCSMaxY = 10000000;
double localCSMinX = -10000000;
double localCSMinY = -10000000;
MgEnvelope mapExtent = new MgEnvelope(localCSMinX, localCSMinY, localCSMaxX, localCSMaxY);
String mapName = "Map";
MgMapBase map = new MgMapBase();
map.Create(mapCRS, mapExtent, mapName);

StringCollection layerResourceIds;
String sdfFeatureSourceResourcePath =  "Library://PlatformApiDocTests/SdfFeatureClass.FeatureSource";
MgResourceIdentifier featureSourceResId = new MgResourceIdentifier(sdfFeatureSourceResourcePath);
layerResIds = AddAllLayersToMap(map, featureClassResId);

public StringCollection AddAllLayersToMap(MgMapBase map, MgResourceIdentifier featureSourceId)
{
	StringCollection layerResIds = new StringCollection();

	StringCollection schemaNames = featureService.GetSchemas(featureSourceId);
	foreach (string schemaName in schemaNames)
	{
		// If a schema contains multiple classes, a group will be added
		StringCollection classNames = featureService.GetClasses(featureSourceId, schemaName);
		string groupName = "";
		if (classNames.Count > 1)
		{
			groupName = PopulateUniqueGroupName(map, featureSourceId.Name);
		}

		String unqualifiedClassName = null;
		// Map class names are unqualified while MapGuide class names are qualified
		// need an unqualified name
		// figure out which DLL is being used and do the appropriate thing
		// For every feature class, add a layer into Map
		foreach (string className in classNames)
		{
			if (utilities.AssemblyRootName == "MapGuideAPIRef")
			{
				unqualifiedClassName = className.Split(':')[1];
			}
			else
			{
				unqualifiedClassName = className;
			}
			MgResourceIdentifier layerResId = AddVectorLayerToMap(map, featureSourceId, schemaName, unqualifiedClassName, groupName);
			if (layerResId != null)
			{
				layerResIds.Add(layerResId.ToString());
			}
		}
	}
	return layerResIds;
}

private string PopulateUniqueGroupName(MgMapBase map, string defaultName)
{
	// Get all group names in the current map
	StringCollection names = new StringCollection();
	foreach (MgLayerGroup group in map.GetLayerGroups()) // TODO: Only a level of group
	{
		names.Add(group.Name);
	}

	// Create a unique name
	string name = defaultName;
	int index = 1;
	while (names.IndexOf(name) >= 0)
	{
		name = string.Format("{0} ({1})", defaultName, index.ToString());
		++index;
	}
	return name;
}

public MgResourceIdentifier AddVectorLayerToMap(MgMapBase map,
	MgResourceIdentifier featureSourceId, string schemaName,
	string className, string groupName)
{
	// Determine the layer name and layer definition name
	string fsName = featureSourceId.ToString();
	int pathSepPos = fsName.LastIndexOf('/');
	string path = fsName.Substring(0, pathSepPos + 1); // Get the prefix, ie. Library://{path}
	string layerName = PopulateUniqueLayerName(map, className);
	string layerDefName = path + layerName + LayerDefinitionExtention;
	MgResourceIdentifier layerId = new MgResourceIdentifier(layerDefName);

	// Create layer definition
	string defaultGeomName = GetDefaultGeometryPropertyName(featureSourceId, schemaName, className);
	int geomType = GetGeometricType(featureSourceId, schemaName, className);
	String layerDefXmlStr = CreateLayerDefinitionXml(layerDefName, featureSourceId,
		schemaName, className, defaultGeomName, geomType);

	// Add LayerDefinition into resource system of Map
	byte[] bytes = Utilities.StringToBytes(layerDefXmlStr);
	MgByteSource source = new MgByteSource(bytes, bytes.Length);
	resourceService.SetResource(layerId, source.GetReader(), null);

	// Add layer to Map
	MgLayerBase layer = new MgLayerBase(layerId, resourceService);
	layer.SetName(layerName);
	layer.Name = layerName;
	map.GetLayers().Add(layer);

	// Add layer into the specified group
	if (!string.IsNullOrEmpty(groupName))
	{
		// Create group if it doesn't exist
		MgLayerGroupCollection groups = map.GetLayerGroups();
		bool bGroupExist = false;
		MgLayerGroup group = null;
		for (int i = 0; i < groups.GetCount(); i++)
		{
			group = groups.GetItem(i);
			if (groupName == group.GetName())
			{
				bGroupExist = true;
				break;
			}
		}
		if (!bGroupExist)
		{
			group = new MgLayerGroup(groupName);
			map.GetLayerGroups().Add(group);
		}
		layer.SetGroup(group);
	}
	return layerId;
}

private string PopulateUniqueLayerName(MgMapBase map, string defaultName)
{
	// Get all layer names in the current map
	StringCollection names = new StringCollection();
	foreach (MgLayerBase layer in map.GetLayers())
	{
		names.Add(layer.Name);
	}

	// Create a unique name
	string name = defaultName;
	int index = 1;
	while (names.IndexOf(name) >= 0)
	{
		name = string.Format("{0} ({1})", defaultName, index.ToString());
		++index;
	}
	return name;
}

\endcode
\htmlinclude ExampleBottom.html
**/