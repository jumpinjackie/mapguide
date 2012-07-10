using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide;
using OSGeo.MapGuide.Viewer.AppLayoutEngine;

namespace SampleExtension
{
    public partial class DigitizingAndRedlining : MgControlView
    {
        private IMapViewer _viewer;
        private MgGeometryFactory _geomFact;
        private MgWktReaderWriter _wktRW;
        private MgAgfReaderWriter _agfRW;
        private MgdLayer _redlineLayer;

        public DigitizingAndRedlining(IMapViewer viewer)
        {
            InitializeComponent();
            _viewer = viewer;
            _geomFact = new MgGeometryFactory();
            _wktRW = new MgWktReaderWriter();
            _agfRW = new MgAgfReaderWriter();
            this.Title = "Digitizing and Redlining";
            _viewer.PropertyChanged += OnViewerPropertyChanged;
            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            if (layers.IndexOf("Redline") >= 0)
            {
                _redlineLayer = (MgdLayer)layers.GetItem("Redline");
            }
            CheckDigitizingState();
        }

        void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "DigitizingType")
                CheckDigitizingState();
        }

        private void InsertRedlineGeometry(MgGeometry geom)
        {
            MgPropertyCollection feature = new MgPropertyCollection();
            MgByteReader agf = _agfRW.Write(geom);
            MgGeometryProperty geomProp = new MgGeometryProperty("Geometry", agf);
            feature.Add(geomProp);

            _redlineLayer.ForceRefresh();
            var reader = _redlineLayer.InsertFeatures(feature);
            int inserted = 0;
            while (reader.ReadNext())
            {
                inserted++;
            }
            reader.Close();
            if (inserted > 0)
            {
                _viewer.RefreshMap();
                IMapLegend legend = Shell.Instance.Legend;
                if (legend != null)
                    legend.RefreshLegend();
            }
        }

        private void CheckRedlineLayer()
        {
            if (_redlineLayer == null)
            {
                MgdMap map = (MgdMap)_viewer.GetMap();
                MgMapViewerProvider provider = _viewer.GetProvider();
                MgdFeatureService featSvc = (MgdFeatureService)provider.CreateService(MgServiceType.FeatureService);
                MgResourceService resSvc = (MgResourceService)provider.CreateService(MgServiceType.ResourceService);

                string sessionId = Guid.NewGuid().ToString();
                MgResourceIdentifier fsId = new MgResourceIdentifier("Session:" + sessionId + "//Redline.FeatureSource");
                MgResourceIdentifier ldfId = new MgResourceIdentifier("Session:" + sessionId + "//Redline.LayerDefinition");
                string featureClass = "Default:Redline";
                string geometry = "Geometry";
                string xml = string.Format(Layers.Redline, fsId.ToString(), featureClass, geometry);

                //Construct our schema for the redline data store
                MgFeatureSchema schema = new MgFeatureSchema("Default", "Redline schema");
                MgClassDefinition cls = new MgClassDefinition();
                cls.Name = "Redline";

                MgDataPropertyDefinition id = new MgDataPropertyDefinition("ID");
                id.DataType = MgPropertyType.Int32;
                id.SetAutoGeneration(true);

                MgGeometricPropertyDefinition geom = new MgGeometricPropertyDefinition(geometry);
                geom.SpatialContextAssociation = "Default";
                geom.GeometryTypes = MgFeatureGeometricType.Curve | MgFeatureGeometricType.Point | MgFeatureGeometricType.Solid | MgFeatureGeometricType.Surface;

                MgPropertyDefinitionCollection clsProps = cls.GetProperties();
                clsProps.Add(id);
                clsProps.Add(geom);

                MgPropertyDefinitionCollection idProps = cls.GetIdentityProperties();
                idProps.Add(id);

                cls.DefaultGeometryPropertyName = geometry;
                MgClassDefinitionCollection classes = schema.GetClasses();
                classes.Add(cls);

                //Create the feature source
                MgCreateSdfParams create = new MgCreateSdfParams("Default", map.GetMapSRS(), schema);
                featSvc.CreateFeatureSource(fsId, create);

                //Then the layer definition
                byte[] bytes = Encoding.UTF8.GetBytes(xml);
                MgByteSource source = new MgByteSource(bytes, bytes.Length);
                resSvc.SetResource(ldfId, source.GetReader(), null);

                //Now create the runtime layer and add to map
                _redlineLayer = new MgdLayer(ldfId, resSvc);
                _redlineLayer.LegendLabel = "Redlining";
                _redlineLayer.Name = "Redline";
                _redlineLayer.Visible = true;
                _redlineLayer.Selectable = true;
                _redlineLayer.DisplayInLegend = true;

                MgLayerCollection layers = map.GetLayers();
                layers.Insert(0, _redlineLayer);
            }
        }

        private void CheckDigitizingState()
        {
            btnLine.Enabled = btnLineString.Enabled = btnPoint.Enabled = btnPolygon.Enabled = (_viewer.DigitizingType == MapDigitizationType.None);
        }

        protected override void SubCleanup()
        {
            _geomFact.Dispose();
            _geomFact = null;
            _viewer.PropertyChanged -= OnViewerPropertyChanged;
        }

        private void btnPoint_Click(object sender, EventArgs e)
        {
            _viewer.DigitizePoint(OnPointDigitized);
        }

        private void OnPointDigitized(double x, double y)
        {
            CheckRedlineLayer();
            MgGeometry point = _geomFact.CreatePoint(_geomFact.CreateCoordinateXY(x, y));
            InsertRedlineGeometry(point);
        }

        private void btnLine_Click(object sender, EventArgs e)
        {
            _viewer.DigitizeLine(OnLineDigitized);
        }

        private void OnLineDigitized(double x1, double y1, double x2, double y2)
        {
            CheckRedlineLayer();
            MgCoordinateCollection coords = new MgCoordinateCollection();
            coords.Add(_geomFact.CreateCoordinateXY(x1, y1));
            coords.Add(_geomFact.CreateCoordinateXY(x2, y2));
            MgGeometry line = _geomFact.CreateLineString(coords);
            InsertRedlineGeometry(line);
        }

        private void btnLineString_Click(object sender, EventArgs e)
        {
            _viewer.DigitizeLineString(OnLineStringDigitized);
        }

        private void OnLineStringDigitized(double[,] coordinates)
        {
            CheckRedlineLayer();
            MgCoordinateCollection coords = new MgCoordinateCollection();
            for (int i = 0; i < coordinates.GetLength(0); i++)
            {
                coords.Add(_geomFact.CreateCoordinateXY(coordinates[i, 0], coordinates[i, 1]));
            }
            MgGeometry line = _geomFact.CreateLineString(coords);
            InsertRedlineGeometry(line);
        }

        private void btnPolygon_Click(object sender, EventArgs e)
        {
            _viewer.DigitizePolygon(OnPolygonDigitized);
        }

        private void OnPolygonDigitized(double[,] coordinates)
        {
            CheckRedlineLayer();
            MgCoordinateCollection coords = new MgCoordinateCollection();
            for (int i = 0; i < coordinates.GetLength(0); i++)
            {
                coords.Add(_geomFact.CreateCoordinateXY(coordinates[i, 0], coordinates[i, 1]));
            }
            coords.Add(_geomFact.CreateCoordinateXY(coordinates[0, 0], coordinates[0, 1]));
            MgLinearRing ring = _geomFact.CreateLinearRing(coords);
            MgGeometry poly = _geomFact.CreatePolygon(ring, null);
            InsertRedlineGeometry(poly);
        }

        private void btnClearRedlines_Click(object sender, EventArgs e)
        {
            if (_redlineLayer != null)
            {
                _redlineLayer.DeleteFeatures(""); //Empty string means delete everything
                _viewer.RefreshMap();
                MessageBox.Show("Redlines cleared");
            }
        }
    }
}
