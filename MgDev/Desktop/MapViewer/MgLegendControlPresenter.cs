using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Windows.Forms.VisualStyles;

namespace OSGeo.MapGuide.Viewer
{
    using Legend.Model;
    using System.Xml;
    using System.IO;

    interface ILegendView
    {
        ContextMenuStrip LayerContextMenu { get; }
        ContextMenuStrip GroupContextMenu { get; }
        void AddLegendIcon(string id, Image icon);
        int ThemeCompressionLimit { get; }
        void OnRequestRefresh();
    }

    class MgLegendControlPresenter
    {
        const string IMG_BROKEN = "lc_broken";
        const string IMG_DWF = "lc_dwf";
        const string IMG_GROUP = "lc_group";
        const string IMG_RASTER = "lc_raster";
        const string IMG_SELECT = "lc_select";
        const string IMG_THEME = "lc_theme";
        const string IMG_UNSELECT = "lc_unselect";
        const string IMG_OTHER = "icon_etc";

        private MgResourceService _resSvc;
        private MgMapViewerProvider _provider;
        private MgMapBase _map;

        private Image _selectableIcon;
        private Image _unselectableIcon;

        private ILegendView _legend;

        //TODO: Coalesce this all into a single object. ie. Merge into the respective metadata classes.
        private Dictionary<string, bool> _initialLayerSelectabilityState = new Dictionary<string, bool>();
        private Dictionary<string, MgLayerBase> _layers = new Dictionary<string, MgLayerBase>();
        private Dictionary<string, MgLayerGroup> _groups = new Dictionary<string, MgLayerGroup>();
        private Dictionary<string, string> _layerDefinitionContents = new Dictionary<string, string>();

        public MgLegendControlPresenter(ILegendView legend, MgMapViewerProvider provider)
        {
            _legend = legend;
            _provider = provider;
            _map = _provider.GetMap();
            InitInitialSelectabilityStates();
            _resSvc = (MgResourceService)_provider.CreateService(MgServiceType.ResourceService);
            _selectableIcon = Properties.Resources.lc_select;
            _unselectableIcon = Properties.Resources.lc_unselect;
        }

        private void InitInitialSelectabilityStates()
        {
            if (_map != null)
            {
                _initialLayerSelectabilityState.Clear();
                var layers = _map.GetLayers();
                for (int i = 0; i < layers.GetCount(); i++)
                {
                    var layer = layers.GetItem(i);
                    _initialLayerSelectabilityState[layer.GetObjectId()] = layer.Selectable;
                }
            }
        }

        private bool HasVisibleParent(MgLayerGroup grp)
        {
            var current = grp.Group;
            if (current != null)
                return current.IsVisible();
            return true;
        }

        private bool HasVisibleParent(MgLayerBase layer)
        {
            var current = layer.Group;
            if (current != null)
                return current.IsVisible();

            return true;
        }

        private TreeNode CreateLayerNode(MgLayerBase layer)
        {
            var node = new TreeNode();
            node.Name = layer.GetObjectId();
            node.Text = layer.GetLegendLabel();
            node.Checked = layer.GetVisible();
            node.ContextMenuStrip = _legend.LayerContextMenu;
            var lt = layer.GetLayerType();
            var fsId = layer.GetFeatureSourceId();

            if (fsId.EndsWith("DrawingSource"))
            {
                node.SelectedImageKey = node.ImageKey = IMG_DWF;
                //If not in the dictionary, assume it is a dynamically added layer
                bool bInitiallySelectable = _initialLayerSelectabilityState.ContainsKey(layer.GetObjectId()) ? _initialLayerSelectabilityState[layer.GetObjectId()] : layer.Selectable;
                node.Tag = new LayerNodeMetadata(layer, bInitiallySelectable);
                node.ToolTipText = string.Format(Properties.Resources.DrawingLayerTooltip, Environment.NewLine, layer.Name, layer.FeatureSourceId);
            }
            else
            {
                string layerData = null;
                var ldfId = layer.LayerDefinition;
                if (_layerDefinitionContents.ContainsKey(ldfId.ToString()))
                    layerData = _layerDefinitionContents[ldfId.ToString()];

                if (layerData == null)
                    return null;

                XmlDocument doc = new XmlDocument();
                doc.LoadXml(layerData);
                int type = 0;
                XmlNodeList scaleRanges = doc.GetElementsByTagName("VectorScaleRange");
                if (scaleRanges.Count == 0)
                {
                    scaleRanges = doc.GetElementsByTagName("GridScaleRange");
                    if (scaleRanges.Count == 0)
                    {
                        scaleRanges = doc.GetElementsByTagName("DrawingLayerDefinition");
                        if (scaleRanges.Count == 0)
                            return null;
                        type = 2;
                    }
                    else
                        type = 1;
                }

                String[] typeStyles = new String[] { "PointTypeStyle", "LineTypeStyle", "AreaTypeStyle", "CompositeTypeStyle" };
                String[] ruleNames = new String[] { "PointRule", "LineRule", "AreaRule", "CompositeRule" };

                try
                {
                    MgByteReader layerIcon = _provider.GenerateLegendImage(layer.LayerDefinition,
                                                                            _map.ViewScale,
                                                                            16,
                                                                            16,
                                                                            "PNG",
                                                                            -1,
                                                                            -1);
                    if (layerIcon != null)
                    {
                        try
                        {
                            byte[] b = new byte[layerIcon.GetLength()];
                            layerIcon.Read(b, b.Length);
                            using (var ms = new MemoryStream(b))
                            {
                                string id = Guid.NewGuid().ToString();
                                Image img = Image.FromStream(ms);

                                _legend.AddLegendIcon(id, img);
                                node.SelectedImageKey = node.ImageKey = id;
                                //If not in the dictionary, assume it is a dynamically added layer
                                bool bInitiallySelectable = _initialLayerSelectabilityState.ContainsKey(layer.GetObjectId()) ? _initialLayerSelectabilityState[layer.GetObjectId()] : layer.Selectable;
                                node.Tag = new LayerNodeMetadata(layer, bInitiallySelectable)
                                {
                                    ThemeIcon = img
                                };
                                node.ToolTipText = string.Format(Properties.Resources.DefaultLayerTooltip, Environment.NewLine, layer.Name, layer.FeatureSourceId, layer.FeatureClassName);
                            }
                        }
                        finally
                        {
                            layerIcon.Dispose();
                        }
                    }
                    else
                    {
                        node.SelectedImageKey = node.ImageKey = IMG_BROKEN;
                    }
                }
                catch
                {
                    node.SelectedImageKey = node.ImageKey = IMG_BROKEN;
                }

                for (int sc = 0; sc < scaleRanges.Count; sc++)
                {
                    XmlElement scaleRange = (XmlElement)scaleRanges[sc];
                    XmlNodeList minElt = scaleRange.GetElementsByTagName("MinScale");
                    XmlNodeList maxElt = scaleRange.GetElementsByTagName("MaxScale");
                    String minScale, maxScale;
                    minScale = "0";
                    maxScale = "1000000000000.0";   // as MDF's VectorScaleRange::MAX_MAP_SCALE
                    if (minElt.Count > 0)
                        minScale = minElt[0].ChildNodes[0].Value;
                    if (maxElt.Count > 0)
                        maxScale = maxElt[0].ChildNodes[0].Value;

                    if (type != 0)
                        break;

                    for (int geomType = 0; geomType < typeStyles.Length; geomType++)
                    {
                        int catIndex = 0;
                        XmlNodeList typeStyle = scaleRange.GetElementsByTagName(typeStyles[geomType]);
                        for (int st = 0; st < typeStyle.Count; st++)
                        {
                            // We will check if this typestyle is going to be shown in the legend
                            XmlNodeList showInLegend = ((XmlElement)typeStyle[st]).GetElementsByTagName("ShowInLegend");
                            if (showInLegend.Count > 0)
                                if (!bool.Parse(showInLegend[0].ChildNodes[0].Value))
                                    continue;   // This typestyle does not need to be shown in the legend

                            XmlNodeList rules = ((XmlElement)typeStyle[st]).GetElementsByTagName(ruleNames[geomType]);
                            if (rules.Count > 1)
                            {
                                node.SelectedImageKey = node.ImageKey = IMG_THEME;
                                var layerMeta = node.Tag as LayerNodeMetadata;
                                if (layerMeta != null)
                                {
                                    layerMeta.ThemeIcon = Properties.Resources.lc_theme;
                                    node.ToolTipText = string.Format(Properties.Resources.ThemedLayerTooltip, Environment.NewLine, layer.Name, layer.FeatureSourceId, layer.FeatureClassName, rules.Count);
                                }
                                if (_legend.ThemeCompressionLimit > 0 && rules.Count > _legend.ThemeCompressionLimit)
                                {
                                    AddThemeRuleNode(layer, node, geomType, 0, rules, 0);
                                    node.Nodes.Add(CreateCompressedThemeNode(rules.Count - 2));
                                    AddThemeRuleNode(layer, node, geomType, rules.Count - 1, rules, rules.Count - 1);
                                }
                                else
                                {
                                    for (int r = 0; r < rules.Count; r++)
                                    {
                                        AddThemeRuleNode(layer, node, geomType, catIndex++, rules, r);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            return node;
        }

        private void AddThemeRuleNode(MgLayerBase layer, TreeNode node, int geomType, int catIndex, XmlNodeList rules, int r)
        {
            XmlElement rule = (XmlElement)rules[r];
            XmlNodeList label = rule.GetElementsByTagName("LegendLabel");
            XmlNodeList filter = rule.GetElementsByTagName("Filter");

            String labelText = "";
            if (label != null && label.Count > 0 && label[0].ChildNodes.Count > 0)
                labelText = label[0].ChildNodes[0].Value;
            //String filterText = "";
            //if (filter != null && filter.Count > 0 && filter[0].ChildNodes.Count > 0)
            //    filterText = filter[0].ChildNodes[0].Value;

            var child = CreateThemeRuleNode(layer.LayerDefinition, _map.ViewScale, labelText, (geomType + 1), catIndex);
            node.Nodes.Add(child);
        }

        private TreeNode CreateCompressedThemeNode(int count)
        {
            TreeNode node = new TreeNode();
            node.Text = (count + " other styles");
            node.ImageKey = node.SelectedImageKey = IMG_OTHER;
            node.Tag = new LayerNodeMetadata(null, false)
            {
                IsBaseLayer = false,
                ThemeIcon = Properties.Resources.icon_etc,
                IsThemeRule = true
            };
            return node;
        }

        private TreeNode CreateThemeRuleNode(MgResourceIdentifier layerDefId, double viewScale, string labelText, int geomType, int categoryIndex)
        {
            MgByteReader icon = _provider.GenerateLegendImage(layerDefId,
                                                              viewScale,
                                                              16,
                                                              16,
                                                              "PNG",
                                                              geomType,
                                                              categoryIndex);
            TreeNode node = new TreeNode();
            node.Text = labelText;
            if (icon != null)
            {
                try
                {

                    byte[] b = new byte[icon.GetLength()];
                    icon.Read(b, b.Length);
                    var tag = new LayerNodeMetadata(null, false)
                    {
                        IsBaseLayer = false,
                        IsThemeRule = true
                    };
                    using (var ms = new MemoryStream(b))
                    {
                        string id = Guid.NewGuid().ToString();
                        tag.ThemeIcon = Image.FromStream(ms);
                    }
                    node.Tag = tag;
                }
                finally
                {
                    icon.Dispose();
                }
            }
            return node;
        }

        private TreeNode CreateGroupNode(MgLayerGroup group)
        {
            var node = new TreeNode();
            node.Name = group.GetObjectId();
            node.Text = group.GetLegendLabel();
            node.Checked = group.IsVisible();
            node.SelectedImageKey = node.ImageKey = IMG_GROUP;
            node.Tag = new GroupNodeMetadata(group);
            node.ContextMenuStrip = _legend.GroupContextMenu;
            return node;
        }

        public TreeNode[] CreateNodes()
        {
            List<TreeNode> output = new List<TreeNode>();
            var nodesById = new Dictionary<string, TreeNode>();

            var scale = _map.ViewScale;
            var groups = _map.GetLayerGroups();
            var layers = _map.GetLayers();

            _layerDefinitionContents.Clear();
            _layers.Clear();
            _groups.Clear();

            //Process groups first
            List<MgLayerGroup> remainingNodes = new List<MgLayerGroup>();
            for (int i = 0; i < groups.GetCount(); i++)
            {
                var group = groups.GetItem(i);
                _groups.Add(group.GetObjectId(), group);
                if (!group.GetDisplayInLegend())
                    continue;

                //Add ones without parents first.
                if (group.Group != null)
                {
                    remainingNodes.Add(group);
                }
                else
                {
                    var node = CreateGroupNode(group);
                    output.Add(node);
                    nodesById.Add(group.GetObjectId(), node);
                }

                while (remainingNodes.Count > 0)
                {
                    List<MgLayerGroup> toRemove = new List<MgLayerGroup>();
                    for (int j = 0; j < remainingNodes.Count; j++)
                    {
                        var parentId = remainingNodes[j].Group.GetObjectId();
                        if (nodesById.ContainsKey(parentId))
                        {
                            var node = CreateGroupNode(remainingNodes[j]);
                            nodesById[parentId].Nodes.Add(node);
                            toRemove.Add(remainingNodes[j]);
                        }
                    }
                    //Whittle down this list
                    if (toRemove.Count > 0)
                    {
                        foreach (var g in toRemove)
                        {
                            remainingNodes.Remove(g);
                        }
                    }
                }
            }

            //Now process layers
            for (int i = 0; i < layers.GetCount(); i++)
            {
                var lyr = layers.GetItem(i);
                var ldfId = lyr.LayerDefinition;

                if (!_layerDefinitionContents.ContainsKey(ldfId.ToString()))
                {
                    _layerDefinitionContents[ldfId.ToString()] = string.Empty;
                }
            }

            //Collect all resource contents in a batch
            MgStringCollection layerIds = new MgStringCollection();
            foreach (var lid in _layerDefinitionContents.Keys)
            {
                layerIds.Add(lid);
            }

            MgStringCollection layerContents = _resSvc.GetResourceContents(layerIds, null);
            for (int i = 0; i < layerIds.GetCount(); i++)
            {
                string lid = layerIds.GetItem(i);
                _layerDefinitionContents[lid] = layerContents.GetItem(i);
            }

            List<MgLayerBase> remainingLayers = new List<MgLayerBase>();
            for (int i = 0; i < layers.GetCount(); i++)
            {
                var layer = layers.GetItem(i);
                _layers.Add(layer.GetObjectId(), layer);

                bool display = layer.DisplayInLegend;
                bool visible = _provider.IsLayerPotentiallyVisibleAtScale(layer, false);
                if (!display)
                    continue;

                if (!visible)
                    continue;

                //Add ones without parents first.
                if (layer.Group != null)
                {
                    remainingLayers.Add(layer);
                }
                else
                {
                    var node = CreateLayerNode(layer);
                    if (node != null)
                    {
                        output.Add(node);
                        nodesById.Add(layer.GetObjectId(), node);
                        if (layer.ExpandInLegend)
                            node.Expand();
                    }
                }

                while (remainingLayers.Count > 0)
                {
                    List<MgLayerBase> toRemove = new List<MgLayerBase>();
                    for (int j = 0; j < remainingLayers.Count; j++)
                    {
                        var parentId = remainingLayers[j].Group.GetObjectId();
                        if (nodesById.ContainsKey(parentId))
                        {
                            var node = CreateLayerNode(remainingLayers[j]);
                            if (node != null)
                            {
                                nodesById[parentId].Nodes.Add(node);
                                if (remainingLayers[j].ExpandInLegend)
                                    node.Expand();
                            }
                            toRemove.Add(remainingLayers[j]);
                        }
                    }
                    //Whittle down this list
                    if (toRemove.Count > 0)
                    {
                        foreach (var g in toRemove)
                        {
                            remainingLayers.Remove(g);
                        }
                    }
                }
            }

            //Now expand any relevant groups
            for (int i = 0; i < groups.GetCount(); i++)
            {
                var group = groups.GetItem(i);
                if (group.ExpandInLegend)
                {
                    var groupId = group.GetObjectId();
                    if (nodesById.ContainsKey(groupId))
                    {
                        nodesById[groupId].Expand();
                    }
                }
            }

            return output.ToArray();
        }

        private static bool IsThemeLayerNode(TreeNode node)
        {
            var meta = node.Tag as LayerNodeMetadata;
            if (meta != null)
                return meta.ThemeIcon != null || meta.IsBaseLayer;

            return false;
        }

        private static bool IsLayerNode(TreeNode node)
        {
            var meta = node.Tag as LayerNodeMetadata;
            return meta != null;
        }

        internal void DrawNode(DrawTreeNodeEventArgs e, bool showPlusMinus, Font font)
        {
            if (IsLayerNode(e.Node) && !e.Bounds.IsEmpty)
            {
                Color backColor, foreColor;

                //For some reason, the default bounds are way off from what you would
                //expect it to be. So we apply this offset for any text/image draw operations
                int xoffset = -36;
                if (showPlusMinus && e.Node.Nodes.Count > 0)
                {
                    // Use the VisualStyles renderer to use the proper OS-defined glyphs
                    Rectangle glyphRect = new Rectangle(e.Node.Bounds.X - 52, e.Node.Bounds.Y, 16, 16);
                    if (Application.RenderWithVisualStyles)
                    {
                        VisualStyleElement element = (e.Node.IsExpanded) ?
                            VisualStyleElement.TreeView.Glyph.Opened : VisualStyleElement.TreeView.Glyph.Closed;

                        VisualStyleRenderer renderer = new VisualStyleRenderer(element);
                        renderer.DrawBackground(e.Graphics, glyphRect);
                    }
                    else //Visual Styles disabled, fallback to drawing the +/- using geometric primitives
                    {
                        int h = 8;
                        int w = 8;
                        int x = glyphRect.X;
                        int y = glyphRect.Y + (glyphRect.Height / 2) - 4;

                        //Draw the -
                        e.Graphics.DrawRectangle(new Pen(SystemBrushes.ControlDark), x, y, w, h);
                        e.Graphics.FillRectangle(new SolidBrush(Color.White), x + 1, y + 1, w - 1, h - 1);
                        e.Graphics.DrawLine(new Pen(new SolidBrush(Color.Black)), x + 2, y + 4, x + w - 2, y + 4);

                        //Draw the |
                        if (!e.Node.IsExpanded)
                            e.Graphics.DrawLine(new Pen(new SolidBrush(Color.Black)), x + 4, y + 2, x + 4, y + h - 2);
                    }
                }

                if ((e.State & TreeNodeStates.Selected) == TreeNodeStates.Selected)
                {
                    backColor = SystemColors.Highlight;
                    foreColor = SystemColors.HighlightText;
                }
                else if ((e.State & TreeNodeStates.Hot) == TreeNodeStates.Hot)
                {
                    backColor = SystemColors.HotTrack;
                    foreColor = SystemColors.HighlightText;
                }
                else
                {
                    backColor = e.Node.BackColor;
                    foreColor = e.Node.ForeColor;
                }

                var tag = e.Node.Tag as LayerNodeMetadata;
                var checkBoxOffset = xoffset;
                var selectabilityOffset = xoffset + 16;
                var iconOffsetNoSelect = xoffset + 16;
                if (tag != null && tag.IsThemeRule) //No checkbox for theme rule nodes
                {
                    selectabilityOffset = xoffset;
                    iconOffsetNoSelect = xoffset;
                }
                var iconOffset = selectabilityOffset + 20;
                var textOffset = iconOffset + 20;
                var textOffsetNoSelect = iconOffsetNoSelect + 20;

                //Uncomment if you need to "see" the bounds of the node
                //e.Graphics.DrawRectangle(Pens.Black, e.Node.Bounds);

                if (tag != null && !tag.IsThemeRule) //No checkbox for theme rule nodes
                {
                    if (Application.RenderWithVisualStyles)
                    {
                        CheckBoxRenderer.DrawCheckBox(
                            e.Graphics,
                            new Point(e.Node.Bounds.X + checkBoxOffset, e.Node.Bounds.Y),
                            e.Node.Checked ? CheckBoxState.CheckedNormal : CheckBoxState.UncheckedNormal);
                    }
                    else
                    {
                        //We don't have to do this, but with Visual Styles disabled, there is a noticeable jarring visual difference from DrawDefault'd checkboxes
                        //So we might as well emulate that style for the sake of consistency
                        var rect = new Rectangle(e.Node.Bounds.X + checkBoxOffset, e.Node.Bounds.Y, 16, 16);
                        ControlPaint.DrawCheckBox(e.Graphics, rect, e.Node.Checked ? ButtonState.Checked | ButtonState.Flat : ButtonState.Flat);
                        rect.Inflate(-2, -2);
                        e.Graphics.DrawRectangle(new Pen(Brushes.Black, 2.0f), rect);
                    }
                }
                if (tag != null)
                {
                    if (tag.DrawSelectabilityIcon)
                    {
                        var icon = tag.IsSelectable ? _selectableIcon : _unselectableIcon;
                        e.Graphics.DrawImage(icon, e.Node.Bounds.X + selectabilityOffset, e.Node.Bounds.Y);
                        //Trace.TraceInformation("Painted icon at ({0},{1})", e.Node.Bounds.X, e.Node.Bounds.Y);
                    }
                    if (tag.ThemeIcon != null)
                    {
                        if (tag.DrawSelectabilityIcon)
                        {
                            e.Graphics.DrawImage(tag.ThemeIcon, e.Node.Bounds.X + iconOffset, e.Node.Bounds.Y);
                            //Trace.TraceInformation("Painted icon at ({0},{1})", e.Node.Bounds.X, e.Node.Bounds.Y);
                        }
                        else
                        {
                            e.Graphics.DrawImage(tag.ThemeIcon, e.Node.Bounds.X + iconOffsetNoSelect, e.Node.Bounds.Y);
                            //Trace.TraceInformation("Painted icon at ({0},{1})", e.Node.Bounds.X, e.Node.Bounds.Y);
                        }
                    }

                    using (SolidBrush brush = new SolidBrush(Color.Black))
                    {
                        e.Graphics.DrawString(e.Node.Text, font, brush, e.Node.Bounds.X + (tag.DrawSelectabilityIcon ? textOffset : textOffsetNoSelect), e.Node.Bounds.Y);
                    }
                }
                else
                {
                    using (SolidBrush brush = new SolidBrush(Color.Black))
                    {
                        e.Graphics.DrawString(e.Node.Text, font, brush, e.Node.Bounds.X + 17.0f + xoffset, e.Node.Bounds.Y);
                    }
                }

                e.DrawDefault = false;
            }
            else
            {
                e.DrawDefault = true;
            }
        }

        internal void SetGroupExpandInLegend(string groupName, bool expand)
        {
            if (_groups.ContainsKey(groupName))
                _provider.SetGroupExpandInLegend(_groups[groupName], expand);
        }

        internal void SetLayerExpandInLegend(string layerName, bool expand)
        {
            if (_layers.ContainsKey(layerName))
                _provider.SetLayerExpandInLegend(_layers[layerName], expand);
        }

        internal void SetGroupVisible(string name, bool bChecked)
        {
            if (_groups.ContainsKey(name))
            {
                var grp = _groups[name];
                grp.SetVisible(bChecked);
                var bVis = HasVisibleParent(grp);
                if (bVis)
                    _legend.OnRequestRefresh();
            }
        }

        internal void SetLayerVisible(string name, bool bChecked)
        {
            if (_layers.ContainsKey(name))
            {
                var layer = _layers[name];
                layer.SetVisible(bChecked);
                var bVis = HasVisibleParent(layer);
                if (bVis)
                {
                    layer.ForceRefresh();
                    _legend.OnRequestRefresh();
                }
            }
        }
    }

    namespace Legend.Model
    {
        class LegendNodeMetadata
        {
            public bool IsGroup { get; protected set; }
        }

        class GroupNodeMetadata : LegendNodeMetadata
        {
            internal MgLayerGroup Group { get; set; }

            public GroupNodeMetadata(MgLayerGroup group)
            {
                base.IsGroup = true;
                this.Group = group;
            }
        }

        class LayerNodeMetadata : LegendNodeMetadata
        {
            public LayerNodeMetadata(MgLayerBase layer, bool bInitiallySelectable)
            {
                base.IsGroup = false;
                this.Layer = layer;
                this.IsSelectable = (layer != null) ? layer.Selectable : false;
                this.DrawSelectabilityIcon = (layer != null && bInitiallySelectable);
                this.IsThemeRule = false;
            }

            internal MgLayerBase Layer { get; set; }

            public bool DrawSelectabilityIcon { get; set; }

            public bool IsSelectable { get; set; }

            public bool IsThemeRule { get; set; }

            public bool IsBaseLayer { get; set; }

            public Image ThemeIcon { get; set; }
        }
    }
}
