using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common.ExternalTests
{
    internal class ByteReaderTestData
    {
        internal static byte[] testBytes;
        internal static int nBytes = 32768;
        internal static int nBlocks = 256;
        internal static string testString = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        internal static string infileName;
        internal static string outfileName;
        private static bool bFirstTime = true;

        public static void Init()
        {
            if (bFirstTime)
            {
                infileName = System.IO.Path.GetTempFileName();
                outfileName = System.IO.Path.GetTempFileName();
                testBytes = new byte[nBytes];
                for (int i = 0; i < nBytes; i++)
                {
                    testBytes[i] = (byte)(i % 255);
                }

                System.IO.FileStream fp = System.IO.File.OpenWrite(infileName);
                for (int j = 0; j < nBlocks; j++)
                {
                    fp.Write(testBytes, 0, nBytes);
                }
                fp.Close();

                bFirstTime = false;
            }
        }
    }

    public class ByteReaderTest : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            ByteReaderTestData.Init();
            var nBytes = ByteReaderTestData.nBytes;
            var nBlocks = ByteReaderTestData.nBlocks;
            var testBytes = ByteReaderTestData.testBytes;
            var infileName = ByteReaderTestData.infileName;
            var outfileName = ByteReaderTestData.outfileName;
            byte[] buf = new byte[nBytes];
            MgByteReader reader = new MgByteReader(infileName, "png", false);
            Assert.AreEqual(nBlocks * nBytes, reader.GetLength());
            reader.Read(buf, nBytes);
            Assert.AreEqual(buf, testBytes);
            Assert.AreEqual((nBlocks - 1) * nBytes, reader.GetLength());
            reader.Rewind();
            Assert.AreEqual(nBlocks * nBytes, reader.GetLength());
            reader.ToFile(outfileName);
            reader.Rewind();

            byte[] buf2 = new byte[nBytes];
            System.IO.FileStream fp = System.IO.File.OpenRead(outfileName);
            for (int j = 0; j < nBlocks; j++)
            {
                fp.Read(buf2, 0, nBytes);
                reader.Read(buf, nBytes);
                Assert.AreEqual(buf, buf2);
            }
            fp.Close();
        }
    }

    public class MemoryConstructor : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            ByteReaderTestData.Init();
            var nBytes = ByteReaderTestData.nBytes;
            var testBytes = ByteReaderTestData.testBytes;
            var outfileName = ByteReaderTestData.outfileName;

            byte[] buf = new byte[nBytes];
            MgByteReader reader = new MgByteReader(testBytes, nBytes, "png");
            Assert.AreEqual(nBytes, reader.GetLength());
            reader.Read(buf, nBytes);
            Assert.AreEqual(buf, testBytes);
            Assert.AreEqual(0, reader.GetLength());
            reader.Rewind();
            Assert.AreEqual(nBytes, reader.GetLength());

            reader.ToFile(outfileName);

            System.IO.FileStream fp = System.IO.File.OpenRead(outfileName);
            fp.Read(buf, 0, nBytes);
            Assert.AreEqual(buf, testBytes);
            fp.Close();
        }
    }

    public class StringConstructor : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            ByteReaderTestData.Init();
            var testString = ByteReaderTestData.testString;
            MgByteReader reader = new MgByteReader(testString, "text/html");
            Assert.AreEqual(testString.Length, reader.GetLength());
            string buf = reader.ToString();
            Assert.AreEqual(testString, buf);
            Assert.AreEqual(testString.Length, reader.GetLength());
            reader.Rewind();
            Assert.AreEqual(testString.Length, reader.GetLength());
        }
    }
}
