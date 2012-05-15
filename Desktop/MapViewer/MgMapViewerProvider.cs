using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Collections.Specialized;
using System.Diagnostics;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// Viewer rendering options
    /// </summary>
    public class MgViewerRenderingOptions
    {
        public MgViewerRenderingOptions(string format, int behavior, MgColor color)
        {
            this.Format = format;
            this.Behavior = behavior;
            this.Color = color;
        }

        /// <summary>
        /// Gets the format.
        /// </summary>
        public string Format { get; private set; }

        /// <summary>
        /// Gets the behavior.
        /// </summary>
        public int Behavior { get; private set; }

        /// <summary>
        /// Gets the color.
        /// </summary>
        public MgColor Color { get; private set; }
    }

    /// <summary>
    /// A map viewer provider that handles the platform-specific implementation of the runtime map
    /// </summary>
    public abstract class MgMapViewerProvider
    {
        /// <summary>
        /// The runtime map
        /// </summary>
        protected MgMapBase _map;

        /// <summary>
        /// The resource service
        /// </summary>
        protected MgResourceService _resSvc;

        protected MgMapViewerProvider(MgMapBase map)
        {
            SubInit();
            LoadMap(map);
        }

        protected virtual void SubInit() { }

        public MgMapBase GetMap() { return _map; }

        protected virtual void DisposeExistingMap()
        {
            if (_map != null)
            {
                _map.Dispose();
                _map = null;
            }
        }

        internal void LoadMap(MgMapBase map)
        {
            if (map == null)
                return;

            DisposeExistingMap();
            _map = map;
            RebuildLayerInfoCache();
            CacheGeometryProperties(_map.GetLayers());
            OnNewMapLoaded(map);
            var h = this.MapLoaded;
            if (h != null)
                h(this, EventArgs.Empty);
        }

        protected virtual void OnNewMapLoaded(MgMapBase map) { }

        internal event EventHandler MapLoaded;

        /// <summary>
        /// The coordinate system factory
        /// </summary>
        protected MgCoordinateSystemFactory _csFact;

        /// <summary>
        /// Gets the coord sys factory.
        /// </summary>
        protected MgCoordinateSystemFactory CoordSysFactory
        {
            get
            {
                if (null == _csFact)
                    _csFact = new MgCoordinateSystemFactory();
                return _csFact;
            }
        }

        public MgCoordinateSystem GetMapCoordinateSystem()
        {
            return CoordSysFactory.Create(_map.GetMapSRS());
        }

        private Dictionary<string, string> _layerGeomProps = new Dictionary<string, string>();

        public void CacheGeometryProperties(MgLayerCollection layers)
        {
            //Cache geometry properties
            for (int i = 0; i < layers.GetCount(); i++)
            {
                var layer = layers.GetItem(i);
                //if (!layer.Selectable || !layer.IsVisible())
                //    continue;

                var objId = layer.GetObjectId();
                if (_layerGeomProps.ContainsKey(objId))
                    continue;

                string fsId = layer.FeatureSourceId;
                if (fsId.EndsWith(MgResourceType.DrawingSource))
                    continue;

                CheckAndCacheGeometryProperty(layer);
            }
        }

        public void CheckAndCacheGeometryProperty(MgLayerBase layer)
        {
            var objId = layer.GetObjectId();
            if (!_layerGeomProps.ContainsKey(objId))
            {
                try
                {
                    var cls = layer.GetClassDefinition();
                    var geomName = cls.DefaultGeometryPropertyName;
                    if (!string.IsNullOrEmpty(geomName))
                    {
                        _layerGeomProps[objId] = geomName;
                    }
                }
                catch (MgException ex)
                {
                    Trace.TraceWarning("Failed to get geometry property for layer: " + layer.Name + Environment.NewLine + ex.ToString());
                    ex.Dispose();
                }
            }
        }

        private Dictionary<string, MgCoordinateSystemTransform> _mapToLayerTransforms = new Dictionary<string, MgCoordinateSystemTransform>();
        private Dictionary<string, NameValueCollection> _propertyMappings = new Dictionary<string, NameValueCollection>();

        internal Dictionary<string, NameValueCollection> AllPropertyMappings { get { return _propertyMappings; } }

        private Dictionary<string, XmlDocument> _cachedLayerDefinitions = new Dictionary<string, XmlDocument>();
        private Dictionary<string, string> _tooltipExpressions = new Dictionary<string, string>();

        protected void RebuildLayerInfoCache()
        {
            _cachedLayerDefinitions.Clear();
            _tooltipExpressions.Clear();
            _propertyMappings.Clear();

            foreach (var trans in _mapToLayerTransforms.Values)
            {
                if (trans != null)
                    trans.Dispose();
            }
            _mapToLayerTransforms.Clear();

            if (_resSvc == null)
                _resSvc = (MgResourceService)CreateService(MgServiceType.ResourceService);

            //Pre-cache layer definitions and tooltip properties
            var layers = _map.GetLayers();
            MgStringCollection resIds = new MgStringCollection();
            for (int i = 0; i < layers.GetCount(); i++)
            {
                var layer = layers.GetItem(i);
                var ldf = layer.GetLayerDefinition();
                resIds.Add(ldf.ToString());

                //Make sure geometry property checks out
                var clsDef = layer.GetClassDefinition();
                var geomName = clsDef.DefaultGeometryPropertyName;
                var props = clsDef.GetProperties();
                if (props.IndexOf(geomName) < 0)
                    continue;

                var geomProp = props.GetItem(geomName) as MgGeometricPropertyDefinition;
                if (geomProp == null)
                    continue;

                var trans = GetMapToLayerTransform(layer, geomProp);
            }
            MgStringCollection contents = _resSvc.GetResourceContents(resIds, null);
            for (int i = 0; i < contents.GetCount(); i++)
            {
                XmlDocument doc = new XmlDocument();
                doc.LoadXml(contents.GetItem(i));

                _cachedLayerDefinitions[resIds.GetItem(i)] = doc;
                XmlNodeList nodes = doc.GetElementsByTagName("ToolTip");
                if (nodes.Count == 1)
                    _tooltipExpressions[resIds.GetItem(i)] = nodes[0].InnerText;

                XmlNodeList propMaps = doc.GetElementsByTagName("PropertyMapping");
                if (propMaps.Count > 0)
                {
                    NameValueCollection propertyMappings = new NameValueCollection();
                    foreach (XmlNode pm in propMaps)
                    {
                        propertyMappings[pm["Name"].InnerText] = pm["Value"].InnerText;
                    }
                    _propertyMappings[resIds.GetItem(i)] = propertyMappings;
                }
            }
        }

        internal MgCoordinateSystemTransform GetMapToLayerTransform(MgLayerBase layer, MgGeometricPropertyDefinition geomProp)
        {
            string objId = layer.GetObjectId();
            string mapCsWkt = _map.GetMapSRS();
            bool bChecked = false;
            MgCoordinateSystemTransform trans = this.GetLayerTransform(objId, out bChecked);
            if (trans == null && !bChecked)
            {
                MgSpatialContextReader scReader = GetSpatialContexts(layer, false);
                try
                {
                    while (scReader.ReadNext())
                    {
                        if (scReader.GetName() == geomProp.SpatialContextAssociation)
                        {
                            //Only need to set up transform if layer and map wkts do not match
                            if (!string.IsNullOrEmpty(mapCsWkt) && !string.IsNullOrEmpty(scReader.GetCoordinateSystemWkt()))
                            {
                                if (mapCsWkt != scReader.GetCoordinateSystemWkt())
                                {
                                    var csFact = this.CoordSysFactory;
                                    var layerCs = csFact.Create(scReader.GetCoordinateSystemWkt());
                                    trans = csFact.GetTransform(this.GetMapCoordinateSystem(), layerCs);
                                    this.CacheLayerTransform(objId, trans);
                                    break;
                                }
                            }
                        }
                    }
                }
                finally
                {
                    scReader.Close();
                }
                if (trans == null)
                    this.CacheLayerTransform(objId, null);
            }
            return trans;
        }

        protected abstract MgSpatialContextReader GetSpatialContexts(MgLayerBase layer, bool activeOnly);

        public abstract MgMapBase CreateMap(MgResourceIdentifier mapDefinitionId, string name);
        public abstract MgByteReader RenderDynamicOverlay(MgSelectionBase selection, MgViewerRenderingOptions args);
        public abstract void SetDisplaySize(int width, int height);
        public abstract MgSelectionBase CreateSelectionForMap();
        public abstract double GetMetersPerUnit();
        public abstract void SetViewCenterXY(double x, double y);

        public abstract void ClearSelection(MgSelectionBase sel);
        public abstract void SetViewScale(double scale);
        public abstract void MakeGroupNormal(MgLayerGroup group);

        public abstract MgLayerBase CreateLayer(MgResourceIdentifier layerDefId);
        public abstract MgService CreateService(int serviceType);
        public abstract MgByteReader GenerateLegendImage(MgResourceIdentifier layerDefinition, double viewScale, int width, int height, string format, int geomType, int themeCategory);

        public abstract void SetGroupExpandInLegend(MgLayerGroup group, bool bExpanded);
        public abstract void SetLayerExpandInLegend(MgLayerBase layer, bool bExpanded);

        public abstract bool LayerHasTooltips(MgLayerBase layer);

        public abstract bool IsLayerPotentiallyVisibleAtScale(MgLayerBase layer, bool bConsiderParentGroupVisibility);

        public string GetGeometryProperty(string objId)
        {
            return _layerGeomProps[objId];
        }

        public string GetTooltipExpression(MgResourceIdentifier ldfId)
        {
            return _tooltipExpressions[ldfId.ToString()];
        }

        public bool HasTooltips(MgResourceIdentifier ldfId)
        {
            return _tooltipExpressions.ContainsKey(ldfId.ToString());
        }

        internal MgCoordinateSystemTransform GetLayerTransform(string objId, out bool bAlreadyChecked)
        {
            bAlreadyChecked = false;
            if (_mapToLayerTransforms.ContainsKey(objId))
            {
                bAlreadyChecked = true;
                return _mapToLayerTransforms[objId];
            }
            return null;
        }

        internal void CacheLayerTransform(string objId, MgCoordinateSystemTransform trans)
        {
            _mapToLayerTransforms[objId] = trans;
        }
    }
}
