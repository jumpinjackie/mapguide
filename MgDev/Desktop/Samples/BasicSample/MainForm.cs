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

namespace BasicSample
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
    }
}
