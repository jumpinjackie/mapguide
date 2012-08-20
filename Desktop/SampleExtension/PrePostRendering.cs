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
    public partial class PrePostRendering : MgControlView
    {
        private IMapViewer _viewer;

        private bool _bPreRender;
        private bool _bPostRender;

        public PrePostRendering(IMapViewer viewer)
        {
            InitializeComponent();
            _viewer = viewer;
            _viewer.PreMapRender += new PaintEventHandler(OnPreMapRender);
            _viewer.PostMapRender += new PaintEventHandler(OnPostMapRender);
            this.Title = "Pre/Post Rendering";
        }

        void OnPostMapRender(object sender, PaintEventArgs e)
        {
            if (_bPostRender)
            {
                string str = "Post-render text";
                SizeF size = e.Graphics.MeasureString(str, SystemFonts.DialogFont);
                e.Graphics.DrawString(str, SystemFonts.DialogFont, Brushes.Green, new PointF(0, _viewer.ControlHeight - size.Height));
                txtMessages.AppendText("Invoked post-map render handler" + Environment.NewLine);
            }
        }

        void OnPreMapRender(object sender, PaintEventArgs e)
        {
            txtMessages.Clear();
            if (_bPreRender)
            {
                e.Graphics.DrawString("Pre-render text", SystemFonts.DialogFont, Brushes.Red, new PointF(0, 0));
                txtMessages.AppendText("Invoked pre-map render handler" + Environment.NewLine);
            }
        }

        protected override void SubCleanup()
        {
            base.SubCleanup();
        }

        private void chkEnablePreRender_CheckedChanged(object sender, EventArgs e)
        {
            _bPreRender = chkEnablePreRender.Checked;
            _viewer.RefreshMap(); //IMapViewer currently does not expose a control invalidation mechanism, so a RefreshMap() needs to be done
        }

        private void chkEnablePostRender_CheckedChanged(object sender, EventArgs e)
        {
            _bPostRender = chkEnablePostRender.Checked;
            _viewer.RefreshMap(); //IMapViewer currently does not expose a control invalidation mechanism, so a RefreshMap() needs to be done
        }
    }
}
