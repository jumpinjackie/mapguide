using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace OSGeo.MapGuide.Viewer
{
    internal partial class MgLineMeasureControlImpl : MgControlView
    {
        private IMapViewer _viewer;
        private MgCoordinateSystem _mapCs;
        private BindingList<MeasuredLineSegment> _segments;

        public MgLineMeasureControlImpl(IMapViewer viewer, MeasurementUnit preferredUnit)
        {
            InitializeComponent();
            this.Title = Strings.TitleMeasure;
            _viewer = viewer;
            _segments = new BindingList<MeasuredLineSegment>();
            cmbUnits.DataSource = Enum.GetValues(typeof(MeasurementUnit));
            cmbUnits.SelectedItem = preferredUnit;
            lstSegments.DataSource = _segments;
            _mapCs = _viewer.GetProvider().GetMapCoordinateSystem();
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            btnStart.Enabled = false;
            _segments.Clear();
            _viewer.DigitizeLineString((coordinates) => //Completed
            {
                btnStart.Enabled = true;
            }, (x1, y1, x2, y2) => //Segment completed
            {
                double dist = 0.0;
                if (_mapCs.GetType() == MgCoordinateSystemType.Geographic)
                    dist = _mapCs.MeasureGreatCircleDistance(x1, y1, x2, y2);
                else
                    dist = _mapCs.MeasureEuclideanDistance(x1, y1, x2, y2);

                //Convert this distance to meters
                dist = _mapCs.ConvertCoordinateSystemUnitsToMeters(dist);
                //Add to list of line segments
                _segments.Add(new MeasuredLineSegment() { MapDistanceMeters = dist, Units = (MeasurementUnit)cmbUnits.SelectedItem });

                lblUnits.Text = TotalUnits(_segments).ToString();
            });
        }

        private static double TotalUnits(IEnumerable<MeasuredLineSegment> segments)
        {
            double total = 0.0;
            foreach (var seg in segments)
            {
                total += seg.DisplayUnits;
            }
            return total;
        }

        private void cmbUnits_SelectedIndexChanged(object sender, EventArgs e)
        {
            //Update segments units, then total
            foreach (var seg in _segments)
            {
                seg.Units = (MeasurementUnit)cmbUnits.SelectedItem;
            }
            lblUnits.Text = TotalUnits(_segments).ToString(); 
        }
    }

    /// <summary>
    /// Defines a unit of measurement
    /// </summary>
    public enum MeasurementUnit
    {
        /// <summary>
        /// Feet
        /// </summary>
        Feet,
        /// <summary>
        /// Meters
        /// </summary>
        Meters,
        /// <summary>
        /// Kilometers
        /// </summary>
        Kilometers,
        /// <summary>
        /// Miles
        /// </summary>
        Miles
    }

    internal class MeasuredLineSegment : INotifyPropertyChanged
    {
        public double MapDistanceMeters { get; set; }

        private MeasurementUnit _units;

        public MeasurementUnit Units
        {
            get { return _units; }
            set 
            {
                if (!value.Equals(_units))
                {
                    _units = value;
                    var h = this.PropertyChanged;
                    if (h != null) 
                    {
                        h(this, new PropertyChangedEventArgs("Units")); //NOXLATE
                        h(this, new PropertyChangedEventArgs("DisplayUnits")); //NOXLATE
                    }
                }
            }
        }

        const double M_TO_KM = 0.001;
        const double M_TO_MI = 0.000621371192;

        public double DisplayUnits
        {
            get
            {
                var dist = this.MapDistanceMeters;
                switch (this.Units)
                {
                    case MeasurementUnit.Kilometers:
                        dist *= M_TO_KM;
                        break;
                    case MeasurementUnit.Miles:
                        dist *= M_TO_MI;
                        break;
                }
                return dist;
            }
        }

        public override string ToString()
        {
            return string.Format("{0} {1}", this.DisplayUnits, this.Units);
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
