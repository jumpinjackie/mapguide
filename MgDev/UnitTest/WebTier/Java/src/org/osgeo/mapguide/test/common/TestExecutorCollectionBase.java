package org.osgeo.mapguide.test.common;

public abstract class TestExecutorCollectionBase implements ITestExecutorCollection
{
    public String TestExecutionMode;
    public String DumpFile;

    public String getTestExecutionMode() { return this.TestExecutionMode; }
    public String getDumpFile() { return this.DumpFile; }

    public abstract String getApiType();

    public void Initialize(String testExecutionMode, String dumpFile)
    {
        this.TestExecutionMode = testExecutionMode;
        this.DumpFile = dumpFile;
    }

    protected abstract void SetupExecutors(String dbPath);

    public int Execute(MutableInteger testsRun, ITestLogger logger, boolean isEnterprise)
    {
        int exitStatus = 0;
        /*
        String dbPath = CommonUtility.GetDbPath(this.DumpFile);
        String dbName = CommonUtility.GetPath(dbPath);

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
                String testName = vm.GetString("TestName");
                String testType = vm.GetString("TestType");

                Console.WriteLine("Executing {0} test: {1}", testType, testName);
                using (var run = new TestExecutionRun(dbPath, this))
                {
                    try
                    {
                        exitStatus += run.RunTests(testName, logger, ref testsRun);
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(CommonUtility.ExceptionToString(ex));
                        exitStatus += 1;
                    }
                }
                status = vm.NextRow();
            }
            vm.SqlFinalize();
            vm = null;
            db = null;
        }
        */
        return exitStatus;
    }

    public abstract ITestExecutor GetTestExecutor(String opName);

    public abstract int ValidateRequest(SqliteDb db, String testName, int paramSetId, String operation, TestResult actualResult, ITestLogger logger);
    
    public abstract void Cleanup();
}