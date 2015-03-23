package org.osgeo.mapguide.test.common;

import com.almworks.sqlite4java.*;

public interface ITestExecutorCollection
{
    /// <summary>
    /// Initializes this collection of test executors
    /// </summary>
    /// <param name="testExecutionMode"></param>
    /// <param name="dumpFile"></param>
    void Initialize(String testExecutionMode, String dumpFile);

    /// <summary>
    /// Gets the dump file path
    /// </summary>
    String getDumpFile();

    /// <summary>
    /// Gets the test execution mode
    /// </summary>
    String getTestExecutionMode();

    /// <summary>
    /// Gets the test executor of the specified operation name
    /// </summary>
    /// <param name="opName"></param>
    /// <returns></returns>
    ITestExecutor GetTestExecutor(String opName);

    /// <summary>
    /// Executes the test suite as defined in the dump file
    /// </summary>
    /// <param name="testsRun"></param>
    /// <param name="logger"></param>
    /// <param name="isEnterprise"></param>
    /// <returns></returns>
    int Execute(MutableInteger testsRun, ITestLogger logger, boolean isEnterprise);

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
    int ValidateRequest(SQLiteConnection db, String testName, int paramSetId, String operation, TestResult actualResult, ITestLogger logger);

    /// <summary>
    /// Performs necessary cleanup
    /// </summary>
    void Cleanup();
}