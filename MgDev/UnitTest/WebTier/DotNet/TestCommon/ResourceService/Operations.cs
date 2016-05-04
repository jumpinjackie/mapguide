using OSGeo.MapGuide.Test.Common;
using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Operations
{
    public class EnumerateResources : ResourceServiceOperationExecutor<EnumerateResources>
    {
        public EnumerateResources(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "TYPE", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DEPTH", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }
                MgByteReader byteReader = _resourceService.EnumerateResources(resId, Convert.ToInt32(param["DEPTH"]), param["TYPE"] ?? "");

                return TestResult.FromByteReader(byteReader, "GETRESOURCEDATA");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class SetResource : ResourceServiceOperationExecutor<SetResource>
    {
        public SetResource(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "CONTENT", param, true);
                _unitTestVm.ReadParameterValue(paramSetId, "HEADER", param, true);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader content = CommonUtility.GetByteReaderFromPath(param["CONTENT"]);
                MgByteReader header = CommonUtility.GetByteReaderFromPath(param["HEADER"]);

                _resourceService.SetResource(resId, content, header);

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class DeleteResource : ResourceServiceOperationExecutor<DeleteResource>
    {
        public DeleteResource(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
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

                _resourceService.DeleteResource(resId);

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class GetResourceContent : ResourceServiceOperationExecutor<GetResourceContent>
    {
        public GetResourceContent(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "PREPROCESS", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader reader = _resourceService.GetResourceContent(resId);

                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class GetResourceHeader : ResourceServiceOperationExecutor<GetResourceHeader>
    {
        public GetResourceHeader(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
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

                MgByteReader reader = _resourceService.GetResourceHeader(resId);

                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class EnumerateResourceData : ResourceServiceOperationExecutor<EnumerateResourceData>
    {
        public EnumerateResourceData(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
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

                MgByteReader reader = _resourceService.EnumerateResourceData(resId);

                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class GetResourceData : ResourceServiceOperationExecutor<GetResourceData>
    {
        public GetResourceData(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DATANAME", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader reader = _resourceService.GetResourceData(resId, param["DATANAME"]);

                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class SetResourceData : ResourceServiceOperationExecutor<SetResourceData>
    {
        public SetResourceData(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DATANAME", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DATATYPE", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DATA", param, true);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                string extension = CommonUtility.GetExtension(param["DATANAME"]);
                string mimeType = CommonUtility.GetMimeType(extension);
                MgByteSource dataSource = new MgByteSource(param["DATA"]);
                dataSource.SetMimeType(mimeType);
                MgByteReader dataReader = dataSource.GetReader();

                _resourceService.SetResourceData(resId, param["DATANAME"], param["DATATYPE"], dataReader);

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class RenameResourceData : ResourceServiceOperationExecutor<RenameResourceData>
    {
        public RenameResourceData(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "OLDDATANAME", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWDATANAME", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                _resourceService.RenameResourceData(resId, param["OLDDATANAME"], param["NEWDATANAME"], false);

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class DeleteResourceData : ResourceServiceOperationExecutor<DeleteResourceData>
    {
        public DeleteResourceData(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DATANAME", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                _resourceService.DeleteResourceData(resId, param["DATANAME"] ?? "");

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class GetRepositoryContent : ResourceServiceOperationExecutor<GetRepositoryContent>
    {
        public GetRepositoryContent(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
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

                MgByteReader result = _resourceService.GetRepositoryContent(resId);

                return TestResult.FromByteReader(result);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class GetRepositoryHeader : ResourceServiceOperationExecutor<GetRepositoryHeader>
    {
        public GetRepositoryHeader(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
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

                MgByteReader result = _resourceService.GetRepositoryHeader(resId);

                return TestResult.FromByteReader(result);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class UpdateRepository : ResourceServiceOperationExecutor<UpdateRepository>
    {
        public UpdateRepository(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "CONTENT", param, true);
                _unitTestVm.ReadParameterValue(paramSetId, "HEADER", param, true);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                MgByteReader content = CommonUtility.GetByteReaderFromPath(param["CONTENT"]);
                MgByteReader header = CommonUtility.GetByteReaderFromPath(param["HEADER"]);

                _resourceService.UpdateRepository(resId, content, header);

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class EnumerateResourceReferences : ResourceServiceOperationExecutor<EnumerateResourceReferences>
    {
        public EnumerateResourceReferences(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
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

                MgByteReader reader = _resourceService.EnumerateReferences(resId);

                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class MoveResource : ResourceServiceOperationExecutor<MoveResource>
    {
        public MoveResource(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "SOURCE", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DESTINATION", param);

                MgResourceIdentifier srcId = null;
                if (param["SOURCE"] != null)
                {
                    srcId = new MgResourceIdentifier(param["SOURCE"]);
                }

                MgResourceIdentifier dstId = null;
                if (param["DESTINATION"] != null)
                {
                    dstId = new MgResourceIdentifier(param["DESTINATION"]);
                }

                _resourceService.MoveResource(srcId, dstId, false);

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class CopyResource : ResourceServiceOperationExecutor<CopyResource>
    {
        public CopyResource(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "SOURCE", param);
                _unitTestVm.ReadParameterValue(paramSetId, "DESTINATION", param);

                MgResourceIdentifier srcId = null;
                if (param["SOURCE"] != null)
                {
                    srcId = new MgResourceIdentifier(param["SOURCE"]);
                }

                MgResourceIdentifier dstId = null;
                if (param["DESTINATION"] != null)
                {
                    dstId = new MgResourceIdentifier(param["DESTINATION"]);
                }

                _resourceService.CopyResource(srcId, dstId, false);

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class ChangeResourceOwner : ResourceServiceOperationExecutor<ChangeResourceOwner>
    {
        public ChangeResourceOwner(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "RESOURCEID", param);
                _unitTestVm.ReadParameterValue(paramSetId, "OWNER", param);
                _unitTestVm.ReadParameterValue(paramSetId, "INCLUDEDESCENDANTS", param);

                MgResourceIdentifier resId = null;
                if (param["RESOURCEID"] != null)
                {
                    resId = new MgResourceIdentifier(param["RESOURCEID"]);
                }

                _resourceService.ChangeResourceOwner(resId, param["OWNER"], (param["INCLUDEDESCENDANTS"] == "1"));

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class InheritPermissionsFrom : ResourceServiceOperationExecutor<InheritPermissionsFrom>
    {
        public InheritPermissionsFrom(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
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

                _resourceService.InheritPermissionsFrom(resId);

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }

    public class ApplyResourcePackage : ResourceServiceOperationExecutor<ApplyResourcePackage>
    {
        public ApplyResourcePackage(MgResourceService resSvc, string unitTestVm)
            : base(resSvc, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "PACKAGE", param, true);

                MgByteReader reader = CommonUtility.GetByteReaderFromPath(param["PACKAGE"] ?? "", false);

                _resourceService.ApplyResourcePackage(reader);

                return TestResult.FromByteReader(null);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
            catch (Exception ex)
            {
                return TestResult.FromException(ex);
            }
        }
    }
}
