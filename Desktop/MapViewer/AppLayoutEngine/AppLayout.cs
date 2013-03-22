using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;

namespace OSGeo.MapGuide.Viewer.AppLayoutEngine
{
    /// <summary>
    /// Models an Application Layout, defining the user interface and functionality of your application
    /// </summary>
    public class AppLayout
    {
        /// <summary>
        /// The window title
        /// </summary>
        [XmlElement]
        public string Title { get; set; }

        /// <summary>
        /// The application icon
        /// </summary>
        [XmlElement]
        public string Icon { get; set; }

        /// <summary>
        /// The application locale
        /// </summary>
        [XmlElement]
        public string Language { get; set; }

        /// <summary>
        /// A set of top-level application settings
        /// </summary>
        [XmlArray]
        public List<NameValue> Settings { get; set; }

        /// <summary>
        /// Settings for the information pane (legend + properties)
        /// </summary>
        [XmlElement]
        public InfoPaneSettings InfoPane { get; set; }

        /// <summary>
        /// Settings for the Map Definition to be loaded
        /// </summary>
        [XmlElement]
        public MapReference Map { get; set; }

        /// <summary>
        /// Defines the layout of the application's menus
        /// </summary>
        [XmlElement]
        public MenuDefinition Menu { get; set; }

        /// <summary>
        /// Defines the layout of the application's toolbar
        /// </summary>
        [XmlElement]
        public ToolbarDefinition Toolbar { get; set; }

        /// <summary>
        /// Defines the layout of the viewer's context menu
        /// </summary>
        [XmlElement]
        public MenuDefinition ContextMenu { get; set; }

        /// <summary>
        /// Defines settings for the application's task pane
        /// </summary>
        [XmlElement]
        public TaskPaneDefinition TaskPane { get; set; }

        /// <summary>
        /// Defines the list of components available to this application
        /// </summary>
        [XmlArray]
        public List<ComponentDefinition> Components { get; set; }

        /// <summary>
        /// Creates a default AppLayout based on the given map definition
        /// </summary>
        /// <param name="title"></param>
        /// <param name="mapDefinition"></param>
        /// <returns></returns>
        public static AppLayout CreateDefault(string title, string mapDefinition)
        {
            var layout = new AppLayout();
            layout.Title = title;
            layout.Settings = new List<NameValue>()
            {
                new NameValue() { Name = "ConvertTiledGroupsToNonTiled", Value = "true" }, //NOXLATE
                new NameValue() { Name = "UseRenderMap", Value = "true" }, //NOXLATE
                new NameValue() { Name = "SelectionColor", Value = "color:" + Util.ToHtmlColorWithAlpha(System.Drawing.Color.Blue) }, //NOXLATE
                new NameValue() { Name = "ShowVertexCoordinatesWhenDigitizing", Value = "true" }, //NOXLATE
                new NameValue() { Name = "ZoomInFactor", Value = "0.5" }, //NOXLATE
                new NameValue() { Name = "ZoomOutFactor", Value = "2.0" }, //NOXLATE
                new NameValue() { Name = "PointPixelBuffer", Value = "3" } //NOXLATE
            };
            layout.InfoPane = new InfoPaneSettings()
            {
                Legend = new LegendSettings() 
                {
                    Visible = true,
                    ThemeCompressionLimit = 25
                },
                PropertyPane = new PropertyPaneSettings() 
                {
                    Visible = true
                },
                Width = 200
            };
            layout.Map = new MapReference()
            {
                MapDefinition = mapDefinition
            };
            layout.Menu = new MenuDefinition()
            {
                Items = new List<ItemBase>()
                {
                    new SubMenu(Strings.MenuFile) 
                    {
                        Items = new List<ItemBase>()
                        {
                            new CommandItem("LoadMap"), //NOXLATE
                            new CommandItem("LoadPackage"), //NOXLATE
                            new SeparatorItem(),
                            new CommandItem("Quit") //NOXLATE
                        }
                    },
                    new SubMenu(Strings.MenuTools)
                    {
                        Items = new List<ItemBase>()
                        {
                            new CommandItem("Buffer"), //NOXLATE
                            new CommandItem("Measure"), //NOXLATE
                            new CommandItem("Query"), //NOXLATE
                            new CommandItem("Theme"), //NOXLATE
                            new SeparatorItem(),
                            new CommandItem("ViewerOptions") //NOXLATE
                        }
                    }
                }
            };
            layout.Toolbar = new ToolbarDefinition()
            {
                Items = new List<ItemBase>()
                {
                    new CommandItem("PrintMap", false), //NOXLATE
                    new SeparatorItem(),
                    new CommandItem("CopyMap", false), //NOXLATE
                    new SeparatorItem(),
                    new CommandItem("ZoomIn", false), //NOXLATE
                    new CommandItem("ZoomOut", false), //NOXLATE
                    new CommandItem("InitialView", false), //NOXLATE
                    new SeparatorItem(),
                    new CommandItem("ZoomPrev", false), //NOXLATE
                    new CommandItem("ZoomNext", false), //NOXLATE
                    new SeparatorItem(),
                    new CommandItem("Select", false), //NOXLATE
                    new CommandItem("SelectRadius", false), //NOXLATE
                    new CommandItem("SelectPolygon", false), //NOXLATE
                    new CommandItem("Pan", false), //NOXLATE
                    new SeparatorItem(),
                    new CommandItem("ClearSelection", false), //NOXLATE
                    new CommandItem("RefreshMap", false), //NOXLATE
                    new SeparatorItem(),
                    new CommandItem("TooltipToggle", true), //NOXLATE
                    new SeparatorItem(),
                    new SubMenu(Strings.MenuTools)
                    {
                        Items = new List<ItemBase>()
                        {
                            new CommandItem("Buffer"), //NOXLATE
                            new CommandItem("Measure"), //NOXLATE
                            new CommandItem("Query"), //NOXLATE
                            new CommandItem("Theme"), //NOXLATE
                            new SeparatorItem(),
                            new CommandItem("ViewerOptions") //NOXLATE
                        }
                    }
                }
            };
            layout.ContextMenu = new MenuDefinition()
            {
                Items = new List<ItemBase>()
                {
                    new CommandItem("RefreshMap"), //NOXLATE
                    new SeparatorItem(),
                    new CommandItem("ZoomIn"), //NOXLATE
                    new CommandItem("ZoomOut"), //NOXLATE
                    new CommandItem("ZoomToSelection"), //NOXLATE
                    new CommandItem("InitialView"), //NOXLATE
                    new SeparatorItem(),
                    new CommandItem("Pan"), //NOXLATE
                    new CommandItem("Select"), //NOXLATE
                    new CommandItem("ClearSelection"), //NOXLATE
                    new SeparatorItem(),
                    new SubMenu(Strings.MenuTools) 
                    {
                        Items = new List<ItemBase>() 
                        {
                            new CommandItem("Buffer"), //NOXLATE
                            new CommandItem("Measure"), //NOXLATE
                            new CommandItem("Query"), //NOXLATE
                            new CommandItem("Theme") //NOXLATE
                        }
                    },
                    new SeparatorItem(),
                    new CommandItem("ViewerOptions"), //NOXLATE
                }
            };
            layout.TaskPane = new TaskPaneDefinition()
            {
                Width = 250,
                TaskMenu = new MenuDefinition()
                {
                    Items = new List<ItemBase>()
                    {
                        new CommandItem("Buffer"), //NOXLATE
                        new CommandItem("Measure"), //NOXLATE
                        new CommandItem("Query"), //NOXLATE
                        new CommandItem("Theme") //NOXLATE
                    }
                }
            };
            layout.Components = new List<ComponentDefinition>()
            {
                new ComponentDefinition()
                {
                    ClassName = typeof(Viewer.MgBufferComponent).FullName,
                    Assembly = null,
                    ComponentID = "Buffer", //NOXLATE
                    Properties = new List<NameValue>()
                    {
                        new NameValue("DefaultLayerName", "BufferLayer"), //NOXLATE
                        new NameValue("DefaultBufferUnits", "enum:" + typeof(MeasurementUnit).FullName + ":" + MeasurementUnit.Meters.ToString()), //NOXLATE
                        new NameValue("Target", "enum:" + typeof(MgViewerTarget).FullName + ":" + MgViewerTarget.TaskPane), //NOXLATE
                        new NameValue("TaskPane", "taskpane:") //NOXLATE
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgCircleSelectComponent).FullName, "SelectRadius"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgClearSelectionComponent).FullName, "ClearSelection"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgCopyMapComponent).FullName, "CopyMap"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgInitialViewComponent).FullName, "InitialView"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgLoadMapComponent).FullName, "LoadMap") //NOXLATE
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("MapDefinition", mapDefinition) //NOXLATE
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgLoadPackageComponent).FullName, "LoadPackage") //NOXLATE
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("InvokeOnPackageLoad", "component:LoadMap") //NOXLATE
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgMeasureComponent).FullName, "Measure") //NOXLATE
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("MeasureMode", "enum:" + typeof(MeasureMode).FullName + ":" + MeasureMode.Line.ToString()), //NOXLATE
                        new NameValue("PreferredUnits", "enum:" + typeof(MeasurementUnit).FullName + ":" + MeasurementUnit.Meters.ToString()), //NOXLATE
                        new NameValue("Target", "enum:" + typeof(MgViewerTarget).FullName + ":" + MgViewerTarget.TaskPane), //NOXLATE
                        new NameValue("TaskPane", "taskpane:") //NOXLATE
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgPanComponent).FullName, "Pan"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgPolygonSelectComponent).FullName, "SelectPolygon"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgPrintComponent).FullName, "PrintMap"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgQueryComponent).FullName, "Query") //NOXLATE
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("Target", "enum:" + typeof(MgViewerTarget).FullName + ":" + MgViewerTarget.TaskPane), //NOXLATE
                        new NameValue("TaskPane", "taskpane:") //NOXLATE
                    }
                },
                new ComponentDefinition(typeof(Viewer.AppLayoutEngine.MgQuitComponent).FullName, "Quit"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgRefreshMapComponent).FullName, "RefreshMap"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgSelectComponent).FullName, "Select"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgThemeComponent).FullName, "Theme") //NOXLATE
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("Target", "enum:" + typeof(MgViewerTarget).FullName + ":" + MgViewerTarget.TaskPane), //NOXLATE
                        new NameValue("TaskPane", "taskpane:") //NOXLATE
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgTooltipToggleComponent).FullName, "TooltipToggle"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgViewerOptionsComponent).FullName, "ViewerOptions"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgZoomInComponent).FullName, "ZoomIn"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgZoomNextComponent).FullName, "ZoomNext"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgZoomOutComponent).FullName, "ZoomOut"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgZoomPreviousComponent).FullName, "ZoomPrev"), //NOXLATE
                new ComponentDefinition(typeof(Viewer.MgZoomToSelectionComponent).FullName, "ZoomToSelection") //NOXLATE
            };
            return layout;
        }
    }

    /// <summary>
    /// Defines settings for the information pane (legend + properties)
    /// </summary>
    public class InfoPaneSettings
    {
        /// <summary>
        /// The width of this pane
        /// </summary>
        [XmlElement]
        public uint Width { get; set; }

        /// <summary>
        /// Settings for the legend
        /// </summary>
        [XmlElement]
        public LegendSettings Legend { get; set; }

        /// <summary>
        /// Settings for the property pane
        /// </summary>
        [XmlElement]
        public PropertyPaneSettings PropertyPane { get; set; }

        /// <summary>
        /// Gets whether this pane is visible
        /// </summary>
        [XmlIgnore]
        public bool IsVisible { get { return this.Legend.Visible || this.PropertyPane.Visible; } }
    }

    /// <summary>
    /// Defines settigs for the legend
    /// </summary>
    public class LegendSettings
    {
        /// <summary>
        /// Gets or sets whether this element is visible
        /// </summary>
        [XmlElement]
        public bool Visible { get; set; }

        /// <summary>
        /// Gets or sets the value at which themes (containing a number of rules greater than this property) are compressed 
        /// </summary>
        [XmlElement]
        public int? ThemeCompressionLimit { get; set; }

        /// <summary>
        /// Gets or sets whether tooltips are shown
        /// </summary>
        [XmlElement]
        public bool ShowTooltips { get; set; }
    }

    /// <summary>
    /// Defines settings for the property pane
    /// </summary>
    public class PropertyPaneSettings
    {
        /// <summary>
        /// Gets or sets whether this element is visible
        /// </summary>
        [XmlElement]
        public bool Visible { get; set; }
    }
    
    /// <summary>
    /// Defines settings for the Map Definition
    /// </summary>
    public class MapReference
    {
        /// <summary>
        /// The name of this map
        /// </summary>
        [XmlElement]
        public string Name { get; set; }

        /// <summary>
        /// The Map Definition resource id
        /// </summary>
        [XmlElement]
        public string MapDefinition { get; set; }
    }

    /// <summary>
    /// Defines the layout of the application's menus
    /// </summary>
    public class MenuDefinition
    {
        /// <summary>
        /// The list of top level items
        /// </summary>
        [XmlArray]
        public List<ItemBase> Items { get; set; }
    }

    /// <summary>
    /// Defines the layout of the application's toolbar
    /// </summary>
    public class ToolbarDefinition
    {
        /// <summary>
        /// The list of top level items
        /// </summary>
        [XmlArray]
        public List<ItemBase> Items { get; set; }
    }

    /// <summary>
    /// Defines settings for the Task Pane
    /// </summary>
    public class TaskPaneDefinition
    {
        /// <summary>
        /// The width of this element
        /// </summary>
        [XmlElement]
        public uint Width { get; set; }

        /// <summary>
        /// Specifies the component ID of the Task Pane resident component. A Task Pane resident component
        /// has a "Target" property of Task Pane.
        /// </summary>
        [XmlElement]
        public string InitialComponentID { get; set; }

        /// <summary>
        /// Defines the layout of the menu in the Task Pane
        /// </summary>
        [XmlElement]
        public MenuDefinition TaskMenu { get; set; }
    }

    /// <summary>
    /// The super class of all toolbar and menu items
    /// </summary>
    [XmlInclude(typeof(CommandItem))]
    [XmlInclude(typeof(SeparatorItem))]
    [XmlInclude(typeof(SubMenu))]
    public class ItemBase
    {
        
    }

    /// <summary>
    /// A menu/toolbar separator
    /// </summary>
    public class SeparatorItem : ItemBase
    {
    }

    /// <summary>
    /// A sub-menu item
    /// </summary>
    public class SubMenu : ItemBase
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public SubMenu() { }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="label"></param>
        public SubMenu(string label) { this.Label = label; }

        /// <summary>
        /// The label
        /// </summary>
        [XmlElement]
        public string Label { get; set; }

        /// <summary>
        /// The list of child items
        /// </summary>
        [XmlArray]
        public List<ItemBase> Items { get; set; }
    }

    /// <summary>
    /// A menu/toolbar items that invokes a registered component
    /// </summary>
    public class CommandItem : ItemBase
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public CommandItem() { }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="componentId"></param>
        public CommandItem(string componentId) { this.ComponentID = componentId; }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="componentId"></param>
        /// <param name="showLabel"></param>
        public CommandItem(string componentId, bool showLabel) { this.ComponentID = componentId; this.ShowLabel = showLabel; }

        /// <summary>
        /// The id of the component to invoke when this item is clicked
        /// </summary>
        [XmlElement]
        public string ComponentID { get; set; }

        /// <summary>
        /// Gets whether to display labels in this item
        /// </summary>
        [XmlElement]
        public bool ShowLabel { get; set; }
    }

    /// <summary>
    /// Defines a component, a discrete piece of built-in or custom application functionality
    /// </summary>
    public class ComponentDefinition
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public ComponentDefinition() { this.Properties = new List<NameValue>(); }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="typeName"></param>
        /// <param name="componentID"></param>
        public ComponentDefinition(string typeName, string componentID) : base()
        {
            this.Assembly = null;
            this.ClassName = typeName;
            this.ComponentID = componentID;
        }

        /// <summary>
        /// The id of this component
        /// </summary>
        [XmlElement]
        public string ComponentID { get; set; }

        /// <summary>
        /// The label to assign to any toolbar/menu item that's tied to this component
        /// </summary>
        [XmlElement]
        public string Label { get; set; }

        /// <summary>
        /// The fully qualified .net name of the class that implements this component
        /// </summary>
        [XmlElement]
        public string ClassName { get; set; }

        /// <summary>
        /// The assembly that contains the .net class that implements this component. If empty
        /// it is assumed to originate from the viewer assembly
        /// </summary>
        [XmlElement]
        public string Assembly { get; set; }

        /// <summary>
        /// A list of properties that define and customize component behaviour
        /// </summary>
        [XmlArray]
        public List<NameValue> Properties { get; set; }
    }

    /// <summary>
    /// A key-value pair used to define properties
    /// </summary>
    public class NameValue
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public NameValue() { }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        public NameValue(string name, string value)
        {
            this.Name = name;
            this.Value = value;
        }

        /// <summary>
        /// The name of the property
        /// </summary>
        [XmlElement]
        public string Name { get; set; }

        /// <summary>
        /// The property's value
        /// </summary>
        [XmlElement]
        public string Value { get; set; }
    }

    /// <summary>
    /// Defines the set of prefixes for serialized component property string values
    /// to hint at what type to deserialize this value as. CLR property values are
    /// serialized/deserialized as is without any need for prefixing.
    /// </summary>
    public class StringPrefixes
    {
        /// <summary>
        /// Map Definition
        /// </summary>
        public const string MAPDEFINITION = "map:"; //NOXLATE

        /// <summary>
        /// Component ID
        /// </summary>
        public const string COMPONENTID = "component:"; //NOXLATE

        /// <summary>
        /// Color
        /// </summary>
        public const string COLOR = "color:"; //NOXLATE

        /// <summary>
        /// A .net enum
        /// </summary>
        public const string ENUM = "enum:"; //NOXLATE

        /// <summary>
        /// Viewer ID
        /// </summary>
        public const string VIEWERID = "viewer:"; //NOXLATE

        /// <summary>
        /// Task Pane ID
        /// </summary>
        public const string TASKPANEID = "taskpane:"; //NOXLATE
        
        /// <summary>
        /// An array of string values
        /// </summary>
        public const string STRINGARRAY = "stringarray:"; //NOXLATE
    }
}
