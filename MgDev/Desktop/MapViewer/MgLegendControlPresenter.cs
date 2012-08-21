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
using System.Diagnostics;

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

        private Dictionary<string, LayerNodeMetadata> _layers = new Dictionary<string, LayerNodeMetadata>();
        private Dictionary<string, GroupNodeMetadata> _groups = new Dictionary<string, GroupNodeMetadata>();

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
                _layers.Clear();
                var layers = _map.GetLayers();
                for (int i = 0; i < layers.GetCount(); i++)
                {
                    var layer = layers.GetItem(i);
                    _layers[layer.GetObjectId()] = new LayerNodeMetadata(layer, layer.Selectable);
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

            LayerNodeMetadata layerMeta = null;
            if (fsId.EndsWith("DrawingSource"))
            {
                node.SelectedImageKey = node.ImageKey = IMG_DWF;
                bool bInitiallySelectable = layer.Selectable;
                if (_layers.ContainsKey(layer.GetObjectId()))
                {
                    layerMeta = _layers[layer.GetObjectId()];
                    bInitiallySelectable = layerMeta.WasInitiallySelectable;
                }
                else //If not in the dictionary, assume it is a dynamically added layer
                {
                    layerMeta = new LayerNodeMetadata(layer, bInitiallySelectable);
                    _layers[layer.GetObjectId()] = layerMeta;
                }
                node.Tag = layerMeta;
                node.ToolTipText = string.Format(Properties.Resources.DrawingLayerTooltip, Environment.NewLine, layer.Name, layer.FeatureSourceId);
            }
            else
            {
                var ldfId = layer.LayerDefinition;
                if (_layers.ContainsKey(layer.GetObjectId()))
                {
                    layerMeta = _layers[layer.GetObjectId()];
                }
                else
                {
                    layerMeta = new LayerNodeMetadata(layer, layer.Selectable);
                    _layers[layer.GetObjectId()] = layerMeta;
                }
                if (string.IsNullOrEmpty(layerMeta.LayerDefinitionContent))
                    return null;

                node.Tag = layerMeta;

                XmlDocument doc = new XmlDocument();
                doc.LoadXml(layerMeta.LayerDefinitionContent);
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
                    if (layerMeta.Icon == null)
                    {
                        MgByteReader layerIcon = _provider.GenerateLegendImage(layer.LayerDefinition,
                                                                                _map.ViewScale,
                                                                                16,
                                                                                16,
                                                                                "PNG",
                                                                                -1,
                                                                                -1);
                        legendCallCount++;
                        if (layerIcon != null)
                        {
                            try
                            {
                                byte[] b = new byte[layerIcon.GetLength()];
                                layerIcon.Read(b, b.Length);
                                using (var ms = new MemoryStream(b))
                                {
                                    layerMeta.Icon = Image.FromStream(ms);
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
                            layerMeta.Icon = Properties.Resources.lc_broken;
                        }
                    }
                }
                catch
                {
                    layerMeta.Icon = Properties.Resources.lc_broken;
                }

                if (!layerMeta.HasTheme())
                {
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
                            ThemeCategory themeCat = new ThemeCategory()
                            {
                                MinScale = minScale,
                                MaxScale = maxScale,
                                GeometryType = geomType
                            };

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
                                    layerMeta.Icon = Properties.Resources.lc_theme;
                                    node.ToolTipText = string.Format(Properties.Resources.ThemedLayerTooltip, Environment.NewLine, layer.Name, layer.FeatureSourceId, layer.FeatureClassName, rules.Count);

                                    if (_legend.ThemeCompressionLimit > 0 && rules.Count > _legend.ThemeCompressionLimit)
                                    {
                                        AddThemeRuleNode(layerMeta, themeCat, node, geomType, 0, rules, 0);
                                        node.Nodes.Add(CreateCompressedThemeNode(layerMeta, themeCat, rules.Count - 2));
                                        AddThemeRuleNode(layerMeta, themeCat, node, geomType, rules.Count - 1, rules, rules.Count - 1);
                                    }
                                    else
                                    {
                                        for (int r = 0; r < rules.Count; r++)
                                        {
                                            AddThemeRuleNode(layerMeta, themeCat, node, geomType, catIndex++, rules, r);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else //Already cached
                {
                    Trace.TraceInformation("Icons already cached for: " + layer.Name);
                    node.Nodes.AddRange(layerMeta.CreateThemeNodesFromCachedMetadata(_map.ViewScale));
                }
            }

            return node;
        }

        private void AddThemeRuleNode(LayerNodeMetadata layerMeta, ThemeCategory themeCat, TreeNode node, int geomType, int catIndex, XmlNodeList rules, int r)
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

            var child = CreateThemeRuleNode(layerMeta, themeCat, _map.ViewScale, labelText, (geomType + 1), catIndex);
            node.Nodes.Add(child);
        }

        private TreeNode CreateCompressedThemeNode(LayerNodeMetadata layer, ThemeCategory cat, int count)
        {
            TreeNode node = new TreeNode();
            node.Text = (count + " other styles");
            node.ImageKey = node.SelectedImageKey = IMG_OTHER;
            var meta = new LayerThemeNodeMetadata(true, Properties.Resources.icon_etc, node.Text);
            node.Tag = meta;
            layer.AddThemeNode(cat, meta);
            return node;
        }

        private TreeNode CreateThemeRuleNode(LayerNodeMetadata layer, ThemeCategory themeCat, double viewScale, string labelText, int geomType, int categoryIndex)
        {
            MgLayerBase lyr = layer.Layer;
            MgByteReader icon = _provider.GenerateLegendImage(lyr.LayerDefinition,
                                                              viewScale,
                                                              16,
                                                              16,
                                                              "PNG",
                                                              geomType,
                                                              categoryIndex);
            legendCallCount++;
            TreeNode node = new TreeNode();
            node.Text = labelText;
            if (icon != null)
            {
                try
                {

                    byte[] b = new byte[icon.GetLength()];
                    icon.Read(b, b.Length);
                    using (var ms = new MemoryStream(b))
                    {
                        var tag = new LayerThemeNodeMetadata(false, Image.FromStream(ms), labelText);
                        layer.AddThemeNode(themeCat, tag);
                        node.Tag = tag;
                    }
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
            var meta = new GroupNodeMetadata(group);
            node.Tag = meta;
            _groups[group.GetObjectId()] = meta;
            node.ContextMenuStrip = _legend.GroupContextMenu;
            return node;
        }

        private int legendCallCount = 0;

        public TreeNode[] CreateNodes()
        {
            List<TreeNode> output = new List<TreeNode>();
            var nodesById = new Dictionary<string, TreeNode>();

            var scale = _map.ViewScale;
            if (scale < 10.0)
                return output.ToArray();

            var groups = _map.GetLayerGroups();
            var layers = _map.GetLayers();

            legendCallCount = 0;

            //Process groups first
            List<MgLayerGroup> remainingNodes = new List<MgLayerGroup>();
            for (int i = 0; i < groups.GetCount(); i++)
            {
                var group = groups.GetItem(i);
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

            //Collect all resource contents in a batch
            MgStringCollection layerIds = new MgStringCollection();
            //Also collect the layer metadata nodes to create or update
            var layerMetaNodesToUpdate = new Dictionary<string, MgLayerBase>();
            //Now process layers. Layers without metadata nodes or without layer definition content
            //are added to the list
            for (int i = 0; i < layers.GetCount(); i++)
            {
                var lyr = layers.GetItem(i);
                bool display = lyr.DisplayInLegend;
                bool visible = _provider.IsLayerPotentiallyVisibleAtScale(lyr, false);
                if (!display)
                    continue;

                if (!visible)
                    continue;

                if (_layers.ContainsKey(lyr.GetObjectId()))
                {
                    if (string.IsNullOrEmpty(_layers[lyr.GetObjectId()].LayerDefinitionContent))
                    {
                        var ldfId = lyr.LayerDefinition;
                        layerIds.Add(ldfId.ToString());
                        layerMetaNodesToUpdate[ldfId.ToString()] = lyr;
                    }
                }
                else
                {
                    var ldfId = lyr.LayerDefinition;
                    layerIds.Add(ldfId.ToString());
                    layerMetaNodesToUpdate[ldfId.ToString()] = lyr;
                }
            }

            if (layerIds.GetCount() > 0)
            {
                int added = 0;
                int updated = 0;
                //Fetch the contents and create/update the required layer metadata nodes
                MgStringCollection layerContents = _resSvc.GetResourceContents(layerIds, null);
                for (int i = 0; i < layerIds.GetCount(); i++)
                {
                    string lid = layerIds.GetItem(i);
                    var lyr = layerMetaNodesToUpdate[lid];
                    var objId = lyr.GetObjectId();
                    LayerNodeMetadata meta = null;
                    if (_layers.ContainsKey(objId))
                    {
                        meta = _layers[objId];
                        updated++;
                    }
                    else
                    {
                        meta = new LayerNodeMetadata(lyr, lyr.Selectable);
                        _layers[objId] = meta;
                        added++;
                    }
                    meta.LayerDefinitionContent = layerContents.GetItem(i);
                }
                Trace.TraceInformation("CreateNodes: {0} layer contents added, {1} layer contents updated", added, updated);
            }

            List<MgLayerBase> remainingLayers = new List<MgLayerBase>();
            for (int i = 0; i < layers.GetCount(); i++)
            {
                var layer = layers.GetItem(i);
                
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
            Trace.TraceInformation("{0} calls made to GenerateLegendImage", legendCallCount);
            return output.ToArray();
        }

        private static bool IsThemeLayerNode(TreeNode node)
        {
            var meta = node.Tag as LayerThemeNodeMetadata;
            if (meta != null)
                return true;

            return false;
        }

        private static bool IsLayerNode(TreeNode node)
        {
            var meta = node.Tag as LayerNodeMetadata;
            return meta != null;
        }

        internal void DrawNode(DrawTreeNodeEventArgs e, bool showPlusMinus, Font font)
        {
            if (!e.Bounds.IsEmpty && (IsLayerNode(e.Node) || IsThemeLayerNode(e.Node)))
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

                var layerMeta = e.Node.Tag as LayerNodeMetadata;
                var themeMeta = e.Node.Tag as LayerThemeNodeMetadata;
                var checkBoxOffset = xoffset;
                var selectabilityOffset = xoffset + 16;
                var iconOffsetNoSelect = xoffset + 16;
                if (themeMeta != null) //No checkbox for theme rule nodes
                {
                    selectabilityOffset = xoffset;
                    iconOffsetNoSelect = xoffset;
                }
                var iconOffset = selectabilityOffset + 20;
                var textOffset = iconOffset + 20;
                var textOffsetNoSelect = iconOffsetNoSelect + 20;

                //Uncomment if you need to "see" the bounds of the node
                //e.Graphics.DrawRectangle(Pens.Black, e.Node.Bounds);

                if (layerMeta != null) //No checkbox for theme rule nodes
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
                if (layerMeta != null)
                {
                    if (layerMeta.DrawSelectabilityIcon)
                    {
                        var icon = layerMeta.IsSelectable ? _selectableIcon : _unselectableIcon;
                        e.Graphics.DrawImage(icon, e.Node.Bounds.X + selectabilityOffset, e.Node.Bounds.Y);
                        //Trace.TraceInformation("Painted icon at ({0},{1})", e.Node.Bounds.X, e.Node.Bounds.Y);
                    }
                    if (layerMeta.Icon != null)
                    {
                        if (layerMeta.DrawSelectabilityIcon)
                        {
                            e.Graphics.DrawImage(layerMeta.Icon, e.Node.Bounds.X + iconOffset, e.Node.Bounds.Y);
                            //Trace.TraceInformation("Painted icon at ({0},{1})", e.Node.Bounds.X, e.Node.Bounds.Y);
                        }
                        else
                        {
                            e.Graphics.DrawImage(layerMeta.Icon, e.Node.Bounds.X + iconOffsetNoSelect, e.Node.Bounds.Y);
                            //Trace.TraceInformation("Painted icon at ({0},{1})", e.Node.Bounds.X, e.Node.Bounds.Y);
                        }
                    }

                    using (SolidBrush brush = new SolidBrush(Color.Black))
                    {
                        e.Graphics.DrawString(e.Node.Text, font, brush, e.Node.Bounds.X + (layerMeta.DrawSelectabilityIcon ? textOffset : textOffsetNoSelect), e.Node.Bounds.Y);
                    }
                }
                else if (themeMeta != null)
                {
                    if (themeMeta.ThemeIcon != null)
                    {
                        e.Graphics.DrawImage(themeMeta.ThemeIcon, e.Node.Bounds.X + iconOffsetNoSelect, e.Node.Bounds.Y);
                    }

                    using (SolidBrush brush = new SolidBrush(Color.Black))
                    {
                        e.Graphics.DrawString(e.Node.Text, font, brush, e.Node.Bounds.X + textOffsetNoSelect, e.Node.Bounds.Y);
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

        internal void SetGroupExpandInLegend(string objectId, bool expand)
        {
            if (_groups.ContainsKey(objectId))
            {
                var grp = _groups[objectId].Group;
                _provider.SetGroupExpandInLegend(grp, expand);
            }
        }

        internal void SetLayerExpandInLegend(string objectId, bool expand)
        {
            if (_layers.ContainsKey(objectId))
            {
                var lyr = _layers[objectId].Layer;
                _provider.SetLayerExpandInLegend(lyr, expand);
            }
        }

        internal void SetGroupVisible(string objectId, bool bChecked)
        {
            if (_groups.ContainsKey(objectId))
            {
                var grp = _groups[objectId].Group;
                grp.SetVisible(bChecked);
                var bVis = HasVisibleParent(grp);
                if (bVis)
                    _legend.OnRequestRefresh();
            }
        }

        internal void SetLayerVisible(string objectId, bool bChecked)
        {
            if (_layers.ContainsKey(objectId))
            {
                var layer = _layers[objectId].Layer;
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
        abstract class LegendNodeMetadata
        {
            public bool IsGroup { get; protected set; }

            public abstract string ObjectId { get; }
        }

        [DebuggerDisplay("Name = {Layer.Name}, Label = {Layer.LegendLabel}")]
        class GroupNodeMetadata : LegendNodeMetadata
        {
            internal MgLayerGroup Group { get; private set; }

            public GroupNodeMetadata(MgLayerGroup group)
            {
                base.IsGroup = true;
                this.Group = group;
            }

            public override string ObjectId
            {
                get { return this.Group.GetObjectId(); }
            }
        }

        [DebuggerDisplay("Layer Theme Node")]
        class LayerThemeNodeMetadata : LegendNodeMetadata
        {
            public LayerThemeNodeMetadata(bool bPlaceholder, Image themeIcon, string labelText)
            {
                this.IsPlaceholder = bPlaceholder;
                this.ThemeIcon = themeIcon;
                this.Label = labelText;
            }

            public bool IsPlaceholder { get; private set; }

            public Image ThemeIcon { get; set; }

            public string Label { get; private set; }

            public override string ObjectId
            {
                get { return ""; }
            }
        }

        class ThemeCategory
        {
            public string MinScale { get; set; }

            public string MaxScale { get; set; }

            public int GeometryType { get; set; }

            public override int GetHashCode()
            {
                unchecked // Overflow is fine, just wrap
                {
                    int hash = 17;
                    // Suitable nullity checks etc, of course :)
                    if (MinScale != null)
                        hash = hash * 23 + MinScale.GetHashCode();
                    if (MaxScale != null)
                        hash = hash * 23 + MaxScale.GetHashCode();
                    hash = hash * 23 + GeometryType.GetHashCode();
                    return hash;
                }
            }
        }

        [DebuggerDisplay("Name = {Layer.Name}, Label = {Layer.LegendLabel}")]
        class LayerNodeMetadata : LegendNodeMetadata
        {
            public LayerNodeMetadata(MgLayerBase layer, bool bInitiallySelectable)
            {
                base.IsGroup = false;
                this.Layer = layer;
                this.IsSelectable = (layer != null) ? layer.Selectable : false;
                this.DrawSelectabilityIcon = (layer != null && bInitiallySelectable);
                this.WasInitiallySelectable = bInitiallySelectable;
                this.LayerDefinitionContent = null;
                _themeNodes = new Dictionary<ThemeCategory, List<LayerThemeNodeMetadata>>();
            }

            public override string ObjectId
            {
                get { return this.Layer.GetObjectId(); }
            }

            public Image Icon { get; set; }

            public string IconId { get; set; }

            internal MgLayerBase Layer { get; private set; }

            public bool DrawSelectabilityIcon { get; set; }

            public bool IsSelectable { get; set; }

            public bool WasInitiallySelectable { get; private set; }

            public bool IsBaseLayer { get; set; }

            public string LayerDefinitionContent { get; set; }

            private Dictionary<ThemeCategory, List<LayerThemeNodeMetadata>> _themeNodes;

            public List<LayerThemeNodeMetadata> GetThemeNodes(ThemeCategory category)
            {
                if (_themeNodes.ContainsKey(category))
                    return _themeNodes[category];
                return null;
            }

            public void AddThemeNode(ThemeCategory category, LayerThemeNodeMetadata themeMeta)
            {
                if (!_themeNodes.ContainsKey(category))
                    _themeNodes[category] = new List<LayerThemeNodeMetadata>();

                _themeNodes[category].Add(themeMeta);
            }

            internal bool HasTheme()
            {
                if (_themeNodes.Count == 0)
                    return false;

                foreach (var coll in _themeNodes.Values)
                    if (coll.Count > 0)
                        return true;

                return false;
            }

            internal TreeNode[] CreateThemeNodesFromCachedMetadata(double scale)
            {
                var nodes = new List<TreeNode>();

                //Find the applicable scale range(s)
                foreach (var cat in _themeNodes.Keys)
                {
                    bool bApplicable = false;

                    bool bHasMin = !string.IsNullOrEmpty(cat.MinScale);
                    bool bHasMax = !string.IsNullOrEmpty(cat.MaxScale);

                    if (bHasMin)
                    {
                        double minVal = double.Parse(cat.MinScale);
                        if (bHasMax) //bHasMin = true, bHasMax = true
                        {
                            double maxVal = double.Parse(cat.MaxScale);
                            if (scale >= minVal && scale < maxVal)
                                bApplicable = true;
                        }
                        else         //bHasMin = true, bHasMax = false
                        {
                            if (scale >= minVal)
                                bApplicable = true;
                        }
                    }
                    else
                    {
                        if (bHasMax) //bHasMin = false, bHasMax = true
                        {
                            double maxVal = double.Parse(cat.MaxScale);
                            if (scale < maxVal)
                                bApplicable = true;
                        }
                        else         //bHasMin = false, bHasMax = false
                        {
                            bApplicable = true;
                        }
                    }

                    if (bApplicable)
                    {
                        var metadata = _themeNodes[cat];
                        nodes.AddRange(CreateThemeNodes(metadata));
                    }
                }

                return nodes.ToArray();
            }

            private IEnumerable<TreeNode> CreateThemeNodes(List<LayerThemeNodeMetadata> metadata)
            {
                foreach (var meta in metadata)
                {
                    var node = new TreeNode();
                    node.Text = meta.Label;
                    node.Tag = meta;
                    yield return node;
                }
            }
        }
    }
}
