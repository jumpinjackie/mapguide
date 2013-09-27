using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Text;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A generic component that allows the user to annotate the map with redlines
    /// </summary>
    [ToolboxItem(true)]
    public partial class MgRedlineComponent : MgViewerComponent
    {
        //TODO: Feature parity with the Fusion implementation
        // - Configurable Data Store choice
        // - Configurable Geometry Types
        // - Redline import
        // - Redline export
        // - Configurable default style template

        public MgRedlineComponent()
        {
            this.Icon = Properties.Resources.redline;
            this.Label = this.ToolTipText = Strings.TitleRedline;

            this.StylizationType = OSGeo.MapGuide.Viewer.Redlining.RedlineStylizationType.Advanced;
            this.UseDefaultSettings = false;
            this.DefaultDataStoreFormat = OSGeo.MapGuide.Viewer.Redlining.RedlineDataStoreFormat.SDF;
            this.DefaultGeometryTypes = MgFeatureGeometricType.Point | MgFeatureGeometricType.Curve | MgFeatureGeometricType.Surface;
        }

        [Category("MapGuide Component Properties")] //NOXLATE
        [Description("The type of stylization to use for redline features")] //NOXLATE
        [DefaultValue(Redlining.RedlineStylizationType.Advanced)]
        [MgComponentProperty]
        public Redlining.RedlineStylizationType StylizationType
        {
            get;
            set;
        }

        [Category("MapGuide Component Properties")] //NOXLATE
        [Description("Indicates whether to use the default settings provided by this component. Setting this to true will bypass any prompts for such information when creating a new redline layer")] //NOXLATE
        [DefaultValue(false)]
        [MgComponentProperty]
        public bool UseDefaultSettings
        {
            get;
            set;
        }

        [Category("MapGuide Component Properties")] //NOXLATE
        [Description("The default data store format to use to store redline features. Only takes effect if UseDefaultSettings is true")] //NOXLATE
        [DefaultValue(Redlining.RedlineDataStoreFormat.SDF)]
        [MgComponentProperty]
        public Redlining.RedlineDataStoreFormat DefaultDataStoreFormat
        {
            get;
            set;
        }

        [Category("MapGuide Component Properties")] //NOXLATE
        [Description("Indicates the default geometry storage types allowed for a redline data store")] //NOXLATE
        [DefaultValue(MgFeatureGeometricType.Curve | MgFeatureGeometricType.Point | MgFeatureGeometricType.Surface)]
        [MgComponentProperty]
        public int DefaultGeometryTypes
        {
            get;
            set;
        }

        protected override MgControlView CreateControlView()
        {
            return new MgRedlineControlImpl(this);
        }
    }
}
