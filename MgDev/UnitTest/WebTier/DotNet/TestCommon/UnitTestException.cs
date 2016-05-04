using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common
{
    [Serializable]
    public class UnitTestException : Exception
    {
        public UnitTestException() { }
        public UnitTestException(string message) : base(message) { }
        public UnitTestException(string message, Exception inner) : base(message, inner) { }
        protected UnitTestException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    }
}
