using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace OSGeo.MapGuide.Viewer.Redlining
{
    internal partial class RedlineEditingCtrl : UserControl
    {
        private RedlineEditingCtrl()
        {
            InitializeComponent();
            _activeRedlines = new BindingList<RedlineObject>();
            grdActiveRedlines.DataSource = _activeRedlines;
        }

        private IMapViewer _viewer;
        private RedlineLayer _layer;
        private RedlineEditor _ed;

        public RedlineEditingCtrl(IMapViewer viewer, RedlineLayer layer)
            : this()
        {
            _viewer = viewer;
            _layer = layer;
            _viewer.PropertyChanged += OnViewerPropertyChanged;
            _ed = new RedlineEditor(_viewer, _layer);
            this.Disposed += OnDisposed;
            lblRedlineLayerName.Text = _layer.Name;
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            RefreshActiveRedlineList(false);
        }

        private void RefreshActiveRedlineList(bool refreshMap)
        {
            _activeRedlines.Clear();
            var redlines = _ed.GetAllFeatures();
            foreach (var rdl in redlines)
            {
                _activeRedlines.Add(rdl);
            }
            if (refreshMap)
                _viewer.RefreshMap();
        }

        void OnDisposed(object sender, EventArgs e)
        {
            _viewer.PropertyChanged -= OnViewerPropertyChanged;
        }

        void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "DigitizingType")
            {
                bool bNotDrawing = (_viewer.DigitizingType == MapDigitizationType.None);
                btnDrawCircle.Enabled =
                btnDrawLine.Enabled =
                btnDrawLineString.Enabled =
                btnDrawPoint.Enabled =
                btnDrawPolygon.Enabled =
                btnDrawRect.Enabled = bNotDrawing;

                lblDrawingStatus.Visible = !bNotDrawing;
            }
        }

        private string GetInputRedlineText()
        {
            if (chkPromptForLabel.Checked)
                return TextPrompt.GetValue(Strings.TitleRedline, Strings.SpecifyRedlineText) ?? string.Empty;
            else
                return string.Empty;
        }

        private BindingList<RedlineObject> _activeRedlines;

        private void OnRedlineAdded(int key, string text)
        {
            _activeRedlines.Add(new RedlineObject(key, text));
        }

        private void btnDrawPoint_Click(object sender, EventArgs e)
        {
            _ed.AddObject(RedlineGeometryType.Point, GetInputRedlineText, OnRedlineAdded);
        }

        private void btnDrawLine_Click(object sender, EventArgs e)
        {
            _ed.AddObject(RedlineGeometryType.Line, GetInputRedlineText, OnRedlineAdded);
        }

        private void btnDrawLineString_Click(object sender, EventArgs e)
        {
            _ed.AddObject(RedlineGeometryType.LineString, GetInputRedlineText, OnRedlineAdded);
        }

        private void btnDrawPolygon_Click(object sender, EventArgs e)
        {
            _ed.AddObject(RedlineGeometryType.Polygon, GetInputRedlineText, OnRedlineAdded);
        }

        private void btnDrawRect_Click(object sender, EventArgs e)
        {
            _ed.AddObject(RedlineGeometryType.Rectangle, GetInputRedlineText, OnRedlineAdded);
        }

        private void btnDrawCircle_Click(object sender, EventArgs e)
        {
            _ed.AddObject(RedlineGeometryType.Circle, GetInputRedlineText, OnRedlineAdded);
        }

        private void grdActiveRedlines_SelectionChanged(object sender, EventArgs e)
        {
            btnDelete.Enabled = btnEditRedlineText.Enabled = btnSelectRedlines.Enabled = grdActiveRedlines.SelectedRows.Count > 0;
        }

        private int[] GetSelectedRedlineIds()
        {
            List<int> ids = new List<int>();
            foreach (DataGridViewRow row in grdActiveRedlines.SelectedRows)
            {
                var rdl = row.DataBoundItem as RedlineObject;
                ids.Add(rdl.ID);
            }
            return ids.ToArray();
        }

        private void btnEditRedlineText_Click(object sender, EventArgs e)
        {
            string label = TextPrompt.GetValue(Strings.TitleEditRedlineText, Strings.SpecifyRedlineText);
            if (label != null)
            {
                var ids = GetSelectedRedlineIds();
                _ed.UpdateRedlineText(label, ids);
                //TODO: Full refresh of list is probably overkill
                RefreshActiveRedlineList(true);
            }
        }

        private void btnDelete_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show(Strings.ConfirmDeleteRedlines, Strings.TitleDeleteRedlines, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                var ids = GetSelectedRedlineIds();
                _ed.DeleteRedlines(ids);
                //TODO: Full refresh of list is probably overkill
                RefreshActiveRedlineList(true);
            }
        }

        private void btnSelectRedlines_Click(object sender, EventArgs e)
        {
            var ids = GetSelectedRedlineIds();
            var sel = _viewer.GetSelection();
            var map = _viewer.GetMap();
            var layers = map.GetLayers();
            var lidx = layers.IndexOf(_layer.SystemName);
            if (lidx >= 0)
            {
                var layer = layers.GetItem(lidx);
                sel.FromXml("");
                foreach (var id in ids)
                {
                    sel.AddFeatureIdInt32(layer, layer.FeatureClassName, id);
                }
                _viewer.UpdateSelection();
            }
        }
    }
}
