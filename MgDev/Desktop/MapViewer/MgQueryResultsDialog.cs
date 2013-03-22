using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A dialog for displaying feature query results in a grid
    /// </summary>
    public partial class MgQueryResultsDialog : Form
    {
        private string _geomProp;
        private string[] _identityProps;
        private MgReader _reader;
        private IMapViewer _viewer;
        private Dictionary<string, int> _propMap;
        private MgLayerBase _layer;
        private int _limit;

        private string[] _columns;

        public MgQueryResultsDialog(IMapViewer viewer, MgLayerBase layer, MgReader reader, int limit)
        {
            InitializeComponent();
            _viewer = viewer;
            _reader = reader;
            _geomProp = layer.GetFeatureGeometryName();
            _layer = layer;
            _limit = limit;
            _propMap = new Dictionary<string, int>();
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            bgGridLoader.RunWorkerAsync(_reader);
        }

        private void bgGridLoader_DoWork(object sender, DoWorkEventArgs e)
        {
            var reader = (MgReader)e.Argument;
            var wktRw = new MgWktReaderWriter();
            var agfRw = new MgAgfReaderWriter();

            var cls = _layer.GetClassDefinition();
            var idProps = cls.GetIdentityProperties();
            var idNames = new List<string>();
            for (int i = 0; i < idProps.Count; i++)
            {
                var prop = idProps.GetItem(i);
                idNames.Add(prop.Name);
            }
            _identityProps = idNames.ToArray();

            var propNames = new List<string>();
            int propCount = reader.GetPropertyCount();
            //.net 2.0 compatibility hack for set-like behaviour
            var skipProps = new Dictionary<int, int>();
            for (int i = 0; i < propCount; i++)
            {
                var pt = reader.GetPropertyType(i);
                if (pt == MgPropertyType.Blob ||
                    pt == MgPropertyType.Clob ||
                    pt == MgPropertyType.Feature ||
                    pt == MgPropertyType.Null ||
                    pt == MgPropertyType.Raster)
                {
                    skipProps[i] = i;
                }
                else
                {
                    var name = reader.GetPropertyName(i);
                    _propMap.Add(name, pt);
                    propNames.Add(name);
                }
            }
            try
            {
                bgGridLoader.ReportProgress(-1, new InitGridAction() { ColumnNames = propNames.ToArray() });
                int read = 0;
                while (reader.ReadNext())
                {
                    object[] values = new object[propCount];
                    for (int i = 0; i < propCount; i++)
                    {
                        if (skipProps.ContainsKey(i))
                            continue;

                        if (reader.IsNull(i))
                        {
                            values[i] = null;
                        }
                        else
                        {
                            switch (reader.GetPropertyType(i))
                            {
                                case MgPropertyType.Boolean:
                                    values[i] = reader.GetBoolean(i);
                                    break;
                                case MgPropertyType.Byte:
                                    values[i] = reader.GetByte(i);
                                    break;
                                case MgPropertyType.DateTime:
                                    values[i] = ToClrDateTime(reader.GetDateTime(i));
                                    break;
                                case MgPropertyType.Decimal:
                                case MgPropertyType.Double:
                                    values[i] = reader.GetDouble(i);
                                    break;
                                case MgPropertyType.Geometry:
                                    {
                                        try
                                        {
                                            var agf = reader.GetGeometry(i);
                                            var geom = agfRw.Read(agf);
                                            var env = geom.Envelope();
                                            var envLL = env.LowerLeftCoordinate;
                                            var envUR = env.UpperRightCoordinate;
                                            var wkt = wktRw.Write(geom);

                                            values[i] = new BoxedGeometry()
                                            {
                                                Text = wkt,
                                                GeomBounds = new Bounds()
                                                {
                                                    LowerX = envLL.X,
                                                    LowerY = envLL.Y,
                                                    UpperX = envUR.X,
                                                    UpperY = envUR.Y
                                                }
                                            };
                                        }
                                        catch
                                        {
                                            values[i] = null;
                                        }
                                    }
                                    break;
                                case MgPropertyType.Int16:
                                    values[i] = reader.GetInt16(i);
                                    break;
                                case MgPropertyType.Int32:
                                    values[i] = reader.GetInt32(i);
                                    break;
                                case MgPropertyType.Int64:
                                    values[i] = reader.GetInt64(i);
                                    break;
                                case MgPropertyType.Single:
                                    values[i] = reader.GetSingle(i);
                                    break;
                                case MgPropertyType.String :
                                    values[i] = reader.GetString(i);
                                    break;
                            }
                        }
                    }
                    bgGridLoader.ReportProgress(-1, new AddRow() { Values = values });
                    read++;
                    if ((_limit > 0 && read >= _limit))
                        break;
                }
            }
            finally
            {
                reader.Close();
                wktRw.Dispose();
                agfRw.Dispose();
                wktRw = null;
                agfRw = null;
            }
        }

        private void bgGridLoader_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            var action = (Progress)e.UserState;
            if (action.Action == ProgressAction.InitGrid)
            {
                var columns = ((InitGridAction)action).ColumnNames;
                for (int i = 0; i < columns.Length; i++)
                {
                    grdResults.Columns.Add(columns[i], columns[i]);
                }
                _columns = columns;
            }
            else
            {
                grdResults.Rows.Add(((AddRow)action).Values);
            }
        }

        private void bgGridLoader_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            lblResults.Text = string.Format(Strings.CountResults, grdResults.Rows.Count);
        }

        static DateTime ToClrDateTime(MgDateTime dt)
        {
            return new DateTime(dt.Year, dt.Day, dt.Month, dt.Hour, dt.Minute, dt.Second, dt.Microsecond / 1000);
        }

        abstract class Progress
        {
            public abstract ProgressAction Action { get; }
        }

        class InitGridAction : Progress
        {
            public override ProgressAction Action
            {
                get { return ProgressAction.InitGrid; }
            }

            public string[] ColumnNames { get; set; }
        }

        class AddRow : Progress
        {
            public override ProgressAction Action
            {
                get { return ProgressAction.AddRow; }
            }

            public object[] Values { get; set; }
        }

        enum ProgressAction
        {
            InitGrid,
            AddRow
        }

        private void grdResults_SelectionChanged(object sender, EventArgs e)
        {
            btnZoom.Enabled = btnSelect.Enabled = (grdResults.SelectedRows.Count > 0);
        }

        private void btnSelect_Click(object sender, EventArgs e)
        {
            _viewer.ClearSelection();
            var sel = _viewer.GetSelection();

            foreach (DataGridViewRow row in grdResults.SelectedRows)
            {
                MgPropertyCollection idProps = new MgPropertyCollection();
                foreach (string name in _identityProps)
                {
                    int idx = Array.IndexOf(_columns, name);
                    if (idx >= 0)
                    {
                        int type = _propMap[name];
                        switch (type)
                        {
                            case MgPropertyType.Boolean:
                                idProps.Add(new MgBooleanProperty(name, Convert.ToBoolean(row.Cells[idx].Value)));
                                break;
                            case MgPropertyType.Byte:
                                idProps.Add(new MgByteProperty(name, Convert.ToByte(row.Cells[idx].Value)));
                                break;
                            case MgPropertyType.DateTime:
                                idProps.Add(new MgDateTimeProperty(name, FromClrDateTime((DateTime)row.Cells[idx].Value)));
                                break;
                            case MgPropertyType.Decimal:
                            case MgPropertyType.Double:
                                idProps.Add(new MgDoubleProperty(name, Convert.ToDouble(row.Cells[idx].Value)));
                                break;
                            case MgPropertyType.Int16:
                                idProps.Add(new MgInt16Property(name, Convert.ToInt16(row.Cells[idx].Value)));
                                break;
                            case MgPropertyType.Int32:
                                idProps.Add(new MgInt32Property(name, Convert.ToInt32(row.Cells[idx].Value)));
                                break;
                            case MgPropertyType.Int64:
                                idProps.Add(new MgInt64Property(name, Convert.ToInt64(row.Cells[idx].Value)));
                                break;
                            case MgPropertyType.Single:
                                idProps.Add(new MgSingleProperty(name, Convert.ToSingle(row.Cells[idx].Value)));
                                break;
                            case MgPropertyType.String:
                                idProps.Add(new MgStringProperty(name, Convert.ToString(row.Cells[idx].Value)));
                                break;
                        }
                    }
                }
                if (idProps.Count == _identityProps.Length)
                {
                    sel.AddFeatureIds(_layer, _layer.FeatureClassName, idProps);
                }
                else
                {
                    idProps.Dispose();
                    idProps = null;
                }
            }
            _viewer.UpdateSelection(true);
        }

        static MgDateTime FromClrDateTime(DateTime dt)
        {
            return new MgDateTime((short)dt.Year, (short)dt.Month, (short)dt.Day, (short)dt.Hour, (short)dt.Minute, (short)dt.Second, dt.Millisecond * 1000);
        }

        private void btnZoom_Click(object sender, EventArgs e)
        {
            //Collect the bounding boxes of all geometries and zoom to that
            MgEnvelope env = new MgEnvelope();
            int idx = Array.IndexOf(_columns, _geomProp);
            if (idx < 0)
            {
                MessageBox.Show(string.Format(Strings.ErrorGeometryPropertyNotFound, _geomProp));
                return;
            }

            foreach (DataGridViewRow row in grdResults.SelectedRows)
            {
                var geom = row.Cells[idx].Value as BoxedGeometry;
                MgEnvelope tmp = new MgEnvelope(geom.GeomBounds.LowerX, geom.GeomBounds.LowerY, geom.GeomBounds.UpperX, geom.GeomBounds.UpperY);
                env.ExpandToInclude(tmp);
            }

            var ll = env.LowerLeftCoordinate;
            var ur = env.UpperRightCoordinate;

            _viewer.ZoomToExtents(ll.X, ll.Y, ur.X, ur.Y);
        }
    }

    internal struct Bounds
    {
        public double LowerX;
        public double LowerY;
        public double UpperX;
        public double UpperY;
    }

    internal class BoxedGeometry
    {
        public override string ToString()
        {
            return this.Text;
        }

        public string Text { get; set; }

        public Bounds GeomBounds { get; set; }
    }
}
