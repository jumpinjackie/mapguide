using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide;

namespace SampleExtension
{
    public partial class HelloViewer : MgControlView
    {
        private IMapViewer _viewer;

        public HelloViewer(IMapViewer viewer)
        {
            InitializeComponent();
            this.Title = "Hello Viewer";
            _viewer = viewer;

            _viewer.PropertyChanged += OnViewerPropertyChanged;
            _viewer.SelectionChanged += OnViewerSelectionChanged;
            CheckSelectionState();
            CheckBusyState();
            CheckActiveTool();
        }

        protected override void SubCleanup()
        {
            //When subscribing to events, be a responsible user and 
            //unsubscribe on cleanup
            _viewer.PropertyChanged -= OnViewerPropertyChanged;
            _viewer.SelectionChanged -= OnViewerSelectionChanged;
        }

        void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsBusy")
                CheckBusyState();
            else if (e.PropertyName == "ActiveTool")
                CheckActiveTool();
        }

        void OnViewerSelectionChanged(object sender, EventArgs e)
        {
            CheckSelectionState();
        }

        private void CheckActiveTool()
        {
            lblActiveTool.Text = "Active Tool: " + _viewer.ActiveTool.ToString();
        }

        private void CheckBusyState()
        {
            lblViewerBusyState.Text = "Viewer " + ((_viewer.IsBusy) ? "is busy" : "is not busy");
        }

        private void CheckSelectionState()
        {
            MgSelectionBase selection = _viewer.GetSelection();
            MgReadOnlyLayerCollection layers = selection.GetLayers();
            if (layers != null)
                btnClearSelection.Enabled = true;
            else
                btnClearSelection.Enabled = false;
        }

        private void btnZoomToLocation_Click(object sender, EventArgs e)
        {
            _viewer.ZoomToView(-87.732492, 43.743934, 5000, true);
        }

        private void btnClearSelection_Click(object sender, EventArgs e)
        {
            _viewer.ClearSelection();
        }

        private void btnInitialView_Click(object sender, EventArgs e)
        {
            _viewer.InitialMapView();
        }

        private void btnCopyMap_Click(object sender, EventArgs e)
        {
            _viewer.CopyMap();
            MessageBox.Show("Map image (current view) copied to clipboard");
        }
    }
}
