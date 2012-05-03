using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace OSGeo.MapGuide.Viewer
{
    internal partial class MgQueryControlImpl : MgControlImpl
    {
        private IMapViewer _viewer;
        private MgWktReaderWriter _wktRw;
        private MgGeometryFactory _geomFact;

        private BindingList<MgDataPropertyDefinition> _properties;
        private BindingList<MgLayerBase> _layers;

        public MgQueryControlImpl(IMapViewer viewer)
        {
            InitializeComponent();
            this.Title = Properties.Resources.TitleQuery;
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
            
            var map = viewer.GetMap();
            var layers = map.GetLayers();
            for (var i = 0; i < layers.GetCount(); i++)
            {
                _layers.Add(layers.GetItem(i));
            }
            cmbLayer.SelectedIndex = 0;
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

        static string MakeWktPolygon(double x1, double y1, double x2, double y2)
        {
            return "POLYGON((" + x1 + " " + y1 + ", " + x2 + " " + y1 + ", " + x2 + " " + y2 + ", " + x1 + " " + y2 + ", " + x1 + " " + y1 + "))";
        }

        private MgGeometry _filterGeometry;

        private void btnRectangle_Click(object sender, EventArgs e)
        {
            _viewer.DigitizeRectangle((llx, lly, urx, ury) =>
            {
                ClearFilterGeometry();
                _filterGeometry = _wktRw.Read(MakeWktPolygon(llx, lly, urx, ury));
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
            });
        }

        private void ClearFilterGeometry()
        {
            if (_filterGeometry != null)
                _filterGeometry.Dispose();
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

            sb.Append(prop.Name + " ");

            switch (op)
            {
                case QueryOperator.EqualTo:
                    sb.Append(" = ");
                    break;
                case QueryOperator.GreaterThan:
                    sb.Append(" > ");
                    break;
                case QueryOperator.GreaterThanOrEqualTo:
                    sb.Append(" >= ");
                    break;
                case QueryOperator.In:
                    sb.Append(" IN ");
                    break;
                case QueryOperator.LessThan:
                    sb.Append(" < ");
                    break;
                case QueryOperator.LessThanOrEqualTo:
                    sb.Append(" <= ");
                    break;
                case QueryOperator.Like:
                    sb.Append(" LIKE ");
                    break;
                case QueryOperator.NotEqualTo:
                    sb.Append(" <> ");
                    break;
            }

            if (op == QueryOperator.In)
            {
                sb.Append("(");
                sb.Append(value);
                sb.Append(")");
            }
            else
            {
                if (prop.DataType == MgPropertyType.String)
                {
                    sb.Append("'");
                    sb.Append(value);
                    sb.Append("'");
                }
                else
                {
                    sb.Append(value);
                }
            }

            return sb.ToString();
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
