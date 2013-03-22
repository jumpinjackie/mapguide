using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using System.Globalization;

namespace OSGeo.MapGuide.Viewer.AppLayoutEngine
{
    /// <summary>
    /// The top-level window of an AppLayout application
    /// </summary>
    public partial class Shell : Form, IShell
    {
        private Shell()
        {
            InitializeComponent();
            UpdateLegendTabText();
        }

        private static Shell _instance;

        /// <summary>
        /// The shell instance
        /// </summary>
        public static IShell Instance
        {
            get
            {
                if (null == _instance)
                {
                    _instance = new Shell();
                }
                return _instance;
            }
        }

        /// <summary>
        /// Initializes this map viewer and constructs the user interface based on the
        /// given AppLayout document
        /// </summary>
        /// <param name="layout"></param>
        /// <param name="provider"></param>
        public void Initialize(AppLayout layout, MgMapViewerProvider provider)
        {
            this.Text = layout.Title;

            if (!string.IsNullOrEmpty(layout.Language))
            {
                try
                {
                    var ci = CultureInfo.GetCultureInfo(layout.Language);
                    System.Threading.Thread.CurrentThread.CurrentUICulture 
                        = System.Threading.Thread.CurrentThread.CurrentCulture
                        = ci;
                    SetLanguage(ci);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, Strings.SetLanguage);
                }
            }

            _menuInvoker = new MgMenuItemComponentInvoker();
            _toolInvoker = new MgToolButtonComponentInvoker();

            ValidateMapNames(layout);

            InitializeComponentSet(layout);
            InitializeMenu(layout.Menu);
            InitializeToolbar(layout.Toolbar);
            InitializeContextMenu(layout.ContextMenu);
            InitializeTaskPaneMenu(layout.TaskPane.TaskMenu);

            _layout = layout;
            if (!string.IsNullOrEmpty(_layout.Icon) && System.IO.File.Exists(_layout.Icon))
            {
                this.Icon = new System.Drawing.Icon(_layout.Icon);
            }

            SetInfoPaneWidth(_layout.InfoPane.Width);
            SetInfoPaneVisible(_layout.InfoPane.IsVisible);
            if (_layout.InfoPane.IsVisible)
            {
                SetLegendVisbility(_layout.InfoPane.Legend.Visible);
                legend.ShowTooltips = _layout.InfoPane.Legend.ShowTooltips;
                if (_layout.InfoPane.Legend.ThemeCompressionLimit.HasValue)
                    legend.ThemeCompressionLimit = _layout.InfoPane.Legend.ThemeCompressionLimit.Value;
                SetPropertyPaneVisbility(_layout.InfoPane.PropertyPane.Visible);
            }
            SetTaskPaneWidth(_layout.TaskPane.Width);
            if (!string.IsNullOrEmpty(_layout.TaskPane.InitialComponentID))
            {
                var cmp = GetComponent(_layout.TaskPane.InitialComponentID) as MgViewerComponent;
                if (cmp != null)
                    taskPane.SetInitialComponent(cmp);
                else
                    MessageBox.Show(string.Format(Strings.WarnInitialTaskComponentNotFound, _layout.TaskPane.InitialComponentID));
            }

            _provider = provider;
            mapViewer.PropertyChanged += new PropertyChangedEventHandler(OnMapViewerPropertyChanged);
        }

        const string RESERVED_CHARS = "\\:*?\"<>|&'%=/"; //NOXLATE

        private static void ValidateMapNames(AppLayout layout)
        {
            string mapName = layout.Map.Name;
            if (mapName.Contains(" ")) //NOXLATE
                throw new InvalidOperationException(string.Format(Strings.ErrorInvalidMapName, mapName));

            foreach (char c in mapName)
            {
                if (RESERVED_CHARS.IndexOf(c) >= 0)
                    throw new InvalidOperationException(string.Format(Strings.ErrorInvalidMapName, mapName));
            }
        }

        private void SetLanguage(CultureInfo lang)
        {
            //The reason we have to do this is because we're setting the language after this object
            //has been initialized and resources applied. We basically have to re-apply against the
            //current language
            ComponentResourceManager resources = new ComponentResourceManager(this.GetType());
            ApplyResourceToControl(resources, this, lang);
            resources.ApplyResources(this, "$this", lang); //NOXLATE

            //NOTE: Property pane is a separate case that has to be handled individually
            propertyPane.SetLanguage(lang);
        }

        private static void ApplyResourceToControl(ComponentResourceManager resources, Control control, CultureInfo lang)
        {
            foreach (Control c in control.Controls)
            {
                ApplyResourceToControl(resources, c, lang);
                resources.ApplyResources(c, c.Name, lang);
            }
        }

        internal void SetTaskPaneWidth(uint width)
        {
            if (appContainer.Panel2Collapsed)
                appContainer.Panel2Collapsed = false;
            //appContainer.Panel2.Width = (int)width;
            appContainer.SplitterDistance = (int)(appContainer.Width - width);
        }

        internal void SetInfoPaneWidth(uint width)
        {
            if (infoPaneViewerContainer.Panel1Collapsed)
                infoPaneViewerContainer.Panel1Collapsed = false;
            //infoPaneViewerContainer.Panel1.Width = (int)width;
            infoPaneViewerContainer.SplitterDistance = (int)width;
        }

        internal void SetLegendVisbility(bool visible)
        {
            layerPropertiesContainer.Panel1Collapsed = !visible;
        }

        internal void SetPropertyPaneVisbility(bool visible)
        {
            layerPropertiesContainer.Panel2Collapsed = !visible;
        }

        internal void SetTaskPaneVisible(bool visible)
        {
            appContainer.Panel2Collapsed = !visible;
        }

        internal void SetInfoPaneVisible(bool visible)
        {
            infoPaneViewerContainer.Panel1Collapsed = !visible;
        }

        void OnMapViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsBusy") //NOXLATE
            {
                if (_loader != null)
                    _loader.Visible = mapViewer.IsBusy;
            }
        }

        private AppLayout _layout;
        private MgMapViewerProvider _provider;

        private string _invokeComponentOnStartup;
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="e"></param>
        protected override void OnLoad(EventArgs e)
        {
            //Optimization: If legend or property pane aren't visible
            //don't wire them up to the controller
            var theLegend = legend;
            var thePropertyPane = propertyPane;
            if (!_layout.InfoPane.Legend.Visible)
                theLegend = null;
            if (!_layout.InfoPane.PropertyPane.Visible)
                thePropertyPane = null;

            new MapViewerController(mapViewer, theLegend, this, thePropertyPane);
            mapViewer.Init(_provider);

            if (!string.IsNullOrEmpty(_invokeComponentOnStartup))
            {
                if (_components.ContainsKey(_invokeComponentOnStartup))
                {
                    _components[_invokeComponentOnStartup].Invoke();
                }
                else
                {
                    MessageBox.Show(string.Format(Strings.WarnInvokeNonExistentComponent, _invokeComponentOnStartup));
                }
            }
        }

        private MgMenuItemComponentInvoker _menuInvoker;
        private MgToolButtonComponentInvoker _toolInvoker;
        private Dictionary<string, MgComponent> _components;

        private void InitializeTaskPaneMenu(MenuDefinition menuDefinition)
        {
            taskMenu.DropDown.Items.Clear();
            taskMenu.DropDownDirection = ToolStripDropDownDirection.BelowLeft;
            foreach (var item in menuDefinition.Items)
            {
                taskMenu.DropDown.Items.Add(CreateMenuItem(item));
            }
        }

        private void InitializeContextMenu(MenuDefinition menuDefinition)
        {
            viewerContextMenu.Items.Clear();
            foreach (var item in menuDefinition.Items)
            {
                viewerContextMenu.Items.Add(CreateMenuItem(item));
            }
        }

        private ToolStripLabel _loader;

        private void InitializeToolbar(ToolbarDefinition toolbarDefinition)
        {
            viewerToolbar.Items.Clear();
            foreach (var item in toolbarDefinition.Items)
            {
                viewerToolbar.Items.Add(CreateToolbarItem(item));
            }

            if (_loader == null)
            {
                _loader = new ToolStripLabel(Properties.Resources.icon_loading);
                _loader.Alignment = ToolStripItemAlignment.Right;
                _loader.Text = string.Empty;
                _loader.ImageScaling = ToolStripItemImageScaling.None;
            }
            viewerToolbar.Items.Add(_loader);
            _loader.Visible = mapViewer.IsBusy;
        }

        private ToolStripItem CreateToolbarItem(ItemBase item)
        {
            if (item == null)
                throw new ArgumentNullException("item"); //NOXLATE

            if (item is SeparatorItem)
            {
                return new ToolStripSeparator();
            }
            else if (item is CommandItem)
            {
                var cmd = (CommandItem)item;
                var tsi = new ToolStripButton();
                _toolInvoker.SetTargetComponent(tsi, _components[cmd.ComponentID]);
                if (cmd.ShowLabel)
                    tsi.DisplayStyle = ToolStripItemDisplayStyle.ImageAndText;
                else
                    tsi.DisplayStyle = ToolStripItemDisplayStyle.Image;
                return tsi;
            }
            else if (item is SubMenu)
            {
                var sm = (SubMenu)item;
                var tsi = new ToolStripDropDownButton();
                tsi.Image = Properties.Resources.icon_tasks;
                tsi.Text = sm.Label;
                tsi.ToolTipText = sm.Label;

                foreach (var child in sm.Items)
                {
                    tsi.DropDown.Items.Add(CreateMenuItem(child));
                }
                return tsi;
            }
            else
            {
                throw new NotSupportedException(string.Format(Strings.ErrorUnsupportedItemType, item.GetType().Name));
            }
        }

        private ToolStripItem CreateMenuItem(ItemBase item)
        {
            if (item == null)
                throw new ArgumentNullException("item"); //NOXLATE

            if (item is SeparatorItem)
            {
                return new ToolStripSeparator();
            }
            else if (item is CommandItem)
            {
                var cmd = (CommandItem)item;
                var tsi = new ToolStripMenuItem();
                _menuInvoker.SetTargetComponent(tsi, _components[cmd.ComponentID]);
                //Disregard ShowLabel property
                tsi.DisplayStyle = ToolStripItemDisplayStyle.ImageAndText;
                //if (cmd.ShowLabel)
                //    tsi.DisplayStyle = ToolStripItemDisplayStyle.ImageAndText;
                //else
                //    tsi.DisplayStyle = ToolStripItemDisplayStyle.Image;
                return tsi;
            }
            else if (item is SubMenu)
            {
                var sm = (SubMenu)item;
                var tsi = new ToolStripMenuItem();
                //tsi.Image = Properties.Resources.icon_tasks;
                tsi.Text = sm.Label;
                tsi.ToolTipText = sm.Label;

                foreach (var child in sm.Items)
                {
                    tsi.DropDown.Items.Add(CreateMenuItem(child));
                }
                return tsi;
            }
            else
            {
                throw new NotSupportedException(string.Format(Strings.ErrorUnsupportedItemType, item.GetType().Name));
            }
        }

        private void InitializeMenu(MenuDefinition menuDefinition)
        {
            mainMenu.Items.Clear();
            foreach (var item in menuDefinition.Items)
            {
                mainMenu.Items.Add(CreateMenuItem(item));
            }
        }

        private void InitializeComponentSet(AppLayout layout)
        {
            var thisAsm = this.GetType().Assembly;
            var thisPublicTypes = thisAsm.GetExportedTypes();
            var assemblies = new Dictionary<string, Assembly>();
            _components = new Dictionary<string, MgComponent>();

            // We do this in 4 passes:
            //
            // 1. Create the components in the component set
            // 2. Then set the properties of the instantiated components
            // 3. Assign the viewer to all these components
            // 4. Set the owner parent and Task Pane of any MgViewerComponent instances to this instance

            // 1st pass
            foreach (var compDef in layout.Components)
            {
                if (compDef.Assembly == null)
                {
                    foreach (var type in thisPublicTypes)
                    {
                        if (type.FullName == compDef.ClassName)
                        {
                            if (_components.ContainsKey(compDef.ComponentID))
                                throw new InvalidOperationException(string.Format(Strings.ErrorComponentAlreadyExists, compDef.ComponentID));
                            var comp = (MgComponent)Activator.CreateInstance(type);
                            _components[compDef.ComponentID] = comp;
                            //Override default label if specified
                            if (!string.IsNullOrEmpty(compDef.Label))
                                comp.Label = compDef.Label;
                            break;
                        }
                    }
                }
                else
                {
                    if (!assemblies.ContainsKey(compDef.Assembly))
                    {
                        assemblies[compDef.Assembly] = Assembly.LoadFrom(compDef.Assembly);
                    }

                    foreach (var type in assemblies[compDef.Assembly].GetExportedTypes())
                    {
                        if (type.FullName == compDef.ClassName)
                        {
                            if (_components.ContainsKey(compDef.ComponentID))
                                throw new InvalidOperationException(string.Format(Strings.ErrorComponentAlreadyExists, compDef.ComponentID));
                            var comp = (MgComponent)Activator.CreateInstance(type);
                            _components[compDef.ComponentID] = comp;
                            break;
                        }
                    }
                }
            }
            //2nd pass
            foreach (var compDef in layout.Components)
            {
                var comp = _components[compDef.ComponentID];
                if (compDef.Properties == null)
                    continue;

                foreach (var prop in compDef.Properties)
                {
                    if (prop.Value.StartsWith(StringPrefixes.MAPDEFINITION))
                    {
                        var mapName = prop.Value.Substring(StringPrefixes.MAPDEFINITION.Length);
                        //TODO: Update for multi-maps if/when we support it
                        if (layout.Map.Name == mapName)
                            comp.SetPropertyValue(prop.Name, layout.Map.MapDefinition);
                        else
                            throw new InvalidOperationException(string.Format(Strings.ErrorMapNotFound, mapName));
                    }
                    else if (prop.Value.StartsWith(StringPrefixes.COLOR))
                    {
                        var colorStr = prop.Value.Substring(StringPrefixes.COLOR.Length);
                        var color = Util.FromHtmlColor(colorStr);
                        comp.SetPropertyValue(prop.Name, color);
                    }
                    else if (prop.Value.StartsWith(StringPrefixes.COMPONENTID))
                    {
                        var compID = prop.Value.Substring(StringPrefixes.COMPONENTID.Length);
                        if (!_components.ContainsKey(compID))
                            throw new InvalidOperationException(string.Format(Strings.ErrorComponentNotFound, compID));

                        comp.SetPropertyValue(prop.Name, _components[compID]);
                    }
                    else if (prop.Value.StartsWith(StringPrefixes.ENUM))
                    {
                        string [] tokens = prop.Value.Split(':'); //NOXLATE
                        if (tokens.Length != 3)
                            throw new InvalidOperationException(Strings.ErrorMalformedEnumString);
                        comp.SetPropertyValue(prop.Name, Enum.Parse(Type.GetType(tokens[1]), tokens[2]));
                    }
                    else if (prop.Value.StartsWith(StringPrefixes.STRINGARRAY))
                    {
                        var csvList = prop.Value.Substring(StringPrefixes.STRINGARRAY.Length);
                        var values = csvList.Split(',');
                        comp.SetPropertyValue(prop.Name, values);
                    }
                    else if (prop.Value.StartsWith(StringPrefixes.TASKPANEID)) //NOTE: only one taskpane instance, but we're checking this as a forward-looking measure
                    {
                        comp.SetPropertyValue(prop.Name, taskPane);
                    }
                    else if (prop.Value.StartsWith(StringPrefixes.VIEWERID)) //NOTE: only one viewer instance, but we're checking this as a forward-looking measure
                    {
                        comp.SetPropertyValue(prop.Name, mapViewer);
                    }
                    else
                    {
                        comp.SetPropertyValue(prop.Name, prop.Value);
                    }
                }
            }

            //Apply viewer properties. We do this here because we want to respect the viewer options component
            //So we apply before the viewer options component gets its chance to
            foreach (var prop in layout.Settings)
            {
                //Special case
                if (prop.Name == "InvokeOnStartup") //NOXLATE
                {
                    _invokeComponentOnStartup = prop.Value.Substring(StringPrefixes.COMPONENTID.Length);
                    continue;
                }

                if (prop.Value.StartsWith(StringPrefixes.MAPDEFINITION))
                {
                    var mapName = prop.Value.Substring(StringPrefixes.MAPDEFINITION.Length);
                    //TODO: Update for multi-maps if/when we support it
                    if (layout.Map.Name == mapName)
                        mapViewer.SetPropertyValue(prop.Name, layout.Map.MapDefinition);
                    else
                        throw new InvalidOperationException(string.Format(Strings.ErrorMapNotFound, mapName));
                }
                else if (prop.Value.StartsWith(StringPrefixes.COLOR))
                {
                    var colorStr = prop.Value.Substring(StringPrefixes.COLOR.Length);
                    var color = Util.FromHtmlColor(colorStr);
                    mapViewer.SetPropertyValue(prop.Name, color);
                }
                else if (prop.Value.StartsWith(StringPrefixes.COMPONENTID))
                {
                    var compID = prop.Value.Substring(StringPrefixes.COMPONENTID.Length);
                    if (!_components.ContainsKey(compID))
                        throw new InvalidOperationException(string.Format(Strings.ErrorComponentNotFound, compID));

                    mapViewer.SetPropertyValue(prop.Name, _components[compID]);
                }
                else if (prop.Value.StartsWith(StringPrefixes.ENUM))
                {
                    string[] tokens = prop.Value.Split(':'); //NOXLATE
                    if (tokens.Length != 3)
                        throw new InvalidOperationException(Strings.ErrorMalformedEnumString);
                    mapViewer.SetPropertyValue(prop.Name, Enum.Parse(Type.GetType(tokens[1]), tokens[2]));
                }
                else if (prop.Value.StartsWith(StringPrefixes.TASKPANEID)) //NOTE: only one taskpane instance, but we're checking this as a forward-looking measure
                {
                    mapViewer.SetPropertyValue(prop.Name, taskPane);
                }
                else if (prop.Value.StartsWith(StringPrefixes.VIEWERID)) //NOTE: only one viewer instance, but we're checking this as a forward-looking measure
                {
                    mapViewer.SetPropertyValue(prop.Name, mapViewer);
                }
                else
                {
                    mapViewer.SetPropertyValue(prop.Name, prop.Value);
                }
            }

            //3rd pass
            foreach (var compDef in layout.Components)
            {
                _components[compDef.ComponentID].Viewer = mapViewer;
            }

            //4th pass, set the owner parent and Task Pane of any MgViewerComponent instances
            //to this instance
            foreach (var comp in _components.Values)
            {
                var vc = comp as MgViewerComponent;
                if (vc != null)
                {
                    vc.OwnerParent = this;
                    if (vc.Target == MgViewerTarget.TaskPane)
                        vc.TaskPane = taskPane;
                }
            }
        }

        /// <summary>
        /// Sets the message for the cursor position element of the status bar
        /// </summary>
        /// <param name="message"></param>
        public void SetCursorPositionMessage(string message)
        {
            lblCoordinates.Text = message;
        }

        /// <summary>
        /// Sets the message for the selected feature count element of the status bar
        /// </summary>
        /// <param name="message"></param>
        public void SetFeatureSelectedMessage(string message)
        {
            lblSelected.Text = message;
        }

        /// <summary>
        /// Sets the message for the map scale element of the status bar
        /// </summary>
        /// <param name="message"></param>
        public void SetMapScaleMessage(string message)
        {
            lblMapScale.Text = message;
        }

        /// <summary>
        /// Sets the message for the map size element of the status bar
        /// </summary>
        /// <param name="message"></param>
        public void SetMapSizeMessage(string message)
        {
            lblMapSize.Text = message;
        }

        private void btnInitialTask_Click(object sender, EventArgs e)
        {
            taskPane.LoadInitialTask();
        }

        /// <summary>
        /// Gets the component by its registered component id
        /// </summary>
        /// <param name="componentId"></param>
        /// <returns></returns>
        public MgComponent GetComponent(string componentId)
        {
            return _components.ContainsKey(componentId) ? _components[componentId] : null;
        }

        /// <summary>
        /// Gets the legend component
        /// </summary>
        public IMapLegend Legend
        {
            get { return legend; }
        }

        /// <summary>
        /// Gets the property pane component
        /// </summary>
        public IPropertyPane PropertyPane
        {
            get { return propertyPane; }
        }

        /// <summary>
        /// Gets the task pane component
        /// </summary>
        public MgTaskPane TaskPane
        {
            get { return taskPane; }
        }

        /// <summary>
        /// Reloads the map viewer
        /// </summary>
        /// <param name="provider"></param>
        public void ReloadViewer(MgMapViewerProvider provider)
        {
            _provider = provider;
            mapViewer.Init(_provider);
        }

        private void legend_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsBusy")
            {
                UpdateLegendTabText();
            }
        }

        private void UpdateLegendTabText()
        {
            TAB_LEGEND.Text = legend.IsBusy ? Strings.TextLayersRefreshing : Strings.TextLayers;
        }
    }

    /// <summary>
    /// Defines the top-level window of an AppLayout application
    /// </summary>
    public interface IShell : IMapStatusBar
    {
        /// <summary>
        /// Gets the component by its registered component id
        /// </summary>
        /// <param name="componentId"></param>
        /// <returns></returns>
        MgComponent GetComponent(string componentId);

        /// <summary>
        /// Gets the legend component
        /// </summary>
        IMapLegend Legend { get; }

        /// <summary>
        /// Gets the property pane component
        /// </summary>
        IPropertyPane PropertyPane { get; }

        /// <summary>
        /// Gets the task pane component
        /// </summary>
        MgTaskPane TaskPane { get; }

        /// <summary>
        /// Reloads the map viewer
        /// </summary>
        /// <param name="provider"></param>
        void ReloadViewer(MgMapViewerProvider provider);
    }
}
