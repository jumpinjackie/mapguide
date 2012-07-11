using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer.Desktop
{
    /// <summary>
    /// Represents a stack of transient map states. This is used to manipulate the display parameters
    /// of a <see cref="T:OSGeo.MapGuide.MgdMap"/> in a manner that can be undone upon disposal
    /// of this stack, allowing your map to be used for <see cref="T:OSGeo.MapGuide.MgdRenderingService.RenderMap()"/>
    /// calls without permanently altering state.
    /// </summary>
    public class MgdTransientMapState : MgTransientMapState<MgdMap>
    {
        internal MgdTransientMapState(MgdMap map) : base(map) { } 

        protected override void ApplyViewCenter(double x, double y)
        {
            _map.SetViewCenterXY(x, y);
        }

        protected override void ApplyViewScale(double scale)
        {
            _map.SetViewScale(scale);
        }

        protected override void ApplyDisplaySize(int width, int height)
        {
            _map.SetDisplaySize(width, height);
        }

        protected override void ApplyDPI(int dpi)
        {
            _map.DisplayDpi = dpi;
        }
    }
}
