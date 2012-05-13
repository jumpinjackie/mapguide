using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide;

namespace OSGeo.MapGuide.Viewer.Desktop
{
    public class MgDesktopMapViewerProvider : MgMapViewerProvider
    {
        private MgdMap _implMap;
        private MgRenderingService _renderSvc;
        private MgServiceFactory _fact;

        public MgDesktopMapViewerProvider(MgdMap map) : base(map) 
        {
            _implMap = map;
        }

        protected override void SubInit()
        {
            _fact = new MgServiceFactory();
            _renderSvc = (MgRenderingService)_fact.CreateService(MgServiceType.RenderingService);
        }
        
        private MgRenderingOptions _lastRenderOpts;

        private MgRenderingOptions CreateRenderingOptions(MgViewerRenderingOptions renderOpts)
        {
            if (null == _lastRenderOpts)
            {
                _lastRenderOpts = new MgRenderingOptions(renderOpts.Format, renderOpts.Behavior, renderOpts.Color);
            }
            else
            {
                var color = _lastRenderOpts.GetSelectionColor();
                if (!(color.Red == renderOpts.Color.Red &&
                    color.Green == renderOpts.Color.Green &&
                    color.Blue == renderOpts.Color.Blue &&
                    color.Alpha == renderOpts.Color.Alpha &&
                    _lastRenderOpts.GetBehavior() == renderOpts.Behavior &&
                    _lastRenderOpts.GetImageFormat() == renderOpts.Format))
                {
                    _lastRenderOpts = new MgRenderingOptions(renderOpts.Format, renderOpts.Behavior, renderOpts.Color);
                }
            }
            return _lastRenderOpts;
        }

        public override OSGeo.MapGuide.MgByteReader RenderDynamicOverlay(OSGeo.MapGuide.MgSelectionBase selection, MgViewerRenderingOptions args)
        {
            return _renderSvc.RenderDynamicOverlay(_implMap, selection as MgdSelection, CreateRenderingOptions(args));
        }

        public override void SetDisplaySize(int width, int height)
        {
            _implMap.SetDisplaySize(width, height);
        }

        public override OSGeo.MapGuide.MgSelectionBase CreateSelectionForMap()
        {
            return new MgdSelection(_implMap);
        }

        public override double GetMetersPerUnit()
        {
            return _implMap.GetMetersPerUnit();
        }

        public override void SetViewCenterXY(double x, double y)
        {
            _implMap.SetViewCenterXY(x, y);
        }

        public override void ClearSelection(OSGeo.MapGuide.MgSelectionBase sel)
        {
            var selImpl = sel as MgdSelection;
            if (selImpl != null)
                selImpl.Clear();
        }

        public override void SetViewScale(double scale)
        {
            _implMap.SetViewScale(scale);
        }

        public override void MakeGroupNormal(OSGeo.MapGuide.MgLayerGroup group)
        {
            _implMap.MakeGroupNormal(group);
        }

        public override MgService CreateService(int serviceType)
        {
            return _fact.CreateService(serviceType);
        }

        public override MgByteReader GenerateLegendImage(MgResourceIdentifier layerDefinition, double viewScale, int width, int height, string format, int geomType, int themeCategory)
        {
            return _renderSvc.GenerateLegendImage(layerDefinition, viewScale, width, height, format, geomType, themeCategory);
        }

        public override void SetGroupExpandInLegend(MgLayerGroup group, bool bExpanded)
        {
            MgdMap.SetGroupExpandInLegend(group, bExpanded);
        }

        public override void SetLayerExpandInLegend(MgLayerBase layer, bool bExpanded)
        {
            MgdMap.SetLayerExpandInLegend(layer, bExpanded);
        }

        public override bool LayerHasTooltips(MgLayerBase layer)
        {
            var layerImpl = layer as MgdLayer;
            if (layerImpl != null)
                return layerImpl.HasTooltips();

            throw new InvalidOperationException("The given layer is not of the expected implementation (" + typeof(MgdLayer).FullName + ")");
        }

        public override bool IsLayerPotentiallyVisibleAtScale(MgLayerBase layer, bool bConsiderParentGroupVisibility)
        {
            var layerImpl = layer as MgdLayer;
            if (layerImpl != null)
                return layerImpl.IsPotentiallyVisibleAtScale(_implMap.ViewScale, bConsiderParentGroupVisibility);

            throw new InvalidOperationException("The given layer is not of the expected implementation (" + typeof(MgdLayer).FullName + ")");
        }

        public override MgLayerBase CreateLayer(MgResourceIdentifier layerDefId)
        {
            return new MgdLayer(layerDefId, _resSvc);
        }
    }
}
