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
        public AppLayoutSettings Settings { get; set; }

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
            layout.Settings = new AppLayoutSettings()
            {
                ConvertTiledGroupsToNonTiled = true,
                SelectionColor = Util.ToHtmlColorWithAlpha(System.Drawing.Color.Blue),
                ShowVertexCoordinatesWhenDigitizing = false,
                ZoomInFactor = 0.5,
                ZoomOutFactor = 2.0
            };
            layout.InfoPane = new InfoPaneSettings()
            {
                Legend = new LegendSettings() 
                {
                    Visible = true
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
                    new SubMenu("File") 
                    {
                        Items = new List<ItemBase>()
                        {
                            new CommandItem("LoadMap"),
                            new CommandItem("LoadPackage"),
                            new SeparatorItem(),
                            new CommandItem("Quit")
                        }
                    },
                    new SubMenu("Tools")
                    {
                        Items = new List<ItemBase>()
                        {
                            new CommandItem("Buffer"),
                            new CommandItem("Measure"),
                            new CommandItem("Query"),
                            new CommandItem("Theme"),
                            new SeparatorItem(),
                            new CommandItem("ViewerOptions")
                        }
                    }
                }
            };
            layout.Toolbar = new ToolbarDefinition()
            {
                Items = new List<ItemBase>()
                {
                    new CommandItem("PrintMap", false),
                    new SeparatorItem(),
                    new CommandItem("CopyMap", false),
                    new SeparatorItem(),
                    new CommandItem("ZoomIn", false),
                    new CommandItem("ZoomOut", false),
                    new CommandItem("InitialView", false),
                    new SeparatorItem(),
                    new CommandItem("Select", false),
                    new CommandItem("SelectRadius", false),
                    new CommandItem("SelectPolygon", false),
                    new CommandItem("Pan", false),
                    new SeparatorItem(),
                    new CommandItem("ClearSelection", false),
                    new CommandItem("RefreshMap", false),
                    new SeparatorItem(),
                    new CommandItem("TooltipToggle", true),
                    new SeparatorItem(),
                    new SubMenu("Tools")
                    {
                        Items = new List<ItemBase>()
                        {
                            new CommandItem("Buffer"),
                            new CommandItem("Measure"),
                            new CommandItem("Query"),
                            new CommandItem("Theme"),
                            new SeparatorItem(),
                            new CommandItem("ViewerOptions")
                        }
                    }
                }
            };
            layout.ContextMenu = new MenuDefinition()
            {
                Items = new List<ItemBase>()
                {
                    new CommandItem("RefreshMap"),
                    new SeparatorItem(),
                    new CommandItem("ZoomIn"),
                    new CommandItem("ZoomOut"),
                    new CommandItem("ZoomToSelection"),
                    new CommandItem("InitialView"),
                    new SeparatorItem(),
                    new CommandItem("Pan"),
                    new CommandItem("Select"),
                    new CommandItem("ClearSelection"),
                    new SeparatorItem(),
                    new SubMenu("Tools") 
                    {
                        Items = new List<ItemBase>() 
                        {
                            new CommandItem("Buffer"),
                            new CommandItem("Measure"),
                            new CommandItem("Query"),
                            new CommandItem("Theme")
                        }
                    },
                    new SeparatorItem(),
                    new CommandItem("ViewerOptions"),
                }
            };
            layout.TaskPane = new TaskPaneDefinition()
            {
                Width = 250,
                TaskMenu = new MenuDefinition()
                {
                    Items = new List<ItemBase>()
                    {
                        new CommandItem("Buffer"),
                        new CommandItem("Measure"),
                        new CommandItem("Query"),
                        new CommandItem("Theme")
                    }
                }
            };
            layout.Components = new List<ComponentDefinition>()
            {
                new ComponentDefinition()
                {
                    ClassName = typeof(Viewer.MgBufferComponent).FullName,
                    Assembly = null,
                    ComponentID = "Buffer",
                    Properties = new List<NameValue>()
                    {
                        new NameValue("DefaultLayerName", "BufferLayer"),
                        new NameValue("DefaultBufferUnits", "enum:" + typeof(MeasurementUnit).FullName + ":" + MeasurementUnit.Meters.ToString()),
                        new NameValue("Target", "enum:" + typeof(MgViewerTarget).FullName + ":" + MgViewerTarget.TaskPane),
                        new NameValue("TaskPane", "taskpane:")
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgCircleSelectComponent).FullName, "SelectRadius"),
                new ComponentDefinition(typeof(Viewer.MgClearSelectionComponent).FullName, "ClearSelection"),
                new ComponentDefinition(typeof(Viewer.MgCopyMapComponent).FullName, "CopyMap"),
                new ComponentDefinition(typeof(Viewer.MgInitialViewComponent).FullName, "InitialView"),
                new ComponentDefinition(typeof(Viewer.MgLoadMapComponent).FullName, "LoadMap")
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("MapDefinition", mapDefinition)
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgLoadPackageComponent).FullName, "LoadPackage")
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("InvokeOnPackageLoad", "component:LoadMap")
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgMeasureComponent).FullName, "Measure")
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("MeasureMode", "enum:" + typeof(MeasureMode).FullName + ":" + MeasureMode.Line.ToString()),
                        new NameValue("PreferredUnits", "enum:" + typeof(MeasurementUnit).FullName + ":" + MeasurementUnit.Meters.ToString()),
                        new NameValue("Target", "enum:" + typeof(MgViewerTarget).FullName + ":" + MgViewerTarget.TaskPane),
                        new NameValue("TaskPane", "taskpane:")
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgPanComponent).FullName, "Pan"),
                new ComponentDefinition(typeof(Viewer.MgPolygonSelectComponent).FullName, "SelectPolygon"),
                new ComponentDefinition(typeof(Viewer.MgPrintComponent).FullName, "PrintMap"),
                new ComponentDefinition(typeof(Viewer.MgQueryComponent).FullName, "Query")
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("Target", "enum:" + typeof(MgViewerTarget).FullName + ":" + MgViewerTarget.TaskPane),
                        new NameValue("TaskPane", "taskpane:")
                    }
                },
                new ComponentDefinition(typeof(Viewer.AppLayoutEngine.MgQuitComponent).FullName, "Quit"),
                new ComponentDefinition(typeof(Viewer.MgRefreshMapComponent).FullName, "RefreshMap"),
                new ComponentDefinition(typeof(Viewer.MgSelectComponent).FullName, "Select"),
                new ComponentDefinition(typeof(Viewer.MgThemeComponent).FullName, "Theme")
                {
                    Properties = new List<NameValue>()
                    {
                        new NameValue("Target", "enum:" + typeof(MgViewerTarget).FullName + ":" + MgViewerTarget.TaskPane),
                        new NameValue("TaskPane", "taskpane:")
                    }
                },
                new ComponentDefinition(typeof(Viewer.MgTooltipToggleComponent).FullName, "TooltipToggle"),
                new ComponentDefinition(typeof(Viewer.MgViewerOptionsComponent).FullName, "ViewerOptions"),
                new ComponentDefinition(typeof(Viewer.MgZoomInComponent).FullName, "ZoomIn"),
                new ComponentDefinition(typeof(Viewer.MgZoomOutComponent).FullName, "ZoomOut"),
                new ComponentDefinition(typeof(Viewer.MgZoomToSelectionComponent).FullName, "ZoomToSelection")
            };
            return layout;
        }
    }

    public class AppLayoutSettings
    {
        [XmlElement]
        public string InvokeOnStartup { get; set; }

        [XmlElement]
        public string SelectionColor { get; set; }

        [XmlElement]
        public bool ConvertTiledGroupsToNonTiled { get; set; }

        [XmlElement]
        public bool ShowVertexCoordinatesWhenDigitizing { get; set; }

        [XmlElement]
        public double ZoomInFactor { get; set; }

        [XmlElement]
        public double ZoomOutFactor { get; set; }
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
    }

    public class PropertyPaneSettings
    {
        [XmlElement]
        public bool Visible { get; set; }
    }

    public class MapReference
    {
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
        public const string COMPONENTID = "component:";
        public const string COLOR = "color:";
        public const string ENUM = "enum:";
        public const string VIEWERID = "viewer:";
        public const string TASKPANEID = "taskpane:";
    }
}
