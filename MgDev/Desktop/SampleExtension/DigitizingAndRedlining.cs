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
    public partial class DigitizingAndRedlining : MgControlView
    {
        private IMapViewer _viewer;

        public DigitizingAndRedlining(IMapViewer viewer)
        {
            InitializeComponent();
            _viewer = viewer;
            this.Title = "Digitizing and Redlining";
        }
    }
}
