package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class ByteReaderMemoryConstructorTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        ByteReaderTestData.Init();
        int nBytes = ByteReaderTestData.nBytes;
        byte[] testBytes = ByteReaderTestData.testBytes;
        String outfileName = ByteReaderTestData.outfileName;

        byte[] buf = new byte[nBytes];
        MgByteReader reader = new MgByteReader(testBytes, nBytes, "png");
        Assert.AreEqual(nBytes, reader.getLength());
        reader.read(buf, nBytes);
        Assert.AreEqual(buf, testBytes);
        Assert.AreEqual(0, reader.getLength());
        reader.rewind();
        Assert.AreEqual(nBytes, reader.getLength());

        reader.toFile(outfileName);

        try {
            FileInputStream fp = new FileInputStream(outfileName);
            fp.read(buf, 0, nBytes);
            Assert.AreEqual(buf, testBytes);
            fp.close();
        } catch (Exception ex) {
            Assert.Fail(CommonUtility.ExceptionToString(ex));
        }
    }
}