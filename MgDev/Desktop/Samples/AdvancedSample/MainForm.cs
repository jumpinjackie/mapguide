using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide;
using OSGeo.MapGuide.Viewer.Desktop;
using System.IO;

namespace AdvancedSample
{
    public partial class MainForm : Form, IMapStatusBar
    {
        public MainForm()
        {
            InitializeComponent();
        }

        protected override void OnLoad(EventArgs e)
        {
            new MapViewerController(mgMapViewer1,          //The MgMapViewer
                                    mgLegend1,             //The MgLegend
                                    this,                  //The IMapStatusBar
                                    mgPropertyPane1,       //The MgPropertyPane
                                    mgDefaultToolbar1);    //The MgDefaultToolbar

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
        }

        public void SetCursorPositionMessage(string message)
        {
            lblCoordinates.Text = message;
        }

        public void SetFeatureSelectedMessage(string message)
        {
            lblMessage.Text = message;
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

        private void btnMeasure_Click(object sender, EventArgs e)
        {
            mgMapViewer1.DigitizeLine(OnLineDigitized);
        }

        private void OnLineDigitized(double x1, double y1, double x2, double y2)
        {
            MgMapBase map = mgMapViewer1.GetMap();
            //Create a coordiante system from the map's SRS
            MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
            MgCoordinateSystem mapCs = csFactory.Create(map.GetMapSRS());

            //Invoke the appropriate measure method depending on the type
            //of coordinate system
            double dist = 0.0;
            if (mapCs.GetType() == MgCoordinateSystemType.Geographic)
                dist = mapCs.MeasureGreatCircleDistance(x1, y1, x2, y2);
            else
                dist = mapCs.MeasureEuclideanDistance(x1, y1, x2, y2);

            //Convert this distance to meters
            dist = mapCs.ConvertCoordinateSystemUnitsToMeters(dist);

            MessageBox.Show("Distance is: " + dist + " meters");
        }

        private MgdLayer _pointLayer;

        private void btnDrawPoint_Click(object sender, EventArgs e)
        {
            mgMapViewer1.DigitizePoint(OnPointDrawn);
        }

        private void OnPointDrawn(double x, double y)
        {
            if (_pointLayer == null) //Our point layer doesn't exist
                CreateRedlineLayer();
            
            //Now insert our point. This code should look familiar
            //to you, setting up the MgPropertyCollection for insertion
            MgPropertyCollection props = new MgPropertyCollection();
            MgWktReaderWriter wktRw = new MgWktReaderWriter();
            MgAgfReaderWriter agfRw = new MgAgfReaderWriter();

            MgGeometry geom = wktRw.Read("POINT (" + x + " " + y + ")");
            MgByteReader agf = agfRw.Write(geom);

            MgGeometryProperty geomProp = new MgGeometryProperty("Geometry", agf);
            props.Add(geomProp);

            //Here's where we differ from the official MapGuide API
            //instead of a monolithic UpdateFeatures() that tries to 
            //do everything, we have individual InsertFeatures/DeleteFeatures/UpdateFeatures
            //methods. So here's the mg-desktop way

            MgFeatureReader result = _pointLayer.InsertFeatures(props);
            result.Close();

            //Or if you have have access to the MgdLayer instance
            /*
            MgResourceIdentifier fsId = new MgResourceIdentifier(_pointLayer.GetFeatureSourceId());
            MgServiceFactory factory = new MgServiceFactory();
            MgdFeatureService featSvc = (MgdFeatureService)factory.CreateService(MgServiceType.FeatureService);
            MgFeatureReader fr = featSvc.InsertFeatures(fsId, "Default:Redline", props);
            fr.Close();
             */
            
            //Now refresh to see your newly drawn point
            mgMapViewer1.RefreshMap();
        }

        private void CreateRedlineLayer()
        {
            MgMapBase map = mgMapViewer1.GetMap();
            MgdServiceFactory fact = new MgdServiceFactory();
            MgdFeatureService featSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);
            MgResourceService resSvc = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);

            //Note that mg-desktop does not have a concept of sessions like the
            //official MapGuide API, but it *does* allow session-based resources 
            //as a way of having temporary resources. Such resources will reside
            //in a special directory for session resources (specified in Platform.ini)
            //
            //You can plug whatever string as the session id, but the resource identifier
            //must satisfy the session id pattern:
            //
            // Session:<session id string>//Path/To/Your.ResourceType
            //
            //These files are removed with MgPlatform.Terminate(), which is called in this
            //application as part of the exiting process.
            string sessionId = Guid.NewGuid().ToString();
            MgResourceIdentifier fsId = new MgResourceIdentifier("Session:" + sessionId + "//Redline.FeatureSource");
            MgResourceIdentifier ldfId = new MgResourceIdentifier("Session:" + sessionId + "//Redline.LayerDefinition");

            //Create our point redline schema. It looks like this:
            //
            // Default
            //    Redline
            //        ID (int32, autogenerated)
            //        Geometry (coordinate system same as map
            string featureClass = "Default:Redline";
            string geometry = "Geometry";

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

            //Create the feature source with this schema. We use the map's
            //coordinate system for the feature source to ensure features
            //that we create, will line up with the map
            MgCreateSdfParams create = new MgCreateSdfParams("Default", map.GetMapSRS(), schema);
            featSvc.CreateFeatureSource(fsId, create);

            //Then create the layer definition. RedlineLayer.xml contains the template
            string xml = string.Format(File.ReadAllText("RedlineLayer.xml"), fsId.ToString(), featureClass, geometry);
            var bytes = Encoding.UTF8.GetBytes(xml);
            MgByteSource source = new MgByteSource(bytes, bytes.Length);
            resSvc.SetResource(ldfId, source.GetReader(), null);

            //Now create the runtime layer and add to map
            _pointLayer = new MgdLayer(ldfId, resSvc);
            _pointLayer.LegendLabel = "Redlining";
            _pointLayer.Name = "Redline";
            _pointLayer.Visible = true;
            _pointLayer.Selectable = true;
            _pointLayer.DisplayInLegend = true;

            var layers = map.GetLayers();
            layers.Insert(0, _pointLayer);
        }
    }
}
