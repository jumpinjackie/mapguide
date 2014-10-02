package org.osgeo.mapguide.test.common;

import java.io.*;

public interface ITestLogger extends Closeable
{
    void Write(String format, Object ... args);
    void WriteLine(String format, Object ... args);
}