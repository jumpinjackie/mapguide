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
    /// A design-time invoker that invokes the target MgComponent when the source
    /// UI component is clicked
    /// </summary>
    /// <typeparam name="T"></typeparam>
    [ToolboxItem(false)]
    public class ComponentInvokerBase<T> : Component where T : ToolStripItem
    {
        /// <summary>
        /// The set of bindings
        /// </summary>
        protected Dictionary<T, MgComponent> _bindings;

        /// <summary>
        /// Constructor
        /// </summary>
        protected ComponentInvokerBase() { _bindings = new Dictionary<T, MgComponent>(); }

        /// <summary>
        /// Gets the target bound component to invoke when this item is clicked
        /// </summary>
        /// <param name="component"></param>
        /// <returns></returns>
        [Category("MapGuide Viewer")] //NOXLATE
        [DisplayName("Target Component")] //NOXLATE
        [Description("The target MapGuide component to invoke when this item is clicked")] //NOXLATE
        [DefaultValue(null)]
        public MgComponent GetTargetComponent(T component)
        {
            if (_bindings.ContainsKey(component))
                return _bindings[component];
            else
                return null;
        }

        /// <summary>
        /// Clears all registered bindings
        /// </summary>
        public void Clear()
        {
            foreach (var key in _bindings.Keys)
            {
                TeardownComponent(key, _bindings[key]);
            }
            _bindings.Clear();
        }

        void OnComponentClicked(object sender, EventArgs e)
        {
            var item = sender as T;
            var binding = item.Tag as ComponentBinding;
            if (binding != null)
                binding.Component.Invoke();
        }

        /// <summary>
        /// Performs component setup
        /// </summary>
        /// <param name="component"></param>
        /// <param name="value"></param>
        protected virtual void SetupComponent(T component, MgComponent value)
        {
            component.Click += OnComponentClicked;
        }

        /// <summary>
        /// Performs component teardown
        /// </summary>
        /// <param name="component"></param>
        /// <param name="value"></param>
        protected virtual void TeardownComponent(T component, MgComponent value)
        {
            component.Click -= OnComponentClicked;
        }

        /// <summary>
        /// Associates the target component with teh source UI element
        /// </summary>
        /// <param name="component"></param>
        /// <param name="value"></param>
        public void SetTargetComponent(T component, MgComponent value)
        {
            if (value == null)
            {
                //Un-bind existing component if it exists, otherwise nothing needs to be done
                if (_bindings.ContainsKey(component))
                {
                    TeardownComponent(component, _bindings[component]);
                    _bindings.Remove(component);
                }
            }
            else
            {
                if (_bindings.ContainsKey(component))
                {
                    TeardownComponent(component, _bindings[component]);
                    _bindings[component] = value;
                    SetupComponent(component, value);
                }
                else
                {
                    _bindings[component] = value;
                    SetupComponent(component, value);
                }
            }
        }
    }

    abstract class ComponentBinding : IButtonStateListener 
    {
        public MgComponent Component { get; protected set; }

        protected ComponentBinding(MgComponent component) { this.Component = component; }

        public abstract void SetEnabled(bool enabled);

        public abstract void SetActive(bool outlined);

        public abstract void SetText(string text);

        public abstract void SetIcon(Image icon);
    }

    class ToolbarButtonComponentBinding : ComponentBinding
    {
        private ToolStripButton _item;

        public ToolbarButtonComponentBinding(ToolStripButton item, MgComponent component) 
            : base(component)
        {
            _item = item;
        }

        public override void SetEnabled(bool enabled)
        {
            _item.Enabled = enabled;
        }

        public override void SetActive(bool outlined)
        {
            _item.Checked = outlined;
        }

        public override void SetText(string text)
        {
            _item.Text = text;
        }

        public override void SetIcon(Image icon)
        {
            _item.Image = icon;
        }
    }

    class MenuItemComponentBinding : ComponentBinding
    {
        private ToolStripMenuItem _item;

        public MenuItemComponentBinding(ToolStripMenuItem item, MgComponent component) 
            : base(component)
        { 
            _item = item;
        }

        public override void SetEnabled(bool enabled)
        {
            _item.Enabled = enabled;
        }

        public override void SetActive(bool outlined)
        {
            _item.Checked = outlined;
        }

        public override void SetText(string text)
        {
            _item.Text = text;
        }

        public override void SetIcon(Image icon)
        {
            _item.Image = icon;
        }
    }

    /// <summary>
    /// An design-time invoker that ties a <see cref="T:System.Windows.Forms.ToolStripMenuItem"/> to a
    /// <see cref="T:OSGeo.MapGuide.Viewer.MgComponent"/>
    /// </summary>
    [ToolboxItem(true)]
    [ProvideProperty("TargetComponent", typeof(ToolStripMenuItem))] //NOXLATE
    public class MgMenuItemComponentInvoker : ComponentInvokerBase<ToolStripMenuItem>, IExtenderProvider
    {
        /// <summary>
        /// Specifies whether this object can provide its extender properties to the specified object.
        /// </summary>
        /// <param name="extendee"></param>
        /// <returns></returns>
        public bool CanExtend(object extendee)
        {
            return typeof(ToolStripMenuItem) == extendee.GetType();
        }

        /// <summary>
        /// Performs component setup
        /// </summary>
        /// <param name="component"></param>
        /// <param name="value"></param>
        protected override void SetupComponent(ToolStripMenuItem component, MgComponent value)
        {
            component.ToolTipText = value.ToolTipText;
            component.Text = value.Label;
            component.Image = value.Icon;
            var listener = new MenuItemComponentBinding(component, value);
            component.Tag = listener;
            value.AddListener(listener);
            base.SetupComponent(component, value);
        }

        /// <summary>
        /// Performs component teardown
        /// </summary>
        /// <param name="component"></param>
        /// <param name="value"></param>
        protected override void TeardownComponent(ToolStripMenuItem component, MgComponent value)
        {
            var listener = component.Tag as MenuItemComponentBinding;
            if (listener != null)
                value.RemoveListener(listener);
            base.TeardownComponent(component, value);
        }
    }

    /// <summary>
    /// An design-time invoker that ties a <see cref="T:System.Windows.Forms.ToolStripButton"/> to a
    /// <see cref="T:OSGeo.MapGuide.Viewer.MgComponent"/>
    /// </summary>
    [ToolboxItem(true)]
    [ProvideProperty("TargetComponent", typeof(ToolStripButton))] //NOXLATE
    public class MgToolButtonComponentInvoker : ComponentInvokerBase<ToolStripButton>, IExtenderProvider
    {
        /// <summary>
        /// Specifies whether this object can provide its extender properties to the specified object.
        /// </summary>
        /// <param name="extendee"></param>
        /// <returns></returns>
        public bool CanExtend(object extendee)
        {
            return typeof(ToolStripButton) == extendee.GetType();
        }

        /// <summary>
        /// Performs component setup
        /// </summary>
        /// <param name="component"></param>
        /// <param name="value"></param>
        protected override void SetupComponent(ToolStripButton component, MgComponent value)
        {
            component.ToolTipText = value.ToolTipText;
            component.Text = value.Label;
            component.Image = value.Icon;
            var listener = new ToolbarButtonComponentBinding(component, value);
            component.Tag = listener;
            value.AddListener(listener);
            base.SetupComponent(component, value);
        }

        /// <summary>
        /// Performs component teardown
        /// </summary>
        /// <param name="component"></param>
        /// <param name="value"></param>
        protected override void TeardownComponent(ToolStripButton component, MgComponent value)
        {
            var listener = component.Tag as ToolbarButtonComponentBinding;
            if (listener != null)
                value.RemoveListener(listener);
            base.TeardownComponent(component, value);
        }
    }
}
