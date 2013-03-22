using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Globalization;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    internal partial class MgBufferControlImpl : MgControlView
    {
        private IMapViewer _viewer;
        private MgResourceService _resSvc;
        private MgFeatureService _featSvc;
        private string _sessionId;

        public MgBufferControlImpl(IMapViewer viewer, string defaultLayerName, MeasurementUnit units)
        {
            InitializeComponent();
            this.Title = Strings.TitleBuffer;
            _viewer = viewer;
            _sessionId = Guid.NewGuid().ToString();
            var provider = viewer.GetProvider();
            _resSvc = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);
            _featSvc = (MgFeatureService)provider.CreateService(MgServiceType.FeatureService);

            cmbUnits.DataSource = Enum.GetValues(typeof(MeasurementUnit));
            cmbUnits.SelectedItem = units;
            cmbBorderPattern.DataSource = Enum.GetValues(typeof(StockPattern));
            cmbFillPattern.DataSource = Enum.GetValues(typeof(StockPattern));

            cmbBorderPattern.SelectedItem = StockPattern.Solid;
            cmbFillPattern.SelectedItem = StockPattern.Solid;

            pnlFillColor.BackColor = Color.Red;
            pnlBorderColor.BackColor = Color.Black;

            numBufferDistance.Value = 1;
            numFillTransparency.Value = 50;
            numLineThickness.Value = 1;

            txtBufferLayer.Text = defaultLayerName;

            _viewer.SelectionChanged += new EventHandler(OnViewerSelectedChanged);
            OnViewerSelectedChanged(this, EventArgs.Empty);
        }

        void OnViewerSelectedChanged(object sender, EventArgs e)
        {
            lstLayers.Items.Clear();
            var sel = _viewer.GetSelection();
            var layers = sel.GetLayers();
            if (layers != null)
            {
                for (var i = 0; i < layers.GetCount(); i++)
                {
                    var layer = layers.GetItem(i);
                    var name = layer.GetName();

                    lstLayers.Items.Add(name);
                }
            }

            lblSelectFeatures.Visible = (lstLayers.Items.Count == 0);
        }

        private void btnFillColor_Click(object sender, EventArgs e)
        {
            if (colorPicker.ShowDialog() == DialogResult.OK)
            {
                pnlFillColor.BackColor = colorPicker.Color;
            }
        }

        private void btnBorderColor_Click(object sender, EventArgs e)
        {
            if (colorPicker.ShowDialog() == DialogResult.OK)
            {
                pnlBorderColor.BackColor = colorPicker.Color;
            }
        }

        private void btnFillBackColor_Click(object sender, EventArgs e)
        {
            if (colorPicker.ShowDialog() == DialogResult.OK)
            {
                pnlFillBackColor.BackColor = colorPicker.Color;
            }
        }

        private void btnCreate_Click(object sender, EventArgs e)
        {
            btnCreate.Enabled = false;
            try
            {
                var layerName = txtBufferLayer.Text.Trim();
                if (string.IsNullOrEmpty(layerName))
                {
                    MessageBox.Show(Strings.MsgEnterNameForLayer);
                    return;
                }

                if (lstLayers.SelectedItems.Count == 0)
                {
                    MessageBox.Show(Strings.MsgIncludeLayersToBuffer);
                    return;
                }

                var map = _viewer.GetMap();
                var layers = map.GetLayers();
                var provider = _viewer.GetProvider();

                //From here, it's the same logic as buffer.aspx in .net MapGuide AJAX viewer
                MgResourceIdentifier fsId = new MgResourceIdentifier("Session:" + _sessionId + "//" + txtBufferLayer.Text + "_Buffer.FeatureSource"); //NOXLATE
                MgResourceIdentifier ldfId = new MgResourceIdentifier("Session:" + _sessionId + "//" + txtBufferLayer.Text + "_Buffer.LayerDefinition"); //NOXLATE

                MgLayerBase layer = Util.FindLayer(layers, txtBufferLayer.Text);
                string[] layerNames = GetLayerNames();

                double distance = Convert.ToDouble(numBufferDistance.Value);
                MeasurementUnit bUnits = (MeasurementUnit)cmbUnits.SelectedItem;
                switch (bUnits)
                {
                    case MeasurementUnit.Feet:
                        distance *= 0.30480;
                        break;
                    case MeasurementUnit.Kilometers:
                        distance *= 1000;
                        break;
                    case MeasurementUnit.Miles:
                        distance *= 1609.35;
                        break;
                }

                String srsDefMap = Util.GetMapSrs(map);
                MgCoordinateSystem srsMap = provider.GetMapCoordinateSystem();
                string mapSrsUnits = "";
                bool arbitraryMapSrs = (srsMap.GetType() == MgCoordinateSystemType.Arbitrary);
                if (arbitraryMapSrs)
                    mapSrsUnits = srsMap.GetUnits();

                String xtrans = String.Format("{0:x2}", ((int)(255 * Convert.ToInt32(numFillTransparency.Value) / 100))); //NOXLATE
                var lineColor = Util.ToHtmlColor(pnlBorderColor.BackColor);
                var foreColor = Util.ToHtmlColor(pnlFillColor.BackColor);
                var backColor = Util.ToHtmlColor(pnlFillBackColor.BackColor);
                String layerTempl = string.Format(Properties.Resources.AreaLayerDef,
                        fsId.ToString(),
                        "BufferSchema:Buffer", //NOXLATE
                        "GEOM", //NOXLATE
                        cmbFillPattern.SelectedItem,
                        xtrans + foreColor,
                        ((0 != 1/*transparent*/) ? "ff" : "00") + backColor, //NOXLATE
                        cmbBorderPattern.SelectedItem,
                        numLineThickness.Value.ToString(NumberFormatInfo.InvariantInfo),
                        lineColor
                );
                byte[] bytes = Encoding.UTF8.GetBytes(layerTempl);
                MgByteSource src = new MgByteSource(bytes, bytes.Length);
                MgByteReader layerDefContent = src.GetReader();
                _resSvc.SetResource(ldfId, layerDefContent, null);

                bool newBuffer = false;
                if (layer == null)
                {
                    newBuffer = true;

                    //Targetting a new layer. create a data source for it
                    //
                    MgClassDefinition classDef = new MgClassDefinition();

                    classDef.SetName("Buffer"); //NOXLATE
                    classDef.SetDescription("Feature class for buffer layer"); //NOXLATE
                    classDef.SetDefaultGeometryPropertyName("GEOM"); //NOXLATE

                    //Set KEY property
                    MgDataPropertyDefinition prop = new MgDataPropertyDefinition("KEY"); //NOXLATE
                    prop.SetDataType(MgPropertyType.Int32);
                    prop.SetAutoGeneration(true);
                    prop.SetReadOnly(true);
                    classDef.GetIdentityProperties().Add(prop);
                    classDef.GetProperties().Add(prop);

                    //Set ID property. Hold this segment ID
                    prop = new MgDataPropertyDefinition("ID"); //NOXLATE
                    prop.SetDataType(MgPropertyType.Int32);
                    classDef.GetProperties().Add(prop);

                    //Set geometry property
                    MgGeometricPropertyDefinition geomProp = new MgGeometricPropertyDefinition("GEOM"); //NOXLATE
                    //prop.SetGeometryTypes(MgFeatureGeometricType.mfgtSurface); //TODO use the constant when exposed
                    geomProp.SetGeometryTypes(4);
                    classDef.GetProperties().Add(geomProp);

                    //Create the schema
                    MgFeatureSchema schema = new MgFeatureSchema("BufferSchema", "Temporary buffer schema"); //NOXLATE
                    schema.GetClasses().Add(classDef);

                    //finally, creation of the feature source
                    MgFileFeatureSourceParams sdfParams = new MgFileFeatureSourceParams("OSGeo.SDF", "LatLong", map.GetMapSRS(), schema); //NOXLATE
                    _featSvc.CreateFeatureSource(fsId, sdfParams);

                    //Add layer to map
                    layer = provider.CreateLayer(ldfId);
                    layer.SetName(txtBufferLayer.Text);
                    layer.SetLegendLabel(txtBufferLayer.Text);
                    layer.SetDisplayInLegend(true);
                    layer.SetSelectable(true);
                    layers.Insert(0, layer);
                }
                else
                {
                    //data source already exist. clear its content
                    //
                    Util.ClearDataSource(_featSvc, fsId, "BufferSchema:Buffer"); //NOXLATE
                }

                var sel = _viewer.GetSelection();
                var selLayers = sel.GetLayers();

                MgAgfReaderWriter agfRW = new MgAgfReaderWriter();
                MgGeometryCollection bufferGeometries = new MgGeometryCollection();
                MgGeometry geomBuffer;

                MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
                int featId = 0;

                MgBatchPropertyCollection propCollection = new MgBatchPropertyCollection();

                int excludedLayers = 0;
                MgCoordinateSystem srsDs = null;
                MgGeometryCollection inputGeometries = new MgGeometryCollection();

                int bufferFeatures = 0;
                for (int li = 0; li < selLayers.GetCount(); li++)
                {
                    MgLayerBase selLayer = selLayers.GetItem(li);
                    bool inputLayer = false;
                    String selLayerName = selLayer.GetName();
                    for (int il = 0; il < layerNames.Length; il++)
                    {
                        if (layerNames[il].Equals(selLayerName))
                        {
                            inputLayer = true;
                            break;
                        }
                    }
                    if (inputLayer == false)
                    {
                        continue;
                    }

                    // get the data source SRS
                    //
                    MgResourceIdentifier featSourceId = new MgResourceIdentifier(selLayer.GetFeatureSourceId());
                    MgSpatialContextReader ctxs = _featSvc.GetSpatialContexts(featSourceId, false);
                    String srsDefDs = string.Empty;
                    if (ctxs != null && ctxs.ReadNext())
                        srsDefDs = ctxs.GetCoordinateSystemWkt();

                    if (srsDefDs == null || srsDefDs.Length == 0)
                    {
                        excludedLayers++;
                        continue;
                    }

                    var srsFactory = new MgCoordinateSystemFactory();
                    srsDs = srsFactory.Create(srsDefDs);
                    bool arbitraryDsSrs = (srsDs.GetType() == MgCoordinateSystemType.Arbitrary);
                    String dsSrsUnits = string.Empty;

                    if (arbitraryDsSrs)
                        dsSrsUnits = srsDs.GetUnits();

                    // exclude layer if:
                    //  the map is non-arbitrary and the layer is arbitrary or vice-versa
                    //     or
                    //  layer and map are both arbitrary but have different units
                    //
                    if ((arbitraryDsSrs != arbitraryMapSrs) || (arbitraryDsSrs && (dsSrsUnits != mapSrsUnits)))
                    {
                        excludedLayers++;
                        continue;
                    }

                    // calculate distance in the data source SRS units
                    //
                    double dist = srsDs.ConvertMetersToCoordinateSystemUnits(distance);

                    // calculate great circle unless data source srs is arbitrary
                    MgCoordinateSystemMeasure measure;
                    if (!arbitraryDsSrs)
                        measure = srsDs.GetMeasure();
                    else
                        measure = null;

                    // create a SRS transformer if necessary
                    MgCoordinateSystemTransform srsXform;
                    if (!srsDefDs.Equals(srsDefMap))
                        srsXform = srsFactory.GetTransform(srsDs, srsMap);
                    else
                        srsXform = null;

                    String featureClassName = selLayer.GetFeatureClassName();
                    String filter = sel.GenerateFilter(selLayer, featureClassName);
                    if (filter == null || filter.Length == 0)
                        continue;

                    MgFeatureQueryOptions query = new MgFeatureQueryOptions();
                    query.SetFilter(filter);

                    MgResourceIdentifier featureSource = new MgResourceIdentifier(selLayer.GetFeatureSourceId());

                    MgFeatureReader features = _featSvc.SelectFeatures(featureSource, featureClassName, query);

                    if (features.ReadNext())
                    {
                        MgClassDefinition classDef = features.GetClassDefinition();
                        String geomPropName = classDef.GetDefaultGeometryPropertyName();

                        do
                        {
                            MgByteReader geomReader = features.GetGeometry(geomPropName);
                            MgGeometry geom = agfRW.Read(geomReader);

                            if (!chkMergeBuffers.Checked)
                            {
                                geomBuffer = geom.Buffer(dist, measure);
                                if (geomBuffer != null)
                                {
                                    if (srsXform != null)
                                        geomBuffer = (MgGeometry)geomBuffer.Transform(srsXform);
                                    Util.AddFeatureToCollection(propCollection, agfRW, featId++, geomBuffer);
                                    bufferFeatures++;
                                }
                            }
                            else
                            {
                                if (srsXform != null)
                                    geom = (MgGeometry)geom.Transform(srsXform);
                                inputGeometries.Add(geom);
                            }
                        }
                        while (features.ReadNext());

                        features.Close();
                    }
                }

                if (chkMergeBuffers.Checked)
                {
                    if (inputGeometries.GetCount() > 0)
                    {
                        double dist = srsMap.ConvertMetersToCoordinateSystemUnits(distance);
                        MgCoordinateSystemMeasure measure;
                        if (!arbitraryMapSrs)
                            measure = srsMap.GetMeasure();
                        else
                            measure = null;

                        MgGeometryFactory geomFactory = new MgGeometryFactory();
                        geomBuffer = geomFactory.CreateMultiGeometry(inputGeometries).Buffer(dist, measure);
                        if (geomBuffer != null)
                        {
                            Util.AddFeatureToCollection(propCollection, agfRW, featId, geomBuffer);
                            bufferFeatures = 1;
                        }
                    }
                }

                if (propCollection.GetCount() > 0)
                {
                    commands.Add(new MgInsertFeatures("BufferSchema:Buffer", propCollection)); //NOXLATE

                    //Insert the features in the temporary data source
                    //
                    Util.ReleaseReader(_featSvc.UpdateFeatures(fsId, commands, false), commands);
                }

                // Save the new map state
                //
                layer.ForceRefresh();
                _viewer.RefreshMap();

                //build report message
                if (newBuffer)
                    MessageBox.Show(string.Format(Strings.MsgBufferLayerCreated, txtBufferLayer.Text));
                else
                    MessageBox.Show(string.Format(Strings.MsgBufferLayerUpdated, txtBufferLayer.Text));
            }
            finally
            {
                btnCreate.Enabled = true;
            }
        }

        private string[] GetLayerNames()
        {
            var items = new List<string>();
            foreach (var it in lstLayers.Items)
            {
                items.Add(it.ToString());
            }
            return items.ToArray();
        }
    }

    public enum StockPattern
    {
        Solid,
        Net,
        Line,
        Line_45,
        Line_90,
        Line_135,
        Square,
        Box,
        Cross,
        Dash,
        Dolmit,
        Hex,
        Sacncr,
        Steel
    }
}
