using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common
{
    [Serializable]
    public class AssertException : Exception
    {
        public AssertException() { }
        public AssertException(string message) : base(message) { }
        public AssertException(string message, Exception inner) : base(message, inner) { }
        protected AssertException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    }

    public class Assert
    {
        public static void AreBytesEqual(byte[] expected, byte[] actual)
        {
            byte[] a = expected;
            byte[] b = actual;
            if (a.Length != b.Length)
                throw new AssertException("Expected byte array size of " + a.Length + ", got size of: " + b.Length);

            for (int i = 0; i < a.Length; i++)
            {
                if (a[i] != b[i])
                    throw new AssertException("Byte number " + (i + 1) + " in expected byte array does not match byte in actual byte array");
            }
        }

        public static void AreEqual<T>(T expected, T actual)
        {
            if (typeof(T) == typeof(byte[]))
            {
                AreBytesEqual(expected as byte[], actual as byte[]);
            }
            else if (!EqualityComparer<T>.Default.Equals(expected, actual))
                throw new AssertException("Expected: " + expected + ", got: " + actual);
        }

        public static void Greater<T>(T value, T against) where T : IComparable
        {
            if (value.CompareTo(against) <= 0)
                throw new AssertException(value + " is not greater than " + against);
        }

        public static void IsNull(object obj)
        {
            if (obj != null)
                throw new AssertException("Reference is not null");
        }

        public static void IsNotNull(object obj)
        {
            if (obj == null)
                throw new AssertException("Reference is null");
        }

        public static void IsTrue(bool condition)
        {
            if (!condition)
                throw new AssertException("Condition evaluated to false. Expected: true");
        }

        public static void IsFalse(bool condition)
        {
            if (condition)
                throw new AssertException("Condition evaluated to true. Expected: false");
        }

        public static void IsNaN(double value)
        {
            if (!double.IsNaN(value))
                throw new AssertException("Double is a number. Expected: NaN");
        }

        public static void Fail(string msg)
        {
            throw new AssertException(msg);
        }
    }
}
