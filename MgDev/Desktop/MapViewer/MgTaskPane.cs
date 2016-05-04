using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer.Tasks;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    public partial class MgTaskPane : UserControl
    {
        public MgTaskPane()
        {
            InitializeComponent();
        }

        public Control InitialTask
        {
            get;
            set;
        }

        public void LoadInitialTask()
        {
            this.SetContent(this.InitialTask);
        }

        internal void SetContent(Control c)
        {
            c.Dock = DockStyle.Top; //This allows for scrolling
            rootContainer.Controls.Clear();
            rootContainer.Controls.Add(c);
            var mgc = c as MgControlView;
            if (mgc != null)
                lblTitle.Text = mgc.Title;
            else
                lblTitle.Text = Strings.TextTaskPane;
        }

        protected override void OnLoad(EventArgs e)
        {
            if (this.InitialTask == null)
            {
                this.InitialTask = new MgTaskPaneStub();
            }
            LoadInitialTask();
            base.OnLoad(e);
        }

        private void btnInitialTask_Click(object sender, EventArgs e)
        {
            LoadInitialTask();
        }

        internal void SetInitialComponent(MgViewerComponent cmp)
        {
            if (cmp.Target != MgViewerTarget.TaskPane)
                throw new InvalidOperationException(Strings.ErrorComponentTargetNotTaskPane);
            this.InitialTask = cmp.CreateControl();
        }
    }
}
