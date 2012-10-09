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

namespace MapViewerTest
{
    public partial class CompactViewer : Form
    {
        public CompactViewer()
        {
            InitializeComponent();
            new MapViewerController(viewer, null, null, null, toolbar);
        }

        public void LoadMap(MgResourceIdentifier resId)
        {
            var map = new MgdMap(resId);
            viewer.Init(new MgDesktopMapViewerProvider(map));
            //viewer.RefreshMap();
        }
    }
}
