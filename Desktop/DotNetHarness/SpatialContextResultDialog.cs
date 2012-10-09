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
    public partial class SpatialContextResultDialog : Form
    {
        private SpatialContextResultDialog()
        {
            InitializeComponent();
        }

        private MgSpatialContextReader _reader;
        private MgAgfReaderWriter _agfRW;
        private MgWktReaderWriter _wktRW;

        public SpatialContextResultDialog(MgSpatialContextReader reader)
            : this()
        {
            _reader = reader;
            _wktRW = new MgWktReaderWriter();
            _agfRW=  new MgAgfReaderWriter();

            StringBuilder sb = new StringBuilder();
            while (_reader.ReadNext())
            {
                MgGeometry geom = null;
                if (_reader.ExtentType == MgSpatialContextExtentType.scStatic)
                {
                    geom = _agfRW.Read(_reader.Extent);
                }

                sb.AppendFormat("Spatial Context{0}\tName: {1}{0}\tDescription: {2}{0}\tCoordinate System: {3}{0}\tCoordinate System Wkt: {4}{0}\tExtent Type {5}{0}\tExtents (if applicable): {6}{0}\tXY Tolerance: {7}{0}\tZ Tolerance: {8}{0}\tIs Active: {9}{0}{0}",
                    Environment.NewLine,
                    _reader.Name,
                    _reader.Description,
                    _reader.CoordinateSystem,
                    _reader.CoordinateSystemWkt,
                    _reader.ExtentType == MgSpatialContextExtentType.scDynamic ? "Dynamic" : "Static",
                    geom != null ? _wktRW.Write(geom) : "N/A",
                    _reader.XYTolerance,
                    _reader.ZTolerance,
                    _reader.IsActive());
            }
            textBox1.Text = sb.ToString();
            _reader.Close();
        }
    }
}
