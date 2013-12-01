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
using System.Windows.Forms.VisualStyles;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    using Legend.Model;

    /// <summary>
    /// A control that displays and controls visibility of layers in a runtime map
    /// </summary>
    public partial class MgLegend : UserControl, IMapLegend, ILegendView, INotifyPropertyChanged 
    {
        // TODO:
        // 
        // 1. Instead of refreshing the legend on save, just flip visibility of the client-side tree
        // 2. Do not refresh if toggling a layer whose parent group is not visible
        // 3. Do not refresh if toggling a group whose parent group is not visible

        const string IMG_BROKEN = "lc_broken"; //NOXLATE
        const string IMG_DWF = "lc_dwf"; //NOXLATE
        const string IMG_GROUP = "lc_group"; //NOXLATE
        const string IMG_RASTER = "lc_raster"; //NOXLATE
        const string IMG_SELECT = "lc_select"; //NOXLATE
        const string IMG_THEME = "lc_theme"; //NOXLATE
        const string IMG_UNSELECT = "lc_unselect"; //NOXLATE
        const string IMG_OTHER = "icon_etc"; //NOXLATE

        // The thing that does all the heavy lifting and dirty work
        private MgLegendControlPresenter _presenter;

        /// <summary>
        /// Initializes a new instance of the <see cref="MgLegend"/> class.
        /// </summary>
        public MgLegend()
        {
            InitializeComponent();
        }

        internal void Init(MgMapViewerProvider provider)
        {
            _presenter = new MgLegendControlPresenter(this, provider);
            RefreshLegend();
        }

        private Stopwatch _legendUpdateStopwatch = new Stopwatch();

        /// <summary>
        /// Refreshes this component
        /// </summary>
        public void RefreshLegend()
        {
            if (_noUpdate)
                return;

            if (_presenter == null)
                return;

            if (IsBusy)
                return;

            ResetTreeView();
            trvLegend.BeginUpdate();
            _legendUpdateStopwatch.Start();
            this.IsBusy = true;
            _currentCulture = System.Threading.Thread.CurrentThread.CurrentUICulture;
            bgLegendUpdate.RunWorkerAsync();
            /*
            //Synchronous version
            try
            {
                trvLegend.Nodes.AddRange(_presenter.CreateNodes());
            }
            finally
            {
                trvLegend.EndUpdate();
            }*/
        }

        private bool _busy = false;

        /// <summary>
        /// Gets whether the legend is currently in the state of rendering its tree
        /// </summary>
        [Browsable(false)]
        public bool IsBusy
        {
            get { return _busy; }
            private set
            {
                if (_busy.Equals(value))
                    return;

                _busy = value;
                Trace.TraceInformation("Legend IsBusy: {0}", this.IsBusy); //NOXLATE
                OnPropertyChanged("IsBusy"); //NOXLATE
            }
        }

        private System.Globalization.CultureInfo _currentCulture;

        private void bgLegendUpdate_DoWork(object sender, DoWorkEventArgs e)
        {
            //This being on a different thread, needs to have its culture information synced up
            System.Threading.Thread.CurrentThread.CurrentUICulture =
                System.Threading.Thread.CurrentThread.CurrentCulture = _currentCulture;
            e.Result = _presenter.CreateNodes();
        }

        private void bgLegendUpdate_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            this.IsBusy = bgLegendUpdate.IsBusy;
            var nodes = e.Result as TreeNode[];
            if (nodes != null)
            {
                //Attach relevant context menus based on attached metadata
                foreach (var n in nodes)
                {
                    var lm = n.Tag as LegendNodeMetadata;
                    if (lm != null)
                    {
                        if (lm.IsGroup)
                        {
                            n.ContextMenuStrip = this.GroupContextMenu;
                        }
                        else
                        {
                            var lyrm = n.Tag as LayerNodeMetadata;
                            if (lyrm != null)
                                n.ContextMenuStrip = this.LayerContextMenu;
                        }
                    }
                }
                trvLegend.Nodes.AddRange(nodes);
            }
            trvLegend.EndUpdate();
            _legendUpdateStopwatch.Stop();
            Trace.TraceInformation("RefreshLegend: Completed in {0}ms", _legendUpdateStopwatch.ElapsedMilliseconds); //NOXLATE
            _legendUpdateStopwatch.Reset();
        }

        private static void ClearNodes(TreeNodeCollection nodes)
        {
            foreach (TreeNode node in nodes)
            {
                if (node.Nodes.Count > 0)
                    ClearNodes(node.Nodes);

                /*
                var layerMeta = node.Tag as LayerNodeMetadata;
                if (layerMeta != null && layerMeta.ThemeIcon != null)
                {
                    layerMeta.Layer = null;
                    layerMeta.ThemeIcon.Dispose();
                    layerMeta.ThemeIcon = null;
                }*/
            }
            nodes.Clear();
        }

        void ILegendView.AddLegendIcon(string id, Image img)
        {
            imgLegend.Images.Add(id, img);
        }

        private void ResetTreeView()
        {
            ClearNodes(trvLegend.Nodes);
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

        private void trvLegend_AfterCheck(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag == null)
                return;

            var meta = ((LegendNodeMetadata)e.Node.Tag);
            if (!meta.Checkable)
                return;

            if (meta.IsGroup) //Group
            {
                _presenter.SetGroupVisible(meta.ObjectId, e.Node.Checked);
            }
            else //Layer
            {
                _presenter.SetLayerVisible(meta.ObjectId, e.Node.Checked);
            }
        }

        private void trvLegend_AfterExpand(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag == null)
                return;

            var meta = ((LegendNodeMetadata)e.Node.Tag);
            if (!meta.Checkable) //Shouldn't happen, but just in case
                return;

            if (meta.IsGroup) //Group
            {
                _presenter.SetGroupExpandInLegend(meta.ObjectId, true);
            }
            else //Layer
            {
                _presenter.SetLayerExpandInLegend(meta.ObjectId, true);
            }
        }

        private void trvLegend_AfterCollapse(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag == null)
                return;

            var meta = ((LegendNodeMetadata)e.Node.Tag);
            if (meta.IsGroup) //Group
            {
                _presenter.SetGroupExpandInLegend(meta.ObjectId, false);
            }
            else //Layer
            {
                _presenter.SetLayerExpandInLegend(meta.ObjectId, false);
            }
        }

        private bool _noUpdate = false;

        void ILegendView.OnRequestRefresh()
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

        

        private void trvLegend_DrawNode(object sender, DrawTreeNodeEventArgs e)
        {
            _presenter.DrawNode(e, trvLegend.ShowPlusMinus, trvLegend.Font);
        }

        private ContextMenuStrip _grpContextMenu;

        private ContextMenuStrip _layerContextMenu;

        /// <summary>
        /// Gets or sets the context menu that is attached to group nodes
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The context menu to attach to group nodes")] //NOXLATE
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
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The context menu to attach to layer nodes")] //NOXLATE
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
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The number of rules a layer style must exceed in order to be displayed as a compressed theme")] //NOXLATE
        public int ThemeCompressionLimit
        {
            get { return _themeCompressionLimit; }
            set 
            {
                if (value == _themeCompressionLimit)
                    return;

                _themeCompressionLimit = value;
                OnPropertyChanged("ThemeCompressionLimit"); //NOXLATE
            }
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
            var meta = e.Node.Tag as LayerNodeMetadata;
            if (meta != null && meta.DrawSelectabilityIcon)
            {
                //Toggle layer's selectability if it's within the bounds of the selectability icon
                var box = new Rectangle(
                    new Point((e.Node.Bounds.Location.X - 36) + 16, e.Node.Bounds.Location.Y), 
                    new Size(16, e.Node.Bounds.Height));

                //Uncheckable items need to move 16px to the left
                if (!meta.Checkable)
                    box.Offset(-16, 0);

                if (box.Contains(e.X, e.Y))
                {
                    var layer = meta.Layer;
                    layer.Selectable = !layer.Selectable;
                    meta.IsSelectable = layer.Selectable;

                    //TODO: This bounds is a guess. We should calculate the bounds as part of node rendering, so we know the exact bounds by which to invalidate
                    trvLegend.Invalidate(new Rectangle(e.Node.Bounds.Location.X - 36, e.Node.Bounds.Location.Y, e.Node.Bounds.Width + 36, e.Node.Bounds.Height));
                }
            }
        }

        /// <summary>
        /// Gets the selected layer
        /// </summary>
        /// <returns></returns>
        public MgLayerBase GetSelectedLayer()
        {
            if (_presenter == null)
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
            if (_presenter == null)
                return null;

            if (trvLegend.SelectedNode == null)
                return null;

            var grp = trvLegend.SelectedNode.Tag as GroupNodeMetadata;
            if (grp != null)
                return grp.Group;

            return null;
        }

        /// <summary>
        /// Gets or sets whether to display tooltips for tree nodes
        /// </summary>
        public bool ShowTooltips
        {
            get { return trvLegend.ShowNodeToolTips; }
            set 
            {
                if (value != trvLegend.ShowNodeToolTips)
                    return;

                trvLegend.ShowNodeToolTips = value;
                OnPropertyChanged("ShowTooltips"); //NOXLATE
            }
        }

        /// <summary>
        /// Raised when a property has changed
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        private void OnPropertyChanged(string propertyName)
        {
            var h = this.PropertyChanged;
            if (h != null)
                h(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
