using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide;

namespace MapViewerTest
{
    public partial class MgAppWindow : Form, IMapStatusBar
    {
        public MgAppWindow()
        {
            InitializeComponent();
            taskMenu.DropDownDirection = ToolStripDropDownDirection.BelowLeft;
            if (!this.DesignMode)
            {
                new MapViewerController(mapViewer, legendCtrl, this, propertiesCtrl);
            }
        }

        public string MapDefinition
        {
            get;
            set;
        }

        public void LoadMap(MgMapViewerProvider provider)
        {
            mapViewer.Init(provider);
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
        }

        public void SetCursorPositionMessage(string message)
        {
            lblPosition.Text = message;
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

        private void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsBusy")
            {
                lblLoading.Visible = mapViewer.IsBusy;
            }
        }

        private void btnInitialTask_Click(object sender, EventArgs e)
        {
            taskPane.LoadInitialTask();
        }

        private void quitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void plotToDwfComponent_Invoked(object sender, EventArgs e)
        {
            var diag = new ResourceIdDialog();
            if (diag.ShowDialog() == DialogResult.OK)
            {
                var layoutId = diag.ResourceID;
                using (var save = new SaveFileDialog())
                {
                    save.Filter = "DWF Files (*.dwf)|*.dwf";
                    if (save.ShowDialog() == DialogResult.OK)
                    {
                        var mappingSvc = (MgdMappingService)mapViewer.GetProvider().CreateService(MgServiceType.MappingService);
                        var map = (MgdMap)mapViewer.GetMap();
                        var dwfVer = new MgdDwfVersion("6.01", "1.2");

                        var layout = new MgdLayout(layoutId, "TestPlot", MgdPageUnitsType.Inches);
                        var plotSpec = new MgdPlotSpecification(8.5f, 11.0f, MgdPageUnitsType.Inches, 0.5f, 0.5f, 0.5f, 0.5f);

                        var result = mappingSvc.GeneratePlot(map, plotSpec, layout, dwfVer);
                        var sink = new MgByteSink(result);
                        sink.ToFile(save.FileName);

                        MessageBox.Show("Saved to " + save.FileName);
                    }
                }
            }
        }

        private void mgLayerSelectionHandler1_SelectionMade(MgSelectionSet selectedFeatures)
        {
            if (Array.IndexOf(selectedFeatures.LayerNames, "Parcels") >= 0)
            {
                MgFeature[] features = selectedFeatures.GetFeaturesForLayer("Parcels");
                MessageBox.Show(features.Length + " parcels selected");
            }
        }

        private void loadCompactViewerComponent_Invoked(object sender, EventArgs e)
        {
            var map = mapViewer.GetMap();
            var viewer = new CompactViewer();
            viewer.LoadMap(map.MapDefinition);
            viewer.Show(this); //or viewer.Show(null) works too. You just have to call the overload of Show() that takes a owner parameter
        }

        private void profileComponent_Invoked(object sender, EventArgs e)
        {
            var provider = mapViewer.GetProvider();
            var map = mapViewer.GetMap();
            var prof = (MgdProfilingService)provider.CreateService(MgServiceType.ProfilingService);
            var opts = new MgdRenderingOptions("PNG", 2, new MgColor(mapViewer.SelectionColor));
            var result = prof.ProfileRenderDynamicOverlay((MgdMap)map, (MgdSelection)mapViewer.GetSelection(), opts); 
 	        new XmlResponseDialog(result).ShowDialog(); 
        }

        private void pointToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mapViewer.DigitizePoint((x, y) => { MessageBox.Show("Done"); }, "Custom point digitization prompt");
        }

        private void lineToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mapViewer.DigitizeLine((x1, y1, x2, y2) => { MessageBox.Show("Done"); }, "Custom line digitization prompt");
        }

        private void rectangleToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mapViewer.DigitizeRectangle((x1, y1, x2, y2) => { MessageBox.Show("Done"); }, "Custom rectangle digitization prompt");
        }

        private void lineStringToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mapViewer.DigitizeLineString((coords) => { MessageBox.Show("Done"); }, "Custom line string digitization prompt");
        }

        private void polygonToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mapViewer.DigitizePolygon((coords) => { MessageBox.Show("Done"); }, "Custom polygon digitization prompt");
        }

        private void circleToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mapViewer.DigitizeCircle((x, y, r) => { MessageBox.Show("Done"); }, "Custom circle digitization prompt");
        }

        private void oneshotCallbackToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MgGeometryFactory geomFact = new MgGeometryFactory();
            mapViewer.DigitizePoint((x, y) => {
                MgCoordinate coord = geomFact.CreateCoordinateXY(x, y);
                MgPoint pt = geomFact.CreatePoint(coord);

                mapViewer.SelectByGeometry(pt, -1, (selection) => {
                    if (selection == null)
                    {
                        MessageBox.Show("No selected features");
                        return;
                    }
                    else
                    {
                        MgReadOnlyLayerCollection layers = selection.GetLayers();
                        if (layers != null)
                        {
                            StringBuilder sb = new StringBuilder("Selection summary:");
                            for (int i = 0; i < layers.GetCount(); i++)
                            {
                                MgLayerBase lyr = layers.GetItem(i);
                                sb.Append(Environment.NewLine + lyr.GetName() + ": " + selection.GetSelectedFeaturesCount(lyr, lyr.GetFeatureClassName()));
                            }
                            MessageBox.Show(sb.ToString());
                        }
                        else 
                        {
                            MessageBox.Show("No selected features");
                        }
                    }
                });
            });
        }
    }
}
