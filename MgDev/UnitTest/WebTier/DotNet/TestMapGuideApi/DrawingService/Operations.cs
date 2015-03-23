using OSGeo.MapGuide.Test.Common;
using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Operations
{
    public class DescribeDrawing : DrawingServiceOperationExecutor<DescribeDrawing>
    {
        public DescribeDrawing(MgDrawingService drawSvc, string vm)
            : base(drawSvc, vm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader reader = _drawingService.DescribeDrawing(resId);
                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GetDrawing : DrawingServiceOperationExecutor<GetDrawing>
    {
        public GetDrawing(MgDrawingService drawSvc, string vm)
            : base(drawSvc, vm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader reader = _drawingService.GetDrawing(resId);
                return TestResult.FromByteReader(reader, "GETDRAWING");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class EnumerateDrawingLayers : DrawingServiceOperationExecutor<EnumerateDrawingLayers>
    {
        public EnumerateDrawingLayers(MgDrawingService drawSvc, string vm)
            : base(drawSvc, vm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "SECTION", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgStringCollection coll = _drawingService.EnumerateLayers(resId, param["SECTION"] ?? "");
                MgByteReader reader = coll.ToXml();
                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GetDrawingLayer : DrawingServiceOperationExecutor<GetDrawingLayer>
    {
        public GetDrawingLayer(MgDrawingService drawSvc, string vm)
            : base(drawSvc, vm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "SECTION", param);
                _unitTestVm.ReadParameterValue(paramSetId, "LAYER", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader reader = _drawingService.GetLayer(resId, param["SECTION"], param["LAYER"]);
                return TestResult.FromByteReader(reader, "GETDRAWINGLAYER");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GetDrawingSection : DrawingServiceOperationExecutor<GetDrawingSection>
    {
        public GetDrawingSection(MgDrawingService drawSvc, string vm)
            : base(drawSvc, vm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "SECTION", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader reader = _drawingService.GetSection(resId, param["SECTION"]);
                return TestResult.FromByteReader(reader, "GETDRAWINGSECTION");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class EnumerateDrawingSections : DrawingServiceOperationExecutor<EnumerateDrawingSections>
    {
        public EnumerateDrawingSections(MgDrawingService drawSvc, string vm)
            : base(drawSvc, vm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader reader = _drawingService.EnumerateSections(resId);
                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class EnumerateDrawingSectionResources : DrawingServiceOperationExecutor<EnumerateDrawingSectionResources>
    {
        public EnumerateDrawingSectionResources(MgDrawingService drawSvc, string vm)
            : base(drawSvc, vm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "SECTION", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader reader = _drawingService.EnumerateSectionResources(resId, param["SECTION"] ?? "");
                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GetDrawingSectionResource : DrawingServiceOperationExecutor<GetDrawingSectionResource>
    {
        public GetDrawingSectionResource(MgDrawingService drawSvc, string vm)
            : base(drawSvc, vm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCENAME", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader reader = _drawingService.GetSectionResource(resId, param["RESOURCENAME"]);
                return TestResult.FromByteReader(reader, "GETDRAWINGSECTIONRESOURCE");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }
}
