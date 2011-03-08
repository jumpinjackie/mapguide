﻿#region Disclaimer / License
// Copyright (C) 2010, Jackie Ng
// http://trac.osgeo.org/mapguide/wiki/maestro, jumpinjackie@gmail.com
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
// 
#endregion
using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using OSGeo.MapGuide.MaestroAPI;
using OSGeo.MapGuide.MaestroAPI.Resource;
using OSGeo.MapGuide.ObjectModels.ApplicationDefinition;
using System.Collections.Specialized;

#pragma warning disable 1591, 0114, 0108

namespace OSGeo.MapGuide.ObjectModels.ApplicationDefinition_1_0_0
{
    partial class CustomContentType : IExtension
    {
        XmlElement[] IExtension.Content
        {
            get
            {
                return this.Any;
            }
            set
            {
                this.Any = value;
            }
        }
    }

    partial class ApplicationDefinitionType : IApplicationDefinition, IMapSet
    {
        internal ApplicationDefinitionType() 
        {
            this.Extension = new CustomContentType() { Any = new XmlElement[0] };
        }

        private static readonly Version RES_VERSION = new Version(1, 0, 0);

        public string[] GetAllReferenceableWidgetNames()
        {
            List<string> names = new List<string>();

            foreach (var ws in this.WidgetSets)
            {
                foreach (var wgt in ws.Widgets)
                {
                    names.Add(wgt.Name);
                }
            }

            return names.ToArray();
        }

        public IMapWidget CreateMapWidget(string mapId, string contextMenuId)
        {
            var map = new MapWidgetType()
            {
                MapId = mapId,
                Name = "Map",
                Type = "Map",
                Extension = new CustomContentType() { Any = new XmlElement[0] }
            };
            map.SetValue("MenuContainer", contextMenuId);
            return map;
        }

        public IWidgetItem CreateWidgetReference(string name) { return new WidgetItemType() { Function = UiItemFunctionType.Widget, Widget = name }; }

        public ISeparator CreateSeparator() { return new SeparatorItemType() { Function = UiItemFunctionType.Separator }; }

        public IFlyoutItem CreateFlyout(string label) 
        { 
            return new FlyoutItemType() 
            { 
                Function = UiItemFunctionType.Flyout, 
                Label = label,
                Item = new System.ComponentModel.BindingList<UiItemType>()
            }; 
        }

        public IMapGroup AddMapGroup(string id, bool singleTile, string mapDefinitionId)
        {
            var map = new MapGroupType()
            {
                id = id,
                Map = new System.ComponentModel.BindingList<MapType>()
                {
                    new MapType()
                    {
                        Type = "MapGuide",
                        SingleTile = singleTile.ToString(),
                        Extension = new CustomContentType()
                        {
                            Any = new XmlElement[1]
                        }
                    }
                },
            };
            map.Map[0].Extension.Any[0] = AppDefDocument.Instance.CreateElement("ResourceId");
            map.Map[0].Extension.Any[0].InnerText = mapDefinitionId;

            this.MapSet.Add(map);

            return map;
        }

        public IMapGroup AddMapGroup(string id, bool singleTile, string mapDefinitionId, double centerX, double centerY, double scale)
        {
            var map = AddMapGroup(id, singleTile, mapDefinitionId);
            map.InitialView = new MapViewType() { CenterX = centerX, CenterY = centerY, Scale = scale };

            return map;
        }

        [XmlIgnore]
        public OSGeo.MapGuide.MaestroAPI.IServerConnection CurrentConnection
        {
            get;
            set;
        }

        private string _resId;

        [XmlIgnore]
        public string ResourceID
        {
            get
            {
                return _resId;
            }
            set
            {
                if (!ResourceIdentifier.Validate(value))
                    throw new InvalidOperationException("Not a valid resource identifier"); //LOCALIZE

                var res = new ResourceIdentifier(value);
                if (res.Extension != ResourceTypes.ApplicationDefinition.ToString())
                    throw new InvalidOperationException("Invalid resource identifier for this type of object: " + res.Extension); //LOCALIZE

                _resId = value;
                this.OnPropertyChanged("ResourceID");
            }
        }

        [XmlIgnore]
        public ResourceTypes ResourceType
        {
            get
            {
                return ResourceTypes.ApplicationDefinition;
            }
        }

        [XmlIgnore]
        public Version ResourceVersion
        {
            get
            {
                return RES_VERSION;
            }
        }

        object ICloneable.Clone()
        {
            return this.Clone();
        }

        [XmlAttribute("noNamespaceSchemaLocation", Namespace = "http://www.w3.org/2001/XMLSchema-instance")]
        public string ValidatingSchema 
        { 
            get { return "ApplicationDefinition-1.0.0.xsd"; }
            set { }
        }

        [XmlIgnore]
        public bool IsStronglyTyped
        {
            get { return true; }
        }

        IMapGroup IMapSet.GetGroupAt(int index)
        {
            return this.MapSet[index];
        }

        IWidget IApplicationDefinition.CreateWidget(string name, IWidgetInfo widgetInfo)
        {
            var wparams = widgetInfo.Parameters;

            IWidget widget = null;

            if (widgetInfo.StandardUi)
            {
                widget = new UiWidgetType()
                {
                    Disabled = "false",
                    Extension = new CustomContentType() { Any = new XmlElement[0] },
                    ImageClass = widgetInfo.ImageClass,
                    ImageUrl = widgetInfo.ImageUrl,
                    Label = widgetInfo.Label,
                    Location = widgetInfo.Location ?? string.Empty, //Required to satisfy content model
                    Name = name,
                    StatusText = widgetInfo.StatusText ?? string.Empty, //Required to satisfy content model
                    Tooltip = widgetInfo.Tooltip,
                    Type = widgetInfo.Type
                };
            }
            else
            {
                widget = new WidgetType()
                {
                    Extension = new CustomContentType() { Any = new XmlElement[0] },
                    Location = widgetInfo.Location ?? string.Empty, //Required to satisfy content model
                    Name = name,
                    Type = widgetInfo.Type,
                };
            }

            NameValueCollection extProperties = new NameValueCollection();

            foreach (var wp in widgetInfo.Parameters)
            {
                if (!string.IsNullOrEmpty(wp.DefaultValue))
                {
                    extProperties.Add(wp.Name, wp.DefaultValue);
                }
                else
                {
                    extProperties.Add(wp.Name, string.Empty);
                }
            }

            widget.SetAllValues(extProperties);

            return widget;
        }

        private XmlElement CreateExtensionElement(IWidgetParameter wparam)
        {
            var el = AppDefDocument.Instance.CreateElement(wparam.Name);
            el.InnerText = wparam.DefaultValue;
            return el;
        }

        IUIItemContainer IApplicationDefinition.CreateContainer(string name, IApplicationDefinitionContainerInfo containerInfo)
        {
            return new UiItemContainerType()
            {
                Name = name,
                Type = containerInfo.Type,
                Item = new System.ComponentModel.BindingList<UiItemType>(),
                Extension = new CustomContentType() { Any = new XmlElement[0] }
            };
        }

        IMapSet IApplicationDefinition.MapSet
        {
            get { return this; }
        }

        IExtension IExtensibleElement.Extension
        {
            get
            {
                return this.Extension;
            }
        }

        IEnumerable<IMapGroup> IMapSet.MapGroups
        {
            get 
            {
                foreach (var grp in this.MapSet)
                {
                    yield return grp;
                }
            }
        }

        int IMapSet.MapGroupCount
        {
            get { return this.MapSet.Count; }
        }

        void IMapSet.AddGroup(IMapGroup group)
        {
            var grp = group as MapGroupType;
            if (grp != null)
            {
                this.MapSet.Add(grp);
                OnPropertyChanged("MapSet");
            }
        }

        void IMapSet.RemoveGroup(IMapGroup group)
        {
            var grp = group as MapGroupType;
            if (grp != null)
            {
                this.MapSet.Remove(grp);
                OnPropertyChanged("MapSet");
            }
        }

        public IEnumerable<IWidgetSet> WidgetSets
        {
            get 
            {
                foreach (var wset in this.WidgetSet)
                {
                    yield return wset;
                }
            }
        }

        public void AddWidgetSet(IWidgetSet set)
        {
            var wset = set as WidgetSetType;
            if (wset != null)
            {
                this.WidgetSet.Add(wset);
                OnPropertyChanged("WidgetSet");
            }
        }

        public void RemoveWidgetSet(IWidgetSet set)
        {
            var wset = set as WidgetSetType;
            if (wset != null)
            {
                this.WidgetSet.Remove(wset);
                OnPropertyChanged("WidgetSet");
            }
        }

        public IWidgetSet CreateWidgetSet(IMapWidget mapWidget)
        {
            return new WidgetSetType()
            {
                Widget = new System.ComponentModel.BindingList<WidgetType>(),
                Container = new System.ComponentModel.BindingList<ContainerType>(),
                MapWidget = (MapWidgetType)mapWidget
            };
        }
    }

    partial class UiItemContainerType : IUIItemContainer
    {
        public int ItemCount
        {
            get { return this.Item.Count; }
        }

        public IEnumerable<IUIItem> Items
        {
            get
            {
                foreach (var it in this.Item)
                {
                    yield return it;
                }
            }
        }

        public bool MoveUp(IUIItem item)
        {
            var idx = GetIndex(item);
            if (idx > 0)
            {
                RemoveItem(item);
                Insert(item, idx - 1);
                return true;
            }
            return false;
        }

        public bool MoveDown(IUIItem item)
        {
            var idx = GetIndex(item);
            if (idx < this.Item.Count - 1)
            {
                RemoveItem(item);
                Insert(item, idx + 1);
                return true;
            }
            return false;
        }

        public int GetIndex(IUIItem item)
        {
            var it = item as UiItemType;
            if (it != null)
            {
                return this.Item.IndexOf(it);
            }
            return -1;
        }

        public void Insert(IUIItem item, int index)
        {
            var it = item as UiItemType;
            if (it != null)
            {
                this.Item.Insert(index, it);
                it.Parent = this;
                OnPropertyChanged("Item");
            }
        }

        public void AddItem(IUIItem item)
        {
            var it = item as UiItemType;
            if (it != null)
            {
                this.Item.Add(it);
                it.Parent = this;
                OnPropertyChanged("Item");
            }
        }

        public void RemoveItem(IUIItem item)
        {
            var it = item as UiItemType;
            if (it != null)
            {
                this.Item.Remove(it);
                it.Parent = null;
                OnPropertyChanged("Item");
            }
        }
    }

    partial class FlyoutItemType : IFlyoutItem
    {
        public int ItemCount
        {
            get { return this.Item.Count; }
        }

        public IEnumerable<IUIItem> Items
        {
            get 
            {
                foreach (var it in this.Item)
                {
                    yield return it;
                }
            }
        }

        public bool MoveUp(IUIItem item)
        {
            var idx = GetIndex(item);
            if (idx > 0)
            {
                RemoveItem(item);
                Insert(item, idx - 1);
                return true;
            }
            return false;
        }

        public bool MoveDown(IUIItem item)
        {
            var idx = GetIndex(item);
            if (idx < this.Item.Count - 1)
            {
                RemoveItem(item);
                Insert(item, idx + 1);
                return true;
            }
            return false;
        }

        public int GetIndex(IUIItem item)
        {
            var it = item as UiItemType;
            if (it != null)
            {
                return this.Item.IndexOf(it);
            }
            return -1;
        }

        public void Insert(IUIItem item, int index)
        {
            var it = item as UiItemType;
            if (it != null)
            {
                this.Item.Insert(index, it);
                it.Parent = this;
                OnPropertyChanged("Item");
            }
        }

        public void AddItem(IUIItem item)
        {
            var it = item as UiItemType;
            if (it != null)
            {
                this.Item.Add(it);
                it.Parent = this;
                OnPropertyChanged("Item");
            }
        }

        public void RemoveItem(IUIItem item)
        {
            var it = item as UiItemType;
            if (it != null)
            {
                this.Item.Remove(it);
                it.Parent = null;
                OnPropertyChanged("Item");
            }
        }
    }

    partial class UiItemType : IUIItem 
    {
        [XmlIgnore]
        public IMenu Parent { get; set; }
    }

    partial class WidgetItemType : IWidgetItem { }

    partial class SeparatorItemType : ISeparator { }

    partial class WidgetSetType : IWidgetSet
    {
        public IEnumerable<IWidgetContainer> Containers
        {
            get 
            {
                foreach (var cnt in this.Container)
                {
                    yield return cnt;
                }
            }
        }

        public int ContainerCount
        {
            get { return this.Container.Count; }
        }

        public void AddContainer(IWidgetContainer container)
        {
            var cnt = container as ContainerType;
            if (cnt != null)
            {
                this.Container.Add(cnt);
                OnPropertyChanged("Container");
            }
        }

        public void RemoveContainer(IWidgetContainer container)
        {
            var cnt = container as ContainerType;
            if (cnt != null)
            {
                this.Container.Remove(cnt);
                OnPropertyChanged("Container");
            }
        }

        IMapWidget IWidgetSet.MapWidget
        {
            get 
            {
                return this.MapWidget;  
            }
        }


        public int WidgetCount
        {
            get { return this.Widget.Count; }
        }

        public IEnumerable<IWidget> Widgets
        {
            get 
            {
                foreach (var wgt in this.Widget)
                {
                    yield return wgt;
                }
            }
        }

        public void AddWidget(IWidget widget)
        {
            var wgt = widget as WidgetType;
            if (wgt != null)
            {
                this.Widget.Add(wgt);
            }
        }

        public void RemoveWidget(IWidget widget)
        {
            var wgt = widget as WidgetType;
            if (wgt != null)
            {
                this.Widget.Remove(wgt);
            }
        }
    }

    partial class ContainerType : IWidgetContainer
    {
        IExtension IExtensibleElement.Extension
        {
            get { return this.Extension; }
        }
    }

    partial class WidgetType : IWidget 
    {
        IExtension IExtensibleElement.Extension
        {
            get
            {
                return this.Extension;
            }
        }

        string IWidget.ToXml()
        {
            return this.Serialize();
        }
    }

    partial class UiWidgetType : IUIWidget 
    {
        IUIWidget IUIWidget.Clone()
        {
            var clone = new UiWidgetType()
            {
                Disabled = this.Disabled,
                ImageClass = this.ImageClass,
                ImageUrl = this.ImageUrl,
                Label = this.Label,
                Location = this.Location,
                Name = this.Name,
                StatusText = this.StatusText,
                Tooltip = this.Tooltip,
                Type = this.Type,
                Extension = new CustomContentType() { Any = new XmlElement[0] }
            };

            var ext = this.GetAllValues();
            if (ext.Count > 0)
                clone.SetAllValues(ext);

            return clone;
        }
    }

    partial class MapWidgetType : IMapWidget { }

    partial class MapViewType : IMapView { }

    partial class MapGroupType : IMapGroup
    {
        IMap IMapGroup.CreateCmsMapEntry(string type, bool singleTile, string name, string olType)
        {
            var el = AppDefDocument.Instance.CreateElement("Options");
            var n = AppDefDocument.Instance.CreateElement("name");
            var t = AppDefDocument.Instance.CreateElement("type");

            n.InnerText = name;
            t.InnerText = olType;
            el.AppendChild(n);
            el.AppendChild(t);

            return new MapType()
            {
                Extension = new CustomContentType()
                {
                    Any = new XmlElement[] { el }
                },
                SingleTile = "true",
                Type = type
            };
        }

        IMapView IMapGroup.CreateInitialView(double x, double y, double scale)
        {
            return new MapViewType() { CenterX = x, CenterY = y, Scale = scale };
        }

        int IMapGroup.MapCount
        {
            get
            {
                return this.Map.Count;
            }
        }

        IMap IMapGroup.GetMapAt(int index)
        {
            return (IMap)this.Map[index];
        }

        IMapView IMapGroup.InitialView
        {
            get
            {
                return this.InitialView;
            }
            set
            {
                this.InitialView = (MapViewType)value;
            }
        }

        IEnumerable<IMap> IMapGroup.Map
        {
            get 
            {
                foreach (var map in this.Map)
                {
                    yield return map;
                }
            }
        }

        public void AddMap(IMap map)
        {
            var m = map as MapType;
            if (m != null)
            {
                this.Map.Add(m);
                OnPropertyChanged("Map");
            }
        }

        public void RemoveMap(IMap map)
        {
            var m = map as MapType;
            if (m != null)
            {
                this.Map.Remove(m);
                OnPropertyChanged("Map");
            }
        }

        IExtension IExtensibleElement.Extension
        {
            get
            {
                return this.Extension;
            }
        }
    }

    public class MapGuideOverlayOptions : IMapGuideOverlayOptions
    {

        public bool IsBaseLayer
        {
            get;
            set;
        }

        public bool UseOverlay
        {
            get;
            set;
        }

        public string Projection
        {
            get;
            set;
        }
    }

    partial class MapType : IMap
    {
        IExtension IExtensibleElement.Extension
        {
            get
            {
                return this.Extension;
            }
        }

        bool IMap.SingleTile
        {
            get
            {
                bool result;
                if (bool.TryParse(this.SingleTile, out result))
                    return result;

                return default(bool);
            }
            set
            {
                this.SingleTile = value.ToString();
            }
        }

        public ICmsMapOptions CreateOptions(string name, string type)
        {
            return new MapOptions() { Name = name, Type = type };
        }

        [XmlIgnore]
        public ICmsMapOptions CmsMapOptions
        {
            get
            {
                if (this.extensionField.Any.Length > 0)
                {
                     var els = new List<XmlElement>(this.extensionField.Any);
                     for (int i = 0; i < els.Count; i++)
                     {
                         if (els[i].Name.Equals("Options"))
                         {
                             try
                             {
                                 var n = els[i]["name"];
                                 var t = els[i]["type"];

                                 if (n != null && t != null)
                                     return new MapOptions() { Name = n.InnerText, Type = t.InnerText };
                             }
                             catch
                             {
                                 return null;
                             }
                         }
                     }
                     return null;
                }
                else
                {
                    return null;
                }
            }
            set
            {
                if (this.Type.Equals("MapGuide"))
                    throw new InvalidOperationException("CMS options are only available for non-MapGuide maps");

                if (value == null)
                {
                    if (this.extensionField.Any.Length > 0)
                    {
                        var els = new List<XmlElement>(this.extensionField.Any);
                        for (int i = 0; i < els.Count; i++)
                        {
                            if (els[i].Name.Equals("Options"))
                            {
                                els.RemoveAt(i);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    XmlElement el = null;
                    if (this.Extension.Any.Length == 0)
                    {
                        el = AppDefDocument.Instance.CreateElement("Options");

                        var n = AppDefDocument.Instance.CreateElement("name");
                        var t = AppDefDocument.Instance.CreateElement("type");

                        n.InnerText = value.Name;
                        t.InnerText = value.Type;

                        el.AppendChild(n);
                        el.AppendChild(t);

                        this.Extension.Any = new XmlElement[] { el };
                    }
                    else
                    {
                        var els = new List<XmlElement>(this.extensionField.Any);
                        for (int i = 0; i < els.Count; i++)
                        {
                            if (els[i].Name.Equals("Options"))
                            {
                                els[i].RemoveAll();

                                var n = AppDefDocument.Instance.CreateElement("name");
                                var t = AppDefDocument.Instance.CreateElement("type");

                                n.InnerText = value.Name;
                                t.InnerText = value.Type;

                                els[i].AppendChild(n);
                                els[i].AppendChild(t);
                                break;
                            }
                        }
                    }
                }
            }
        }

        [XmlIgnore]
        public IMapGuideOverlayOptions OverlayOptions
        {
            /*
            From Fusion Wiki:
            
            Note that the commercial layer must be the base layer so any overlays must 
            have the isBaseLayer: false option set. The projection tag is needed so 
            that OpenLayers will initialize the layer in the same projection as the base 
            map.
            
            <Options>
               <isBaseLayer>false</isBaseLayer>
               <useOverlay>true</useOverlay>
               <projection>EPSG:900913</projection>
            </Options>
             */
            get
            {
                if (this.extensionField.Any.Length > 0)
                {
                    var els = new List<XmlElement>(this.extensionField.Any);
                    for (int i = 0; i < els.Count; i++)
                    {
                        if (els[i].Name.Equals("Options"))
                        {
                            try
                            {
                                var n = els[i]["isBaseLayer"];
                                var t = els[i]["useOverlay"];
                                var p = els[i]["projection"];

                                if (n != null && t != null)
                                    return CreateOverlayOptions(Convert.ToBoolean(n.InnerText), Convert.ToBoolean(t.InnerText), p.InnerText);
                            }
                            catch
                            {
                                return null;
                            }
                        }
                    }
                    return null;
                }
                else
                {
                    return null;
                }
            }
            set
            {
                if (!this.Type.Equals("MapGuide"))
                    throw new InvalidOperationException("Overlay options are only applicable to MapGuide maps");

                if (value == null)
                {
                    if (this.extensionField.Any.Length > 0)
                    {
                        var els = new List<XmlElement>(this.extensionField.Any);
                        for (int i = 0; i < els.Count; i++)
                        {
                            if (els[i].Name.Equals("Options"))
                            {
                                els.RemoveAt(i);
                                this.extensionField.Any = els.ToArray();
                                break;
                            }
                        }
                    }
                }
                else
                {
                    XmlElement el = null;
                    if (this.Extension.Any.Length == 0)
                    {
                        el = AppDefDocument.Instance.CreateElement("Options");

                        this.Extension.Any = new XmlElement[] { el };
                    }
                    else
                    {
                        var els = new List<XmlElement>(this.extensionField.Any);
                        for (int i = 0; i < els.Count; i++)
                        {
                            if (els[i].Name.Equals("Options"))
                            {
                                el = els[i];
                                break;
                            }
                        }

                        if (el == null)
                        {
                            el = AppDefDocument.Instance.CreateElement("Options");
                            els.Add(el);
                            this.Extension.Any = els.ToArray();
                        }
                    }

                    el.RemoveAll();

                    var n = AppDefDocument.Instance.CreateElement("isBaseLayer");
                    var t = AppDefDocument.Instance.CreateElement("useOverlay");
                    var p = AppDefDocument.Instance.CreateElement("projection");

                    n.InnerText = value.IsBaseLayer.ToString().ToLower();
                    t.InnerText = value.UseOverlay.ToString().ToLower();
                    p.InnerText = value.Projection;

                    el.AppendChild(n);
                    el.AppendChild(t);
                    el.AppendChild(p);
                }
            }
        }

        public IMapGuideOverlayOptions CreateOverlayOptions(bool isBaseLayer, bool useOverlay, string projection)
        {
            return new MapGuideOverlayOptions() { IsBaseLayer = isBaseLayer, UseOverlay = useOverlay, Projection = projection };
        }
    }

    public class MapOptions : ICmsMapOptions
    {
        public string Name { get; set; }

        public string Type { get; set; }
    }
}
