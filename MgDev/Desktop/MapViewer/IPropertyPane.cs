using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using OSGeo.MapGuide;
using System.Collections.Specialized;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A component that displays properties of selected features
    /// </summary>
    public interface IPropertyPane
    {
        /// <summary>
        /// Initializes the property pane from the given selection set
        /// </summary>
        /// <param name="set"></param>
        void Init(MgSelectionSet set);

        /// <summary>
        /// Gets the current feature displayed on the property pane
        /// </summary>
        MgFeature CurrentFeature { get; }

        /// <summary>
        /// Raised when a request to zoom to the current selected feature has been made
        /// </summary>
        event EventHandler RequestZoomToCurrentFeature;
    }

    /// <summary>
    /// Represents a selected feature
    /// </summary>
    public class MgFeature
    {
        /// <summary>
        /// Gets or sets the geometry for this feature
        /// </summary>
        public MgGeometry Geometry { get; set; }

        /// <summary>
        /// Constructor
        /// </summary>
        public MgFeature() 
        { 
            this.Properties = new Hashtable(); 
        }

        /// <summary>
        /// The attributes of this feature
        /// </summary>
        public IDictionary Properties { get; private set; }
    }

    /// <summary>
    /// Represents a set of selected features
    /// </summary>
    public class MgSelectionSet : IDisposable
    {
        private MgAgfReaderWriter _agfRw;

        /// <summary>
        /// Gets whether this selection set is empty
        /// </summary>
        public bool IsEmpty
        {
            get { return _features.Count == 0; }
        }

        /// <summary>
        /// Gest the number of features in this selection
        /// </summary>
        public int TotalCount
        {
            get
            {
                int total = 0;
                foreach (var list in _features.Values)
                {
                    foreach (var feat in list)
                    {
                        total++;
                    }
                }
                return total;
            }
        }

        /// <summary>
        /// Disposes of this instance
        /// </summary>
        public void Dispose()
        {
            if (_agfRw != null)
            {
                _agfRw.Dispose();
                _agfRw = null;
            }

            foreach (var list in _features.Values)
            {
                foreach (var feat in list)
                {
                    if (feat.Geometry != null)
                    {
                        feat.Geometry.Dispose();
                        feat.Geometry = null;
                    }
                }
            }

            _features.Clear();
        }

        private Dictionary<string, List<MgFeature>> _features;

        /// <summary>
        /// Gets an array of layers that are included in this selection set
        /// </summary>
        public string[] LayerNames { get { return new List<string>(_features.Keys).ToArray(); } }

        /// <summary>
        /// Gets an array of selected features that belong to the specified layer
        /// </summary>
        /// <param name="layerName"></param>
        /// <returns></returns>
        public MgFeature[] GetFeaturesForLayer(string layerName)
        {
            return _features[layerName].ToArray();
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="MgSelectionSet"/> class.
        /// </summary>
        /// <param name="selection">The selection.</param>
        public MgSelectionSet(MgSelectionBase selection)
            : this(selection, null)
        {
            
        }

        internal MgSelectionSet(MgSelectionBase selection, MgMapViewerProvider provider)
        {
            _agfRw = new MgAgfReaderWriter();
            _features = new Dictionary<string, List<MgFeature>>();
            var selLayers = selection.GetLayers();
            if (selLayers != null)
            {
                for (int i = 0; i < selLayers.GetCount(); i++)
                {
                    var layer = selLayers.GetItem(i);
                    var clsName = layer.GetFeatureClassName();
                    int selCount = selection.GetSelectedFeaturesCount(layer, clsName);
                    if (selCount == 0)
                    {
                        System.Diagnostics.Trace.TraceWarning("No items in selection set for this layer (" + layer.GetName() + "). Layer shouldn't be here then");
                        continue;
                    }

                    NameValueCollection mappings = (provider != null) ? provider.GetPropertyMappings(layer) : null;
                    _features[layer.Name] = new List<MgFeature>();
                    
                    var reader = selection.GetSelectedFeatures(layer, layer.GetFeatureClassName(), false);
                    while (reader.ReadNext())
                    {
                        var feature = CreateFeatureFromReader(reader);
                        if (mappings != null)
                            PerformDisplayNameSubstitution(feature, mappings);
                        _features[layer.Name].Add(feature);
                    }
                    reader.Close();
                }
            }
        }

        private static void PerformDisplayNameSubstitution(MgFeature feature, NameValueCollection mappings)
        {
            var remove = new List<string>();
            foreach (var name in feature.Properties.Keys)
            {
                if (mappings[name.ToString()] == null)
                    remove.Add(name.ToString());
            }

            foreach (var name in remove)
            {
                feature.Properties.Remove(name);
            }

            foreach (var key in mappings.AllKeys)
            {
                object value = feature.Properties[key];
                feature.Properties.Remove(key);
                feature.Properties[mappings[key]] = value;
            }
        }

        private MgFeature CreateFeatureFromReader(MgFeatureReader reader)
        {
            var feature = new MgFeature();
            var hash = feature.Properties;
            for (int i = 0; i < reader.GetPropertyCount(); i++)
            {
                var pt = reader.GetPropertyType(i);
                //Skip un-presentable types
                if (pt == MgPropertyType.Null ||
                    pt == MgPropertyType.Raster ||
                    pt == MgPropertyType.Blob ||
                    pt == MgPropertyType.Feature ||
                    pt == MgPropertyType.Clob)
                    continue;

                string name = reader.GetPropertyName(i);

                if (reader.IsNull(i))
                {
                    hash[name] = null;
                }
                else
                {
                    switch (pt)
                    {
                        case MgPropertyType.Boolean:
                            hash[name] = reader.GetBoolean(i);
                            break;
                        case MgPropertyType.Byte:
                            hash[name] = reader.GetByte(i);
                            break;
                        case MgPropertyType.DateTime:
                            {
                                var dt = reader.GetDateTime(i);
                                hash[name] = new DateTime(dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second, 0);
                            }
                            break;
                        case MgPropertyType.Double:
                        case MgPropertyType.Decimal:
                            hash[name] = reader.GetDouble(i);
                            break;
                        case MgPropertyType.Int16:
                            hash[name] = reader.GetInt16(i);
                            break;
                        case MgPropertyType.Int32:
                            hash[name] = reader.GetInt32(i);
                            break;
                        case MgPropertyType.Int64:
                            hash[name] = reader.GetInt64(i);
                            break;
                        case MgPropertyType.Single:
                            hash[name] = reader.GetSingle(i);
                            break;
                        case MgPropertyType.String:
                            hash[name] = reader.GetString(i);
                            break;
                        case MgPropertyType.Geometry:
                            {
                                MgByteReader agf = reader.GetGeometry(i);
                                feature.Geometry = _agfRw.Read(agf);
                            }
                            break;
                    }
                }
            }
            return feature;
        }
    }
}
