using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common
{
    /// <summary>
    /// The base class of all MapGuide API test executors
    /// </summary>
    public abstract class PlatformApiTestExecutor : TestExecutorBase
    {
        protected string _unitTestVmPath;
        private SqliteDb _unitTestDb;
        protected SqliteVm _unitTestVm;

        protected PlatformApiTestExecutor(string opName, string apiType, string unitTestVm)
        {
            _opName = opName;
            _apiType = apiType;
            _unitTestVmPath = unitTestVm;

            _unitTestDb = new SqliteDb();
            _unitTestDb.Open(_unitTestVmPath);

            _unitTestVm = new SqliteVm(_unitTestDb, true);
        }

        public override void Dispose()
        {
            _unitTestVm.SqlFinalize();
            _unitTestVm = null;
            try
            {
                _unitTestDb.Close();
            }
            catch { }
            _unitTestDb = null;
        }

        private string _apiType;

        public override string Api
        {
            get { return _apiType; }
        }

        private string _opName;

        public override string OperationName
        {
            get { return _opName; }
        }
    }
}
