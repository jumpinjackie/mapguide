﻿using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing;
using System.Diagnostics;
using System.Reflection;

namespace OSGeo.MapGuide.Viewer
{
    public interface IMapComponent
    {
        IEnumerable<PropertyInfo> ComponentProperties { get; }

        void SetPropertyValue(string propertyName, object value);

        object GetPropertyValue(string propertyName);
    }

    /// <summary>
    /// Indicates that a given CLR property is dynamically invokable
    /// </summary>
    [AttributeUsage(AttributeTargets.Property)]
    public class MgComponentPropertyAttribute : Attribute
    {

    }

    /// <summary>
    /// The base class of all viewer components. This is analogous to a command in the MapGuide
    /// AJAX viewer and a widget in the Fusion viewer.
    /// </summary>
    [ToolboxItem(false)]
    public class MgComponent : Component, IMapComponent
    {
        private IMapViewer _viewer;

        /// <summary>
        /// Gets whether to disable invocation entry points (eg. buttons, menus, etc) to this component when the
        /// viewer is digitizing
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("Disables this component while the digitizing is happening")]
        [MgComponentProperty]
        public virtual bool DisableWhenDigitizing { get { return true; } }

        /// <summary>
        /// Gets whether to disable invocation entry points (eg. buttons, menus, etc) to this component when the
        /// viewer is busy
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("Disables this component while the map is loading")]
        [MgComponentProperty]
        public virtual bool DisableWhenMapIsLoading { get { return true; } }

        /// <summary>
        /// Gets or sets the description of this component
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The description of this component")]
        [MgComponentProperty]
        public string Label { get; set; }

        /// <summary>
        /// Gets or sets the tooltip text of this component
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The tooltip text of this component")]
        [MgComponentProperty]
        public string ToolTipText { get; set; }

        /// <summary>
        /// The icon for this component
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The icon for this component")]
        [MgComponentProperty]
        public Image Icon { get; set; }

        /// <summary>
        /// Gets or sets the view instance to subscribe events to
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The viewer instance to subscribe events to")]
        [MgComponentProperty]
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

        protected virtual void SubscribeViewerEvents(IMapViewer viewer)
        {
            if (viewer != null)
            {
                viewer.PropertyChanged += OnViewerPropertyChanged;
                viewer.MapLoaded += OnViewerMapLoaded;
                viewer.MapRefreshed += OnMapRefreshed;

                if (this.RequiresLoadedMap)
                {
                    foreach (var l in _listeners)
                        l.SetEnabled(viewer.HasLoadedMap);
                }
            }
        }

        /// <summary>
        /// Unsubscribes events from the specified viewer instance
        /// </summary>
        /// <param name="viewer"></param>
        protected virtual void UnsubscribeViewerEvents(IMapViewer viewer)
        {
            if (viewer != null)
            {
                viewer.PropertyChanged -= OnViewerPropertyChanged;
                viewer.MapLoaded -= OnViewerMapLoaded;
            }
        }

        protected virtual void OnViewerMapLoaded(object sender, EventArgs e)
        {
            foreach (var l in _listeners)
                l.SetEnabled(this.Viewer.HasLoadedMap);
        }

        protected virtual void OnMapRefreshed(object sender, EventArgs e) { }

        protected virtual void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsBusy")
            {
                var busy = this.Viewer.IsBusy;
                //Trace.TraceInformation("Dispatching busy state event to " + _listeners.Count + " listeners");
                if (this.DisableWhenMapIsLoading)
                {
                    foreach (var l in _listeners)
                        l.SetEnabled(!busy);
                }
            }
            else if (e.PropertyName == "DigitizingType")
            {
                var bDigitizing = (this.Viewer.DigitizingType != MapDigitizationType.None);
                if (this.DisableWhenDigitizing)
                {
                    foreach (var l in _listeners)
                        l.SetEnabled(!bDigitizing);
                }
            }
        }
        
        /// <summary>
        /// Gets whether this component requires a loaded map. If true, and no map is loaded this
        /// component will be disabled until a map is loaded. If false, this component stays enabled
        /// and can operate without a loaded map
        /// </summary>
        protected virtual bool RequiresLoadedMap { get { return true; } }

        protected List<IButtonStateListener> _listeners = new List<IButtonStateListener>();

        public void AddListener(IButtonStateListener listener)
        {
            _listeners.Add(listener);
            if (this.RequiresLoadedMap)
                listener.SetEnabled(this.Viewer != null && this.Viewer.HasLoadedMap);
        }

        public void RemoveListener(IButtonStateListener listener)
        {
            _listeners.Remove(listener);
        }

        /// <summary>
        /// Invokes this component
        /// </summary>
        public virtual void Invoke() 
        {
            
        }

        private Dictionary<string, PropertyInfo> _properties;

        public IEnumerable<PropertyInfo> ComponentProperties
        {
            get 
            {
                CheckAndInitProperties();
                return _properties.Values;
            }
        }

        private void CheckAndInitProperties()
        {
            if (_properties == null)
            {
                _properties = new Dictionary<string, PropertyInfo>();
                var props = this.GetType().GetProperties();
                foreach (var p in props)
                {
                    var attributes = p.GetCustomAttributes(true);
                    foreach (var att in attributes)
                    {
                        var compAttr = att as MgComponentPropertyAttribute;
                        if (compAttr != null)
                        {
                            _properties[p.Name] = p;
                            break;
                        }
                    }
                }
            }
        }

        public void SetPropertyValue(string propertyName, object value)
        {
            CheckAndInitProperties();
            if (!_properties.ContainsKey(propertyName))
                throw new InvalidOperationException("Property " + propertyName + " is not a valid component property as it has not been marked with the MgComponentProperty attribute");

            _properties[propertyName].SetValue(this, value, null);
        }

        public object GetPropertyValue(string propertyName)
        {
            CheckAndInitProperties();
            if (!_properties.ContainsKey(propertyName))
                throw new InvalidOperationException("Property " + propertyName + " is not a valid component property as it has not been marked with the MgComponentProperty attribute");

            return _properties[propertyName].GetValue(propertyName, null);
        }
    }

    public delegate void ViewerBusyStateEventHandler(bool busy);

    public interface IButtonStateListener
    {
        void SetEnabled(bool enabled);
        void SetActive(bool outlined);
        void SetText(string text);
        void SetIcon(Image icon);
    }

    /// <summary>
    /// Indicates the target a UI-based component should display its UI view in
    /// </summary>
    public enum MgViewerTarget
    {
        /// <summary>
        /// Display the UI view within the specified parent container
        /// </summary>
        TaskPane,
        /// <summary>
        /// Display the UI view within a new window
        /// </summary>
        NewWindow
    }

    /// <summary>
    /// The base class of all UI-based components
    /// </summary>
    [ToolboxItem(false)]
    public class MgViewerComponent : MgComponent, ISupportInitialize
    {
        protected MgViewerComponent()
        {
            this.TaskPane = null;
            this.Target = MgViewerTarget.NewWindow;
        }

        /// <summary>
        /// 
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The task pane which will host the UI view")]
        [MgComponentProperty]
        public MgTaskPane TaskPane
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
        [MgComponentProperty]
        public MgViewerTarget Target
        {
            get { return _target; }
            set
            {
                //Must have a task pane assigned, but we suppress this check during initialization
                if (!_init && (value == MgViewerTarget.TaskPane && this.TaskPane == null))
                    throw new ArgumentException("Target: No task pane assigned");
                _target = value;
            }
        }

        /// <summary>
        /// Gets or sets the owner form that any new windows displayed by this component will belong to.
        /// </summary>
        [Category("MapGuide Component Properties")]
        [Description("The parent form which will be the owner for any new windows displayed by this component")]
        public Form OwnerParent
        {
            get;
            set;
        }

        /// <summary>
        /// Creates the associated view. Must be overridden by subclasses
        /// </summary>
        /// <returns></returns>
        protected virtual MgControlView CreateControlView() { throw new NotImplementedException(); }

        internal MgControlView CreateControl() { return CreateControlView(); }

        public override void Invoke()
        {
            var control = CreateControlView();
            control.Dock = DockStyle.Fill;
            if (this.Target == MgViewerTarget.TaskPane)
            {
                new TaskPaneContentCloser(this.TaskPane, control);
            }
            else //New Window
            {
                new NewWindowContentCloser(control, this.OwnerParent);
            }
        }

        class TaskPaneContentCloser : IContentCloser
        {
            private MgTaskPane _taskPane;
            private MgControlView _control;

            public TaskPaneContentCloser(MgTaskPane taskPane, MgControlView control)
            {
                _taskPane = taskPane;
                _control = control;

                _control.Closer = this;
                _taskPane.SetContent(_control);
            }

            public void Close()
            {
                _control.Dispose();
                _taskPane.LoadInitialTask();
            }
        }

        class NewWindowContentCloser : IContentCloser 
        {
            private Form _frm;
            private MgControlView _control;

            public NewWindowContentCloser(MgControlView control, Form owner)
            {
                _frm = new Form();
                _control = control;
                Rectangle screenRectangle = _frm.RectangleToScreen(_frm.ClientRectangle);
                int titleHeight = screenRectangle.Top - _frm.Top;
                _frm.Width = _control.PreferredSize.Width;
                _frm.Height = _control.PreferredSize.Height + titleHeight + 10; //HACK: height calculation is imperfect, so pad out
                _frm.Text = _control.Title;
                _frm.Controls.Add(_control);
                _control.Closer = this;

                if (control.ModalWindow)
                    _frm.ShowDialog(owner);
                else
                    _frm.Show(owner);
            }

            public void Close()
            {
                if (_control.ModalWindow)
                    _frm.DialogResult = DialogResult.OK;
                else
                    _frm.Close();

                if (!_control.IsDisposed)
                    _control.Dispose();
            }
        }

        private bool _init = true;

        public void BeginInit()
        {
            _init = true;
        }

        public void EndInit()
        {
            _init = false;
            //Now it's safe to check that task pane must be assigned if target is task pane
            if (this.TaskPane == null && this.Target == MgViewerTarget.TaskPane)
                throw new InvalidOperationException("EndInit: No task pane assigned");
        }
    }
}
