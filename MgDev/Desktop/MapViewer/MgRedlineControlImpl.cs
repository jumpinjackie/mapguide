using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer.Redlining;

namespace OSGeo.MapGuide.Viewer
{
    public partial class MgRedlineControlImpl : MgControlView
    {
        private MgRedlineControlImpl()
        {
            InitializeComponent();
        }

        private MgRedlineComponent _component;

        public MgRedlineControlImpl(MgRedlineComponent component)
            : this()
        {
            _component = component;
            this.Title = Strings.TitleRedline;
            SetBodyControl(new RedlineMainCtrl());
        }

        private void lstLayersOnMap_SelectedIndexChanged(object sender, EventArgs e)
        {
            btnRemoveLayerFromMap.Enabled = btnEditStyle.Enabled = btnEditObjects.Enabled = (lstLayersOnMap.SelectedItems.Count == 1);
        }

        private void lstRedlineLayers_SelectedIndexChanged(object sender, EventArgs e)
        {
            btnDeleteRedlineStore.Enabled = btnAddToMap.Enabled = (lstRedlineLayers.SelectedItems.Count == 1);
        }

        private void btnDeleteRedlineStore_Click(object sender, EventArgs e)
        {
            if (lstRedlineLayers.SelectedItems.Count == 1)
            {
                var item = lstRedlineLayers.SelectedItems[0];
                var layer = item.Tag as RedlineLayer;
                if (layer != null)
                {
                    var mgr = GetRedlineManager();
                    mgr.DeleteLayer(layer);
                    lstRedlineLayers.Items.Remove(item);
                }
            }
        }

        private void btnAddToMap_Click(object sender, EventArgs e)
        {
            if (lstRedlineLayers.SelectedItems.Count == 1)
            {
                var item = lstRedlineLayers.SelectedItems[0];
                var layer = item.Tag as RedlineLayer;
                if (layer != null)
                {
                    var mgr = GetRedlineManager();
                    mgr.AddLayerToMap(layer);
                    lstRedlineLayers.Items.Remove(item);

                    AddRedlineLayerOnMap(layer);
                    _component.Viewer.RefreshMap();
                }
            }
        }

        private void btnRemoveLayerFromMap_Click(object sender, EventArgs e)
        {
            if (lstLayersOnMap.SelectedItems.Count == 1)
            {
                var item = lstLayersOnMap.SelectedItems[0];
                var layer = item.Tag as RedlineLayer;
                if (layer != null)
                {
                    var mgr = GetRedlineManager();
                    mgr.RemoveLayerFromMap(layer);

                    lstLayersOnMap.Items.Remove(item);

                    AddAvailableRedlineLayer(layer);
                    _component.Viewer.RefreshMap();
                    SetBodyControl(new RedlineMainCtrl());
                }
            }
        }

        private void btnEditStyle_Click(object sender, EventArgs e)
        {
            if (lstLayersOnMap.SelectedItems.Count == 1)
            {
                var item = lstLayersOnMap.SelectedItems[0];
                var layer = item.Tag as RedlineLayer;
                if (layer != null)
                {
                    var style = _manager.GetLayerStyle(layer);
                    using (var diag = new RedlineStyleDialog(style))
                    {
                        if (diag.ShowDialog() == DialogResult.OK)
                        {
                            _manager.UpdateLayerStyle(layer, diag.GetUpdatedStyle());
                            _component.Viewer.RefreshMap();
                        }
                    }
                }
            }
        }

        private void btnEditObjects_Click(object sender, EventArgs e)
        {
            if (lstLayersOnMap.SelectedItems.Count == 1)
            {
                var item = lstLayersOnMap.SelectedItems[0];
                var layer = item.Tag as RedlineLayer;
                if (layer != null)
                {
                    var control = new RedlineEditingCtrl(_component.Viewer, layer);
                    SetBodyControl(control);
                }
            }
        }

        private void SetBodyControl(Control c)
        {
            ClearBody();
            c.Dock = DockStyle.Fill;
            mainBody.Controls.Add(c);
        }

        private void ClearBody()
        {
            List<Control> controls = new List<Control>();
            foreach (Control cnt in mainBody.Controls)
                controls.Add(cnt);
            mainBody.Controls.Clear();
            foreach (Control cnt in controls)
                cnt.Dispose();
        }

        private string GetName()
        {
            return "RedlineLayer";
        }

        private void AddAvailableRedlineLayer(RedlineLayer layer)
        {
            ListViewItem item = new ListViewItem(layer.Name);
            item.Name = layer.Name;
            item.Tag = layer;
            item.ImageIndex = 0;
            item.StateImageIndex = 0;
            lstRedlineLayers.Items.Add(item);

            lstLayersOnMap.Items.RemoveByKey(layer.Name);
        }

        private RedlineManager _manager;

        private RedlineManager GetRedlineManager()
        {
            if (_manager == null)
            {
                _manager = new RedlineManager(_component.Viewer);
            }
            return _manager;
        }

        private void AddRedlineLayerOnMap(RedlineLayer layer)
        {
            ListViewItem item = new ListViewItem(layer.Name);
            item.Name = layer.Name;
            item.Tag = layer;
            item.ImageIndex = 0;
            item.StateImageIndex = 0;
            lstLayersOnMap.Items.Add(item);

            lstRedlineLayers.Items.RemoveByKey(layer.Name);
        }

        private void btnNew_Click(object sender, EventArgs e)
        {
            var mgr = GetRedlineManager();
            var p = GetCreateParams();
            if (p == null)
                return;

            bool bAddedToMap = false;
            var layerInfo = mgr.CreateRedlineLayer(p, out bAddedToMap);
            if (bAddedToMap)
                AddRedlineLayerOnMap(layerInfo);
            else
                AddAvailableRedlineLayer(layerInfo);
        }

        private CreateRedlineLayerParams GetCreateParams()
        {
            if (_component.UseDefaultSettings)
            {
                //TODO: Popup UI dialog for user to enter this information if desired
                return new CreateRedlineLayerParams()
                {
                    AddToMap = true,
                    Format = _component.DefaultDataStoreFormat,
                    GeometryTypes = _component.DefaultGeometryTypes,
                    Name = GetName(),
                    Style = RedlineStyle.CreateDefault(),
                    StyleType = _component.StylizationType
                };
            }
            else
            {
                using (var diag = new NewRedlineLayerDialog(RedlineStyle.CreateDefault(),
                                                            _component.DefaultDataStoreFormat,
                                                            _component.DefaultGeometryTypes,
                                                            GetName()))
                {
                    if (diag.ShowDialog() == DialogResult.OK)
                    {
                        return new CreateRedlineLayerParams()
                        {
                            AddToMap = true,
                            Format = diag.Format,
                            GeometryTypes = diag.GeometryTypes,
                            Name = diag.LayerName,
                            Style = diag.Style,
                            StyleType = _component.StylizationType
                        };
                    }
                }
            }
            return null;
        }

        private void btnRefreshAvailableLayers_Click(object sender, EventArgs e)
        {
            lstRedlineLayers.Clear();
            var mgr = GetRedlineManager();
            MgMapBase map = _component.Viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            foreach (var layer in mgr.GetAvailableLayers())
            {
                //Omit those already on the map
                if (layers.IndexOf(layer.SystemName) >= 0)
                    continue;

                AddAvailableRedlineLayer(layer);
            }
        }

        private void btnRefreshLayersOnMap_Click(object sender, EventArgs e)
        {
            lstLayersOnMap.Clear();
            var mgr = GetRedlineManager();
            MgMapBase map = _component.Viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            foreach (var layer in mgr.GetAvailableLayers())
            {
                //Omit those not on the map
                if (layers.IndexOf(layer.SystemName) < 0)
                    continue;

                AddRedlineLayerOnMap(layer);
            }
        }
    }
}
