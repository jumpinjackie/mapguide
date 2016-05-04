using OSGeo.MapGuide.Test.Common;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Operations
{
    /*
    public class GetProperties : ServerAdminOperationExecutor<GetProperties>
    {
        public GetProperties(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }
    */
    public class Offline : ServerAdminOperationExecutor<Offline>
    {
        public Offline(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                _serverAdmin.TakeOffline();
                return new TestResult(CommonUtility.BooleanToString(_serverAdmin.IsOnline()), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class Online : ServerAdminOperationExecutor<Online>
    {
        public Online(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                _serverAdmin.BringOnline();
                return new TestResult(CommonUtility.BooleanToString(_serverAdmin.IsOnline()), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GetLog : ServerAdminOperationExecutor<GetLog>
    {
        public GetLog(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "LOGTYPE", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NUMENTRIES", param);

                MgByteReader reader = null;
                if (param["NUMENTRIES"] == null)
                {
                    reader = _serverAdmin.GetLog(param["LOGTYPE"]);
                }
                else
                {
                    reader = _serverAdmin.GetLog(param["LOGTYPE"], Convert.ToInt32(param["NUMENTRIES"]));
                }

                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GetLogByDate : ServerAdminOperationExecutor<GetLogByDate>
    {
        public GetLogByDate(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "LOGTYPE", param);
                _unitTestVm.ReadParameterValue(paramSetId, "FROMDATE", param);
                _unitTestVm.ReadParameterValue(paramSetId, "TODATE", param);

                string[] fromDatePieces = (param["FROMDATE"] ?? "").Split(',');
                string[] toDatePieces = (param["TODATE"] ?? "").Split(',');

                MgDateTime fromDate = null;
                MgDateTime toDate = null;

                if (fromDatePieces.Length == 3)
                {
                    fromDate = new MgDateTime(Convert.ToInt16(fromDatePieces[0]), Convert.ToInt16(fromDatePieces[1]), Convert.ToInt16(fromDatePieces[2]));
                }
                else if (fromDatePieces.Length == 7)
                {
                    fromDate = new MgDateTime(Convert.ToInt16(fromDatePieces[0]), Convert.ToInt16(fromDatePieces[1]), Convert.ToInt16(fromDatePieces[2]), Convert.ToInt16(fromDatePieces[3]), Convert.ToInt16(fromDatePieces[4]), Convert.ToInt16(fromDatePieces[5]), Convert.ToInt32(fromDatePieces[6]));
                }

                if (toDatePieces.Length == 3)
                {
                    toDate = new MgDateTime(Convert.ToInt16(toDatePieces[0]), Convert.ToInt16(toDatePieces[1]), Convert.ToInt16(toDatePieces[2]));
                }
                else if (toDatePieces.Length == 7)
                {
                    toDate = new MgDateTime(Convert.ToInt16(toDatePieces[0]), Convert.ToInt16(toDatePieces[1]), Convert.ToInt16(toDatePieces[2]), Convert.ToInt16(toDatePieces[3]), Convert.ToInt16(toDatePieces[4]), Convert.ToInt16(toDatePieces[5]), Convert.ToInt32(toDatePieces[6]));
                }

                MgByteReader reader = _serverAdmin.GetLog(param["LOGTYPE"] ?? "", fromDate, toDate);
                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class ClearLog : ServerAdminOperationExecutor<ClearLog>
    {
        public ClearLog(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "LOGTYPE", param);

                bool cleared = _serverAdmin.ClearLog(param["LOGTYPE"]);
                return new TestResult(CommonUtility.BooleanToString(cleared), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    /*
    public class EnumerateLogs : ServerAdminOperationExecutor<EnumerateLogs>
    {
        public EnumerateLogs(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }
     */

    public class DeleteLog : ServerAdminOperationExecutor<DeleteLog>
    {
        public DeleteLog(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "FILENAME", param);

                _serverAdmin.DeleteLog(param["FILENAME"]);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class RenameLog : ServerAdminOperationExecutor<RenameLog>
    {
        public RenameLog(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "OLDFILENAME", param);
                _unitTestVm.ReadParameterValue(paramSetId, "NEWFILENAME", param);

                _serverAdmin.RenameLog(param["OLDFILENAME"], param["NEWFILENAME"]);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class EnumeratePackages : ServerAdminOperationExecutor<EnumeratePackages>
    {
        public EnumeratePackages(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var packages = _serverAdmin.EnumeratePackages();
                return new TestResult(CommonUtility.MgStringCollectionToString(packages), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class DeletePackage : ServerAdminOperationExecutor<DeletePackage>
    {
        public DeletePackage(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "PACKAGENAME", param);

                _serverAdmin.DeletePackage(param["PACKAGENAME"]);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class LoadPackage : ServerAdminOperationExecutor<LoadPackage>
    {
        public LoadPackage(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "PACKAGENAME", param);

                _serverAdmin.LoadPackage(param["PACKAGENAME"]);
                return new TestResult();
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GetPackageStatus : ServerAdminOperationExecutor<GetPackageStatus>
    {
        public GetPackageStatus(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "PACKAGENAME", param);

                MgPackageStatusInformation status = _serverAdmin.GetPackageStatus(param["PACKAGENAME"]);
                string code = status.GetStatusCode();
                return new TestResult(code, "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class GetPackageLog : ServerAdminOperationExecutor<GetPackageLog>
    {
        public GetPackageLog(MgServerAdmin admin, string unitTestVm)
            : base(admin, unitTestVm)
        {
        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "PACKAGENAME", param);

                MgByteReader reader = _serverAdmin.GetPackageLog(param["PACKAGENAME"]);
                return TestResult.FromByteReader(reader);
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }
}
