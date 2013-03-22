using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A generic component for measuring
    /// </summary>
    [ToolboxItem(true)]
    public class MgMeasureComponent : MgViewerComponent
    {
        public MgMeasureComponent()
        {
            this.Icon = Properties.Resources.measure;
            this.Label = this.ToolTipText = Strings.TitleMeasure;
            this.MeasureMode = MeasureMode.Line;
            this.PreferredUnits = MeasurementUnit.Meters;
        }

        protected override MgControlView CreateControlView()
        {
            switch (_measureMode)
            {
                case MeasureMode.Line:
                    return new MgLineMeasureControlImpl(this.Viewer, this.PreferredUnits);
                default:
                    throw new NotImplementedException();
            }
        }

        [Category("MapGuide Component Properties")] //NOXLATE
        [Description("The default units to measure in")] //NOXLATE
        [DefaultValue(MeasurementUnit.Meters)]
        [MgComponentProperty]
        public MeasurementUnit PreferredUnits
        {
            get;
            set;
        }

        private MeasureMode _measureMode;

        [Category("MapGuide Component Properties")] //NOXLATE
        [DefaultValue(MeasureMode.Line)]
        [Browsable(true)]
        [Description("The mode of measurement")] //NOXLATE
        [MgComponentProperty]
        public MeasureMode MeasureMode
        {
            get { return _measureMode; }
            set 
            {
                if (value == MeasureMode.Area)
                    throw new NotImplementedException(Strings.ErrorNotImplemented);
                _measureMode = value; 
            }
        }
    }

    /// <summary>
    /// Defines the valid types of measurements that can be done by the <see cref="T:OSGeo.MapGuide.Viewer.MgMeasureControl"/> component
    /// </summary>
    public enum MeasureMode
    {
        /// <summary>
        /// Measure distance by tracing 1 or more line segments
        /// </summary>
        Line,
        /// <summary>
        /// Measure area by tracing a polygon
        /// </summary>
        Area
    }
}
