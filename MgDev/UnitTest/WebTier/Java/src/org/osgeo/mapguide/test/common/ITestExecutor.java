package org.osgeo.mapguide.test.common;

public interface ITestExecutor extends java.io.Closeable
{
    String getApi();

    String getOperationName();

    TestResult Execute(int paramSetId);
}