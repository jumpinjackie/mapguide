using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Globalization;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    public partial class MgThemeControlImpl : MgControlView
    {
        private IMapViewer _viewer;
        private BindingList<MgDataPropertyDefinition> _properties;
        private BindingList<MgLayerBase> _layers;
        private MgResourceService _resSvc;
        private string _sessionId;

        private Dictionary<string, string> _distros;

        public MgThemeControlImpl(IMapViewer viewer)
        {
            InitializeComponent();
            this.Title = Strings.TitleTheme;
            this.Disposed += new EventHandler(OnDisposed);
            _viewer = viewer;
            _properties = new BindingList<MgDataPropertyDefinition>();
            _layers = new BindingList<MgLayerBase>();

            numRules.Minimum = Int32.MinValue;
            numRules.Maximum = Int32.MaxValue;

            _distros = new Dictionary<string,string>() 
            {
                { THEME_INDIVIDUAL, Strings.ThemeIndividual },
                { THEME_EQUAL, Strings.ThemeEqual },
                { THEME_STDDEV, Strings.ThemeStandardDeviation },
                { THEME_QUANT, Strings.ThemeQuantile },
                { THEME_JENK, Strings.ThemeJenks }
            };

            cmbLayer.DataSource = _layers;
            cmbProperty.DataSource = _properties;
            cmbDistribution.DataSource = GetDistroValues(THEME_INDIVIDUAL, THEME_EQUAL, THEME_STDDEV, THEME_QUANT, THEME_JENK);

            var map = viewer.GetMap();
            var layers = map.GetLayers();
            //TODO: Obviously support point and line layers
            for (var i = 0; i < layers.GetCount(); i++)
            {
                var layer = layers.GetItem(i);
                var cls = layer.GetClassDefinition();
                var geom = layer.GetFeatureGeometryName();
                if (string.IsNullOrEmpty(geom))
                    continue;

                var clsProps = cls.GetProperties();
                var geomProp = clsProps.GetItem(geom) as MgGeometricPropertyDefinition;
                if ((geomProp.GeometryTypes & MgFeatureGeometricType.Surface) == MgFeatureGeometricType.Surface)
                {
                    _layers.Add(layer);
                }
            }
            cmbLayer.SelectedIndex = 0;
            cmbLayer_SelectedIndexChanged(this, EventArgs.Empty);
            cmbDistribution.SelectedIndex = 0;
            _sessionId = Guid.NewGuid().ToString();
        }

        private KeyValuePair<string, string>[] GetDistroValues(params string[] distroKeys)
        {
            var values = new List<KeyValuePair<string, string>>();
            foreach (var key in distroKeys)
            {
                if (_distros.ContainsKey(key))
                    values.Add(new KeyValuePair<string, string>(key, _distros[key]));
            }
            return values.ToArray();
        }

        void OnDisposed(object sender, EventArgs e)
        {
            _properties.Clear();
            _layers.Clear();
            if (_resSvc != null)
            {
                _resSvc.Dispose();
                _resSvc = null;
            }
        }

        private void cmbLayer_SelectedIndexChanged(object sender, EventArgs e)
        {
            var layer = cmbLayer.SelectedItem as MgLayerBase;
            if (layer != null)
            {
                var cls = layer.GetClassDefinition();
                var clsProps = cls.GetProperties();
                _properties.Clear();
                for (var i = 0; i < clsProps.GetCount(); i++)
                {
                    var prop = clsProps.GetItem(i);
                    if (prop.GetPropertyType() == MgFeaturePropertyType.DataProperty)
                    {
                        _properties.Add((MgDataPropertyDefinition)prop);
                    }
                }
                cmbProperty.SelectedIndex = 0;
                cmbProperty_SelectedIndexChanged(this, EventArgs.Empty);
                cmbScaleRange.DataSource = GetScaleRanges(layer);
            }
        }

        private string[] GetScaleRanges(MgLayerBase layer)
        {
            if (_resSvc == null)
            {
                var provider = _viewer.GetProvider();
                _resSvc = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
            }

            MgResourceIdentifier layerDefResId = layer.GetLayerDefinition();
            MgByteReader byteReader = _resSvc.GetResourceContent(layerDefResId);

            XmlDocument doc = new XmlDocument();
            doc.LoadXml(byteReader.ToString());
            XmlNodeList nodeList = doc.GetElementsByTagName("VectorScaleRange"); //NOXLATE

            var scaleRanges = new List<string>();
            foreach (XmlElement node in nodeList)
            {
                String range = null;

                XmlNodeList minNodeList = node.GetElementsByTagName("MinScale"); //NOXLATE
                if (minNodeList.Count > 0)
                {
                    range = minNodeList.Item(0).FirstChild.Value;
                }
                else
                {
                    range = "0"; //NOXLATE
                }

                XmlNodeList maxNodeList = node.GetElementsByTagName("MaxScale"); //NOXLATE
                if (maxNodeList.Count > 0)
                {
                    range = range + " - " + maxNodeList.Item(0).FirstChild.Value; //NOXLATE
                }
                else
                {
                    range = range + " - " + Strings.Infinity; //NOXLATE
                }

                scaleRanges.Add(range);
            }
            return scaleRanges.ToArray();
        }

        private int featureCount;
        private int ruleCount = 8;

        private void cmbDistribution_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cmbDistribution.SelectedIndex == 0)
            {
                numRules.Enabled = false;
                numRules.Value = featureCount;
            }
            else
            {
                numRules.Enabled = true;
                numRules.Value = ruleCount;
            }
        }

        private void cmbProperty_SelectedIndexChanged(object sender, EventArgs e)
        {
            var layer = cmbLayer.SelectedItem as MgLayerBase;
            var prop = cmbProperty.SelectedItem as MgDataPropertyDefinition;
            if (layer != null && prop != null)
            {
                SetPropertyMinMaxCount(layer, prop);
                cmbDistribution.DataSource = GetDistributionsForDataType(prop.DataType);
            }
        }

        private void btnFromFillColor_Click(object sender, EventArgs e)
        {
            if (colorDialog.ShowDialog() == DialogResult.OK)
            {
                pnlFromFillColor.BackColor = colorDialog.Color;
            }
        }

        private void btnToFillColor_Click(object sender, EventArgs e)
        {
            if (colorDialog.ShowDialog() == DialogResult.OK)
            {
                pnlToFillColor.BackColor = colorDialog.Color;
            }
        }

        private void btnFromBorderColor_Click(object sender, EventArgs e)
        {
            if (colorDialog.ShowDialog() == DialogResult.OK)
            {
                pnlFromBorderColor.BackColor = colorDialog.Color;
            }
        }

        private void btnToBorderColor_Click(object sender, EventArgs e)
        {
            if (colorDialog.ShowDialog() == DialogResult.OK)
            {
                pnlToBorderColor.BackColor = colorDialog.Color;
            }
        }

        private void btnApply_Click(object sender, EventArgs e)
        {
            var tp = new ThemeParams() 
            {
                borderFrom = Util.ToHtmlColor(pnlFromBorderColor.BackColor),
                borderTo = Util.ToHtmlColor(pnlToBorderColor.BackColor),
                distro = cmbDistribution.SelectedValue.ToString(),
                fillFrom = Util.ToHtmlColor(pnlFromFillColor.BackColor),
                fillTo = Util.ToHtmlColor(pnlToFillColor.BackColor),
                fillTrans = Convert.ToInt32(numFillTransparency.Value),
                layer = (MgLayerBase)cmbLayer.SelectedItem,
                maxValue = txtMax.Text,
                minValue = txtMin.Text,
                numRules = Convert.ToInt32(numRules.Value),
                property = (MgDataPropertyDefinition)cmbProperty.SelectedItem,
                scaleRangeIndex = cmbScaleRange.SelectedIndex,
                themeName = txtThemeName.Text
            };
            string name = ApplyTheme(tp);
            _viewer.RefreshMap();
            MessageBox.Show(string.Format(Strings.MsgThemeLayerCreated, name));
        }

        private void SetPropertyMinMaxCount(MgLayerBase layer, MgDataPropertyDefinition prop)
        {
            var provider = _viewer.GetProvider();

            var featureService = (MgFeatureService)provider.CreateService(MgServiceType.FeatureService);
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());

            String minValue = null;
            String maxValue = null;
            int count = 0;

            MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
            queryOptions.AddFeatureProperty(prop.Name);

            MgFeatureReader featureReader = featureService.SelectFeatures(resId, layer.GetFeatureClassName(), queryOptions);
            Dictionary<string, string> bucket = new Dictionary<string, string>();
            while (featureReader.ReadNext())
            {
                String value = Util.GetFeaturePropertyValue(featureReader, prop.Name);
                bucket[value] = value;
                int propertyType = featureReader.GetPropertyType(prop.Name);
                if (count == 0)
                {
                    maxValue = value;
                    minValue = value;
                }
                switch (propertyType)
                {
                    case MgPropertyType.String:
                        if (value.Length > 0)
                        {
                            if (value.CompareTo(maxValue) > 0)
                                maxValue = value;
                            if (value.CompareTo(minValue) < 0)
                                minValue = value;
                        }
                        break;
                    case MgPropertyType.Byte:
                    case MgPropertyType.Int16:
                    case MgPropertyType.Int32:
                    case MgPropertyType.Int64:
                        if (value.Length > 0)
                        {
                            if (Int64.Parse(value) > Int64.Parse(maxValue))
                                maxValue = value;
                            if (Int64.Parse(value) < Int64.Parse(minValue))
                                minValue = value;
                        }
                        break;
                    case MgPropertyType.Single:
                    case MgPropertyType.Double:
                        if (value != null)
                        {
                            if (Double.Parse(value) > Double.Parse(maxValue))
                                maxValue = value;
                            if (Double.Parse(value) < Double.Parse(minValue))
                                minValue = value;
                        }
                        count++;
                        break;
                    case MgPropertyType.Boolean:
                    case MgPropertyType.DateTime:
                    case MgPropertyType.Null:
                    case MgPropertyType.Blob:
                    case MgPropertyType.Clob:
                    case MgPropertyType.Feature:
                    case MgPropertyType.Geometry:
                        break;
                }
                count++;
            }
            featureReader.Close();

            txtMin.Text = minValue;
            txtMax.Text = maxValue;
            if (THEME_INDIVIDUAL == cmbDistribution.SelectedValue.ToString())
                count = bucket.Count;
            numRules.Value = featureCount = count;
        }

        const string THEME_INDIVIDUAL = "INDIV_DIST"; //NOXLATE
        const string THEME_EQUAL = "EQUAL_DIST"; //NOXLATE
        const string THEME_STDDEV = "STDEV_DIST"; //NOXLATE
        const string THEME_QUANT = "QUANT_DIST"; //NOXLATE
        const string THEME_JENK = "JENK_DIST"; //NOXLATE

        class ThemeParams
        {
            public MgLayerBase layer;
            public MgDataPropertyDefinition property;
            public int scaleRangeIndex;
            public int numRules;
            public string distro;
            public string fillFrom;
            public string fillTo;
            public string borderFrom;
            public string borderTo;
            public int fillTrans;
            public string minValue;
            public string maxValue;
            public string themeName;
            //public string layerName;
        }

        private string ApplyTheme(ThemeParams themeParams)
        {
            var provider = _viewer.GetProvider();
            var map = _viewer.GetMap();
            var layers = map.GetLayers();

            MgResourceService resourceService = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
            MgFeatureService featureService = (MgFeatureService)provider.CreateService(MgServiceType.FeatureService);

            MgResourceIdentifier resId = new MgResourceIdentifier(themeParams.layer.GetFeatureSourceId());
            MgResourceIdentifier layerDefResId = themeParams.layer.GetLayerDefinition();
            MgByteReader byteReader = resourceService.GetResourceContent(layerDefResId);

            // Load the Layer Definition and Navigate to the specified <VectorScaleRange>

            XmlDocument doc = new XmlDocument();
            String xmlLayerDef = byteReader.ToString();
            doc.LoadXml(xmlLayerDef);
            XmlNodeList nodeList = doc.GetElementsByTagName("VectorScaleRange"); //NOXLATE

            XmlElement vectorScaleRangecElement = (XmlElement)nodeList.Item(themeParams.scaleRangeIndex);
            XmlElement areaTypeStyle = (XmlElement)vectorScaleRangecElement.GetElementsByTagName("AreaTypeStyle").Item(0); //NOXLATE

            // Remove any existing <AreaRule> elements.

            XmlNodeList areaRuleList = areaTypeStyle.GetElementsByTagName("AreaRule"); //NOXLATE
            int count = areaRuleList.Count;
            for (int i = 0; i < count; i++)
            {
                //The areaRuleList shrinks as we remove items, so always
                //remove the first item (don't use the index i)
                areaTypeStyle.RemoveChild(areaRuleList.Item(0));
            }

            // Now create the new <AreaRule> elements.

            String areaRuleTemplate = Properties.Resources.AreaRuleTemplate;
            MgFeatureAggregateOptions aggregateOptions = new MgFeatureAggregateOptions();

            String value = null;
            String filterText = null;
            String areaRuleXML = null;
            XmlDocument areaDoc = null;
            XmlNode areaNode = null;
            double portion = 0.0;
            double increment = (themeParams.numRules > 1) ? 1.0 / (themeParams.numRules - 1) : 1.0;

            if (THEME_INDIVIDUAL == themeParams.distro)
            {
                aggregateOptions.AddComputedProperty("THEME_VALUE", "UNIQUE(\"" + themeParams.property.Name + "\")"); //NOXLATE

                MgDataReader dataReader = featureService.SelectAggregate(resId, themeParams.layer.GetFeatureClassName(), aggregateOptions);
                while (dataReader.ReadNext())
                {
                    value = Util.GetFeaturePropertyValue(dataReader, "THEME_VALUE"); //NOXLATE

                    filterText = "&quot;" + themeParams.property.Name + "&quot; = "; //NOXLATE
                    if (themeParams.property.DataType == MgPropertyType.String)
                        filterText = filterText + "'" + value + "'"; //NOXLATE
                    else
                        filterText = filterText + value;

                    areaRuleXML = String.Format(areaRuleTemplate,
                                                themeParams.property.Name + ":" + value, //NOXLATE
                                                filterText,
                                                Util.InterpolateColor(portion, themeParams.fillFrom, themeParams.fillTo, themeParams.fillTrans),
                                                Util.InterpolateColor(portion, themeParams.borderFrom, themeParams.borderTo, 0));
                    areaDoc = new XmlDocument();
                    areaDoc.LoadXml(areaRuleXML);
                    areaNode = doc.ImportNode(areaDoc.DocumentElement, true);
                    areaTypeStyle.AppendChild(areaNode);

                    portion = portion + increment;
                }
                dataReader.Close();
            }
            else
            {
                var values = new List<string>();

                var expr = themeParams.distro + "(\"" + themeParams.property.Name + "\"," + themeParams.numRules + "," + themeParams.minValue + "," + themeParams.maxValue + ")"; //NOXLATE
                aggregateOptions.AddComputedProperty("THEME_VALUE", expr); //NOXLATE
                MgDataReader dataReader = featureService.SelectAggregate(resId, themeParams.layer.GetFeatureClassName(), aggregateOptions);
                while (dataReader.ReadNext())
                {
                    value = Util.GetFeaturePropertyValue(dataReader, "THEME_VALUE"); //NOXLATE
                    values.Add(value);
                }
                dataReader.Close();

                for (int i = 0; i < values.Count - 1; i++)
                {
                    filterText = "&quot;" + themeParams.property.Name + "&quot; &gt;= " + values[i] + " AND &quot;" + themeParams.property.Name; //NOXLATE
                    if (i == values.Count - 1)
                        filterText = filterText + "&quot; &lt;= " + values[i + 1]; //NOXLATE
                    else
                        filterText = filterText + "&quot; &lt; " + values[i + 1]; //NOXLATE

                    areaRuleXML = String.Format(areaRuleTemplate,
                                                themeParams.property.Name + ":" + values[i] + " - " + values[i + 1], //NOXLATE
                                                filterText,
                                                Util.InterpolateColor(portion, themeParams.fillFrom, themeParams.fillTo, themeParams.fillTrans),
                                                Util.InterpolateColor(portion, themeParams.borderFrom, themeParams.borderTo, 0));

                    areaDoc = new XmlDocument();
                    areaDoc.LoadXml(areaRuleXML);
                    areaNode = doc.ImportNode(areaDoc.DocumentElement, true);
                    areaTypeStyle.AppendChild(areaNode);

                    portion = portion + increment;
                }
            }

            // Now save our new layer definition to the session and add it to the map.

            String xmlString = doc.DocumentElement.OuterXml;
            String uniqueName = Util.MakeUniqueLayerName(map, themeParams.layer.Name, themeParams.themeName);
            String legendLabel = themeParams.layer.GetLegendLabel();
            if (!string.IsNullOrEmpty(themeParams.themeName))
                legendLabel = legendLabel + " (" + themeParams.themeName + ")"; //NOXLATE

            MgResourceIdentifier layerResId = new MgResourceIdentifier("Session:" + _sessionId + "//" + uniqueName + ".LayerDefinition"); //NOXLATE
            resourceService.SetResource(layerResId, new MgByteReader(xmlString, "text/xml"), null); //NOXLATE

            var newLayer = provider.CreateLayer(layerResId);
            newLayer.SetName(uniqueName);
            newLayer.SetLegendLabel(legendLabel);
            newLayer.SetDisplayInLegend(themeParams.layer.GetDisplayInLegend());
            newLayer.SetVisible(true);
            newLayer.SetSelectable(themeParams.layer.GetSelectable());
            //HACK: This has to be true otherwise owner-drawn nodes will not display its children (the theme rules)
            provider.SetLayerExpandInLegend(newLayer, true);
            layers.Insert(layers.IndexOf(themeParams.layer), newLayer);

            //map.Save(resourceService);

            return uniqueName;
        }

        private KeyValuePair<string, string>[] GetDistributionsForDataType(int type)
        {
            var distroTypes = new List<string>();

            switch (type)
            {
                case MgPropertyType.String:
                    distroTypes.Add(THEME_INDIVIDUAL);
                    break;
                case MgPropertyType.Byte:
                case MgPropertyType.Int16:
                case MgPropertyType.Int32:
                case MgPropertyType.Int64:
                    distroTypes.Add(THEME_INDIVIDUAL);
                    distroTypes.Add(THEME_EQUAL);
                    distroTypes.Add(THEME_STDDEV);
                    distroTypes.Add(THEME_QUANT);
                    distroTypes.Add(THEME_JENK);
                    break;
                case MgPropertyType.Single:
                case MgPropertyType.Double:
                    distroTypes.Add(THEME_EQUAL);
                    distroTypes.Add(THEME_STDDEV);
                    distroTypes.Add(THEME_QUANT);
                    distroTypes.Add(THEME_JENK);
                    break;
                case MgPropertyType.Boolean:
                case MgPropertyType.DateTime:
                case MgPropertyType.Blob:
                case MgPropertyType.Clob:
                case MgPropertyType.Feature:
                case MgPropertyType.Geometry:
                case MgPropertyType.Null:
                    break;
            }
            return GetDistroValues(distroTypes.ToArray());
        }
    }
}
