using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;

namespace DotNetHarness
{
    public partial class ReaderResponseDialog : Form
    {
        private ReaderResponseDialog()
        {
            InitializeComponent();
            _wktRW = new MgWktReaderWriter();
            _agfRW = new MgAgfReaderWriter();
        }

        private MgReader _reader;
        private MgWktReaderWriter _wktRW;
        private MgAgfReaderWriter _agfRW;

        public ReaderResponseDialog(MgReader reader)
            : this()
        {
            _reader = reader;
        }

        protected override void OnLoad(EventArgs e)
        {
            for (int i = 0; i < _reader.GetPropertyCount(); i++)
            {
                string name = _reader.GetPropertyName(i);

                grdResults.Columns.Add(name, name);
            }

            int count = 0;
            try
            {
                this.Cursor = Cursors.WaitCursor;
                while (_reader.ReadNext())
                {
                    count++;
                    object[] values = new object[_reader.GetPropertyCount()];
                    for (int i = 0; i < _reader.GetPropertyCount(); i++)
                    {
                        if (_reader.IsNull(i))
                        {
                            values[i] = null;
                        }
                        else
                        {
                            int pt = _reader.GetPropertyType(i);
                            switch (pt)
                            {
                                case MgPropertyType.Blob:
                                    values[i] = "BLOB VALUE";
                                    break;
                                case MgPropertyType.Boolean:
                                    values[i] = _reader.GetBoolean(i);
                                    break;
                                case MgPropertyType.Byte:
                                    values[i] = _reader.GetByte(i);
                                    break;
                                case MgPropertyType.Clob:
                                    values[i] = "CLOB VALUE";
                                    break;
                                case MgPropertyType.DateTime:
                                    var mdt = _reader.GetDateTime(i);
                                    values[i] = mdt.ToString();
                                    break;
                                case MgPropertyType.Decimal:
                                case MgPropertyType.Double:
                                    values[i] = _reader.GetDouble(i);
                                    break;
                                case MgPropertyType.Feature:
                                    values[i] = "NESTED FEATURE READER";
                                    break;
                                case MgPropertyType.Geometry:
                                    MgByteReader agf = _reader.GetGeometry(i);
                                    MgGeometry geom = _agfRW.Read(agf);
                                    values[i] = _wktRW.Write(geom);
                                    break;
                                case MgPropertyType.Int16:
                                    values[i] = _reader.GetInt16(i);
                                    break;
                                case MgPropertyType.Int32:
                                    values[i] = _reader.GetInt32(i);
                                    break;
                                case MgPropertyType.Int64:
                                    values[i] = _reader.GetInt64(i);
                                    break;
                                case MgPropertyType.Raster:
                                    values[i] = "RASTER IMAGE";
                                    break;
                                case MgPropertyType.Single:
                                    values[i] = _reader.GetSingle(i);
                                    break;
                                case MgPropertyType.String:
                                    values[i] = _reader.GetString(i);
                                    break;
                            }
                        }
                    }
                    grdResults.Rows.Add(values);
                }
            }
            finally
            {
                this.Cursor = Cursors.Default;
                _reader.Close();
                toolStripStatusLabel1.Text = count + " features returned";
            }
        }
    }
}
