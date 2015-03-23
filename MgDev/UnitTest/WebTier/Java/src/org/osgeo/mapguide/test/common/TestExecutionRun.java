package org.osgeo.mapguide.test.common;

import java.io.File;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;

public class TestExecutionRun implements java.io.Closeable
{
    private ITestExecutorCollection _executors;
    private SQLiteConnection _unitTestDb;

    public TestExecutionRun(String dbPath, ITestExecutorCollection exec) throws SQLiteException
    {
        _executors = exec;
        _unitTestDb = new SQLiteConnection(new File(dbPath));
        _unitTestDb.open(false);
    }

    public void close()
    {
        try
        {
            _unitTestDb.dispose();
        }
        catch (Exception ex) {

        }
        _unitTestDb = null;
    }

    private int[] ParamSetStrToArray(String pSet) {
        if (CommonUtility.IsNullOrEmpty(pSet))
            return new int[0];

        String[] parts = pSet.split(",");
        int[] result = new int[parts.length];
        int i = 0;
        for (String s : parts) {
            result[i] = Integer.parseInt(s.trim());
            i++;
        }
        return result;
    }

    public int RunTests(String testName, ITestLogger logger, MutableInteger testsRun) throws SQLiteException
    {
        TestResult actualResult = null;
        int exitStatus = 0;
        try
        {
            //Add the test in the log file
            String msg = "\n\n************************************************************\n" +
                         String.format("Executing %s\n", testName) +
                         "************************************************************\n\n";

            logger.Write(msg);

            //Get the list of operations to be executed for this test. This list should comma separated, no spaces.
            SQLiteStatement stmt = _unitTestDb.prepare(String.format("Select ParamSets from TestCase where TestName=\"%s\"", testName));
            while (stmt.step()) {
                String sets = CommonUtility.GetStringFromStatement(stmt, "ParamSets");

                //Extract the paramset ids
                int[] paramSet = ParamSetStrToArray(sets);
                for (int paramSetId : paramSet)
                {
                    SQLiteStatement pstmt = _unitTestDb.prepare(String.format("Select ParamValue from Params where ParamSet=%d AND ParamName=\"OPERATION\"", paramSetId));
                    while (pstmt.step()) {
                        String paramValue = CommonUtility.GetStringFromStatement(pstmt, "ParamValue");

                        //Add the operation to the log file
                        AddLogFileEntry(logger, paramValue, paramSetId);

                        try {
                            ITestExecutor exec = _executors.GetTestExecutor(paramValue);
                            if (DebugSettings.Enabled)
                                Console.WriteLine("\tExecuting operation: %s", exec.getOperationName());
                            actualResult = exec.Execute(paramSetId);
                            testsRun.increment();
                            exitStatus += _executors.ValidateRequest(_unitTestDb, testName, paramSetId, paramValue, actualResult, logger);
                        } catch (MissingTestExecutorException ex) {
                            if (DebugSettings.Enabled)
                                Console.WriteLine("No test executor for %s in test: %s. Skipping test.", paramValue, testName);
                        }
                    }
                }
            }
            return exitStatus;
        }
        catch (MgException ex)
        {
            throw new UnitTestException(String.format("Exception from MapGuide:\n%s", ex.getDetails()));
        }
    }

    private void AddLogFileEntry(ITestLogger logger, String operation, int paramSetId) throws SQLiteException
    {
        logger.Write("\nParamSet: %s\n", paramSetId);
        SQLiteStatement stmt = _unitTestDb.prepare(String.format("Select * from Params where ParamSet=%d", paramSetId));

        while (stmt.step())
        {
            String paramName = CommonUtility.GetStringFromStatement(stmt, "ParamName");
            String paramValue = CommonUtility.GetStringFromStatement(stmt, "ParamValue");

            logger.Write("%s: %s\n", paramName, paramValue);
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