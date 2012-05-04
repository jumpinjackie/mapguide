using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms.Design;
using System.Diagnostics;
using System.ComponentModel.Design;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// Defines UI controls that support MapGuide Viewer component invocation
    /// </summary>
    public interface IInvokeViewerComponent : IComponent
    {
        /// <summary>
        /// Gets or sets the target component to be invoked
        /// </summary>
        MgComponent TargetComponent { get; set; }
    }

    /// <summary>
    /// A specialized instance of <see cref="T:System.Windows.Forms.ToolStripButton"/> that can
    /// invoke the assigned <see cref="T:OSGeo.MapGuide.Viewer.MgComponent"/> instance.
    /// </summary>
    [ToolStripItemDesignerAvailability(ToolStripItemDesignerAvailability.ToolStrip)]
    public class MgInvokeComponentButton : ToolStripButton, IButtonStateListener, IInvokeViewerComponent
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

        public void SetDisabled(bool busy)
        {
            this.Enabled = !busy;
        }

        public void SetActive(bool outlined)
        {
            this.Checked = outlined;
        }

        public void SetText(string text)
        {
            this.Text = text;
        }

        public void SetIcon(Image icon)
        {
            this.Image = icon;
        }
    }

    /// <summary>
    /// A specialized instance of <see cref="T:System.Windows.Forms.ToolStripMenuItem"/> that can
    /// invoke the assigned <see cref="T:OSGeo.MapGuide.Viewer.MgComponent"/> instance.
    /// </summary>
    [ToolStripItemDesignerAvailability(ToolStripItemDesignerAvailability.ContextMenuStrip | ToolStripItemDesignerAvailability.MenuStrip)]
    public class MgInvokeComponentMenuItem : ToolStripMenuItem, IButtonStateListener, IInvokeViewerComponent
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

        public void SetDisabled(bool busy)
        {
            this.Enabled = !busy;
        }

        public void SetActive(bool outlined)
        {
            this.Checked = outlined;
        }

        public void SetText(string text)
        {
            this.Text = text;
        }

        public void SetIcon(Image icon)
        {
            this.Image = icon;
        }
    }
}
