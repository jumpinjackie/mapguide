﻿using System;
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
                        var renderSvc = (MgRenderingService)mapViewer.GetProvider().CreateService(MgServiceType.RenderingService);
                        var map = (MgdMap)mapViewer.GetMap();
                        var dwfVer = new MgDwfVersion("6.01", "1.2");

                        var layout = new MgLayout(layoutId, "TestPlot", MgPageUnitsType.Inches);
                        var plotSpec = new MgPlotSpecification(8.5f, 11.0f, MgPageUnitsType.Inches, 0.5f, 0.5f, 0.5f, 0.5f);

                        var result = renderSvc.GeneratePlot(map, plotSpec, layout, dwfVer);
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
            viewer.Show();
        }

        private void profileComponent_Invoked(object sender, EventArgs e)
        {
            var provider = mapViewer.GetProvider();
            var map = mapViewer.GetMap();
            var prof = (MgProfilingService)provider.CreateService(MgServiceType.ProfilingService);
            var opts = new MgRenderingOptions("PNG", 2, new MgColor(mapViewer.SelectionColor));
            var result = prof.ProfileRenderDynamicOverlay((MgdMap)map, (MgdSelection)mapViewer.GetSelection(), opts); 
 	        new XmlResponseDialog(result).ShowDialog(); 
        }
    }
}
