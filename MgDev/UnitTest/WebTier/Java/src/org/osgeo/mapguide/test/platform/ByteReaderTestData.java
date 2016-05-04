package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.test.common.*;

public class ByteReaderTestData
{
    public static byte[] testBytes;
    public static int nBytes = 32768;
    public static int nBlocks = 256;
    public static String testString = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    public static String infileName;
    public static String outfileName;
    private static boolean bFirstTime = true;

    public static void Init()
    {
        if (bFirstTime)
        {
            infileName = CommonUtility.GetTempFileName();
            outfileName = CommonUtility.GetTempFileName();
            testBytes = new byte[nBytes];
            for (int i = 0; i < nBytes; i++)
            {
                testBytes[i] = (byte)(i % 255);
            }

            try {
                FileOutputStream fp = new FileOutputStream(infileName);
                for (int j = 0; j < nBlocks; j++)
                {
                    fp.write(testBytes, 0, nBytes);
                }
                fp.close();
            } catch (Exception ex) {
                Assert.Fail(CommonUtility.ExceptionToString(ex));
            }

            bFirstTime = false;
        }
    }
}