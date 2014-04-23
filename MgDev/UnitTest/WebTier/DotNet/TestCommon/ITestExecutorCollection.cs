using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common
{
    /// <summary>
    /// A collection of test executors
    /// </summary>
    public interface ITestExecutorCollection
    {
        /// <summary>
        /// Initializes this collection of test executors
        /// </summary>
        /// <param name="testExecutionMode"></param>
        /// <param name="dumpFile"></param>
        void Initialize(string testExecutionMode, string dumpFile);

        /// <summary>
        /// Gets the dump file path
        /// </summary>
        string DumpFile { get; }

        /// <summary>
        /// Gets the test execution mode
        /// </summary>
        string TestExecutionMode { get; }

        /// <summary>
        /// Gets the test executor of the specified operation name
        /// </summary>
        /// <param name="opName"></param>
        /// <returns></returns>
        ITestExecutor GetTestExecutor(string opName);

        /// <summary>
        /// Executes the test suite as defined in the dump file
        /// </summary>
        /// <param name="testsRun"></param>
        /// <param name="logger"></param>
        /// <param name="isEnterprise"></param>
        /// <returns></returns>
        int Execute(ref int testsRun, ITestLogger logger, bool isEnterprise);

        /// <summary>
        /// Validates a test execution result
        /// </summary>
        /// <param name="db"></param>
        /// <param name="testName"></param>
        /// <param name="paramSetId"></param>
        /// <param name="operation"></param>
        /// <param name="actualResult"></param>
        /// <param name="logger"></param>
        /// <returns></returns>
        int ValidateRequest(SqliteDb db, string testName, int paramSetId, string operation, TestResult actualResult, ITestLogger logger);

        /// <summary>
        /// Performs necessary cleanup
        /// </summary>
        void Cleanup();
    }

    public abstract class TestExecutorCollectionBase : ITestExecutorCollection
    {
        public string TestExecutionMode { get; protected set; }

        public string DumpFile { get; protected set; }

        public abstract string ApiType { get; }

        public void Initialize(string testExecutionMode, string dumpFile)
        {
            this.TestExecutionMode = testExecutionMode;
            this.DumpFile = dumpFile;
        }

        protected abstract void SetupExecutors(string dbPath);

        public int Execute(ref int testsRun, ITestLogger logger, bool isEnterprise)
        {
            int exitStatus = 0;
            string dbPath = CommonUtility.GetDbPath(this.DumpFile);
            string dbName = CommonUtility.GetPath(dbPath);

            if (File.Exists(dbName))
            {
                var db = new SqliteDb();
                db.Open(dbName);

                var vm = new SqliteVm(db, true);

                int status = vm.Execute("Select TestName, TestType from TestCase where TestType=\"{0}\" order by ExecuteSequence", this.ApiType);

                //NOTE: We can't share the SqliteVm instance among our executor objects as this messes up query results
                //we must be able to re-create a new SqliteVm for each executor, so we pass down the db path
                SetupExecutors(dbName);

                while (status == Sqlite.Row)
                {
                    string testName = vm.GetString("TestName");
                    string testType = vm.GetString("TestType");

                    Console.WriteLine("Executing {0} test: {1}", testType, testName);
                    using (var run = new TestExecutionRun(dbPath, this))
                    {
                        try
                        {
                            exitStatus += run.RunTests(testName, logger, ref testsRun);
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine(ex.ToString());
                            exitStatus += 1;
                        }
                    }
                    status = vm.NextRow();
                }
                vm.SqlFinalize();
                vm = null;
                db = null;
            }
            return exitStatus;
        }

        public abstract ITestExecutor GetTestExecutor(string opName);

        public abstract int ValidateRequest(SqliteDb db, string testName, int paramSetId, string operation, TestResult actualResult, ITestLogger logger);
        
        public abstract void Cleanup();
    }
}
