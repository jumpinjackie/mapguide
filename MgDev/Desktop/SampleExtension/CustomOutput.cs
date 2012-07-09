using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;

namespace SampleExtension
{
    public partial class CustomOutput : MgControlView
    {
        private IMapViewer _viewer;

        public CustomOutput(IMapViewer viewer)
        {
            InitializeComponent();
            this.Title = "Custom Output";
            _viewer = viewer;
        }


    }
}
