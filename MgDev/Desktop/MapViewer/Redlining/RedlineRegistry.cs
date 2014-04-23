using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer.Redlining
{
    internal class RedlineRegistry
    {
        private MgResourceIdentifier _registryFsId;
        private MgFeatureService _featSvc;
        private MgResourceService _resSvc;
        private string _sessionID;

        public RedlineRegistry(string sessionID, MgResourceService resSvc, MgFeatureService featSvc)
        {
            _sessionID = sessionID;
            _resSvc = resSvc;
            _featSvc = featSvc;
        }

        private void Ensure()
        {
            if (_registryFsId == null)
            {
                _registryFsId = new MgResourceIdentifier("Session:" + _sessionID + "//MarkupRegistry.FeatureSource");

                if (!_resSvc.ResourceExists(_registryFsId))
                {
                    //Markup Registry Feature Source Schema
                    //
                    //Default
                    //  MarkupRegistry
                    //    ResourceId (String, Identity, Not Null)
                    //    LayerDefintion (String, Not Null)
                    //    Name (String, Not Null)
                    //    FdoProvider (String, Not Null)
                    //    GeometryTypes (Int, Not Null)
                    MgFeatureSchema schema = new MgFeatureSchema("Default", "");
                    MgClassDefinition cls = new MgClassDefinition();
                    cls.SetName("MarkupRegistry");

                    MgDataPropertyDefinition regId = new MgDataPropertyDefinition("ResourceId");
                    regId.SetDataType(MgPropertyType.String);
                    regId.SetLength(1024);
                    regId.SetNullable(false);

                    MgDataPropertyDefinition layerDefId = new MgDataPropertyDefinition("LayerDefinition");
                    layerDefId.SetDataType(MgPropertyType.String);
                    layerDefId.SetLength(1024);
                    layerDefId.SetNullable(false);

                    MgDataPropertyDefinition name = new MgDataPropertyDefinition("Name");
                    name.SetDataType(MgPropertyType.String);
                    name.SetLength(512);
                    name.SetNullable(false);

                    MgDataPropertyDefinition provider = new MgDataPropertyDefinition("FdoProvider");
                    provider.SetDataType(MgPropertyType.String);
                    provider.SetLength(512);
                    provider.SetNullable(false);

                    MgDataPropertyDefinition geomTypes = new MgDataPropertyDefinition("GeometryTypes");
                    geomTypes.SetDataType(MgPropertyType.Int32);
                    geomTypes.SetNullable(false);

                    MgDataPropertyDefinition styleType = new MgDataPropertyDefinition("StyleType");
                    styleType.SetDataType(MgPropertyType.Int32);
                    styleType.SetNullable(false);

                    MgPropertyDefinitionCollection dataProps = cls.GetProperties();
                    dataProps.Add(regId);
                    dataProps.Add(layerDefId);
                    dataProps.Add(name);
                    dataProps.Add(provider);
                    dataProps.Add(geomTypes);
                    dataProps.Add(styleType);

                    MgPropertyDefinitionCollection idProps = cls.GetIdentityProperties();
                    idProps.Add(regId);

                    MgClassDefinitionCollection classes = schema.GetClasses();
                    classes.Add(cls);

                    //No coordinate system required here because we're not storing spatial data
                    MgFileFeatureSourceParams createSdf = new MgFileFeatureSourceParams("OSGeo.SDF", "Default", "", schema);
                    _featSvc.CreateFeatureSource(_registryFsId, createSdf);
                }
            }
        }

        private MgResourceIdentifier GetRegistryFeatureSource()
        {
            Ensure();
            return _registryFsId;
        }

        internal void AddRedlineLayer(RedlineLayer layer, string fdoProvider)
        {
            var fsId = GetRegistryFeatureSource();
            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            MgPropertyCollection insertVals = new MgPropertyCollection();

            MgStringProperty regId = new MgStringProperty("ResourceId", layer.FeatureSource);
            MgStringProperty ldfId = new MgStringProperty("LayerDefinition", layer.LayerDefinition);
            MgStringProperty name = new MgStringProperty("Name", layer.Name);
            MgStringProperty provider = new MgStringProperty("FdoProvider", fdoProvider);
            MgInt32Property geomTypes = new MgInt32Property("GeometryTypes", layer.GeometryTypes);
            MgInt32Property styleType = new MgInt32Property("StyleType", (int)layer.StyleType);

            insertVals.Add(regId);
            insertVals.Add(ldfId);
            insertVals.Add(name);
            insertVals.Add(provider);
            insertVals.Add(geomTypes);
            insertVals.Add(styleType);

            MgInsertFeatures insert = new MgInsertFeatures("Default:MarkupRegistry", insertVals);
            commands.Add(insert);

            MgPropertyCollection result = _featSvc.UpdateFeatures(fsId, commands, false);
            MgFeatureProperty insertResult = result.GetItem(0) as MgFeatureProperty;
            if (insertResult != null)
            {
                MgFeatureReader fr = insertResult.GetValue();
                fr.Close();
            }
        }

        internal RedlineLayer[] GetAvailableLayers(MgMapBase map)
        {
            var fsId = GetRegistryFeatureSource();
            MgLayerCollection mapLayers = map.GetLayers();
            List<RedlineLayer> layers = new List<RedlineLayer>();
            MgFeatureReader reader = null;
            try
            {
                reader = _featSvc.SelectFeatures(fsId, "Default:MarkupRegistry", null);
                while (reader.ReadNext())
                {
                    layers.Add(new RedlineLayer(reader.GetString("Name"),
                                                reader.GetString("ResourceId"),
                                                reader.GetString("LayerDefinition"),
                                                reader.GetInt32("GeometryTypes"),
                                                (RedlineStylizationType)reader.GetInt32("StyleType")));
                }
            }
            finally
            {
                if (reader != null)
                    reader.Close();
            }
            return layers.ToArray();
        }

        internal void DeleteLayer(RedlineLayer layer)
        {
            var regId = GetRegistryFeatureSource();
            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            MgDeleteFeatures delete = new MgDeleteFeatures("Default:MarkupRegistry", "ResourceId = '" + layer.FeatureSource + "'");
            commands.Add(delete);

            MgPropertyCollection results = _featSvc.UpdateFeatures(regId, commands, false);
            MgInt32Property deleteResult = results.GetItem(0) as MgInt32Property;
            if (deleteResult != null && deleteResult.GetValue() > 0)
            {
                var ldfId = new MgResourceIdentifier(layer.LayerDefinition);
                var fsId = new MgResourceIdentifier(layer.FeatureSource);

                if (_resSvc.ResourceExists(ldfId))
                    _resSvc.DeleteResource(ldfId);

                if (_resSvc.ResourceExists(fsId))
                    _resSvc.DeleteResource(fsId);
            }
        }
    }
}
