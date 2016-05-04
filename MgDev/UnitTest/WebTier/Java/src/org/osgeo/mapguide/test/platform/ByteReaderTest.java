package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class ByteReaderTest implements IExternalTest
{
    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        ByteReaderTestData.Init();
        int nBytes = ByteReaderTestData.nBytes;
        int nBlocks = ByteReaderTestData.nBlocks;
        byte[] testBytes = ByteReaderTestData.testBytes;
        String infileName = ByteReaderTestData.infileName;
        String outfileName = ByteReaderTestData.outfileName;
        byte[] buf = new byte[nBytes];
        MgByteReader reader = new MgByteReader(infileName, "png", false);
        Assert.AreEqual(nBlocks * nBytes, reader.getLength());
        reader.read(buf, nBytes);
        Assert.AreEqual(buf, testBytes);
        Assert.AreEqual((nBlocks - 1) * nBytes, reader.getLength());
        reader.rewind();
        Assert.AreEqual(nBlocks * nBytes, reader.getLength());
        reader.toFile(outfileName);
        reader.rewind();

        try {
            byte[] buf2 = new byte[nBytes];
            FileInputStream fp = new FileInputStream(outfileName);
            for (int j = 0; j < nBlocks; j++)
            {
                fp.read(buf2, 0, nBytes);
                reader.read(buf, nBytes);
                Assert.AreEqual(buf, buf2);
            }
            fp.close();
        } catch (Exception ex) {
            Assert.Fail(CommonUtility.ExceptionToString(ex));
        }
    }
}