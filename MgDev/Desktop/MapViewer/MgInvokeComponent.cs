using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms.Design;
using System.Diagnostics;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A specialized instance of <see cref="T:System.Windows.Forms.ToolStripButton"/> that can
    /// invoke the assigned <see cref="T:OSGeo.MapGuide.Viewer.MgComponent"/> instance.
    /// </summary>
    [ToolStripItemDesignerAvailability(ToolStripItemDesignerAvailability.ToolStrip)]
    public class MgInvokeComponentButton : ToolStripButton, IMapViewerBusyStateListener
    {
        private MgComponent _component;

        /// <summary>
        /// Gets or sets the target component to be invoked
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The target component to invoke when this component is clicked. All other UI properties are inherited from this compoent")]
        public MgComponent TargetComponent
        {
            get { return _component; }
            set 
            {
                if (_component != null)
                    _component.RemoveListener(this);
                _component = value;
                if (_component != null)
                {
                    base.ToolTipText = _component.ToolTipText;
                    base.Text = _component.Label;
                    base.Image = _component.Icon;
                    _component.AddListener(this);
                }
            }
        }

        protected override void OnClick(EventArgs e)
        {
            if (this.TargetComponent == null)
                throw new InvalidOperationException("No target component assigned to this component");

            this.TargetComponent.Invoke();

            base.OnClick(e);
        }

        public void SetBusy(bool busy)
        {
            this.Enabled = !busy;
        }
    }

    /// <summary>
    /// A specialized instance of <see cref="T:System.Windows.Forms.ToolStripMenuItem"/> that can
    /// invoke the assigned <see cref="T:OSGeo.MapGuide.Viewer.MgComponent"/> instance.
    /// </summary>
    [ToolStripItemDesignerAvailability(ToolStripItemDesignerAvailability.ContextMenuStrip | ToolStripItemDesignerAvailability.MenuStrip)]
    public class MgInvokeComponentMenuStripItem : ToolStripMenuItem, IMapViewerBusyStateListener
    {
        private MgComponent _component;

        /// <summary>
        /// Gets or sets the target component to be invoked
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The target component to invoke when this component is clicked. All other UI properties are inherited from this compoent")]
        public MgComponent TargetComponent
        {
            get { return _component; }
            set
            {
                if (_component != null)
                    _component.RemoveListener(this);
                _component = value;
                if (_component != null)
                {
                    base.ToolTipText = _component.ToolTipText;
                    base.Text = _component.Label;
                    base.Image = _component.Icon;
                    _component.AddListener(this);
                }
            }
        }

        protected override void OnClick(EventArgs e)
        {
            if (this.TargetComponent == null)
                throw new InvalidOperationException("No target component assigned to this component");

            this.TargetComponent.Invoke();

            base.OnClick(e);
        }

        public void SetBusy(bool busy)
        {
            this.Enabled = !busy;
        }
    }
}
