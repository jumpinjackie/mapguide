using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;

namespace OSGeo.MapGuide.Viewer.AppLayoutEngine
{
    public class AppLayout
    {
        [XmlElement]
        public string Title { get; set; }

        [XmlElement]
        public string Icon { get; set; }

        [XmlElement]
        public string Language { get; set; }

        [XmlArray]
        public List<NameValue> Settings { get; set; }

        [XmlElement]
        public InfoPaneSettings InfoPane { get; set; }

        [XmlElement]
        public MapReference Map { get; set; }

        [XmlElement]
        public MenuDefinition Menu { get; set; }

        [XmlElement]
        public ToolbarDefinition Toolbar { get; set; }

        [XmlElement]
        public MenuDefinition ContextMenu { get; set; }

        [XmlElement]
        public TaskPaneDefinition TaskPane { get; set; }

        [XmlArray]
        public List<ComponentDefinition> Components { get; set; }

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

    public class InfoPaneSettings
    {
        [XmlElement]
        public uint Width { get; set; }

        [XmlElement]
        public LegendSettings Legend { get; set; }

        [XmlElement]
        public PropertyPaneSettings PropertyPane { get; set; }

        [XmlIgnore]
        public bool IsVisible { get { return this.Legend.Visible || this.PropertyPane.Visible; } }
    }

    public class LegendSettings
    {
        [XmlElement]
        public bool Visible { get; set; }

        [XmlElement]
        public int? ThemeCompressionLimit { get; set; }

        [XmlElement]
        public bool ShowTooltips { get; set; }
    }

    public class PropertyPaneSettings
    {
        [XmlElement]
        public bool Visible { get; set; }
    }

    public class MapReference
    {
        [XmlElement]
        public string Name { get; set; }

        [XmlElement]
        public string MapDefinition { get; set; }
    }

    public class MenuDefinition
    {
        [XmlArray]
        public List<ItemBase> Items { get; set; }
    }

    public class ToolbarDefinition
    {
        [XmlArray]
        public List<ItemBase> Items { get; set; }
    }

    public class TaskPaneDefinition
    {
        [XmlElement]
        public uint Width { get; set; }

        [XmlElement]
        public string InitialComponentID { get; set; }

        [XmlElement]
        public MenuDefinition TaskMenu { get; set; }
    }

    [XmlInclude(typeof(CommandItem))]
    [XmlInclude(typeof(SeparatorItem))]
    [XmlInclude(typeof(SubMenu))]
    public class ItemBase
    {
        
    }

    public class SeparatorItem : ItemBase
    {
    }

    public class SubMenu : ItemBase
    {
        public SubMenu() { }

        public SubMenu(string label) { this.Label = label; }

        [XmlElement]
        public string Label { get; set; }

        [XmlArray]
        public List<ItemBase> Items { get; set; }
    }

    public class CommandItem : ItemBase
    {
        public CommandItem() { }

        public CommandItem(string componentId) { this.ComponentID = componentId; }

        public CommandItem(string componentId, bool showLabel) { this.ComponentID = componentId; this.ShowLabel = showLabel; }

        [XmlElement]
        public string ComponentID { get; set; }

        [XmlElement]
        public bool ShowLabel { get; set; }
    }

    public class ComponentDefinition
    {
        public ComponentDefinition() { this.Properties = new List<NameValue>(); }

        public ComponentDefinition(string typeName, string componentID) : base()
        {
            this.Assembly = null;
            this.ClassName = typeName;
            this.ComponentID = componentID;
        }

        [XmlElement]
        public string ComponentID { get; set; }

        [XmlElement]
        public string ClassName { get; set; }

        [XmlElement]
        public string Assembly { get; set; }

        [XmlArray]
        public List<NameValue> Properties { get; set; }
    }

    public class NameValue
    {
        public NameValue() { }

        public NameValue(string name, string value)
        {
            this.Name = name;
            this.Value = value;
        }

        [XmlElement]
        public string Name { get; set; }

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
        public const string MAPDEFINITION = "map:"; //NOXLATE
        public const string COMPONENTID = "component:"; //NOXLATE
        public const string COLOR = "color:"; //NOXLATE
        public const string ENUM = "enum:"; //NOXLATE
        public const string VIEWERID = "viewer:"; //NOXLATE
        public const string TASKPANEID = "taskpane:"; //NOXLATE
    }
}
