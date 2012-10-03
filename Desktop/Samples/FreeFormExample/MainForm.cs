using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer.Desktop;
using OSGeo.MapGuide;
using OSGeo.MapGuide.Viewer;
using System.Globalization;

namespace FreeFormExample
{
    public partial class MainForm : Form, IMapStatusBar
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private MgWktReaderWriter _wktRw;
        private MgGeometryFactory _geomFact;

        protected override void OnLoad(EventArgs e)
        {
            _wktRw = new MgWktReaderWriter();
            _geomFact = new MgGeometryFactory();
            new MapViewerController(mgMapViewer1,          //The MgMapViewer
                                    mgLegend1,             //The MgLegend
                                    this,                  //The IMapStatusBar
                                    mgPropertyPane1);      //The MgPropertyPane

            MgdServiceFactory factory = new MgdServiceFactory();
            MgdResourceService resSvc = (MgdResourceService)factory.CreateService(MgServiceType.ResourceService);
            MgResourceIdentifier mapDefId = new MgResourceIdentifier("Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition");
            //If this map definition doesn't exist, we ask the user to
            //load the Sheboygan package
            if (!resSvc.ResourceExists(mapDefId))
            {
                using (OpenFileDialog diag = new OpenFileDialog())
                {
                    diag.Filter = "MapGuide Packages (*.mgp)|*.mgp";
                    if (diag.ShowDialog() == DialogResult.OK)
                    {
                        MgByteSource source = new MgByteSource(diag.FileName);
                        MgByteReader reader = source.GetReader();
                        resSvc.ApplyResourcePackage(reader);
                    }
                    else
                    {
                        //No map, nothing to do here
                        Application.Exit();
                    }
                }
            }

            //Create our runtime map
            MgdMap map = new MgdMap(mapDefId);
            //Create our viewer provider
            MgMapViewerProvider provider = new MgDesktopMapViewerProvider(map);
            //Initialize our viewer with this provider
            mgMapViewer1.Init(provider);
            UpdateButtonCheckedState();
        }

        public void SetCursorPositionMessage(string message)
        {
            lblCoords.Text = message;
        }

        public void SetFeatureSelectedMessage(string message)
        {
            lblSelected.Text = message;
        }

        public void SetMapScaleMessage(string message)
        {
            lblScale.Text = message;
        }

        public void SetMapSizeMessage(string message)
        {
            lblSize.Text = message;
        }

        private void mgMapViewer1_SelectionChanged(object sender, EventArgs e)
        {
            MgSelectionBase selection = mgMapViewer1.GetSelection();
            MgReadOnlyLayerCollection layers = selection.GetLayers();
            if (layers != null)
            {
                for (int i = 0; i < layers.GetCount(); i++)
                {
                    MgLayerBase layer = layers.GetItem(i);
                    if (layer.Name == "Parcels") //The selected layer is parcels
                    {
                        //Check that we only have one selected object
                        int count = selection.GetSelectedFeaturesCount(layer, layer.FeatureClassName);
                        if (count == 1)
                        {
                            MgFeatureReader reader = null;
                            try
                            {
                                reader = selection.GetSelectedFeatures(layer, layer.FeatureClassName, false);
                                if (reader.ReadNext())
                                {
                                    //Address is in the RPROPAD property
                                    if (reader.IsNull("RPROPAD"))
                                        MessageBox.Show("Selected parcel has no address");
                                    else
                                        MessageBox.Show("Address: " + reader.GetString("RPROPAD"));
                                }
                            }
                            finally //Must close all readers, otherwise connections will leak
                            {
                                reader.Close();
                            }
                        }
                        else
                        {
                            MessageBox.Show("Please select only one parcel");
                        }
                        break;
                    }
                }
            }
        }

        private void btnZoomIn_Click(object sender, EventArgs e)
        {
            mgMapViewer1.ActiveTool = MapActiveTool.ZoomIn;
        }

        private void btnZoomOut_Click(object sender, EventArgs e)
        {
            mgMapViewer1.ActiveTool = MapActiveTool.ZoomOut;
        }

        private void btnZoomExtents_Click(object sender, EventArgs e)
        {
            mgMapViewer1.InitialMapView();
        }

        private void btnSelect_Click(object sender, EventArgs e)
        {
            mgMapViewer1.ActiveTool = MapActiveTool.Select;
        }

        private void btnSelectPolygon_Click(object sender, EventArgs e)
        {
            mgMapViewer1.DigitizePolygon((coordinates) =>
            {
                MgCoordinateCollection coords = new MgCoordinateCollection();
                for (int i = 0; i < coordinates.GetLength(0); i++)
                {
                    coords.Add(_geomFact.CreateCoordinateXY(coordinates[i, 0], coordinates[i, 1]));
                }
                coords.Add(_geomFact.CreateCoordinateXY(coordinates[0, 0], coordinates[0, 1]));
                MgLinearRing ring = _geomFact.CreateLinearRing(coords);
                MgGeometry poly = _geomFact.CreatePolygon(ring, null);
                mgMapViewer1.SelectByGeometry(poly);
            });
        }

        static string MakeWktCircle(double x, double y, double r)
        {
            return "CURVEPOLYGON ((" + (x - r).ToString(CultureInfo.InvariantCulture) + " " + y.ToString(CultureInfo.InvariantCulture) + " (CIRCULARARCSEGMENT (" + x.ToString(CultureInfo.InvariantCulture) + " " + (y - r).ToString(CultureInfo.InvariantCulture) + ", " + (x + r).ToString(CultureInfo.InvariantCulture) + " " + y.ToString(CultureInfo.InvariantCulture) + "), CIRCULARARCSEGMENT (" + x.ToString(CultureInfo.InvariantCulture) + " " + (y + r).ToString(CultureInfo.InvariantCulture) + ", " + (x - r).ToString(CultureInfo.InvariantCulture) + " " + y.ToString(CultureInfo.InvariantCulture) + "))))"; //NOXLATE
        }

        static string MakeWktPolygon(double x1, double y1, double x2, double y2)
        {
            string x1str = x1.ToString(CultureInfo.InvariantCulture);
            string y1str = y1.ToString(CultureInfo.InvariantCulture);
            string x2str = x2.ToString(CultureInfo.InvariantCulture);
            string y2str = y2.ToString(CultureInfo.InvariantCulture);
            return "POLYGON((" + x1str + " " + y1str + ", " + x2str + " " + y1str + ", " + x2str + " " + y2str + ", " + x1str + " " + y2str + ", " + x1str + " " + y1str + "))"; //NOXLATE
        }

        private void btnSelectCircle_Click(object sender, EventArgs e)
        {
            mgMapViewer1.DigitizeCircle((x, y, r) =>
            {
                MgGeometry geom = _wktRw.Read(MakeWktCircle(x, y, r));
                mgMapViewer1.SelectByGeometry(geom);
            });
        }

        private void btnClearSelection_Click(object sender, EventArgs e)
        {
            mgMapViewer1.ClearSelection();
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            mgMapViewer1.RefreshMap();
        }

        private void mgMapViewer1_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsBusy") //NOXLATE
            {
                var busy = mgMapViewer1.IsBusy;
                btnZoomExtents.Enabled = btnZoomIn.Enabled
                                         = btnZoomOut.Enabled
                                         = btnClearSelection.Enabled
                                         = btnPan.Enabled
                                         = btnSelect.Enabled
                                         = btnSelectPolygon.Enabled
                                         = btnSelectCircle.Enabled
                                         = btnRefresh.Enabled = !busy;
            }
            else if (e.PropertyName == "ActiveTool" || e.PropertyName == "DigitizingType") //NOXLATE
            {
                UpdateButtonCheckedState();
            }
        }

        private void UpdateButtonCheckedState()
        {
            var at = (mgMapViewer1 == null) ? MapActiveTool.None : mgMapViewer1.ActiveTool;
            btnPan.BackColor = (at == MapActiveTool.Pan) ? Color.LemonChiffon : Control.DefaultBackColor;
            btnSelect.BackColor = (at == MapActiveTool.Select) ? Color.LemonChiffon : Control.DefaultBackColor;
            btnZoomIn.BackColor = (at == MapActiveTool.ZoomIn) ? Color.LemonChiffon : Control.DefaultBackColor;
            btnZoomOut.BackColor = (at == MapActiveTool.ZoomOut) ? Color.LemonChiffon : Control.DefaultBackColor;
        }

        private void btnPan_Click(object sender, EventArgs e)
        {
            mgMapViewer1.ActiveTool = MapActiveTool.Pan;
        }
    }
}
