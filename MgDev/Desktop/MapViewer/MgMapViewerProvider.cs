using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Collections.Specialized;
using System.Diagnostics;

#pragma warning disable 1591

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

        public void LoadMap(MgMapBase map)
        {
            if (map == null)
                return;

            DisposeExistingMap();
            _map = map;
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

        /// <summary>
        /// Gets the coordinate system for the map
        /// </summary>
        /// <returns></returns>
        public MgCoordinateSystem GetMapCoordinateSystem()
        {
            return CoordSysFactory.Create(_map.GetMapSRS());
        }

        private Dictionary<string, NameValueCollection> _propertyMappings = new Dictionary<string, NameValueCollection>();

        internal NameValueCollection GetPropertyMappings(MgLayerBase layer)
        {
            MgResourceIdentifier resId = layer.GetLayerDefinition();
            string resIdStr = resId.ToString();
            if (_propertyMappings.ContainsKey(resIdStr))
                return _propertyMappings[resIdStr];

            MgByteReader content = _resSvc.GetResourceContent(resId);
            XmlDocument doc = new XmlDocument();
            string xml = content.ToString();
            doc.LoadXml(xml);

            XmlNodeList propMaps = doc.GetElementsByTagName("PropertyMapping"); //NOXLATE
            if (propMaps.Count > 0)
            {
                NameValueCollection propertyMappings = new NameValueCollection();
                foreach (XmlNode pm in propMaps)
                {
                    propertyMappings[pm["Name"].InnerText] = pm["Value"].InnerText; //NOXLATE
                }
                _propertyMappings[resIdStr] = propertyMappings;
            }
            else
            {
                //NULL is a legit dictionary value
                _propertyMappings[resIdStr] = null;
            }
            return _propertyMappings[resIdStr];
        }

        protected abstract MgSpatialContextReader GetSpatialContexts(MgLayerBase layer, bool activeOnly);

        public abstract MgMapBase CreateMap(MgResourceIdentifier mapDefinitionId, string name);
        public abstract MgByteReader RenderMap(MgSelectionBase selection, string format);
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

        public abstract MgTransientMapState CreateTransientState(MgMapBase map);

        public MgTransientMapState CreateTransientState()
        {
            return CreateTransientState(_map);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="queryType"></param>
        /// <param name="layerNames"></param>
        /// <param name="filterGeom"></param>
        /// <param name="selectionVariant"></param>
        /// <param name="featureFilter"></param>
        /// <param name="maxFeatures"></param>
        /// <param name="layerAttributeFilter"></param>
        /// <returns></returns>
        public abstract MgQueryMapFeaturesResult QueryMapFeatures(MgQueryRequestType queryType, MgStringCollection layerNames, MgGeometry filterGeom, int selectionVariant, string featureFilter, int maxFeatures, int layerAttributeFilter);

        protected MgTooltipQueryResult MakeTooltip(string tooltip) { return new MgTooltipQueryResult(tooltip); }

        protected MgSelectionQueryResult MakeSelectionResult(MgSelectionBase sel) { return new MgSelectionQueryResult(sel); }
    }

    public abstract class MgQueryMapFeaturesResult
    {

    }

    public class MgTooltipQueryResult : MgQueryMapFeaturesResult
    {
        internal MgTooltipQueryResult(string tooltip) { this.Tooltip = tooltip; }

        public string Tooltip { get; private set; }
    }

    public class MgSelectionQueryResult : MgQueryMapFeaturesResult
    {
        internal MgSelectionQueryResult(MgSelectionBase newSelection) { this.Selection = newSelection; }

        public MgSelectionBase Selection { get; private set; }
    }

    public enum MgQueryRequestType
    {
        Tooltip,
        Selection
    }
}
