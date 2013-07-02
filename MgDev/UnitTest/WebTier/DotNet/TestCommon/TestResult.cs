using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common
{
    /// <summary>
    /// Encapsulates a unit test result
    /// </summary>
    public class TestResult
    {
        public string ResultData
        {
            get;
            private set;
        }

        public string ContentType
        {
            get;
            private set;
        }

        public string HttpStatusCode
        {
            get;
            private set;
        }

        public TestResult(string resultData = "", string contentType = "", string statusCode = "")
        {
            this.ResultData = resultData;
            this.ContentType = contentType;
            this.HttpStatusCode = statusCode;
        }

        public static TestResult FromByteReader(MgByteReader byteReader)
        {
            try
            {
                TestResult res = new TestResult();
                if (byteReader != null)
                {
                    res.ContentType = byteReader.GetMimeType();
                    res.ResultData = byteReader.ToString();
                }
                return res;
            }
            catch (MgException ex)
            {
                return FromMgException(ex);
            }
        }

        public static TestResult FromMgException(MgException ex)
        {
            return new TestResult(ex.GetType().Name, "text/plain");
        }

        public static TestResult FromException(Exception ex)
        {
            return new TestResult(ex.Message, "text/plain");
        }
    }
}
