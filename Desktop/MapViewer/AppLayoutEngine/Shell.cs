using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace OSGeo.MapGuide.Viewer.AppLayoutEngine
{
    public partial class Shell : Form, IMapStatusBar
    {
        public Shell(AppLayout layout, MgMapViewerProvider provider)
        {
            InitializeComponent();
            this.Text = layout.Title;
            _menuInvoker = new MgMenuItemComponentInvoker();
            _toolInvoker = new MgToolButtonComponentInvoker();

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
                SetPropertyPaneVisbility(_layout.InfoPane.PropertyPane.Visible);
            }
            SetTaskPaneWidth(_layout.TaskPane.Width);

            //Apply viewer properties
            mapViewer.ConvertTiledGroupsToNonTiled = _layout.Settings.ConvertTiledGroupsToNonTiled;
            mapViewer.SelectionColor = Util.FromHtmlColor(_layout.Settings.SelectionColor);
            mapViewer.ShowVertexCoordinatesWhenDigitizing = _layout.Settings.ShowVertexCoordinatesWhenDigitizing;
            mapViewer.ZoomInFactor = _layout.Settings.ZoomInFactor;
            mapViewer.ZoomOutFactor = _layout.Settings.ZoomOutFactor;

            _provider = provider;
            mapViewer.PropertyChanged += new PropertyChangedEventHandler(OnMapViewerPropertyChanged);
        }

        public void SetTaskPaneWidth(uint width)
        {
            if (appContainer.Panel2Collapsed)
                appContainer.Panel2Collapsed = false;
            //appContainer.Panel2.Width = (int)width;
            appContainer.SplitterDistance = (int)(appContainer.Width - width);
        }

        public void SetInfoPaneWidth(uint width)
        {
            if (infoPaneViewerContainer.Panel1Collapsed)
                infoPaneViewerContainer.Panel1Collapsed = false;
            //infoPaneViewerContainer.Panel1.Width = (int)width;
            infoPaneViewerContainer.SplitterDistance = (int)width;
        }

        public void SetLegendVisbility(bool visible)
        {
            layerPropertiesContainer.Panel1Collapsed = !visible;
        }

        public void SetPropertyPaneVisbility(bool visible)
        {
            layerPropertiesContainer.Panel2Collapsed = !visible;
        }

        public void SetTaskPaneVisible(bool visible)
        {
            appContainer.Panel2Collapsed = !visible;
        }

        public void SetInfoPaneVisible(bool visible)
        {
            infoPaneViewerContainer.Panel1Collapsed = !visible;
        }

        void OnMapViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsBusy")
            {
                if (_loader != null)
                    _loader.Visible = mapViewer.IsBusy;
            }
        }

        private AppLayout _layout;
        private MgMapViewerProvider _provider;

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

            if (!string.IsNullOrEmpty(_layout.Settings.InvokeOnStartup))
            {
                if (_components.ContainsKey(_layout.Settings.InvokeOnStartup))
                {
                    _components[_layout.Settings.InvokeOnStartup].Invoke();
                }
                else
                {
                    MessageBox.Show("WARNING: Attempted to invoke the component with ID (" + _layout.Settings.InvokeOnStartup + "), but could not find any component with this ID. The application will continue");
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
                _loader.Text = "";
                _loader.ImageScaling = ToolStripItemImageScaling.None;
            }
            viewerToolbar.Items.Add(_loader);
            _loader.Visible = mapViewer.IsBusy;
        }

        private ToolStripItem CreateToolbarItem(ItemBase item)
        {
            if (item == null)
                throw new ArgumentNullException("item");

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
                throw new NotSupportedException("Unsupported item type: " + item.GetType().Name);
            }
        }

        private ToolStripItem CreateMenuItem(ItemBase item)
        {
            if (item == null)
                throw new ArgumentNullException("item");

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
                throw new NotSupportedException("Unsupported item type: " + item.GetType().Name);
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

            // We do this in 3 passes:
            //
            // 1. Create the components in the component set
            // 2. Then set the properties of the instantiated components
            // 3. Assign the viewer to all these components

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
                                throw new InvalidOperationException("A component with ID " + compDef.ComponentID + " already exists");
                            var comp = (MgComponent)Activator.CreateInstance(type);
                            _components[compDef.ComponentID] = comp;
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
                                throw new InvalidOperationException("A component with ID " + compDef.ComponentID + " already exists");
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
                    if (prop.Value.StartsWith(StringPrefixes.COLOR))
                    {
                        var colorStr = prop.Value.Substring(StringPrefixes.COLOR.Length);
                        var color = Util.FromHtmlColor(colorStr);
                        comp.SetPropertyValue(prop.Name, color);
                    }
                    else if (prop.Value.StartsWith(StringPrefixes.COMPONENTID))
                    {
                        var compID = prop.Value.Substring(StringPrefixes.COMPONENTID.Length);
                        if (!_components.ContainsKey(compID))
                            throw new InvalidOperationException("Component " + compID + " does not exist");

                        comp.SetPropertyValue(prop.Name, _components[compID]);
                    }
                    else if (prop.Value.StartsWith(StringPrefixes.ENUM))
                    {
                        string [] tokens = prop.Value.Split(':');
                        if (tokens.Length != 3)
                            throw new InvalidOperationException("Malformed enum string. Expected enum:className:value");
                        comp.SetPropertyValue(prop.Name, Enum.Parse(Type.GetType(tokens[1]), tokens[2]));
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

            //3rd pass
            foreach (var compDef in layout.Components)
            {
                _components[compDef.ComponentID].Viewer = mapViewer;
            }
        }

        void IMapStatusBar.SetCursorPositionMessage(string message)
        {
            lblCoordinates.Text = message;
        }

        void IMapStatusBar.SetFeatureSelectedMessage(string message)
        {
            lblSelected.Text = message;
        }

        void IMapStatusBar.SetMapScaleMessage(string message)
        {
            lblMapScale.Text = message;
        }

        void IMapStatusBar.SetMapSizeMessage(string message)
        {
            lblMapSize.Text = message;
        }

        private void btnInitialTask_Click(object sender, EventArgs e)
        {
            taskPane.LoadInitialTask();
        }
    }
}
