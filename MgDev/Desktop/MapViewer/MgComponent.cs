using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing;
using System.Diagnostics;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// The base class of all viewer components. This is analogous to a command in the MapGuide
    /// AJAX viewer and a widget in the Fusion viewer.
    /// </summary>
    [ToolboxItem(false)]
    public class MgComponent : Component
    {
        private IMapViewer _viewer;

        /// <summary>
        /// Gets whether to disable invocation entry points (eg. buttons, menus, etc) to this component when the
        /// viewer is busy
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("Disables this component while the map is loading")]
        public virtual bool DisableWhenMapIsLoading { get { return true; } }

        /// <summary>
        /// Gets or sets the description of this component
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The description of this component")]
        public string Label { get; set; }

        /// <summary>
        /// Gets or sets the tooltip text of this component
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The tooltip text of this component")]
        public string ToolTipText { get; set; }

        /// <summary>
        /// The icon for this component
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The icon for this component")]
        public Image Icon { get; set; }

        /// <summary>
        /// Gets or sets the view instance to subscribe events to
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The viewer instance to subscribe events to")]
        public IMapViewer Viewer
        {
            get { return _viewer; }
            set
            {
                if (_viewer != null)
                    UnsubscribeViewerEvents(_viewer);
                _viewer = value;
                SubscribeViewerEvents(_viewer);
            }
        }

        /// <summary>
        /// Subscribes to viewer events
        /// </summary>
        /// <param name="viewer"></param>
        protected virtual void SubscribeViewerEvents(IMapViewer viewer)
        {
            if (viewer != null)
                viewer.PropertyChanged += OnViewerPropertyChanged;
        }

        public event ViewerBusyStateEventHandler ViewerBusyStateChanged;

        void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsBusy")
            {
                var busy = this.Viewer.IsBusy;
                Trace.TraceInformation("Dispatching busy state event to " + _listeners.Count + " listeners");
                foreach (var l in _listeners)
                    l.SetBusy(busy);
            }
        }

        /// <summary>
        /// Unsubscribes events from the specified viewer instance
        /// </summary>
        /// <param name="viewer"></param>
        protected virtual void UnsubscribeViewerEvents(IMapViewer viewer)
        {
            if (viewer != null)
                viewer.PropertyChanged -= OnViewerPropertyChanged;
        }

        private List<IMapViewerBusyStateListener> _listeners = new List<IMapViewerBusyStateListener>();

        public void AddListener(IMapViewerBusyStateListener listener)
        {
            _listeners.Add(listener);
        }

        public void RemoveListener(IMapViewerBusyStateListener listener)
        {
            _listeners.Remove(listener);
        }

        /// <summary>
        /// Invokes this component
        /// </summary>
        public virtual void Invoke() 
        {
            
        }
    }

    public delegate void ViewerBusyStateEventHandler(bool busy);

    public interface IMapViewerBusyStateListener
    {
        void SetBusy(bool busy);
    }

    /// <summary>
    /// Indicates the target a UI-based component should display its UI view in
    /// </summary>
    public enum MgViewerTarget
    {
        /// <summary>
        /// Display the UI view within the specified parent container
        /// </summary>
        ParentContainer,
        /// <summary>
        /// Display the UI view within a new window
        /// </summary>
        NewWindow
    }

    /// <summary>
    /// The base class of all UI-based components
    /// </summary>
    [ToolboxItem(false)]
    public class MgViewerComponent : MgComponent
    {
        protected MgViewerComponent()
        {
            this.ParentContainer = null;
            this.Target = MgViewerTarget.NewWindow;
        }

        /// <summary>
        /// 
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The control which will house this component")]
        public Control ParentContainer
        {
            get;
            set;
        }

        private MgViewerTarget _target;

        /// <summary>
        /// Gets or sets the target that this component should display its UI view in
        /// </summary>
        [Category("MapGuide Component Properties")]
        [DefaultValue(MgViewerTarget.NewWindow)]
        [Description("Target that this component should display its UI view in")]
        public MgViewerTarget Target
        {
            get { return _target; }
            set
            {
                if (value == MgViewerTarget.ParentContainer && this.ParentContainer == null)
                    throw new ArgumentException("Must specify a parent container");
                _target = value;
            }
        }

        /// <summary>
        /// Creates the associated view. Must be overridden by subclasses
        /// </summary>
        /// <returns></returns>
        protected virtual MgControlImpl CreateControlImpl() { throw new NotImplementedException(); }

        public override void Invoke()
        {
            var control = CreateControlImpl();
            control.Dock = DockStyle.Fill;
            if (this.Target == MgViewerTarget.ParentContainer)
            {
                this.ParentContainer.Controls.Clear();
                this.ParentContainer.Controls.Add(control);
            }
            else //New Window
            {
                var form = new Form();
                Rectangle screenRectangle = form.RectangleToScreen(form.ClientRectangle);
                int titleHeight = screenRectangle.Top - form.Top;
                form.Width = control.PreferredSize.Width;
                form.Height = control.PreferredSize.Height + titleHeight + 10; //HACK: height calculation is imperfect, so pad out
                form.Text = control.Title;
                form.Controls.Add(control);

                if (control.ModalWindow)
                    form.ShowDialog();
                else
                    form.Show();
            }
        }
    }
}
