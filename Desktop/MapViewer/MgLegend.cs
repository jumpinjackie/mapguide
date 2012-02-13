using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.IO;
using System.Xml;
using System.Diagnostics;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A control that displays and controls visibility of layers in a runtime map
    /// </summary>
    public partial class MgLegend : UserControl, IMapLegend
    {
        // TODO:
        // 
        // 1. This should build the full client-side tree (w/ applicable scales) ala. legend.aspx in the AJAX viewer
        // 2. This should be done in a background worker to avoid clogging up the UI thread.
        // 3. Instead of refreshing the legend on save, just flip visibility of the client-side tree
        // 4. Load layer icons on demand.
        // 5. Do not refresh if toggling a layer whose parent group is not visible
        // 6. Do not refresh if toggling a group whose parent group is not visible

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

        /// <summary>
        /// Initializes a new instance of the <see cref="MgLegend"/> class.
        /// </summary>
        public MgLegend()
        {
            InitializeComponent();
        }

        internal void Init(MgMapViewerProvider provider)
        {
            _provider = provider;
            _map = _provider.GetMap();
            _resSvc = (MgResourceService)_provider.CreateService(MgServiceType.ResourceService);
            RefreshLegend();
        }

        private Dictionary<string, MgLayerBase> _layers = new Dictionary<string, MgLayerBase>();
        private Dictionary<string, MgLayerGroup> _groups = new Dictionary<string, MgLayerGroup>();
        private Dictionary<string, string> _layerDefinitionContents = new Dictionary<string, string>();

        /// <summary>
        /// Refreshes this component
        /// </summary>
        public void RefreshLegend()
        {
            if (_noUpdate)
                return;

            if (_map == null)
                return;

            //System.Diagnostics.Trace.TraceInformation("MgLegend.RefreshLegend()");
            var scale = _map.ViewScale;
            var groups = _map.GetLayerGroups();
            var layers = _map.GetLayers();

            ResetTreeView();

            _layerDefinitionContents.Clear();
            _layers.Clear();
            _groups.Clear();

            trvLegend.BeginUpdate();
            try
            {
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
                        trvLegend.Nodes.Add(node);
                    }

                    while (remainingNodes.Count > 0)
                    {
                        List<MgLayerGroup> toRemove = new List<MgLayerGroup>();
                        for (int j = 0; j < remainingNodes.Count; j++)
                        {
                            var parentId = remainingNodes[j].Group.GetObjectId();
                            var nodes = trvLegend.Nodes.Find(parentId, false);
                            if (nodes.Length == 1)
                            {
                                var node = CreateGroupNode(remainingNodes[j]);
                                nodes[0].Nodes.Add(node);
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
                            trvLegend.Nodes.Add(node);
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
                            var nodes = trvLegend.Nodes.Find(parentId, false);
                            if (nodes.Length == 1)
                            {
                                var node = CreateLayerNode(remainingLayers[j]);
                                if (node != null)
                                {
                                    nodes[0].Nodes.Add(node);
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
                        var nodes = trvLegend.Nodes.Find(group.GetObjectId(), false);
                        if (nodes.Length == 1)
                        {
                            nodes[0].Expand();
                        }
                    }
                }
            }
            finally
            {
                trvLegend.EndUpdate();
            }
        }

        private void ResetTreeView()
        {
            trvLegend.Nodes.Clear();
            imgLegend.Images.Clear();

            imgLegend.Images.Add(IMG_BROKEN, Properties.Resources.lc_broken);
            imgLegend.Images.Add(IMG_DWF, Properties.Resources.lc_dwf);
            imgLegend.Images.Add(IMG_GROUP, Properties.Resources.lc_group);
            imgLegend.Images.Add(IMG_RASTER, Properties.Resources.lc_raster);
            imgLegend.Images.Add(IMG_SELECT, Properties.Resources.lc_select);
            imgLegend.Images.Add(IMG_THEME, Properties.Resources.lc_theme);
            imgLegend.Images.Add(IMG_UNSELECT, Properties.Resources.lc_unselect);
            imgLegend.Images.Add(IMG_OTHER, Properties.Resources.icon_etc);
        }

        private TreeNode CreateLayerNode(MgLayerBase layer)
        {
            var node = new TreeNode();
            node.Name = layer.GetObjectId();
            node.Text = layer.GetLegendLabel();
            node.Checked = layer.GetVisible();
            node.ContextMenuStrip = this.LayerContextMenu;
            var lt = layer.GetLayerType();
            var fsId = layer.GetFeatureSourceId();

            if (fsId.EndsWith("DrawingSource"))
            {
                node.SelectedImageKey = node.ImageKey = IMG_DWF;
                node.Tag = new LayerNodeMetadata(layer);
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

                                imgLegend.Images.Add(id, img);
                                node.SelectedImageKey = node.ImageKey = id;
                            }
                            node.Tag = new LayerNodeMetadata(layer);
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
                                if (this.ThemeCompressionLimit > 0 && rules.Count > this.ThemeCompressionLimit)
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
            node.Tag = new LayerNodeMetadata(null) {
                IsBaseLayer = false,
                ThemeIcon = Properties.Resources.icon_etc
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
                    var tag = new LayerNodeMetadata(null)
                    {
                        IsBaseLayer = false
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
            node.ContextMenuStrip = this.GroupContextMenu;
            return node;
        }

        private double _scale;

        /// <summary>
        /// Sets the applicable scale
        /// </summary>
        /// <param name="scale"></param>
        public void SetScale(double scale)
        {
            _scale = scale;
            RefreshLegend();
        }

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
            public LayerNodeMetadata(MgLayerBase layer) 
            { 
                base.IsGroup = false;
                this.Layer = layer;
            }

            internal MgLayerBase Layer { get; set; }

            public bool IsBaseLayer { get; set; }

            public Image ThemeIcon { get; set; }
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

        private void trvLegend_AfterCheck(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag == null)
                return;

            if (((LegendNodeMetadata)e.Node.Tag).IsGroup) //Group
            {
                if (_groups.ContainsKey(e.Node.Name))
                {
                    var grp = _groups[e.Node.Name];
                    grp.SetVisible(e.Node.Checked);
                    var bVis = HasVisibleParent(grp);
                    if (bVis)
                        OnRequestRefresh();
                }
            }
            else //Layer
            {
                if (_layers.ContainsKey(e.Node.Name))
                {
                    var layer = _layers[e.Node.Name];
                    layer.SetVisible(e.Node.Checked);
                    var bVis = HasVisibleParent(layer);
                    if (bVis)
                    {
                        layer.ForceRefresh();
                        OnRequestRefresh();
                    }
                }
            }
        }

        private void trvLegend_AfterExpand(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag == null)
                return;

            if (((LegendNodeMetadata)e.Node.Tag).IsGroup) //Group
            {
                if (_groups.ContainsKey(e.Node.Name))
                {
                    _provider.SetGroupExpandInLegend(_groups[e.Node.Name], true);
                }
            }
            else //Layer
            {
                if (_layers.ContainsKey(e.Node.Name))
                {
                    var layer = _layers[e.Node.Name];
                    _provider.SetLayerExpandInLegend(layer, true);
                }
            }
        }

        private void trvLegend_AfterCollapse(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag == null)
                return;

            if (((LegendNodeMetadata)e.Node.Tag).IsGroup) //Group
            {
                if (_groups.ContainsKey(e.Node.Name))
                {
                    _provider.SetGroupExpandInLegend(_groups[e.Node.Name], false);
                }
            }
            else //Layer
            {
                if (_layers.ContainsKey(e.Node.Name))
                {
                    var layer = _layers[e.Node.Name];
                    _provider.SetLayerExpandInLegend(layer, false);
                }
            }
        }

        private bool _noUpdate = false;

        private void OnRequestRefresh()
        {
            var handler = this.VisibilityChanged;
            if (handler != null)
                handler(this, EventArgs.Empty);
        }

        /// <summary>
        /// Raised when a layer's visibility has changed
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public event EventHandler VisibilityChanged;

        private static bool IsThemeLayerNode(TreeNode node)
        {
            var meta = node.Tag as LayerNodeMetadata;
            if (meta != null)
                return meta.ThemeIcon != null || meta.IsBaseLayer;

            return false;
        }

        private void trvLegend_DrawNode(object sender, DrawTreeNodeEventArgs e)
        {
            if (IsThemeLayerNode(e.Node) && !e.Bounds.IsEmpty)
            {
                Color backColor, foreColor;

                //For some reason, the default bounds are way off from what you would
                //expect it to be. So we apply this offset for any text/image draw operations
                int xoffset = -36;

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

                /*
                using (SolidBrush brush = new SolidBrush(backColor))
                {
                    e.Graphics.FillRectangle(brush, e.Node.Bounds);
                }*/

                //TextRenderer.DrawText(e.Graphics, e.Node.Text, trvLegend.Font, e.Node.Bounds, foreColor, backColor);
                using (SolidBrush brush = new SolidBrush(Color.Black))
                {
                    e.Graphics.DrawString(e.Node.Text, trvLegend.Font, brush, e.Node.Bounds.X + 17.0f + xoffset, e.Node.Bounds.Y);
                }

                /*
                if ((e.State & TreeNodeStates.Focused) == TreeNodeStates.Focused)
                {
                    ControlPaint.DrawFocusRectangle(e.Graphics, e.Node.Bounds, foreColor, backColor);
                }*/

                var tag = e.Node.Tag as LayerNodeMetadata;
                if (tag != null && tag.ThemeIcon != null)
                {
                    e.Graphics.DrawImage(tag.ThemeIcon, e.Node.Bounds.X + xoffset, e.Node.Bounds.Y);
                    Trace.TraceInformation("Painted icon at ({0},{1})", e.Node.Bounds.X, e.Node.Bounds.Y);
                }

                e.DrawDefault = false;
            }
            else
            {
                e.DrawDefault = true;
            }
        }

        private ContextMenuStrip _grpContextMenu;

        private ContextMenuStrip _layerContextMenu;

        /// <summary>
        /// Gets or sets the context menu that is attached to group nodes
        /// </summary>
        [Category("MapGuide Viewer")]
        [Description("The context menu to attach to group nodes")]
        public ContextMenuStrip GroupContextMenu
        {
            get { return _grpContextMenu; }
            set
            {
                if (_grpContextMenu != null)
                    _grpContextMenu.Opening -= OnGroupContextMenuOpening;

                _grpContextMenu = value;
                if (_grpContextMenu != null)
                    _grpContextMenu.Opening += OnGroupContextMenuOpening;
                if (!this.DesignMode)
                    RefreshLegend();
            }
        }

        private void OnGroupContextMenuOpening(object sender, CancelEventArgs e)
        {

        }

        /// <summary>
        /// Gets or sets the context menu that is attached to layer nodes
        /// </summary>
        [Category("MapGuide Viewer")]
        [Description("The context menu to attach to layer nodes")]
        public ContextMenuStrip LayerContextMenu
        {
            get { return _layerContextMenu; }
            set
            {
                if (_layerContextMenu != null)
                    _layerContextMenu.Opening -= OnLayerContextMenuOpening;

                _layerContextMenu = value;
                if (_layerContextMenu != null)
                    _layerContextMenu.Opening += OnLayerContextMenuOpening;
                if (!this.DesignMode)
                    RefreshLegend();
            }
        }

        private int _themeCompressionLimit;

        /// <summary>
        /// Gets or sets the theme compression limit.
        /// </summary>
        /// <value>
        /// The theme compression limit.
        /// </value>
        [Category("MapGuide Viewer")]
        [Description("The number of rules a layer style must exceed in order to be displayed as a compressed theme")]
        public int ThemeCompressionLimit
        {
            get { return _themeCompressionLimit; }
            set { _themeCompressionLimit = value; }
        }

        private void OnLayerContextMenuOpening(object sender, CancelEventArgs e)
        {

        }

        private void trvLegend_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                trvLegend.SelectedNode = e.Node;
            }
        }

        /// <summary>
        /// Gets the selected layer
        /// </summary>
        /// <returns></returns>
        public MgLayerBase GetSelectedLayer()
        {
            if (_map == null)
                return null;

            if (trvLegend.SelectedNode == null)
                return null;

            var lyr = trvLegend.SelectedNode.Tag as LayerNodeMetadata;
            if (lyr != null)
                return lyr.Layer;

            return null;
        }

        /// <summary>
        /// Gets the selected group
        /// </summary>
        /// <returns></returns>
        public MgLayerGroup GetSelectedGroup()
        {
            if (_map == null)
                return null;

            if (trvLegend.SelectedNode == null)
                return null;

            var grp = trvLegend.SelectedNode.Tag as GroupNodeMetadata;
            if (grp != null)
                return grp.Group;

            return null;
        }
    }
}
