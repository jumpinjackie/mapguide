using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace OSGeo.MapGuide.Viewer.Tasks
{
    [ToolboxItem(false)]
    internal partial class MgTaskPaneStub : MgControlImpl
    {
        public MgTaskPaneStub()
        {
            InitializeComponent();
            this.Title = "About the Task Pane";
        }
    }
}
