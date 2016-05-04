using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    internal partial class MgQueryControlImpl : MgControlView
    {
        private IMapViewer _viewer;
        private MgWktReaderWriter _wktRw;
        private MgGeometryFactory _geomFact;

        private BindingList<MgDataPropertyDefinition> _properties;
        private BindingList<MgLayerBase> _layers;

        private string[] _restrictedLayerList;

        public MgQueryControlImpl(IMapViewer viewer, string[] layerList)
        {
            InitializeComponent();
            _restrictedLayerList = layerList;
            InitBase(viewer);
            LoadLayerList(viewer);
            cmbLayer.SelectedIndex = 0;
        }

        private void InitBase(IMapViewer viewer)
        {
            this.Title = Strings.TitleQuery;
            this.Disposed += new EventHandler(OnDisposed);
            _viewer = viewer;
            _properties = new BindingList<MgDataPropertyDefinition>();
            _layers = new BindingList<MgLayerBase>();
            _wktRw = new MgWktReaderWriter();
            _geomFact = new MgGeometryFactory();

            cmbOperator.DataSource = Enum.GetValues(typeof(QueryOperator));
            cmbLayer.DataSource = _layers;
            cmbProperty.DataSource = _properties;

            cmbOperator.SelectedIndex = 0;
        }

        public MgQueryControlImpl(IMapViewer viewer)
        {
            InitializeComponent();
            InitBase(viewer);
            LoadLayerList(viewer);
            cmbLayer.SelectedIndex = 0;
        }

        private void LoadLayerList(IMapViewer viewer)
        {
            var map = viewer.GetMap();
            var layers = map.GetLayers();
            if (_restrictedLayerList != null && _restrictedLayerList.Length > 0)
            {
                foreach (var name in _restrictedLayerList)
                {
                    if (layers.IndexOf(name) < 0)
                        continue;

                    var layer = layers.GetItem(name);
                    _layers.Add(layer);
                }
            }
            else
            {
                for (var i = 0; i < layers.GetCount(); i++)
                {
                    _layers.Add(layers.GetItem(i));
                }
            }
        }

        void OnDisposed(object sender, EventArgs e)
        {
            _properties.Clear();
            _layers.Clear();
            _wktRw.Dispose();
            _wktRw = null;
            _geomFact.Dispose();
            _geomFact = null;
        }

        private void cmbLayer_SelectedIndexChanged(object sender, EventArgs e)
        {
            var layer = cmbLayer.SelectedItem as MgLayerBase;
            if (layer != null)
            {
                var cls = layer.GetClassDefinition();
                var clsProps = cls.GetProperties();
                _properties.Clear();
                for (var i = 0; i < clsProps.GetCount(); i++)
                {
                    var prop = clsProps.GetItem(i);
                    if (prop.GetPropertyType() == MgFeaturePropertyType.DataProperty)
                    {
                        _properties.Add((MgDataPropertyDefinition)prop);
                    }
                }
                cmbProperty.SelectedIndex = 0;
            }
        }

        private MgGeometry _filterGeometry;

        private void btnRectangle_Click(object sender, EventArgs e)
        {
            _viewer.DigitizeRectangle((llx, lly, urx, ury) =>
            {
                ClearFilterGeometry();
                _filterGeometry = _wktRw.Read(Util.MakeWktPolygon(llx, lly, urx, ury));
                lblSpatialFilterGeomSet.Visible = true;
            });
        }

        private void btnPolygon_Click(object sender, EventArgs e)
        {
            _viewer.DigitizePolygon((coordinates) =>
            {
                ClearFilterGeometry();
                MgCoordinateCollection coords = new MgCoordinateCollection();
                for (int i = 0; i < coordinates.GetLength(0); i++)
                {
                    coords.Add(_geomFact.CreateCoordinateXY(coordinates[i, 0], coordinates[i, 1]));
                }
                coords.Add(_geomFact.CreateCoordinateXY(coordinates[0, 0], coordinates[0, 1]));
                MgLinearRing ring = _geomFact.CreateLinearRing(coords);
                _filterGeometry = _geomFact.CreatePolygon(ring, null);
                lblSpatialFilterGeomSet.Visible = true;
            });
        }

        private void ClearFilterGeometry()
        {
            if (_filterGeometry != null)
                _filterGeometry.Dispose();
            lblSpatialFilterGeomSet.Visible = false;
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            ClearFilterGeometry();
        }

        private void btnExecute_Click(object sender, EventArgs e)
        {
            var query = new MgFeatureQueryOptions();
            var provider = _viewer.GetProvider();
            var layer = (MgLayerBase)cmbLayer.SelectedItem;
            var prop = (MgDataPropertyDefinition)cmbProperty.SelectedItem;
            var op = (QueryOperator)cmbOperator.SelectedItem;

            if (chkPropFilterEnabled.Checked)
                query.SetFilter(FilterToString(prop, op, txtValue.Text));
            if (chkSpatialFilter.Checked)
                query.SetSpatialFilter(layer.GetFeatureGeometryName(), _filterGeometry, MgFeatureSpatialOperations.Intersects);

            var reader = layer.SelectFeatures(query);
            new MgQueryResultsDialog(_viewer, layer, reader, Convert.ToInt32(numResults.Value)).Show();
        }

        static string FilterToString(MgDataPropertyDefinition prop, QueryOperator op, string value)
        {
            var sb = new StringBuilder();

            sb.Append(prop.Name + " "); //NOXLATE

            switch (op)
            {
                case QueryOperator.EqualTo:
                    sb.Append(" = "); //NOXLATE
                    break;
                case QueryOperator.GreaterThan:
                    sb.Append(" > "); //NOXLATE
                    break;
                case QueryOperator.GreaterThanOrEqualTo:
                    sb.Append(" >= "); //NOXLATE
                    break;
                case QueryOperator.In:
                    sb.Append(" IN "); //NOXLATE
                    break;
                case QueryOperator.LessThan:
                    sb.Append(" < "); //NOXLATE
                    break;
                case QueryOperator.LessThanOrEqualTo:
                    sb.Append(" <= "); //NOXLATE
                    break;
                case QueryOperator.Like:
                    sb.Append(" LIKE "); //NOXLATE
                    break;
                case QueryOperator.NotEqualTo:
                    sb.Append(" <> "); //NOXLATE
                    break;
            }

            if (op == QueryOperator.In)
            {
                sb.Append("("); //NOXLATE
                sb.Append(value);
                sb.Append(")"); //NOXLATE
            }
            else
            {
                if (prop.DataType == MgPropertyType.String)
                {
                    sb.Append("'"); //NOXLATE
                    sb.Append(value);
                    sb.Append("'"); //NOXLATE
                }
                else
                {
                    sb.Append(value);
                }
            }

            return sb.ToString();
        }

        private void lnkRefreshLayers_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            _layers.Clear();
            LoadLayerList(_viewer);
            if (cmbLayer.SelectedIndex != 0)
                cmbLayer.SelectedIndex = 0;
            else
                cmbLayer_SelectedIndexChanged(this, EventArgs.Empty);
        }
    }

    public enum QueryOperator
    {
        EqualTo,
        NotEqualTo,
        GreaterThan,
        GreaterThanOrEqualTo,
        LessThan,
        LessThanOrEqualTo,
        Like,
        In
    }
}
