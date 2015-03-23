package org.osgeo.mapguide.test.common;

import java.io.File;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.test.*;

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
        
        try {

            String dbPath = CommonUtility.GetDbPath(this.DumpFile);
            String dbName = CommonUtility.GetPath(dbPath);

            if (CommonUtility.FileExists(dbName))
            {
                SQLiteConnection db = new SQLiteConnection(new File(dbName));
                try {
                    db.open(false);

                    SQLiteStatement stmt = db.prepare(String.format("Select TestName, TestType from TestCase where TestType=\"%s\" order by ExecuteSequence", this.getApiType()));

                    //NOTE: We can't share the SqliteVm instance among our executor objects as this messes up query results
                    //we must be able to re-create a new SqliteVm for each executor, so we pass down the db path
                    SetupExecutors(dbName);

                    while (stmt.step()) {
                        String testName = CommonUtility.GetStringFromStatement(stmt, "TestName");
                        String testType = CommonUtility.GetStringFromStatement(stmt, "TestType");
                        Console.WriteLine("Executing %s test: %s", testType, testName);
                        try (TestExecutionRun run = new TestExecutionRun(dbPath, this))
                        {
                            try
                            {
                                exitStatus += run.RunTests(testName, logger, testsRun);
                            }
                            catch (Exception ex)
                            {
                                Console.WriteLine(CommonUtility.ExceptionToString(ex));
                                exitStatus += 1;
                            }
                        }
                    }
                } finally {
                    db.dispose();
                }
            }

        } catch (Exception ex) {
            logger.WriteLine("Exception occurred during test. %s", CommonUtility.ExceptionToString(ex));
            exitStatus += 1;
        } 

        return exitStatus;
    }

    public abstract ITestExecutor GetTestExecutor(String opName);

    public abstract int ValidateRequest(SQLiteConnection db, String testName, int paramSetId, String operation, TestResult actualResult, ITestLogger logger);
    
    public abstract void Cleanup();
}