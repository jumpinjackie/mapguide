using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using System.Xml;
using System.IO;
using OSGeo.MapGuide;
using OSGeo.MapGuide.Viewer.AppLayoutEngine;

namespace SampleExtension
{
    public partial class ModifyingMapsAndLayers : MgControlView
    {
        private IMapViewer _viewer;

        public ModifyingMapsAndLayers(IMapViewer viewer)
        {
            InitializeComponent();
            _viewer = viewer;
            this.Title = "Modifying Maps and Layers";
        }

        private void btnShowBuildings_Click(object sender, EventArgs e)
        {
            MgMapViewerProvider provider = _viewer.GetProvider();
            MgResourceService resourceService = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);

            string layerName = "RecentlyBuilt";
            string legendLabel = "Built after 1980";
            string groupName = "Analysis";

            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            MgLayerGroupCollection groups = map.GetLayerGroups();

            if (layers.IndexOf(layerName) >= 0)
            {
                MessageBox.Show("Layer (" + layerName + ") already created");
                return;
            }

            //NOTE: mg-desktop has no formal concept of a session repository, but we still
            //support session-based resources as a form of temporary resource (for the duration
            //of the application). Session ids in mg-desktop can be any arbitrary string
            string sessionId = Guid.NewGuid().ToString();

            XmlDocument domDocument = new XmlDocument();
            domDocument.LoadXml(Layers.RecentlyBuilt);

            // Get a list of all the <AreaRule><Filter> elements in
            // the XML.
            XmlNodeList nodes = domDocument.SelectNodes("//AreaRule/Filter");
            // Find the correct node and change it
            foreach (XmlNode node in nodes)
            {
                if (node.InnerText == "YRBUILT > 1950")
                {
                    node.InnerText = "YRBUILT > 1980";
                }
            }

            // Get a list of all the <LegendLabel> elements in the
            // XML.
            nodes = domDocument.SelectNodes("//LegendLabel");
            // Find the correct node and change it
            foreach (XmlNode node in nodes)
            {
                if (node.InnerText == "Built after 1950")
                {
                    node.InnerText = "Built after 1980";
                }
            }

            //Add the layer to the map
            // TODO: Should probably validate this XML content
            using (MemoryStream ms = new MemoryStream())
            {
                domDocument.Save(ms);
                ms.Position = 0L;
                //Note we do this to ensure our XML content is free of any BOM characters
                byte[] layerDefinition = ms.ToArray();
                Encoding utf8 = Encoding.UTF8;
                String layerDefStr = new String(utf8.GetChars(layerDefinition));
                layerDefinition = new byte[layerDefStr.Length - 1];
                int byteCount = utf8.GetBytes(layerDefStr, 1, layerDefStr.Length - 1, layerDefinition, 0);
                // Save the new layer definition to the session repository  
                MgByteSource byteSource = new MgByteSource(layerDefinition, layerDefinition.Length);
                MgResourceIdentifier resourceID = new MgResourceIdentifier("Session:" + sessionId + "//" + layerName + ".LayerDefinition");
                resourceService.SetResource(resourceID, byteSource.GetReader(), null);

                //Insert this layer to our map
                MgdLayer newLayer = new MgdLayer(resourceID, resourceService);
                newLayer.Name = layerName;
                newLayer.LegendLabel = legendLabel;
                newLayer.DisplayInLegend = true;
                newLayer.SetVisible(true);
                layers.Insert(0, newLayer);

                //Create analysis group if not already created
                if (groups.IndexOf(groupName) < 0)
                {
                    MgLayerGroup group = new MgLayerGroup(groupName);
                    group.LegendLabel = groupName;
                    group.DisplayInLegend = true;
                    groups.Add(group);
                    newLayer.Group = group;
                }

                //Turn off square footage if it exists
                if (layers.IndexOf("SquareFootage") >= 0)
                {
                    MgLayerBase layer = layers.GetItem("SquareFootage");
                    layer.SetVisible(false);
                }

                MessageBox.Show("Layer (" + layerName + ") created");
                _viewer.RefreshMap();
                IMapLegend legend = Shell.Instance.Legend;
                if (legend != null)
                    legend.RefreshLegend();
            }
        }

        private void btnCreateSquareFootage_Click(object sender, EventArgs e)
        {
            string layerName = "SquareFootage";
            string legendLabel = "Square Footage";
            string groupName = "Analysis";

            MgMapViewerProvider provider = _viewer.GetProvider();
            MgResourceService resourceService = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);

            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            MgLayerGroupCollection groups = map.GetLayerGroups();

            //NOTE: mg-desktop has no formal concept of a session repository, but we still
            //support session-based resources as a form of temporary resource (for the duration
            //of the application). Session ids in mg-desktop can be any arbitrary string
            string sessionId = Guid.NewGuid().ToString();

            if (layers.IndexOf(layerName) >= 0)
            {
                MessageBox.Show("Layer (" + layerName + ") already created");
                return;
            }

            //---------------------------------------------------//
            // Create a new layer
            LayerDefinitionFactory factory = new LayerDefinitionFactory();

            /// Create three area rules for three different
            // scale ranges.
            String areaRule1 = factory.CreateAreaRule("1 to 800",
              "SQFT &gt;= 1 AND SQFT &lt; 800", "FFFFFF00");
            String areaRule2 = factory.CreateAreaRule("800 to 1600",
              "SQFT &gt;= 800 AND SQFT &lt; 1600", "FFFFBF20");
            String areaRule3 = factory.CreateAreaRule("1600 to 2400",
              "SQFT &gt;= 1600 AND SQFT &lt; 2400", "FFFF8040");

            // Create an area type style.
            String areaTypeStyle = factory.CreateAreaTypeStyle(areaRule1 + areaRule2 + areaRule3);

            // Create a scale range.
            String minScale = "0";
            String maxScale = "10000";
            String areaScaleRange = factory.CreateScaleRange(minScale, maxScale, areaTypeStyle);

            // Create the layer definiton.
            String featureClass = "Library://Samples/Sheboygan/Data/Parcels.FeatureSource";
            String featureName = "SHP_Schema:Parcels";
            String geometry = "SHPGEOM";
            String layerDefinition = factory.CreateLayerDefinition(featureClass, featureName, geometry, areaScaleRange);

            XmlDocument domDocument = new XmlDocument();
            domDocument.LoadXml(layerDefinition);

            //Add the layer to the map
            // TODO: Should probably validate this XML content
            using (MemoryStream ms = new MemoryStream())
            {
                domDocument.Save(ms);
                ms.Position = 0L;
                //Note we do this to ensure our XML content is free of any BOM characters
                byte[] ldfBytes = ms.ToArray();
                Encoding utf8 = Encoding.UTF8;
                String layerDefStr = new String(utf8.GetChars(ldfBytes));
                ldfBytes = new byte[layerDefStr.Length - 1];
                int byteCount = utf8.GetBytes(layerDefStr, 1, layerDefStr.Length - 1, ldfBytes, 0);
                // Save the new layer definition to the session repository  
                MgByteSource byteSource = new MgByteSource(ldfBytes, ldfBytes.Length);
                MgResourceIdentifier resourceID = new MgResourceIdentifier("Session:" + sessionId + "//" + layerName + ".LayerDefinition");
                resourceService.SetResource(resourceID, byteSource.GetReader(), null);

                //Insert this layer to our map
                MgdLayer newLayer = new MgdLayer(resourceID, resourceService);
                newLayer.Name = layerName;
                newLayer.LegendLabel = legendLabel;
                newLayer.DisplayInLegend = true;
                newLayer.SetVisible(true);
                layers.Insert(0, newLayer);

                //Create analysis group if not already created
                if (groups.IndexOf(groupName) < 0)
                {
                    MgLayerGroup group = new MgLayerGroup(groupName);
                    group.LegendLabel = groupName;
                    group.DisplayInLegend = true;
                    groups.Add(group);
                    newLayer.Group = group;
                }

                //Turn off recently built if it exists
                if (layers.IndexOf("RecentlyBuilt") >= 0)
                {
                    MgLayerBase layer = layers.GetItem("RecentlyBuilt");
                    layer.SetVisible(false);
                }

                MessageBox.Show("Layer (" + layerName + ") created");
                _viewer.RefreshMap();
                IMapLegend legend = Shell.Instance.Legend;
                if (legend != null)
                    legend.RefreshLegend();
            }
        }

        private void btnCreateHydroLine_Click(object sender, EventArgs e)
        {
            string layerName = "Hydro";
            string legendLabel = "Hydro";
            string groupName = "Analysis";

            MgMapViewerProvider provider = _viewer.GetProvider();
            MgResourceService resourceService = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);

            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            MgLayerGroupCollection groups = map.GetLayerGroups();

            //NOTE: mg-desktop has no formal concept of a session repository, but we still
            //support session-based resources as a form of temporary resource (for the duration
            //of the application). Session ids in mg-desktop can be any arbitrary string
            string sessionId = Guid.NewGuid().ToString();

            if (layers.IndexOf(layerName) >= 0)
            {
                MessageBox.Show("Layer (" + layerName + ") already created");
                return;
            }

            //---------------------------------------------------//
            // Create a new layer

            LayerDefinitionFactory factory = new LayerDefinitionFactory();

            // Create a line rule.
            String ruleLegendLabel = "";
            String filter = "";
            String color = "FF0000FF";
            String lineRule = factory.CreateLineRule(ruleLegendLabel, filter, color);

            // Create a line type style.
            String lineTypeStyle = factory.CreateLineTypeStyle(lineRule);

            // Create a scale range.
            String minScale = "0";
            String maxScale = "1000000000000";
            String lineScaleRange = factory.CreateScaleRange(minScale, maxScale, lineTypeStyle);

            // Create the layer definiton.
            String featureClass = "Library://Samples/Sheboygan/Data/HydrographicLines.FeatureSource";
            String featureName = "SHP_Schema:HydrographicLines";
            String geometry = "SHPGEOM";
            String layerDefinition = factory.CreateLayerDefinition(featureClass, featureName, geometry, lineScaleRange);

            //---------------------------------------------------//
            // ...

            XmlDocument domDocument = new XmlDocument();
            domDocument.LoadXml(layerDefinition);

            //Add the layer to the map
            // TODO: Should probably validate this XML content
            using (MemoryStream ms = new MemoryStream())
            {
                domDocument.Save(ms);
                ms.Position = 0L;
                //Note we do this to ensure our XML content is free of any BOM characters
                byte[] ldfBytes = ms.ToArray();
                Encoding utf8 = Encoding.UTF8;
                String layerDefStr = new String(utf8.GetChars(ldfBytes));
                ldfBytes = new byte[layerDefStr.Length - 1];
                int byteCount = utf8.GetBytes(layerDefStr, 1, layerDefStr.Length - 1, ldfBytes, 0);
                // Save the new layer definition to the session repository  
                MgByteSource byteSource = new MgByteSource(ldfBytes, ldfBytes.Length);
                MgResourceIdentifier resourceID = new MgResourceIdentifier("Session:" + sessionId + "//" + layerName + ".LayerDefinition");
                resourceService.SetResource(resourceID, byteSource.GetReader(), null);

                //Insert this layer to our map
                MgdLayer newLayer = new MgdLayer(resourceID, resourceService);
                newLayer.Name = layerName;
                newLayer.LegendLabel = legendLabel;
                newLayer.DisplayInLegend = true;
                newLayer.SetVisible(true);
                layers.Insert(0, newLayer);

                //Create analysis group if not already created
                if (groups.IndexOf(groupName) < 0)
                {
                    MgLayerGroup group = new MgLayerGroup(groupName);
                    group.LegendLabel = groupName;
                    group.DisplayInLegend = true;
                    groups.Add(group);
                    newLayer.Group = group;
                }

                MessageBox.Show("Layer (" + layerName + ") created");
                _viewer.RefreshMap();
                IMapLegend legend = Shell.Instance.Legend;
                if (legend != null)
                    legend.RefreshLegend();
            }
        }

        private void btnCreatePointsOfInterest_Click(object sender, EventArgs e)
        {
            string layerName = "Points";
            string legendLabel = "Points of Interest";
            string groupName = "Analysis";

            MgMapViewerProvider provider = _viewer.GetProvider();
            MgResourceService resourceService = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
            //We use MgdFeatureService instead of MgFeatureService as this sample demonstrates APIs unique to mg-desktop
            MgdFeatureService featureService = (MgdFeatureService)provider.CreateService(MgServiceType.FeatureService);

            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            MgLayerGroupCollection groups = map.GetLayerGroups();

            //NOTE: mg-desktop has no formal concept of a session repository, but we still
            //support session-based resources as a form of temporary resource (for the duration
            //of the application). Session ids in mg-desktop can be any arbitrary string
            string sessionId = Guid.NewGuid().ToString();

            if (layers.IndexOf(layerName) >= 0)
            {
                MessageBox.Show("Layer (" + layerName + ") already created");
                return;
            }

            //---------------------------------------------------//
            // Create a feature source with point data.
            // (The Sheboygan sample data does not contain such data,
            // so we"ll create it.)

            // Create a feature class definition for the new feature source
            MgClassDefinition classDefinition = new MgClassDefinition();
            classDefinition.SetName("Points");
            classDefinition.SetDescription("Feature class with point data.");
            classDefinition.SetDefaultGeometryPropertyName("GEOM");

            MgPropertyDefinitionCollection idProps = classDefinition.GetIdentityProperties();
            MgPropertyDefinitionCollection clsProps = classDefinition.GetProperties();

            // Create an identify property
            MgDataPropertyDefinition identityProperty = new MgDataPropertyDefinition("KEY");
            identityProperty.SetDataType(MgPropertyType.Int32);
            identityProperty.SetAutoGeneration(true);
            identityProperty.SetReadOnly(true);
            // Add the identity property to the class definition
            clsProps.Add(identityProperty);
            idProps.Add(identityProperty);

            // Create a name property
            MgDataPropertyDefinition nameProperty = new MgDataPropertyDefinition("NAME");
            nameProperty.SetDataType(MgPropertyType.String);
            // Add the name property to the class definition
            clsProps.Add(nameProperty);

            // Create a geometry property
            MgGeometricPropertyDefinition geometryProperty = new MgGeometricPropertyDefinition("GEOM");
            geometryProperty.SetGeometryTypes(MgFeatureGeometricType.Point);
            // Add the geometry property to the class definition
            clsProps.Add(geometryProperty);

            // Create a feature schema
            MgFeatureSchema featureSchema = new MgFeatureSchema("PointSchema", "Point schema");
            MgClassDefinitionCollection classes = featureSchema.GetClasses();
            // Add the feature schema to the class definition
            classes.Add(classDefinition);

            // Create the feature source
            String featureSourceName = "Library://Samples/DevGuide/Data/points.FeatureSource";
            MgResourceIdentifier resourceIdentifier = new MgResourceIdentifier(featureSourceName);
            //wkt = "LOCALCS[\"*XY-MT*\",LOCAL_DATUM[\"*X-Y*\",10000],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
            String wkt = "GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.000,298.25722293]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.01745329251994]]";
            MgCreateSdfParams sdfParams = new MgCreateSdfParams("LatLong", wkt, featureSchema);
            featureService.CreateFeatureSource(resourceIdentifier, sdfParams);

            // We need to add some data to the sdf before using it.  The spatial context
            // reader must have an extent.
            MgBatchPropertyCollection batchPropertyCollection = new MgBatchPropertyCollection();
            MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
            MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
            MgGeometryFactory geometryFactory = new MgGeometryFactory();

            // Make four points
            batchPropertyCollection.Add(MakePoint("Point A", -87.727, 43.748, wktReaderWriter, agfReaderWriter));

            batchPropertyCollection.Add(MakePoint("Point B", -87.728, 43.730, wktReaderWriter, agfReaderWriter));

            batchPropertyCollection.Add(MakePoint("Point C", -87.726, 43.750, wktReaderWriter, agfReaderWriter));

            batchPropertyCollection.Add(MakePoint("Point D", -87.728, 43.750, wktReaderWriter, agfReaderWriter));

            // Old way commented out
            
            /*
            // Add the batch property collection to the feature source
            MgInsertFeatures cmd = new MgInsertFeatures("Points", batchPropertyCollection);
            MgFeatureCommandCollection featureCommandCollection = new MgFeatureCommandCollection();
            featureCommandCollection.Add(cmd);

            // Execute the "add" commands
            featureService.UpdateFeatures(resourceIdentifier, featureCommandCollection, false);
            */

            // Here's the mg-desktop way
            MgPropertyCollection insertResult = featureService.InsertFeatures(resourceIdentifier, "Points", batchPropertyCollection);
            for (int i = 0; i < insertResult.GetCount(); i++)
            {
                MgFeatureProperty fp = insertResult.GetItem(i) as MgFeatureProperty;
                if (fp != null)
                {
                    MgFeatureReader fr = fp.GetValue();
                    fr.Close();
                }
            }

            // ...
            //---------------------------------------------------//
            // Create a new layer

            LayerDefinitionFactory factory = new LayerDefinitionFactory();

            // Create a mark symbol
            String resourceId = "Library://Samples/Sheboygan/Symbols/BasicSymbols.SymbolLibrary";
            String symbolName = "PushPin";
            String width = "24";  // unit = points
            String height = "24"; // unit = points
            String color = "FFFF0000";
            String markSymbol = factory.CreateMarkSymbol(resourceId, symbolName, width, height, color);

            // Create a text symbol
            String text = "ID";
            String fontHeight = "12";
            String foregroundColor = "FF000000";
            String textSymbol = factory.CreateTextSymbol(text, fontHeight, foregroundColor);

            // Create a point rule.
            String ruleLegendLabel = "trees";
            String filter = "";
            String pointRule = factory.CreatePointRule(ruleLegendLabel, filter, textSymbol, markSymbol);

            // Create a point type style.
            String pointTypeStyle = factory.CreatePointTypeStyle(pointRule);

            // Create a scale range.
            String minScale = "0";
            String maxScale = "1000000000000";
            String pointScaleRange = factory.CreateScaleRange(minScale, maxScale, pointTypeStyle);

            // Create the layer definiton.
            String featureName = "PointSchema:Points";
            String geometry = "GEOM";
            String layerDefinition = factory.CreateLayerDefinition(featureSourceName, featureName, geometry, pointScaleRange);
            //---------------------------------------------------//
            // ...

            XmlDocument domDocument = new XmlDocument();
            domDocument.LoadXml(layerDefinition);

            //Add the layer to the map
            // TODO: Should probably validate this XML content
            using (MemoryStream ms = new MemoryStream())
            {
                domDocument.Save(ms);
                ms.Position = 0L;
                //Note we do this to ensure our XML content is free of any BOM characters
                byte[] ldfBytes = ms.ToArray();
                Encoding utf8 = Encoding.UTF8;
                String layerDefStr = new String(utf8.GetChars(ldfBytes));
                ldfBytes = new byte[layerDefStr.Length - 1];
                int byteCount = utf8.GetBytes(layerDefStr, 1, layerDefStr.Length - 1, ldfBytes, 0);
                // Save the new layer definition to the session repository  
                MgByteSource byteSource = new MgByteSource(ldfBytes, ldfBytes.Length);
                MgResourceIdentifier resourceID = new MgResourceIdentifier("Session:" + sessionId + "//" + layerName + ".LayerDefinition");
                resourceService.SetResource(resourceID, byteSource.GetReader(), null);

                //Insert this layer to our map
                MgdLayer newLayer = new MgdLayer(resourceID, resourceService);
                newLayer.Name = layerName;
                newLayer.LegendLabel = legendLabel;
                newLayer.DisplayInLegend = true;
                newLayer.SetVisible(true);
                layers.Insert(0, newLayer);

                //Create analysis group if not already created
                if (groups.IndexOf(groupName) < 0)
                {
                    MgLayerGroup group = new MgLayerGroup(groupName);
                    group.LegendLabel = groupName;
                    group.DisplayInLegend = true;
                    groups.Add(group);
                    newLayer.Group = group;
                }

                MessageBox.Show("Layer (" + layerName + ") created");
                _viewer.RefreshMap();
                IMapLegend legend = Shell.Instance.Legend;
                if (legend != null)
                    legend.RefreshLegend();
            }
        }

        MgPropertyCollection MakePoint(String name, double x, double y, MgWktReaderWriter wktReaderWriter, MgAgfReaderWriter agfReaderWriter)
        {
            MgPropertyCollection propertyCollection = new MgPropertyCollection();
            MgStringProperty nameProperty = new MgStringProperty("NAME", name);
            propertyCollection.Add(nameProperty);

            MgGeometry geometry = wktReaderWriter.Read("POINT XY (" + x + " " + y + ")");
            MgByteReader geometryByteReader = agfReaderWriter.Write(geometry);
            MgGeometryProperty geometryProperty = new MgGeometryProperty("GEOM", geometryByteReader);
            propertyCollection.Add(geometryProperty);

            return propertyCollection;
        }
    }
}
