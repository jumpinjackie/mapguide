package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class ByteReaderStringConstructorTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        ByteReaderTestData.Init();
        String testString = ByteReaderTestData.testString;
        MgByteReader reader = new MgByteReader(testString, "text/html");
        Assert.AreEqual(testString.length(), reader.getLength());
        String buf = reader.toString();
        Assert.AreEqual(testString, buf);
        Assert.AreEqual(testString.length(), reader.getLength());
        reader.rewind();
        Assert.AreEqual(testString.length(), reader.getLength());
    }
}