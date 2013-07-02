using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SqliteDotNet;

namespace OSGeo.MapGuide.Test.Common
{
    /// <summary>
    /// Executes a <see cref="T:OSGeo.MapGuide.Test.Common.ITestExecutorCollection"/> instance
    /// </summary>
    public class TestExecutionRun : IDisposable
    {
        private ITestExecutorCollection _executors;
        private SqliteDb _unitTestDb;
        private SqliteVm _unitTestVm;
        private SqliteVm _unitTestResultVm;
        private bool _generateDump;

        public TestExecutionRun(string dbPath, ITestExecutorCollection exec, bool generateDump = false)
        {
            _executors = exec;
            _generateDump = generateDump;
            _unitTestDb = new SqliteDb();
            _unitTestDb.Open(dbPath);

            _unitTestVm = new SqliteVm(_unitTestDb, false);
            _unitTestResultVm = new SqliteVm(_unitTestDb, false);
        }

        public void Dispose()
        {
            _unitTestResultVm.SqlFinalize();
            _unitTestResultVm = null;
            _unitTestVm.SqlFinalize();
            _unitTestVm = null;
            try
            {
                _unitTestDb.Close();
            }
            catch { }
            _unitTestDb = null;
        }

        public int RunTests(string testName, ITestLogger logger, ref int testsRun)
        {
            TestResult actualResult = null;
            int exitStatus = 0;
            try
            {
                //Add the test in the log file
                string msg = "\n\n************************************************************\n" +
                             string.Format("Executing {0}\n", testName) +
                             "************************************************************\n\n";

                logger.Write(msg);

                //Get the list of operations to be executed for this test. This list should comma separated, no spaces.
                _unitTestVm.Execute("Select ParamSets from TestCase where TestName=\"{0}\"", testName);
                string sets = _unitTestVm.GetString("ParamSets");

                //Extract the paramset ids
                int[] paramSet = sets.Split(',').Where(x => !string.IsNullOrEmpty(x)).Select(x => Convert.ToInt32(x)).ToArray();
                foreach (int paramSetId in paramSet)
                {
                    _unitTestVm.Execute("Select ParamValue from Params where ParamSet={0} AND ParamName=\"OPERATION\"", paramSetId);
                    string paramValue = _unitTestVm.GetString("ParamValue");

                    testsRun++;

                    //Add the operation to the log file
                    AddLogFileEntry(logger, paramValue, paramSetId, _unitTestResultVm);

                    var exec = _executors.GetTestExecutor(paramValue);
                    actualResult = exec.Execute(paramSetId);

                    exitStatus += _executors.ValidateRequest(_unitTestDb, testName, paramSetId, paramValue, actualResult, logger);
                }

                return exitStatus;
            }
            catch (MgException ex)
            {
                throw new UnitTestException(string.Format("Exception from MapGuide:\n{0}", ex.GetDetails()));
            }
        }

        private static void AddLogFileEntry(ITestLogger logger, string operation, int paramSetId, SqliteVm vm)
        {
            logger.Write("\nParamSet: {0}\n", paramSetId);
            int status = vm.Execute("Select * from Params where ParamSet={0}", paramSetId);

            while (status == Sqlite.Row)
            {
                string paramName = vm.GetString("ParamName");
                string paramValue = vm.GetString("ParamValue");

                logger.Write("{0}: {1}\n", paramName, paramValue);
                status = vm.NextRow();
            }
        }

        public void Cleanup()
        {
            if (_executors != null)
            {
                _executors.Cleanup();
                _executors = null;
            }
        }
    }
}
