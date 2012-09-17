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
    internal partial class MgTaskPaneStub : MgControlView
    {
        public MgTaskPaneStub()
        {
            InitializeComponent();
            this.Title = Strings.TitleTaskPaneAbout;
        }
    }
}
