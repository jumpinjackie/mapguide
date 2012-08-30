using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide.Viewer.AppLayoutEngine;
using OSGeo.MapGuide;
using OSGeo.MapGuide.Viewer.Desktop;

namespace SampleExtension
{
    public partial class SamplesTaskPane : MgControlView
    {
        private IMapViewer _viewer;

        public SamplesTaskPane(IMapViewer viewer)
        {
            InitializeComponent();
            _viewer = viewer;
            this.Title = "mg-desktop API Samples";
        }

        private void OnActionClick(object sender, EventArgs e)
        {
            string componentId = ((Button)sender).Tag.ToString();
            IShell shell = Shell.Instance;
            MgComponent component = shell.GetComponent(componentId);
            if (component != null)
                component.Invoke();
            else
                MessageBox.Show("ERROR: Component (" + componentId + ") not found in AppLayout");
        }

        private void btnReload_Click(object sender, EventArgs e)
        {
            _viewer.ClearSelection();
            MgResourceIdentifier mdfId = new MgResourceIdentifier("Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition");
            MgdMap map = new MgdMap(mdfId);
            MgDesktopMapViewerProvider provider = new MgDesktopMapViewerProvider(map);
            Shell.Instance.ReloadViewer(provider);
        }
    }
}
