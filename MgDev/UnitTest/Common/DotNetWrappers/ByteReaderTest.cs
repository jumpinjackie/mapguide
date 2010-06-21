using System;
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Text;
using NUnit.Framework;
using OSGeo.MapGuide;

namespace OSGeo.MapGuide
{
    [TestFixture]
    public class ByteReaderTest
    {
        private byte[] testBytes;
        private int nBytes = 32768;
        private int nBlocks = 256;
        private string testString = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        private string infileName;
        private string outfileName;
        private bool bFirstTime = true;

        [SetUp]
        public void InitMapGuide()
        {
            if (bFirstTime)
            {
                infileName = System.IO.Path.GetTempFileName();
                outfileName = System.IO.Path.GetTempFileName();
                MapGuideApi.MgInitializeWebTier("d:/mgdev/web/src/webconfig.ini");
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

        [Test]
        public void FileConstructor()
        {
            byte[] buf = new byte[nBytes];
            MgByteReader reader = new MgByteReader(infileName, "png", false);
            Assert.AreEqual(nBlocks * nBytes, reader.GetLength());
            reader.Read(buf, nBytes);
            Assert.AreEqual(buf, testBytes);
            Assert.AreEqual((nBlocks-1) * nBytes, reader.GetLength());
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

        [Test]
        public void MemoryConstructor()
        {
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

        [Test]
        public void StringConstructor()
        {
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
