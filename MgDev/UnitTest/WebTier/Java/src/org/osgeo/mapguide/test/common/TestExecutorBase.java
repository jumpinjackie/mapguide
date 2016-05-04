package org.osgeo.mapguide.test.common;

public abstract class TestExecutorBase implements ITestExecutor
{
    public abstract String getApi();

    public abstract String getOperationName();

    public abstract TestResult Execute(int paramSetId);

    public abstract void close();
}